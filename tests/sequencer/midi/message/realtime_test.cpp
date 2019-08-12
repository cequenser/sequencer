#include <sequencer/midi/message/realtime.hpp>

#include <catch2/catch.hpp>

#include <sstream>

SCENARIO( "realtime message to string conversion", "[realtime_message]" )
{
    using namespace sequencer::midi::message::real_time;

    GIVEN( "realtime clock message" )
    {
        const auto message = message_type::realtime_clock;

        THEN( "to_string returns 'clock'" )
        {
            REQUIRE( to_string( message ) == "clock" );
        }
    }

    GIVEN( "realtime start message" )
    {
        const auto message = message_type::realtime_start;

        THEN( "to_string returns 'start'" )
        {
            REQUIRE( to_string( message ) == "start" );
        }
    }

    GIVEN( "realtime continue message" )
    {
        const auto message = message_type::realtime_continue;

        THEN( "to_string returns 'continue'" )
        {
            REQUIRE( to_string( message ) == "continue" );
        }
    }

    GIVEN( "realtime stop message" )
    {
        const auto message = message_type::realtime_stop;

        THEN( "to_string returns 'stop'" )
        {
            REQUIRE( to_string( message ) == "stop" );
        }
    }

    GIVEN( "realtime invalid message" )
    {
        const auto message = message_type::invalid;

        THEN( "to_string returns 'invalid'" )
        {
            REQUIRE( to_string( message ) == sequencer::midi::message::invalid_string );
        }
    }
}

SCENARIO( "realtime message streaming", "[realtime_message]" )
{
    using namespace sequencer::midi::message::real_time;

    GIVEN( "message_type::realtime_clock" )
    {
        const auto message = message_type::realtime_clock;

        THEN( "stream operator writes 'clock'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "clock" );
        }
    }

    GIVEN( "message_type::realtime_start" )
    {
        const auto message = message_type::realtime_start;

        THEN( "stream operator writes 'start'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "start" );
        }
    }

    GIVEN( "message_type::realtime_stop" )
    {
        const auto message = message_type::realtime_stop;

        THEN( "stream operator writes 'stop'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "stop" );
        }
    }

    GIVEN( "message_type::realtime_continue" )
    {
        const auto message = message_type::realtime_continue;

        THEN( "stream operator writes 'continue'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == "continue" );
        }
    }

    GIVEN( "message_type::invalid" )
    {
        const auto message = message_type::invalid;

        THEN( "stream operator writes 'invalid'" )
        {
            std::stringstream stream;
            stream << message;
            REQUIRE( stream.str() == sequencer::midi::message::invalid_string );
        }
    }
}

SCENARIO( "realtime messages", "[realtime_message]" )
{
    using namespace sequencer::midi::message::real_time;

    GIVEN( "realtime clock message" )
    {
        const auto message = message_type::realtime_clock;

        WHEN( "static_cast to unsigned char" )
        {
            const auto as_char = static_cast< unsigned char >( message );

            THEN( "its value equals 0xF8" )
            {
                REQUIRE( as_char == 0xF8 );
            }
        }
    }

    GIVEN( "realtime start message" )
    {
        const auto message = message_type::realtime_start;

        WHEN( "static_cast to unsigned char" )
        {
            const auto as_char = static_cast< unsigned char >( message );

            THEN( "its value equals 0xFA" )
            {
                REQUIRE( as_char == 0xFA );
            }
        }
    }

    GIVEN( "realtime continue message" )
    {
        const auto message = message_type::realtime_continue;

        WHEN( "static_cast to unsigned char" )
        {
            const auto as_char = static_cast< unsigned char >( message );

            THEN( "its value equals 0xFB" )
            {
                REQUIRE( as_char == 0xFB );
            }
        }
    }

    GIVEN( "realtime stop message" )
    {
        const auto message = message_type::realtime_stop;

        WHEN( "static_cast to unsigned char" )
        {
            const auto as_char = static_cast< unsigned char >( message );

            THEN( "its value equals 0xFC" )
            {
                REQUIRE( as_char == 0xFC );
            }
        }
    }
}
