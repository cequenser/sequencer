#include "testing_clock.hpp"

#include <catch2/catch.hpp>

SCENARIO( "testing_clock.now() returns a specified value", "[testing_clock]" )
{
    GIVEN( "a default, non-steady testing_clock" )
    {
        using clock_type = sequencer::chrono::testing_clock<>;

        clock_type clock;

        constexpr auto value = clock_type::time_point{clock_type::duration{42}};
        constexpr auto other_value = clock_type::time_point{clock_type::duration{23}};
        static_assert( value != other_value );

        WHEN( "the clock is not set" )
        {
            THEN( "now() returns zero" )
            {
                REQUIRE( clock.now().time_since_epoch() == clock_type::duration::zero() );
            }
        }

        WHEN( "the clock is set" )
        {
            clock.set( value );
            THEN( "now() returns the value previously set" )
            {
                REQUIRE( clock.now() == value );
            }
        }

        WHEN( "the clock is set to different values" )
        {
            clock.set( value );
            clock.set( other_value );
            THEN( "now() returns the new value" )
            {
                REQUIRE( clock.now() == other_value );
            }
        }
    }
}

SCENARIO( "steady_testing_clock.set() throws when trying to set a lower value", "[testing_clock]" )
{
    GIVEN( "a steady testing_clock" )
    {
        using clock_type = sequencer::chrono::steady_testing_clock<>;

        clock_type clock;

        constexpr auto value = clock_type::time_point{clock_type::duration{42}};
        constexpr auto lower_value = clock_type::time_point{clock_type::duration{23}};
        constexpr auto higher_value = clock_type::time_point{clock_type::duration{77}};
        static_assert( value > lower_value );
        static_assert( value < higher_value );

        clock.set( value );

        WHEN( "the clock is set to a lower than before value" )
        {
            THEN( "an exception is thrown and now() returns the old value" )
            {
                REQUIRE_THROWS( clock.set( lower_value ) );
                REQUIRE( clock.now() == value );
            }
        }

        WHEN( "the clock is set to a higher than before value" )
        {
            clock.set( higher_value );
            THEN( "no exception is thrown and now() returns the new value" )
            {
                REQUIRE( clock.now() == higher_value );
            }
        }
    }
}
