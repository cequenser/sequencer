#pragma once

#include <array>
#include <cstddef>

namespace sequencer::midi
{
    template < std::size_t number_of_bytes >
    using message_t = std::array< std::byte, number_of_bytes >;
}
