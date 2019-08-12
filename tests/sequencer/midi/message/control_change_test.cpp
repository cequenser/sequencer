#include <sequencer/midi/message/control_change.hpp>

#include <catch2/catch.hpp>

using sequencer::midi::control_change::damper_pedal;

SCENARIO( "control change messages", "[control_change_message]" )
{
    GIVEN( "a damper pedal on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = damper_pedal( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x40" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x40} );
        }

        THEN( "third byte is 0x7F" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x7F} );
        }
    }

    GIVEN( "a damper pedal off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = damper_pedal( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x40" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x40} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }
}
