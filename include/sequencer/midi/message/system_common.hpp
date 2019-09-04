#pragma once

#include <sequencer/assert.hpp>
#include <sequencer/midi/message/byte.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/util.hpp>

#include <cstdint>

namespace sequencer::midi::system::common
{
    constexpr message_t< 3 > song_position_pointer( std::uint16_t position_in_16th_notes ) noexcept
    {
        const auto hex_value = uint16_to_lsb_msb( position_in_16th_notes );
        return {byte::song_position_pointer, hex_value.first, hex_value.second};
    }

    constexpr message_t< 2 > song_select( std::uint8_t song_number ) noexcept
    {
        SEQUENCER_ASSERT( song_number < 128 );
        return {byte::song_select, std::byte{song_number}};
    }

    constexpr message_t< 1 > tune_request() noexcept
    {
        return {byte::tune_request};
    }
} // namespace sequencer::midi::system::common
