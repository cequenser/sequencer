#pragma once

#include <iostream>
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
        const auto period_length = 2 * 1024 / speed;
        switch ( mode )
        {
        case lfo_mode::square:
        {
            const auto phase_offset = int( period_length * phase / 128.0 );
            return ( int( step + phase_offset ) % ( period_length ) < period_length / 2 ) ? max
                                                                                          : min;
        }
        }
        return T{0};
    }

    template < class T >
    T lfo( std::size_t pulse_count, std::size_t steps_per_quarter_note,
           std::size_t pulses_per_quarter_note, int speed, int phase, T min, T max, lfo_mode mode )
    {
        const auto period_length = int( 2 * 1024 * 4 * pulses_per_quarter_note ) /
                                   ( speed * int( steps_per_quarter_note ) );
        switch ( mode )
        {
        case lfo_mode::square:
        {
            const auto phase_offset = int( period_length * phase / 128.0 );
            return ( int( pulse_count + phase_offset ) % ( period_length ) < period_length / 2 )
                       ? max
                       : min;
        }
        }
        return T{0};
    }
} // namespace sequencer::midi
