#include <sequencer/midi/message/realtime.hpp>

#include <catch2/catch.hpp>

SCENARIO( "realtime messages", "[realtime_message]" )
{
    using namespace sequencer::midi;
    using namespace sequencer::midi::realtime;

    GIVEN( "realtime clock message" )
    {
        const auto message = realtime_clock();

        THEN( "its value equals 0xF8" )
        {
            REQUIRE( message.front() == byte::realtime_clock );
        }
    }

    GIVEN( "realtime start message" )
    {
        const auto message = realtime_start();

        THEN( "its value equals 0xFA" )
        {
            REQUIRE( message.front() == byte::realtime_start );
        }
    }

    GIVEN( "realtime continue message" )
    {
        const auto message = realtime_continue();

        THEN( "its value equals 0xFB" )
        {
            REQUIRE( message.front() == byte::realtime_continue );
        }
    }

    GIVEN( "realtime stop message" )
    {
        const auto message = realtime_stop();

        THEN( "its value equals 0xFC" )
        {
            REQUIRE( message.front() == byte::realtime_stop );
        }
    }

    GIVEN( "realtime active sensing message" )
    {
        const auto message = active_sensing();

        THEN( "its value equals 0xFE" )
        {
            REQUIRE( message.front() == byte::active_sensing );
        }
    }

    GIVEN( "realtime reset all message" )
    {
        const auto message = reset_all();

        THEN( "its value equals 0xFF" )
        {
            REQUIRE( message.front() == byte::reset_all );
        }
    }
}
