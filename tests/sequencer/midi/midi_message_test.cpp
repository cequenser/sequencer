#include <sequencer/midi/message.hpp>

#include <catch2/catch.hpp>

#include <bitset>
#include <sstream>
#include <string>

namespace
{
    std::string binary_representation( std::byte byte )
    {
        std::stringstream s;
        s << std::bitset< 8 >( static_cast< unsigned char >( byte ) );
        return s.str();
    }

} // namespace

SCENARIO( "realtime midi messages provide expected binary representation", "[midi_message]" )
{
    WHEN( "we consider the timing clock message" )
    {
        const auto msg = sequencer::midi::message::real_time::timing_clock();
        THEN( "its binary representation is 11111000" )
        {
            CHECK( binary_representation( msg ) == "11111000" );
        }
    }

    WHEN( "we consider the start message" )
    {
        const auto msg = sequencer::midi::message::real_time::start();
        THEN( "its binary representation is 11111010" )
        {
            CHECK( binary_representation( msg ) == "11111010" );
        }
    }

    WHEN( "we consider the continue message" )
    {
        const auto msg = sequencer::midi::message::real_time::cont();
        THEN( "its binary representation is 11111011" )
        {
            CHECK( binary_representation( msg ) == "11111011" );
        }
    }

    WHEN( "we consider the stop message" )
    {
        const auto msg = sequencer::midi::message::real_time::stop();
        THEN( "its binary representation is 11111100" )
        {
            CHECK( binary_representation( msg ) == "11111100" );
        }
    }

    WHEN( "we consider the active sensing message" )
    {
        const auto msg = sequencer::midi::message::real_time::active_sensing();
        THEN( "its binary representation is 11111110" )
        {
            CHECK( binary_representation( msg ) == "11111110" );
        }
    }

    WHEN( "we consider the reset message" )
    {
        const auto msg = sequencer::midi::message::real_time::reset();
        THEN( "its binary representation is 11111111" )
        {
            CHECK( binary_representation( msg ) == "11111111" );
        }
    }
}
