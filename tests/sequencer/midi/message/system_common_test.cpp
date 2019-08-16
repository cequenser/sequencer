#include <sequencer/midi/message/system_common.hpp>

#include <catch2/catch.hpp>

SCENARIO( "uint16_to_two_bytes", "[system_common_message]" )
{
    using namespace sequencer::midi;

    GIVEN( "a value of 64" )
    {
        constexpr auto value = std::uint16_t{64};

        WHEN( "converted to two bytes" )
        {
            constexpr auto two_bytes = uint16_to_two_bytes( value );

            THEN( "first byte is 0x40" )
            {
                STATIC_REQUIRE( two_bytes.first == std::byte{0x40} );
            }

            THEN( "second byte is 0x00" )
            {
                STATIC_REQUIRE( two_bytes.second == std::byte{0x00} );
            }
        }
    }

    GIVEN( "a value of 200" )
    {
        constexpr auto value = std::uint16_t{200};

        WHEN( "converted to two bytes" )
        {
            constexpr auto two_bytes = uint16_to_two_bytes( value );

            THEN( "first byte is 0x48" )
            {
                STATIC_REQUIRE( two_bytes.first == std::byte{0x48} );
            }

            THEN( "second byte is 0x01" )
            {
                STATIC_REQUIRE( two_bytes.second == std::byte{0x01} );
            }
        }
    }
}

SCENARIO( "two_bytes_to_uint16", "[system_common_message]" )
{
    using namespace sequencer::midi;

    GIVEN( "two bytes with values 0x40 and 0x00" )
    {
        constexpr auto two_bytes = std::make_pair( std::byte{0x40}, std::byte{0x00} );

        WHEN( "converted to std::uint16_t" )
        {
            constexpr auto value = two_bytes_to_uint16( two_bytes );

            THEN( "value is 64" )
            {
                STATIC_REQUIRE( value == 64 );
            }
        }
    }

    GIVEN( "two bytes with values 0x48 and 0x01" )
    {
        constexpr auto two_bytes = std::make_pair( std::byte{0x48}, std::byte{0x01} );

        WHEN( "converted to std::uint16_t" )
        {
            constexpr auto value = two_bytes_to_uint16( two_bytes );

            THEN( "value is 200" )
            {
                STATIC_REQUIRE( value == 200 );
            }
        }
    }
}

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

    GIVEN( "end system exclusive message" )
    {
        const auto message = end_system_exclusive();
        STATIC_REQUIRE( message.size() == 1 );

        THEN( "byte is 0xF7" )
        {
            REQUIRE( message.front() == std::byte{0xF7} );
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
