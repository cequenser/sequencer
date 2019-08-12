#pragma once

#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/util.hpp>

#include <cstddef>
#include <cstdint>

namespace sequencer::midi::system::common
{
    constexpr std::pair< std::byte, std::byte > uint16_to_two_bytes( std::uint16_t value )
    {
        if ( value < 128 )
        {
            return {std::byte{static_cast< std::uint8_t >( value )}, std::byte{0x00}};
        }

        return {std::byte{static_cast< std::uint8_t >( value % 128 )},
                std::byte{static_cast< std::uint8_t >( value / 128 )}};
    }

    constexpr std::uint16_t
    two_bytes_to_uint16( const std::pair< std::byte, std::byte >& two_bytes )
    {
        return static_cast< std::uint8_t >( two_bytes.second ) * std::uint16_t( 128 ) +
               static_cast< std::uint8_t >( two_bytes.first );
    }

    inline message_type song_position_pointer( std::uint16_t position_in_16th_notes )
    {
        const auto hex_value = uint16_to_two_bytes( position_in_16th_notes );
        return message_type{{std::byte{0xF2}, hex_value.first, hex_value.second}};
    }
} // namespace sequencer::midi::system::common
