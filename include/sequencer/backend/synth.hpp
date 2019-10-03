#pragma once

#include <sequencer/audio/double_buffer.hpp>
#include <sequencer/audio/envelope.hpp>
#include <sequencer/audio/oscillator.hpp>
#include <sequencer/portaudio/portaudio.hpp>

#include <atomic>
#include <cmath>
#include <future>
#include <iostream>
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
            auto rm = vca_enabled_ ? ring_modulation( pos ) : 1.0;
            if ( vca_mode_ )
            {
                rm = make_vca( rm );
            }
            return envelope( pos ) * rm * oscillator( pos, frequency_modulation );
        }

        audio::oscillator_t oscillator;
        audio::oscillator_t ring_modulation;
        audio::oscillator_t frequency_modulation;
        audio::envelope_t envelope;
        std::atomic_bool vca_mode_{false};
        std::atomic_bool vca_enabled_{false};
    };

    class backend_t
    {
    public:
        using size_type = std::vector< chain_t >::size_type;

        static constexpr auto number_of_oscillators = 2u;

        backend_t() : chains_( number_of_oscillators )
        {
            generating_thread_terminated_ = std::async( std::launch::async, [this] {
                using namespace sequencer::audio;
                constexpr auto N = 2 * 1024;
                read_write_lockable< sample_t > buffer{N, audio::mode_t::stereo};
                const auto initial_size = buffer.frames.size();

                auto write_buffer = [this, &buffer, initial_size] {
                    buffer.frames.resize( initial_size );
                    for ( auto i = 0; i < N; ++i )
                    {
                        const auto pos = double( frame_pos_ ) / sample_rate_;
                        const auto osc_value =
                            0.5f * float( chain( 0 )( pos ) + chain( 1 )( pos ) );
                        buffer.frames[ size_type( ( 2 * i ) % initial_size ) ] = osc_value;
                        buffer.frames[ size_type( ( ( 2 * i ) % initial_size ) + 1 ) ] = osc_value;
                        ++frame_pos_;
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

            playing_thread_terminated_ = std::async( std::launch::async, [this] {
                while ( !stopped_ )
                {
                    std::lock_guard lock{stream_mutex_};
                    if ( !running_ || !stream_.is_active() )
                    {
                        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
                    }
                }
            } );
        }

        ~backend_t()
        {
            stopped_ = true;
            generating_thread_terminated_.wait();
            playing_thread_terminated_.wait();
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

    private:
        portaudio::portaudio portaudio_{};
        std::mutex stream_mutex_{};
        sequencer::portaudio::stream_t stream_{};
        std::vector< chain_t > chains_{};
        audio::double_buffer_t double_buffer_{};
        std::shared_ptr< audio::double_buffer_reader_t > double_buffer_reader_{
            std::make_shared< audio::double_buffer_reader_t >( double_buffer_ )};
        std::future< void > generating_thread_terminated_;
        std::future< void > playing_thread_terminated_;
        int sample_rate_{44100};
        int device_id_{-1};
        std::atomic_int frame_pos_{0};
        std::size_t frames_per_buffer_{512};
        std::atomic_bool running_{false};
        std::atomic_bool stopped_{false};
    };
} // namespace sequencer::backend::synth
