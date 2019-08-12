#include <sequencer/midi/message/channel_voice.hpp>

#include <catch2/catch.hpp>

using sequencer::midi::message::channel::voice::note_off;
using sequencer::midi::message::channel::voice::note_on;

SCENARIO( "channel voice messages", "[channel_voice_message]" )
{
    GIVEN( "a note on message for channel 1 with value 40 and velocity 73" )
    {
        const auto channel = 1;
        const auto note = 40;
        const auto velocity = 73;
        const auto message = note_on( channel, note, velocity );

        THEN( "the first byte is 0x91" )
        {
            CHECK( message[ 0 ] == std::byte{0x91} );
        }

        THEN( "the second byte is 0x28" )
        {
            CHECK( message[ 1 ] == std::byte{0x28} );
        }

        THEN( "the third byte is 0x49" )
        {
            CHECK( message[ 2 ] == std::byte{0x49} );
        }
    }

    GIVEN( "a note on message for channel 15 with value 37 and velocity 0" )
    {
        const auto channel = 15;
        const auto value = 47;
        const auto velocity = 0;
        const auto message = note_on( channel, value, velocity );

        THEN( "the first byte is 0x9F" )
        {
            CHECK( message[ 0 ] == std::byte{0x9F} );
        }

        THEN( "the second byte is 0x2F" )
        {
            CHECK( message[ 1 ] == std::byte{0x2F} );
        }

        THEN( "the third byte is 0x00" )
        {
            CHECK( message[ 2 ] == std::byte{0x00} );
        }
    }
    GIVEN( "a note off message for channel 1 with value 40 and velocity 73" )
    {
        const auto channel = 1;
        const auto note = 40;
        const auto velocity = 73;
        const auto message = note_off( channel, note, velocity );

        THEN( "the first byte is 0x81" )
        {
            CHECK( message[ 0 ] == std::byte{0x81} );
        }

        THEN( "the second byte is 0x28" )
        {
            CHECK( message[ 1 ] == std::byte{0x28} );
        }

        THEN( "the third byte is 0x49" )
        {
            CHECK( message[ 2 ] == std::byte{0x49} );
        }
    }

    GIVEN( "a note off message for channel 15 with value 37 and velocity 0" )
    {
        const auto channel = 15;
        const auto value = 47;
        const auto velocity = 0;
        const auto message = note_off( channel, value, velocity );

        THEN( "the first byte is 0x8F" )
        {
            CHECK( message[ 0 ] == std::byte{0x8F} );
        }

        THEN( "the second byte is 0x2F" )
        {
            CHECK( message[ 1 ] == std::byte{0x2F} );
        }

        THEN( "the third byte is 0x00" )
        {
            CHECK( message[ 2 ] == std::byte{0x00} );
        }
    }
}
