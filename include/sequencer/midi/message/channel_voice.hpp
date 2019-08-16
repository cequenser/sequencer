#pragma once

#include <sequencer/midi/message/util.hpp>

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace sequencer::midi::channel::voice
{
    constexpr std::array< std::byte, 3 > note_on( std::uint8_t channel, std::uint8_t note,
                                                  std::uint8_t velocity ) noexcept
    {
        assert( channel < 16 );
        assert( note < 128 );
        assert( velocity < 128 );

        return {status_byte_for( std::byte{0x90}, channel ), std::byte{note}, std::byte{velocity}};
    }

    constexpr std::array< std::byte, 3 > note_off( std::uint8_t channel, std::uint8_t note,
                                                   std::uint8_t velocity ) noexcept
    {
        assert( channel < 16 );
        assert( note < 128 );
        assert( velocity < 128 );

        return {status_byte_for( std::byte{0x80}, channel ), std::byte{note}, std::byte{velocity}};
    }

    constexpr std::array< std::byte, 3 > polymorphic_key_pressure( std::uint8_t channel,
                                                                   std::uint8_t key,
                                                                   std::uint8_t pressure ) noexcept
    {
        assert( channel < 16 );
        assert( key < 128 );
        assert( pressure < 128 );

        return {status_byte_for( std::byte{0xA0}, channel ), std::byte{key}, std::byte{pressure}};
    }

    constexpr std::array< std::byte, 3 >
    control_change( std::uint8_t channel, std::uint8_t controller, std::uint8_t value ) noexcept
    {
        assert( channel < 16 );
        assert( controller < 120 );
        assert( value < 128 );

        return {status_byte_for( std::byte{0xB0}, channel ), std::byte{controller},
                std::byte{value}};
    }

    constexpr std::array< std::byte, 2 > program_change( std::uint8_t channel,
                                                         std::uint8_t program ) noexcept
    {
        assert( channel < 16 );
        assert( program < 128 );

        return {status_byte_for( std::byte{0xC0}, channel ), std::byte{program}};
    }

    constexpr std::array< std::byte, 2 > channel_pressure( std::uint8_t channel,
                                                           std::uint8_t pressure ) noexcept
    {
        assert( channel < 16 );
        assert( pressure < 128 );

        return {status_byte_for( std::byte{0xD0}, channel ), std::byte{pressure}};
    }

    constexpr std::array< std::byte, 3 > pitch_bend_change( std::uint8_t channel,
                                                            std::uint16_t value ) noexcept
    {
        assert( channel < 16 );
        assert( value < 16384 );

        const auto hex_value = uint16_to_two_bytes( value );
        return {status_byte_for( std::byte{0xE0}, channel ), hex_value.first, hex_value.second};
    }
} // namespace sequencer::midi::channel::voice
