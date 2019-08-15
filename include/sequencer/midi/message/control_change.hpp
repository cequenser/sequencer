#pragma once

#include <sequencer/midi/message/util.hpp>

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace sequencer::midi::control_change
{
    constexpr std::byte status_byte( std::uint8_t channel ) noexcept
    {
        return status_byte_for( std::byte{0xB0}, channel );
    }

    constexpr std::byte on_off_byte( bool on ) noexcept
    {
        return on ? std::byte{0x7F} : std::byte{0x00};
    }

    constexpr std::array< std::byte, 3 > damper_pedal( std::uint8_t channel, bool on ) noexcept
    {
        return {status_byte( channel ), std::byte{0x40}, on_off_byte( on )};
    }

    constexpr std::array< std::byte, 3 > portamento( std::uint8_t channel, bool on ) noexcept
    {
        return {status_byte( channel ), std::byte{0x41}, on_off_byte( on )};
    }

    constexpr std::array< std::byte, 3 > sostenuto( std::uint8_t channel, bool on ) noexcept
    {
        return {status_byte( channel ), std::byte{0x42}, on_off_byte( on )};
    }

    constexpr std::array< std::byte, 3 > soft_pedal( std::uint8_t channel, bool on ) noexcept
    {
        return {status_byte( channel ), std::byte{0x43}, on_off_byte( on )};
    }

    constexpr std::array< std::byte, 3 > hold_2( std::uint8_t channel, bool on ) noexcept
    {
        return {status_byte( channel ), std::byte{0x45}, on_off_byte( on )};
    }
} // namespace sequencer::midi::control_change
