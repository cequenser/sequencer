#include <sequencer/wave_form.hpp>

#include <catch2/catch.hpp>

#include <limits>

SCENARIO( "pulse", "[wave_form]" )
{
    using namespace sequencer;

    GIVEN( "a pulse with pulse width 0.7" )
    {
        const auto pulse = wave_form::pulse_t( 0.7 );

        WHEN( "evaluated at 0.5" )
        {
            const auto value = pulse( 0.5 );

            THEN( "is 1" )
            {
                CHECK( value == 1.0 );
            }
        }

        WHEN( "evaluated at 0.7" )
        {
            const auto value = pulse( 0.7 );

            THEN( "is 0" )
            {
                CHECK( value == 0.0 );
            }
        }

        WHEN( "evaluated at -0.2" )
        {
            const auto value = pulse( -0.2 );

            THEN( "is 0" )
            {
                CHECK( value == 0.0 );
            }
        }

        WHEN( "evaluated at -0.5" )
        {
            const auto value = pulse( -0.5 );

            THEN( "is 1" )
            {
                CHECK( value == 1.0 );
            }
        }
    }
}
