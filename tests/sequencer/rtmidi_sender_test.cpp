#include <sequencer/midi_clock.hpp>

#include <catch2/catch.hpp>

#include <cstddef>

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
