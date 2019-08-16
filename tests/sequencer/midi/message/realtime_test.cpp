#include <sequencer/midi/message/realtime.hpp>

#include <catch2/catch.hpp>

using sequencer::midi::realtime::message_type;

SCENARIO( "realtime messages", "[realtime_message]" )
{
    using namespace sequencer::midi::realtime;

    GIVEN( "realtime clock message" )
    {
        const auto message = message_type::realtime_clock;

        WHEN( "static_cast to unsigned char" )
        {
            const auto as_char = static_cast< unsigned char >( message );

            THEN( "its value equals 0xF8" )
            {
                REQUIRE( as_char == 0xF8 );
            }
        }
    }

    GIVEN( "realtime start message" )
    {
        const auto message = message_type::realtime_start;

        WHEN( "static_cast to unsigned char" )
        {
            const auto as_char = static_cast< unsigned char >( message );

            THEN( "its value equals 0xFA" )
            {
                REQUIRE( as_char == 0xFA );
            }
        }
    }

    GIVEN( "realtime continue message" )
    {
        const auto message = message_type::realtime_continue;

        WHEN( "static_cast to unsigned char" )
        {
            const auto as_char = static_cast< unsigned char >( message );

            THEN( "its value equals 0xFB" )
            {
                REQUIRE( as_char == 0xFB );
            }
        }
    }

    GIVEN( "realtime stop message" )
    {
        const auto message = message_type::realtime_stop;

        WHEN( "static_cast to unsigned char" )
        {
            const auto as_char = static_cast< unsigned char >( message );

            THEN( "its value equals 0xFC" )
            {
                REQUIRE( as_char == 0xFC );
            }
        }
    }
}
