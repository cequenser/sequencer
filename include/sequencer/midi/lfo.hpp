#pragma once

#include <sequencer/wave_form.hpp>

#include <cmath>
#include <limits>
#include <utility>

namespace sequencer::midi
{
    enum class lfo_mode
    {
        triangular = 0,
        sine,
        square,
        saw,
        exp,
        ramp,
        random
    };

    template < class T >
    T lfo_impl( double pos, double period_length, double speed, double phase, T min, T max,
                lfo_mode mode )
    {
        period_length /= std::abs( speed );
        pos += phase * period_length;
        pos = std::fmod( pos, period_length ) / period_length;
        if ( speed < 0 )
        {
            pos = std::fmod( 1 - pos, period_length ) - std::numeric_limits< double >::epsilon();
        }
        auto value = 0.0;
        switch ( mode )
        {
        case lfo_mode::triangular:
            value = wave_form::triangular( pos );
            break;
        case lfo_mode::sine:
            value = wave_form::sine( pos );
            break;
        case lfo_mode::square:
            value = wave_form::square( pos );
            break;
        case lfo_mode::saw:
            value = wave_form::saw( pos );
            break;
        case lfo_mode::exp:
            value = wave_form::exp_t{8}( pos );
            break;
        case lfo_mode::ramp:
            value = wave_form::ramp_t{4.0 / 3}( pos );
            break;
        case lfo_mode::random:
            value = wave_form::random();
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
