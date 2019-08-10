#include <sequencer/midi/message_type.hpp>

#include <catch2/catch.hpp>

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
    }

    GIVEN( "message_type::realtime_start" )
    {
        const auto message = midi::message_type::realtime_start;

        THEN( "to_string returns 'start'" )
        {
            REQUIRE( to_string( message ) == "start" );
        }
    }

    GIVEN( "message_type::realtime_stop" )
    {
        const auto message = midi::message_type::realtime_stop;

        THEN( "to_string returns 'stop'" )
        {
            REQUIRE( to_string( message ) == "stop" );
        }
    }

    GIVEN( "message_type::realtime_continue" )
    {
        const auto message = midi::message_type::realtime_continue;

        THEN( "to_string returns 'continue'" )
        {
            REQUIRE( to_string( message ) == "continue" );
        }
    }

    GIVEN( "message_type::invalid" )
    {
        const auto message = midi::message_type::invalid;

        THEN( "to_string returns 'invalid'" )
        {
            REQUIRE( to_string( message ) == "invalid" );
        }
    }
}
