#pragma once

#include <sequencer/audio/delay.hpp>
#include <sequencer/audio/double_buffer.hpp>
#include <sequencer/audio/dry_wet.hpp>
#include <sequencer/audio/envelope.hpp>
#include <sequencer/audio/fft.hpp>
#include <sequencer/audio/oscillator.hpp>
#include <sequencer/audio/ring_buffer.hpp>
#include <sequencer/audio/transfer_function.hpp>
#include <sequencer/audio/wave_reader.hpp>
#include <sequencer/beat_duration.hpp>
#include <sequencer/copyable_atomic.hpp>
#include <sequencer/midi/message/byte.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/portaudio/portaudio.hpp>

#include <chrono>
#include <cmath>
#include <experimental/filesystem>
#include <future>
#include <mutex>
#include <string>
#include <vector>

namespace sequencer::backend::synth
{
    inline std::vector< float > create_blackman_table( int N )
    {
        std::vector< float > table( N + 1, 0 );
        for ( auto n = 0; n <= N; ++n )
        {
            table[ n ] = float( audio::window::blackman( n, N ) );
        }
        return table;
    }

    template < int M >
    inline auto create_kernels( int N, float lowpass_cutoff, float highpass_cutoff )
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

    template < int M >
    inline auto create_filter_response( int N, float lowpass_cutoff, float highpass_cutoff )
    {
        std::vector< float > lowpass_kernel;
        std::vector< float > highpass_kernel;
        std::tie( lowpass_kernel, highpass_kernel ) =
            create_kernels< M >( N, lowpass_cutoff, highpass_cutoff );
        auto low = std::async( std::launch::async, [&] { return audio::fft( lowpass_kernel ); } );
        auto high = std::async( std::launch::async, [&] { return audio::fft( highpass_kernel ); } );
        return std::make_pair( low.get(), high.get() );
    }

    constexpr double make_vca( double signal )
    {
        return std::max( signal, 0.0 );
    }

    struct chain_t
    {
        static constexpr auto sub_sample_size = 4 * 1024;
        static constexpr auto M = 256;

        using spectrum_t = std::vector< std::complex< float > >;

        double operator()( double pos )
        {
            auto rm = vca_enabled ? ring_modulation( pos ) : 1.0;
            if ( vca_mode )
            {
                rm = make_vca( rm );
            }
            return envelope( pos ) * rm * oscillator( pos, frequency_modulation );
        }

        auto get_sample( int frame_pos, int sample_rate )
        {
            sample_.clean_and_move( sub_sample_size );
            std::vector< float > osc( 2 * sub_sample_size, 0.f );
            for ( auto i = 0; i < sub_sample_size; ++i )
            {
                const auto pos = double( frame_pos ) / sample_rate;
                auto scale = 0.0;
                if ( !silent_ )
                {
                    scale = ( osc_fade_out_ > 0 )
                                ? ( osc_fade_out_-- ) / double( fade_sample_count_ )
                                : 1.0;
                }
                osc[ std::size_t( i ) ] = scale * ( *this )( pos );
                ++frame_pos;
            }
            auto spectrum = audio::fft( osc );
            filter( spectrum );
            osc = audio::inverse_fft( spectrum );
            for ( auto k = 0u; k < osc.size(); ++k )
            {
                sample_[ k ] += osc[ k ];
            }

            using std::begin;
            std::vector< std::pair< float, float > > result( sub_sample_size );
            for ( auto k = 0u; k < result.size(); ++k )
            {
                result[ k ] = delay( sample_[ k ] );
            }
            return result;
        }

        void set_highpass_frequency( double frequency )
        {
            highpass_frequency = frequency;
            update_filter_spectrum( sub_sample_size );
        }

        void set_lowpass_frequency( double frequency )
        {
            lowpass_frequency = frequency;
            update_filter_spectrum( sub_sample_size );
        }

        void update_filter_spectrum( int N )
        {
            auto [ lowpass_filter_response, highpass_filter_response ] =
                create_filter_response< M >( N, lowpass_frequency / sample_rate,
                                             highpass_frequency / sample_rate );
            std::lock_guard lock{filter_mutex};
            filter_spectrum.resize( lowpass_filter_response.size() );
            for ( auto i = 0u; i < filter_spectrum.size(); ++i )
            {
                filter_spectrum[ i ] = lowpass_filter_response[ i ] * highpass_filter_response[ i ];
            }
        }

