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
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == device_control_byte );
            CHECK( message[ 4 ] == std::byte{0x04} );
            CHECK( message[ 5 ] == std::byte{0x48} );
            CHECK( message[ 6 ] == std::byte{0x01} );
            CHECK( message[ 7 ] == end_byte );
        }
    }
}

SCENARIO( "system exclusive - key based instrument control", "[system_exclusive]" )
{
    using namespace sequencer::midi::system::exclusive;
    using namespace sequencer::midi::system::exclusive::realtime;

    GIVEN( "a key based instrument control message for manufacturer 65 with channel 5, key 60, "
           "controller number 50 and controller value 40" )
    {
        const std::uint8_t manufacturer_id = 65;
        const std::uint8_t channel = 5;
        const std::uint8_t key = 60;
        const std::array< std::pair< std::uint8_t, std::uint8_t >, 1 > controller_number_value = {
            std::pair{std::uint8_t{50}, std::uint8_t{40}}};
        const auto message =
            key_based_instrument_control( manufacturer_id, channel, key, controller_number_value );

        THEN( "message is 0xF0 0x7F 0x41 0x0A 0x01 0x05 0x3C 0x32 0x28 0xF7" )
        {
            REQUIRE( message.size() == 10 );
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == std::byte{0x0A} );
            CHECK( message[ 4 ] == std::byte{0x01} );
            CHECK( message[ 5 ] == std::byte{0x05} );
            CHECK( message[ 6 ] == std::byte{0x3C} );
            CHECK( message[ 7 ] == std::byte{0x32} );
            CHECK( message[ 8 ] == std::byte{0x28} );
            CHECK( message[ 9 ] == end_byte );
        }
    }

    GIVEN( "a key based instrument control message for manufacturer 65 with channel 5, key 60, "
           "controller numbers 50, 45 and controller values 40, 30" )
    {
        const std::uint8_t manufacturer_id = 65;
        const std::uint8_t channel = 5;
        const std::uint8_t key = 60;
        const std::array< std::pair< std::uint8_t, std::uint8_t >, 2 > controller_number_value = {
            std::pair{std::uint8_t{50}, std::uint8_t{40}},
            std::pair{std::uint8_t{45}, std::uint8_t{30}}};
        const auto message =
            key_based_instrument_control( manufacturer_id, channel, key, controller_number_value );

        THEN( "message is 0xF0 0x7F 0x41 0x0A 0x01 0x05 0x3C 0x32 0x28 0x2D 0x1E 0xF7" )
        {
            REQUIRE( message.size() == 12 );
            CHECK( message[ 0 ] == start_byte );
            CHECK( message[ 1 ] == realtime::id_byte );
            CHECK( message[ 2 ] == std::byte{0x41} );
            CHECK( message[ 3 ] == std::byte{0x0A} );
            CHECK( message[ 4 ] == std::byte{0x01} );
            CHECK( message[ 5 ] == std::byte{0x05} );
            CHECK( message[ 6 ] == std::byte{0x3C} );
            CHECK( message[ 7 ] == std::byte{0x32} );
            CHECK( message[ 8 ] == std::byte{0x28} );
            CHECK( message[ 9 ] == std::byte{0x2D} );
            CHECK( message[ 10 ] == std::byte{0x1E} );
            CHECK( message[ 11 ] == end_byte );
        }
    }

    GIVEN( "key_based_instrument_control_type::note_volume" )
    {
        const auto value = key_based_instrument_control_type::note_volume;

        THEN( "value is 0x07" )
        {
            REQUIRE( to_uint8( value ) == 0x07 );
        }
    }

    GIVEN( "key_based_instrument_control_type::pan" )
    {
        const auto value = key_based_instrument_control_type::pan;

        THEN( "value is 0x0A" )
        {
            REQUIRE( to_uint8( value ) == 0x0A );
        }
    }

    GIVEN( "key_based_instrument_control_type::timbre_harmonic_intensity" )
    {
        const auto value = key_based_instrument_control_type::timbre_harmonic_intensity;

        THEN( "value is 0x47" )
        {
            REQUIRE( to_uint8( value ) == 0x47 );
        }
    }

    GIVEN( "key_based_instrument_control_type::release_time" )
    {
        const auto value = key_based_instrument_control_type::release_time;

        THEN( "value is 0x48" )
        {
            REQUIRE( to_uint8( value ) == 0x48 );
        }
    }

    GIVEN( "key_based_instrument_control_type::attack_time" )
    {
        const auto value = key_based_instrument_control_type::attack_time;

        THEN( "value is 0x49" )
        {
            REQUIRE( to_uint8( value ) == 0x49 );
        }
    }

    GIVEN( "key_based_instrument_control_type::brightness" )
    {
        const auto value = key_based_instrument_control_type::brightness;

        THEN( "value is 0x4A" )
        {
            REQUIRE( to_uint8( value ) == 0x4A );
        }
    }

    GIVEN( "key_based_instrument_control_type::decay_time" )
    {
        const auto value = key_based_instrument_control_type::decay_time;

        THEN( "value is 0x4B" )
        {
            REQUIRE( to_uint8( value ) == 0x4B );
        }
    }

    GIVEN( "key_based_instrument_control_type::decay_time" )
    {
        const auto value = key_based_instrument_control_type::decay_time;

        THEN( "value is 0x4B" )
        {
            REQUIRE( to_uint8( value ) == 0x4B );
        }
    }

    GIVEN( "key_based_instrument_control_type::vibrato_rate" )
    {
        const auto value = key_based_instrument_control_type::vibrato_rate;

        THEN( "value is 0x4C" )
        {
            REQUIRE( to_uint8( value ) == 0x4C );
        }
    }

    GIVEN( "key_based_instrument_control_type::vibrato_depth" )
    {
        const auto value = key_based_instrument_control_type::vibrato_depth;

        THEN( "value is 0x4D" )
        {
            REQUIRE( to_uint8( value ) == 0x4D );
        }
    }

    GIVEN( "key_based_instrument_control_type::vibrato_delay" )
    {
        const auto value = key_based_instrument_control_type::vibrato_delay;

        THEN( "value is 0x4E" )
        {
            REQUIRE( to_uint8( value ) == 0x4E );
        }
    }

    GIVEN( "key_based_instrument_control_type::reverb_send" )
    {
        const auto value = key_based_instrument_control_type::reverb_send;

        THEN( "value is 0x5B" )
        {
            REQUIRE( to_uint8( value ) == 0x5B );
        }
    }

    GIVEN( "key_based_instrument_control_type::chorus_send" )
    {
        const auto value = key_based_instrument_control_type::chorus_send;

        THEN( "value is 0x5D" )
        {
            REQUIRE( to_uint8( value ) == 0x5D );
        }
    }

    GIVEN( "key_based_instrument_control_type::fine_tuning" )
    {
        const auto value = key_based_instrument_control_type::fine_tuning;

        THEN( "value is 0x78" )
        {
            REQUIRE( to_uint8( value ) == 0x78 );
        }
    }

    GIVEN( "key_based_instrument_control_type::coarse_tuning" )
    {
        const auto value = key_based_instrument_control_type::coarse_tuning;

        THEN( "value is 0x79" )
        {
            REQUIRE( to_uint8( value ) == 0x79 );
        }
    }
}

// NOLINTNEXTLINE(readability-function-size)
SCENARIO( "system exclusive - global parameter control", "[system_exclusive]" )
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
