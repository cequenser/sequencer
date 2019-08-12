#pragma once

#include <sequencer/midi/message/message_type.hpp>

#include <cstddef>
#include <cstdint>

namespace sequencer::midi::control_change
{
    constexpr std::byte status_byte( std::uint8_t channel )
    {
        return status_byte_for( std::byte{0xB0}, channel );
    }

    constexpr std::byte on_off_byte( bool on )
    {
        return on ? std::byte{0x7F} : std::byte{0x00};
    }

    inline message_type damper_pedal( std::uint8_t channel, bool on )
    {
        return make_message( status_byte( channel ), std::byte{0x40}, on_off_byte( on ) );
    }

    inline message_type portamento( std::uint8_t channel, bool on )
    {
        return make_message( status_byte( channel ), std::byte{0x41}, on_off_byte( on ) );
    }

    inline message_type sostenuto( std::uint8_t channel, bool on )
    {
        return make_message( status_byte( channel ), std::byte{0x42}, on_off_byte( on ) );
    }

    inline message_type soft_pedal( std::uint8_t channel, bool on )
    {
        return make_message( status_byte( channel ), std::byte{0x43}, on_off_byte( on ) );
    }

    inline message_type hold_2( std::uint8_t channel, bool on )
    {
        return make_message( status_byte( channel ), std::byte{0x45}, on_off_byte( on ) );
    }
} // namespace sequencer::midi::control_change
