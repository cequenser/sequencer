#pragma once

#include <sequencer/beat_time_point.hpp>
#include <sequencer/midi/message_type.hpp>

#include <cassert>

namespace sequencer::midi
{
    class clock_base
    {
    public:
        constexpr clock_base() = default;

        constexpr explicit clock_base( beat_time_point start_time )
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
                while ( last_update_ + tick < t + abs_accuracy_ )
                {
                    last_update_ += tick;
                    abs_accuracy_ += std::numeric_limits< beat_duration >::epsilon();
                    sender( midi::message_type::realtime_clock );
                }
            }
        }

        bool is_started() const noexcept
        {
            return started_;
        }

        void reset() noexcept
        {
            started_ = false;
            continue_ = false;
            last_update_ = start_time_;
            abs_accuracy_ = std::numeric_limits< beat_duration >::epsilon();
        }

        void start() noexcept
        {
            started_ = true;
        }

        void stop() noexcept
        {
            started_ = false;
        }

        void set_pulses_per_quarter_note( int pulses_per_quarter_note ) noexcept
        {
            assert( pulses_per_quarter_note > 0 );
            pulses_per_quarter_note_ = pulses_per_quarter_note;
        }

        int pulses_per_quarter_note() const noexcept
        {
            return pulses_per_quarter_note_;
        }

    private:
        beat_time_point last_update_;
        beat_time_point start_time_;
        beat_duration abs_accuracy_ = std::numeric_limits< beat_duration >::epsilon();
        int pulses_per_quarter_note_{24};
        bool started_ = false;
        bool running_ = false;
        bool continue_ = false;
    };

} // namespace sequencer::midi
