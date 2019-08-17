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

        constexpr void set_steps_per_beat( unsigned steps ) noexcept
        {
            pulses_per_step_ = pulses_per_step( steps );
        }

    private:
        constexpr unsigned pulses_per_step( unsigned steps ) noexcept
        {
            return pulses_per_quarter_note_ / steps;
        }

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

            if ( midi_beat_counter_ % pulses_per_step_ == 0 )
            {
                const auto step = midi_beat_counter_ / pulses_per_step_;
                track_.send_messages( step, sender );
            }
            if ( ++midi_beat_counter_ == track_.steps() * pulses_per_step_ )
            {
                midi_beat_counter_ = 0;
            }
        }

        const Tracks& track_;
        unsigned midi_beat_counter_ = 0;
        unsigned pulses_per_quarter_note_;
        unsigned pulses_per_step_{pulses_per_step( 4 )};
        bool started_ = false;
    };
} // namespace sequencer::midi
