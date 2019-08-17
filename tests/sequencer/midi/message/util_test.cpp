#include <sequencer/midi/message/util.hpp>

#include <catch2/catch.hpp>

SCENARIO( "uint16_to_lsb_msb", "[system_common_message]" )
{
    using namespace sequencer::midi;

    GIVEN( "a value of 64" )
    {
        constexpr auto value = std::uint16_t{64};

        WHEN( "converted to two bytes" )
        {
            constexpr auto two_bytes = uint16_to_lsb_msb( value );

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
            constexpr auto two_bytes = uint16_to_lsb_msb( value );

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

SCENARIO( "lsb_msb_to_uint16", "[system_common_message]" )
{
    using namespace sequencer::midi;

    GIVEN( "two bytes with values 0x40 and 0x00" )
    {
        constexpr auto two_bytes = std::make_pair( std::byte{0x40}, std::byte{0x00} );

        WHEN( "converted to std::uint16_t" )
        {
            constexpr auto value = lsb_msb_to_uint16( two_bytes );

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
            constexpr auto value = lsb_msb_to_uint16( two_bytes );

            THEN( "value is 200" )
            {
                STATIC_REQUIRE( value == 200 );
            }
        }
    }
}
