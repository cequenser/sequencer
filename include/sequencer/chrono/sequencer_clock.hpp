#pragma once

#include <sequencer/chrono/chrono_adapter.hpp>

#include <chrono>

namespace sequencer::chrono
{
    inline namespace v1
    {

        template < typename UnderlyingClock = clock_object_adapter< std::chrono::steady_clock > >
        class sequencer_clock
        {
        public:
            using underlying_clock_type = UnderlyingClock;
            using underlying_time_point = typename underlying_clock_type::time_point;
            using rep = typename underlying_clock_type::rep;
            using period = typename underlying_clock_type::period;
            using duration = std::chrono::duration< rep, period >;
            using time_point = std::chrono::time_point< sequencer_clock >;

            static_assert( underlying_clock_type::is_steady, "Underlying clock must be steady" );

            constexpr static bool is_steady = false;

            explicit sequencer_clock( const underlying_clock_type& underlying_clock ) noexcept
                : underlying_clock_( underlying_clock )
            {
            }

            sequencer_clock( const sequencer_clock& ) = delete;
            sequencer_clock( sequencer_clock&& ) = delete;
            sequencer_clock& operator=( const sequencer_clock& ) = delete;
            sequencer_clock& operator=( sequencer_clock&& ) = delete;

            bool is_running() noexcept
            {
                return is_running_;
            }

            time_point now() const noexcept
            {
                if ( is_running_ )
                {
                    return time_point{elapsed_ + ( underlying_clock_.now() - start_time_ )};
                }
                return time_point{elapsed_};
            }

            void reset() noexcept
            {
                is_running_ = false;
            }

            void start() noexcept
            {
                if ( !is_running_ )
                {
                    start_time_ = underlying_clock_.now();
                    is_running_ = true;
                }
            }

            void stop() noexcept
            {
                if ( is_running_ )
                {
                    elapsed_ += underlying_clock_.now() - start_time_;
                    is_running_ = false;
                }
            }

        private:
            const underlying_clock_type& underlying_clock_;
            bool is_running_ = false;
            underlying_time_point start_time_ = underlying_time_point{duration::zero()};
            duration elapsed_ = duration::zero();
        };

    } // namespace v1
} // namespace sequencer::chrono
