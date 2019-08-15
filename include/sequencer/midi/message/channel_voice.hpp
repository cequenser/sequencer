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
} // namespace sequencer::midi::channel::voice
