#include "chrono/chrono_matchers.hpp"

#include <sequencer/beat_duration.hpp>

#include <catch2/catch.hpp>

#include <sstream>
#include <type_traits>

SCENARIO( "Convert beat_duration to std::chrono::duration<>", "[beat_duration]" )
{
    using namespace sequencer;
    using namespace std::literals::chrono_literals;

    using Catch::Matchers::WithinULP;
    using sequencer::chrono::within_abs;

    GIVEN( "A beat_duration of 16 beats" )
    {
        constexpr auto sixteen_beats = 16.0_beats;
        static_assert( sixteen_beats.to_double() == 16.0 );

        WHEN( "the duration is calculated at 174 beats per minute" )
        {
            constexpr auto duration = sixteen_beats / 174.0_bpm;
            using duration_type = decltype( duration );

            THEN( "the result is represented as double" )
            {
                STATIC_REQUIRE( std::is_same_v< duration_type::rep, double > );
            }

            THEN( "the duration is 5.51724 seconds" )
            {
                REQUIRE_THAT( chrono::seconds{duration}, within_abs( 5.51724s, 0.00001s ) );
            }
        }

        WHEN( "there is another duration of 13 beats" )
        {
            auto another_duration = 13_beats;
            THEN( "in-place summation yields 29 beats" )
            {
                another_duration += sixteen_beats;
                REQUIRE( another_duration == beat_duration{29.0} );
            }
        }

        WHEN( "there is another constexpr duration of 13 beats" )
        {
            constexpr auto thirteen_beats = 13_beats;
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

        WHEN( "written to stream" )
        {
            std::stringstream stream;
            stream << sixteen_beats;

            THEN( "writes '16 beats'" )
            {
                REQUIRE( stream.str() == "16 beats" );
            }
        }
    }

    GIVEN( "A beat_duration of 30 beats" )
    {
        constexpr auto thirty_beats = 30_beats;
        static_assert( thirty_beats.to_double() == 30.0 );

        WHEN( "the duration is calculated at 120 beats per minute" )
        {
            constexpr auto duration = thirty_beats / 120.0_bpm;

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
    using namespace std::literals::chrono_literals;

    using Catch::Matchers::WithinULP;
    using sequencer::chrono::within_abs;

    GIVEN( "a duration of 3 seconds" )
    {
        constexpr auto three_seconds = 3s;

        WHEN( "the tempo is 120.0 BPM" )
        {
            constexpr auto beats = three_seconds * 120.0_bpm;

            THEN( "the beat_duration is (2 beats per second * 3 seconds) = 6.0 beats" )
            {
                REQUIRE( beats == beat_duration{6.0} );
            }
        }

        WHEN( "the tempo is 174.0 BPM" )
        {
            constexpr auto beats = three_seconds * 174.0_bpm;

            THEN( "the beat_duration is (2.9 beats per second * 3 seconds) = 8.7 beats" )
            {
                REQUIRE( beats == beat_duration{8.7} );
            }
        }
    }

    GIVEN( "a duration of a quarter second" )
    {
        constexpr auto quarter_second = 250ms;

        WHEN( "the tempo is 120.0 BPM" )
        {
            constexpr auto beats = quarter_second * 120.0_bpm;

            THEN( "the beat_duration is (2 beats per second * 0.25 seconds) = 0.5 beats" )
            {
                REQUIRE_THAT( beats.to_double(), WithinULP( 0.5, 1 ) );
            }
        }

        WHEN( "the tempo is 174.0 BPM" )
        {
            constexpr auto beats = quarter_second * 174.0_bpm;

            THEN( "the beat_duration is (2.9 beats per second * 0.25 seconds) = 0.725 beats" )
            {
                REQUIRE( beats == beat_duration{0.725} );
            }
        }
    }
}
