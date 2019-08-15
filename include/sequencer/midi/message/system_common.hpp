#pragma once

#include <sequencer/midi/message/util.hpp>

#include <array>
#include <cstddef>
#include <cstdint>

namespace sequencer::midi::system::common
{
    constexpr std::pair< std::byte, std::byte > uint16_to_two_bytes( std::uint16_t value )
    {
        return value < 128
                   ? std::pair( std::byte{static_cast< std::uint8_t >( value )}, std::byte{0x00} )
                   : std::make_pair( std::byte{static_cast< std::uint8_t >( value % 128 )},
                                     std::byte{static_cast< std::uint8_t >( value / 128 )} );
    }

    constexpr std::uint16_t
    two_bytes_to_uint16( const std::pair< std::byte, std::byte >& two_bytes )
    {
        return static_cast< std::uint8_t >( two_bytes.second ) * std::uint16_t( 128 ) +
               static_cast< std::uint8_t >( two_bytes.first );
    }

    constexpr std::array< std::byte, 3 >
    song_position_pointer( std::uint16_t position_in_16th_notes ) noexcept
    {
        const auto hex_value = uint16_to_two_bytes( position_in_16th_notes );
        return {std::byte{0xF2}, hex_value.first, hex_value.second};
    }
} // namespace sequencer::midi::system::common
