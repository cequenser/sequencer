#pragma once

#include <cstdint>

namespace sequencer::midi
{
    enum class note_t : std::uint8_t
    {
        no_note = 0
    };

    constexpr note_t no_note() noexcept
    {
        return note_t::no_note;
    }

    constexpr std::uint8_t to_uint8_t( note_t note ) noexcept
    {
        return static_cast< std::uint8_t >( note );
    }
} // namespace sequencer::midi
