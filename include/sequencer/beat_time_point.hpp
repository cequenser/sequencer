#pragma once

#include <sequencer/beat_duration.hpp>

namespace sequencer
{

    class beat_time_point
    {
    public:
        constexpr beat_time_point() = default;

        constexpr explicit beat_time_point( beat_duration time_since_epoch )
            : time_since_epoch_( time_since_epoch )
        {
        }

        constexpr beat_duration time_since_epoch() const noexcept
        {
            return time_since_epoch_;
        }

        constexpr beat_time_point& operator+=( beat_duration other ) noexcept
        {
            time_since_epoch_ += other;
            return *this;
        }

    private:
        beat_duration time_since_epoch_ = 0.0_beats;
    };

    constexpr beat_time_point operator+( beat_time_point lhs, beat_duration rhs ) noexcept
    {
        return lhs += rhs;
    }

    constexpr bool operator<( beat_time_point lhs, beat_time_point rhs ) noexcept
    {
        return lhs.time_since_epoch() < rhs.time_since_epoch();
    }

    inline std::ostream& operator<<( std::ostream& os, beat_time_point time_point )
    {
        return os << time_point.time_since_epoch() << " since epoch";
    }

} // namespace sequencer
