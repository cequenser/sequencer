#pragma once

#include <cstddef>

namespace sequencer::midi::message::real_time
{
    constexpr std::byte timing_clock() noexcept
    {
        return std::byte{0xF8};
    }

    constexpr std::byte start() noexcept
    {
        return std::byte{0xFA};
    }

    constexpr std::byte cont() noexcept
    {
        return std::byte{0xFB};
    }

    constexpr std::byte stop() noexcept
    {
        return std::byte{0xFC};
    }

    constexpr std::byte active_sensing() noexcept
    {
        return std::byte{0xFE};
    }

    constexpr std::byte reset() noexcept
    {
        return std::byte{0xFF};
    }
} // namespace sequencer::midi::message::real_time
