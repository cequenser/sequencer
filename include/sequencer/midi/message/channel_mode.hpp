#pragma once

#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/util.hpp>

#include <cassert>

namespace sequencer::midi::channel::mode
{
    inline message_type all_sounds_off( std::uint8_t channel )
    {
        assert( channel < 16 );
        return message_type{
            {status_byte_for( std::byte{0xB0}, channel ), std::byte{0x78}, std::byte{0x00}}};
    }
} // namespace sequencer::midi::channel::mode
