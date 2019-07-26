#pragma once

#include <sequencer/beat_time_point.hpp>

#include <ostream>
#include <string_view>

namespace sequencer
{
    namespace midi
    {
        enum class message_type : std::uint8_t
        {
            invalid = 0,
            realtime_clock = 0xF8,
            realtime_start = 0xFA,
            realtime_continue = 0xFB,
            realtime_stop = 0XFC
        };

        inline constexpr std::string_view to_string( message_type type ) noexcept
        {
            switch ( type )
            {
            case message_type::realtime_clock:
                return "clock";
            case message_type::realtime_start:
                return "start";
            case message_type::realtime_continue:
                return "continue";
            case message_type::realtime_stop:
                return "stop";
            default:
                return "invalid";
            }
        }

        inline std::ostream& operator<<( std::ostream& stream, message_type type )
        {
            stream << to_string( type );
            return stream;
        }

    } // namespace midi

    class midi_clock
    {
    public:
        constexpr midi_clock() = default;

        constexpr explicit midi_clock( beat_time_point start_time )
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

        bool is_started() const noexcept
        {
            return started_;
        }

        void reset() noexcept
        {
            started_ = false;
            continue_ = false;
            last_update_ = start_time_;
        }

        void start() noexcept
        {
            started_ = true;
        }

        void stop() noexcept
        {
            started_ = false;
        }

    private:
        bool started_ = false;
        bool running_ = false;
        bool continue_ = false;
        beat_time_point last_update_;
        beat_time_point start_time_;
        double pulses_per_quarter_note_{24.0};
    };

} // namespace sequencer
