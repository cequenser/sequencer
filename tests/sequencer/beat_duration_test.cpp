#include "chrono/chrono_matchers.hpp"

#include <sequencer/beat_duration.hpp>

#include <catch2/catch.hpp>

#include <type_traits>

SCENARIO( "Convert beat_duration to std::chrono::duration<>", "[beat_duration]" )
{
    using namespace sequencer;
    using namespace std::chrono_literals;

    using Catch::Matchers::WithinULP;
    using sequencer::chrono::within_abs;

    GIVEN( "A beat_duration of 16 beats" )
    {
        constexpr beat_duration sixteen_beats{16.0};
        static_assert( sixteen_beats.beats() == 16.0 );

        WHEN( "the duration is calculated at 174 beats per minute" )
        {
            constexpr auto duration = sixteen_beats.duration( 174.0_bpm );
            using duration_type = decltype( duration );

            THEN( "the result is represented as double" )
            {
                STATIC_REQUIRE( std::is_same_v< duration_type::rep, double > );
            }

            THEN( "the result is represented in seconds" )
            {
                STATIC_REQUIRE(
                    std::is_same_v< duration_type::period, std::chrono::seconds::period > );
            }

            THEN( "the duration is 5.51724 seconds" )
            {
                REQUIRE_THAT( duration, within_abs( 5.51724s, 0.00001s ) );
            }
        }

        WHEN( "there is another duration of 13 beats" )
        {
            beat_duration another_duration{13.0};
            THEN( "in-place summation yields 29 beats" )
            {
                another_duration += sixteen_beats;
                REQUIRE( another_duration == beat_duration{29.0} );
            }
        }

        WHEN( "there is another constexpr duration of 13 beats" )
        {
            constexpr beat_duration thirteen_beats{13.0};
            THEN( "summation yields 29 beats" )
            {
                constexpr auto twenty_nine_beats = thirteen_beats + sixteen_beats;
                STATIC_REQUIRE( twenty_nine_beats == beat_duration{29.0} );
            }

            THEN( "thirteen beats is smaller than sixteen beats" )
            {
                STATIC_REQUIRE( thirteen_beats < sixteen_beats );
            }

            THEN( "sixteen beats is not smaller than thirteen beats" )
            {
                STATIC_REQUIRE_FALSE( sixteen_beats < thirteen_beats );
            }

            THEN( "thirteen beats is not smaller than thirteen beats" )
            {
                STATIC_REQUIRE_FALSE( thirteen_beats < thirteen_beats );
            }
        }
    }

    GIVEN( "A beat_duration of 30 beats" )
    {
        constexpr beat_duration thirty_beats{30.0};
        static_assert( thirty_beats.beats() == 30.0 );

        WHEN( "the duration is calculated at 120 beats per minute" )
        {
            constexpr auto duration = thirty_beats.duration( 120.0_bpm );

            THEN( "the duration is 15.0 seconds" )
            {
                REQUIRE( duration == std::chrono::seconds( 15 ) );
            }
        }
    }
}

SCENARIO( "Convert std::chrono::duration<> to beat_duration", "[beat_duration]" )
{
    using namespace sequencer;

    using Catch::Matchers::WithinULP;
    using sequencer::chrono::within_abs;

    GIVEN( "a duration of 3 seconds" )
    {
        const auto three_seconds = std::chrono::seconds( 3 );

        WHEN( "the tempo is 120.0 BPM" )
        {
            beat_duration duration{three_seconds, 120.0_bpm};

            THEN( "the beat_duration is (2 beats per second * 3 seconds) = 6.0 beats" )
            {
                REQUIRE( duration.beats() == 6.0 );
            }
        }

        WHEN( "the tempo is 174.0 BPM" )
        {
            beat_duration duration{three_seconds, 174.0_bpm};

            THEN( "the beat_duration is (2.9 beats per second * 3 seconds) = 8.7 beats" )
            {
                REQUIRE( duration == beat_duration{8.7} );
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
                REQUIRE( duration == beat_duration{0.725} );
            }
        }
    }
}
