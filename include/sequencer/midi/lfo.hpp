#pragma once

#include <cmath>
#include <iostream>
#include <utility>

namespace sequencer::midi
{
    enum class lfo_mode
    {
        triangular = 0,
        sine,
        square
    };

    template < class T >
    T lfo_impl( double pos, double period_length, double speed, double phase, T min, T max,
                lfo_mode mode )
    {
        period_length /= speed;
        pos += phase * period_length;
        switch ( mode )
        {
        case lfo_mode::triangular:
        {
            pos += period_length / 4;
            pos = std::fmod( pos, period_length );
            const auto value = min + ( std::fmod( pos, ( period_length / 2 ) ) ) /
                                         double( period_length / 2 ) * ( max - min + 1 );
            return ( pos < period_length / 2 ) ? value : max - value;
        }
        case lfo_mode::sine:
        {
            pos = std::fmod( pos, period_length );
            return 0.5 *
                   ( max + min + ( max - min ) * std::sin( 2 * M_PI * ( pos / period_length ) ) );
        }
        case lfo_mode::square:
        {
            pos = std::fmod( pos, period_length );
            return ( pos < period_length / 2 ) ? max : min;
        }
        }
        return T{0};
    }

    template < class T >
    T lfo( std::size_t pulse_count, std::size_t pulses_per_quarter_note, int speed, int phase,
           T min, T max, lfo_mode mode )
    {
        const auto period_length = 4 * pulses_per_quarter_note * 128;
        pulse_count %= std::size_t( period_length );
        return lfo_impl< T >( pulse_count, period_length, speed, phase / 128.0, min, max, mode );
    }
} // namespace sequencer::midi
