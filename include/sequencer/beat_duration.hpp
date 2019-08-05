#pragma once

#include <sequencer/beat_tempo.hpp>

#include <cassert>
#include <chrono>
#include <limits>
#include <ostream>

namespace sequencer
{

    class beat_duration
    {
        using minutes = std::chrono::duration< double, std::ratio< 60, 1 > >;

    public:
        using rep = double;
        using seconds = std::chrono::duration< double, std::ratio< 1, 1 > >;

        constexpr explicit beat_duration( rep beats ) : beats_( beats )
        {
        }

        template < typename Duration >
        constexpr explicit beat_duration( Duration duration, beat_tempo tempo )
            : beats_( std::chrono::duration_cast< minutes >( duration ).count() *
                      tempo.beats_per_minute() )
        {
        }

        constexpr rep beats() const noexcept
        {
            return beats_;
        }

        constexpr seconds duration( beat_tempo tempo ) const noexcept
        {
            return minutes{beats_ / tempo.beats_per_minute()};
        }

        constexpr bool operator==( beat_duration other ) const noexcept
        {
            return beats() == other.beats();
        }

        constexpr beat_duration& operator+=( beat_duration other ) noexcept
        {
            beats_ += other.beats();
            return *this;
        }

        constexpr beat_duration operator-() noexcept
        {
            return beat_duration( -beats_ );
        }

    private:
        rep beats_;
    };

    constexpr beat_duration operator+( beat_duration lhs, beat_duration rhs ) noexcept
    {
        return lhs += rhs;
    }

    constexpr bool operator<( beat_duration lhs, beat_duration rhs ) noexcept
    {
        return lhs.beats() < rhs.beats();
    }

    inline std::ostream& operator<<( std::ostream& os, beat_duration duration )
    {
        return os << duration.beats() << " beats";
    }

    constexpr beat_duration operator"" _beats( long double value )
    {
        return beat_duration( value );
    }

    static_assert( beat_duration( std::chrono::seconds( 30 ), 120.0_bpm ).beats() == 60.0 );
    static_assert( beat_duration( std::chrono::seconds( 30 ), 120.0_bpm ).duration( 120.0_bpm ) ==
                   std::chrono::seconds( 30 ) );
} // namespace sequencer

namespace std
{
    template <>
    class numeric_limits< sequencer::beat_duration >
    {
    public:
        static constexpr sequencer::beat_duration epsilon() noexcept
        {
            return sequencer::beat_duration{std::numeric_limits< double >::epsilon()};
        }

        static sequencer::beat_duration max() noexcept
        {
            return sequencer::beat_duration{std::numeric_limits< double >::max()};
        }
    };
} // namespace std
