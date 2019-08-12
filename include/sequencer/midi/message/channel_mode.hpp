#pragma once

#include <sequencer/midi/message/control_change.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/util.hpp>

#include <cassert>

namespace sequencer::midi::channel::mode
{
    using control_change::status_byte;

    inline message_type make_message( std::byte status, std::byte control_function,
                                      std::byte data = std::byte{0x00} )
    {
        return sequencer::midi::make_message( status, control_function, data );
    }

    inline message_type all_sounds_off( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x78} );
    }

    inline message_type reset_all_controllers( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x79} );
    }

    inline message_type local_control( std::uint8_t channel, bool on )
    {
        assert( channel < 16 );
        const auto data_byte = on ? std::byte{0x7F} : std::byte{0x00};
        return make_message( status_byte( channel ), std::byte{0x7A}, data_byte );
    }

    inline message_type all_notes_off( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x7B} );
    }

    inline message_type omni_mode_off( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x7C} );
    }

    inline message_type omni_mode_on( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x7D} );
    }

    inline message_type poly_mode_on( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x7F} );
    }
} // namespace sequencer::midi::channel::mode
