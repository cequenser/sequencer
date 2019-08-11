#include <sequencer/midi/message_type.hpp>

#include <catch2/catch.hpp>

#include <sstream>

SCENARIO( "real time messages to string", "[message_type]" )
{
    using namespace sequencer;

    GIVEN( "message_type::realtime_clock" )
    {
        const auto message = midi::message_type::realtime_clock;

        THEN( "to_string returns 'clock'" )
        {
            REQUIRE( to_string( message ) == "clock" );
        }

        THEN( "stream operator writes 'clock'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "clock" );
        }
    }

    GIVEN( "message_type::realtime_start" )
    {
        const auto message = midi::message_type::realtime_start;

        THEN( "to_string returns 'start'" )
        {
            REQUIRE( to_string( message ) == "start" );
        }

        THEN( "stream operator writes 'start'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "start" );
        }
    }

    GIVEN( "message_type::realtime_stop" )
    {
        const auto message = midi::message_type::realtime_stop;

        THEN( "to_string returns 'stop'" )
        {
            REQUIRE( to_string( message ) == "stop" );
        }

        THEN( "stream operator writes 'stop'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "stop" );
        }
    }

    GIVEN( "message_type::realtime_continue" )
    {
        const auto message = midi::message_type::realtime_continue;

        THEN( "to_string returns 'continue'" )
        {
            REQUIRE( to_string( message ) == "continue" );
        }

        THEN( "stream operator writes 'continue'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "continue" );
        }
    }

    GIVEN( "message_type::invalid" )
    {
        const auto message = midi::message_type::invalid;

        THEN( "to_string returns 'invalid'" )
        {
            REQUIRE( to_string( message ) == "invalid" );
        }

        THEN( "stream operator writes 'invalid'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "invalid" );
        }
    }
}

SCENARIO( "system common messages to string", "[message_type]" )
{
    using namespace sequencer;

    GIVEN( "song pointer position message with value 3" )
    {
        const auto message = midi::system::common::song_position_pointer( 3 );

        THEN( "message contains 3 bytes" )
        {
            REQUIRE( message.size() == 3 );
        }

        THEN( "first byte is 0xF2" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xF2} );
        }

        THEN( "second byte is 0x03" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x03} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }

        THEN( "to_string returns 'spp@3'" )
        {
            REQUIRE( to_string( message ) == "spp@3" );
        }

        THEN( "stream operator writes 'spp@3'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "spp@3" );
        }
    }

    GIVEN( "song pointer position message with value 200" )
    {
        const auto message = midi::system::common::song_position_pointer( 200 );

        THEN( "message contains 3 bytes" )
        {
            REQUIRE( message.size() == 3 );
        }

        THEN( "first byte is 0xF2" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xF2} );
        }

        THEN( "second byte is 0x48" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x48} );
        }

        THEN( "third byte is 0x01" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x01} );
        }

        THEN( "to_string returns 'spp@200'" )
        {
            REQUIRE( to_string( message ) == "spp@200" );
        }

        THEN( "stream operator writes 'spp@200'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "spp@200" );
        }
    }
}
