#include <sequencer/Test.h>

#include <catch2/catch.hpp>

TEST_CASE( "dummy test", "" )
{
    REQUIRE( tst::get_zero() == 0 );
}
