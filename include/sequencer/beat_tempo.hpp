#pragma once

namespace sequencer
{

    class beat_tempo
    {
    public:
        using rep = double;

        constexpr explicit beat_tempo( rep beats_per_minute ) noexcept
            : beats_per_minute_( beats_per_minute )
        {
        }

        constexpr rep beats_per_minute() const noexcept
        {
            return beats_per_minute_;
        }

    private:
        rep beats_per_minute_;
    };

    constexpr beat_tempo operator"" _bpm( long double value )
    {
        return beat_tempo( value );
    }

    static_assert( ( 120.0_bpm ).beats_per_minute() == 120.0 );

} // namespace sequencer
