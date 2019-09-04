#pragma once

#include <cstddef>
#include <cstdint>

namespace sequencer::midi::byte
{
    // clang-format off
    constexpr auto                   note_on = std::byte{0x90};
    constexpr auto                  note_off = std::byte{0x80};
    constexpr auto  polymorphic_key_pressure = std::byte{0xA0};
    constexpr auto            control_change = std::byte{0xB0};
    constexpr auto            program_change = std::byte{0xC0};
    constexpr auto          channel_pressure = std::byte{0xD0};
    constexpr auto         pitch_bend_change = std::byte{0xE0};
    // clang-format on

    namespace cc
    {
        constexpr auto lsb( std::byte msb )
        {
            return std::byte( static_cast< std::uint8_t >( msb ) + 0x20 );
        }

        // clang-format off
        constexpr auto                      bank_select_msb = std::byte{0x00};
        constexpr auto                      bank_select_lsb = lsb(bank_select_msb);
        constexpr auto                 modulation_wheel_msb = std::byte{0x01};
        constexpr auto                 modulation_wheel_lsb = lsb(modulation_wheel_msb);
        constexpr auto                breath_controller_msb = std::byte{0x02};
        constexpr auto                breath_controller_lsb = lsb(breath_controller_msb);
        constexpr auto                  foot_controller_msb = std::byte{0x04};
        constexpr auto                  foot_controller_lsb = lsb(foot_controller_msb);
        constexpr auto                  portamento_time_msb = std::byte{0x05};
        constexpr auto                  portamento_time_lsb = lsb(portamento_time_msb);
        constexpr auto                       data_entry_msb = std::byte{0x06};
        constexpr auto                       data_entry_lsb = lsb(data_entry_msb);
        constexpr auto                   channel_volume_msb = std::byte{0x07};
        constexpr auto                   channel_volume_lsb = lsb(channel_volume_msb);
        constexpr auto                          balance_msb = std::byte{0x08};
        constexpr auto                          balance_lsb = lsb(balance_msb);
        constexpr auto                              pan_msb = std::byte{0x0A};
        constexpr auto                              pan_lsb = lsb(pan_msb);
        constexpr auto            expression_controller_msb = std::byte{0x0B};
        constexpr auto            expression_controller_lsb = lsb(expression_controller_msb);
        constexpr auto                 effect_control_1_msb = std::byte{0x0C};
        constexpr auto                 effect_control_1_lsb = lsb(effect_control_1_msb);
        constexpr auto                 effect_control_2_msb = std::byte{0x0D};
        constexpr auto                 effect_control_2_lsb = lsb(effect_control_2_msb);
        constexpr auto     general_purpose_controller_1_msb = std::byte{0x10};
        constexpr auto     general_purpose_controller_1_lsb = lsb(general_purpose_controller_1_msb);
        constexpr auto     general_purpose_controller_2_msb = std::byte{0x11};
        constexpr auto     general_purpose_controller_2_lsb = lsb(general_purpose_controller_2_msb);
        constexpr auto     general_purpose_controller_3_msb = std::byte{0x12};
        constexpr auto     general_purpose_controller_3_lsb = lsb(general_purpose_controller_3_msb);
        constexpr auto     general_purpose_controller_4_msb = std::byte{0x13};
        constexpr auto     general_purpose_controller_4_lsb = lsb(general_purpose_controller_4_msb);
        constexpr auto                         damper_pedal = std::byte{0x40};
        constexpr auto                           portamento = std::byte{0x41};
        constexpr auto                            sostenuto = std::byte{0x42};
        constexpr auto                           soft_pedal = std::byte{0x43};
        constexpr auto                               hold_2 = std::byte{0x45};
        constexpr auto                      effects_1_depth = std::byte{0x5B};
        constexpr auto                      effects_2_depth = std::byte{0x5C};
        constexpr auto                      effects_3_depth = std::byte{0x5D};
        constexpr auto                      effects_4_depth = std::byte{0x5E};
        constexpr auto                      effects_5_depth = std::byte{0x5F};
        constexpr auto  non_registered_parameter_number_lsb = std::byte{0x62};
        constexpr auto  non_registered_parameter_number_msb = std::byte{0x63};
        constexpr auto      registered_parameter_number_lsb = std::byte{0x64};
        constexpr auto      registered_parameter_number_msb = std::byte{0x65};
        constexpr auto                       all_sounds_off = std::byte{0x78};
        constexpr auto                reset_all_controllers = std::byte{0x79};
        constexpr auto                        local_control = std::byte{0x7A};
        constexpr auto                        all_notes_off = std::byte{0x7B};
        constexpr auto                        omni_mode_off = std::byte{0x7C};
        constexpr auto                         omni_mode_on = std::byte{0x7D};
        constexpr auto                         poly_mode_on = std::byte{0x7F};
        // clang-format on
    } // namespace cc
} // namespace sequencer::midi::byte
