#include <sequencer/audio/delay.hpp>

#include <catch2/catch.hpp>

SCENARIO( "delay", "[delay]" )
{
    using namespace sequencer::audio;

    GIVEN( "delay with frame count 4" )
    {
        auto delay = delay_t{4};

        WHEN( "delay is called" )
        {
            delay( 1.0f );
            delay( 2.0f );
            delay( 3.0f );
            delay( 0.0f );

            THEN( "delay(0.5) on 5th frame returns 1" )
            {
                CHECK( delay( 0.5f ) == 1.0f );

                AND_THEN( "delay(0.0) on 6th frame returns 2" )
                {
                    CHECK( delay( 0.0 ) == 2.0f );

                    AND_THEN( "delay(1.0) on 7th frame returns 3" )
                    {
                        CHECK( delay( 1.0 ) == 3.0f );

                        AND_THEN( "delay(0.0) on 8th frame returns 0" )
                        {
                            CHECK( delay( 0.0 ) == 0.0f );
                        }
                    }
                }
            }
        }
    }

    GIVEN( "delay with frame count 4 and gain 0.5" )
    {
        auto delay = delay_t{4};
        delay.set_gain( 0.5 );

        WHEN( "delay is called" )
        {
            delay( 1.0f );
            delay( 2.0f );
            delay( 3.0f );
            delay( 0.0f );

            THEN( "delay(0.5) on 5th frame returns 0.5" )
            {
                CHECK( delay( 0.5f ) == Approx( 0.5f ) );

                AND_THEN( "delay(0.0) on 6th frame returns 1" )
                {
                    CHECK( delay( 0.0 ) == Approx( 1.0f ) );

                    AND_THEN( "delay(1.0) on 7th frame returns 1.5" )
                    {
                        CHECK( delay( 1.0 ) == Approx( 1.5f ) );

                        AND_THEN( "delay(0.0) on 8th frame returns 0" )
                        {
                            CHECK( delay( 0.0 ) == 0.0f );
                        }
                    }
                }
            }
        }
    }

    GIVEN( "delay with frame count 4" )
    {
        auto delay = delay_t{4};

        WHEN( "delay is called" )
        {
            delay( 1.0f );
            delay( 2.0f );
            delay( 3.0f );
            delay( 0.0f );

            AND_WHEN( "reset is called" )
            {
                delay.reset();

                THEN( "delay(0.5) on 5th frame returns 0" )
                {
                    CHECK( delay( 0.5f ) == 0.0f );

                    AND_THEN( "delay(0.0) on 6th frame returns 0" )
                    {
                        CHECK( delay( 0.0 ) == 0.0f );

                        AND_THEN( "delay(1.0) on 7th frame returns 0" )
                        {
                            CHECK( delay( 1.0 ) == 0.0f );

                            AND_THEN( "delay(0.0) on 8th frame returns 0" )
                            {
                                CHECK( delay( 0.0 ) == 0.0f );
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "repeated delay", "[delay]" )
{
    using namespace sequencer::audio;

    GIVEN( "twice repeated delay with frame count 4 and gain 0.5" )
    {
        auto delay = repeated_delay_t< delay_t >{2, 4, 0.5};

        WHEN( "delay is called" )
        {
            delay( 1.0f );
            delay( 2.0f );
            delay( 3.0f );
            delay( 0.0f );

            THEN( "delay(0.5) on 5th frame returns 0.5" )
            {
                CHECK( delay( 0.0f ) == Approx( 0.5f ) );

                AND_THEN( "delay(0.0) on 6th frame returns 1" )
                {
                    CHECK( delay( 0.0 ) == Approx( 1.0f ) );

                    AND_THEN( "delay(1.0) on 7th frame returns 1.5" )
                    {
                        CHECK( delay( 0.0 ) == Approx( 1.5f ) );

                        AND_THEN( "delay(0.0) on 8th frame returns 0" )
                        {
                            CHECK( delay( 0.0 ) == 0.0f );

                            THEN( "delay(0.5) on 9th frame returns 0.5" )
                            {
                                CHECK( delay( 0.0f ) == Approx( 0.25f ) );

                                AND_THEN( "delay(0.0) on 10th frame returns 1" )
                                {
                                    CHECK( delay( 0.0 ) == Approx( 0.5f ) );

                                    AND_THEN( "delay(1.0) on 11th frame returns 1.5" )
                                    {
                                        CHECK( delay( 0.0 ) == Approx( 0.75f ) );

                                        AND_THEN( "delay(0.0) on 12th frame returns 0" )
                                        {
                                            CHECK( delay( 0.0 ) == Approx( 0.0f ) );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "stereo delay", "[delay]" )
{
    using namespace sequencer::audio;

    GIVEN( "delay with frame count 4" )
    {
        auto delay = stereo_delay_t< delay_t >{4};

        WHEN( "delay is called" )
        {
            delay( 1.0f );
            delay( 2.0f );
            delay( 3.0f );
            delay( 0.0f );

            THEN( "delay(0.5) on 5th frame returns 0.5 left and right" )
            {
                const auto [ left, right ] = delay( 0.5f );
                CHECK( left == Approx( 0.5f ) );
                CHECK( right == Approx( 0.5f ) );

                AND_THEN( "delay(0.0) on 6th frame returns 1 left and right" )
                {
                    const auto [ left, right ] = delay( 0.0f );
                    CHECK( left == Approx( 1.0f ) );
                    CHECK( right == Approx( 1.0f ) );

                    AND_THEN( "delay(1.0) on 7th frame returns 1.5 left and right" )
                    {
                        const auto [ left, right ] = delay( 0.0f );
                        CHECK( left == Approx( 1.5f ) );
                        CHECK( right == Approx( 1.5f ) );

                        AND_THEN( "delay(0.0) on 8th frame returns 0 left and right" )
                        {
                            const auto [ left, right ] = delay( 0.0f );
                            CHECK( left == Approx( 0.0f ) );
                            CHECK( right == Approx( 0.0f ) );
                        }
                    }
                }
            }
        }

        WHEN( "stereo ratio is set to 0" )
        {
            delay.set_stereo_ratio( 0 );

            WHEN( "delay is called" )
            {
                delay( 1.0f );
                delay( 2.0f );
                delay( 3.0f );
                delay( 0.0f );

                THEN( "delay(0.5) on 5th frame returns 0.5 left and right" )
                {
                    const auto [ left, right ] = delay( 0.5f );
                    CHECK( left == Approx( 1.0f ) );
                    CHECK( right == Approx( 0.0f ) );

                    AND_THEN( "delay(0.0) on 6th frame returns 1 left and right" )
                    {
                        const auto [ left, right ] = delay( 0.0f );
                        CHECK( left == Approx( 2.0f ) );
                        CHECK( right == Approx( 0.0f ) );

                        AND_THEN( "delay(1.0) on 7th frame returns 1.5 left and right" )
                        {
                            const auto [ left, right ] = delay( 0.0f );
                            CHECK( left == Approx( 3.0f ) );
                            CHECK( right == Approx( 0.0f ) );

                            AND_THEN( "delay(0.0) on 8th frame returns 0 left and right" )
                            {
                                const auto [ left, right ] = delay( 0.0f );
                                CHECK( left == Approx( 0.0f ) );
                                CHECK( right == Approx( 0.0f ) );
                            }
                        }
                    }
                }
            }
        }
    }
}
