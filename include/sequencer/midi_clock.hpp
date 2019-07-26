#pragma once

#include <sequencer/beat_time_point.hpp>

#include <cstddef>
#include <ostream>

#define MIDI_REAL_TIME_MESSAGE_TYPES                                                               \
    X( invalid, 0x00 )                                                                             \
    X( start, 0xFA )                                                                               \
    X( stop, 0xFC )                                                                                \
    X( cont, 0xFB )                                                                                \
    X( pulse, 0xF8 )

namespace sequencer
{
    class midi_clock
    {
    public:
#define X( message, midi_representation ) message,
        enum class message_type
        {
            MIDI_REAL_TIME_MESSAGE_TYPES
        };
#undef X

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
                sender( continue_ ? message_type::cont : message_type::start );
                running_ = true;
                continue_ = true;
            }
            else if ( running_ && !started_ )
            {
                sender( message_type::stop );
                running_ = false;
            }

            if ( running_ )
            {
                const auto tick = beat_duration( 1.0 / pulses_per_quarter_note_ );
                while ( last_update_ + tick < t )
                {
                    last_update_ += tick;
                    sender( midi_clock::message_type::pulse );
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

#define X( msg, midi_representation )                                                              \
    if ( message == midi_clock::message_type::msg )                                                \
        return os << #msg;

    inline std::ostream& operator<<( std::ostream& os, midi_clock::message_type message )
    {
        MIDI_REAL_TIME_MESSAGE_TYPES
        return os;
    }
#undef X

#define X( msg, midi_representation )                                                              \
    case midi_clock::message_type::msg:                                                            \
    {                                                                                              \
        return midi_representation;                                                                \
    }

    constexpr unsigned char midi_real_time_message( midi_clock::message_type message )
    {
        switch ( message )
        {
            MIDI_REAL_TIME_MESSAGE_TYPES
        }

        return midi_real_time_message( midi_clock::message_type::invalid );
    }
#undef X
} // namespace sequencer
