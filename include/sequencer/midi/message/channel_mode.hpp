#pragma once

#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/util.hpp>

#include <cassert>

namespace sequencer::midi::channel::mode
{
    inline message_type make_message( std::byte status, std::byte control_function,
                                      std::byte data = std::byte{0x00} )
    {
        return message_type{{status, control_function, data}};
    }

    inline message_type all_sounds_off( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte_for( std::byte{0xB0}, channel ), std::byte{0x78} );
    }

    inline message_type reset_all_controllers( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte_for( std::byte{0xB0}, channel ), std::byte{0x79} );
    }

} // namespace sequencer::midi::channel::mode
