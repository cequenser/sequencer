#pragma once

#include <sequencer/midi/message/byte.hpp>
#include <sequencer/midi/message/message_type.hpp>

namespace sequencer::midi::realtime
{
    constexpr message_t< 1 > realtime_clock() noexcept
    {
        return {byte::realtime_clock};
    }

    constexpr message_t< 1 > realtime_start() noexcept
    {
        return {byte::realtime_start};
    }

    constexpr message_t< 1 > realtime_continue() noexcept
    {
        return {byte::realtime_continue};
    }

    constexpr message_t< 1 > realtime_stop() noexcept
    {
        return {byte::realtime_stop};
    }

    constexpr message_t< 1 > active_sensing() noexcept
    {
        return {byte::active_sensing};
    }

    constexpr message_t< 1 > reset_all() noexcept
    {
        return {byte::reset_all};
    }
} // namespace sequencer::midi::realtime
