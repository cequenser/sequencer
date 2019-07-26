#include <sequencer/midi_clock.hpp>

#include <catch2/catch.hpp>

#include <RtMidi.h>
#include <cstddef>

SCENARIO( "midi_real_time_message", "[midi_real_time_message]" )
{
    using namespace sequencer;

    GIVEN( "real time message types" )
    {
        const auto messages = std::vector< midi_clock::message_type >{
            midi_clock::message_type::start, midi_clock::message_type::cont,
            midi_clock::message_type::stop, midi_clock::message_type::pulse};
        THEN( "are converted to midi byte messages" )
        {
            CHECK( midi_real_time_message( messages[ 0 ] ) == 0xFA );
            CHECK( midi_real_time_message( messages[ 1 ] ) == 0xFB );
            REQUIRE( midi_real_time_message( messages[ 2 ] ) == 0xFC );
            REQUIRE( midi_real_time_message( messages[ 3 ] ) == 0xF8 );
        }
    }
}
