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

    namespace func
    {
        inline double sine( double t )
        {
            return std::sin( 2 * M_PI * t );
        }

        inline double square( double t ) noexcept
        {
            return t < 0.5 ? 1 : -1;
        }

        inline double triangular( double t ) noexcept
        {
            t = std::fmod( t + 0.25, 1.0 );
            const auto value = -1 + 4 * std::fmod( t, 0.5 );
            return t < 0.5 ? value : -value;
        }
    } // namespace func

    template < class T >
    T lfo_impl( double pos, double period_length, double speed, double phase, T min, T max,
                lfo_mode mode )
    {
        period_length /= speed;
        pos += phase * period_length;
        pos = std::fmod( pos, period_length ) / period_length;
        auto value = 0.0;
        switch ( mode )
        {
        case lfo_mode::triangular:
            value = func::triangular( pos );
            break;
        case lfo_mode::sine:
            value = func::sine( pos );
            break;
        case lfo_mode::square:
            value = func::square( pos );
            break;
        }
        return 0.5 * ( max + min + ( max - min ) * value );
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
