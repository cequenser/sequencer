#pragma once

#include <sequencer/util/callable.hpp>
#include <sequencer/util/type_erased_storage.hpp>

#include <cassert>
#include <chrono>
#include <random>
#include <type_traits>
#include <utility>

namespace sequencer::midi
{
    using trig_condition_t = callable_t< bool >;

    namespace trig_condition
    {
        template < int trig, int size >
        class deterministic
        {
            static_assert( trig > 0 );
            static_assert( trig <= size );

        public:
            bool operator()() const noexcept
            {
                const auto result = ( current_++ == ( trig - 1 ) );
                current_ %= size;
                return result;
            }

        private:
            mutable int current_{0};
        };

        class random
        {
        public:
            explicit random( int percent ) : percent_( percent )
            {
            }

            bool operator()() const
            {
                using clock = std::chrono::system_clock;

                static const auto seed =
                    clock::from_time_t( std::time( nullptr ) ).time_since_epoch().count();
                static std::minstd_rand engine( seed );
                static std::uniform_int_distribution< int > distribution{};

                return distribution( engine, std::uniform_int_distribution< int >::param_type{
                                                 0, 99} ) < percent_;
            }

        private:
            int percent_{0};
        };
    } // namespace trig_condition
} // namespace sequencer::midi
