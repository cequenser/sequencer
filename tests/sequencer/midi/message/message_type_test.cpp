#include <sequencer/midi/message/message_type.hpp>

#include <catch2/catch.hpp>

SCENARIO( "message_type", "[message_type]" )
{
    using sequencer::midi::message_type;

    GIVEN( "an empty midi message" )
    {
        const auto message = message_type{};

        THEN( "empty() returns true" )
        {
            REQUIRE( message.empty() );
        }

        THEN( "message() returns empty vector" )
        {
            REQUIRE( message.message().empty() );
        }

        THEN( "begin() equals end()" )
        {
            REQUIRE( message.begin() == message.end() );
        }
    }

    GIVEN( "a midi message with two bytes 0xF0 and 0x42" )
    {
        const std::vector< std::byte > byte_array = {std::byte{0xF0}, std::byte{0x42}};
        const auto message = message_type{byte_array};

        THEN( "empty() returns false" )
        {
            REQUIRE( message.empty() == false );
        }

        THEN( "size() returns 2" )
        {
            REQUIRE( message.size() == 2u );
        }

        THEN( "front() returns std::byte{0xF0}" )
        {
            REQUIRE( message.front() == std::byte{0xF0} );
        }

        THEN( "message[0] returns std::byte{0xF0}" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xF0} );
        }

        THEN( "message[1] returns std::byte{0x42}" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x42} );
        }

        THEN( "message() returns given vector" )
        {
            REQUIRE( message.message() == byte_array );
        }

        THEN( "*begin() returns std::byte{0xF0}" )
        {
            REQUIRE( *message.begin() == std::byte{0xF0} );
        }

        THEN( "*(++begin()) returns std::byte{0x42}" )
        {
            REQUIRE( *( ++message.begin() ) == std::byte{0x42} );
        }
    }
}
