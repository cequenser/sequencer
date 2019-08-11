#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace sequencer::midi
{
    static constexpr auto invalid_string = "invalid";

    enum class message_type : std::uint8_t
    {
        invalid = 0,
        realtime_clock = 0xF8,
        realtime_start = 0xFA,
        realtime_continue = 0xFB,
        realtime_stop = 0XFC
    };

    constexpr std::string_view to_string( message_type type ) noexcept
    {
        switch ( type )
        {
        case message_type::realtime_clock:
            return "clock";
        case message_type::realtime_start:
            return "start";
        case message_type::realtime_continue:
            return "continue";
        case message_type::realtime_stop:
            return "stop";
        default:
            return invalid_string;
        }
    }

    inline std::ostream& operator<<( std::ostream& stream, message_type type )
    {
        stream << to_string( type );
        return stream;
    }

    namespace system::common
    {
        struct message_type
        {
            using size_type = std::vector< std::byte >::size_type;

            auto size() const noexcept
            {
                return message.size();
            }

            std::byte& operator[]( size_type i )
            {
                return message[ i ];
            }

            const std::byte& operator[]( size_type i ) const
            {
                return message[ i ];
            }

            std::vector< std::byte > message;
        };

        inline std::pair< std::byte, std::byte > uint16_to_two_byte_hex( std::uint16_t value )
        {
            std::byte first_byte{0x00};
            std::byte second_byte{0x00};
            if ( value < 128 )
            {
                second_byte = std::byte{static_cast< std::uint8_t >( value )};
            }
            else
            {
                first_byte = std::byte{static_cast< std::uint8_t >( value / 128 )};
                second_byte = std::byte{static_cast< std::uint8_t >( value % 128 )};
            }

            return {second_byte, first_byte}; // {lsb, msb}
        }

        inline std::uint16_t
        two_byte_hex_to_uint16( const std::pair< std::byte, std::byte >& two_bytes )
        {
            return static_cast< std::uint8_t >( two_bytes.second ) * std::uint16_t( 128 ) +
                   static_cast< std::uint8_t >( two_bytes.first );
        }

        inline message_type song_position_pointer( std::uint16_t position_in_16th_notes )
        {
            const auto hex_value = uint16_to_two_byte_hex( position_in_16th_notes );
            return message_type{{std::byte{0xF2}, hex_value.first, hex_value.second}};
        }

        inline std::string to_string( const message_type& msg )
        {
            switch ( msg.message.front() )
            {
            // song position pointer
            case std::byte{0xF2}:
                assert( msg.message.size() == 3 );
                return std::string( "spp@" ).append( std::to_string(
                    two_byte_hex_to_uint16( {msg.message[ 1 ], msg.message[ 2 ]} ) ) );
            default:
                return invalid_string;
            }
        }

        inline std::ostream& operator<<( std::ostream& stream, const message_type& msg )
        {
            stream << to_string( msg );
            return stream;
        }

    } // namespace system::common
} // namespace sequencer::midi
