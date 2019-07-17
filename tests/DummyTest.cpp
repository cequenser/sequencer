#include <catch.hpp>
#include <sequencer/Test.h>

TEST_CASE( "dummy test", "success" )
{
    REQUIRE( tst::get_zero() == 0 );
}

TEST_CASE( "dummy test", "fail" )
{
    REQUIRE( tst::get_zero() == 1 );
}
