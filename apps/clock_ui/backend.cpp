#include "backend.hpp"

#include <sequencer/midi/percussion_key.hpp>

#include <cassert>
#include <cstdint>
#include <iostream>

using sequencer::midi::no_note;
using sequencer::midi::note_t;
using sequencer::midi::percussion_key;

namespace qml
{
    backend::backend()
        : midiout_{RtMidiOut()}, tracks_{sequencer::midi::make_tracks( number_of_tracks,
                                                                       number_of_steps )},
          clock_{sequencer::rtmidi::make_clock()},
          clock_done_{start_clock_in_thread( clock_, [this]( auto message ) {
              midiout_.sendMessage(
                  static_cast< const unsigned char* >( static_cast< const void* >( &message ) ),
                  1 );
              send_messages( tracks_, message, sequencer::rtmidi::message_sender{midiout_} );
          } )}
    {
        track_notes_.fill( note_t( min_note() ) );
    }

    backend::~backend()
    {
        send_all_notes_off_message( tracks_, sequencer::rtmidi::message_sender{midiout_} );
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

    void backend::set_step( int step, bool checked )
    {
        const auto new_note = checked ? track_notes_[ current_track_ ] : no_note();
        tracks_[ current_track_ ][ tracks_t::size_type( step ) ] = new_note;
    }

    bool backend::is_step_set( int step ) const
    {
        return tracks_[ current_track_ ][ step ] != no_note();
    }

    void backend::set_current_track( int track )
    {
        assert( track < number_of_tracks );
        current_track_ = std::uint8_t( track );
    }

    int backend::min_note() const
    {
        return static_cast< std::uint8_t >( percussion_key::AcousticBassDrum );
    }

    int backend::max_note() const
    {
        return static_cast< std::uint8_t >( percussion_key::OpenTriangle );
    }

    QString backend::note_to_string( int note ) const
    {
        assert( note >= min_note() );
        assert( note <= max_note() );
        return QString( to_string( static_cast< percussion_key >( std::uint8_t( note ) ) ).data() );
    }

    QString backend::notes_to_string() const
    {
        QString str;
        for ( auto note = min_note(); note <= max_note(); ++note )
        {
            str.append( note_to_string( note ) );
            if ( note != max_note() )
            {
                str.append( ";" );
            }
        }
        return str;
    }

    void backend::set_note_of_current_track( int note )
    {
        track_notes_[ current_track_ ] = note_t( min_note() + note );
        for ( auto step = 0; step < number_of_steps; ++step )
        {
            if ( is_step_set( step ) )
            {
                set_step( step, true );
            }
        }
    }
} // namespace qml