#pragma once

#include <cstdint>
#include <limits>

namespace sequencer::midi
{
    enum class note_t : std::uint8_t
    {
        no_note = std::numeric_limits< std::uint8_t >::max()
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
