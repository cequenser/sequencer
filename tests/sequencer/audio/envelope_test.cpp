#include <sequencer/audio/envelope.hpp>

#include <catch2/catch.hpp>

SCENARIO( "envelope construction and getter/setter", "[envelope]" )
{
    using namespace sequencer::audio;

    GIVEN( "a default constructed envelope" )
    {
        auto envelope = envelope_t{};

        THEN( "attack is zero" )
        {
            CHECK( envelope.attack() == 0 );

            AND_WHEN( "attack is set to 10" )
            {
                envelope.set_attack( 10 );

                THEN( "attack is 10" )
                {
                    CHECK( envelope.attack() == 10 );
                }
            }
        }

        THEN( "sustain is std::numeric_limits<double>::max()" )
        {
            CHECK( envelope.sustain() == std::numeric_limits< double >::max() );

            AND_WHEN( "sustain is set to 10" )
            {
                envelope.set_sustain( 10 );

                THEN( "sustain is 10" )
                {
                    CHECK( envelope.sustain() == 10 );
                }
            }
        }

        THEN( "decay is zero" )
        {
            CHECK( envelope.decay() == 0 );

            AND_WHEN( "decay is set to 10" )
            {
                envelope.set_decay( 10 );

                THEN( "decay is 10" )
                {
                    CHECK( envelope.decay() == 10 );
                }
            }
        }
    }
}

SCENARIO( "envelope", "[envelope]" )
{
    using namespace sequencer::audio;

    GIVEN( "a default constructed envelope" )
    {
        auto envelope = envelope_t{};

        THEN( "call operator always returns 1" )
        {
            CHECK( envelope( 0 ) == 1 );
            CHECK( envelope( 0.1 ) == 1 );
            CHECK( envelope( 0.234231 ) == 1 );
            CHECK( envelope( 0.7546 ) == 1 );
            CHECK( envelope( 1 ) == 1 );
        }

        AND_WHEN( "attack is changed to 0.5" )
        {
            envelope.set_attack( 0.5 );

            THEN( "envelope at 0 is 0" )
            {
                CHECK( envelope( 0 ) == 0 );
            }

            THEN( "envelope at 0.25 is 0.5" )
            {
                CHECK( envelope( 0.25 ) == Approx( 0.5 ) );
            }

            THEN( "envelope at 0.5 is 1" )
            {
                CHECK( envelope( 0.5 ) == 1 );
            }

            THEN( "envelope at 0.75 is 1" )
            {
                CHECK( envelope( 0.75 ) == 1 );
            }

            THEN( "envelope at 1 is 1" )
            {
                CHECK( envelope( 1 ) == 1 );
            }

            AND_WHEN( "sustain is set to 0.25" )
            {
                envelope.set_sustain( 0.25 );

                THEN( "envelope at 0 is 0" )
                {
                    CHECK( envelope( 0 ) == 0 );
                }

                THEN( "envelope at 0.25 is 0.5" )
                {
                    CHECK( envelope( 0.25 ) == Approx( 0.5 ) );
                }

                THEN( "envelope at 0.5 is 1" )
                {
                    CHECK( envelope( 0.5 ) == 1 );
                }

                THEN( "envelope at 0.75 is 1" )
                {
                    CHECK( envelope( 0.75 ) == 0 );
                }

                THEN( "envelope at 1 is 1" )
                {
                    CHECK( envelope( 1 ) == 0 );
                }

                AND_WHEN( "decay is set to 0.1" )
                {
                    envelope.set_decay( 0.1 );

                    THEN( "envelope at 0 is 0" )
                    {
                        CHECK( envelope( 0 ) == 0 );
                    }

                    THEN( "envelope at 0.25 is 0.5" )
                    {
                        CHECK( envelope( 0.25 ) == Approx( 0.5 ) );
                    }

                    THEN( "envelope at 0.5 is 1" )
                    {
                        CHECK( envelope( 0.5 ) == 1 );
                    }

                    THEN( "envelope at 0.75 is 1" )
                    {
                        CHECK( envelope( 0.75 ) == 1 );
                    }

                    THEN( "envelope at 0.8 is 0.5" )
                    {
                        CHECK( envelope( 0.8 ) == Approx( 0.5 ) );
                    }

                    THEN( "envelope at 0.85 is 0" )
                    {
                        CHECK( envelope( 0.85 ) == 0 );
                    }

                    THEN( "envelope at 1 is 1" )
                    {
                        CHECK( envelope( 1 ) == 0 );
                    }
                }
            }
        }
    }
}
