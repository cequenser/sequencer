#include "backend.hpp"

#include <sequencer/audio/fft.hpp>
#include <sequencer/midi/percussion_key.hpp>

#include <cassert>
#include <cstdint>
#include <iostream>

using sequencer::midi::no_note;
using sequencer::midi::note_t;
using sequencer::midi::percussion_key;

namespace qml
{
    backend::backend()
        : midiout_{RtMidiOut()}, tracks_{sequencer::midi::make_tracks( number_of_tracks,
                                                                       number_of_steps )},
          clock_{sequencer::rtmidi::make_clock()},
          clock_done_{start_clock_in_thread( clock_, [this]( auto message ) {
              midiout_.sendMessage(
                  static_cast< const unsigned char* >( static_cast< const void* >( &message ) ),
                  1 );
              send_messages( tracks_, message, sequencer::rtmidi::message_sender{midiout_} );
          } )}
    {
        track_notes_.fill( note_t( min_note() ) );
    }

    backend::~backend()
    {
        stop_recording();
        send_all_notes_off_message( tracks_, sequencer::rtmidi::message_sender{midiout_} );
        clock_.shut_down();
    }

    void backend::start_clock()
    {
        clock_.start();
    }

    void backend::stop_clock()
    {
        clock_.stop();
    }

    void backend::reset_clock()
    {
        clock_.reset();
    }

    void backend::set_clock_tempo( int bpm )
    {
        clock_.set_tempo( sequencer::beats_per_minute( bpm ) );
    }

    void backend::set_song_position_pointer( int song_position_in_16th_notes )
    {
        auto sender = sequencer::rtmidi::message_sender{midiout_};

        static constexpr auto max_song_position = 16383; // 2^14 - 1
        assert( song_position_in_16th_notes >= 0 );
        assert( song_position_in_16th_notes <= max_song_position );
        sender(
            sequencer::midi::system::common::song_position_pointer( song_position_in_16th_notes ) );
    }

    QString backend::available_midi_ports()
    {
        QString ports = "no port selected";
        for ( auto id = 0u; id < midiout_.getPortCount(); ++id )
        {
            ports += ( ";" + midiout_.getPortName( id ) ).c_str();
        }
        return ports;
    }

    QString backend::available_audio_devices()
    {
        QString audio_devices = "no device selected";
        for ( const auto& name : portaudio_.get_device_names() )
        {
            audio_devices += ";";
            audio_devices += name.c_str();
        }
        return audio_devices;
    }

    void backend::set_audio_device( int id )
    {
        audio_device_id_ = id - 1;
    }

    bool backend::open_port( unsigned id )
    {
        assert( id <= midiout_.getPortCount() + 1 );

        if ( midiout_.isPortOpen() )
        {
            midiout_.closePort();
        }
        if ( id == 0 )
        {
            return true;
        }
        midiout_.openPort( id - 1 );
        return midiout_.isPortOpen();
    }

    void backend::set_step( int step, bool checked )
    {
        const auto new_note = checked ? track_notes_[ current_track_ ] : no_note();
        tracks_[ current_track_ ][ tracks_t::size_type( step ) ] = new_note;
    }

    bool backend::is_step_set( int step ) const
    {
        return tracks_[ current_track_ ][ step ] != no_note();
    }

    void backend::set_current_track( int track )
    {
        assert( track < number_of_tracks );
        current_track_ = std::uint8_t( track );
    }

    int backend::min_note() const
    {
        return static_cast< std::uint8_t >( percussion_key::AcousticBassDrum );
    }

    int backend::max_note() const
    {
        return static_cast< std::uint8_t >( percussion_key::OpenTriangle );
    }

    QString backend::note_to_string( int note ) const
    {
        assert( note >= min_note() );
        assert( note <= max_note() );
        return QString( to_string( static_cast< percussion_key >( std::uint8_t( note ) ) ).data() );
    }

    QString backend::notes_to_string() const
    {
        QString str;
        for ( auto note = min_note(); note <= max_note(); ++note )
        {
            str.append( note_to_string( note ) );
            if ( note != max_note() )
            {
                str.append( ";" );
            }
        }
        return str;
    }

    void backend::set_note_of_current_track( int note )
    {
        track_notes_[ current_track_ ] = note_t( min_note() + note );
        for ( auto step = 0; step < number_of_steps; ++step )
        {
            if ( is_step_set( step ) )
            {
                set_step( step, true );
            }
        }
    }

