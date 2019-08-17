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

        THEN( "first byte is 0xF0 0x7F 0x41 0x01 0x34 0xF7" )
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

        THEN( "first byte is 0xF0 0x7F 0x41 <device_control_byte> <master_volume_byte> 0x48 0x01 "
              "0xF7" )
        {
            REQUIRE( message[ 0 ] == start_byte );
            REQUIRE( message[ 1 ] == realtime::id_byte );
            REQUIRE( message[ 2 ] == std::byte{0x41} );
            REQUIRE( message[ 3 ] == device_control_byte );
            REQUIRE( message[ 4 ] == master_volume_byte );
            REQUIRE( message[ 5 ] == std::byte{0x48} );
            REQUIRE( message[ 6 ] == std::byte{0x01} );
            REQUIRE( message[ 7 ] == end_byte );
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

        THEN( "first byte is 0xF0 0x7F 0x41 0x01 0x34 0xF7" )
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

        THEN( "first byte is 0xF0 0x7E 0x41 <gm_byte> 0x02 0xF7" )
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

        THEN( "first byte is 0xF0 0x7E 0x41 <gm_byte> 0x01 0xF7" )
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

        THEN( "first byte is 0xF0 0x7E 0x41 <gm_byte> 0x01 0xF7" )
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

        THEN( "first byte is 0xF0 0x7E 0x41 <dls_byte> 0x01 0xF7" )
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

        THEN( "first byte is 0xF0 0x7E 0x41 <dls_byte> 0x02 0xF7" )
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

        THEN( "first byte is 0xF0 0x7E 0x41 <dls_byte> 0x04 0xF7" )
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

        THEN( "first byte is 0xF0 0x7E 0x41 <dls_byte> 0x03 0xF7" )
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
