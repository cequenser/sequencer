#include <sequencer/audio/fft.hpp>
#include <sequencer/audio/transfer_function.hpp>

#include <catch2/catch.hpp>

constexpr auto signal_size = 8u * 32u;

namespace
{
    template < int Frequency >
    double sine( double t )
    {
        return std::sin( Frequency * 2 * M_PI * t );
    }

    double sine_50_and_80( double t )
    {
        return 0.5 * sine< 80 >( t ) + sine< 50 >( t );
    }

    template < class T >
    std::vector< T > get_signal()
    {
        auto signal = std::vector< T >( signal_size );
        auto counter = T( 0 );
        std::generate( begin( signal ), end( signal ), [&counter] {
            const auto t = counter++ / signal_size;
            return sine_50_and_80( t );
        } );
        return signal;
    }
} // namespace

SCENARIO( "ideal high pass", "[transfer_function]" )
{
    using namespace sequencer::audio;

    GIVEN( "overlapping sine waves of 50 and 80 Hz" )
    {
        auto signal = get_signal< double >();

        WHEN( "fft is computed" )
        {
            auto spectrum = fft( signal );

            AND_WHEN( "spectrum is filtered with ideal high pass filter and cutoff at 49 Hz" )
            {
                filter( spectrum, []( double f ) { return ideal_high_pass( f, 49 ); }, 1 );

                THEN( "the corresponding signal coincides with the given signal" )
                {
                    const auto signal2 = inverse_fft( spectrum );

                    CHECK( std::equal( begin( signal ), end( signal ), begin( signal2 ),
                                       end( signal2 ), []( auto lhs, auto rhs ) {
                                           return std::abs( lhs - rhs ) <
                                                  1e4 * std::numeric_limits< double >::epsilon();
                                       } ) );
                }
            }

            AND_WHEN( "spectrum is filtered with ideal high pass filter and cutoff at 50 Hz" )
            {
                filter( spectrum, []( double f ) { return ideal_high_pass( f, 50 ); }, 1 );

                THEN( "only the 80 Hz component remains" )
                {
                    const auto signal2 = inverse_fft( spectrum );
                    auto counter = 0.0;
                    CHECK(
                        std::all_of( begin( signal2 ), end( signal2 ), [&counter]( double value ) {
                            return std::abs( value -
                                             0.5 * sine< 80 >( ( counter++ ) / signal_size ) ) <
                                   1e4 * std::numeric_limits< double >::epsilon();
                        } ) );
                }
            }
        }
    }
}

SCENARIO( "ideal low pass", "[transfer_function]" )
{
    using namespace sequencer::audio;

    GIVEN( "overlapping sine waves of 50 and 80 Hz" )
    {
        auto signal = get_signal< double >();

        WHEN( "fft is computed" )
        {
            auto spectrum = fft( signal );

            AND_WHEN( "spectrum is filtered with ideal low pass filter and cutoff at 81 Hz" )
            {
                filter( spectrum, []( double f ) { return ideal_low_pass( f, 81 ); }, 1 );

                THEN( "the corresponding signal coincides with the given signal" )
                {
                    const auto signal2 = inverse_fft( spectrum );

                    CHECK( std::equal( begin( signal ), end( signal ), begin( signal2 ),
                                       end( signal2 ), []( auto lhs, auto rhs ) {
                                           return std::abs( lhs - rhs ) <
                                                  1e4 * std::numeric_limits< double >::epsilon();
                                       } ) );
                }
            }

            AND_WHEN( "spectrum is filtered with ideal low pass filter and cutoff at 50 Hz" )
            {
                filter( spectrum, []( double f ) { return ideal_low_pass( f, 80 ); }, 1 );

                THEN( "only the 50 Hz component remains" )
                {
                    const auto signal2 = inverse_fft( spectrum );
                    auto counter = 0.0;
                    CHECK(
                        std::all_of( begin( signal2 ), end( signal2 ), [&counter]( double value ) {
                            return std::abs( value - sine< 50 >( ( counter++ ) / signal_size ) ) <
                                   1e4 * std::numeric_limits< double >::epsilon();
                        } ) );
                }
            }
        }
    }
}
