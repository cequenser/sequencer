#include <sequencer/midi/step.hpp>

#include <catch2/catch.hpp>

SCENARIO( "step construction", "[step]" )
{
    using namespace sequencer::midi;

    GIVEN( "a default constructed step" )
    {
        const auto step = step_t{};

        THEN( "step is not active" )
        {
            CHECK_FALSE( step.is_active() );
        }

        THEN( "step has no note" )
        {
            CHECK_FALSE( step.note() );
        }

        THEN( "step has no velocity" )
        {
            CHECK_FALSE( step.velocity() );
        }
    }

    GIVEN( "an active constructed step" )
    {
        const auto step = step_t{true};

        THEN( "step is active" )
        {
            CHECK( step.is_active() );
        }

        THEN( "step has no note" )
        {
            CHECK_FALSE( step.note() );
        }

        THEN( "step has no velocity" )
        {
            CHECK_FALSE( step.velocity() );
        }
    }

    GIVEN( "a step with note and velocity" )
    {
        const auto note = note_t{1};
        const auto velocity = 100;
        const auto step = step_t{note, velocity};

        THEN( "step is active" )
        {
            CHECK( step.is_active() );
        }

        THEN( "step has given note" )
        {
            REQUIRE( step.note() );
            CHECK( *step.note() == note );
        }

        THEN( "step has given velocity" )
        {
            REQUIRE( step.velocity() );
            CHECK( *step.velocity() == velocity );
        }
    }

    GIVEN( "a step with no_note and velocity" )
    {
        const auto note = no_note();
        const auto velocity = 100;
        const auto step = step_t{note, velocity};

        THEN( "step is active" )
        {
            CHECK_FALSE( step.is_active() );
        }

        THEN( "step has given note" )
        {
            CHECK( step.note() );
        }

        THEN( "step has given velocity" )
        {
            CHECK( step.velocity() );
        }
    }
}

SCENARIO( "step set_active", "[step]" )
{
    using namespace sequencer::midi;

    GIVEN( "a default constructed step" )
    {
        auto step = step_t{};
        CHECK_FALSE( step.is_active() );

        WHEN( "step is actived" )
        {
            step.set_active( true );

            THEN( "step is active" )
            {
                CHECK( step.is_active() );
            }
        }
    }
}

SCENARIO( "step set_note", "[step]" )
{
    using namespace sequencer::midi;

    GIVEN( "a default constructed step" )
    {
        auto step = step_t{};
        CHECK_FALSE( step.note() );

        WHEN( "note is set" )
        {
            const auto note = note_t{1};
            step.set_note( note );

            THEN( "step has given note" )
            {
                REQUIRE( step.note() );
                CHECK( *step.note() == note );
            }

            THEN( "step is active" )
            {
                CHECK( step.is_active() );
            }
        }

        WHEN( "no_note is set" )
        {
            const auto note = no_note();
            step.set_note( note );

            THEN( "step has given note" )
            {
                REQUIRE( step.note() );
                CHECK( *step.note() == note );
            }

            THEN( "step is not active" )
            {
                CHECK_FALSE( step.is_active() );
            }
        }
    }
}

SCENARIO( "step set_velocity", "[step]" )
{
    using namespace sequencer::midi;

    GIVEN( "a default constructed step" )
    {
        auto step = step_t{};
        CHECK_FALSE( step.note() );

        WHEN( "velocity is set" )
        {
            const auto velocity = 100;
            step.set_velocity( velocity );

            THEN( "step has given velocity" )
            {
                REQUIRE( step.velocity() );
                CHECK( *step.velocity() == velocity );
            }
        }
    }
}

SCENARIO( "step comparison", "[step]" )
{
    using namespace sequencer::midi;

    GIVEN( "two default constructed steps" )
    {
        const auto lhs = step_t{};
        const auto rhs = step_t{};

        THEN( "both compare equal" )
        {
            CHECK( lhs == rhs );
            CHECK_FALSE( lhs != rhs );
        }
    }

    GIVEN( "a default constructed and an active step" )
    {
        const auto lhs = step_t{};
        const auto rhs = step_t{true};

        THEN( "both compare not equal" )
        {
            CHECK_FALSE( lhs == rhs );
            CHECK( lhs != rhs );
        }
    }

    GIVEN( "an active step and a default constructed" )
    {
        const auto lhs = step_t{true};
        const auto rhs = step_t{};

        THEN( "both compare not equal" )
        {
            CHECK_FALSE( lhs == rhs );
            CHECK( lhs != rhs );
        }
    }

    GIVEN( "two steps with different notes" )
    {
        const auto lhs = step_t{note_t{1}, 100};
        const auto rhs = step_t{note_t{2}, 100};

        THEN( "both compare not equal" )
        {
            CHECK_FALSE( lhs == rhs );
            CHECK( lhs != rhs );
        }
    }

    GIVEN( "two steps with different velocities" )
    {
        const auto lhs = step_t{note_t{1}, 100};
        const auto rhs = step_t{note_t{1}, 90};

        THEN( "both compare not equal" )
        {
            CHECK_FALSE( lhs == rhs );
            CHECK( lhs != rhs );
        }
    }

    GIVEN( "two steps with same nodes and velocities" )
    {
        const auto lhs = step_t{note_t{1}, 100};
        const auto rhs = step_t{note_t{1}, 100};

        THEN( "both compare equal" )
        {
            CHECK( lhs == rhs );
            CHECK_FALSE( lhs != rhs );
        }
    }

    GIVEN( "an inactive and an active step with same nodes and velocities" )
    {
        auto lhs = step_t{note_t{1}, 100};
        lhs.set_active( false );
        const auto rhs = step_t{note_t{1}, 100};

        THEN( "both compare not equal" )
        {
            CHECK_FALSE( lhs == rhs );
            CHECK( lhs != rhs );
        }
    }

    GIVEN( "an active and an inactive step with same nodes and velocities" )
    {
        const auto lhs = step_t{note_t{1}, 100};
        auto rhs = step_t{note_t{1}, 100};
        rhs.set_active( false );

        THEN( "both compare not equal" )
        {
            CHECK_FALSE( lhs == rhs );
            CHECK( lhs != rhs );
        }
    }
}
