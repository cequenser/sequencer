#pragma once

#include <sequencer/beat_time_point.hpp>
#include <sequencer/midi/message/realtime.hpp>

namespace sequencer::midi
{
    class clock_base
    {
    public:
        static constexpr int initial_pulses_per_quarter_note = 24;

        constexpr clock_base() = default;

        constexpr explicit clock_base(
            beat_time_point start_time,
            int pulses_per_quarter_note = initial_pulses_per_quarter_note ) noexcept
            : last_update_( start_time ), start_time_( start_time ),
              pulses_per_quarter_note_( pulses_per_quarter_note )
        {
        }

        template < typename Sender >
        void update( beat_time_point t, Sender sender )
        {
            using namespace realtime;

            if ( !running_ && started_ )
            {
                sender( continue_ ? realtime_continue() : realtime_start() );
                running_ = true;
                continue_ = true;
            }
            else if ( running_ && !started_ )
            {
                sender( realtime_stop() );
                running_ = false;
            }

            if ( running_ )
            {
                const auto dt = tick();
                while ( last_update_ + dt <= t )
                {
                    last_update_ += dt;
                    sender( realtime_clock() );
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

        constexpr int pulses_per_quarter_note() const noexcept
        {
            return pulses_per_quarter_note_;
        }

        constexpr beat_duration tick() const noexcept
        {
            return beat_duration( 1.0 / pulses_per_quarter_note() );
        }

    private:
        beat_time_point last_update_;
        beat_time_point start_time_;
        int pulses_per_quarter_note_{initial_pulses_per_quarter_note};
        bool started_ = false;
        bool running_ = false;
        bool continue_ = false;
    };

} // namespace sequencer::midi
