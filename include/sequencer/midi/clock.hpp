#pragma once

#include <sequencer/beat_duration.hpp>
#include <sequencer/beats_per_minute.hpp>
#include <sequencer/midi/clock_base.hpp>

#include <atomic>
#include <mutex>

namespace sequencer::midi
{
    template < class SequencerClock, class Sender >
    class clock
    {
    public:
        clock( SequencerClock& sequencer_clock, const Sender& sender, beat_duration max_duration )
            : sequencer_clock_( sequencer_clock ), sender_( sender ), max_duration_( max_duration )
        {
        }

        clock( SequencerClock& sequencer_clock, Sender&& sender, beat_duration max_duration )
            : sequencer_clock_( sequencer_clock ), sender_( std::move( sender ) ),
              max_duration_( max_duration )
        {
        }

        explicit clock( SequencerClock& sequencer_clock, const Sender& sender )
            : sequencer_clock_( sequencer_clock ), sender_( sender )
        {
        }

        explicit clock( SequencerClock& sequencer_clock, Sender&& sender )
            : sequencer_clock_( sequencer_clock ), sender_( std::move( sender ) )
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
            std::lock_guard lock( clock_mutex_ );
            offset_ = now_as_beat_duration();
            sequencer_clock_.reset();
            sequencer_clock_.start();
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

        beat_duration now_as_beat_duration() const noexcept
        {
            return offset_ +
                   ( sequencer_clock_.now() - typename SequencerClock::time_point{} ) * tempo_;
        }

        void update_clock_base( beat_duration dt )
        {
            const auto t = beat_time_point{dt};
            clock_base_.update( t, sender_ );
        }

        std::mutex clock_mutex_;
        SequencerClock& sequencer_clock_;
        clock_base clock_base_{beat_time_point{-clock_base{}.tick()}};
        Sender sender_;
        beat_duration max_duration_ = std::numeric_limits< beat_duration >::max();
        beat_duration offset_ = 0.0_beats;
        beats_per_minute tempo_{120.0_bpm};
        bool shut_down_{false};
    };
} // namespace sequencer::midi
