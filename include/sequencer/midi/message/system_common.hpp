#pragma once

#include <sequencer/midi/message/invalid_string.hpp>
#include <sequencer/midi/message/message_type.hpp>

#include <cassert>
#include <cstddef>
#include <ostream>
#include <string>

namespace sequencer::midi::message::system::common
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

    inline std::string to_string( const message_type& message )
    {
        if ( message.empty() )
        {
            return invalid_string;
        }

        switch ( static_cast< unsigned char >( message.front() ) )
        {
        // song position pointer
        case 0xF2:
            assert( message.size() == 3 );
            return std::string( "spp@" ).append(
                std::to_string( two_bytes_to_uint16( {message[ 1 ], message[ 2 ]} ) ) );
        default:
            return invalid_string;
        }
    }

    inline std::ostream& operator<<( std::ostream& stream, const message_type& msg )
    {
        stream << to_string( msg );
        return stream;
    }
} // namespace sequencer::midi::message::system::common
