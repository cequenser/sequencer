#pragma once

#include <sequencer/copyable_atomic.hpp>

namespace sequencer::audio
{
    class dry_wet_t
    {
    public:
        auto operator()( float dry, float wet ) const noexcept
        {
            return ratio_ * dry + ( 1.0f - ratio_ ) * wet;
        }

        void set_dry_wet_ratio( float ratio ) noexcept
        {
            ratio_ = ratio;
        }

    private:
        copyable_atomic< float > ratio_{1.0};
    };
} // namespace sequencer::audio
