#include <sequencer/midi/message/system_exclusive.hpp>

#include <catch2/catch.hpp>

SCENARIO( "system exclusive messages", "[system_exclusive_message]" )
{
    using namespace sequencer::midi::system::exclusive;

    GIVEN( "end system exclusive message" )
    {
        const auto message = end_system_exclusive();
        STATIC_REQUIRE( message.size() == 1 );

        THEN( "message is 0xF7" )
        {
            REQUIRE( message.front() == std::byte{0xF7} );
        }
    }

    GIVEN( "a sysex message for manufacturer 65 with values 1 and 52" )
    {
        const auto message = system_exclusive( 65, 1, 52 );
        REQUIRE( message.size() == 5 );

        THEN( "message is 0xF0 0x41 0x01 0x34 0xF7" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xF0} );
            REQUIRE( message[ 1 ] == std::byte{0x41} );
            REQUIRE( message[ 2 ] == std::byte{0x01} );
            REQUIRE( message[ 3 ] == std::byte{0x34} );
            REQUIRE( message[ 4 ] == std::byte{0xF7} );
        }
    }
}

SCENARIO( "system exclusive realtime messages", "[system_exclusive_message]" )
{
    using namespace sequencer::midi::system::exclusive;
    using namespace sequencer::midi::system::exclusive::realtime;

    GIVEN( "a sysex realtime message for manufacturer 65 with values 1 and 52" )
    {
        const auto message = realtime::system_exclusive( 65, 1, 52 );
        REQUIRE( message.size() == 6 );

        THEN( "message is 0xF0 0x7F 0x41 0x01 0x34 0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == std::byte{0x01} );
            REQUIRE( message[ 4 ] == std::byte{0x34} );
            REQUIRE( message[ 5 ] == end_byte );
        }
    }

    GIVEN( "a sysex master volume message for manufacturer 65 with value 200" )
    {
        const auto message = master_volume( 65, 200 );
        REQUIRE( message.size() == 8 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x01 0x48 0x01 "
              "0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x01} );
            CHECK( message[ 5 ] == std::byte{0x48} );
            CHECK( message[ 6 ] == std::byte{0x01} );
            CHECK( message[ 7 ] == end_byte );
        }
    }

    GIVEN( "a sysex master balance message for manufacturer 65 with value 200" )
    {
        const auto message = master_balance( 65, 200 );
        REQUIRE( message.size() == 8 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x02 0x48 0x01 "
              "0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x02} );
            CHECK( message[ 5 ] == std::byte{0x48} );
            CHECK( message[ 6 ] == std::byte{0x01} );
            CHECK( message[ 7 ] == end_byte );
        }
    }

    GIVEN( "a sysex master fine tuning message for manufacturer 65 with value 200" )
    {
        const auto message = master_fine_tuning( 65, 200 );
        REQUIRE( message.size() == 8 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x03 0x48 0x01 "
              "0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x03} );
            CHECK( message[ 5 ] == std::byte{0x48} );
            CHECK( message[ 6 ] == std::byte{0x01} );
            CHECK( message[ 7 ] == end_byte );
        }
    }

    GIVEN( "a sysex master coarse tuning message for manufacturer 65 with value 200" )
    {
        const auto message = master_coarse_tuning( 65, 200 );
        REQUIRE( message.size() == 8 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x04 0x48 0x01 "
              "0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == device_control_byte );
            REQUIRE( message[ 4 ] == std::byte{0x04} );
            REQUIRE( message[ 5 ] == std::byte{0x48} );
            REQUIRE( message[ 6 ] == std::byte{0x01} );
            REQUIRE( message[ 7 ] == end_byte );
        }
    }
}

