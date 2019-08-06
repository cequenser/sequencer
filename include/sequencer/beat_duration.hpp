#pragma once

#include <sequencer/beat_tempo.hpp>
#include <sequencer/float_type.hpp>

#include <cassert>
#include <chrono>
#include <limits>
#include <ostream>

namespace sequencer
{

    class beat_duration
    {
        using minutes = std::chrono::duration< double, std::ratio< 60, 1 > >;
        static constexpr auto ticks_per_unit = 1024 * 1024 * 9;

    public:
        using rep = double;
        using seconds = std::chrono::duration< double, std::ratio< 1, 1 > >;

        constexpr explicit beat_duration( rep beats ) noexcept : duration_( beats )
        {
        }

        template < typename Duration >
        constexpr explicit beat_duration( Duration duration, beat_tempo tempo ) noexcept
            : duration_( std::chrono::duration_cast< minutes >( duration ).count() *
                         tempo.beats_per_minute() )
        {
        }

        constexpr rep beats() const noexcept
        {
            return duration_.to_double();
        }

        constexpr seconds duration( beat_tempo tempo ) const noexcept
        {
            return minutes{beats() / tempo.beats_per_minute()};
        }

        constexpr bool operator==( beat_duration other ) const noexcept
        {
            return duration_ == other.duration_;
        }

        constexpr beat_duration& operator+=( beat_duration other ) noexcept
        {
            duration_ += other.duration_;
            return *this;
        }

        constexpr beat_duration operator-() noexcept
        {
            return beat_duration( -beats() );
        }

        constexpr bool operator<( beat_duration other ) const noexcept
        {
            return duration_ < other.duration_;
        }

        constexpr bool operator<=( beat_duration other ) const noexcept
        {
            return duration_ <= other.duration_;
        }

    private:
        float_type< ticks_per_unit > duration_;
    };

    constexpr beat_duration operator+( beat_duration lhs, beat_duration rhs ) noexcept
    {
        return lhs += rhs;
    }

    inline std::ostream& operator<<( std::ostream& os, beat_duration duration )
    {
        return os << duration.beats() << " beats";
    }

    constexpr beat_duration operator"" _beats( long double value ) noexcept
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
    };
} // namespace std
