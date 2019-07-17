#include <catch.hpp>
#include <sequencer/Test.h>

TEST_CASE( "dummy test", "" )
{
    REQUIRE( tst::getZero() == 1 );
}
