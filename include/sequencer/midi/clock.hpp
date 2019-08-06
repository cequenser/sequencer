#pragma once

#include <sequencer/beat_time_point.hpp>
#include <sequencer/midi/message_type.hpp>

#include <cassert>

namespace sequencer::midi
{
    class clock
    {
    public:
        constexpr clock() = default;

        constexpr explicit clock( beat_time_point start_time ) noexcept
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
                const auto dt = tick();
                while ( last_update_ + dt <= t )
                {
                    last_update_ += dt;
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

        constexpr void set_pulses_per_quarter_note( int pulses_per_quarter_note ) noexcept
        {
            assert( pulses_per_quarter_note > 0 );
            pulses_per_quarter_note_ = pulses_per_quarter_note;
        }

        constexpr int pulses_per_quarter_note() const noexcept
        {
            return pulses_per_quarter_note_;
        }

    private:
        constexpr beat_duration tick() const noexcept
        {
            return beat_duration( 1.0 / pulses_per_quarter_note() );
        }

        beat_time_point last_update_;
        beat_time_point start_time_;
        int pulses_per_quarter_note_{24};
        bool started_ = false;
        bool running_ = false;
        bool continue_ = false;
    };

} // namespace sequencer::midi
