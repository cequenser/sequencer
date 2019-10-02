#pragma once

#include <cassert>
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

    constexpr note_t operator+( note_t note, std::int16_t offset )
    {
        assert( to_uint8_t( note ) + offset < 128 );
        return note_t{std::uint8_t( to_uint8_t( note ) + offset )};
    }

    constexpr std::int16_t get_note_distance( note_t lhs, note_t rhs ) noexcept
    {
        return to_uint8_t( rhs ) - to_uint8_t( lhs );
    }
} // namespace sequencer::midi
