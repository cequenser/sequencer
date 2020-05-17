#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>

#include <sequencer/assert.hpp>

namespace sequencer::audio
{
    constexpr auto ideal_high_pass( double freq, double cutoff ) noexcept
    {
        return freq > cutoff ? 1 : 0;
    }

    constexpr auto ideal_low_pass( double freq, double cutoff ) noexcept
    {
        return freq < cutoff ? 1 : 0;
    }

    constexpr auto low_pass( double freq, double gain, double transition )
    {
        return ( gain * transition ) / ( freq + transition );
    }

    constexpr auto high_pass( double freq, double gain, double transition )
    {
        return low_pass( 1 / freq, gain, transition );
    }

    constexpr auto low_shelf( double freq, double gain, double transition )
    {
        return 1 + low_pass( freq, gain, transition );
    }

    constexpr auto high_shelf( double freq, double gain, double transition )
    {
        return 1 + high_pass( freq, gain, transition );
    }

    template < class Container, class TransferFunction >
    void filter( Container& c, TransferFunction f, double base_frequency )
    {
        using std::begin;
        using std::end;
        auto counter = 0;
        std::for_each( begin( c ), end( c ),
                       [&]( auto& value ) { value *= f( ( counter++ ) * base_frequency ); } );
    }

    template < class Container, class TransferFunction1, class TransferFunction2 >
    void filter( Container& c, TransferFunction1 f, TransferFunction2 g, double base_frequency )
    {

        using std::begin;
        using std::end;
        auto counter = 0;
        std::for_each( begin( c ), end( c ), [&]( auto& value ) {
            const auto freq = ( counter++ ) * base_frequency;
            value *= f( freq ) * g( freq );
        } );
    }

    namespace window
    {
        inline double hamming( int n, int N )
        {
            SEQUENCER_ASSERT( n >= 0 )
            SEQUENCER_ASSERT( n <= N )

            return 0.54 + 0.46 * std::cos( 2 * n * M_PI / N );
        }

        inline double blackman( int n, int N )
        {
            SEQUENCER_ASSERT( n >= 0 )
            SEQUENCER_ASSERT( n <= N )

            return 0.42 - 0.5 * std::cos( 2 * n * M_PI / N ) +
                   0.08 * std::cos( ( 4 * n * M_PI / N ) );
        }
    } // namespace window
} // namespace sequencer::audio
