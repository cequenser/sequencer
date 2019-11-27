#include <sequencer/audio/fft.hpp>
#include <sequencer/audio/sample.hpp>
#include <sequencer/audio/wave_reader.hpp>
#include <sequencer/portaudio/portaudio.hpp>

#include <cassert>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>

using std::cout;
using std::endl;

constexpr auto sample_rate = 44100;

using namespace sequencer;

using spectrum_t = std::vector< std::complex< float > >;

auto overlay_add( std::vector< float >::size_type n, const std::vector< float >& sample,
                  const std::vector< spectrum_t >& filter )
{
    using diff_t = std::vector< float >::difference_type;

    std::vector< float > output( 2 * sample.size(), 0 );
    const auto offset = sample.size() / n;
    const auto m = filter.size();
    const auto m_offset = filter.front().size();
    const auto fft_size = 2 * std::max( filter.front().size(), offset );
    std::vector< std::future< std::vector< float > > > done;

    for ( decltype( n ) i = 0; i < n; ++i )
    {
        std::vector<
            std::future< std::pair< std::vector< float >::size_type, std::vector< float > > > >
            sub_done;
        std::vector< float > sub( begin( sample ) + diff_t( i * offset ),
                                  begin( sample ) + diff_t( ( i + 1 ) * offset ) );
        sub.resize( fft_size, 0 );

        const auto sub_spectrum = audio::fft( sub );

        for ( std::remove_const_t< decltype( m ) > j = 0; j < m; ++j )
        {
            sub_done.push_back(
                std::async( std::launch::async, [&, i, j, s = sub_spectrum]() mutable {
                    for ( decltype( s.size() ) k = 0; k < s.size(); ++k )
                    {
                        s[ k ] *= 0.5f * filter[ j ][ k ];
                    }

                    return std::make_pair( j * m_offset + i * offset, audio::inverse_fft( s ) );
                } ) );
        }

        for ( auto& d : sub_done )
        {
            const auto [ offset, sub ] = d.get();
            for ( decltype( sub.size() ) k = 0; k < sub.size(); ++k )
            {
                const auto idx = k + offset;
                if ( idx >= output.size() )
                {
                    break;
                }
                output[ idx ] += sub[ k ];
            }
        }
    }
    return output;
}

auto overlay_add_sub( const spectrum_t& spectrum_left, const spectrum_t& spectrum_right,
                      const std::vector< spectrum_t >& filters_left,
                      const std::vector< spectrum_t >& filters_right )
{
    std::vector< std::future< std::tuple< std::vector< float >::size_type, std::vector< float >,
                                          std::vector< float > > > >
        done;
    for ( decltype( filters_left.size() ) j = 0; j < filters_left.size(); ++j )
    {
        const auto doff = j * spectrum_left.size();
        auto g0 = std::async( std::launch::async, [&spectrum_left, &spectrum_right, j, doff,
                                                   &filters_left, &filters_right]() mutable {
            auto s = spectrum_left;
            auto t = spectrum_right;
            for ( decltype( s.size() ) k = 0; k < s.size(); ++k )
            {
                s[ k ] *= filters_left[ j ][ k ];
                t[ k ] *= filters_right[ j ][ k ];
            }

            return std::make_tuple( doff, audio::inverse_fft( s ), audio::inverse_fft( t ) );
        } );
        done.push_back( std::move( g0 ) );
    }

    std::vector< float > tmp_left(
        2 * spectrum_left.size() + filters_left.size() * spectrum_left.size(), 0 );
    std::vector< float > tmp_right(
        2 * spectrum_right.size() + filters_right.size() * spectrum_left.size(), 0 );

    for ( auto& d : done )
    {
        const auto& [ offset2, lhs, rhs ] = d.get();
        for ( decltype( lhs.size() ) k = 0; k < lhs.size(); ++k )
        {
            const auto idx = offset2 + k;
            if ( idx >= tmp_left.size() )
                break;
            tmp_left[ idx ] += lhs[ k ];
            tmp_right[ idx ] += rhs[ k ];
        }
    }

    return std::make_pair( std::move( tmp_left ), std::move( tmp_right ) );
}

auto overlay_add( std::vector< float >::size_type n, const std::vector< float >& left,
                  const std::vector< float >& right, const std::vector< spectrum_t >& filters_left,
                  const std::vector< spectrum_t >& filters_right )
{
    assert( left.size() == right.size() );
    assert( filters_left.size() == filters_right.size() );

    std::vector< float > left_output( 2 * left.size(), 0 );
    std::vector< float > right_output( 2 * right.size(), 0 );
    const auto offset = left.size() / n;
    const auto fft_size = 2 * std::max( filters_left.front().size(), offset );
    for ( decltype( n ) i = 0; i < n; ++i )
    {
        using diff_t = std::vector< float >::difference_type;
        auto f0 = std::async( std::launch::async, [&] {
            std::vector< float > sub( begin( left ) + diff_t( i * offset ),
                                      begin( left ) + diff_t( ( i + 1 ) * offset ) );
            sub.resize( fft_size, 0 );
            return audio::fft( sub );
        } );
        auto f1 = std::async( std::launch::async, [&] {
            std::vector< float > sub( begin( right ) + diff_t( i * offset ),
                                      begin( right ) + diff_t( ( i + 1 ) * offset ) );
            sub.resize( fft_size, 0 );
            return audio::fft( sub );
        } );
        const auto s0 = f0.get();
        const auto s1 = f1.get();

        const auto [ tmp_left, tmp_right ] = overlay_add_sub( s0, s1, filters_left, filters_right );

        for ( decltype( tmp_left.size() ) k = 0; k < tmp_left.size(); ++k )
        {
            const auto idx = i * offset + k;
            if ( idx >= left_output.size() )
                break;
            left_output[ idx ] += tmp_left[ k ];
            right_output[ idx ] += tmp_right[ k ];
        }
    }

    return std::make_pair( std::move( left_output ), std::move( right_output ) );
}

