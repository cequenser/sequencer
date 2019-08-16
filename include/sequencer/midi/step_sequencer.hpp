#pragma once

#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/realtime.hpp>

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace sequencer::midi
{
    template < class Tracks, class Sender >
    class step_sequencer
    {
    public:
        constexpr step_sequencer( const Tracks& tracks, const Sender& sender ) noexcept(
            std::is_nothrow_copy_constructible_v< Tracks >&&
                std::is_nothrow_copy_constructible_v< Sender > )
            : track_( tracks ), sender_( sender )
        {
        }

        ~step_sequencer()
        {
            track_.send_all_notes_off_message( sender_ );
        }

        void update( realtime::message_type message )
        {
            if ( process_control_message( message ) || !started_ )
            {
                return;
            }

            process_clock_message();
        }

        Tracks& tracks() noexcept
        {
            return track_;
        }

        const Tracks& tracks() const noexcept
        {
            return track_;
        }

    private:
        bool process_control_message( realtime::message_type message )
        {
            if ( message == realtime::message_type::realtime_start )
            {
                started_ = true;
                midi_beat_counter_ = 0;
                return true;
            }
            if ( message == realtime::message_type::realtime_continue )
            {
                started_ = true;
                return true;
            }
            if ( message == realtime::message_type::realtime_stop )
            {
                started_ = false;
                track_.send_all_notes_off_message( sender_ );
                track_.clear_last_note();
                return true;
            }

            return false;
        }

        void process_clock_message()
        {
            constexpr auto midi_clock_messages_per_beat = 24u;
            const auto steps_per_beat = track_.steps() / 4u;
            const auto midi_clock_messages_per_step = midi_clock_messages_per_beat / steps_per_beat;

            if ( midi_beat_counter_ % midi_clock_messages_per_step == 0 )
            {
                const auto step = midi_beat_counter_ / midi_clock_messages_per_step;
                track_.send_messages( step, sender_ );
            }
            if ( ++midi_beat_counter_ == track_.steps() * midi_clock_messages_per_step )
            {
                midi_beat_counter_ = 0;
            }
        }

        Tracks track_;
        Sender sender_;
        unsigned midi_beat_counter_ = 0;
        bool started_ = false;
    };
} // namespace sequencer::midi
