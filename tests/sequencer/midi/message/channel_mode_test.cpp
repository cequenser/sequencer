#include <sequencer/midi/message/channel_mode.hpp>

#include <catch2/catch.hpp>

using sequencer::midi::channel::mode::all_notes_off;
using sequencer::midi::channel::mode::all_sounds_off;
using sequencer::midi::channel::mode::omni_mode_off;
using sequencer::midi::channel::mode::omni_mode_on;
using sequencer::midi::channel::mode::reset_all_controllers;

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

    GIVEN( "a reset all controllers message for channel 1" )
    {
        const auto channel = 1;
        const auto message = reset_all_controllers( channel );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x79" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x79} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }

    GIVEN( "an all notes off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = all_notes_off( channel );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x7B" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x7B} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }

    GIVEN( "an omni mode off message for channel 1" )
    {
        const auto channel = 1;
        const auto message = omni_mode_off( channel );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x7C" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x7C} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }

    GIVEN( "an omni mode on message for channel 1" )
    {
        const auto channel = 1;
        const auto message = omni_mode_on( channel );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x7D" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x7D} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }
}
