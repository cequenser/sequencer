#pragma once

#include <cstddef>
#include <cstdint>

namespace sequencer::midi
{
    static constexpr auto invalid_string = "invalid";

    constexpr std::byte status_byte_for( std::byte status_byte, std::uint8_t channel ) noexcept
    {
        return status_byte | std::byte{channel};
    }
} // namespace sequencer::midi
