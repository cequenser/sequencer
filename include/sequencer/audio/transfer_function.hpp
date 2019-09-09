#pragma once

#include <algorithm>

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

    constexpr auto low_shelf( double freq, double gain, double transition )
    {
        return 1 + ( gain * transition ) / ( freq + transition );
    }

    constexpr auto high_shelf( double freq, double gain, double transition )
    {
        return low_shelf( 1 / freq, gain, transition );
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
} // namespace sequencer::audio
