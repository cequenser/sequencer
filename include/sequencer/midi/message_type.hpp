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
        class message_type
        {
        public:
            using size_type = std::vector< std::byte >::size_type;

            explicit message_type( const std::vector< std::byte >& message ) : message_( message )
            {
            }

            explicit message_type( std::vector< std::byte >&& message )
                : message_( std::move( message ) )
            {
            }

            const std::byte& front() const
            {
                return message_.front();
            }

            auto size() const noexcept
            {
                return message_.size();
            }

            std::byte& operator[]( size_type i )
            {
                return message_[ i ];
            }

            const std::byte& operator[]( size_type i ) const
            {
                return message_[ i ];
            }

            auto begin() const
            {
                return message_.begin();
            }

            auto end() const
            {
                return message_.end();
            }

            const std::vector< std::byte >& message() const noexcept
            {
                return message_;
            }

            const unsigned char* data() const
            {
                return static_cast< const unsigned char* >(
                    static_cast< const void* >( message_.data() ) );
            }

        private:
            std::vector< std::byte > message_;
        };

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

    } // namespace system::common
} // namespace sequencer::midi
