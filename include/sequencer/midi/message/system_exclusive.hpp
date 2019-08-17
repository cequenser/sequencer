#pragma once

#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/util.hpp>

#include <cstdint>

namespace sequencer::midi::system::exclusive
{
    static constexpr auto start_byte = std::byte{0xF0};
    static constexpr auto end_byte = std::byte{0xF7};

    constexpr message_t< 1 > end_system_exclusive() noexcept
    {
        return {end_byte};
    }

    template < class... Data >
    constexpr message_t< sizeof...( Data ) + std::size_t{3} >
    system_exclusive( std::byte manufacturer_id, Data&&... data ) noexcept
    {
        return {start_byte, manufacturer_id, std::byte( std::forward< Data >( data ) )...,
                end_byte};
    }

    template < class... Data >
    constexpr message_t< sizeof...( Data ) + std::size_t{3} >
    system_exclusive( std::uint8_t manufacturer_id, Data&&... data ) noexcept
    {
        return system_exclusive( std::byte{manufacturer_id}, std::forward< Data >( data )... );
    }

    namespace realtime
    {
        static constexpr auto id_byte = std::byte{0x7F};
        static constexpr auto device_control_byte = std::byte{0x04};
        static constexpr auto master_volume_byte = std::byte{0x01};

        template < class... Data >
        constexpr message_t< sizeof...( Data ) + std::size_t{4} >
        system_exclusive( std::uint8_t manufacturer_id, Data&&... data ) noexcept
        {
            return exclusive::system_exclusive( id_byte, manufacturer_id,
                                                std::forward< Data >( data )... );
        }

        constexpr message_t< 8 > master_volume( std::uint8_t manufacturer_id,
                                                std::uint16_t volume ) noexcept
        {
            const auto hex_value = uint16_to_two_bytes( volume );
            return system_exclusive( manufacturer_id, device_control_byte, master_volume_byte,
                                     hex_value.first, hex_value.second );
        }
    } // namespace realtime

    namespace non_realtime
    {
        static constexpr auto id_byte = std::byte{0x7E};
        static constexpr auto general_midi_system_byte = std::byte{0x09};

        template < class... Data >
        constexpr message_t< sizeof...( Data ) + std::size_t{4} >
        system_exclusive( std::uint8_t manufacturer_id, Data&&... data ) noexcept
        {
            return exclusive::system_exclusive( id_byte, manufacturer_id,
                                                std::forward< Data >( data )... );
        }

        constexpr message_t< 6 > general_midi_system( std::uint8_t manufacturer_id,
                                                      std::uint8_t value ) noexcept
        {
            return system_exclusive( manufacturer_id, general_midi_system_byte, value );
        }

        constexpr message_t< 6 >
        general_midi_system_disable( std::uint8_t manufacturer_id ) noexcept
        {
            return general_midi_system( manufacturer_id, 0x02 );
        }

        constexpr message_t< 6 >
        general_midi_1_system_enable( std::uint8_t manufacturer_id ) noexcept
        {
            return general_midi_system( manufacturer_id, 0x01 );
        }

        constexpr message_t< 6 >
        general_midi_2_system_enable( std::uint8_t manufacturer_id ) noexcept
        {
            return general_midi_system( manufacturer_id, 0x03 );
        }
    } // namespace non_realtime
} // namespace sequencer::midi::system::exclusive
