#pragma once

#include <chrono>
#include <cmath>
#include <random>

namespace sequencer::wave_form
{
    class pulse_t
    {
    public:
        constexpr explicit pulse_t( double pulse_width ) noexcept : pulse_width_( pulse_width )
        {
        }

        double operator()( double t ) noexcept
        {
            t = std::fmod( t, 1.0 );
            return ( t < pulse_width_ ) ? 1 : 0;
        }

    private:
        double pulse_width_;
    };

    inline double sine( double t )
    {
        return std::sin( 2 * M_PI * t );
    }

    inline double square( double t ) noexcept
    {
        return std::fmod( t, 1.0 ) < 0.5 ? 1 : -1;
    }

    inline double triangular( double t ) noexcept
    {
        t = std::fmod( t + 0.25, 1.0 );
        const auto value = -1 + 4 * std::fmod( t, 0.5 );
        return t < 0.5 ? value : -value;
    }

    inline double saw( double t ) noexcept
    {
        return 1 - 2 * std::fmod( t, 1.0 );
    }

    class exp_t
    {
    public:
        constexpr explicit exp_t( double scale ) noexcept : scale_{scale}
        {
        }

        double operator()( double t ) noexcept
        {
            return std::exp( -scale_ * std::fmod( t, 1.0 ) );
        }

    private:
        double scale_{1};
    };

    class ramp_t
    {
    public:
        constexpr explicit ramp_t( double slope ) noexcept : slope_{slope}
        {
        }

        double operator()( double t ) noexcept
        {
            t = std::fmod( t, 1.0 );
            return ( t > ( 1 / slope_ ) ) ? 0 : ( slope_ * t );
        }

    private:
        double slope_{1};
    };

    inline double random()
    {
        using clock = std::chrono::system_clock;

        static const auto seed =
            clock::from_time_t( std::time( nullptr ) ).time_since_epoch().count();
        static std::minstd_rand engine( seed );
        static std::uniform_real_distribution< double > distribution{};

        return distribution( engine, std::uniform_real_distribution< double >::param_type{
                                         -1, 1 + std::numeric_limits< double >::epsilon()} );
    }
} // namespace sequencer::wave_form
