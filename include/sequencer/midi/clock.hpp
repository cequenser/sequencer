#pragma once

#include <sequencer/beat_duration.hpp>
#include <sequencer/beats_per_minute.hpp>
#include <sequencer/midi/clock_base.hpp>

#include <mutex>

namespace sequencer::midi
{
    template < class SequencerClock, class Sender >
    class clock
    {
    public:
        explicit clock( const SequencerClock& sequencer_clock, const Sender& sender )
            : sequencer_clock_( sequencer_clock ), sender_( sender )
        {
        }

        explicit clock( SequencerClock&& sequencer_clock, Sender&& sender )
            : sequencer_clock_( std::move( sequencer_clock ) ), sender_( std::move( sender ) )
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
                update_clock_base( now_as_beat_duration() );
                if ( shut_down_ )
                {
                    return;
                }
            }
        }

    private:
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
        SequencerClock sequencer_clock_;
        clock_base clock_base_{beat_time_point{-clock_base{}.tick()}};
        Sender sender_;
        beat_duration offset_ = 0.0_beats;
        beats_per_minute tempo_{120.0_bpm};
        bool shut_down_{false};
    };
} // namespace sequencer::midi
