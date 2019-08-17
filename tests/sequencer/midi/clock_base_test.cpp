#include <sequencer/midi/clock_base.hpp>

#include <catch2/catch.hpp>

SCENARIO( "start and stop messages", "[midi_clock_base]" )
{
    using namespace sequencer;

    GIVEN( "A midi_clock" )
    {
        midi::clock_base clock;

        WHEN( "the clock is not started" )
        {
            THEN( "nothing happens when update is called" )
            {
                clock.update( beat_time_point( 1.0_beats ), []( auto ) { throw 1; } );
            }
            THEN( "is_started returns false" )
            {
                REQUIRE_FALSE( clock.is_started() );
            }
            THEN( "is_started still returns false after stop" )
            {
                clock.stop();
                REQUIRE_FALSE( clock.is_started() );

                THEN( "nothing happens when update is called" )
                {
                    clock.update( beat_time_point( 1.0_beats ), []( auto ) { throw 1; } );
                }
            }
            THEN( "is_started still returns false after reset" )
            {
                clock.reset();
                REQUIRE_FALSE( clock.is_started() );

                THEN( "nothing happens when update is called" )
                {
                    clock.update( beat_time_point( 1.0_beats ), []( auto ) { throw 1; } );
                }
            }
        }

        WHEN( "the clock is started" )
        {
            clock.start();
            THEN( "a start message is sent when update is called" )
            {
                std::array< std::byte, 1 > received = {std::byte{0x00}};
                clock.update( beat_time_point( 0.0_beats ),
                              [&received]( auto message ) { received = message; } );
                REQUIRE( received == midi::realtime::realtime_start() );

                THEN( "nothing happens when update is called again" )
                {
                    clock.update( beat_time_point( 0.0_beats ), []( auto ) { throw 1; } );
                }

                THEN( "is_started returns false after stop" )
                {
                    clock.stop();
                    REQUIRE_FALSE( clock.is_started() );

                    THEN( "a stop message is sent when update is called again" )
                    {
                        std::array< std::byte, 1 > received = {std::byte{0x00}};
                        clock.update( beat_time_point( 1.0_beats ), [&received]( auto message ) {
                            REQUIRE( received.front() == std::byte{0x00} );
                            received = message;
                        } );
                        REQUIRE( received == midi::realtime::realtime_stop() );

                        THEN( "nothing happens when update is called again" )
                        {
                            clock.update( beat_time_point( 1.0_beats ), []( auto ) { throw 1; } );
                        }
                    }
                }

                THEN( "is_started returns false after reset" )
                {
                    clock.reset();
                    REQUIRE_FALSE( clock.is_started() );

                    std::array< std::byte, 1 > received = {std::byte{0x00}};
                    clock.update( beat_time_point( 1.0_beats ), [&received]( auto message ) {
                        REQUIRE( received.front() == std::byte{0x00} );
                        received = message;
                    } );
                    REQUIRE( received == midi::realtime::realtime_stop() );

                    THEN( "nothing happens when update is called again" )
                    {
                        clock.update( beat_time_point( 1.0_beats ), []( auto ) { throw 1; } );
                    }
                }
            }
            THEN( "is_started returns true" )
            {
                REQUIRE( clock.is_started() );
            }
        }
    }

    GIVEN( "A midi clock that starts at -4.0 beats" )
    {
        midi::clock_base clock( beat_time_point( -4.0_beats ) );
        CHECK( clock.pulses_per_quarter_note() == 24 );
        clock.start();

        WHEN( "update is called at 1.0 beat " )
        {
            THEN( "the clock sends 5 x 24 pulses" )
            {
                int pulse_count = 0;
                clock.update( beat_time_point( 1.0_beats ), [&pulse_count]( auto message ) {
                    if ( message == midi::realtime::realtime_clock() )
                    {
                        pulse_count++;
                    }
                } );
                REQUIRE( pulse_count == 5 * clock.pulses_per_quarter_note() );

                WHEN( "update is called at 1.0 beat after reset" )
                {
                    clock.reset();
                    clock.start();
                    THEN( "the clock sends 5 x 24 pulses again" )
                    {
                        int pulse_count = 0;
                        clock.update( beat_time_point( 1.0_beats ), [&pulse_count]( auto message ) {
                            if ( message == midi::realtime::realtime_clock() )
                            {
                                pulse_count++;
                            }
                        } );
                        REQUIRE( pulse_count == 5 * clock.pulses_per_quarter_note() );
                    }
                }
            }
        }
    }
}

