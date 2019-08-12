#include <sequencer/midi/message/to_string.hpp>

#include <catch2/catch.hpp>

#include <sstream>

using sequencer::midi::message_type;
using sequencer::midi::channel::mode::all_notes_off;
using sequencer::midi::channel::mode::all_sounds_off;
using sequencer::midi::channel::mode::omni_mode_off;
using sequencer::midi::channel::mode::reset_all_controllers;
using sequencer::midi::channel::voice::note_off;
using sequencer::midi::channel::voice::note_on;
using sequencer::midi::system::common::song_position_pointer;

SCENARIO( "midi message to string", "[to_string]" )
{
    // system::common
    GIVEN( "song pointer position message with value 3" )
    {
        const auto message = song_position_pointer( 3 );

        THEN( "to_string returns 'spp:3'" )
        {
            REQUIRE( to_string( message ) == "spp:3" );
        }
    }

    GIVEN( "song pointer position message with value 200" )
    {
        const auto message = song_position_pointer( 200 );

        THEN( "to_string returns 'spp:200'" )
        {
            REQUIRE( to_string( message ) == "spp:200" );
        }
    }

    // channel::voice
    GIVEN( "a note on message for channel 1 with value 40 and velocity 73" )
    {
        const auto channel = 1;
        const auto note = 40;
        const auto velocity = 73;
        const auto message = note_on( channel, note, velocity );

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
        const auto message = note_off( channel, note, velocity );

        THEN( "to_string returns 'note_off:1:40:73'" )
        {
            REQUIRE( to_string( message ) == "note_off:1:40:73" );
        }
    }

    // channel::mode
    GIVEN( "an all sound off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = all_sounds_off( channel );

        THEN( "to_string returns 'all_sounds_off:1'" )
        {
            REQUIRE( to_string( message ) == "all_sounds_off:1" );
        }
    }

    GIVEN( "a reset all controllers message for channel 1" )
    {
        const auto channel = 1;
        const auto message = reset_all_controllers( channel );

        THEN( "to_string returns 'reset_all_controllers:1'" )
        {
            REQUIRE( to_string( message ) == "reset_all_controllers:1" );
        }
    }

    GIVEN( "an all notes off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = all_notes_off( channel );

        THEN( "to_string returns 'all_notes_off:1'" )
        {
            REQUIRE( to_string( message ) == "all_notes_off:1" );
        }
    }

    GIVEN( "an omni mode off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = omni_mode_off( channel );

        THEN( "to_string returns 'omni_mode_off:1'" )
        {
            REQUIRE( to_string( message ) == "omni_mode_off:1" );
        }
    }

    // default
    GIVEN( "empty message" )
    {
        const auto message = message_type{{}};

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
        const auto message = song_position_pointer( 3 );

        THEN( "stream operator writes 'spp:3'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "spp:3" );
        }
    }

    GIVEN( "song pointer position message with value 200" )
    {
        const auto message = song_position_pointer( 200 );

        THEN( "stream operator writes 'spp:200'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "spp:200" );
        }
    }

    // channel::voice
    GIVEN( "a note on message for channel 1 with value 40 and velocity 73" )
    {
        const auto channel = 1;
        const auto note = 40;
        const auto velocity = 73;
        const auto message = note_on( channel, note, velocity );

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
        const auto message = note_off( channel, note, velocity );

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
        const auto message = all_sounds_off( channel );

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
        const auto message = reset_all_controllers( channel );

        THEN( "stream operator writes 'reset_all_controllers:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "reset_all_controllers:1" );
        }
    }

    GIVEN( "an all notes off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = all_notes_off( channel );

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
        const auto message = omni_mode_off( channel );

        THEN( "stream operator writes 'omni_mode_off:1'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "omni_mode_off:1" );
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
