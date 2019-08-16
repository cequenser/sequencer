#pragma once

#include <sequencer/midi/message/channel_mode.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/control_change.hpp>
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

    inline std::string on_off_message_to_string( std::string name, unsigned char status_byte,
                                                 std::byte data )
    {
        using std::to_string;
        return name.append( "_" )
            .append( ( static_cast< std::uint8_t >( data ) < 64u ) ? "off:" : "on:" )
            .append( to_string( get_channel( status_byte ) ) );
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
                return std::string( "spp:" ).append(
                    std::to_string( two_bytes_to_uint16( {message[ 1 ], message[ 2 ]} ) ) );
            // song select
            case 0xF3:
                assert( message.size() == 2 );
                return std::string( "song_select:" )
                    .append( std::to_string( static_cast< int >( message.back() ) ) );
            // tune request
            case 0xF6:
                assert( message.size() == 1 );
                return "tune_request";
            // end system exclusive
            case 0xF7:
                assert( message.size() == 1 );
                return "end_sysex";
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
            // damper pedal on/off
            case 0x40:
                return on_off_message_to_string( "damper_pedal", status_byte, message[ 2 ] );
            // portamento on/off
            case 0x41:
                return on_off_message_to_string( "portamento", status_byte, message[ 2 ] );
            // sostenuto on/off
            case 0x42:
                return on_off_message_to_string( "sostenuto", status_byte, message[ 2 ] );
            // soft pedal on/off
            case 0x43:
                return on_off_message_to_string( "soft_pedal", status_byte, message[ 2 ] );
            // hold 2 on/off
            case 0x45:
                return on_off_message_to_string( "hold_2", status_byte, message[ 2 ] );
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
                return on_off_message_to_string( "local_control", status_byte, message[ 2 ] );
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
        // pitch bend change
        case 0xE:
            assert( message.size() == 3 );
            return std::string( "pitch_bend_change:" )
                .append( to_string( get_channel( status_byte ) ) )
                .append( ":" )
                .append( std::to_string( two_bytes_to_uint16( {message[ 1 ], message[ 2 ]} ) ) );

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