        void filter( spectrum_t& spectrum ) const
        {
            std::lock_guard lock{filter_mutex};
            //            assert(spectrum.size() == filter_spectrum.size());
            if ( filter_spectrum.size() != spectrum.size() )
            {
                return;
            }
            for ( auto i = 0u; i < spectrum.size(); ++i )
            {
                spectrum[ i ] *= filter_spectrum[ i ];
            }
        }

        audio::oscillator_t oscillator;
        audio::dry_wet_t< audio::oscillator_t > ring_modulation;
        audio::dry_wet_t< audio::oscillator_t > frequency_modulation;
        audio::envelope_t envelope;
        audio::dry_wet_t< audio::stereo_repeated_delay_t, true > delay{};
        copyable_atomic< bool > vca_mode{false};
        copyable_atomic< bool > vca_enabled{false};
        copyable_atomic< double > lowpass_gain{1.0};
        copyable_atomic< double > highpass_gain{1.0};
        copyable_atomic< int > sample_rate{44100};

        std::atomic_bool silent_{false};
        std::atomic_int osc_fade_out_{0};
        int fade_sample_count_{110};

    private:
        mutable std::mutex filter_mutex;
        copyable_atomic< double > lowpass_frequency{22000};
        copyable_atomic< double > highpass_frequency{20};
        spectrum_t filter_spectrum = spectrum_t( sub_sample_size );
        audio::ring_buffer_t< float > sample_{2 * sub_sample_size};
    };

    inline auto convolution( const std::vector< float >& signal,
                             const std::vector< float >& reverb )
    {
        std::vector< float > output( signal.size() + reverb.size(), 0 );

        //        const auto start_time = std::chrono::steady_clock::now();
        std::vector< std::future< void > > done;
        const auto n_threads = 8;
        const auto parallel_offset = output.size() / n_threads;
        for ( auto l = 0; l < n_threads; ++l )
        {
            done.push_back( std::async( std::launch::async, [&, l] {
                for ( auto j = l * parallel_offset; j < ( l + 1 ) * parallel_offset; ++j )
                {
                    for ( auto k = std::max( 0, int( j + 1 - reverb.size() ) );
                          k < std::min( j + 1, signal.size() ); ++k )
                    {
                        output[ j ] += reverb[ j - k ] * signal[ k ];
                    }
                }
            } ) );
        }

        for ( auto& d : done )
        {
            d.wait();
        } /*
         auto elapsed =
         std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
         start_time).count(); std::cout << "elapsed: " << elapsed << "ms" << std::endl;*/
        return output;
    }

