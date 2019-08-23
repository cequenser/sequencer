#pragma once

#include <array>
#include <cstddef>
#include <type_traits>
#include <vector>

namespace sequencer::midi
{
    template < std::size_t lhs, std::size_t rhs >
    struct greater_than
    {
        static constexpr auto value = lhs > rhs;
    };
    template < std::size_t number_of_bytes >
    using message_t =
        std::conditional_t< greater_than< number_of_bytes, 0 >::value,
                            std::array< std::byte, number_of_bytes >, std::vector< std::byte > >;
} // namespace sequencer::midi
