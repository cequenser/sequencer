#pragma once

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

    constexpr beats_per_minute operator"" _bpm( long double value ) noexcept
    {
        return beats_per_minute( value );
    }

    constexpr beats_per_minute operator"" _bpm( unsigned long long int value ) noexcept
    {
        return beats_per_minute( value );
    }

    static_assert( ( 120.0_bpm ).to_double() == 120.0 );

} // namespace sequencer