    void backend::start_recording()
    {
        if ( sample_.writing_is_blocked() || no_device_selected() )
        {
            return;
        }
        else
        {
            sample_.block_reading( true );
        }

        stop_recording_ = false;
        recording_done_ = std::async( std::launch::async, [this] {
            sample_.clear();
            auto writer = sequencer::audio::sample_writer_t{sample_};

            const auto parameters = portaudio_.get_parameters( audio_device_id_ );

            sequencer::portaudio::stream_t stream;
            stream.open_input_stream( parameters, sample_rate_, frames_per_buffer_,
                                      sequencer::portaudio::record_callback, &writer );
            stream.start();

            while ( stream.is_active() && !stop_recording_ )
            {
                std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
            }

            sample_.trim();
            std::cout << "recorded frames: " << sample_.frames.size() << std::endl;
            sample_.block_reading( false );
        } );
    }

    bool backend::is_recording() const noexcept
    {
        return sample_.reading_is_blocked();
    }

    void backend::stop_recording()
    {
        stop_recording_ = true;
    }

    namespace
    {
        auto high_pass( double cutoff, double freq )
        {
            return freq > cutoff ? 1.f : 0.f;
        }

        auto low_pass( double cutoff, double freq )
        {
            return freq < cutoff ? 1.f : 0.f;
        }

        auto gauss( double a, double b, double c, double x )
        {
            return a * std::exp( -( x - b ) * ( x - b ) / ( 2 * c * c ) );
        }

        auto freq_factor( double f, double g )
        {
            return 1.0 + f * f / ( g * g );
        }

        auto high_shelf( double upper_freq, double lower_freq, double freq )
        {
            return std::sqrt( freq_factor( freq, upper_freq ) / freq_factor( freq, lower_freq ) );
        }

        //        auto high_shelf(double alpha, double freq)
        //        {
        //            const auto beta = std::sqrt(alpha);
        //            const auto upper_freq = 10000.0*beta;
        //            const auto lower_freq = 10000.0/beta;
        //            return high_shelf(upper_freq, lower_freq, freq);
        //        }

        auto low_shelf( double upper_freq, double lower_freq, double freq )
        {
            return upper_freq / lower_freq * high_shelf( upper_freq, lower_freq, freq );
        }

        void filter( sequencer::audio::sample_t& sample, double freq, double gain, double q,
                     double base_frequency )
        {
            const auto beta = std::sqrt( 1.0 + gain * 200 );
            const auto upper_freq = 1000.0 * beta;
            const auto lower_freq = 1000.0 / beta;
            std::cout << "beta: " << beta << ", " << upper_freq << ", " << lower_freq << std::endl;
            std::cout << "filtering " << freq << ", " << gain << ", " << q
                      << ", base freq: " << base_frequency << std::endl;
            std::cout << "d: "
                      << float( high_shelf( upper_freq, lower_freq, 200 * base_frequency ) ) << ", "
                      << float( high_shelf( upper_freq, lower_freq,
                                            sample.frames.size() * base_frequency / 4 ) )
                      << std::endl;
            using namespace sequencer::audio;
            std::vector< float > lhs_sample( sample.frames.size() / 2 );
            std::vector< float > rhs_sample( sample.frames.size() / 2 );
            for ( auto i = 0u; i < lhs_sample.size() / 2; ++i )
            {
                lhs_sample[ i ] = sample.frames[ 2 * i ];
                rhs_sample[ i ] = sample.frames[ 2 * i + 1 ];
            }
            auto spectrum_lhs = fft( lhs_sample );
            auto spectrum_rhs = fft( rhs_sample );
            for ( auto i = 0u; i < spectrum_lhs.size(); ++i )
            {
                //                const auto scale = float(1.0 - gauss(gain, freq, q,
                //                i*base_frequency));
                //                const auto scale =
                //                    float( high_shelf( upper_freq, lower_freq, i * base_frequency
                //                    ) );
                const auto scale = low_pass( freq, i * base_frequency );

                spectrum_lhs[ i ] *= scale;
                spectrum_rhs[ i ] *= scale;
            }

            lhs_sample = inverse_fft( spectrum_lhs );
            rhs_sample = inverse_fft( spectrum_rhs );

            for ( auto i = 0u; i < lhs_sample.size(); ++i )
            {
                sample.frames[ 2 * i ] = lhs_sample[ i ];
                sample.frames[ 2 * i + 1 ] = rhs_sample[ i ];
            }
        }
    } // namespace

