#pragma once

#include <sequencer/assert.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/util.hpp>

#include <cstdint>
#include <tuple>

namespace sequencer::midi::channel::voice
{
    constexpr message_t< 3 > note_on( std::uint8_t channel, std::uint8_t note,
                                      std::uint8_t velocity ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( note < 128 );
        SEQUENCER_ASSERT( velocity < 128 );

        return {status_byte_for( std::byte{0x90}, channel ), std::byte{note}, std::byte{velocity}};
    }

    constexpr message_t< 3 > note_off( std::uint8_t channel, std::uint8_t note,
                                       std::uint8_t velocity ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( note < 128 );
        SEQUENCER_ASSERT( velocity < 128 );

        return {status_byte_for( std::byte{0x80}, channel ), std::byte{note}, std::byte{velocity}};
    }

    constexpr message_t< 3 > polymorphic_key_pressure( std::uint8_t channel, std::uint8_t key,
                                                       std::uint8_t pressure ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( key < 128 );
        SEQUENCER_ASSERT( pressure < 128 );

        return {status_byte_for( std::byte{0xA0}, channel ), std::byte{key}, std::byte{pressure}};
    }

    constexpr std::byte control_change_status_byte( std::uint8_t channel ) noexcept
    {
        return status_byte_for( std::byte{0xB0}, channel );
    }

    constexpr message_t< 3 > control_change( std::uint8_t channel, std::byte controller,
                                             std::byte value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );

