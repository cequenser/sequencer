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

    inline message_type damper_pedal( std::uint8_t channel, bool on )
    {
        const auto data_byte = on ? std::byte{0x7F} : std::byte{0x00};
        return make_message( status_byte( channel ), std::byte{0x40}, data_byte );
    }
} // namespace sequencer::midi::control_change
