#include <sequencer/midi/sender.hpp>

#include <catch2/catch.hpp>

#include <cstddef>
#include <vector>

SCENARIO( "midi_real_time_message", "[midi_real_time_message]" )
{
    using namespace sequencer;

    GIVEN( "real time message types" )
    {
        const auto messages = std::vector< midi::message_type >{
            midi::message_type::realtime_start, midi::message_type::realtime_continue,
            midi::message_type::realtime_stop, midi::message_type::realtime_clock};

        THEN( "are converted to midi byte messages" )
        {
            CHECK( static_cast< std::byte >( messages[ 0 ] ) == std::byte{0xFA} );
            CHECK( static_cast< std::byte >( messages[ 1 ] ) == std::byte{0xFB} );
            CHECK( static_cast< std::byte >( messages[ 2 ] ) == std::byte{0xFC} );
            CHECK( static_cast< std::byte >( messages[ 3 ] ) == std::byte{0xF8} );
        }
    }
}

SCENARIO( "midi_sender", "[midi_sender]" )
{
    using namespace sequencer;

    GIVEN( "a midi sender" )
    {
        std::byte received_midi_byte_message{0x00};
        const auto midi_sender_impl = [&received_midi_byte_message]( std::byte message ) {
            received_midi_byte_message = message;
        };
        const auto midi_sender = midi::sender( midi_sender_impl );

        THEN( "converted midi byte messages are received" )
        {
            midi_sender( midi::message_type::realtime_start );
            CHECK( received_midi_byte_message == std::byte{0xFA} );
            midi_sender( midi::message_type::realtime_continue );
            CHECK( received_midi_byte_message == std::byte{0xFB} );
            midi_sender( midi::message_type::realtime_stop );
            CHECK( received_midi_byte_message == std::byte{0xFC} );
            midi_sender( midi::message_type::realtime_clock );
            CHECK( received_midi_byte_message == std::byte{0xF8} );
        }
    }
}
