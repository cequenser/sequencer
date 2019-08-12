#pragma once

#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/system_common.hpp>

#include <cassert>
#include <ostream>
#include <string>

namespace sequencer::midi::message
{
    inline std::string to_string( const message_type& message )
    {
        if ( message.empty() )
        {
            return invalid_string;
        }

        const auto status_byte = static_cast< unsigned char >( message.front() );
        if ( ( status_byte >> 4 ) == 0x0F )
        {
            switch ( status_byte )
            {
            // song position pointer
            case 0xF2:
                assert( message.size() == 3 );
                return std::string( "spp@" ).append( std::to_string(
                    system::common::two_bytes_to_uint16( {message[ 1 ], message[ 2 ]} ) ) );
            default:
                return invalid_string;
            }
        }

        using std::to_string;
        switch ( ( status_byte >> 4 ) )
        {
        case 0x9:
        {
            assert( message.size() == 3 );
            auto str = std::string( "note_on:" ) += to_string( status_byte & 0x0F ) += ":";
            str += to_string( static_cast< std::uint8_t >( message[ 1 ] ) );
            str += ":";
            str += to_string( static_cast< std::uint8_t >( message[ 2 ] ) );
            return str;
        }
        case 0x8:
        {
            assert( message.size() == 3 );
            auto str = std::string( "note_off:" ) += to_string( status_byte & 0x0F ) += ":";
            str += to_string( static_cast< std::uint8_t >( message[ 1 ] ) );
            str += ":";
            str += to_string( static_cast< std::uint8_t >( message[ 2 ] ) );
            return str;
        }
        default:
            return invalid_string;
        }
    }

    inline std::ostream& operator<<( std::ostream& stream, const message_type& msg )
    {
        stream << to_string( msg );
        return stream;
    }
} // namespace sequencer::midi::message
