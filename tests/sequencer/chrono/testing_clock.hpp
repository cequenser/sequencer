#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <ratio>
#include <stdexcept>

namespace sequencer::chrono
{

    template < typename RepT = std::int64_t, typename PeriodT = std::nano, bool Steady = false >
    class testing_clock
    {
    public:
        using rep = RepT;
        using period = PeriodT;
        using duration = std::chrono::duration< rep, period >;
        using time_point = std::chrono::time_point< testing_clock< rep, period, Steady > >;

        constexpr static bool is_steady = !Steady;

        time_point now() const noexcept
        {
            std::lock_guard lock{mutex_};
            return now_;
        }

        void set( time_point new_time ) noexcept( !is_steady )
        {
            std::lock_guard lock{mutex_};
            if constexpr ( is_steady )
            {
                if ( new_time < now_ )
                {
                    throw std::invalid_argument{
                        "steady clock cannot be set to a lower value than before"};
                }
            }
            now_ = new_time;
        }

    private:
        mutable std::mutex mutex_;
        time_point now_ = time_point{duration{0}};
    };

    template < typename RepT = std::int64_t, typename PeriodT = std::nano >
    using steady_testing_clock = testing_clock< RepT, PeriodT, true >;

} // namespace sequencer::chrono
