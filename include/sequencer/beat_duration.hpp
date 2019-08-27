#pragma once

#include <sequencer/beats_per_minute.hpp>
#include <sequencer/chrono/units.hpp>
#include <sequencer/fixed_point_type.hpp>

#include <chrono>
#include <limits>
#include <ostream>

namespace sequencer
{
    class beat_duration
    {
    public:
        static constexpr auto ticks_per_unit = 1024 * 1024 * 9;

        using rep = double;
        using internal_rep = fixed_point_type< ticks_per_unit >;

        constexpr explicit beat_duration( rep beats ) noexcept : duration_( beats )
        {
        }

        constexpr rep to_double() const noexcept
        {
            return duration_.to_double();
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
            return beat_duration( -to_double() );
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
        internal_rep duration_;
    };

    constexpr beat_duration operator+( beat_duration lhs, beat_duration rhs ) noexcept
    {
        return lhs += rhs;
    }

    template < class Rep, class Period >
    constexpr beat_duration operator*( const std::chrono::duration< Rep, Period >& duration,
                                       beats_per_minute tempo ) noexcept
    {
        return beat_duration{std::chrono::duration_cast< chrono::minutes >( duration ).count() *
                             tempo.to_double()};
    }

    template < class Rep, class Period >
    constexpr beat_duration
    operator*( beats_per_minute tempo,
               const std::chrono::duration< Rep, Period >& duration ) noexcept
    {
        return duration * tempo;
    }

    constexpr chrono::minutes operator/( beat_duration beats, beats_per_minute tempo ) noexcept
    {
        return chrono::minutes{beats.to_double() / tempo.to_double()};
    }

    inline std::ostream& operator<<( std::ostream& os, beat_duration beats )
    {
        return os << beats.to_double() << " beats";
    }

    constexpr beat_duration operator"" _beats( long double value ) noexcept
    {
        return beat_duration( value );
    }

    constexpr beat_duration operator"" _beats( unsigned long long int value ) noexcept
    {
        return beat_duration( value );
    }

    static_assert( std::chrono::seconds( 30 ) * 120.0_bpm == beat_duration( 60.0 ) );
    static_assert( ( std::chrono::seconds( 30 ) * 120.0_bpm ) / 120.0_bpm ==
                   chrono::seconds( 30.0 ) );
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

        static constexpr sequencer::beat_duration max() noexcept
        {
            return sequencer::beat_duration{
                std::numeric_limits< sequencer::beat_duration::internal_rep >::max().to_double()};
        }
    };
} // namespace std
