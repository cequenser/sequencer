#pragma once

#include <sequencer/midi/message/util.hpp>

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace sequencer::midi::system::common
{
    constexpr std::array< std::byte, 3 >
    song_position_pointer( std::uint16_t position_in_16th_notes ) noexcept
    {
        const auto hex_value = uint16_to_two_bytes( position_in_16th_notes );
        return {std::byte{0xF2}, hex_value.first, hex_value.second};
    }

    constexpr std::array< std::byte, 2 > song_select( std::uint8_t song_number ) noexcept
    {
        assert( song_number < 128 );
        return {std::byte{0xF3}, std::byte{song_number}};
    }

    constexpr std::array< std::byte, 1 > tune_request() noexcept
    {
        return {std::byte{0xF6}};
    }

    constexpr std::array< std::byte, 1 > end_system_exclusive() noexcept
    {
        return {std::byte{0xF7}};
    }
} // namespace sequencer::midi::system::common
