#pragma once

#include <chrono>

namespace sequencer::chrono
{

    template < typename TrivialClock >
    class clock_object_adapter
    {
    public:
        using clock = TrivialClock;
        using rep = typename clock::rep;
        using period = typename clock::period;
        using duration = std::chrono::duration< rep, period >;
        using time_point = std::chrono::time_point< clock_object_adapter >;

        static constexpr bool is_steady = clock::is_steady;

        time_point now()
        {
            return clock::now();
        }
    };

} // namespace sequencer::chrono
