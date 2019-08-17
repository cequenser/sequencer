#include <sequencer/midi/message/system_common.hpp>

#include <catch2/catch.hpp>

SCENARIO( "system common messages", "[system_common_message]" )
{
    using namespace sequencer::midi::system::common;

    GIVEN( "song pointer position message with value 3" )
    {
        const auto message = song_position_pointer( 3 );

        THEN( "message contains 3 bytes" )
        {
            REQUIRE( message.size() == 3 );
        }

        THEN( "first byte is 0xF2" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xF2} );
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

    GIVEN( "song pointer position message with value 200" )
    {
        const auto message = song_position_pointer( 200 );

        THEN( "message contains 3 bytes" )
        {
            REQUIRE( message.size() == 3 );
        }

        THEN( "first byte is 0xF2" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xF2} );
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

    GIVEN( "tune request message" )
    {
        const auto message = tune_request();
        STATIC_REQUIRE( message.size() == 1 );

        THEN( "byte is 0xF6" )
        {
            REQUIRE( message.front() == std::byte{0xF6} );
        }
    }

    GIVEN( "song select message with value 73" )
    {
        const auto message = song_select( 73 );
        STATIC_REQUIRE( message.size() == 2 );

        THEN( "first byte is 0xF3" )
        {
            REQUIRE( message.front() == std::byte{0xF3} );
        }

        THEN( "second byte is 0x49" )
        {
            REQUIRE( message.back() == std::byte{0x49} );
        }
    }
}
