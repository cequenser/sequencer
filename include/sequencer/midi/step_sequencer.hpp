#pragma once

#include <sequencer/midi/message/realtime.hpp>

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace sequencer::midi
{
    template < class Tracks >
    class step_sequencer
    {
    public:
        constexpr step_sequencer( const Tracks& tracks,
                                  unsigned pulses_per_quarter_note = 24 ) noexcept
            : track_( tracks ), pulses_per_quarter_note_( pulses_per_quarter_note )
        {
        }

        template < class Sender >
        void update( message_t< 1 > message, const Sender& sender )
        {
            if ( process_control_message( message, sender ) || !started_ )
            {
                return;
            }

            process_clock_message( sender );
        }

    private:
        template < class Sender >
        bool process_control_message( message_t< 1 > message, const Sender& sender )
        {
            if ( message == realtime::realtime_start() )
            {
                started_ = true;
                midi_beat_counter_ = 0;
                return true;
            }
            if ( message == realtime::realtime_continue() )
            {
                started_ = true;
                return true;
            }
            if ( message == realtime::realtime_stop() )
            {
                started_ = false;
                track_.send_all_notes_off_message( sender );
                return true;
            }

            return false;
        }

        template < class Sender >
        void process_clock_message( const Sender& sender )
        {
            const auto steps_per_beat = track_.steps() / 4u;
            const auto pulses_per_step = pulses_per_quarter_note_ / steps_per_beat;

            if ( midi_beat_counter_ % pulses_per_step == 0 )
            {
                const auto step = midi_beat_counter_ / pulses_per_step;
                track_.send_messages( step, sender );
            }
            if ( ++midi_beat_counter_ == track_.steps() * pulses_per_step )
            {
                midi_beat_counter_ = 0;
            }
        }

        const Tracks& track_;
        unsigned midi_beat_counter_ = 0;
        unsigned pulses_per_quarter_note_ = 24;
        bool started_ = false;
    };
} // namespace sequencer::midi
