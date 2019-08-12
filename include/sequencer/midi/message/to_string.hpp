#pragma once

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

        switch ( static_cast< unsigned char >( message.front() ) )
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

    inline std::ostream& operator<<( std::ostream& stream, const message_type& msg )
    {
        stream << to_string( msg );
        return stream;
    }
} // namespace sequencer::midi::message
