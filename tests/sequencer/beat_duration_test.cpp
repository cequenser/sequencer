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

        WHEN( "another duration of 1e-10 beats is added" )
        {
            constexpr auto still_thirty_beats = thirty_beats + beat_duration{1e-10};
            static_assert( still_thirty_beats.beats() == 30.0 );
        }
    }

    GIVEN( "A beat_duration of 30 + 1e-10 beats" )
    {
        constexpr beat_duration thirty_beats{30.0 + 1e-10};
        static_assert( thirty_beats.beats() == 30.0 );
    }

    GIVEN( "A beat_duration of 30" )
    {
        beat_duration thirty_beats{30.0};
        WHEN( "the duration is increased by beat_duration::eps 1/beat_duration::eps times" )
        {
            for ( auto i = 0;
                  i < int( 1 / beat_duration::eps + std::numeric_limits< double >::epsilon() );
                  ++i )
            {
                thirty_beats += beat_duration{beat_duration::eps};
            }

            THEN( "duration is 31 beats" )
            {
                REQUIRE( thirty_beats == beat_duration{31} );
            }
        }
    }

    GIVEN( "A beat_duration of 0" )
    {
        beat_duration duration{0};
        WHEN( "the duration is increased by 1.0/24 12 times" )
        {
            for ( auto i = 0; i < 12; ++i )
            {
                duration += beat_duration{1.0 / 24};
            }

            THEN( "duration is 0.5 beat" )
            {
                REQUIRE( duration == beat_duration{0.5} );
            }
        }
    }

    GIVEN( "A beat_duration of -4 beats" )
    {
        const auto duration = -4.0_beats;
        THEN( "the duration is -4.0 beats" )
        {
            REQUIRE( duration.beats() == -4.0 );
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

SCENARIO( "round", "[round]" )
{
    using namespace sequencer;
    GIVEN( "constexpr auto a = 0.5" )
    {
        constexpr auto a = 0.5;

        THEN( "constexpr_round returns 1" )
        {
            STATIC_REQUIRE( constexpr_round( a ) == 1 );
        }
    }

    GIVEN( "constexpr auto a = 0.499" )
    {
        constexpr auto a = 0.499;

        THEN( "constexpr_round returns 0" )
        {
            STATIC_REQUIRE( constexpr_round( a ) == 0 );
        }
    }

    GIVEN( "constexpr auto a = -0.499" )
    {
        constexpr auto a = -0.499;

        THEN( "constexpr_round returns 0" )
        {
            STATIC_REQUIRE( constexpr_round( a ) == 0 );
        }
    }

    GIVEN( "constexpr auto a = -0.5" )
    {
        constexpr auto a = -0.5;

        THEN( "constexpr_round returns -1" )
        {
            STATIC_REQUIRE( constexpr_round( a ) == -1 );
        }
    }
}
