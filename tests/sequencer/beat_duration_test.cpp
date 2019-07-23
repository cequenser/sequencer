#include "chrono/chrono_matchers.hpp"

#include <sequencer/beat_duration.hpp>

#include <catch2/catch.hpp>

#include <type_traits>

SCENARIO( "Convert beat_duration to std::chrono::duration<>", "[beat_duration]" )
{
    using namespace sequencer::chrono;
    using namespace std::chrono_literals;

    using Catch::Matchers::WithinULP;

    GIVEN( "A beat_duration of 16 beats at 174 beats per minute" )
    {
        constexpr beat_duration sixteen_beats{16.0, 174.0_bpm};
        static_assert( sixteen_beats.beats() == 16.0 );

        WHEN( "the duration is calculated" )
        {
            constexpr auto duration = sixteen_beats.duration();
            using duration_type = decltype( duration );

            THEN( "the result is represented as double" )
            {
                static_assert( std::is_same_v< duration_type::rep, double > );
            }

            THEN( "the result is represented in seconds" )
            {
                static_assert(
                    std::is_same_v< duration_type::period, std::chrono::seconds::period > );
            }

            THEN( "the duration is 5.51724 seconds" )
            {
                REQUIRE_THAT( duration, WithinDuration( 5.51724s, 0.00001s ) );
            }
        }
    }

    GIVEN( "A beat_duration of 30 beats at 120 beats per minute" )
    {
        constexpr beat_duration thirty_beats{30.0, 120.0_bpm};
        static_assert( thirty_beats.beats() == 30.0 );

        WHEN( "the duration is calculated" )
        {
            constexpr auto duration = thirty_beats.duration();

            THEN( "the duration is 15.0 seconds" )
            {
                REQUIRE( duration == std::chrono::seconds( 15 ) );
            }
        }
    }
}

SCENARIO( "Convert std::chrono::duration<> to beat_duration", "[beat_duration]" )
{
    using namespace sequencer::chrono;
    using Catch::Matchers::WithinULP;

    GIVEN( "a duration of 3 seconds" )
    {
        const auto three_seconds = std::chrono::seconds( 3 );

        WHEN( "the tempo is 120.0 BPM" )
        {
            beat_duration duration{three_seconds, 120.0_bpm};

            THEN( "the beat_duration is (2 beats per second * 3 seconds) = 6.0 beats" )
            {
                REQUIRE_THAT( duration.beats(), WithinULP( 6.0, 1 ) );
            }
        }

        WHEN( "the tempo is 174.0 BPM" )
        {
            beat_duration duration{three_seconds, 174.0_bpm};

            THEN( "the beat_duration is (2.9 beats per second * 3 seconds) = 8.7 beats" )
            {
                REQUIRE_THAT( duration.beats(), WithinULP( 8.7, 1 ) );
            }
        }
    }

    GIVEN( "a duration of a quarter second" )
    {
        const auto quarter_second = std::chrono::milliseconds( 250 );

        WHEN( "the tempo is 120.0 BPM" )
        {
            beat_duration duration{quarter_second, 120.0_bpm};

            THEN( "the beat_duration is (2 beats per second * 0.25 seconds) = 0.5 beats" )
            {
                REQUIRE_THAT( duration.beats(), WithinULP( 0.5, 1 ) );
            }
        }

        WHEN( "the tempo is 174.0 BPM" )
        {
            beat_duration duration{quarter_second, 174.0_bpm};

            THEN( "the beat_duration is (2.9 beats per second * 0.25 seconds) = 0.725 beats" )
            {
                REQUIRE_THAT( duration.beats(), WithinULP( 0.725, 1 ) );
            }
        }
    }
}
