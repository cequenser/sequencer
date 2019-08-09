#include <sequencer/fixed_point_type.hpp>

#include <catch2/catch.hpp>

#include <type_traits>

SCENARIO( "Test fixed_point_type<3>", "[fixed_point_type]" )
{
    using namespace sequencer;
    constexpr auto ticks_per_unit = 3;

    GIVEN( "A fixed_point_type of 30" )
    {
        constexpr fixed_point_type< ticks_per_unit > thirty{30.0};
        static_assert( thirty.to_double() == 30.0 );

        WHEN( "another fixed_point_type of 0.1 is added" )
        {
            constexpr auto still_thirty = thirty + fixed_point_type< ticks_per_unit >{0.1};
            static_assert( still_thirty.to_double() == 30.0 );
        }

        WHEN( "taking the negative fixed_point_type" )
        {
            constexpr auto minus_thirty = -thirty;
            THEN( "it is -30" )
            {
                STATIC_REQUIRE( minus_thirty.to_double() == -30.0 );
            }
        }

        WHEN( "there is another fixed_point_type of value 30.1" )
        {
            constexpr auto other = fixed_point_type< ticks_per_unit >( 30.1 );
            THEN( "less-than comparison returns false" )
            {
                STATIC_REQUIRE_FALSE( thirty < other );
            }

            THEN( "less-or-equal comparison returns true" )
            {
                STATIC_REQUIRE( thirty <= other );
            }
        }

        WHEN( "there is another fixed_point_type of value 30.2" )
        {
            constexpr auto other = fixed_point_type< ticks_per_unit >( 30.2 );
            THEN( "less-than comparison returns true" )
            {
                STATIC_REQUIRE( thirty < other );
            }
            THEN( "less-or-equal comparison returns true" )
            {
                STATIC_REQUIRE( thirty <= other );
            }
        }

        WHEN( "there is another fixed_point_type of value 29.8" )
        {
            constexpr auto other = fixed_point_type< ticks_per_unit >( 29.8 );
            THEN( "less-than comparison returns false" )
            {
                STATIC_REQUIRE_FALSE( thirty < other );
            }
            THEN( "less-or-equal comparison returns false" )
            {
                STATIC_REQUIRE_FALSE( thirty <= other );
            }
        }
    }

    GIVEN( "A fixed_point_type of 30 + 0.1" )
    {
        constexpr fixed_point_type< ticks_per_unit > thirty{30.0 + 0.1};
        static_assert( thirty.to_double() == 30.0 );
    }

    GIVEN( "A fixed_point_type of 30" )
    {
        fixed_point_type< ticks_per_unit > thirty{30.0};
        WHEN( "the fixed_point_type is increased by fixed_point_type::eps 1/fixed_point_type::eps "
              "times" )
        {
            for ( auto i = 0; i < int( 1 / fixed_point_type< ticks_per_unit >::eps +
                                       std::numeric_limits< double >::epsilon() );
                  ++i )
            {
                thirty +=
                    fixed_point_type< ticks_per_unit >{fixed_point_type< ticks_per_unit >::eps};
            }

            THEN( "fixed_point_type is 31" )
            {
                REQUIRE( thirty == fixed_point_type< ticks_per_unit >{31} );
            }
        }
    }

    GIVEN( "A fixed_point_type of -4" )
    {
        const auto minus_four = fixed_point_type< ticks_per_unit >{-4.0};
        THEN( "the fixed_point_type is -4.0" )
        {
            REQUIRE( minus_four.to_double() == -4.0 );
        }
    }
}

SCENARIO( "Test fixed_point_type<24>", "[fixed_point_type]" )
{
    using namespace sequencer;
    constexpr auto ticks_per_unit = 24;

    GIVEN( "A fixed_point_type of 0" )
    {
        fixed_point_type< ticks_per_unit > value{0};
        WHEN( "the fixed_point_type is increased by 1.0/24 12 times" )
        {
            for ( auto i = 0; i < 12; ++i )
            {
                value += fixed_point_type< ticks_per_unit >{1.0 / 24};
            }

            THEN( "fixed_point_type is 0.5" )
            {
                REQUIRE( value == fixed_point_type< ticks_per_unit >{0.5} );
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
