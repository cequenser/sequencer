#pragma once

#include <cstdint>
#include <limits>
#include <ostream>

namespace sequencer
{
    template < class T >
    constexpr std::int64_t constexpr_round( T a ) noexcept
    {
        return ( a > 0 ) ? std::int64_t( a + T{0.5} ) : std::int64_t( a - T{0.5} );
    }

    template < int ticks_per_unit >
    class fixed_point_type
    {
    public:
        using rep = double;
        using internal_rep = std::int64_t;
        static constexpr rep eps = 1.0 / ticks_per_unit;

        constexpr explicit fixed_point_type( rep value = 0 ) noexcept
            : count_( constexpr_round( value / eps ) + std::numeric_limits< rep >::epsilon() )
        {
        }

        constexpr double to_double() const noexcept
        {
            return count_ * eps;
        }

        constexpr fixed_point_type& operator+=( fixed_point_type other ) noexcept
        {
            count_ += other.count_;
            return *this;
        }

        constexpr fixed_point_type operator-() const noexcept
        {
            return fixed_point_type{-to_double()};
        }

        constexpr bool operator==( fixed_point_type other ) const noexcept
        {
            return count_ == other.count_;
        }

        constexpr bool operator!=( fixed_point_type other ) const noexcept
        {
            return !( *this == other );
        }

        constexpr bool operator<( fixed_point_type other ) const noexcept
        {
            return count_ < other.count_;
        }

        constexpr bool operator<=( fixed_point_type other ) const noexcept
        {
            return count_ <= other.count_;
        }

        static constexpr fixed_point_type from_count( internal_rep count ) noexcept
        {
            return fixed_point_type( count * eps );
        }

        static constexpr fixed_point_type max() noexcept
        {
            return from_count( std::numeric_limits< internal_rep >::max() - ticks_per_unit );
        }

    private:
        internal_rep count_;
    };

    template < int ticks_per_unit >
    constexpr fixed_point_type< ticks_per_unit > operator+( fixed_point_type< ticks_per_unit > lhs,
                                                            fixed_point_type< ticks_per_unit > rhs )
    {
        return lhs += rhs;
    }

    template < int ticks_per_unit >
    std::ostream& operator<<( std::ostream& os, fixed_point_type< ticks_per_unit > value )
    {
        return os << value.to_double();
    }
} // namespace sequencer

namespace std
{
    template < int ticks_per_unit >
    class numeric_limits< sequencer::fixed_point_type< ticks_per_unit > >
    {
    public:
        static constexpr sequencer::fixed_point_type< ticks_per_unit > max() noexcept
        {
            return sequencer::fixed_point_type< ticks_per_unit >::max();
        }
    };
} // namespace std
