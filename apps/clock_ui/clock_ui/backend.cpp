#include "backend.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>

namespace qml
{
    backend::backend()
        : midiout_{sequencer::rtmidi::make_midi_port< RtMidiOut >()},
          clock_{sequencer::rtmidi::make_clock( *midiout_ )}, clock_done_{
                                                                  start_clock_in_thread( clock_ )}
    {
        if ( !midiout_ )
        {
            std::cout << "Failed to create MIDI out" << std::endl;
        }
    }

    backend::~backend()
    {
        clock_.shut_down();
    }

    void backend::start_clock()
    {
        clock_.start();
    }

    void backend::stop_clock()
    {
        clock_.stop();
    }

    void backend::reset_clock()
    {
        clock_.reset();
    }

    void backend::set_clock_tempo( int bpm )
    {
        clock_.set_tempo( sequencer::beats_per_minute( bpm ) );
    }

    void backend::set_song_position_pointer( int song_position_in_16th_notes )
    {
        auto sender = sequencer::rtmidi::message_sender{*midiout_};

        static constexpr auto max_song_position = 16383; // 2^14 - 1
        assert( song_position_in_16th_notes > 0 );
        assert( song_position_in_16th_notes <= max_song_position );
        sender(
            sequencer::midi::system::common::song_position_pointer( song_position_in_16th_notes ) );
    }
} // namespace qml
