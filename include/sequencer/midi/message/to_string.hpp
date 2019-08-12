#pragma once

#include <sequencer/midi/message/channel_mode.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/system_common.hpp>

#include <cassert>
#include <ostream>
#include <string>

namespace sequencer::midi
{
    constexpr std::uint8_t get_channel( unsigned char status_byte ) noexcept
    {
        return status_byte & 0x0F;
    }

    inline std::string to_string( const message_type& message )
    {
        if ( message.empty() )
        {
            return invalid_string;
        }

        const auto status_byte = static_cast< unsigned char >( message.front() );
        // system common message
        if ( ( status_byte >> 4 ) == 0x0F )
        {
            switch ( status_byte )
            {
            // song position pointer
            case 0xF2:
                assert( message.size() == 3 );
                return std::string( "spp:" ).append( std::to_string(
                    system::common::two_bytes_to_uint16( {message[ 1 ], message[ 2 ]} ) ) );
            default:
                return invalid_string;
            }
        }

        using std::to_string;
        // control change message
        if ( ( status_byte >> 4 ) == 0x0B )
        {
            assert( message.size() == 3 );
            const auto control_byte = static_cast< unsigned char >( message[ 1 ] );
            switch ( control_byte )
            {
            // all sounds off
            case 0x78:
                return std::string( "all_sounds_off:" )
                    .append( to_string( get_channel( status_byte ) ) );
            // reset all controllers
            case 0x79:
                return std::string( "reset_all_controllers:" )
                    .append( to_string( get_channel( status_byte ) ) );
            // local control on/off
            case 0x7A:
                return std::string( "local_control_" )
                    .append( ( message[ 2 ] == std::byte{0x00} ) ? "off:" : "on:" )
                    .append( to_string( get_channel( status_byte ) ) );
            // all notes off
            case 0x7B:
                return std::string( "all_notes_off:" )
                    .append( to_string( get_channel( status_byte ) ) );
            // omni mode off
            case 0x7C:
                return std::string( "omni_mode_off:" )
                    .append( to_string( get_channel( status_byte ) ) );
            // omni mode on
            case 0x7D:
                return std::string( "omni_mode_on:" )
                    .append( to_string( get_channel( status_byte ) ) );
            // poly mode on
            case 0x7F:
                return std::string( "poly_mode_on:" )
                    .append( to_string( get_channel( status_byte ) ) );
            default:
                return invalid_string;
            }
        }

        switch ( ( status_byte >> 4 ) )
        {
        // note on
        case 0x9:
        {
            assert( message.size() == 3 );
            auto str = std::string( "note_on:" ) += to_string( get_channel( status_byte ) ) += ":";
            str += to_string( static_cast< std::uint8_t >( message[ 1 ] ) );
            str += ":";
            str += to_string( static_cast< std::uint8_t >( message[ 2 ] ) );
            return str;
        }
        // note off
        case 0x8:
        {
            assert( message.size() == 3 );
            auto str = std::string( "note_off:" ) += to_string( get_channel( status_byte ) ) += ":";
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
} // namespace sequencer::midi
