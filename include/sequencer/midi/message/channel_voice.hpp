#pragma once

#include <sequencer/assert.hpp>
#include <sequencer/midi/message/byte.hpp>
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

        return {status_byte_for( byte::note_on, channel ), std::byte{note}, std::byte{velocity}};
    }

    constexpr message_t< 3 > note_off( std::uint8_t channel, std::uint8_t note,
                                       std::uint8_t velocity ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( note < 128 );
        SEQUENCER_ASSERT( velocity < 128 );

        return {status_byte_for( byte::note_off, channel ), std::byte{note}, std::byte{velocity}};
    }

    constexpr message_t< 3 > polymorphic_key_pressure( std::uint8_t channel, std::uint8_t key,
                                                       std::uint8_t pressure ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( key < 128 );
        SEQUENCER_ASSERT( pressure < 128 );

        return {status_byte_for( byte::polymorphic_key_pressure, channel ), std::byte{key},
                std::byte{pressure}};
    }

    constexpr message_t< 3 > control_change( std::uint8_t channel, std::byte controller,
                                             std::byte value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );

        return {status_byte_for( byte::control_change, channel ), controller, value};
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

        return {status_byte_for( byte::program_change, channel ), std::byte{program}};
    }

    constexpr message_t< 2 > channel_pressure( std::uint8_t channel,
                                               std::uint8_t pressure ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( pressure < 128 );

        return {status_byte_for( byte::channel_pressure, channel ), std::byte{pressure}};
    }

    constexpr message_t< 3 > pitch_bend_change( std::uint8_t channel, std::int16_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < max_14bit / 2 );
        SEQUENCER_ASSERT( value > -max_14bit / 2 );

        const std::int32_t offset = 8192;
        const auto [ lsb, msb ] = uint16_to_lsb_msb( std::uint16_t( offset + value ) );
        return {status_byte_for( byte::pitch_bend_change, channel ), lsb, msb};
    }

    constexpr std::array< message_t< 3 >, 2 >
    control_change_lsb_msb( std::uint8_t channel, std::byte control_function_msb,
                            std::byte control_function_lsb, std::uint16_t value )
    {
        const auto [ lsb, msb ] = uint16_to_lsb_msb( value );
        return {control_change( channel, control_function_lsb, lsb ),
                control_change( channel, control_function_msb, msb )};
    }

    constexpr std::array< message_t< 3 >, 2 >
    control_change_lsb_msb( std::uint8_t channel, std::byte control_function, std::uint16_t value )
    {
        return control_change_lsb_msb( channel, control_function, byte::cc::lsb( control_function ),
                                       value );
    }

    constexpr std::array< message_t< 3 >, 2 > bank_select( std::uint8_t channel,
                                                           std::uint16_t bank ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::bank_select_msb, bank );
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
        return control_change_lsb_msb( channel, byte::cc::modulation_wheel_msb, value );
    }

    constexpr std::array< message_t< 3 >, 2 > breath_controller( std::uint8_t channel,
                                                                 std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::breath_controller_msb, value );
    }

    constexpr std::array< message_t< 3 >, 2 > foot_controller( std::uint8_t channel,
                                                               std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::foot_controller_msb, value );
    }

    constexpr std::array< message_t< 3 >, 2 > portamento_time( std::uint8_t channel,
                                                               std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::portamento_time_msb, value );
    }

    constexpr std::array< message_t< 3 >, 2 > data_entry( std::uint8_t channel,
                                                          std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::data_entry_msb, value );
    }

    constexpr std::array< message_t< 3 >, 2 > channel_volume( std::uint8_t channel,
                                                              std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::channel_volume_msb, value );
    }

    constexpr std::array< message_t< 3 >, 2 > balance( std::uint8_t channel,
                                                       std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::balance_msb, value );
    }

    constexpr std::array< message_t< 3 >, 2 > pan( std::uint8_t channel,
                                                   std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::pan_msb, value );
    }

    constexpr std::array< message_t< 3 >, 2 > expression_controller( std::uint8_t channel,
                                                                     std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::expression_controller_msb, value );
    }

    constexpr std::array< message_t< 3 >, 2 > effect_control( std::uint8_t id, std::uint8_t channel,
                                                              std::uint16_t value ) noexcept
    {
        SEQUENCER_ASSERT( id >= 1 );
        SEQUENCER_ASSERT( id <= 2 );
        const auto control_function =
            ( id == 1 ) ? byte::cc::effect_control_1_msb : byte::cc::effect_control_2_msb;
        return control_change_lsb_msb( channel, control_function, value );
    }

    constexpr std::array< message_t< 3 >, 2 >
    general_purpose_controller( std::uint8_t id, std::uint8_t channel,
                                std::uint16_t value ) noexcept
    {
        SEQUENCER_ASSERT( id >= 1 );
        SEQUENCER_ASSERT( id <= 4 );
        const auto control_function =
            ( id == 1 ) ? byte::cc::general_purpose_controller_1_msb
                        : ( id == 2 ) ? byte::cc::general_purpose_controller_2_msb
                                      : ( id == 3 ) ? byte::cc::general_purpose_controller_3_msb
                                                    : byte::cc::general_purpose_controller_4_msb;
        return control_change_lsb_msb( channel, control_function, value );
    }

    constexpr std::array< message_t< 3 >, 2 >
    non_registered_parameter_number( std::uint8_t channel, std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::non_registered_parameter_number_msb,
                                       byte::cc::non_registered_parameter_number_lsb, value );
    }

    constexpr std::array< message_t< 3 >, 2 >
    registered_parameter_number( std::uint8_t channel, std::uint16_t value ) noexcept
    {
        return control_change_lsb_msb( channel, byte::cc::registered_parameter_number_msb,
                                       byte::cc::registered_parameter_number_lsb, value );
    }

    constexpr std::byte on_off_byte( bool on ) noexcept
    {
        return on ? std::byte{0x7F} : std::byte{0x00};
    }

    constexpr message_t< 3 > damper_pedal( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, byte::cc::damper_pedal, on_off_byte( on ) );
    }

    constexpr message_t< 3 > portamento( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, byte::cc::portamento, on_off_byte( on ) );
    }

    constexpr message_t< 3 > sostenuto( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, byte::cc::sostenuto, on_off_byte( on ) );
    }

    constexpr message_t< 3 > soft_pedal( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, byte::cc::soft_pedal, on_off_byte( on ) );
    }

    constexpr message_t< 3 > hold_2( std::uint8_t channel, bool on ) noexcept
    {
        return control_change( channel, byte::cc::hold_2, on_off_byte( on ) );
    }

    constexpr message_t< 3 > control_change( std::uint8_t channel,
                                             std::byte control_function ) noexcept
    {
        return control_change( channel, control_function, std::byte{0x00} );
    }

    constexpr message_t< 3 > all_sounds_off( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, byte::cc::all_sounds_off );
    }

    constexpr message_t< 3 > reset_all_controllers( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, byte::cc::reset_all_controllers );
    }

    constexpr message_t< 3 > local_control( std::uint8_t channel, bool on ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        const auto data_byte = on ? std::byte{0x7F} : std::byte{0x00};
        return control_change( channel, byte::cc::local_control, data_byte );
    }

    constexpr message_t< 3 > all_notes_off( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, byte::cc::all_notes_off );
    }

    constexpr message_t< 3 > omni_mode_off( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, byte::cc::omni_mode_off );
    }

    constexpr message_t< 3 > omni_mode_on( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, byte::cc::omni_mode_on );
    }

    constexpr message_t< 3 > poly_mode_on( std::uint8_t channel ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        return control_change( channel, byte::cc::poly_mode_on );
    }

    constexpr message_t< 3 > effects_1_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, byte::cc::effects_1_depth, std::byte{value} );
    }

    constexpr message_t< 3 > effects_2_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, byte::cc::effects_2_depth, std::byte{value} );
    }

    constexpr message_t< 3 > effects_3_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, byte::cc::effects_3_depth, std::byte{value} );
    }

    constexpr message_t< 3 > effects_4_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, byte::cc::effects_4_depth, std::byte{value} );
    }

    constexpr message_t< 3 > effects_5_depth( std::uint8_t channel, std::uint8_t value ) noexcept
    {
        SEQUENCER_ASSERT( channel < 16 );
        SEQUENCER_ASSERT( value < 128 );
        return control_change( channel, byte::cc::effects_5_depth, std::byte{value} );
    }
} // namespace sequencer::midi::channel::voice
