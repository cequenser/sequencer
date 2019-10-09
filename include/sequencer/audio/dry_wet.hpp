#pragma once

#include <sequencer/copyable_atomic.hpp>

#include <utility>

namespace sequencer::audio
{
    inline std::pair< float, float > operator*( float a, std::pair< float, float > p )
    {
        p.first *= a;
        p.second *= a;
        return p;
    }

    inline std::pair< float, float > operator+( float a, std::pair< float, float > p )
    {
        p.first += a;
        p.second += a;
        return p;
    }

    template < class T >
    constexpr T compute_dry_ratio( T x ) noexcept
    {
        return x > T( 0.5 ) ? T( 1 ) : 2 * x;
    }

    template < class T >
    constexpr T compute_wet_ratio( T x ) noexcept
    {
        return x < T( 0.5 ) ? T( 1 ) : 2 * ( T( 1 ) - x );
    }

    template < class F, bool use_input = false >
    class dry_wet_t : public F
    {
    public:
        using F::F;

        auto operator()( float x ) noexcept
        {
            const auto dry_ratio = compute_dry_ratio( ratio_.load() );
            const auto wet_ratio = compute_wet_ratio( ratio_.load() );
            return dry_ratio * ( use_input ? x : 1.0 ) + wet_ratio * F::operator()( x );
        }

        void set_dry_wet_ratio( float ratio ) noexcept
        {
            ratio_ = ratio;
        }

    private:
        copyable_atomic< float > ratio_{0.0};
    };
} // namespace sequencer::audio
