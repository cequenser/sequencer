#include <sequencer/midi/message_type.hpp>

#include <catch2/catch.hpp>

#include <sstream>

SCENARIO( "message types to string", "[message_type]" )
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
