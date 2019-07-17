#include <catch.hpp>
#include <sequencer/Test.h>

TEST_CASE( "dummy test", "success" )
{
    REQUIRE( tst::get_zero() == 0 );
}
