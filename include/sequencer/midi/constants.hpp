#pragma once

#include <sequencer/midi/message/message_type.hpp>

namespace sequencer::midi
{
    constexpr auto number_of_values_per_byte = 128;

    constexpr auto max_number_of_midi_channels = 16;
    constexpr auto default_pulses_per_quarter_note = 24u;

    namespace manufacturer
    {
        constexpr message_t< 3 > elektron = {std::byte{0x00}, std::byte{0x20}, std::byte{0x3C}};
    }
} // namespace sequencer::midi