SCENARIO( "continue", "[midi_clock_base]" )
{
    using namespace sequencer;

    GIVEN( "a started midi clock" )
    {
        midi::clock_base clock;
        clock.start();
        clock.update( beat_time_point( 0.0_beats ), []( auto ) {} );

        WHEN( "the clock is stopped" )
        {
            clock.stop();
            clock.update( beat_time_point( 0.0_beats ), []( auto ) {} );

            THEN( "starting the clock again sends a continue message" )
            {
                clock.start();
                std::array< std::byte, 1 > received = {std::byte{0x00}};
                clock.update( beat_time_point( 0.0_beats ), [&received]( auto message ) {
                    REQUIRE( received.front() == std::byte{0x00} );
                    received = message;
                } );
                REQUIRE( received == midi::realtime::realtime_continue() );
            }
        }

        WHEN( "the clock is reset" )
        {
            clock.reset();
            clock.update( beat_time_point( 0.0_beats ), []( auto ) {} );

            THEN( "starting the clock again sends a start message" )
            {
                clock.start();
                std::array< std::byte, 1 > received = {std::byte{0x00}};
                clock.update( beat_time_point( 0.0_beats ), [&received]( auto message ) {
                    REQUIRE( received.front() == std::byte{0x00} );
                    received = message;
                } );
                REQUIRE( received == midi::realtime::realtime_start() );
            }
        }
    }
}

SCENARIO( "pulses", "[midi_clock_base]" )
{
    using namespace sequencer;

    GIVEN( "a started midi clock" )
    {
        midi::clock_base clock;
        clock.start();
        WHEN( "update is called " )
        {
            THEN( "the clock sends 24 pulses" )
            {
                int pulse_count = 0;
                clock.update( beat_time_point( 1.0_beats ), [&pulse_count]( auto message ) {
                    if ( message == midi::realtime::realtime_clock() )
                    {
                        pulse_count++;
                    }
                } );
                REQUIRE( pulse_count == 24 );
                WHEN( "update is called again with duration of 1 beat" )
                {
                    THEN( "no pulses are send" )
                    {
                        pulse_count = 0;
                        clock.update( beat_time_point( 1.0_beats ), [&pulse_count]( auto message ) {
                            if ( message == midi::realtime::realtime_clock() )
                            {
                                pulse_count++;
                            }
                        } );
                        REQUIRE( pulse_count == 0 );
                    }
                }

                WHEN( "update is called again with time point of 1 beat after reset" )
                {
                    clock.reset();
                    clock.start();
                    THEN( "24 pulses are send" )
                    {
                        pulse_count = 0;
                        clock.update( beat_time_point( 1.0_beats ), [&pulse_count]( auto message ) {
                            if ( message == midi::realtime::realtime_clock() )
                            {
                                pulse_count++;
                            }
                        } );
                        REQUIRE( pulse_count == 24 );
                    }
                }
            }
        }
    }
    GIVEN( "a started midi clock with 36 pulses per quarter" )
    {
        midi::clock_base clock{beat_time_point{beat_duration{0}}, 36};
        REQUIRE( clock.pulses_per_quarter_note() == 36 );
        clock.start();
        WHEN( "update is called at 1.0 beat " )
        {
            THEN( "the clock sends 36 pulses" )
            {
                int pulse_count = 0;
                clock.update( beat_time_point( 1.0_beats ), [&pulse_count]( auto message ) {
                    if ( message == midi::realtime::realtime_clock() )
                    {
                        pulse_count++;
                    }
                } );
                REQUIRE( pulse_count == clock.pulses_per_quarter_note() );
            }
        }
    }
}
