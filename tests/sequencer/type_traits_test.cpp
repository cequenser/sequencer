#include <sequencer/type_traits.hpp>

#include <catch2/catch.hpp>

SCENARIO( "is_complex", "[type_traits]" )
{
    using namespace sequencer;

    GIVEN( "a complex float type" )
    {
        using type = std::complex< float >;

        THEN( "is_complex_v returns true" )
        {
            STATIC_REQUIRE( is_complex_v< type > );
        }
    }

    GIVEN( "a complex double type" )
    {
        using type = std::complex< double >;

        THEN( "is_complex_v returns true" )
        {
            STATIC_REQUIRE( is_complex_v< type > );
        }
    }

    GIVEN( "a float type" )
    {
        using type = float;

        THEN( "is_complex_v returns true" )
        {
            STATIC_REQUIRE_FALSE( is_complex_v< type > );
        }
    }
}
