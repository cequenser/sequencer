#pragma once

#include <sequencer/midi/message/invalid_string.hpp>

#include <cstdint>
#include <ostream>
#include <string_view>

namespace sequencer::midi::real_time
{
    enum class message_type : std::uint8_t
    {
        invalid = 0,
        realtime_clock = 0xF8,
        realtime_start = 0xFA,
        realtime_continue = 0xFB,
        realtime_stop = 0XFC
    };

    constexpr std::string_view to_string( message_type type ) noexcept
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
            return invalid_string;
        }
    }

    inline std::ostream& operator<<( std::ostream& stream, message_type type )
    {
        stream << to_string( type );
        return stream;
    }
} // namespace sequencer::midi::real_time
