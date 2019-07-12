#include <sequencer/Test.h>

#include <catch.hpp>

TEST_CASE( "dummy test", "" )
{
    REQUIRE( tst::getZero() == 0 );
}
