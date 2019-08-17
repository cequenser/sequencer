#pragma once

#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/util.hpp>

#include <cassert>
#include <cstdint>

namespace sequencer::midi::channel::voice
{
    constexpr message_t< 3 > note_on( std::uint8_t channel, std::uint8_t note,
                                      std::uint8_t velocity ) noexcept
    {
        assert( channel < 16 );
        assert( note < 128 );
        assert( velocity < 128 );

        return {status_byte_for( std::byte{0x90}, channel ), std::byte{note}, std::byte{velocity}};
    }

    constexpr message_t< 3 > note_off( std::uint8_t channel, std::uint8_t note,
                                       std::uint8_t velocity ) noexcept
    {
        assert( channel < 16 );
        assert( note < 128 );
        assert( velocity < 128 );

        return {status_byte_for( std::byte{0x80}, channel ), std::byte{note}, std::byte{velocity}};
    }

    constexpr message_t< 3 > polymorphic_key_pressure( std::uint8_t channel, std::uint8_t key,
                                                       std::uint8_t pressure ) noexcept
    {
        assert( channel < 16 );
        assert( key < 128 );
        assert( pressure < 128 );

        return {status_byte_for( std::byte{0xA0}, channel ), std::byte{key}, std::byte{pressure}};
    }

    constexpr std::byte control_change_status_byte( std::uint8_t channel ) noexcept
    {
        return status_byte_for( std::byte{0xB0}, channel );
    }

    constexpr message_t< 3 > control_change( std::uint8_t channel, std::uint8_t controller,
                                             std::uint8_t value ) noexcept
    {
        assert( channel < 16 );
        assert( controller < 120 );
        assert( value < 128 );

        return {control_change_status_byte( channel ), std::byte{controller}, std::byte{value}};
    }

    constexpr message_t< 2 > program_change( std::uint8_t channel, std::uint8_t program ) noexcept
    {
        assert( channel < 16 );
        assert( program < 128 );

        return {status_byte_for( std::byte{0xC0}, channel ), std::byte{program}};
    }

    constexpr message_t< 2 > channel_pressure( std::uint8_t channel,
                                               std::uint8_t pressure ) noexcept
    {
        assert( channel < 16 );
        assert( pressure < 128 );

        return {status_byte_for( std::byte{0xD0}, channel ), std::byte{pressure}};
    }

    constexpr message_t< 3 > pitch_bend_change( std::uint8_t channel, std::uint16_t value ) noexcept
    {
        assert( channel < 16 );
        assert( value < max_14bit );

        const auto hex_value = uint16_to_two_bytes( value );
        return {status_byte_for( std::byte{0xE0}, channel ), hex_value.first, hex_value.second};
    }

    constexpr std::byte on_off_byte( bool on ) noexcept
    {
        return on ? std::byte{0x7F} : std::byte{0x00};
    }

    constexpr message_t< 3 > damper_pedal( std::uint8_t channel, bool on ) noexcept
    {
        return {control_change_status_byte( channel ), std::byte{0x40}, on_off_byte( on )};
    }

    constexpr message_t< 3 > portamento( std::uint8_t channel, bool on ) noexcept
    {
        return {control_change_status_byte( channel ), std::byte{0x41}, on_off_byte( on )};
    }

    constexpr message_t< 3 > sostenuto( std::uint8_t channel, bool on ) noexcept
    {
        return {control_change_status_byte( channel ), std::byte{0x42}, on_off_byte( on )};
    }

    constexpr message_t< 3 > soft_pedal( std::uint8_t channel, bool on ) noexcept
    {
        return {control_change_status_byte( channel ), std::byte{0x43}, on_off_byte( on )};
    }

    constexpr message_t< 3 > hold_2( std::uint8_t channel, bool on ) noexcept
    {
        return {control_change_status_byte( channel ), std::byte{0x45}, on_off_byte( on )};
    }

    constexpr message_t< 3 > make_message( std::byte status, std::byte control_function ) noexcept
    {
        return {status, control_function, std::byte{0x00}};
    }

    constexpr message_t< 3 > all_sounds_off( std::uint8_t channel ) noexcept
    {
        assert( channel < 16 );
        return make_message( control_change_status_byte( channel ), std::byte{0x78} );
    }

    constexpr message_t< 3 > reset_all_controllers( std::uint8_t channel ) noexcept
    {
        assert( channel < 16 );
        return make_message( control_change_status_byte( channel ), std::byte{0x79} );
    }

    constexpr message_t< 3 > local_control( std::uint8_t channel, bool on ) noexcept
    {
        assert( channel < 16 );
        const auto data_byte = on ? std::byte{0x7F} : std::byte{0x00};
        return {control_change_status_byte( channel ), std::byte{0x7A}, data_byte};
    }

    constexpr message_t< 3 > all_notes_off( std::uint8_t channel ) noexcept
    {
        assert( channel < 16 );
        return make_message( control_change_status_byte( channel ), std::byte{0x7B} );
    }

    constexpr message_t< 3 > omni_mode_off( std::uint8_t channel ) noexcept
    {
        assert( channel < 16 );
        return make_message( control_change_status_byte( channel ), std::byte{0x7C} );
    }

    constexpr message_t< 3 > omni_mode_on( std::uint8_t channel ) noexcept
    {
        assert( channel < 16 );
        return make_message( control_change_status_byte( channel ), std::byte{0x7D} );
    }

    constexpr message_t< 3 > poly_mode_on( std::uint8_t channel ) noexcept
    {
        assert( channel < 16 );
        return make_message( control_change_status_byte( channel ), std::byte{0x7F} );
    }
} // namespace sequencer::midi::channel::voice
