#pragma once

#include <sequencer/beat_time_point.hpp>
#include <sequencer/midi/message_type.hpp>

namespace sequencer::midi
{
    class clock
    {
    public:
        constexpr clock() = default;

        constexpr explicit clock( beat_time_point start_time )
            : last_update_( start_time ), start_time_( start_time )
        {
        }

        template < typename Sender >
        void update( beat_time_point t, Sender sender )
        {
            if ( !running_ && started_ )
            {
                sender( continue_ ? midi::message_type::realtime_continue
                                  : midi::message_type::realtime_start );
                running_ = true;
                continue_ = true;
            }
            else if ( running_ && !started_ )
            {
                sender( midi::message_type::realtime_stop );
                running_ = false;
            }

            if ( running_ )
            {
                const auto tick = beat_duration( 1.0 / pulses_per_quarter_note_ );
                while ( last_update_ + tick < t )
                {
                    last_update_ += tick;
                    sender( midi::message_type::realtime_clock );
                }
            }
        }

        constexpr bool is_started() const noexcept
        {
            return started_;
        }

        void reset() noexcept
        {
            started_ = false;
            continue_ = false;
            last_update_ = start_time_;
        }

        constexpr void start() noexcept
        {
            started_ = true;
        }

        constexpr void stop() noexcept
        {
            started_ = false;
        }

    private:
        beat_time_point last_update_;
        beat_time_point start_time_;
        double pulses_per_quarter_note_{24.0};
        bool started_ = false;
        bool running_ = false;
        bool continue_ = false;
    };

} // namespace sequencer::midi
