#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/chrono/testing_clock.hpp>
#include <sequencer/midi/clock.hpp>
#include <sequencer/midi/message/channel_mode.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/step_sequencer_base.hpp>
#include <sequencer/midi/track.hpp>

#include <catch2/catch.hpp>

#include <iostream>
#include <vector>
using sequencer::midi::step_sequencer_base;
using sequencer::midi::track;
using sequencer::midi::channel::mode::all_notes_off;
using sequencer::midi::channel::voice::note_off;
using sequencer::midi::channel::voice::note_on;

using underlying_clock_type = sequencer::chrono::steady_testing_clock<>;
using sequencer_clock_type = sequencer::chrono::sequencer_clock< const underlying_clock_type& >;

SCENARIO( "step_sequencer_base plays 4 beats", "[step_sequencer]" )
{
    using namespace sequencer;

    GIVEN( "a step sequencer with an empty track" )
    {
        std::vector< midi::message_type > received_messages;
        const auto midi_sender = [&received_messages]( midi::message_type message ) {
            received_messages.push_back( message );
        };

        constexpr auto steps = 4u;
        const auto midi_track = track< steps >{};
        auto step_sequencer = step_sequencer_base{midi_track, midi_sender};

        WHEN( "sequencer receives start message and 96 clock messages (i.e. 4 beats @ 24 pulses "
              "per beat)" )
        {
            step_sequencer.update( midi::real_time::message_type::realtime_start );
            for ( auto i = 0u; i < 96; ++i )
            {
                step_sequencer.update( midi::real_time::message_type::realtime_clock );
            }

            THEN( "no message is sent" )
            {
                CHECK( received_messages.empty() );
            }
        }
    }

    GIVEN( "a step sequencer a track with notes on first two quarters" )
    {
        std::vector< midi::message_type > received_messages;
        const auto midi_sender = [&received_messages]( midi::message_type message ) {
            received_messages.push_back( message );
        };

        constexpr auto steps = 16u;
        auto midi_track = track< steps >{};
        const auto note_1 = 0;
        const auto note_2 = 42;
        midi_track[ 0 ] = note_1;
        midi_track[ 4 ] = note_2;
        auto step_sequencer = step_sequencer_base{midi_track, midi_sender};

        WHEN( "sequencer receives start message and one clock message" )
        {
            step_sequencer.update( midi::real_time::message_type::realtime_start );
            step_sequencer.update( midi::real_time::message_type::realtime_clock );

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 1 );
                CHECK( received_messages.front() == note_on( 0, note_1, 0 ) );
            }
        }

        WHEN( "sequencer receives no start message and one clock message" )
        {
            step_sequencer.update( midi::real_time::message_type::realtime_clock );

            THEN( "one note on message is send" )
            {
                CHECK( received_messages.empty() );
            }
        }

        WHEN( "sequencer receives start message and 24 clock messages" )
        {
            step_sequencer.update( midi::real_time::message_type::realtime_start );
            for ( auto i = 0u; i < 23; ++i )
            {
                step_sequencer.update( midi::real_time::message_type::realtime_clock );
            }

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 1 );
                CHECK( received_messages.front() == note_on( 0, note_1, 0 ) );
            }
        }

        WHEN( "sequencer receives start message and 25 clock messages" )
        {
            step_sequencer.update( midi::real_time::message_type::realtime_start );
            for ( auto i = 0u; i < 25; ++i )
            {
                step_sequencer.update( midi::real_time::message_type::realtime_clock );
            }

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 3 );
                CHECK( received_messages[ 0 ] == note_on( 0, note_1, 0 ) );

                THEN( "one note off message is send" )
                {
                    CHECK( received_messages[ 1 ] == note_off( 0, note_1, 0 ) );

                    THEN( "one note on message is send" )
                    {
                        CHECK( received_messages[ 2 ] == note_on( 0, note_2, 0 ) );
                    }
                }
            }
        }

        WHEN( "sequencer receives start message and 24 clock messages" )
        {
            step_sequencer.update( midi::real_time::message_type::realtime_start );
            for ( auto i = 0u; i < 24; ++i )
            {
                step_sequencer.update( midi::real_time::message_type::realtime_clock );
            }

            WHEN( "sequencer receives stop message one clock message" )
            {
                step_sequencer.update( midi::real_time::message_type::realtime_stop );
                step_sequencer.update( midi::real_time::message_type::realtime_clock );

                THEN( "one note on and all notes off message are send" )
                {
                    REQUIRE( received_messages.size() == 2 );
                    CHECK( received_messages[ 0 ] == note_on( 0, note_1, 0 ) );
                    CHECK( received_messages[ 1 ] == all_notes_off( 0 ) );
                }

                WHEN( "sequencer receives continue message and 25 clock messages" )
                {
                    step_sequencer.update( midi::real_time::message_type::realtime_continue );
                    for ( auto i = 0u; i < 25; ++i )
                    {
                        step_sequencer.update( midi::real_time::message_type::realtime_clock );
                    }

                    THEN( "one note off message is send" )
                    {
                        REQUIRE( received_messages.size() == 3 );
                        CHECK( received_messages[ 2 ] == note_on( 0, note_2, 0 ) );
                    }
                }

                WHEN( "sequencer receives start message and 25 clock messages" )
                {
                    step_sequencer.update( midi::real_time::message_type::realtime_start );
                    for ( auto i = 0u; i < 25; ++i )
                    {
                        step_sequencer.update( midi::real_time::message_type::realtime_clock );
                    }

                    THEN( "one note on message is send" )
                    {
                        REQUIRE( received_messages.size() == 5 );
                        CHECK( received_messages[ 2 ] == note_on( 0, note_1, 0 ) );

                        THEN( "one note off message is send" )
                        {
                            CHECK( received_messages[ 3 ] == note_off( 0, note_1, 0 ) );

                            THEN( "one note on message is send" )
                            {
                                CHECK( received_messages[ 4 ] == note_on( 0, note_2, 0 ) );
                            }
                        }
                    }
                }
            }
        }

        WHEN( "sequencer receives start message and 96 clock messages" )
        {
            step_sequencer.update( midi::real_time::message_type::realtime_start );
            for ( auto i = 0u; i < 96; ++i )
            {
                step_sequencer.update( midi::real_time::message_type::realtime_clock );
            }

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 3 );
                CHECK( received_messages[ 0 ] == note_on( 0, note_1, 0 ) );

                THEN( "one note off message is send" )
                {
                    CHECK( received_messages[ 1 ] == note_off( 0, note_1, 0 ) );

                    THEN( "one note on message is send" )
                    {
                        CHECK( received_messages[ 2 ] == note_on( 0, note_2, 0 ) );
                    }
                }
            }
        }
    }
}