// NOLINTNEXTLINE(readability-function-size)
SCENARIO( "system exclusive - global parameter control", "[system_exlusive_message]" )
{
    using namespace sequencer::midi::system::exclusive;
    using namespace sequencer::midi::system::exclusive::realtime;

    GIVEN( "a global parameter control message for manufacturer 65 with no slot path and 1-byte id "
           "2 and 1-byte value 60" )
    {
        std::array< std::pair< std::uint8_t, std::uint8_t >, 1 > id_value_pairs = {
            std::pair{std::uint8_t{2}, std::uint8_t{60}}};
        const auto message = global_parameter_control( 65, id_value_pairs );
        REQUIRE( message.size() == 11 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x05 0x00 0x01 "
              "0x01 0x02 0x3C 0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x05} );
            CHECK( message[ 5 ] == std::byte{0x00} );
            CHECK( message[ 6 ] == std::byte{0x01} );
            CHECK( message[ 7 ] == std::byte{0x01} );
            CHECK( message[ 8 ] == std::byte{0x02} );
            CHECK( message[ 9 ] == std::byte{0x3C} );
            CHECK( message[ 10 ] == end_byte );
        }
    }

    GIVEN( "a global parameter control message for manufacturer 65 with slot path of length 1 with "
           "value 200 and 1-byte id 2 and 1-byte value 60" )
    {
        std::array< std::uint16_t, 1 > slot_path = {200};
        std::array< std::pair< std::uint8_t, std::uint8_t >, 1 > id_value_pairs = {
            std::pair{std::uint8_t{2}, std::uint8_t{60}}};
        const auto message = global_parameter_control( 65, id_value_pairs, slot_path );
        REQUIRE( message.size() == 13 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x05 0x01 0x01 "
              "0x01 0x01 0x048 0x02 0x3C 0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x05} );
            CHECK( message[ 5 ] == std::byte{0x01} );
            CHECK( message[ 6 ] == std::byte{0x01} );
            CHECK( message[ 7 ] == std::byte{0x01} );
            CHECK( message[ 8 ] == std::byte{0x01} );
            CHECK( message[ 9 ] == std::byte{0x48} );
            CHECK( message[ 10 ] == std::byte{0x02} );
            CHECK( message[ 11 ] == std::byte{0x3C} );
            CHECK( message[ 12 ] == end_byte );
        }
    }

    GIVEN( "a global parameter control message for manufacturer 65 with slot path of length 2 with "
           "value 200, 10 and 1-byte id 2 and 1-byte value 60" )
    {
        std::array< std::uint16_t, 2 > slot_path = {200, 10};
        std::array< std::pair< std::uint8_t, std::uint8_t >, 1 > id_value_pairs = {
            std::pair{std::uint8_t{2}, std::uint8_t{60}}};
        const auto message = global_parameter_control( 65, id_value_pairs, slot_path );
        REQUIRE( message.size() == 15 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x05 0x02 0x01 "
              "0x01 0x01 0x048 0x00 0x0A 0x02 0x3C 0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x05} );
            CHECK( message[ 5 ] == std::byte{0x02} );
            CHECK( message[ 6 ] == std::byte{0x01} );
            CHECK( message[ 7 ] == std::byte{0x01} );
            CHECK( message[ 8 ] == std::byte{0x01} );
            CHECK( message[ 9 ] == std::byte{0x48} );
            CHECK( message[ 10 ] == std::byte{0x00} );
            CHECK( message[ 11 ] == std::byte{0x0A} );
            CHECK( message[ 12 ] == std::byte{0x02} );
            CHECK( message[ 13 ] == std::byte{0x3C} );
            CHECK( message[ 14 ] == end_byte );
        }
    }

    GIVEN( "a global parameter control message for manufacturer 65 with no slot path and 1-byte "
           "ids 2, 3 and 1-byte values 60, 70" )
    {
        std::array< std::pair< std::uint8_t, std::uint8_t >, 2 > id_value_pairs = {
            std::pair{std::uint8_t{2}, std::uint8_t{60}},
            std::pair{std::uint8_t{3}, std::uint8_t{70}}};
        const auto message = global_parameter_control( 65, id_value_pairs );
        REQUIRE( message.size() == 13 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x05 0x00 0x01 "
              "0x01 0x02 0x3C 0x03 0x46 0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x05} );
            CHECK( message[ 5 ] == std::byte{0x00} );
            CHECK( message[ 6 ] == std::byte{0x01} );
            CHECK( message[ 7 ] == std::byte{0x01} );
            CHECK( message[ 8 ] == std::byte{0x02} );
            CHECK( message[ 9 ] == std::byte{0x3C} );
            CHECK( message[ 10 ] == std::byte{0x03} );
            CHECK( message[ 11 ] == std::byte{0x46} );
            CHECK( message[ 12 ] == end_byte );
        }
    }

    GIVEN( "a global parameter control message for manufacturer 65 with no slot path and 1-byte "
           "ids 2, 3 and 2-byte values 60, 270" )
    {
        std::array< std::pair< std::uint8_t, std::uint16_t >, 2 > id_value_pairs = {
            std::pair{std::uint8_t{2}, std::uint16_t{60}},
            std::pair{std::uint8_t{3}, std::uint16_t{270}}};
        const auto message = global_parameter_control( 65, id_value_pairs );
        REQUIRE( message.size() == 15 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x05 0x00 0x01 "
              "0x02 0x02 0x3C 0x00 0x03 0x0E 0x02 0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x05} );
            CHECK( message[ 5 ] == std::byte{0x00} );
            CHECK( message[ 6 ] == std::byte{0x01} );
            CHECK( message[ 7 ] == std::byte{0x02} );
            CHECK( message[ 8 ] == std::byte{0x02} );
            CHECK( message[ 9 ] == std::byte{0x3C} );
            CHECK( message[ 10 ] == std::byte{0x00} );
            CHECK( message[ 11 ] == std::byte{0x03} );
            CHECK( message[ 12 ] == std::byte{0x0E} );
            CHECK( message[ 13 ] == std::byte{0x02} );
            CHECK( message[ 14 ] == end_byte );
        }
    }

    GIVEN( "a global parameter control message for manufacturer 65 with no slot path and 2-byte "
           "ids 200, 3 and 1-byte values 60, 70" )
    {
        std::array< std::pair< std::uint16_t, std::uint8_t >, 2 > id_value_pairs = {
            std::pair{std::uint16_t{200}, std::uint8_t{60}},
            std::pair{std::uint16_t{3}, std::uint8_t{70}}};
        const auto message = global_parameter_control( 65, id_value_pairs );
        REQUIRE( message.size() == 15 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x05 0x00 0x02 "
              "0x01 0x01 0x48 0x3C 0x00 0x03 0x46 0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x05} );
            CHECK( message[ 5 ] == std::byte{0x00} );
            CHECK( message[ 6 ] == std::byte{0x02} );
            CHECK( message[ 7 ] == std::byte{0x01} );
            CHECK( message[ 8 ] == std::byte{0x01} );
            CHECK( message[ 9 ] == std::byte{0x48} );
            CHECK( message[ 10 ] == std::byte{0x3C} );
            CHECK( message[ 11 ] == std::byte{0x00} );
            CHECK( message[ 12 ] == std::byte{0x03} );
            CHECK( message[ 13 ] == std::byte{0x46} );
            CHECK( message[ 14 ] == end_byte );
        }
    }

    GIVEN( "a global parameter control message for manufacturer 65 with no slot path and 2-byte "
           "ids 200, 3 and 2-byte values 60, 270" )
    {
        std::array< std::pair< std::uint16_t, std::uint16_t >, 2 > id_value_pairs = {
            std::pair{std::uint16_t{200}, std::uint16_t{60}},
            std::pair{std::uint16_t{3}, std::uint16_t{270}}};
        const auto message = global_parameter_control( 65, id_value_pairs );
        REQUIRE( message.size() == 17 );

        THEN( "message is 0xF0 0x7F 0x41 <device_control_byte> 0x05 0x00 0x02 "
              "0x02 0x01 0x48 0x3C 0x00 0x00 0x03 0x0E 0x02 0xF7" )
        {
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x05} );
            CHECK( message[ 5 ] == std::byte{0x00} );
            CHECK( message[ 6 ] == std::byte{0x02} );
            CHECK( message[ 7 ] == std::byte{0x02} );
            CHECK( message[ 8 ] == std::byte{0x01} );
            CHECK( message[ 9 ] == std::byte{0x48} );
            CHECK( message[ 10 ] == std::byte{0x3C} );
            CHECK( message[ 11 ] == std::byte{0x00} );
            CHECK( message[ 12 ] == std::byte{0x00} );
            CHECK( message[ 13 ] == std::byte{0x03} );
            CHECK( message[ 14 ] == std::byte{0x0E} );
            CHECK( message[ 15 ] == std::byte{0x02} );
            CHECK( message[ 16 ] == end_byte );
        }
    }
}

