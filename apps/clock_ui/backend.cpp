#include "backend.hpp"

#include <sequencer/midi/percussion_key.hpp>

#include <cassert>
#include <cstdint>
#include <iostream>

namespace qml
{
    backend::backend()
        : midiout_{RtMidiOut()}, step_sequencer_{track{},
                                                 sequencer::rtmidi::message_sender{midiout_}},
          clock_{sequencer::rtmidi::make_clock( midiout_, step_sequencer_ )},
          clock_done_{start_clock_in_thread( clock_ )}
    {
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
        auto sender = sequencer::rtmidi::message_sender{midiout_};

        static constexpr auto max_song_position = 16383; // 2^14 - 1
        assert( song_position_in_16th_notes >= 0 );
        assert( song_position_in_16th_notes <= max_song_position );
        sender(
            sequencer::midi::system::common::song_position_pointer( song_position_in_16th_notes ) );
    }

    QString backend::available_ports()
    {
        QString ports = "no port selected";
        for ( auto id = 0u; id < midiout_.getPortCount(); ++id )
        {
            ports += ( ";" + midiout_.getPortName( id ) ).c_str();
        }
        return ports;
    }

    bool backend::open_port( unsigned id )
    {
        assert( id <= midiout_.getPortCount() + 1 );

        if ( midiout_.isPortOpen() )
        {
            midiout_.closePort();
        }
        if ( id == 0 )
        {
            return true;
        }
        midiout_.openPort( id - 1 );
        return midiout_.isPortOpen();
    }

    void backend::set_step( int i, bool checked )
    {
        step_sequencer_.track()[ track::size_type( i ) ] =
            checked ? static_cast< std::uint8_t >( sequencer::midi::percussion_key::BassDrum_1 )
                    : std::uint8_t{0};
    }
} // namespace qml
