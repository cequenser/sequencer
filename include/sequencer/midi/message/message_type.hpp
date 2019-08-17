#pragma once

#include <array>
#include <cstddef>
#include <type_traits>
#include <vector>

namespace sequencer::midi
{
    template < std::size_t number_of_bytes >
    using message_t =
        std::conditional_t< std::greater<>{}( number_of_bytes, 0 ),
                            std::array< std::byte, number_of_bytes >, std::vector< std::byte > >;
}
