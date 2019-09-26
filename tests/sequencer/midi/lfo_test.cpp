#include <sequencer/midi/lfo.hpp>

#include <catch2/catch.hpp>

SCENARIO( "lfo with squared wave form", "[lfo]" )
{
    using namespace sequencer::midi;
    constexpr auto mode = lfo_mode::square;

    GIVEN( "speed of 1, min of 0 and max of 127" )
    {
        const auto speed = 1;
        const auto phase = 0;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for step at start" )
        {
            const auto value = lfo_impl( 0.0, 1.0, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed at 0.49" )
        {
            const auto value = lfo_impl( 0.49, 1.0, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed at 0.5" )
        {
            const auto value = lfo_impl( 0.5, 1.0, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed at 0.99" )
        {
            const auto value = lfo_impl( 0.99, 1.0, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }
    }

    GIVEN( "speed of 2, phase of 0, min of 0 and max of 127" )
    {
        const auto speed = 2;
        const auto phase = 0;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for step at start" )
        {
            const auto value = lfo_impl( 0.0, 1.0, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed at 0.24" )
        {
            const auto value = lfo_impl( 0.24, 1.0, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed at 0.25" )
        {
            const auto value = lfo_impl( 0.25, 1.0, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed at 0.49" )
        {
            const auto value = lfo_impl( 0.49, 1.0, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }
    }

    GIVEN( "speed of 2, phase of 32, min of 0 and max of 127" )
    {
        const auto speed = 2;
        const auto phase = 32;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for at 0" )
        {
            const auto value = lfo_impl( 0, 1.0, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed at 0.2" )
        {
            const auto value = lfo_impl( 0.2, 1.0, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed at 0.25" )
        {
            const auto value = lfo_impl( 0.25, 1.0, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed at 0.49" )
        {
            const auto value = lfo_impl( 0.49, 1.0, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed at 0.5" )
        {
            const auto value = lfo_impl( 0.5, 1.0, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }
    }
}

SCENARIO( "lfo with squared wave form from pulse count", "[lfo]" )
{
    using namespace sequencer::midi;
    constexpr auto pulses_per_quarter_note = 4;
    constexpr auto mode = lfo_mode::square;

    GIVEN( "speed of 128, min of 0 and max of 127" )
    {
        const auto speed = 128;
        const auto phase = 0;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for step 0" )
        {
            const auto value = lfo( 0, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 7" )
        {
            const auto value = lfo( 7, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 8" )
        {
            const auto value = lfo( 8, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 15" )
        {
            const auto value = lfo( 15, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 16" )
        {
            const auto value = lfo( 16, pulses_per_quarter_note, speed, phase, min, max, mode );

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
            const auto value = lfo( 0, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 7" )
        {
            const auto value = lfo( 7, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 8" )
        {
            const auto value = lfo( 8, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 15" )
        {
            const auto value = lfo( 15, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 16" )
        {
            const auto value = lfo( 16, pulses_per_quarter_note, speed, phase, min, max, mode );

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
            const auto value = lfo( 0, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 3" )
        {
            const auto value = lfo( 3, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 4" )
        {
            const auto value = lfo( 4, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 11" )
        {
            const auto value = lfo( 11, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 12" )
        {
            const auto value = lfo( 12, pulses_per_quarter_note, speed, phase, min, max, mode );

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
            const auto value = lfo( 0, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 15" )
        {
            const auto value = lfo( 15, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 16" )
        {
            const auto value = lfo( 16, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 31" )
        {
            const auto value = lfo( 31, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 32" )
        {
            const auto value = lfo( 32, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }
    }
}

SCENARIO( "lfo with triangular wave form", "[lfo]" )
{
    using namespace sequencer::midi;
    constexpr auto pulses_per_quarter_note = 4;
    constexpr auto mode = lfo_mode::triangular;

    GIVEN( "speed of 128, min of 0 and max of 127" )
    {
        const auto speed = 128;
        const auto phase = 0;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for step 0" )
        {
            const auto value = lfo( 0, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 64" )
            {
                CHECK( value == 64 );
            }
        }

        WHEN( "lfo is computed for step 2" )
        {
            const auto value = lfo( 2, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 96" )
            {
                CHECK( value == 96 );
            }
        }

        WHEN( "lfo is computed for step 4" )
        {
            const auto value = lfo( 4, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 8" )
        {
            const auto value = lfo( 8, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 63" )
            {
                CHECK( value == 63 );
            }
        }

        WHEN( "lfo is computed for step 12" )
        {
            const auto value = lfo( 12, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 16" )
        {
            const auto value = lfo( 16, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 64" )
            {
                CHECK( value == 64 );
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
            const auto value = lfo( 0, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 4" )
        {
            const auto value = lfo( 4, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 63" )
            {
                CHECK( value == 63 );
            }
        }

        WHEN( "lfo is computed for step 8" )
        {
            const auto value = lfo( 8, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 12" )
        {
            const auto value = lfo( 12, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 64" )
            {
                CHECK( value == 64 );
            }
        }
    }
}

SCENARIO( "lfo with sine wave form", "[lfo]" )
{
    using namespace sequencer::midi;
    constexpr auto pulses_per_quarter_note = 4;
    constexpr auto mode = lfo_mode::sine;

    GIVEN( "speed of 128, min of 0 and max of 127" )
    {
        const auto speed = 128;
        const auto phase = 0;
        const auto min = std::uint8_t{0};
        const auto max = std::uint8_t{127};

        WHEN( "lfo is computed for step 0" )
        {
            const auto value = lfo( 0, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 63" )
            {
                CHECK( value == 63 );
            }
        }

        WHEN( "lfo is computed for step 2" )
        {
            const auto value = lfo( 2, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 108" )
            {
                CHECK( value == 108 );
            }
        }

        WHEN( "lfo is computed for step 4" )
        {
            const auto value = lfo( 4, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 8" )
        {
            const auto value = lfo( 8, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 63" )
            {
                CHECK( value == 63 );
            }
        }

        WHEN( "lfo is computed for step 12" )
        {
            const auto value = lfo( 12, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 16" )
        {
            const auto value = lfo( 16, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 63" )
            {
                CHECK( value == 63 );
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
            const auto value = lfo( 0, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 127" )
            {
                CHECK( value == 127 );
            }
        }

        WHEN( "lfo is computed for step 4" )
        {
            const auto value = lfo( 4, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 63" )
            {
                CHECK( value == 63 );
            }
        }

        WHEN( "lfo is computed for step 8" )
        {
            const auto value = lfo( 8, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 0" )
            {
                CHECK( value == 0 );
            }
        }

        WHEN( "lfo is computed for step 12" )
        {
            const auto value = lfo( 12, pulses_per_quarter_note, speed, phase, min, max, mode );

            THEN( "value is 63" )
            {
                CHECK( value == 63 );
            }
        }
    }
}
