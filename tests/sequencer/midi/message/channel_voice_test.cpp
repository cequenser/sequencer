#include <sequencer/midi/message/channel_voice.hpp>

#include <catch2/catch.hpp>

SCENARIO( "channel voice messages", "[channel_voice_message]" )
{
    using namespace sequencer::midi::channel::voice;
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

    GIVEN( "a polymorphic key pressure message for channel 15 with value 37 and velocity 0" )
    {
        const auto channel = 15;
        const auto key = 47;
        const auto pressure = 3;
        const auto message = polymorphic_key_pressure( channel, key, pressure );

        THEN( "the first byte is 0xAF" )
        {
            CHECK( message[ 0 ] == std::byte{0xAF} );
        }

        THEN( "the second byte is 0x2F" )
        {
            CHECK( message[ 1 ] == std::byte{0x2F} );
        }

        THEN( "the third byte is 0x03" )
        {
            CHECK( message[ 2 ] == std::byte{0x03} );
        }
    }

    GIVEN( "pitch bend change message with value 3" )
    {
        const auto channel = 15;
        const auto message = pitch_bend_change( channel, 3 );

        THEN( "message contains 3 bytes" )
        {
            REQUIRE( message.size() == 3 );
        }

        THEN( "first byte is 0xEF" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xEF} );
        }

        THEN( "second byte is 0x03" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x03} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }

    GIVEN( "pitch bend change message with value 200" )
    {
        const auto channel = 15;
        const auto message = pitch_bend_change( channel, 200 );

        THEN( "message contains 3 bytes" )
        {
            REQUIRE( message.size() == 3 );
        }

        THEN( "first byte is 0xEF" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xEF} );
        }

        THEN( "second byte is 0x48" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x48} );
        }

        THEN( "third byte is 0x01" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x01} );
        }
    }
}
