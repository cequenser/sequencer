#include <sequencer/vector_view.hpp>

#include <catch2/catch.hpp>

SCENARIO( "vector_view", "[vector_view]" )
{
    using namespace sequencer;

    GIVEN( "a vector v with entries 1.0, 2.0, 3.0, 4.0, 5.0" )
    {
        std::vector< double > v = {1.0, 2.0, 3.0, 4.0, 5.0};

        WHEN( "a const vector_view with offset 0 is constructed from v" )
        {
            const auto view = vector_view{&v, 0};

            THEN( "the view has the same size as v" )
            {
                CHECK( view.size() == v.size() );
            }

            THEN( "the view provides access to the same entries as v" )
            {
                CHECK( view[ 0 ] == v[ 0 ] );
                CHECK( view[ 1 ] == v[ 1 ] );
                CHECK( view[ 2 ] == v[ 2 ] );
                CHECK( view[ 3 ] == v[ 3 ] );
                CHECK( view[ 4 ] == v[ 4 ] );
            }
        }

        WHEN( "a const vector_view with offset 2 is constructed from v" )
        {
            const auto view = vector_view{&v, 2};

            THEN( "the view is two elements smaller than v" )
            {
                CHECK( view.size() + 2 == v.size() );
            }

            THEN( "the view provides access to the last three entries of v" )
            {
                CHECK( view[ 0 ] == v[ 2 ] );
                CHECK( view[ 1 ] == v[ 3 ] );
                CHECK( view[ 2 ] == v[ 4 ] );
            }
        }

        WHEN( "a vector_view with offset 1 is constructed from v" )
        {
            auto view = vector_view{&v, 1};

            THEN( "the view is one element smaller than v" )
            {
                CHECK( view.size() + 1 == v.size() );
            }

            THEN( "the view provides access to the last four entries of v" )
            {
                CHECK( view[ 0 ] == v[ 1 ] );
                CHECK( view[ 1 ] == v[ 2 ] );
                CHECK( view[ 2 ] == v[ 3 ] );
                CHECK( view[ 3 ] == v[ 4 ] );
            }

            AND_WHEN( "the third entry of view is changed to 42.0" )
            {
                const auto new_value = 42.0;
                CHECK_FALSE( v[ 3 ] == new_value );
                view[ 2 ] = new_value;

                THEN( "the fourth entry of v is 42.0" )
                {
                    CHECK( v[ 3 ] == new_value );
                }
            }

            AND_WHEN( "a vector_view view2 is created from view with offset 1" )
            {
                auto view2 = vector_view{&view, 1};

                THEN( "the view2 is one element smaller than view" )
                {
                    CHECK( view2.size() + 1 == view.size() );
                }

                THEN( "view2 provides access to the last three elements of v" )
                {
                    CHECK( view2[ 0 ] == v[ 2 ] );
                    CHECK( view2[ 1 ] == v[ 3 ] );
                    CHECK( view2[ 2 ] == v[ 4 ] );
                }

                AND_WHEN( "the third entry of view2 is changed to 42.0" )
                {
                    const auto new_value = 42.0;
                    CHECK_FALSE( v[ 4 ] == new_value );
                    view2[ 2 ] = new_value;

                    THEN( "the fifth entry of v is 42.0" )
                    {
                        CHECK( v[ 4 ] == new_value );
                    }
                }
            }
        }
    }
}

SCENARIO( "vector_view stride", "[vector_view]" )
{
    using namespace sequencer;

    GIVEN( "a vector v with entries 1.0, 2.0, 3.0, 4.0, 5.0, 6.0" )
    {
        std::vector< double > v = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};

        WHEN( "a const vector_view with offset 0 and stride 2 is constructed from v" )
        {
            const auto offset = 0u;
            const auto stride = 2u;
            const auto view = vector_view{&v, offset, stride};

            THEN( "the view has half the size of v" )
            {
                CHECK( view.size() == v.size() / 2 );
            }

            THEN( "the view provides access to the even indexed entries of v" )
            {
                CHECK( view[ 0 ] == v[ 0 ] );
                CHECK( view[ 1 ] == v[ 2 ] );
                CHECK( view[ 2 ] == v[ 4 ] );
            }
        }

        WHEN( "a const vector_view with offset 1 and stride 2 is constructed from v" )
        {
            const auto offset = 1u;
            const auto stride = 2u;
            const auto view = vector_view{&v, offset, stride};

            THEN( "the view has half the size of v" )
            {
                CHECK( view.size() == v.size() / 2 );
            }

            THEN( "the view provides access to the odd indexed entries of v" )
            {
                CHECK( view[ 0 ] == v[ 1 ] );
                CHECK( view[ 1 ] == v[ 3 ] );
                CHECK( view[ 2 ] == v[ 5 ] );
            }
        }

        WHEN( "a const vector_view with offset 2 and stride 2 is constructed from v" )
        {
            const auto offset = 2u;
            const auto stride = 2u;
            const auto view = vector_view{&v, offset, stride};

            THEN( "the view is one less than half the size of v" )
            {
                CHECK( view.size() + 1 == v.size() / 2 );
            }

            THEN( "the view provides access to the last two even indexed entries of v" )
            {
                CHECK( view[ 0 ] == v[ 2 ] );
                CHECK( view[ 1 ] == v[ 4 ] );
            }
        }

        WHEN( "a vector_view with offset 1 and stride 2 is constructed from v" )
        {
            const auto offset = 1u;
            const auto stride = 2u;
            auto view = vector_view{&v, offset, stride};

            THEN( "the view is half the size of v" )
            {
                CHECK( view.size() == v.size() / 2 );
            }

            THEN( "the view provides access to the odd indexed entries of v" )
            {
                CHECK( view[ 0 ] == v[ 1 ] );
                CHECK( view[ 1 ] == v[ 3 ] );
                CHECK( view[ 2 ] == v[ 5 ] );
            }

            AND_WHEN( "the second entry of view is changed to 42.0" )
            {
                const auto new_value = 42.0;
                CHECK_FALSE( v[ 3 ] == new_value );
                view[ 1 ] = new_value;

                THEN( "the fourth entry of v is 42.0" )
                {
                    CHECK( v[ 3 ] == new_value );
                }
            }

            AND_WHEN( "a vector_view view2 is created from view with offset 1 and stride 3" )
            {
                const auto offset = 1u;
                const auto stride = 3u;
                auto view2 = vector_view{&view, offset, stride};

                THEN( "the view2 has one element" )
                {
                    CHECK( view2.size() == 1 );
                }

                THEN( "view2 provides access to the fourth element of v" )
                {
                    CHECK( view2[ 0 ] == v[ 3 ] );
                }

                AND_WHEN( "the first entry of view2 is changed to 42.0" )
                {
                    const auto new_value = 42.0;
                    CHECK_FALSE( v[ 3 ] == new_value );
                    view2[ 0 ] = new_value;

                    THEN( "the fourth entry of v is 42.0" )
                    {
                        CHECK( v[ 3 ] == new_value );
                    }

                    THEN( "the second entry of view is 42.0" )
                    {
                        CHECK( view[ 1 ] == new_value );
                    }
                }
            }
        }
    }
}
