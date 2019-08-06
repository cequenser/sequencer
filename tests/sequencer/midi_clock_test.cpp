#include <sequencer/midi_clock.hpp>

#include <catch2/catch.hpp>

SCENARIO( "start and stop messages", "[midi_clock]" )
{
    using namespace sequencer;

    GIVEN( "A midi_clock" )
    {
        midi_clock clock;

        WHEN( "the clock is not started" )
        {
            THEN( "nothing happens when update is called" )
            {
                clock.update( beat_time_point( 1.0_beats ), []( midi::message_type ) { throw 1; } );
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
                    clock.update( beat_time_point( 1.0_beats ),
                                  []( midi::message_type ) { throw 1; } );
                }
            }
            THEN( "is_started still returns false after reset" )
            {
                clock.reset();
                REQUIRE_FALSE( clock.is_started() );

                THEN( "nothing happens when update is called" )
                {
                    clock.update( beat_time_point( 1.0_beats ),
                                  []( midi::message_type ) { throw 1; } );
                }
            }
        }

        WHEN( "the clock is started" )
        {
            clock.start();
            THEN( "a start message is sent when update is called" )
            {
                midi::message_type message = midi::message_type::invalid;
                clock.update( beat_time_point( 0.0_beats ),
                              [& msg = message]( midi::message_type message ) { msg = message; } );
                REQUIRE( message == midi::message_type::realtime_start );

                THEN( "nothing happens when update is called again" )
                {
                    clock.update( beat_time_point( 0.0_beats ),
                                  []( midi::message_type ) { throw 1; } );
                }

                THEN( "is_started returns false after stop" )
                {
                    clock.stop();
                    REQUIRE_FALSE( clock.is_started() );

                    THEN( "a stop message is sent when update is called again" )
                    {
                        midi::message_type received = midi::message_type::invalid;
                        clock.update( beat_time_point( 1.0_beats ),
                                      [&received]( midi::message_type message ) {
                                          REQUIRE( received == midi::message_type::invalid );
                                          received = message;
                                      } );
                        REQUIRE( received == midi::message_type::realtime_stop );

                        THEN( "nothing happens when update is called again" )
                        {
                            clock.update( beat_time_point( 1.0_beats ),
                                          []( midi::message_type ) { throw 1; } );
                        }
                    }
                }

                THEN( "is_started returns false after reset" )
                {
                    clock.reset();
                    REQUIRE_FALSE( clock.is_started() );

                    midi::message_type received = midi::message_type::invalid;
                    clock.update( beat_time_point( 1.0_beats ),
                                  [&received]( midi::message_type message ) {
                                      REQUIRE( received == midi::message_type::invalid );
                                      received = message;
                                  } );
                    REQUIRE( received == midi::message_type::realtime_stop );

                    THEN( "nothing happens when update is called again" )
                    {
                        clock.update( beat_time_point( 1.0_beats ),
                                      []( midi::message_type ) { throw 1; } );
                    }
                }
            }
            THEN( "is_started returns true" )
            {
                REQUIRE( clock.is_started() );
            }
        }
    }

    GIVEN( "A midi_clock that starts at -4.0 beats" )
    {
        midi_clock clock( beat_time_point( -4.0_beats ) );
        CHECK( clock.pulses_per_quarter_note() == 24.0 );
        clock.start();

        WHEN( "update is called at 1.0 beat " )
        {
            THEN( "the clock sends 5 x 24 pulses" )
            {
                int pulse_count = 0;
                clock.update( beat_time_point( 1.0_beats ),
                              [&pulse_count]( midi::message_type message ) {
                                  if ( message == midi::message_type::realtime_clock )
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
                        clock.update( beat_time_point( 1.0_beats ),
                                      [&pulse_count]( midi::message_type message ) {
                                          if ( message == midi::message_type::realtime_clock )
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

SCENARIO( "continue", "[midi_clock]" )
{
    using namespace sequencer;

    GIVEN( "a started midi_clock" )
    {
        midi_clock clock;
        clock.start();
        clock.update( beat_time_point( 0.0_beats ), []( midi::message_type ) {} );

        WHEN( "the clock is stopped" )
        {
            clock.stop();
            clock.update( beat_time_point( 0.0_beats ), []( midi::message_type ) {} );

            THEN( "starting the clock again sends a continue message" )
            {
                clock.start();
                midi::message_type received = midi::message_type::invalid;
                clock.update( beat_time_point( 0.0_beats ),
                              [&received]( midi::message_type message ) {
                                  REQUIRE( received == midi::message_type::invalid );
                                  received = message;
                              } );
                REQUIRE( received == midi::message_type::realtime_continue );
            }
        }

        WHEN( "the clock is reset" )
        {
            clock.reset();
            clock.update( beat_time_point( 0.0_beats ), []( midi::message_type ) {} );

            THEN( "starting the clock again sends a start message" )
            {
                clock.start();
                midi::message_type received = midi::message_type::invalid;
                clock.update( beat_time_point( 0.0_beats ),
                              [&received]( midi::message_type message ) {
                                  REQUIRE( received == midi::message_type::invalid );
                                  received = message;
                              } );
                REQUIRE( received == midi::message_type::realtime_start );
            }
        }
    }
}

SCENARIO( "pulses", "[midi_clock]" )
{
    using namespace sequencer;

    GIVEN( "a started midi_clock" )
    {
        midi_clock clock;
        clock.start();
        WHEN( "update is called " )
        {
            THEN( "the clock sends 24 pulses" )
            {
                int pulse_count = 0;
                clock.update( beat_time_point( 1.0_beats ),
                              [&pulse_count]( midi::message_type message ) {
                                  if ( message == midi::message_type::realtime_clock )
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
                        clock.update( beat_time_point( 1.0_beats ),
                                      [&pulse_count]( midi::message_type message ) {
                                          if ( message == midi::message_type::realtime_clock )
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
                        clock.update( beat_time_point( 1.0_beats ),
                                      [&pulse_count]( midi::message_type message ) {
                                          if ( message == midi::message_type::realtime_clock )
                                          {
                                              pulse_count++;
                                          }
                                      } );
                        REQUIRE( pulse_count == 24 );
                    }
                }
            }
        }

        WHEN( "pulses per quarter are changed to 36" )
        {
            REQUIRE( clock.pulses_per_quarter_note() == 24.0 );
            clock.set_pulses_per_quarter_note( 36.0 );
            REQUIRE( clock.pulses_per_quarter_note() == 36.0 );
            WHEN( "update is called at 1.0 beat " )
            {
                THEN( "the clock sends 36 pulses" )
                {
                    int pulse_count = 0;
                    clock.update( beat_time_point( 1.0_beats ),
                                  [&pulse_count]( midi::message_type message ) {
                                      if ( message == midi::message_type::realtime_clock )
                                      {
                                          pulse_count++;
                                      }
                                  } );
                    REQUIRE( pulse_count == clock.pulses_per_quarter_note() );
                }
            }
        }
    }
}
