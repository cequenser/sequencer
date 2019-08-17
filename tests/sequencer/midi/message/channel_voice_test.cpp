#include <sequencer/midi/message/channel_voice.hpp>

#include <catch2/catch.hpp>

// NOLINTNEXTLINE(readability-function-size)
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

    GIVEN( "a control change message for channel 15 and controller 47 with value 42" )
    {
        const auto channel = 15;
        const auto controller = 47;
        const auto value = 42;
        const auto message = control_change( channel, controller, value );

        THEN( "the first byte is 0xBF" )
        {
            CHECK( message[ 0 ] == std::byte{0xBF} );
        }

        THEN( "the second byte is 0x2F" )
        {
            CHECK( message[ 1 ] == std::byte{0x2F} );
        }

        THEN( "the third byte is 0x2A" )
        {
            CHECK( message[ 2 ] == std::byte{0x2A} );
        }
    }

    GIVEN( "program change message with value 73" )
    {
        const auto channel = 15;
        const auto message = program_change( channel, 73 );

        THEN( "message contains 2 bytes" )
        {
            REQUIRE( message.size() == 2 );
        }

        THEN( "first byte is 0xCF" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xCF} );
        }

        THEN( "second byte is 0x49" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x49} );
        }
    }

    GIVEN( "channel pressure message with value 73" )
    {
        const auto channel = 15;
        const auto message = channel_pressure( channel, 73 );

        THEN( "message contains 2 bytes" )
        {
            REQUIRE( message.size() == 2 );
        }

        THEN( "first byte is 0xDF" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xDF} );
        }

        THEN( "second byte is 0x49" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x49} );
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

        THEN( "third byte is 0x41" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "bank select lsb and msb messages for channel 1 and bank 8392" )
    {
        const auto [ bank_select_lsb, bank_select_msb ] = bank_select( 1, 8392 );

        THEN( "bank select lsb message is 0xB1 0x20 0x48" )
        {
            REQUIRE( bank_select_lsb.size() == 3 );
            REQUIRE( bank_select_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( bank_select_lsb[ 1 ] == std::byte{0x20} );
            REQUIRE( bank_select_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "bank select msb message is 0xB1 0x00 0x41" )
        {
            REQUIRE( bank_select_msb.size() == 3 );
            REQUIRE( bank_select_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( bank_select_msb[ 1 ] == std::byte{0x00} );
            REQUIRE( bank_select_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "bank select lsb and msb and program change messages for channel 1 and bank 8392 and "
           "program 3" )
    {
        const auto [ bank_select_lsb, bank_select_msb, prog_change ] =
            bank_select_with_program( 1, 8392, 3 );

        THEN( "bank select lsb message is 0xB1 0x20 0x48" )
        {
            REQUIRE( bank_select_lsb.size() == 3 );
            REQUIRE( bank_select_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( bank_select_lsb[ 1 ] == std::byte{0x20} );
            REQUIRE( bank_select_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "bank select msb message is 0xB1 0x00 0x41" )
        {
            REQUIRE( bank_select_msb.size() == 3 );
            REQUIRE( bank_select_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( bank_select_msb[ 1 ] == std::byte{0x00} );
            REQUIRE( bank_select_msb[ 2 ] == std::byte{0x41} );
        }

        THEN( "program change message is 0xCF 0x03" )
        {
            REQUIRE( prog_change.size() == 2 );
            REQUIRE( prog_change[ 0 ] == std::byte{0xC1} );
            REQUIRE( prog_change[ 1 ] == std::byte{0x03} );
        }
    }

    GIVEN( "modulation wheel lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ modulation_wheel_lsb, modulation_wheel_msb ] = modulation_wheel( 1, 8392 );

        THEN( "modulation wheel lsb message is 0xB1 0x21 0x48" )
        {
            REQUIRE( modulation_wheel_lsb.size() == 3 );
            REQUIRE( modulation_wheel_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( modulation_wheel_lsb[ 1 ] == std::byte{0x21} );
            REQUIRE( modulation_wheel_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "modulation wheel msb message is 0xB1 0x01 0x41" )
        {
            REQUIRE( modulation_wheel_msb.size() == 3 );
            REQUIRE( modulation_wheel_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( modulation_wheel_msb[ 1 ] == std::byte{0x01} );
            REQUIRE( modulation_wheel_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "breath controller lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ breath_controller_lsb, breath_controller_msb ] = breath_controller( 1, 8392 );

        THEN( "breath controller lsb message is 0xB1 0x22 0x48" )
        {
            REQUIRE( breath_controller_lsb.size() == 3 );
            REQUIRE( breath_controller_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( breath_controller_lsb[ 1 ] == std::byte{0x22} );
            REQUIRE( breath_controller_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "breath controller msb message is 0xB1 0x02 0x41" )
        {
            REQUIRE( breath_controller_msb.size() == 3 );
            REQUIRE( breath_controller_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( breath_controller_msb[ 1 ] == std::byte{0x02} );
            REQUIRE( breath_controller_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "foot controller lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ foot_controller_lsb, foot_controller_msb ] = foot_controller( 1, 8392 );

        THEN( "foot controller lsb message is 0xB1 0x24 0x48" )
        {
            REQUIRE( foot_controller_lsb.size() == 3 );
            REQUIRE( foot_controller_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( foot_controller_lsb[ 1 ] == std::byte{0x24} );
            REQUIRE( foot_controller_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "foot controller msb message is 0xB1 0x04 0x41" )
        {
            REQUIRE( foot_controller_msb.size() == 3 );
            REQUIRE( foot_controller_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( foot_controller_msb[ 1 ] == std::byte{0x04} );
            REQUIRE( foot_controller_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "portamento time lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ portamento_time_lsb, portamento_time_msb ] = portamento_time( 1, 8392 );

        THEN( "portamento time lsb message is 0xB1 0x25 0x48" )
        {
            REQUIRE( portamento_time_lsb.size() == 3 );
            REQUIRE( portamento_time_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( portamento_time_lsb[ 1 ] == std::byte{0x25} );
            REQUIRE( portamento_time_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "portamento time msb message is 0xB1 0x05 0x41" )
        {
            REQUIRE( portamento_time_msb.size() == 3 );
            REQUIRE( portamento_time_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( portamento_time_msb[ 1 ] == std::byte{0x05} );
            REQUIRE( portamento_time_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "data entry lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ data_entry_lsb, data_entry_msb ] = data_entry( 1, 8392 );

        THEN( "data entry lsb message is 0xB1 0x26 0x48" )
        {
            REQUIRE( data_entry_lsb.size() == 3 );
            REQUIRE( data_entry_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( data_entry_lsb[ 1 ] == std::byte{0x26} );
            REQUIRE( data_entry_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "data entry msb message is 0xB1 0x06 0x41" )
        {
            REQUIRE( data_entry_msb.size() == 3 );
            REQUIRE( data_entry_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( data_entry_msb[ 1 ] == std::byte{0x06} );
            REQUIRE( data_entry_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "channel volume lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ channel_volume_lsb, channel_volume_msb ] = channel_volume( 1, 8392 );

        THEN( "channel volume lsb message is 0xB1 0x27 0x48" )
        {
            REQUIRE( channel_volume_lsb.size() == 3 );
            REQUIRE( channel_volume_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( channel_volume_lsb[ 1 ] == std::byte{0x27} );
            REQUIRE( channel_volume_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "channel volume msb message is 0xB1 0x07 0x41" )
        {
            REQUIRE( channel_volume_msb.size() == 3 );
            REQUIRE( channel_volume_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( channel_volume_msb[ 1 ] == std::byte{0x07} );
            REQUIRE( channel_volume_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "balance lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ balance_lsb, balance_msb ] = balance( 1, 8392 );

        THEN( "balance lsb message is 0xB1 0x28 0x48" )
        {
            REQUIRE( balance_lsb.size() == 3 );
            REQUIRE( balance_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( balance_lsb[ 1 ] == std::byte{0x28} );
            REQUIRE( balance_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "balance msb message is 0xB1 0x08 0x41" )
        {
            REQUIRE( balance_msb.size() == 3 );
            REQUIRE( balance_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( balance_msb[ 1 ] == std::byte{0x08} );
            REQUIRE( balance_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "pan lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ pan_lsb, pan_msb ] = pan( 1, 8392 );

        THEN( "pan lsb message is 0xB1 0x2A 0x48" )
        {
            REQUIRE( pan_lsb.size() == 3 );
            REQUIRE( pan_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( pan_lsb[ 1 ] == std::byte{0x2A} );
            REQUIRE( pan_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "pan msb message is 0xB1 0x0A 0x41" )
        {
            REQUIRE( pan_msb.size() == 3 );
            REQUIRE( pan_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( pan_msb[ 1 ] == std::byte{0x0A} );
            REQUIRE( pan_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "expression controller lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ expression_controller_lsb, expression_controller_msb ] =
            expression_controller( 1, 8392 );

        THEN( "expression controller lsb message is 0xB1 0x2B 0x48" )
        {
            REQUIRE( expression_controller_lsb.size() == 3 );
            REQUIRE( expression_controller_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( expression_controller_lsb[ 1 ] == std::byte{0x2B} );
            REQUIRE( expression_controller_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "expression controller msb message is 0xB1 0x0B 0x41" )
        {
            REQUIRE( expression_controller_msb.size() == 3 );
            REQUIRE( expression_controller_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( expression_controller_msb[ 1 ] == std::byte{0x0B} );
            REQUIRE( expression_controller_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "effect control 1 lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ effect_control_1_lsb, effect_control_1_msb ] = effect_control_1( 1, 8392 );

        THEN( "effect control 1 lsb message is 0xB1 0x2C 0x48" )
        {
            REQUIRE( effect_control_1_lsb.size() == 3 );
            REQUIRE( effect_control_1_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( effect_control_1_lsb[ 1 ] == std::byte{0x2C} );
            REQUIRE( effect_control_1_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "effect control 1 msb message is 0xB1 0x0C 0x41" )
        {
            REQUIRE( effect_control_1_msb.size() == 3 );
            REQUIRE( effect_control_1_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( effect_control_1_msb[ 1 ] == std::byte{0x0C} );
            REQUIRE( effect_control_1_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "effect control 2 lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ effect_control_2_lsb, effect_control_2_msb ] = effect_control_2( 1, 8392 );

        THEN( "effect control 2 lsb message is 0xB1 0x2D 0x48" )
        {
            REQUIRE( effect_control_2_lsb.size() == 3 );
            REQUIRE( effect_control_2_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( effect_control_2_lsb[ 1 ] == std::byte{0x2D} );
            REQUIRE( effect_control_2_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "effect control 2 msb message is 0xB1 0x0D 0x41" )
        {
            REQUIRE( effect_control_2_msb.size() == 3 );
            REQUIRE( effect_control_2_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( effect_control_2_msb[ 1 ] == std::byte{0x0D} );
            REQUIRE( effect_control_2_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "general purpose controller 1 lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ general_purpose_controller_lsb, general_purpose_controller_msb ] =
            general_purpose_controller( 1, 1, 8392 );

        THEN( "general purpose controller 1 lsb message is 0xB1 0x30 0x48" )
        {
            REQUIRE( general_purpose_controller_lsb.size() == 3 );
            REQUIRE( general_purpose_controller_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( general_purpose_controller_lsb[ 1 ] == std::byte{0x30} );
            REQUIRE( general_purpose_controller_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "general purpose controller 1 msb message is 0xB1 0x10 0x41" )
        {
            REQUIRE( general_purpose_controller_msb.size() == 3 );
            REQUIRE( general_purpose_controller_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( general_purpose_controller_msb[ 1 ] == std::byte{0x10} );
            REQUIRE( general_purpose_controller_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "general purpose controller 2 lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ general_purpose_controller_lsb, general_purpose_controller_msb ] =
            general_purpose_controller( 2, 1, 8392 );

        THEN( "general purpose controller 2 lsb message is 0xB1 0x31 0x48" )
        {
            REQUIRE( general_purpose_controller_lsb.size() == 3 );
            REQUIRE( general_purpose_controller_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( general_purpose_controller_lsb[ 1 ] == std::byte{0x31} );
            REQUIRE( general_purpose_controller_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "general purpose controller 2 msb message is 0xB1 0x11 0x41" )
        {
            REQUIRE( general_purpose_controller_msb.size() == 3 );
            REQUIRE( general_purpose_controller_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( general_purpose_controller_msb[ 1 ] == std::byte{0x11} );
            REQUIRE( general_purpose_controller_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "general purpose controller 3 lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ general_purpose_controller_lsb, general_purpose_controller_msb ] =
            general_purpose_controller( 3, 1, 8392 );

        THEN( "general purpose controller 3 lsb message is 0xB1 0x32 0x48" )
        {
            REQUIRE( general_purpose_controller_lsb.size() == 3 );
            REQUIRE( general_purpose_controller_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( general_purpose_controller_lsb[ 1 ] == std::byte{0x32} );
            REQUIRE( general_purpose_controller_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "general purpose controller 3 msb message is 0xB1 0x12 0x41" )
        {
            REQUIRE( general_purpose_controller_msb.size() == 3 );
            REQUIRE( general_purpose_controller_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( general_purpose_controller_msb[ 1 ] == std::byte{0x12} );
            REQUIRE( general_purpose_controller_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "general purpose controller 4 lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ general_purpose_controller_lsb, general_purpose_controller_msb ] =
            general_purpose_controller( 4, 1, 8392 );

        THEN( "general purpose controller 4 lsb message is 0xB1 0x33 0x48" )
        {
            REQUIRE( general_purpose_controller_lsb.size() == 3 );
            REQUIRE( general_purpose_controller_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( general_purpose_controller_lsb[ 1 ] == std::byte{0x33} );
            REQUIRE( general_purpose_controller_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "general purpose controller 4 msb message is 0xB1 0x13 0x41" )
        {
            REQUIRE( general_purpose_controller_msb.size() == 3 );
            REQUIRE( general_purpose_controller_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( general_purpose_controller_msb[ 1 ] == std::byte{0x13} );
            REQUIRE( general_purpose_controller_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "non-registered parameter number lsb and msb messages for channel 1 and value 8392" )
    {
        const auto [ non_registered_parameter_number_lsb, non_registered_parameter_number_msb ] =
            non_registered_parameter_number( 1, 8392 );

        THEN( "non-registered parameter number lsb message is 0xB1 0x62 0x48" )
        {
            REQUIRE( non_registered_parameter_number_lsb.size() == 3 );
            REQUIRE( non_registered_parameter_number_lsb[ 0 ] == std::byte{0xB1} );
            REQUIRE( non_registered_parameter_number_lsb[ 1 ] == std::byte{0x62} );
            REQUIRE( non_registered_parameter_number_lsb[ 2 ] == std::byte{0x48} );
        }

        THEN( "non-registered parameter number msb message is 0xB1 0x63 0x41" )
        {
            REQUIRE( non_registered_parameter_number_msb.size() == 3 );
            REQUIRE( non_registered_parameter_number_msb[ 0 ] == std::byte{0xB1} );
            REQUIRE( non_registered_parameter_number_msb[ 1 ] == std::byte{0x63} );
            REQUIRE( non_registered_parameter_number_msb[ 2 ] == std::byte{0x41} );
        }
    }

    GIVEN( "an effects 1 depth message for channel 1 with value 17" )
    {
        const auto channel = 1;
        const auto message = effects_1_depth( channel, 17 );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x5B" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x5B} );
        }

        THEN( "third byte is 0x11" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x11} );
        }
    }

    GIVEN( "an effects 2 depth message for channel 1 with value 17" )
    {
        const auto channel = 1;
        const auto message = effects_2_depth( channel, 17 );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x5C" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x5C} );
        }

        THEN( "third byte is 0x11" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x11} );
        }
    }

    GIVEN( "an effects 3 depth message for channel 1 with value 17" )
    {
        const auto channel = 1;
        const auto message = effects_3_depth( channel, 17 );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x5D" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x5D} );
        }

        THEN( "third byte is 0x11" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x11} );
        }
    }

    GIVEN( "an effects 4 depth message for channel 1 with value 17" )
    {
        const auto channel = 1;
        const auto message = effects_4_depth( channel, 17 );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x5E" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x5E} );
        }

        THEN( "third byte is 0x11" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x11} );
        }
    }

    GIVEN( "an effects 5 depth message for channel 1 with value 17" )
    {
        const auto channel = 1;
        const auto message = effects_5_depth( channel, 17 );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x5F" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x5F} );
        }

        THEN( "third byte is 0x11" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x11} );
        }
    }

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

    GIVEN( "a local control on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = local_control( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x7A" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x7A} );
        }

        THEN( "third byte is 0x7F" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x7F} );
        }
    }

    GIVEN( "a local control off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = local_control( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x7A" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x7A} );
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

    GIVEN( "an poly mode on message for channel 1" )
    {
        const auto channel = 1;
        const auto message = poly_mode_on( channel );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x7F" )
        {
            REQUIRE( message[ 1 ] == std::byte{0x7F} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }

    GIVEN( "a damper pedal on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = damper_pedal( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x40" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x40} );
        }

        THEN( "third byte is 0x7F" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x7F} );
        }
    }

    GIVEN( "a damper pedal off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = damper_pedal( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x40" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x40} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }

    GIVEN( "a portamento on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = portamento( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x41" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x41} );
        }

        THEN( "third byte is 0x7F" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x7F} );
        }
    }

    GIVEN( "a portamento off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = portamento( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x41" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x41} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }

    GIVEN( "a sostenuto on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = sostenuto( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x42" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x42} );
        }

        THEN( "third byte is 0x7F" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x7F} );
        }
    }

    GIVEN( "a sostenuto off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = sostenuto( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x42" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x42} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }

    GIVEN( "a soft pedal on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = soft_pedal( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x43" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x43} );
        }

        THEN( "third byte is 0x7F" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x7F} );
        }
    }

    GIVEN( "a soft pedal off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = soft_pedal( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x43" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x43} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }

    GIVEN( "a hold 2 on message for channel 1" )
    {
        const auto channel = 1;
        const auto on = true;
        const auto message = hold_2( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x45" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x45} );
        }

        THEN( "third byte is 0x7F" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x7F} );
        }
    }

    GIVEN( "a hold 2 off message for channel 1" )
    {
        const auto channel = 1;
        const auto on = false;
        const auto message = hold_2( channel, on );

        THEN( "first byte is 0xB1" )
        {
            REQUIRE( message[ 0 ] == std::byte{0xB1} );
        }

        THEN( "second byte is 0x45" )
        {
            REQUIRE( message[ 01 ] == std::byte{0x45} );
        }

        THEN( "third byte is 0x00" )
        {
            REQUIRE( message[ 2 ] == std::byte{0x00} );
        }
    }
}
