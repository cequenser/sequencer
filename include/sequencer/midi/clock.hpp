#pragma once

#include <sequencer/beat_duration.hpp>
#include <sequencer/beats_per_minute.hpp>
#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/midi/clock_base.hpp>

#include <atomic>
#include <mutex>

namespace sequencer::midi
{
    template < class Sender >
    class clock
    {
        using underlying_clock = chrono::clock_object_adapter< std::chrono::steady_clock >;
        using sequencer_clock = chrono::sequencer_clock< underlying_clock >;

    public:
        clock( const Sender& sender, beat_duration max_duration )
            : sender_( sender ), max_duration_( max_duration )
        {
        }

        clock( Sender&& sender, beat_duration max_duration )
            : sender_( std::move( sender ) ), max_duration_( max_duration )
        {
        }

        explicit clock( const Sender& sender ) : sender_( sender )
        {
        }

        explicit clock( Sender&& sender ) : sender_( std::move( sender ) )
        {
        }

        void start() noexcept
        {
            std::lock_guard lock{clock_mutex_};
            clock_base_.start();
            sequencer_clock_.start();
        }

        void stop() noexcept
        {
            std::lock_guard lock{clock_mutex_};
            clock_base_.stop();
            sequencer_clock_.stop();
        }

        void reset() noexcept
        {
            std::lock_guard lock{clock_mutex_};
            clock_base_.reset();
            sequencer_clock_.reset();
        }

        void shut_down() noexcept
        {
            std::lock_guard lock{clock_mutex_};
            shut_down_ = true;
            clock_base_.stop();
            sequencer_clock_.stop();
        }

        constexpr int pulses_per_quarter_note() const noexcept
        {
            return clock_base_.pulses_per_quarter_note();
        }

        void set_tempo( beats_per_minute tempo ) noexcept
        {
            tempo_ = tempo;
        }

        void run()
        {
            update_clock_base( 0.0_beats );
            while ( true )
            {
                std::lock_guard lock{clock_mutex_};
                const auto dt = now_as_beat_duration();
                if ( max_duration_ < dt )
                {
                    unlocked_shut_down();
                }
                update_clock_base( dt );

                if ( shut_down_ )
                {
                    return;
                }
            }
        }

    private:
        void unlocked_shut_down() noexcept
        {
            shut_down_ = true;
            clock_base_.stop();
            sequencer_clock_.stop();
        }

        constexpr beat_duration now_as_beat_duration() const noexcept
        {
            return ( sequencer_clock_.now() - sequencer_clock::time_point{} ) * tempo_;
        }

        void update_clock_base( beat_duration dt )
        {
            const auto t = beat_time_point{dt};
            clock_base_.update( t, sender_ );
        }

        std::mutex clock_mutex_;
        underlying_clock underlying_clock_;
        sequencer_clock sequencer_clock_{underlying_clock_};
        clock_base clock_base_{beat_time_point{-clock_base{}.tick()}};
        Sender sender_;
        beat_duration max_duration_ = std::numeric_limits< beat_duration >::max();
        std::atomic< beats_per_minute > tempo_{120.0_bpm};
        bool shut_down_{false};
    };
} // namespace sequencer::midi
