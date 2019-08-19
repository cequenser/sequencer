#pragma once

#include <sequencer/chrono/chrono_adapter.hpp>

#include <chrono>
#include <type_traits>

namespace sequencer::chrono
{
    template < typename UnderlyingClock = clock_object_adapter< std::chrono::steady_clock > >
    class sequencer_clock
    {
    public:
        using underlying_clock_type = std::decay_t< UnderlyingClock >;
        using underlying_time_point = typename underlying_clock_type::time_point;
        using rep = typename underlying_clock_type::rep;
        using period = typename underlying_clock_type::period;
        using duration = std::chrono::duration< rep, period >;
        using time_point = std::chrono::time_point< sequencer_clock >;

        static_assert( underlying_clock_type::is_steady, "Underlying clock must be steady" );

        constexpr static bool is_steady = false;

        explicit sequencer_clock( const underlying_clock_type& underlying_clock ) noexcept
            : underlying_clock_( underlying_clock )
        {
        }

        bool is_running() const noexcept
        {
            return is_running_;
        }

        time_point now() const noexcept
        {
            if ( is_running_ )
            {
                return time_point{elapsed_ + ( underlying_clock_.now() - start_time_ )};
            }
            return time_point{elapsed_};
        }

        void reset() noexcept
        {
            elapsed_ = duration::zero();
            is_running_ = false;
        }

        void start() noexcept
        {
            if ( !is_running_ )
            {
                start_time_ = underlying_clock_.now();
                is_running_ = true;
            }
        }

        void stop() noexcept
        {
            if ( is_running_ )
            {
                elapsed_ += underlying_clock_.now() - start_time_;
                is_running_ = false;
            }
        }

    private:
        UnderlyingClock underlying_clock_;
        bool is_running_ = false;
        underlying_time_point start_time_ = underlying_time_point{duration::zero()};
        duration elapsed_ = duration::zero();
    };

} // namespace sequencer::chrono
