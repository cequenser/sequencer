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
    class float_type
    {
    public:
        using rep = double;
        using internal_rep = std::int64_t;
        static constexpr rep eps = 1.0 / ticks_per_unit;

        constexpr explicit float_type( rep value = 0 ) noexcept
            : count_( constexpr_round( value / eps ) + std::numeric_limits< rep >::epsilon() )
        {
        }

        constexpr double to_double() const noexcept
        {
            return count_ * eps;
        }

        constexpr float_type& operator+=( float_type other ) noexcept
        {
            count_ += other.count_;
            return *this;
        }

        constexpr float_type operator-() const noexcept
        {
            return float_type{-to_double()};
        }

        constexpr bool operator==( float_type other ) const noexcept
        {
            return count_ == other.count_;
        }

        constexpr bool operator!=( float_type other ) const noexcept
        {
            return !( *this == other );
        }

        constexpr bool operator<( float_type other ) const noexcept
        {
            return count_ < other.count_;
        }

        constexpr bool operator<=( float_type other ) const noexcept
        {
            return count_ <= other.count_;
        }

        static constexpr float_type from_count( internal_rep count ) noexcept
        {
            return float_type( count * eps );
        }

        static constexpr float_type max() noexcept
        {
            return from_count( std::numeric_limits< internal_rep >::max() - ticks_per_unit );
        }

    private:
        internal_rep count_;
    };

    template < int ticks_per_unit >
    constexpr float_type< ticks_per_unit > operator+( float_type< ticks_per_unit > lhs,
                                                      float_type< ticks_per_unit > rhs )
    {
        return lhs += rhs;
    }

    template < int ticks_per_unit >
    std::ostream& operator<<( std::ostream& os, float_type< ticks_per_unit > value )
    {
        return os << value.to_double();
    }
} // namespace sequencer

namespace std
{
    template < int ticks_per_unit >
    class numeric_limits< sequencer::float_type< ticks_per_unit > >
    {
    public:
        static constexpr sequencer::float_type< ticks_per_unit > max() noexcept
        {
            return sequencer::float_type< ticks_per_unit >::max();
        }
    };
} // namespace std
