#pragma once

#include <sequencer/audio/delay.hpp>
#include <sequencer/audio/double_buffer.hpp>
#include <sequencer/audio/dry_wet.hpp>
#include <sequencer/audio/envelope.hpp>
#include <sequencer/audio/fft.hpp>
#include <sequencer/audio/oscillator.hpp>
#include <sequencer/audio/transfer_function.hpp>
#include <sequencer/copyable_atomic.hpp>
#include <sequencer/midi/message/byte.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/portaudio/portaudio.hpp>

#include <cmath>
#include <future>
#include <mutex>
#include <string>
#include <vector>

namespace sequencer::backend::synth
{
    constexpr double make_vca( double signal )
    {
        return std::max( signal, 0.0 );
    }

    struct chain_t
    {
        double operator()( double pos )
        {
            auto rm = vca_enabled ? ring_modulation( pos ) : 1.0;
            if ( vca_mode )
            {
                rm = make_vca( rm );
            }
            return envelope( pos ) * rm * oscillator( pos, frequency_modulation );
        }

        audio::oscillator_t oscillator;
        audio::dry_wet_t< audio::oscillator_t > ring_modulation;
        audio::dry_wet_t< audio::oscillator_t > frequency_modulation;
        audio::envelope_t envelope;
        audio::dry_wet_t< audio::delay_t, true > delay{};
        copyable_atomic< bool > vca_mode{false};
        copyable_atomic< bool > vca_enabled{false};
        copyable_atomic< double > lowpass_gain{1.0};
        copyable_atomic< double > lowpass_frequency{22000};
        copyable_atomic< double > highpass_gain{1.0};
        copyable_atomic< double > highpass_frequency{20};
    };

    template < class LowPass, class HighPass >
    void filter( std::vector< float >& sample, LowPass low_pass, HighPass high_pass,
                 double base_frequency )
    {
        auto spectrum = audio::fft( sample );
        audio::filter( spectrum, low_pass, high_pass, base_frequency );
        sample = audio::inverse_fft( spectrum );
    }

    inline std::vector< float > create_blackman_table( int N )
    {
        std::vector< float > table( N + 1, 0 );
        for ( auto n = 0; n <= N; ++n )
        {
            table[ n ] = float( audio::window::blackman( n, N ) );
        }
        return table;
    }

    template < int N, int M >
    inline auto create_kernels( float lowpass_cutoff, float highpass_cutoff )
    {
        std::vector< float > highpass_kernel( 2 * N, 0.f );
        std::vector< float > lowpass_kernel( 2 * N, 0.f );
        static auto blackman_table = create_blackman_table( M );
        for ( auto i = 0; i <= M; ++i )
        {
            if ( i == M / 2 )
            {
                highpass_kernel[ i ] =
                    blackman_table[ M / 2 ] * ( 1.f - wave_form::sinc( highpass_cutoff, 0 ) );
            }
            else
            {
                highpass_kernel[ i ] =
                    -blackman_table[ i ] * wave_form::sinc( highpass_cutoff, ( i - M / 2 ) );
            }
            lowpass_kernel[ i ] =
                blackman_table[ i ] * wave_form::sinc( lowpass_cutoff, ( i - M / 2 ) );
        }
        return std::make_pair( lowpass_kernel, highpass_kernel );
    }

    template < int N, int M >
    inline auto create_filter_response( float lowpass_cutoff, float highpass_cutoff )
    {
        auto [ lowpass_kernel, highpass_kernel ] =
            create_kernels< N, M >( lowpass_cutoff, highpass_cutoff );
        return std::make_pair( audio::fft( lowpass_kernel ), audio::fft( highpass_kernel ) );
    }

    template < class ClockReceiver >
    class backend_t : public ClockReceiver
    {
    public:
        using size_type = std::vector< chain_t >::size_type;

        static constexpr auto number_of_oscillators = 2u;