SCENARIO( "system exclusive non-realtime messages", "[system_exclusive_message]" )
{
    using namespace sequencer::midi::system::exclusive;
    using namespace sequencer::midi::system::exclusive::non_realtime;

    GIVEN( "a sysex message for manufacturer 65 with values 1 and 52" )
    {
        const auto message = non_realtime::system_exclusive( 65, 1, 52 );
        REQUIRE( message.size() == 6 );

        THEN( "message is 0xF0 0x7F 0x41 0x01 0x34 0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == non_realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == std::byte{0x01} );
            REQUIRE( message[ 4 ] == std::byte{0x34} );
            REQUIRE( message[ 5 ] == end_byte );
        }
    }

    GIVEN( "a sysex general midi system disable message for manufacturer 65" )
    {
        const auto message = general_midi_system_disable( 65 );
        REQUIRE( message.size() == 6 );

        THEN( "message is 0xF0 0x7E 0x41 <gm_byte> 0x02 0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == non_realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == gm_byte );
            REQUIRE( message[ 4 ] == std::byte{0x02} );
            REQUIRE( message[ 5 ] == end_byte );
        }
    }

    GIVEN( "a sysex general midi 1 system enable message for manufacturer 65" )
    {
        const auto message = general_midi_1_system_enable( 65 );
        REQUIRE( message.size() == 6 );

        THEN( "message is 0xF0 0x7E 0x41 <gm_byte> 0x01 0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == non_realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == gm_byte );
            REQUIRE( message[ 4 ] == std::byte{0x01} );
            REQUIRE( message[ 5 ] == end_byte );
        }
    }

    GIVEN( "a sysex general midi 2 system enable message for manufacturer 65" )
    {
        const auto message = general_midi_2_system_enable( 65 );
        REQUIRE( message.size() == 6 );

        THEN( "message is 0xF0 0x7E 0x41 <gm_byte> 0x01 0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == non_realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == gm_byte );
            REQUIRE( message[ 4 ] == std::byte{0x03} );
            REQUIRE( message[ 5 ] == end_byte );
        }
    }

    GIVEN( "a sysex downloadable sounds on message for manufacturer 65" )
    {
        const auto on = true;
        const auto message = downloadable_sounds( 65, on );
        REQUIRE( message.size() == 6 );

        THEN( "message is 0xF0 0x7E 0x41 <dls_byte> 0x01 0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == non_realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == dls_byte );
            REQUIRE( message[ 4 ] == std::byte{0x01} );
            REQUIRE( message[ 5 ] == end_byte );
        }
    }

    GIVEN( "a sysex downloadable sounds off message for manufacturer 65" )
    {
        const auto off = false;
        const auto message = downloadable_sounds( 65, off );
        REQUIRE( message.size() == 6 );

        THEN( "message is 0xF0 0x7E 0x41 <dls_byte> 0x02 0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == non_realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == dls_byte );
            REQUIRE( message[ 4 ] == std::byte{0x02} );
            REQUIRE( message[ 5 ] == end_byte );
        }
    }

    GIVEN( "a sysex downloadable sounds voice allocation on message for manufacturer 65" )
    {
        const auto on = true;
        const auto message = downloadable_sounds_voice_allocation( 65, on );
        REQUIRE( message.size() == 6 );

        THEN( "message is 0xF0 0x7E 0x41 <dls_byte> 0x04 0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == non_realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == dls_byte );
            REQUIRE( message[ 4 ] == std::byte{0x04} );
            REQUIRE( message[ 5 ] == end_byte );
        }
    }

    GIVEN( "a sysex downloadable sounds voice allocation off message for manufacturer 65" )
    {
        const auto off = false;
        const auto message = downloadable_sounds_voice_allocation( 65, off );
        REQUIRE( message.size() == 6 );

        THEN( "message is 0xF0 0x7E 0x41 <dls_byte> 0x03 0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == non_realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == dls_byte );
            REQUIRE( message[ 4 ] == std::byte{0x03} );
            REQUIRE( message[ 5 ] == end_byte );
        }
    }
}