    template < class spectrum_t >
    auto overlay_add_sub(
        const spectrum_t& spectrum_left, const spectrum_t& spectrum_right,
        const std::vector< std::pair< spectrum_t, spectrum_t > >& filters,
        std::vector< std::future< std::pair< std::vector< float >, std::vector< float > > > >& done,
        bool first_long_part )
    {
        //        std::vector< std::future< std::tuple< int, std::vector< float >, std::vector<
        //        float > > > >
        //            done;
        //        done[0] = std::async(
        //                      std::launch::async, [spectrum_left, spectrum_right, filter =
        //                      filters[1]] {
        //                          auto s = spectrum_left;
        //                          auto t = spectrum_right;
        //                          for ( auto k = 0u; k < s.size(); ++k )
        //                          {
        //                              s[ k ] *= 0.5f * filter.first[ k ];
        //                              t[ k ] *= 0.5f * filter.second[ k ];
        //                          }

        //                          return std::pair{audio::inverse_fft( s ), audio::inverse_fft( t
        //                          )};
        //                      } );
        //        done[first_long_part ? 1 : 2] = std::async(std::launch::async,
        //                                                   [spectrum_left, spectrum_right, filter1
        //                                                   = filters[2], filter2 = filters[3]]
        //        {
        //            auto s = spectrum_left;
        //            auto t = spectrum_right;
        //            for ( auto k = 0u; k < s.size(); ++k )
        //            {
        //                s[ k ] *= 0.5f * filter1.first[ k ];
        //                t[ k ] *= 0.5f * filter1.second[ k ];
        //            }

        //            auto left = audio::inverse_fft(s);
        //            auto right = audio::inverse_fft(t);
        //            const auto offset = left.size()/2;
        //            left.resize(left.size() + offset, 0);
        //            right.resize(right.size() + offset, 0);

        //            s = spectrum_left;
        //            t = spectrum_right;
        //            for ( auto k = 0u; k < s.size(); ++k )
        //            {
        //                s[ k ] *= 0.5f * filter2.first[ k ];
        //                t[ k ] *= 0.5f * filter2.second[ k ];
        //            }

        //            auto left2 = audio::inverse_fft(s);
        //            auto right2 = audio::inverse_fft(t);
        //            for(auto i=0u; i<left2.size(); ++i)
        //            {
        //                left[offset + i] += left2[i];
        //                right[offset + i] += right2[i];
        //            }

        //            return std::pair{std::move(left), std::move(right)};
        //        });

        //        for ( auto j = 1; j < filters.size(); ++j )
        //        {
        ////            const auto doff = int( j * spectrum_left.size() );
        //            auto g0 = std::async(
        //                std::launch::async, [&spectrum_left, &spectrum_right, j, doff, &filters]()
        //                mutable {
        auto lf = std::async( std::launch::async, [&] {
            auto s = spectrum_left;
            for ( auto k = 0u; k < s.size(); ++k )
            {
                s[ k ] *= 0.5f * filters[ 0 ].first[ k ];
            }
            return audio::inverse_fft( s );
        } );
        auto s = spectrum_right;
        for ( auto k = 0u; k < s.size(); ++k )
        {
            s[ k ] *= 0.5f * filters[ 0 ].first[ k ];
        }
        auto rf = audio::inverse_fft( s );
        return std::tuple{lf.get(), rf};
        //                    auto s = spectrum_left;
        //                    auto t = spectrum_right;
        //                    for ( auto k = 0u; k < s.size(); ++k )
        //                    {
        //                        s[ k ] *= 0.5f * filters[ 0 ].first[ k ];
        //                        t[ k ] *= 0.5f * filters[ 0 ].second[ k ];
        //                    }

        //                    return std::tuple{audio::inverse_fft( s ), audio::inverse_fft( t )};
        //                } );
        //            done.push_back( std::move( g0 ) );
        //        }

        //        std::vector< float > tmp_left( spectrum_left.size() + filters.size() *
        //        spectrum_left.size(),
        //                                       0 );
        //        std::vector< float > tmp_right(
        //            spectrum_right.size() + filters.size() * spectrum_left.size(), 0 );

        //        for ( auto& d : done )
        //        {
        //            const auto& [ offset2, lhs, rhs ] = d.get();
        //            for ( auto k = 0u; k < lhs.size(); ++k )
        //            {
        //                const auto idx = offset2 + k;
        //                if ( idx >= tmp_left.size() )
        //                    break;
        //                tmp_left[ idx ] += lhs[ k ];
        //                tmp_right[ idx ] += rhs[ k ];
        //            }
        //        }

        //        return std::pair{std::move( tmp_left ), std::move( tmp_right )};
    }

    template < class ClockReceiver >
    class backend_t : public ClockReceiver
    {
    public:
        using size_type = std::vector< chain_t >::size_type;

        static constexpr auto number_of_oscillators = 2u;
        static constexpr auto sub_sample_size = chain_t::sub_sample_size;
        static constexpr auto M = chain_t::M;

