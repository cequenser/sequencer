#pragma once

#include <chrono>

namespace sequencer::chrono
{
    using milliseconds = std::chrono::duration< double, std::milli >;
    using seconds = std::chrono::duration< double, std::ratio< 1, 1 > >;
    using minutes = std::chrono::duration< double, std::ratio< 60, 1 > >;
} // namespace sequencer::chrono
