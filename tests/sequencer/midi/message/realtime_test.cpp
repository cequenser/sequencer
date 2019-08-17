#include <sequencer/midi/message/realtime.hpp>

#include <catch2/catch.hpp>

SCENARIO( "realtime messages", "[realtime_message]" )
{
    using namespace sequencer::midi::realtime;

    GIVEN( "realtime clock message" )
    {
        const auto message = realtime_clock();

        THEN( "its value equals 0xF8" )
        {
            REQUIRE( message.front() == std::byte{0xF8} );
        }
    }

    GIVEN( "realtime start message" )
    {
        const auto message = realtime_start();

        THEN( "its value equals 0xFA" )
        {
            REQUIRE( message.front() == std::byte{0xFA} );
        }
    }

    GIVEN( "realtime continue message" )
    {
        const auto message = realtime_continue();

        THEN( "its value equals 0xFB" )
        {
            REQUIRE( message.front() == std::byte{0xFB} );
        }
    }

    GIVEN( "realtime stop message" )
    {
        const auto message = realtime_stop();

        THEN( "its value equals 0xFC" )
        {
            REQUIRE( message.front() == std::byte{0xFC} );
        }
    }
}
