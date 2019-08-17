#pragma once

#include <sequencer/midi/message/message_type.hpp>

namespace sequencer::midi::realtime
{
    constexpr message_t< 1 > realtime_clock() noexcept
    {
        return {std::byte{0xF8}};
    }

    constexpr message_t< 1 > realtime_start() noexcept
    {
        return {std::byte{0xFA}};
    }

    constexpr message_t< 1 > realtime_continue() noexcept
    {
        return {std::byte{0xFB}};
    }

    constexpr message_t< 1 > realtime_stop() noexcept
    {
        return {std::byte{0xFC}};
    }

    constexpr message_t< 1 > active_sensing() noexcept
    {
        return {std::byte{0xFE}};
    }

    constexpr message_t< 1 > reset_all() noexcept
    {
        return {std::byte{0xFF}};
    }
} // namespace sequencer::midi::realtime