    void backend::playback()
    {
        if ( sample_.reading_is_blocked() )
        {
            return;
        }
        else
        {
            sample_.block_writing( true );
        }

        //        update_sample();

        std::cout << "1" << std::endl;
        using namespace sequencer::audio;
        constexpr auto N = 32 * 1024;
        read_write_lockable< sample_t > buffer{N, sample_.mode};
        const auto initial_size = buffer.frames.size();
        sample_reader_t sample_reader{sample_};

        if ( recording_done_.valid() )
        {
            recording_done_.wait();
        }
        //        filter( sample_, double( eq_freq_1_ ), double( eq_gain_1_ ) / 100,
        //                double( eq_q_factor_1_ ), double( 2 * sample_rate_ ) /
        //                sample_.frames.size() );

        auto read_buffer = [this, &buffer, &sample_reader, initial_size] {
            buffer.frames.resize( initial_size );
            buffer.clear();
            sample_reader.read( buffer.frames.data(), buffer.number_of_frames() );
            //            filter( buffer, double( eq_freq_1_ ), double( eq_gain_1_ ) / 100,
            //                    double( eq_q_factor_1_ ), double( 2 * sample_rate_ ) /
            //                    buffer.frames.size() );
            double_buffer_.swap_write_buffer( buffer );
        };

        std::cout << "read data" << std::endl;
        read_buffer();
        double_buffer_.swap_buffers();
        std::cout << "read data" << std::endl;
        read_buffer();
        stop_recording_ = false;
        auto reader =
            std::make_shared< sequencer::audio::double_buffer_reader_t >( double_buffer_ );
        recording_done_ = std::async( std::launch::async, [this, reader] {
            const auto parameters = portaudio_.get_parameters( audio_device_id_ );

            sequencer::portaudio::stream_t stream;
            stream.open_output_stream(
                parameters, sample_rate_, frames_per_buffer_,
                sequencer::portaudio::play_callback< std::decay_t< decltype( *reader ) > >,
                reader.get() );
            stream.start();
            while ( stream.is_active() && !stop_recording_ )
                std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
            stream.close();
            sample_.block_writing( false );
        } );

        while ( true )
        {
            if ( !double_buffer_.has_new_data() )
            {
                double_buffer_.swap_buffers();
                if ( !sample_reader.has_frames_left() )
                {
                    std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
                    reader->set_continue_reading( false );
                    break;
                }

                std::cout << "read data" << std::endl;
                read_buffer();
                //                using namespace sequencer::audio;
                //                auto spectrum = fft(buffer.frames);
                //                auto signal = inverse_fft(spectrum);
                //                for(auto i = 0; i<signal.size(); ++i)
                //                {
                //                    if(std::abs(signal[i] - buffer.frames[i]) >
                //                    std::numeric_limits<float>::epsilon()*100)
                //                    {
                //                        std::cout << i << ": " << signal[i] << ", " <<
                //                        buffer.frames[i] << std::endl;
                //                    }
                //                }
            }
        }
    }

    void backend::set_eq_freq_1( int value )
    {
        eq_freq_1_ = value;
    }

    void backend::set_eq_gain_1( int value )
    {
        eq_gain_1_ = value;
    }

    void backend::set_eq_q_factor_1( int value )
    {
        eq_q_factor_1_ = value;
    }

    void backend::update_sample()
    {
        //        using namespace sequencer::audio;
        //        sample_.block_writing(true);
        //        auto frames = sample_.frames;
        //        sample_.block_writing(false);
        //        auto result = fft(sample_.frames);
        //        const auto base_frequency_ = double(sample_rate_)/sample_.frames.size();
        ////        for(auto i=0u; i<result.size(); ++i)
        ////        {
        ////            result[i] *= (1.0 - gauss(double(eq_gain_1_)/100, double(eq_freq_1_),
        /// double(eq_q_factor_1_), i*base_frequency_)); /        }

        //        sample_.block_reading(true);
        //        sample_.block_writing(true);
        //        sample_.frames = inverse_fft(result);
        //        sample_.block_reading(false);
        //        sample_.block_writing(false);
    }
} // namespace qml
