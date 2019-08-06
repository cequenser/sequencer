#pragma once

#include <sequencer/beat_tempo.hpp>

#include <cassert>
#include <chrono>
#include <limits>
#include <ostream>

namespace sequencer
{
    template < class T >
    constexpr int constexpr_round( T a ) noexcept
    {
        return ( a > 0 ) ? int( a + T{0.5} ) : int( a - T{0.5} );
    }

    class beat_duration
    {
        using minutes = std::chrono::duration< double, std::ratio< 60, 1 > >;

    public:
        static constexpr double eps = 1.0 / ( 1024 * 1024 * 9 );
        using rep = double;
        using seconds = std::chrono::duration< double, std::ratio< 1, 1 > >;

        constexpr explicit beat_duration( rep beats )
            : tick_count_( constexpr_round( beats / eps ) + std::numeric_limits< rep >::epsilon() )
        {
        }

        template < typename Duration >
        constexpr explicit beat_duration( Duration duration, beat_tempo tempo )
            : beat_duration( std::chrono::duration_cast< minutes >( duration ).count() *
                             tempo.beats_per_minute() )
        {
        }

        constexpr rep beats() const noexcept
        {
            return tick_count_ * eps;
        }

        constexpr seconds duration( beat_tempo tempo ) const noexcept
        {
            return minutes{beats() / tempo.beats_per_minute()};
        }

        constexpr bool operator==( beat_duration other ) const noexcept
        {
            return tick_count_ == other.tick_count_;
        }

        constexpr beat_duration& operator+=( beat_duration other ) noexcept
        {
            tick_count_ += other.tick_count_;
            return *this;
        }

        constexpr beat_duration operator-() noexcept
        {
            return beat_duration( -beats() );
        }

        constexpr bool operator<( beat_duration other ) const noexcept
        {
            return tick_count_ < other.tick_count_;
        }

        constexpr bool operator<=( beat_duration other ) const noexcept
        {
            return tick_count_ <= other.tick_count_;
        }

    private:
        std::int64_t tick_count_;
    };

    constexpr beat_duration operator+( beat_duration lhs, beat_duration rhs ) noexcept
    {
        return lhs += rhs;
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
    };
} // namespace std