auto convolution( int n, const std::vector< float >& signal, const std::vector< float >& reverb )
{
    std::vector< float > output( signal.size() + reverb.size(), 0 );
    const auto offset = signal.size() / n;

    for ( auto i = 0; i < n; ++i )
    {
        using diff_t = std::vector< float >::difference_type;
        std::vector< float > sub_signal( begin( signal ) + diff_t( i * offset ),
                                         begin( signal ) + diff_t( ( i + 1 ) * offset ) );
        std::vector< std::future< void > > done;

        using size_t = std::vector< float >::size_type;
        using diff_t = std::vector< float >::difference_type;
        const diff_t n_threads = 2;
        const diff_t parallel_offset = ( reverb.size() + offset ) / n_threads;
        for ( diff_t l = 0; l < n_threads; ++l )
        {
            done.push_back( std::async( std::launch::async, [&, l] {
                for ( auto j = l * parallel_offset; j < ( l + 1 ) * parallel_offset; ++j )
                {
                    for ( auto k =
                              std::max( diff_t( 0 ), diff_t( j + 1 - diff_t( reverb.size() ) ) );
                          k < std::min( j + 1, diff_t( sub_signal.size() ) ); ++k )
                    {
                        output[ size_t( i * offset + j ) ] +=
                            reverb[ size_t( j - k ) ] * sub_signal[ size_t( k ) ];
                    }
                }
            } ) );
        }

        for ( auto& d : done )
        {
            d.wait();
        }
    }
    return output;
}

auto convolution( std::vector< float >::size_type n, const std::vector< float >& left,
                  const std::vector< float >& right, const std::vector< float >& left_reverb,
                  const std::vector< float > right_reverb )
{
    std::vector< float > left_output( left.size() + left_reverb.size(), 0 );
    std::vector< float > right_output( right.size() + right_reverb.size(), 0 );
    const auto offset = left.size() / n;

    using size_t = std::vector< float >::size_type;
    using diff_t = std::vector< float >::difference_type;

    for ( size_t i = 0; i < n; ++i )
    {
        const auto offset0 = diff_t( i * offset );
        const auto offset1 = offset0 + diff_t( offset );
        std::vector< float > sub_left( begin( left ) + offset0, begin( left ) + offset1 );
        std::vector< float > sub_right( begin( right ) + offset0, begin( right ) + offset1 );

        const auto start_time = std::chrono::steady_clock::now();
        std::vector< std::future< void > > done;
        const size_t n_threads = 1;
        const size_t parallel_offset = ( left_reverb.size() + offset ) / n_threads;
        for ( size_t l = 0; l < n_threads; ++l )
        {
            done.push_back( std::async( std::launch::async, [&, l] {
                for ( size_t j = l * parallel_offset; j < ( l + 1 ) * parallel_offset; ++j )
                {
                    for ( auto k = std::max( size_t( 0 ), j + size_t( 1 ) - left_reverb.size() );
                          k < std::min( j + size_t( 1 ), sub_left.size() ); ++k )
                    {
                        left_output[ i * offset + j ] += left_reverb[ j - k ] * sub_left[ k ];
                        right_output[ i * offset + j ] += right_reverb[ j - k ] * sub_right[ k ];
                    }
                }
            } ) );
        }

        for ( auto& d : done )
        {
            d.wait();
        }
        auto elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(
                           std::chrono::steady_clock::now() - start_time )
                           .count();
        std::cout << "elapsed: " << elapsed << "ms" << std::endl;
    }
    return std::make_pair( std::move( left_output ), std::move( right_output ) );
}

void write_spectrum( const std::vector< std::complex< float > >& spectrum, const std::string& name )
{
    std::ofstream stream( name );
    for ( decltype( spectrum.size() ) i = 0; i < spectrum.size(); ++i )
    {
        stream << i << "," << std::abs( spectrum[ i ] ) << std::endl;
    }
}

