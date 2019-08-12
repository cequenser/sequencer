#include <sequencer/midi/message/to_string.hpp>

#include <catch2/catch.hpp>

#include <sstream>

using sequencer::midi::message::message_type;
using sequencer::midi::message::system::common::song_position_pointer;

SCENARIO( "midi message to string", "[to_string]" )
{

    GIVEN( "song pointer position message with value 3" )
    {
        const auto message = song_position_pointer( 3 );

        THEN( "to_string returns 'spp@3'" )
        {
            REQUIRE( to_string( message ) == "spp@3" );
        }
    }

    GIVEN( "song pointer position message with value 200" )
    {
        const auto message = song_position_pointer( 200 );

        THEN( "to_string returns 'spp@200'" )
        {
            REQUIRE( to_string( message ) == "spp@200" );
        }
    }

    GIVEN( "empty message" )
    {
        const auto message = message_type{{}};

        THEN( "to_string returns 'invalid'" )
        {
            REQUIRE( to_string( message ) == sequencer::midi::message::invalid_string );
        }
    }

    GIVEN( "invalid message" )
    {
        const auto message = message_type{{std::byte{0x00}}};

        THEN( "to_string returns 'invalid'" )
        {
            REQUIRE( to_string( message ) == sequencer::midi::message::invalid_string );
        }
    }
}

SCENARIO( "midi message streaming", "[midi_message_stream]" )
{
    GIVEN( "song pointer position message with value 3" )
    {
        const auto message = song_position_pointer( 3 );

        THEN( "stream operator writes 'spp@3'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "spp@3" );
        }
    }

    GIVEN( "song pointer position message with value 200" )
    {
        const auto message = song_position_pointer( 200 );

        THEN( "stream operator writes 'spp@200'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "spp@200" );
        }
    }

    GIVEN( "empty message" )
    {
        const auto message = message_type{{}};

        THEN( "stream operator writes 'invalid'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == sequencer::midi::message::invalid_string );
        }
    }

    GIVEN( "invalid message" )
    {
        const auto message = message_type{{std::byte{0x00}}};

        THEN( "to_string returns 'invalid'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == sequencer::midi::message::invalid_string );
        }
    }
}
