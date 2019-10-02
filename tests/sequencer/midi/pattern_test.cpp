#include <sequencer/backend/digitakt_parameter.hpp>
#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/midi/pattern.hpp>
#include <sequencer/midi/step.hpp>
#include <sequencer/midi/track.hpp>

#include <catch2/catch.hpp>

const auto active_step = sequencer::midi::step_t{true};

SCENARIO( "loop length", "[pattern]" )
{
    using namespace sequencer::midi;
    using sequencer::backend::digitakt::track_parameter_t;

    GIVEN( "a pattern with 2 tracks and 4 steps each" )
    {
        const auto tracks = 2;
        const auto steps = 4;
        auto pattern = pattern_t< track_t< step_t, track_parameter_t > >{tracks, steps};
        pattern[ 0 ].parameter().set_note_length_idx( 14 );
        pattern[ 1 ].parameter().set_note_length_idx( 14 );
        pattern.set_steps_per_beat( 1 );

        AND_GIVEN( "a note on the 3rd step of each track" )
        {
            pattern[ 0 ][ 2 ] = active_step;
            pattern[ 1 ][ 2 ] = active_step;

            WHEN( "4*24 clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                pattern.send_messages( realtime::realtime_start(), sender );
                for ( auto i = 0; i < 4 * 24; ++i )
                {
                    pattern.send_messages( realtime::realtime_clock(), sender );
                }

                THEN( "4 messages are received" )
                {
                    REQUIRE( received_messages.size() == 4 );
                }
            }

            WHEN( "the length of the second track is changed to three steps" )
            {
                pattern[ 0 ].set_steps( 3 );

                AND_WHEN( "3*4*24 clock messages are sent" )
                {
                    std::vector< message_t< 3 > > received_messages;
                    const auto sender = [&received_messages]( const auto& msg ) {
                        received_messages.push_back( msg );
                    };

                    pattern.send_messages( realtime::realtime_start(), sender );
                    for ( auto i = 0; i < 3 * 4 * 24; ++i )
                    {
                        pattern.send_messages( realtime::realtime_clock(), sender );
                    }

                    THEN( "14 messages are received" )
                    {
                        REQUIRE( received_messages.size() == 14 );
                    }
                }

                AND_WHEN( "loop_length is set to 8" )
                {
                    pattern.set_loop_length( 8 );

                    AND_WHEN( "3*4*24 clock messages are sent" )
                    {
                        std::vector< message_t< 3 > > received_messages;
                        const auto sender = [&received_messages]( const auto& msg ) {
                            received_messages.push_back( msg );
                        };

                        pattern.send_messages( realtime::realtime_start(), sender );
                        for ( auto i = 0; i < 3 * 4 * 24; ++i )
                        {
                            pattern.send_messages( realtime::realtime_clock(), sender );
                        }

                        THEN( "12 messages are received" )
                        {
                            REQUIRE( received_messages.size() == 12 );
                        }
                    }
                }

                AND_WHEN( "loop_length is set to 0" )
                {
                    pattern.set_loop_length( 0 );

                    AND_WHEN( "3*4*24 clock messages are sent" )
                    {
                        std::vector< message_t< 3 > > received_messages;
                        const auto sender = [&received_messages]( const auto& msg ) {
                            received_messages.push_back( msg );
                        };

                        pattern.send_messages( realtime::realtime_start(), sender );
                        for ( auto i = 0; i < 3 * 4 * 24; ++i )
                        {
                            pattern.send_messages( realtime::realtime_clock(), sender );
                        }

                        THEN( "14 messages are received" )
                        {
                            REQUIRE( received_messages.size() == 14 );
                        }
                    }
                }
            }
        }
    }
}