        template < class Callback >
        explicit backend_t( Callback callback )
            : ClockReceiver{callback}, chains_( number_of_oscillators )
        {
            generating_thread_terminated_ = std::async( std::launch::async, [this] {
                using namespace sequencer::audio;
                constexpr auto N = 4 * 1024;
                read_write_lockable< sample_t > buffer{N, audio::mode_t::stereo};
                const auto initial_size = buffer.frames.size();

                constexpr auto M = N / 16;
                static auto blackman_table = create_blackman_table( M );

                std::vector< float > last_osc0( N, 0.f );
                std::vector< float > last_osc1( N, 0.f );

                auto write_buffer = [&] {
                    auto osc_0 = std::async(
                        std::launch::async, [frame_pos = frame_pos_.load(), this]() mutable {
                            auto [ lowpass_filter_response, highpass_filter_response ] =
                                create_filter_response< N, M >(
                                    chain( 0 ).lowpass_frequency / sample_rate_,
                                    chain( 0 ).highpass_frequency / sample_rate_ );

                            std::vector< float > osc( 2 * N, 0.f );
                            for ( auto i = 0; i < N; ++i )
                            {
                                const auto pos = double( frame_pos ) / sample_rate_;
                                osc[ size_type( i ) ] = float( silent_ ? 0.0f : chain( 0 )( pos ) );
                                ++frame_pos;
                            }
                            auto spectrum = audio::fft( osc );
                            for ( auto i = 0u; i < spectrum.size(); ++i )
                            {
                                spectrum[ i ] *=
                                    lowpass_filter_response[ i ] * highpass_filter_response[ i ];
                            }
                            return audio::inverse_fft( spectrum );
                        } );
                    auto osc_1 = std::async(
                        std::launch::async, [frame_pos = frame_pos_.load(), this]() mutable {
                            auto [ lowpass_filter_response, highpass_filter_response ] =
                                create_filter_response< N, M >(
                                    chain( 1 ).lowpass_frequency / sample_rate_,
                                    chain( 1 ).highpass_frequency / sample_rate_ );

                            std::vector< float > osc( 2 * N, 0.f );
                            for ( auto i = 0; i < N; ++i )
                            {
                                const auto pos = double( frame_pos ) / sample_rate_;
                                osc[ size_type( i ) ] = float( silent_ ? 0.0f : chain( 1 )( pos ) );
                                ++frame_pos;
                            }
                            auto spectrum = audio::fft( osc );
                            for ( auto i = 0u; i < spectrum.size(); ++i )
                            {
                                spectrum[ i ] *=
                                    lowpass_filter_response[ i ] * highpass_filter_response[ i ];
                            }
                            return audio::inverse_fft( spectrum );
                        } );
                    frame_pos_ += N;
                    buffer.frames.resize( initial_size );
                    const auto osc0 = osc_0.get();
                    const auto osc1 = osc_1.get();
                    for ( auto i = 0; i < N; ++i )
                    {
                        const auto value0 = osc0[ i ] + last_osc0[ i ];
                        const auto [ lhs0, rhs0 ] = chain( 0 ).delay( value0 );
                        const auto value1 = osc1[ i ] + last_osc1[ i ];
                        const auto [ lhs1, rhs1 ] = chain( 1 ).delay( value1 );
                        //                        const auto lhs0 = osc0[i] + last_osc0[i];
                        //                        const auto rhs0 = lhs0;
                        //                        const auto lhs1 = osc1[1] + last_osc1[i];
                        //                        const auto rhs1 = lhs1;
                        buffer.frames[ size_type( 2 * i ) ] = 0.5 * ( lhs0 + lhs1 );
                        buffer.frames[ size_type( 2 * i + 1 ) ] = 0.5 * ( rhs0 + rhs1 );
                        last_osc0[ i ] = osc0[ N + i ];
                        last_osc1[ i ] = osc1[ N + i ];
                    }
                    double_buffer_.swap_write_buffer( buffer );
                };

                write_buffer();
                double_buffer_.swap_buffers();
                write_buffer();

                auto at_end = false;
                while ( !stopped_ )
                {
                    if ( running_ && !double_buffer_.has_new_data() )
                    {
                        if ( at_end )
                        {
                            double_buffer_reader_->set_continue_reading( false );
                            break;
                        }
                        double_buffer_.swap_buffers();

                        write_buffer();
                    }
                }
            } );
        }

        ~backend_t()
        {
            stopped_ = true;
            generating_thread_terminated_.wait();
        }

        chain_t& chain( size_type idx ) noexcept
        {
            return chains_[ idx ];
        }

        const chain_t& chain( size_type idx ) const noexcept
        {
            return chains_[ idx ];
        }

        void start() noexcept
        {
            if ( device_id_ < 0 )
            {
                return;
            }
            std::lock_guard lock{stream_mutex_};
            if ( stream_.is_active() )
            {
                running_ = true;
                return;
            }

            const auto parameters = portaudio_.get_parameters( device_id_ );
            stream_.open_output_stream( parameters, sample_rate_, frames_per_buffer_,
                                        sequencer::portaudio::play_callback<
                                            std::decay_t< decltype( *double_buffer_reader_ ) > >,
                                        double_buffer_reader_.get() );
            stream_.start();

            running_ = true;
        }

        void stop() noexcept
        {
            running_ = false;

            std::lock_guard lock{stream_mutex_};
            if ( stream_.is_active() )
            {
                stream_.close();
            }
        }

        void trigger() noexcept
        {
            frame_pos_ = 0;
        }

        void set_device( int device_id )
        {
            device_id_ = device_id;
            stop();
        }

        std::vector< std::string > available_audio_devices() const
        {
            return portaudio_.get_device_names();
        }

        template < int N >
        void receive_message( const midi::message_t< N >& message )
        {
            if constexpr ( N == 3 )
            {
                if ( message[ 0 ] == midi::byte::note_on )
                {
                    trigger();
                    silent_ = false;
                    return;
                }
                //                if( message[0] == midi::byte::note_off )
                //                {
                //                    silent_ = true;
                //                    return;
                //                }
            }
        }

    private:
        portaudio::portaudio portaudio_{};
        std::mutex stream_mutex_{};
        sequencer::portaudio::stream_t stream_{};
        std::vector< chain_t > chains_{};
        audio::double_buffer_t double_buffer_{};
        std::shared_ptr< audio::double_buffer_reader_t > double_buffer_reader_{
            std::make_shared< audio::double_buffer_reader_t >( double_buffer_ )};
        std::future< void > generating_thread_terminated_;
        int sample_rate_{44100};
        int device_id_{-1};
        std::atomic_int frame_pos_{0};
        std::size_t frames_per_buffer_{512};
        std::atomic_bool running_{false};
        std::atomic_bool stopped_{false};
        std::atomic_bool silent_{false};
    };
} // namespace sequencer::backend::synth