        return {control_change_status_byte( channel ), controller, value};
    }

    constexpr message_t< 3 > control_change( std::uint8_t channel, std::uint8_t controller,
                                             std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( controller < 120 );
        SEQUENCER_ASSERT( value < 128 );

        return control_change( channel, std::byte{controller}, std::byte{value} );
    }

    constexpr message_t< 2 > program_change( std::uint8_t channel, std::uint8_t program ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( program < 128 );

        return {status_byte_for( std::byte{0xC0}, channel ), std::byte{program}};
    }

    constexpr message_t< 2 > channel_pressure( std::uint8_t channel,
                                               std::uint8_t pressure ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( pressure < 128 );

        return {status_byte_for( std::byte{0xD0}, channel ), std::byte{pressure}};
    }

    constexpr message_t< 3 > pitch_bend_change( std::uint8_t channel, std::int16_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < max_14bit / 2 );
        SEQUENCER_ASSERT( value > -max_14bit / 2 );

        const std::int32_t offset = 8192;
        const auto [ lsb, msb ] = uint16_to_lsb_msb( std::uint16_t( offset + value ) );
        return {status_byte_for( std::byte{0xE0}, channel ), lsb, msb};
    }

    constexpr std::array< message_t< 3 >, 2 > control_change_lsb_msb( std::uint8_t channel,
                                                                      std::uint8_t control_function,
                                                                      std::uint16_t value )
    {
        const auto [ lsb, msb ] = uint16_to_lsb_msb( value );
        return {control_change( channel, std::byte( control_function + 32 ), lsb ),
                control_change( channel, std::byte{control_function}, msb )};
    }

    constexpr std::array< message_t< 3 >, 2 > bank_select( std::uint8_t channel,
                                                           std::uint16_t bank ) noexcept
    {
        return control_change_lsb_msb( channel, 0x00, bank );
    }

    constexpr std::tuple< message_t< 3 >, message_t< 3 >, message_t< 2 > >
    bank_select_with_program( std::uint8_t channel, std::uint16_t bank,
                              std::uint8_t program ) noexcept
    {
        const auto [ lsb, msb ] = bank_select( channel, bank );
        return {lsb, msb, program_change( channel, program )};
    }

    constexpr std::array< message_t< 3 >, 2 > modulation_wheel( std::uint8_t channel,
                                                                std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, 0x01, value );
    }

    constexpr std::array< message_t< 3 >, 2 > breath_controller( std::uint8_t channel,
                                                                 std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, 0x02, value );
    }

    constexpr std::array< message_t< 3 >, 2 > foot_controller( std::uint8_t channel,
                                                               std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, 0x04, value );
    }

    constexpr std::array< message_t< 3 >, 2 > portamento_time( std::uint8_t channel,
                                                               std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, 0x05, value );
    }

    constexpr std::array< message_t< 3 >, 2 > data_entry( std::uint8_t channel,
                                                          std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, 0x06, value );
    }

    constexpr std::array< message_t< 3 >, 2 > channel_volume( std::uint8_t channel,
                                                              std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, 0x07, value );
    }

    constexpr std::array< message_t< 3 >, 2 > balance( std::uint8_t channel,
                                                       std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, 0x08, value );
    }

    constexpr std::array< message_t< 3 >, 2 > pan( std::uint8_t channel,
                                                   std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, 0x0A, value );
    }

    constexpr std::array< message_t< 3 >, 2 > expression_controller( std::uint8_t channel,
                                                                     std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, 0x0B, value );
    }

    constexpr std::array< message_t< 3 >, 2 > effect_control( std::uint8_t id, std::uint8_t channel,
                                                              std::uint16_t value ) noexcept
    {
        SEQUENCER_ASSERT( id >= 1 );
        SEQUENCER_ASSERT( id <= 2 );
        const auto control_function = ( id == 1 ) ? 0x0C : 0x0D;
        return control_change_lsb_msb( channel, control_function, value );
    }

    constexpr std::array< message_t< 3 >, 2 >
    general_purpose_controller( std::uint8_t id, std::uint8_t channel,
                                std::uint16_t value ) noexcept
    {
        SEQUENCER_ASSERT( id >= 1 );
        SEQUENCER_ASSERT( id <= 4 );
        const auto control_function =
            ( id == 1 ) ? 0x10 : ( id == 2 ) ? 0x11 : ( id == 3 ) ? 0x12 : 0x13;
        return control_change_lsb_msb( channel, control_function, value );
    }

    constexpr std::array< message_t< 3 >, 2 >
    non_registered_parameter_number( std::uint8_t channel, std::uint16_t value ) noexcept
    {
        const auto [ lsb, msb ] = uint16_to_lsb_msb( value );
        return {control_change( channel, std::byte{0x62}, lsb ),
                control_change( channel, std::byte{0x63}, msb )};
    }

    constexpr std::array< message_t< 3 >, 2 >
    registered_parameter_number( std::uint8_t channel, std::uint16_t value ) noexcept
    {
        const auto [ lsb, msb ] = uint16_to_lsb_msb( value );
        return {control_change( channel, std::byte{0x64}, lsb ),
                control_change( channel, std::byte{0x65}, msb )};
    }

    constexpr std::byte on_off_byte( bool on ) noexcept
    {
        return on ? std::byte{0x7F} : std::byte{0x00};
    }

    constexpr message_t< 3 > damper_pedal( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, std::byte{0x40}, on_off_byte( on ) );
    }

    constexpr message_t< 3 > portamento( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, std::byte{0x41}, on_off_byte( on ) );
    }

    constexpr message_t< 3 > sostenuto( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, std::byte{0x42}, on_off_byte( on ) );
    }

    constexpr message_t< 3 > soft_pedal( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, std::byte{0x43}, on_off_byte( on ) );
    }

    constexpr message_t< 3 > hold_2( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, std::byte{0x45}, on_off_byte( on ) );
    }

    constexpr message_t< 3 > control_change( std::uint8_t channel,
                                             std::byte control_function ) noexcept
    {
        return {control_change_status_byte( channel ), control_function, std::byte{0x00}};
    }

    constexpr message_t< 3 > all_sounds_off( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, std::byte{0x78} );
    }

    constexpr message_t< 3 > reset_all_controllers( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, std::byte{0x79} );
    }

    constexpr message_t< 3 > local_control( std::uint8_t channel, bool on ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        const auto data_byte = on ? std::byte{0x7F} : std::byte{0x00};
        return control_change( channel, std::byte{0x7A}, data_byte );
    }

    constexpr message_t< 3 > all_notes_off( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, std::byte{0x7B} );
    }

    constexpr message_t< 3 > omni_mode_off( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, std::byte{0x7C} );
    }

    constexpr message_t< 3 > omni_mode_on( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, std::byte{0x7D} );
    }

    constexpr message_t< 3 > poly_mode_on( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, std::byte{0x7F} );
    }

    constexpr message_t< 3 > effects_1_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, std::byte{0x5B}, std::byte{value} );
    }

    constexpr message_t< 3 > effects_2_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, std::byte{0x5C}, std::byte{value} );
    }

    constexpr message_t< 3 > effects_3_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, std::byte{0x5D}, std::byte{value} );
    }

    constexpr message_t< 3 > effects_4_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, std::byte{0x5E}, std::byte{value} );
    }

    constexpr message_t< 3 > effects_5_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, std::byte{0x5F}, std::byte{value} );
    }
} // namespace sequencer::midi::channel::voice
