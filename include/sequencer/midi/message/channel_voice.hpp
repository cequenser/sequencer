#pragma once

#include <sequencer/midi/message/message_type.hpp>

#include <cassert>
#include <cstdint>

namespace sequencer::midi::channel::voice
{
    inline message_type note_on( std::uint8_t channel, std::uint8_t note, std::uint8_t velocity )
    {
        assert( channel < 16 );
        assert( note < 128 );
        assert( velocity < 128 );

        return make_message( status_byte_for( std::byte{0x90}, channel ), std::byte{note},
                             std::byte{velocity} );
    }

    inline message_type note_off( std::uint8_t channel, std::uint8_t note, std::uint8_t velocity )
    {
        assert( channel < 16 );
        assert( note < 128 );
        assert( velocity < 128 );

        return make_message( status_byte_for( std::byte{0x80}, channel ), std::byte{note},
                             std::byte{velocity} );
    }
} // namespace sequencer::midi::channel::voice
