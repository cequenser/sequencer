#pragma once

#include <sequencer/midi/message/control_change.hpp>

#include <array>
#include <cassert>
#include <cstddef>

namespace sequencer::midi::channel::mode
{
    using control_change::status_byte;

    constexpr std::array< std::byte, 3 > make_message( std::byte status, std::byte control_function,
                                                       std::byte data = std::byte{0x00} )
    {
        return {status, control_function, data};
    }

    constexpr std::array< std::byte, 3 > all_sounds_off( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x78} );
    }

    constexpr std::array< std::byte, 3 > reset_all_controllers( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x79} );
    }

    constexpr std::array< std::byte, 3 > local_control( std::uint8_t channel, bool on )
    {
        assert( channel < 16 );
        const auto data_byte = on ? std::byte{0x7F} : std::byte{0x00};
        return make_message( status_byte( channel ), std::byte{0x7A}, data_byte );
    }

    constexpr std::array< std::byte, 3 > all_notes_off( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x7B} );
    }

    constexpr std::array< std::byte, 3 > omni_mode_off( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x7C} );
    }

    constexpr std::array< std::byte, 3 > omni_mode_on( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x7D} );
    }

    constexpr std::array< std::byte, 3 > poly_mode_on( std::uint8_t channel )
    {
        assert( channel < 16 );
        return make_message( status_byte( channel ), std::byte{0x7F} );
    }
} // namespace sequencer::midi::channel::mode
