#pragma once

#include <sequencer/assert.hpp>
#include <sequencer/midi/message/message_type.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace sequencer::midi
{
    static constexpr auto invalid_string = "invalid";
    static constexpr auto max_14bit = 16384;

    constexpr std::byte status_byte_for( std::byte status_byte, std::uint8_t channel ) noexcept
    {
        return status_byte | std::byte{channel};
    }

    constexpr bool check_status_byte( std::byte status_byte, std::byte message_byte )
    {
        return ( message_byte & status_byte ) == status_byte;
    }

    constexpr std::array< std::uint8_t, 3 >
    read_channel_with_two_7bit_values( const message_t< 3 >& message ) noexcept
    {
        return {static_cast< std::uint8_t >( message[ 0 ] & std::byte{0x0F} ),
                static_cast< std::uint8_t >( message[ 1 ] ),
                static_cast< std::uint8_t >( message[ 2 ] )};
    }

    constexpr std::pair< std::byte, std::byte > uint16_to_lsb_msb( std::uint16_t value ) noexcept
    {
        SEQUENCER_ASSERT( value < max_14bit );
        return value < 128
                   ? std::pair( std::byte{static_cast< std::uint8_t >( value )}, std::byte{0x00} )
                   : std::make_pair( std::byte{static_cast< std::uint8_t >( value % 128 )},
                                     std::byte{static_cast< std::uint8_t >( value / 128 )} );
    }

    constexpr std::uint16_t
    lsb_msb_to_uint16( const std::pair< std::byte, std::byte >& two_bytes ) noexcept
    {
        return static_cast< std::uint8_t >( two_bytes.second ) * std::uint16_t( 128 ) +
               static_cast< std::uint8_t >( two_bytes.first );
    }
} // namespace sequencer::midi
