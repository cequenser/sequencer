#pragma once

#include <cstdint>

namespace sequencer::midi::realtime
{
    enum class message_type : std::uint8_t
    {
        invalid = 0,
        realtime_clock = 0xF8,
        realtime_start = 0xFA,
        realtime_continue = 0xFB,
        realtime_stop = 0XFC
    };
} // namespace sequencer::midi::realtime
