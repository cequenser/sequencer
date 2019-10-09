#pragma once

#include <sequencer/copyable_atomic.hpp>
#include <sequencer/wave_form.hpp>

#include <iostream>
namespace sequencer::audio
{
    enum class oscillator_wave_form
    {
        sine,
        triangular,
        square,
        saw,
        pulse,
        ramp,
        noise
    };

    class oscillator_t
    {
    public:
        template < class FM >
        double operator()( double t, FM fm ) const noexcept
        {
            switch ( wave_form_ )
            {
            case oscillator_wave_form::sine:
                return signal( wave_form::sine, t, frequency_ + fm( t ) );
            case oscillator_wave_form::triangular:
                return signal( wave_form::triangular, t, frequency_ + fm( t ) );
            case oscillator_wave_form::square:
                return signal( wave_form::square, t, frequency_ + fm( t ) );
            case oscillator_wave_form::saw:
                return signal( wave_form::saw, t, frequency_ + fm( t ) );
            case oscillator_wave_form::pulse:
                return signal( wave_form::pulse_t{pulse_length_}, t, frequency_ + fm( t ) );
            case oscillator_wave_form::ramp:
                return signal( wave_form::ramp_t{1.0}, t, frequency_ + fm( t ) );
            case oscillator_wave_form::noise:
                return amplitude_ * wave_form::random();
            }
        }

        double operator()( double t ) const noexcept
        {
            return ( *this )( t, []( auto ) { return 0; } );
        }

        void set_wave_form( oscillator_wave_form wave_form )
        {
            wave_form_ = wave_form;
        }

        double frequency() const noexcept
        {
            return frequency_;
        }

        void set_frequency( double frequency ) noexcept
        {
            frequency_ = frequency;
        }

        void set_amplitude( double amplitude ) noexcept
        {
            amplitude_ = amplitude;
        }

        void set_phase( double phase ) noexcept
        {
            phase_ = phase;
        }

        void set_pulse_length( double pulse_length ) noexcept
        {
            pulse_length_ = pulse_length;
        }

        float dry_wet_ratio() const noexcept
        {
            return dry_wet_ratio_;
        }

        void set_dry_wet_ratio( float ratio ) noexcept
        {
            dry_wet_ratio_ = ratio;
        }

    private:
        template < class F >
        double signal( F f, double t, double freq ) const
        {
            return amplitude_ * f( t * freq + phase_ / freq );
        }

        copyable_atomic< oscillator_wave_form > wave_form_{oscillator_wave_form::sine};
        copyable_atomic< double > frequency_{100};
        copyable_atomic< double > amplitude_{1};
        copyable_atomic< double > phase_{0};
        copyable_atomic< double > pulse_length_{50};
        copyable_atomic< float > dry_wet_ratio_;
    };
} // namespace sequencer::audio
