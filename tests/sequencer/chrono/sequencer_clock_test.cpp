#include "testing_clock.hpp"

#include <sequencer/chrono/sequencer_clock.hpp>

#include <catch2/catch.hpp>

SCENARIO( "creating a sequencer clock", "[sequencer_clock]" )
{
    GIVEN( "a steady clock object" )
    {
        using underlying_clock_type = sequencer::chrono::steady_testing_clock<>;
        using sequencer_clock_type = sequencer::chrono::sequencer_clock< underlying_clock_type >;

        underlying_clock_type underlying_clock;
        underlying_clock.set(
            underlying_clock_type::time_point{underlying_clock_type::duration{42}} );

        WHEN( "a sequencer_clock is created with said clock" )
        {
            sequencer_clock_type sequencer_clock( underlying_clock );

            THEN( "the sequencer_clock is not running" )
            {

                REQUIRE( !sequencer_clock.is_running() );
            }

            THEN( "the sequencer_clock returns zero" )
            {
                REQUIRE( sequencer_clock.now().time_since_epoch() ==
                         sequencer_clock_type::duration::zero() );
            }
        }
    }
}

SCENARIO( "starting and stopping a sequencer clock", "[sequencer_clock]" )
{
    using underlying_clock_type = sequencer::chrono::steady_testing_clock<>;
    using sequencer_clock_type = sequencer::chrono::sequencer_clock< underlying_clock_type >;

    GIVEN( "a sequencer clock object" )
    {
        underlying_clock_type underlying_clock;
        sequencer_clock_type sequencer_clock( underlying_clock );

        WHEN( "sequencer_clock is started" )
        {
            sequencer_clock.start();

            THEN( "the sequencer_clock is running" )
            {
                REQUIRE( sequencer_clock.is_running() );
            }

            WHEN( "sequencer_clock is stopped" )
            {
                REQUIRE( sequencer_clock.is_running() );
                sequencer_clock.stop();

                THEN( "the sequencer_clock is not running" )
                {

                    REQUIRE( !sequencer_clock.is_running() );
                }
            }
        }
    }
}

SCENARIO( "running a sequencer clock", "[sequencer_clock]" )
{
    using underlying_clock_type = sequencer::chrono::steady_testing_clock<>;
    using sequencer_clock_type = sequencer::chrono::sequencer_clock< underlying_clock_type >;

    GIVEN( "a started sequencer clock" )
    {
        underlying_clock_type underlying_clock;
        sequencer_clock_type sequencer_clock( underlying_clock );

        sequencer_clock.start();

        WHEN( "5 seconds passed on the underlying clock" )
        {
            underlying_clock.set( underlying_clock.now() + std::chrono::seconds( 5 ) );

            THEN( "sequencer_clock returns 5 seconds" )
            {
                REQUIRE( sequencer_clock.now().time_since_epoch() == std::chrono::seconds( 5 ) );
            }

            WHEN( "the clock is stopped and another 3 seconds passed on the underlying clock" )
            {
                sequencer_clock.stop();
                underlying_clock.set( underlying_clock.now() + std::chrono::seconds( 3 ) );

                THEN( "sequencer_clock still returns 5 seconds" )
                {
                    REQUIRE( sequencer_clock.now().time_since_epoch() ==
                             std::chrono::seconds( 5 ) );
                }

                WHEN( "the clock is stopped again" )
                {
                    sequencer_clock.stop();

                    THEN( "sequencer_clock still returns 5 seconds" )
                    {
                        REQUIRE( sequencer_clock.now().time_since_epoch() ==
                                 std::chrono::seconds( 5 ) );
                    }
                }

                WHEN( "the clock is started again and another 3 seconds pass" )
                {
                    sequencer_clock.start();
                    underlying_clock.set( underlying_clock.now() + std::chrono::seconds( 3 ) );

                    THEN( "sequencer_clock returns 8 seconds" )
                    {
                        REQUIRE( sequencer_clock.now().time_since_epoch() ==
                                 std::chrono::seconds( 8 ) );
                    }

                    WHEN( "the clock is stopped and started again and another 4 seconds pass" )
                    {
                        sequencer_clock.stop();
                        sequencer_clock.start();
                        underlying_clock.set( underlying_clock.now() + std::chrono::seconds( 4 ) );

                        THEN( "sequencer_clock returns 12 seconds" )
                        {
                            REQUIRE( sequencer_clock.now().time_since_epoch() ==
                                     std::chrono::seconds( 12 ) );
                        }
                    }
                }
            }
        }

        WHEN( "sequencer_clock is started and reset" )
        {
            sequencer_clock.start();
            underlying_clock.set( underlying_clock.now() + std::chrono::seconds( 4 ) );
            sequencer_clock.reset();

            THEN( "the sequencer_clock is not running" )
            {

                REQUIRE( !sequencer_clock.is_running() );
            }

            THEN( "the sequencer_clock returns zero" )
            {
                REQUIRE( sequencer_clock.now().time_since_epoch() ==
                         sequencer_clock_type::duration::zero() );
            }

            WHEN( "sequencer_clock is restarted after reset" )
            {
                sequencer_clock.start();

                THEN( "the sequencer_clock returns zero" )
                {
                    REQUIRE( sequencer_clock.now().time_since_epoch() ==
                             sequencer_clock_type::duration::zero() );
                }
            }
        }

        WHEN( "sequencer_clock is started, stopped and reset" )
        {
            sequencer_clock.start();
            underlying_clock.set( underlying_clock.now() + std::chrono::seconds( 4 ) );
            sequencer_clock.stop();
            sequencer_clock.reset();

            THEN( "the sequencer_clock is not running" )
            {

                REQUIRE( !sequencer_clock.is_running() );
            }

            THEN( "the sequencer_clock returns zero" )
            {
                REQUIRE( sequencer_clock.now().time_since_epoch() ==
                         sequencer_clock_type::duration::zero() );
            }

            WHEN( "sequencer_clock is restarted after reset" )
            {
                sequencer_clock.start();

                THEN( "the sequencer_clock returns zero" )
                {
                    REQUIRE( sequencer_clock.now().time_since_epoch() ==
                             sequencer_clock_type::duration::zero() );
                }
            }
        }
    }
}

SCENARIO( "restarting a running sequencer clock has no effect", "[sequencer_clock]" )
{
    using underlying_clock_type = sequencer::chrono::steady_testing_clock<>;
    using sequencer_clock_type = sequencer::chrono::sequencer_clock< underlying_clock_type >;

    GIVEN( "a started sequencer clock" )
    {
        underlying_clock_type underlying_clock;
        sequencer_clock_type sequencer_clock( underlying_clock );

        sequencer_clock.start();

        WHEN( "5 seconds passed on the underlying clock" )
        {
            underlying_clock.set( underlying_clock.now() + std::chrono::seconds( 5 ) );

            THEN( "sequencer_clock returns 5 seconds" )
            {
                REQUIRE( sequencer_clock.now().time_since_epoch() == std::chrono::seconds( 5 ) );
            }

            WHEN(
                "the clock is started again and another 3 seconds passed on the underlying clock" )
            {
                sequencer_clock.start();
                underlying_clock.set( underlying_clock.now() + std::chrono::seconds( 3 ) );

                THEN( "sequencer_clock returns 8 seconds" )
                {
                    REQUIRE( sequencer_clock.now().time_since_epoch() ==
                             std::chrono::seconds( 8 ) );
                }
            }
        }
    }
}
