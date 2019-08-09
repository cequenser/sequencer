#pragma once

#include <sequencer/chrono/units.hpp>

#include <chrono>

namespace sequencer
{

    class beats_per_minute
    {
    public:
        using rep = double;

        constexpr explicit beats_per_minute( rep beats_per_minute ) noexcept
            : beats_per_minute_( beats_per_minute )
        {
        }

        constexpr rep to_double() const noexcept
        {
            return beats_per_minute_;
        }

    private:
        rep beats_per_minute_;
    };

    class beats
    {
    public:
        constexpr explicit beats( double value ) noexcept : value_( value )
        {
        }

        constexpr double to_double() const noexcept
        {
            return value_;
        }

    private:
        double value_;
    };

    constexpr beats_per_minute operator"" _bpm( long double value ) noexcept
    {
        return beats_per_minute( value );
    }

    static_assert( ( 120.0_bpm ).to_double() == 120.0 );

} // namespace sequencer
