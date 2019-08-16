#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace sequencer::midi
{
    static constexpr auto invalid_string = "invalid";

    constexpr std::byte status_byte_for( std::byte status_byte, std::uint8_t channel ) noexcept
    {
        return status_byte | std::byte{channel};
    }

    constexpr std::pair< std::byte, std::byte > uint16_to_two_bytes( std::uint16_t value ) noexcept
    {
        assert( value < 16384 );
        return value < 128
                   ? std::pair( std::byte{static_cast< std::uint8_t >( value )}, std::byte{0x00} )
                   : std::make_pair( std::byte{static_cast< std::uint8_t >( value % 128 )},
                                     std::byte{static_cast< std::uint8_t >( value / 128 )} );
    }

    constexpr std::uint16_t
    two_bytes_to_uint16( const std::pair< std::byte, std::byte >& two_bytes ) noexcept
    {
        return static_cast< std::uint8_t >( two_bytes.second ) * std::uint16_t( 128 ) +
               static_cast< std::uint8_t >( two_bytes.first );
    }
} // namespace sequencer::midi
