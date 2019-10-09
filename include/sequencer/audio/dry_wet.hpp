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

    template < class F, bool use_input = false >
    class dry_wet_t : public F
    {
    public:
        using F::F;

        auto operator()( float x ) noexcept
        {
            return ratio_ * ( use_input ? x : 1.0 ) + ( 1.0 - ratio_ ) * F::operator()( x );
        }

        void set_dry_wet_ratio( float ratio ) noexcept
        {
            ratio_ = ratio;
        }

    private:
        copyable_atomic< float > ratio_{0.0};
    };
} // namespace sequencer::audio