int main()
{
    try
    {
        auto channels =
            audio::wave::read( "/home/lars/Downloads/EchoThiefImpulseResponseLibrary/"
                               "EchoThiefImpulseResponseLibrary/Brutalism/PacificHall.wav" );
        channels[ 0 ].resize( 24 * 1024 );
        channels[ 1 ].resize( 24 * 1024 );

        auto sample_channels =
            audio::wave::read( "/usr/lib/libreoffice/share/gallery/sounds/space3.wav" );
        sample_channels.resize( 2 );
        sample_channels[ 1 ] = sample_channels[ 0 ];
        sample_channels[ 0 ].resize( 64 * 1024, 0 );
        sample_channels[ 1 ].resize( 64 * 1024, 0 );

        auto sample = audio::read_write_lockable< audio::sample_t >{sample_channels[ 0 ].size(),
                                                                    audio::mode_t::stereo};
        for ( decltype( sample_channels[ 0 ].size() ) i = 0; i < sample_channels[ 0 ].size(); ++i )
        {
            sample.frames[ 2 * i ] = sample_channels[ 0 ][ i ];
            sample.frames[ 2 * i + 1 ] = sample_channels[ 1 ][ i ];
        }
        portaudio::portaudio pa;

        constexpr auto frames_per_buffer = 512;
        portaudio::stream_t stream;
        auto parameters = pa.get_parameters( pa.get_default_output_device() );

        using size_t = std::vector< float >::size_type;
        using diff_t = std::vector< float >::difference_type;
        const auto n_samples = 128;
        const size_t n_filters = 128;
        const diff_t n_offset = sample_channels[ 0 ].size() / n_samples;
        const diff_t n_filter_offset = channels[ 0 ].size() / n_filters;
        std::cout << "offsets: " << n_offset << ", " << n_filter_offset << std::endl;
        const size_t fft_size = 2 * std::max( n_offset, n_filter_offset );
        std::vector< std::vector< std::complex< float > > > filter_parts0( n_filters );
        std::vector< std::vector< std::complex< float > > > filter_parts1( n_filters );

        for ( diff_t i = 0; i < n_filters; ++i )
        {
            auto ch0 = std::vector< float >( begin( channels[ 0 ] ) + i * n_filter_offset,
                                             begin( channels[ 0 ] ) + ( i + 1 ) * n_filter_offset );
            auto ch1 = std::vector< float >( begin( channels[ 1 ] ) + i * n_filter_offset,
                                             begin( channels[ 1 ] ) + ( i + 1 ) * n_filter_offset );
            ch0.resize( fft_size, 0 );
            ch1.resize( fft_size, 0 );
            filter_parts0[ size_t( i ) ] = audio::fft( ch0 );
            filter_parts1[ size_t( i ) ] = audio::fft( ch1 );
        }

        const auto start_time2 = std::chrono::steady_clock::now();
        /// try other implementations to see which works better when. Direct convolutions are faster
        /// for shorter samples or large differences in sample sizes, else fft-based convolution
        /// performs better.

        /// separate direct convolutions for left and right
        //        auto ch0f = std::async( std::launch::async, [&] {
        //            return convolution( n_samples, sample_channels[ 0 ], channels[ 0 ] );
        //        } );
        //        auto ch1f = std::async( std::launch::async, [&] {
        //            return convolution( n_samples, sample_channels[ 1 ], channels[ 1 ] );
        //        } );
        //        const auto ch0_fin = ch0f.get();
        //        const auto ch1_fin = ch1f.get();

        /// simultaneous direct convolution of left and right channel
        //        const auto [ch0_fin, ch1_fin] = convolution(n_samples, sample_channels[0],
        //        sample_channels[1], channels[0], channels[1]);

        /// overlap-add fft-convolution
        const auto [ ch0_fin, ch1_fin ] = overlay_add(
            n_samples, sample_channels[ 0 ], sample_channels[ 1 ], filter_parts0, filter_parts1 );
        std::cout << "reverb time: "
                  << std::chrono::duration_cast< std::chrono::milliseconds >(
                         std::chrono::steady_clock::now() - start_time2 )
                         .count()
                  << "ms" << std::endl;
        auto sample2 =
            audio::read_write_lockable< audio::sample_t >{ch0_fin.size(), audio::mode_t::stereo};
        for ( decltype( ch0_fin.size() ) i = 0; i < ch0_fin.size(); ++i )
        {
            sample2.frames[ 2 * i ] = ch0_fin[ i ];
            sample2.frames[ 2 * i + 1 ] = ch1_fin[ i ];
        }

        cout << " === Playing input back. === " << endl;
        auto reader = audio::sample_reader_t{sample};
        stream.open_output_stream( parameters, sample_rate, frames_per_buffer,
                                   portaudio::play_callback< audio::sample_reader_t >, &reader );
        stream.start();
        cout << "Waiting for playback to finish." << endl;
        while ( stream.is_active() )
            std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        stream.close();
        cout << "Done." << endl;

        cout << " === Playing back convolution reverb. === " << endl;
        auto reader2 = audio::sample_reader_t{sample2};
        stream.open_output_stream( parameters, sample_rate, frames_per_buffer,
                                   portaudio::play_callback< audio::sample_reader_t >, &reader2 );
        stream.start();
        cout << "Waiting for playback to finish." << endl;
        while ( stream.is_active() )
            std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        stream.close();
        cout << "Done." << endl;
    }
    catch ( const std::exception& exception )
    {
        cout << "Failure: " << exception.what() << endl;
    }

    return 0;
}
