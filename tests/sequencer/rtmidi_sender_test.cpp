#include <sequencer/midi/message/realtime.hpp>

#include <catch2/catch.hpp>

#include <cstddef>

SCENARIO( "midi_realtime_message", "[midi_realtime_message]" )
{
    using namespace sequencer::midi::realtime;

    GIVEN( "realtime message types" )
    {
        const auto messages = std::vector< message_type >{
            message_type::realtime_start, message_type::realtime_continue,
            message_type::realtime_stop, message_type::realtime_clock};

        THEN( "are converted to midi byte messages" )
        {
            CHECK( static_cast< std::byte >( messages[ 0 ] ) == std::byte{0xFA} );
            CHECK( static_cast< std::byte >( messages[ 1 ] ) == std::byte{0xFB} );
            CHECK( static_cast< std::byte >( messages[ 2 ] ) == std::byte{0xFC} );
            CHECK( static_cast< std::byte >( messages[ 3 ] ) == std::byte{0xF8} );
        }
    }
}
