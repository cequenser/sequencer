#include <sequencer/midi/message/channel_mode.hpp>

#include <catch2/catch.hpp>

using sequencer::midi::channel::mode::all_sounds_off;

SCENARIO( "channel mode messages", "[channel_mode_message" )
{
    GIVEN( "an all sounds off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = all_sounds_off( channel );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x78" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x78} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }
}
