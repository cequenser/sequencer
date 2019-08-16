#include <sequencer/midi/message/to_string.hpp>

#include <catch2/catch.hpp>

#include <sstream>

using sequencer::midi::make_message;
using sequencer::midi::message_type;
using sequencer::midi::channel::mode::all_notes_off;
using sequencer::midi::channel::mode::all_sounds_off;
using sequencer::midi::channel::mode::local_control;
using sequencer::midi::channel::mode::omni_mode_off;
using sequencer::midi::channel::mode::omni_mode_on;
using sequencer::midi::channel::mode::poly_mode_on;
using sequencer::midi::channel::mode::reset_all_controllers;
using sequencer::midi::channel::voice::note_off;
using sequencer::midi::channel::voice::note_on;
using sequencer::midi::control_change::damper_pedal;
using sequencer::midi::control_change::hold_2;
using sequencer::midi::control_change::portamento;
using sequencer::midi::control_change::soft_pedal;
using sequencer::midi::control_change::sostenuto;
using sequencer::midi::system::common::end_system_exclusive;
using sequencer::midi::system::common::song_position_pointer;
using sequencer::midi::system::common::song_select;
using sequencer::midi::system::common::tune_request;

SCENARIO( "midi message to string", "[to_string]" )
{
    // system::common
    GIVEN( "song pointer position message with value 3" )
    {
        const auto message = make_message( song_position_pointer( 3 ) );

        THEN( "to_string returns 'spp:3'" )
        {
            REQUIRE( to_string( message ) == "spp:3" );
        }
    }

    GIVEN( "song pointer position message with value 200" )
    {
        const auto message = make_message( song_position_pointer( 200 ) );

        THEN( "to_string returns 'spp:200'" )
        {
            REQUIRE( to_string( message ) == "spp:200" );
        }
    }

    GIVEN( "song select message with value 42" )
    {
        const auto message = make_message( song_select( 42 ) );

        THEN( "to_string returns 'song_select:42'" )
        {
            REQUIRE( to_string( message ) == "song_select:42" );
        }
    }

    GIVEN( "tune request message" )
    {
        const auto message = make_message( tune_request() );

        THEN( "to_string returns 'tune_request'" )
        {
            REQUIRE( to_string( message ) == "tune_request" );
        }
    }

    GIVEN( "end system exclusive message" )
    {
        const auto message = make_message( end_system_exclusive() );

        THEN( "to_string returns 'end_sysex'" )
        {
            REQUIRE( to_string( message ) == "end_sysex" );
        }
    }

    // channel::voice
    GIVEN( "a note on message for channel 1 with value 40 and velocity 73" )
    {
        const auto channel = 1;
        const auto note = 40;
        const auto velocity = 73;
        const auto message = make_message( note_on( channel, note, velocity ) );

        THEN( "to_string returns 'note_on:1:40:73'" )
        {
            REQUIRE( to_string( message ) == "note_on:1:40:73" );
        }
    }

    GIVEN( "a note off message for channel 1 with value 40 and velocity 73" )
    {
        const auto channel = 1;
        const auto note = 40;
        const auto velocity = 73;
        const auto message = make_message( note_off( channel, note, velocity ) );

        THEN( "to_string returns 'note_off:1:40:73'" )
        {
            REQUIRE( to_string( message ) == "note_off:1:40:73" );
        }
    }

    // channel::mode
    GIVEN( "an all sound off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( all_sounds_off( channel ) );

        THEN( "to_string returns 'all_sounds_off:1'" )
        {
            REQUIRE( to_string( message ) == "all_sounds_off:1" );
        }
    }

    GIVEN( "a reset all controllers message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( reset_all_controllers( channel ) );

        THEN( "to_string returns 'reset_all_controllers:1'" )
        {
            REQUIRE( to_string( message ) == "reset_all_controllers:1" );
        }
    }

    GIVEN( "a local control on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( local_control( channel, on ) );

        THEN( "to_string returns 'local_control_on:1'" )
        {
            REQUIRE( to_string( message ) == "local_control_on:1" );
        }
    }

    GIVEN( "a local control off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( local_control( channel, on ) );

        THEN( "to_string returns 'local_control_off:1'" )
        {
            REQUIRE( to_string( message ) == "local_control_off:1" );
        }
    }

    GIVEN( "an all notes off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( all_notes_off( channel ) );

        THEN( "to_string returns 'all_notes_off:1'" )
        {
            REQUIRE( to_string( message ) == "all_notes_off:1" );
        }
    }

    GIVEN( "an omni mode off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( omni_mode_off( channel ) );

        THEN( "to_string returns 'omni_mode_off:1'" )
        {
            REQUIRE( to_string( message ) == "omni_mode_off:1" );
        }
    }

    GIVEN( "an omni mode on message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( omni_mode_on( channel ) );

        THEN( "to_string returns 'omni_mode_on:1'" )
        {
            REQUIRE( to_string( message ) == "omni_mode_on:1" );
        }
    }

    GIVEN( "a poly mode on message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( poly_mode_on( channel ) );

        THEN( "to_string returns 'poly_mode_on:1'" )
        {
            REQUIRE( to_string( message ) == "poly_mode_on:1" );
        }
    }

    // control change
    GIVEN( "a damper pedal on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( damper_pedal( channel, on ) );

        THEN( "to_string returns 'damper_pedal_on:1'" )
        {
            REQUIRE( to_string( message ) == "damper_pedal_on:1" );
        }
    }

    GIVEN( "a damper pedal off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( damper_pedal( channel, on ) );

        THEN( "to_string returns 'damper_pedal_off:1'" )
        {
            REQUIRE( to_string( message ) == "damper_pedal_off:1" );
        }
    }

    GIVEN( "a portamento on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( portamento( channel, on ) );

        THEN( "to_string returns 'portamento_on:1'" )
        {
            REQUIRE( to_string( message ) == "portamento_on:1" );
        }
    }

    GIVEN( "a portamento off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( portamento( channel, on ) );

        THEN( "to_string returns 'portamento_off:1'" )
        {
            REQUIRE( to_string( message ) == "portamento_off:1" );
        }
    }

    GIVEN( "a sostenuto on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( sostenuto( channel, on ) );

        THEN( "to_string returns 'sostenuto_on:1'" )
        {
            REQUIRE( to_string( message ) == "sostenuto_on:1" );
        }
    }

    GIVEN( "a sostenuto off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( sostenuto( channel, on ) );

        THEN( "to_string returns 'sostenuto_off:1'" )
        {
            REQUIRE( to_string( message ) == "sostenuto_off:1" );
        }
    }

    GIVEN( "a soft pedal on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( soft_pedal( channel, on ) );

        THEN( "to_string returns 'soft_pedal_on:1'" )
        {
            REQUIRE( to_string( message ) == "soft_pedal_on:1" );
        }
    }

    GIVEN( "a soft pedal off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( soft_pedal( channel, on ) );

        THEN( "to_string returns 'soft_pedal_off:1'" )
        {
            REQUIRE( to_string( message ) == "soft_pedal_off:1" );
        }
    }

    GIVEN( "a hold 2 on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( hold_2( channel, on ) );

        THEN( "to_string returns 'hold_2_on:1'" )
        {
            REQUIRE( to_string( message ) == "hold_2_on:1" );
        }
    }

    GIVEN( "a hold 2 off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( hold_2( channel, on ) );

        THEN( "to_string returns 'hold_2_off:1'" )
        {
            REQUIRE( to_string( message ) == "hold_2_off:1" );
        }
    }

    // default
    GIVEN( "empty message" )
    {
        const auto message = message_type{};

        THEN( "to_string returns 'invalid'" )
        {
            REQUIRE( to_string( message ) == sequencer::midi::invalid_string );
        }
    }

    GIVEN( "invalid message" )
    {
        const auto message = message_type{{std::byte{0x00}}};

        THEN( "to_string returns 'invalid'" )
        {
            REQUIRE( to_string( message ) == sequencer::midi::invalid_string );
        }
    }
}

