#pragma once

#include <sequencer/midi/message/channel_mode.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/realtime.hpp>

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace sequencer::midi
{
    template < class Track, class Sender >
    class step_sequencer
    {
    public:
        constexpr step_sequencer( const Track& track, const Sender& sender ) noexcept(
            std::is_nothrow_copy_constructible_v< Track >&&
                std::is_nothrow_copy_constructible_v< Sender > )
            : track_( track ), sender_( sender )
        {
        }

        ~step_sequencer()
        {
            sender_( channel::mode::all_notes_off( channel_ ) );
        }

        void update( realtime::message_type message )
        {
            if ( process_control_message( message ) || !started_ )
            {
                return;
            }

            process_clock_message();
        }

        Track& track() noexcept
        {
            return track_;
        }

        const Track& track() const noexcept
        {
            return track_;
        }

        void set_channel( std::uint8_t channel ) noexcept
        {
            assert( channel < 16 );
            channel_ = channel;
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
                sender_( channel::mode::all_notes_off( channel_ ) );
                started_ = false;
                last_note_ = Track::no_note;
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
                const auto note = track_[ step ].load();
                if ( note != Track::no_note )
                {
                    if ( last_note_ != Track::no_note )
                    {
                        sender_( channel::voice::note_off( channel_, last_note_, velocity_ ) );
                    }
                    sender_( channel::voice::note_on( channel_, note, velocity_ ) );
                    last_note_ = note;
                }
            }
            midi_beat_counter_ += 1;
            if ( midi_beat_counter_ == track_.steps() * midi_clock_messages_per_step )
            {
                midi_beat_counter_ = 0;
            }
        }

        Track track_;
        Sender sender_;
        unsigned midi_beat_counter_ = 0;
        int last_note_ = Track::no_note;
        std::uint8_t channel_ = 0;
        std::uint8_t velocity_ = 32;
        bool started_ = false;
    };
} // namespace sequencer::midi
