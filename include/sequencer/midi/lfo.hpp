#pragma once

#include <utility>

namespace sequencer::midi
{
    enum class lfo_mode
    {
        square
    };

    template < class T >
    T lfo( std::size_t step, int speed, int phase, T min, T max, lfo_mode mode )
    {
        const auto a = 1024 / speed;
        switch ( mode )
        {
        case lfo_mode::square:
        {
            const auto phase_offset = int( a * phase / 64.0 );
            return ( int( step + phase_offset ) % ( 2 * a ) < a ) ? max : min;
        }
        }
        return T{0};
    }
} // namespace sequencer::midi