SCENARIO( "midi message streaming", "[midi_message_stream]" )
{
    // system::common
    GIVEN( "song pointer position message with value 3" )
    {
        const auto message = make_message( song_position_pointer( 3 ) );

        THEN( "stream operator writes 'spp:3'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "spp:3" );
        }
    }

    GIVEN( "song pointer position message with value 200" )
    {
        const auto message = make_message( song_position_pointer( 200 ) );

        THEN( "stream operator writes 'spp:200'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "spp:200" );
        }
    }
    GIVEN( "song select message with value 42" )
    {
        const auto message = make_message( song_select( 42 ) );

        THEN( "stream operator writes 'song_select:42'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "song_select:42" );
        }
    }

    GIVEN( "tune request message" )
    {
        const auto message = make_message( tune_request() );

        THEN( "stream operator writes 'tune_request'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "tune_request" );
        }
    }

    GIVEN( "end system exclusive message" )
    {
        const auto message = make_message( end_system_exclusive() );

        THEN( "stream operator writes 'end_sysex'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "end_sysex" );
        }
    }

    // channel::voice
    GIVEN( "a note on message for channel 1 with value 40 and velocity 73" )
    {
        const auto channel = 1;
        const auto note = 40;
        const auto velocity = 73;
        const auto message = make_message( note_on( channel, note, velocity ) );

        THEN( "stream operator writes 'note_on:1:40:73'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "note_on:1:40:73" );
        }
    }

    GIVEN( "a note off message for channel 1 with value 40 and velocity 73" )
    {
        const auto channel = 1;
        const auto note = 40;
        const auto velocity = 73;
        const auto message = make_message( note_off( channel, note, velocity ) );

        THEN( "stream operator writes 'note_off:1:40:73'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "note_off:1:40:73" );
        }
    }

    // channel::mode
    GIVEN( "an all sound off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( all_sounds_off( channel ) );

        THEN( "stream operator writes 'all_sounds_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "all_sounds_off:1" );
        }
    }

    GIVEN( "a reset all controllers message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( reset_all_controllers( channel ) );

        THEN( "stream operator writes 'reset_all_controllers:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "reset_all_controllers:1" );
        }
    }

    GIVEN( "a local control on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( local_control( channel, on ) );

        THEN( "stream operator writes 'local_control_on:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "local_control_on:1" );
        }
    }

    GIVEN( "a local control off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( local_control( channel, on ) );

        THEN( "stream operator writes 'local_control_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "local_control_off:1" );
        }
    }

    GIVEN( "an all notes off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( all_notes_off( channel ) );

        THEN( "stream operator writes 'all_notes_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "all_notes_off:1" );
        }
    }

    GIVEN( "an omni mode off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( omni_mode_off( channel ) );

        THEN( "stream operator writes 'omni_mode_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "omni_mode_off:1" );
        }
    }

    GIVEN( "an omni mode on message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( omni_mode_on( channel ) );

        THEN( "stream operator writes 'omni_mode_on:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "omni_mode_on:1" );
        }
    }

    GIVEN( "a poly mode on message for channel 1" )
    {
        const auto channel = 1;
        const auto message = make_message( poly_mode_on( channel ) );

        THEN( "stream operator writes 'poly_mode_on:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "poly_mode_on:1" );
        }
    }

    // control change
    GIVEN( "a damper pedal on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( damper_pedal( channel, on ) );

        THEN( "stream operator writes 'damper_pedal_on:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "damper_pedal_on:1" );
        }
    }

    GIVEN( "a damper pedal off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( damper_pedal( channel, on ) );

        THEN( "stream operator writes 'damper_pedal_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "damper_pedal_off:1" );
        }
    }

    GIVEN( "a portamento on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( portamento( channel, on ) );

        THEN( "stream operator writes 'portamento_on:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "portamento_on:1" );
        }
    }

    GIVEN( "a portamento off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( portamento( channel, on ) );

        THEN( "stream operator writes 'portamento_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "portamento_off:1" );
        }
    }

    GIVEN( "a sostenuto on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( sostenuto( channel, on ) );

        THEN( "stream operator writes 'sostenuto_on:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "sostenuto_on:1" );
        }
    }

    GIVEN( "a sostenuto off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( sostenuto( channel, on ) );

        THEN( "stream operator writes 'sostenuto_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "sostenuto_off:1" );
        }
    }

    GIVEN( "a soft pedal on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( soft_pedal( channel, on ) );

        THEN( "stream operator writes 'soft_pedal_on:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "soft_pedal_on:1" );
        }
    }

    GIVEN( "a soft pedal off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( soft_pedal( channel, on ) );

        THEN( "stream operator writes 'soft_pedal_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "soft_pedal_off:1" );
        }
    }

    GIVEN( "a hold 2 on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = make_message( hold_2( channel, on ) );

        THEN( "stream operator writes 'hold_2_on:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "hold_2_on:1" );
        }
    }

    GIVEN( "a hold 2 off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = make_message( hold_2( channel, on ) );

        THEN( "stream operator writes 'hold_2_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "hold_2_off:1" );
        }
    }

    // default
    GIVEN( "empty message" )
    {
        const auto message = message_type{{}};

        THEN( "stream operator writes 'invalid'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == sequencer::midi::invalid_string );
        }
    }

    GIVEN( "invalid message" )
    {
        const auto message = message_type{{std::byte{0x00}}};

        THEN( "to_string returns 'invalid'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == sequencer::midi::invalid_string );
        }
    }
}
