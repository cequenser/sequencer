#include <sequencer/beat_time_point.hpp>

#include <catch2/catch.hpp>

using namespace sequencer;

SCENARIO( "Construction", "[beat_time_point]" )
{
    GIVEN( "a default constructed beat_time_point" )
    {
        constexpr beat_time_point t;

        THEN( "time_since_epoch is zero" )
        {
            STATIC_REQUIRE( t.time_since_epoch() == 0_beats );
        }
    }
    GIVEN( "a beat_duration of 3 beats" )
    {
        constexpr beat_duration three( 3.0 );
        WHEN( "a beat_time_point is constructed" )
        {
            constexpr beat_time_point t( three );
            THEN( "time_since_epoch is three" )
            {
                STATIC_REQUIRE( t.time_since_epoch() == 3_beats );
            }
        }
    }
}

SCENARIO( "Addition", "[beat_time_point]" )
{

    GIVEN( "a beat_time_point of 16 since epoch" )
    {
        beat_time_point time_point( 16_beats );

        WHEN( "there is a duration of 13 beats" )
        {
            constexpr auto duration = 13_beats;
            THEN( "in-place summation yields 29 beats" )
            {
                time_point += duration;
                REQUIRE( time_point.time_since_epoch() == 29_beats );
            }
        }
    }

    GIVEN( "a constexpr beat_time_point of 16 since epoch" )
    {
        constexpr beat_time_point time_point( 16_beats );
        WHEN( "there is a constexpr duration of 13 beats" )
        {
            constexpr beat_duration thirteen_beats{13.0};
            THEN( "summation yields 29 beats" )
            {
                constexpr auto twenty_nine_beats = time_point + thirteen_beats;
                STATIC_REQUIRE( twenty_nine_beats.time_since_epoch() == 29.0_beats );
            }

            THEN( "thirteen beats is smaller than sixteen beats" )
            {

                STATIC_REQUIRE( beat_time_point( thirteen_beats ) < time_point );
            }

            THEN( "sixteen beats is not smaller than thirteen beats" )
            {
                STATIC_REQUIRE_FALSE( time_point < beat_time_point( thirteen_beats ) );
            }

            THEN( "sixteen beats is not smaller than sixteen beats" )
            {
                STATIC_REQUIRE_FALSE( time_point < time_point );
            }
        }
    }
}
