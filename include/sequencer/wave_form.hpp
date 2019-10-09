#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <random>

namespace sequencer::wave_form
{
    inline double normalize_time( double t ) noexcept
    {
        return std::abs( std::fmod( t, 1.0 ) );
    }

    class pulse_t
    {
    public:
        constexpr explicit pulse_t( double pulse_width ) noexcept : pulse_width_( pulse_width )
        {
        }

        double operator()( double t ) noexcept
        {
            t = normalize_time( t );
            return ( t < pulse_width_ ) ? 1 : 0;
        }

    private:
        double pulse_width_;
    };

    template < int N >
    std::array< double, N > create_sine_sample() noexcept
    {
        std::array< double, N > sample;
        typename std::array< double, N >::size_type counter = 0;
        std::generate( begin( sample ), end( sample ),
                       [&counter] { return std::sin( 2 * M_PI * double( counter++ ) / N ); } );
        return sample;
    }

    inline double sine( double t )
    {
        constexpr auto sample_rate = 4 * 44100u;
        static const auto sine_sample = create_sine_sample< sample_rate >();
        return sine_sample[ std::size_t( t * sample_rate ) % sample_rate ];
    }

    inline double sinc( double cutoff, int i )
    {
        if ( i == 0 )
        {
            return 2 * cutoff;
        }
        if ( i < 0 )
        {
            return -sine( cutoff * -i ) / ( M_PI * i );
        }
        return sine( cutoff * i ) / ( M_PI * i );
    }

    constexpr double square( double t ) noexcept
    {
        return normalize_time( t ) < 0.5 ? 1 : -1;
    }

    inline double triangular( double t ) noexcept
    {
        t = normalize_time( t + 0.25 );
        const auto value = -1 + 4 * std::fmod( t, 0.5 );
        return t < 0.5 ? value : -value;
    }

    constexpr double saw( double t ) noexcept
    {
        return 1 - 2 * normalize_time( t );
    }

    class exp_t
    {
    public:
        constexpr explicit exp_t( double scale ) noexcept : scale_{scale}
        {
        }

        double operator()( double t ) const noexcept
        {
            return std::exp( -scale_ * normalize_time( t ) );
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

        constexpr double operator()( double t ) const noexcept
        {
            t = normalize_time( t );
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
