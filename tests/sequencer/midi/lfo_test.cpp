#include <sequencer/midi/lfo.hpp>

#include <catch2/catch.hpp>

SCENARIO( "lfo with squared wave form", "[lfo]" )
{
    using namespace sequencer::midi;

    GIVEN( "speed of 128, min of 0 and max of 127" )
    {
        const auto speed = 128;
        const auto phase = 0;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for step 0" )
        {
            const auto value = lfo( 0, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 7" )
        {
            const auto value = lfo( 7, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 8" )
        {
            const auto value = lfo( 8, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 15" )
        {
            const auto value = lfo( 15, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 16" )
        {
            const auto value = lfo( 16, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }
    }

    GIVEN( "speed of 128, phase of 64, min of 0 and max of 127" )
    {
        const auto speed = 128;
        const auto phase = 64;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for step 0" )
        {
            const auto value = lfo( 0, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 7" )
        {
            const auto value = lfo( 7, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 8" )
        {
            const auto value = lfo( 8, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 15" )
        {
            const auto value = lfo( 15, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 16" )
        {
            const auto value = lfo( 16, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }
    }

    GIVEN( "speed of 128, phase of 32, min of 0 and max of 127" )
    {
        const auto speed = 128;
        const auto phase = 32;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for step 0" )
        {
            const auto value = lfo( 0, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 3" )
        {
            const auto value = lfo( 3, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 4" )
        {
            const auto value = lfo( 4, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 11" )
        {
            const auto value = lfo( 11, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 12" )
        {
            const auto value = lfo( 12, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }
    }

    GIVEN( "speed of 64, min of 0 and max of 127" )
    {
        const auto speed = 64;
        const auto phase = 0;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for step 0" )
        {
            const auto value = lfo( 0, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 15" )
        {
            const auto value = lfo( 15, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 16" )
        {
            const auto value = lfo( 16, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 31" )
        {
            const auto value = lfo( 31, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 32" )
        {
            const auto value = lfo( 32, speed, phase, min, max, lfo_mode::square );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }
    }
}