        template < class Callback >
        explicit backend_t( Callback callback )
            : ClockReceiver{callback}, chains_( number_of_oscillators ), clock_dt_( 24, 0 )
        {
            for ( auto& chain : chains_ )
            {
                chain.update_filter_spectrum( sub_sample_size );
            }

            generating_thread_terminated_ = std::async( std::launch::async, [this] {
                using namespace sequencer::audio;
                constexpr auto N = sub_sample_size;
                read_write_lockable< sample_t > buffer{N, audio::mode_t::stereo};
                const auto initial_size = buffer.frames.size();

                static auto blackman_table = create_blackman_table( M );

                std::vector< float > last_osc1( N, 0.f );

                auto write_buffer = [&] {
                    using namespace std::chrono;
                    const auto start_time = steady_clock::now();
                    auto signal0_f = std::async( std::launch::async, [&] {
                        return chain( 0 ).get_sample( frame_pos_, sample_rate_ );
                    } );

                    auto signal1 = chain( 1 ).get_sample( frame_pos_, sample_rate_ );
                    auto signal0 = signal0_f.get();
                    std::vector< float > left( 2 * sub_sample_size, 0 );
                    std::vector< float > right( 2 * sub_sample_size, 0 );
                    for ( auto k = 0u; k < signal0.size(); ++k )
                    {
                        left[ k ] = signal0[ k ].first + signal1[ k ].first;
                        right[ k ] = signal0[ k ].second + signal1[ k ].second;
                    }

                    bool use_reverb = false;
                    spectrum_t spectrum_left;
                    spectrum_t spectrum_right;
                    {
                        std::lock_guard lock{reverb_mutex_};
                        if ( !reverb_spectra_.empty() )
                        {
                            use_reverb = true;

                            auto spectrum0_f = std::async( std::launch::async,
                                                           [&] { return audio::fft( left ); } );
                            spectrum_right = audio::fft( right );
                            spectrum_left = spectrum0_f.get();
                            const auto [ tmp_l, tmp_r ] =
                                overlay_add_sub( spectrum_left, spectrum_right, reverb_spectra_,
                                                 sub_parts_, first_double_part_ );
                            //                                first_long_part_ = !first_long_part_;
                            {
                                std::lock_guard lock{left_mutex_};
                                for ( auto k = 0u; k < tmp_l.size(); ++k )
                                {
                                    last_osc0_lhs_before_filter[ k ] += tmp_l[ k ];
                                }
                            }
                            {
                                std::lock_guard lock{right_mutex_};
                                for ( auto k = 0u; k < tmp_r.size(); ++k )
                                {
                                    last_osc0_rhs_before_filter[ k ] += tmp_r[ k ];
                                }
                            }
                            if ( sub_parts_[ 0 ].valid() )
                            {
                                const auto [ lhs1, rhs1 ] = sub_parts_[ 0 ].get();
                                for ( auto k = 0u; k < lhs1.size(); ++k )
                                {
                                    last_osc0_lhs_before_filter[ k ] += lhs1[ k ];
                                    last_osc0_rhs_before_filter[ k ] += rhs1[ k ];
                                }
                            }

                            if ( sub_parts_[ 1 ].valid() && sub_parts_[ 2 ].valid() )
                            {
                                const auto [ lhs2, rhs2 ] =
                                    sub_parts_[ first_double_part_ ? 1 : 2 ].get();
                                for ( auto k = 0u; k < lhs2.size(); ++k )
                                {
                                    last_osc0_lhs_before_filter[ k ] += lhs2[ k ];
                                    last_osc0_rhs_before_filter[ k ] += rhs2[ k ];
                                }
                            }

                            if ( sub_parts_[ 3 + quad_part_id_ ].valid() )
                            {
                                const auto [ lhs, rhs ] = sub_parts_[ 3 + quad_part_id_ ].get();
                                for ( auto k = 0u; k < lhs.size(); ++k )
                                {
                                    last_osc0_lhs_before_filter[ k ] += lhs[ k ];
                                    last_osc0_rhs_before_filter[ k ] += rhs[ k ];
                                }
                            }
                        }
                    }

                    frame_pos_ += N;
                    buffer.frames.resize( initial_size );
                    if ( !use_reverb )
                    {
                        for ( auto i = 0; i < N; ++i )
                        {
                            buffer.frames[ size_type( 2 * i ) ] = 0.5 * left[ i ];
                            buffer.frames[ size_type( 2 * i + 1 ) ] = 0.5 * right[ i ];
                        }
                    }
                    else
                    {
                        for ( auto i = 0; i < N; ++i )
                        {
                            buffer.frames[ size_type( 2 * i ) ] =
                                0.5 * ( dry_reverb_ratio_ * left[ i ] +
                                        wet_reverb_ratio_ * last_osc0_lhs_before_filter[ i ] );
                            buffer.frames[ size_type( 2 * i + 1 ) ] =
                                0.5 * ( dry_reverb_ratio_ * right[ i ] +
                                        wet_reverb_ratio_ * last_osc0_rhs_before_filter[ i ] );
                        }
                    }

                    double_buffer_.swap_write_buffer( buffer );
                    {
                        std::cout << "elapsed: "
                                  << duration_cast< milliseconds >( steady_clock::now() -
                                                                    start_time )
                                         .count()
                                  << std::endl;
                        if ( use_reverb )
                        {
                            sub_parts_[ 0 ] = std::async( std::launch::async, [spectrum_left,
                                                                               spectrum_right,
                                                                               filter =
                                                                                   reverb_spectra_
                                                                                       [ 1 ]] {
                                auto s = spectrum_left;
                                auto t = spectrum_right;
                                for ( auto k = 0u; k < s.size(); ++k )
                                {
                                    s[ k ] *= 0.5f * filter.first[ k ];
                                    t[ k ] *= 0.5f * filter.second[ k ];
                                }

                                return std::pair{audio::inverse_fft( s ), audio::inverse_fft( t )};
                            } );
                            sub_parts_[ first_double_part_ ? 1 : 2 ] =
                                std::async( std::launch::async, [spectrum_left, spectrum_right,
                                                                 filter1 = reverb_spectra_[ 2 ],
                                                                 filter2 = reverb_spectra_[ 3 ]] {
                                    auto s = spectrum_left;
                                    auto t = spectrum_right;
                                    for ( auto k = 0u; k < s.size(); ++k )
                                    {
                                        s[ k ] *= 0.5f * filter1.first[ k ];
                                        t[ k ] *= 0.5f * filter1.second[ k ];
                                    }

                                    auto left = audio::inverse_fft( s );
                                    auto right = audio::inverse_fft( t );
                                    const auto offset = left.size() / 2;
                                    left.resize( left.size() + offset, 0 );
                                    right.resize( right.size() + offset, 0 );

                                    s = spectrum_left;
                                    t = spectrum_right;
                                    for ( auto k = 0u; k < s.size(); ++k )
                                    {
                                        s[ k ] *= 0.5f * filter2.first[ k ];
                                        t[ k ] *= 0.5f * filter2.second[ k ];
                                    }

                                    auto left2 = audio::inverse_fft( s );
                                    auto right2 = audio::inverse_fft( t );
                                    for ( auto i = 0u; i < left2.size(); ++i )
                                    {
                                        left[ offset + i ] += left2[ i ];
                                        right[ offset + i ] += right2[ i ];
                                    }

                                    return std::pair{std::move( left ), std::move( right )};
                                } );
                            sub_parts_[ 3 + quad_part_id_ ] = std::async(
                                std::launch::async,
                                [spectrum_left = std::move( spectrum_left ),
                                 spectrum_right = std::move( spectrum_right ), this] {
                                    auto s = spectrum_left;
                                    auto t = spectrum_right;
                                    for ( auto k = 0u; k < s.size(); ++k )
                                    {
                                        s[ k ] *= 0.5f * reverb_spectra_[ 4 ].first[ k ];
                                        t[ k ] *= 0.5f * reverb_spectra_[ 4 ].second[ k ];
                                    }

                                    auto left = audio::inverse_fft( s );
                                    auto right = audio::inverse_fft( t );
                                    const auto offset = left.size() / 2;
                                    left.resize( left.size() + 3 * offset, 0 );
                                    right.resize( right.size() + 3 * offset, 0 );

                                    s = spectrum_left;
                                    t = spectrum_right;
                                    for ( auto k = 0u; k < s.size(); ++k )
                                    {
                                        s[ k ] *= 0.5f * reverb_spectra_[ 5 ].first[ k ];
                                        t[ k ] *= 0.5f * reverb_spectra_[ 5 ].second[ k ];
                                    }

                                    auto left2 = audio::inverse_fft( s );
                                    auto right2 = audio::inverse_fft( t );
                                    for ( auto i = 0u; i < left2.size(); ++i )
                                    {
                                        left[ offset + i ] += left2[ i ];
                                        right[ offset + i ] += right2[ i ];
                                    }

                                    s = spectrum_left;
                                    t = spectrum_right;
                                    for ( auto k = 0u; k < s.size(); ++k )
                                    {
                                        s[ k ] *= 0.5f * reverb_spectra_[ 6 ].first[ k ];
                                        t[ k ] *= 0.5f * reverb_spectra_[ 6 ].second[ k ];
                                    }

                                    left2 = audio::inverse_fft( s );
                                    right2 = audio::inverse_fft( t );
                                    for ( auto i = 0u; i < left2.size(); ++i )
                                    {
                                        left[ 2 * offset + i ] += left2[ i ];
                                        right[ 2 * offset + i ] += right2[ i ];
                                    }

                                    s = spectrum_left;
                                    t = spectrum_right;
                                    for ( auto k = 0u; k < s.size(); ++k )
                                    {
                                        s[ k ] *= 0.5f * reverb_spectra_[ 7 ].first[ k ];
                                        t[ k ] *= 0.5f * reverb_spectra_[ 7 ].second[ k ];
                                    }

                                    left2 = audio::inverse_fft( s );
                                    right2 = audio::inverse_fft( t );
                                    for ( auto i = 0u; i < left2.size(); ++i )
                                    {
                                        left[ 3 * offset + i ] += left2[ i ];
                                        right[ 3 * offset + i ] += right2[ i ];
                                    }

                                    return std::pair{std::move( left ), std::move( right )};
                                } );
                            first_double_part_ = !first_double_part_;
                            quad_part_id_ = ( ++quad_part_id_ ) % 4;
                        }
                        std::lock_guard lock_left{left_mutex_};
                        std::lock_guard lock_right{right_mutex_};
                        last_osc0_lhs_before_filter.clean_and_move( N );
                        last_osc0_rhs_before_filter.clean_and_move( N );
                    }
                };

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
        void receive_message( double dt, const midi::message_t< N >& message )
        {
            if constexpr ( N == 1 )
            {
                if ( message == midi::realtime::realtime_clock() )
                {
                    clock_dt_[ clock_dt_counter_++ ] = dt;
                    if ( clock_dt_counter_ == clock_dt_.size() )
                    {
                        clock_dt_counter_ = 0;
                        auto beat_dt = std::accumulate( begin( clock_dt_ ), end( clock_dt_ ), 0.0 );
                        const auto bpm = int( std::round( 60 / beat_dt * 10 ) +
                                              std::numeric_limits< double >::epsilon() ) /
                                         10.0;
                        set_bpm_( beat_duration{bpm} );
                    }
                }
                return;
            }

            if constexpr ( N == 3 )
            {
                if ( message[ 0 ] == midi::byte::note_on )
                {
                    trigger();
                    chain( 0 ).silent_ = false;
                    chain( 1 ).silent_ = false;
                    return;
                }
                if ( message[ 0 ] == midi::byte::note_off )
                {
                    chain( 0 ).silent_ = true;
                    chain( 1 ).silent_ = true;

                    chain( 0 ).osc_fade_out_ = chain( 0 ).fade_sample_count_;
                    chain( 1 ).osc_fade_out_ = chain( 1 ).fade_sample_count_;
                    return;
                }
            }
        }

        template < class F >
        void set_bpm_functor( F f )
        {
            set_bpm_ = f;
        }

        std::vector< std::string > available_reverb_banks()
        {
            reverb_banks_.clear();
            std::vector< std::string > folders;
            const auto path = std::experimental::filesystem::current_path() /
                              std::experimental::filesystem::path(
                                  "impulse_responses/EchoThiefImpulseResponseLibrary/" );
            for ( auto& entry : std::experimental::filesystem::directory_iterator( path ) )
            {
                if ( !entry.path().has_extension() )
                {
                    reverb_banks_.emplace_back( entry.path() );
                    folders.emplace_back( entry.path().filename() );
                }
            }
            return folders;
        }

        void set_reverb_bank_id( int id )
        {
            assert( id < reverb_banks_.size() + 1 );
            if ( id > 0 )
            {
                current_reverb_bank_ = reverb_banks_[ id - 1 ];
            }
            set_reverb_type_id( 0 );
        }

        std::vector< std::string > available_reverbs()
        {
            if ( current_reverb_bank_.empty() )
            {
                return {};
            }

            std::vector< std::string > reverbs;
            reverb_files_.clear();
            for ( auto& entry :
                  std::experimental::filesystem::directory_iterator( current_reverb_bank_ ) )
            {
                if ( entry.path().has_stem() )
                {
                    const auto filename = entry.path().filename();
                    if ( filename.has_extension() && filename.extension() == ".wav" )
                    {
                        reverb_files_.emplace_back( entry.path() );
                        reverbs.emplace_back( filename.stem() );
                    }
                }
            }
            return reverbs;
        }

        void set_reverb_type_id( int id )
        {
            if ( id <= 0 || current_reverb_bank_.empty() )
            {
                std::lock_guard lock{reverb_mutex_};
                reverb_spectra_.clear();
                std::lock_guard lock_left{left_mutex_};
                last_osc0_lhs_before_filter = audio::ring_buffer_t< float >( 2 * sub_sample_size );
                std::lock_guard lock_right{right_mutex_};
                last_osc0_rhs_before_filter = audio::ring_buffer_t< float >( 2 * sub_sample_size );
                return;
            }

            auto reverb_channels = audio::wave::read( reverb_files_[ id - 1 ] );

            reverb_channels[ 0 ].resize( 32 * 1024 );
            reverb_channels[ 1 ].resize( 32 * 1024 );

            const auto start_time = std::chrono::steady_clock::now();
            const auto n_filters = reverb_channels[ 0 ].size() / ( sub_sample_size );
            const auto filter_offset = reverb_channels[ 0 ].size() / n_filters;
            std::vector< std::future< std::pair< spectrum_t, spectrum_t > > > done;
            for ( auto i = 0u; i < n_filters; ++i )
            {
                done.push_back(
                    std::async( std::launch::async, [i, filter_offset, &reverb_channels] {
                        auto left = std::vector< float >(
                            begin( reverb_channels[ 0 ] ) + i * filter_offset,
                            begin( reverb_channels[ 0 ] ) + ( i + 1 ) * filter_offset );
                        auto right = std::vector< float >(
                            begin( reverb_channels[ 1 ] ) + i * filter_offset,
                            begin( reverb_channels[ 1 ] ) + ( i + 1 ) * filter_offset );
                        left.resize( 2 * filter_offset, 0 );
                        right.resize( 2 * filter_offset, 0 );
                        return std::make_pair( audio::fft( left ), audio::fft( right ) );
                    } ) );
            }
            sub_parts_.resize( 7 );

            std::lock_guard lock{reverb_mutex_};
            reverb_spectra_.resize( n_filters );
            for ( auto i = 0; i < n_filters; ++i )
            {
                reverb_spectra_[ i ] = done[ i ].get();
            }

            const auto elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(
                                     std::chrono::steady_clock::now() - start_time )
                                     .count();
            std::cout << "set reverb in " << elapsed << "ms" << std::endl;
            std::lock_guard lock_left{left_mutex_};
            last_osc0_lhs_before_filter =
                audio::ring_buffer_t< float >( sub_sample_size + n_filters * filter_offset );
            std::lock_guard lock_right{right_mutex_};
            last_osc0_rhs_before_filter =
                audio::ring_buffer_t< float >( sub_sample_size + n_filters * filter_offset );
        }

        void set_dry_wet_ratio( double ratio ) noexcept
        {
            dry_reverb_ratio_ = audio::compute_dry_ratio( ratio );
            wet_reverb_ratio_ = audio::compute_wet_ratio( ratio );
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
        std::size_t frames_per_buffer_{256};
        std::atomic_bool running_{false};
        std::atomic_bool stopped_{false};
        std::function< void( beat_duration ) > set_bpm_ = []( beat_duration ) {};
        std::vector< double > clock_dt_;
        std::vector< double >::size_type clock_dt_counter_{0};

        std::mutex reverb_mutex_;
        using spectrum_t = std::vector< std::complex< float > >;
        std::vector< std::pair< spectrum_t, spectrum_t > > reverb_spectra_;

        std::mutex left_mutex_;
        std::mutex right_mutex_;
        audio::ring_buffer_t< float > last_osc0_lhs_before_filter{2 * sub_sample_size};
        audio::ring_buffer_t< float > last_osc0_rhs_before_filter{2 * sub_sample_size};

        std::vector< std::experimental::filesystem::path > reverb_banks_{};
        std::vector< std::experimental::filesystem::path > reverb_files_{};
        std::experimental::filesystem::path current_reverb_bank_{};

        std::atomic< double > dry_reverb_ratio_{1};
        std::atomic< double > wet_reverb_ratio_{0};

        std::vector< std::future< std::pair< std::vector< float >, std::vector< float > > > >
            sub_parts_{};
        bool first_double_part_{true};
        int quad_part_id_{0};
    };
} // namespace sequencer::backend::synth
