#pragma once

#include <sequencer/beat_duration.hpp>
#include <sequencer/beats_per_minute.hpp>
#include <sequencer/midi/clock_base.hpp>

#include <future>
#include <mutex>
#include <thread>

namespace sequencer::midi
{
    template < class SequencerClock >
    class clock
    {
    public:
        explicit clock( const SequencerClock& sequencer_clock )
            : sequencer_clock_( sequencer_clock )
        {
        }

        explicit clock( SequencerClock&& sequencer_clock )
            : sequencer_clock_( std::move( sequencer_clock ) )
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

        bool is_running() const noexcept
        {
            return sequencer_clock_.is_running();
        }

        template < class Sender >
        void run( const Sender& sender )
        {
            update_clock_base( 0.0_beats, sender );
            while ( true )
            {
                std::lock_guard lock{clock_mutex_};
                update_clock_base( now_as_beat_duration(), sender );
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

        template < class Sender >
        void update_clock_base( beat_duration dt, const Sender& sender )
        {
            const auto t = beat_time_point{dt};
            clock_base_.update( t, sender );
        }

        std::mutex clock_mutex_;
        SequencerClock sequencer_clock_;
        clock_base clock_base_{beat_time_point{-clock_base{}.tick()}};
        beat_duration offset_ = 0.0_beats;
        beats_per_minute tempo_{120.0_bpm};
        bool shut_down_{false};
    };

    template < class MidiClock, class Sender >
    auto start_clock_in_thread( MidiClock& midi_clock, const Sender& sender )
    {
        std::promise< void > controller_ready_promise;
        const auto controller_ready = controller_ready_promise.get_future();
        auto clock_done =
            std::async( std::launch::async, [sender, &midi_clock, &controller_ready_promise] {
                controller_ready_promise.set_value();
                midi_clock.run( sender );
                // wait a bit to make sure that the last messages where sent
                // TODO solve without sleep if possible
                std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
            } );
        controller_ready.wait();
        return clock_done;
    }
} // namespace sequencer::midi
