#include <sequencer/audio/ring_buffer.hpp>

#include <catch2/catch.hpp>

SCENARIO( "ring buffer move", "[ring_buffer]" )
{
    using namespace sequencer::audio;

    GIVEN( "ring buffer with 8 entries 1 2 3 4 5 6 7 8" )
    {
        auto ring_buffer = ring_buffer_t< float >{8};
        for ( auto i = 0u; i < 8; ++i )
        {
            ring_buffer[ i ] = i + 1;
        }

        THEN( "entries of buffer are 1 2 3 4 5 6 7 8" )
        {
            for ( auto i = 1; i <= 8; ++i )
            {
                CHECK( ring_buffer[ i - 1 ] == Approx( i ) );
            }
        }

        WHEN( "buffer is moved by 3" )
        {
            ring_buffer.move( 3 );

            THEN( "entries of buffer are 4 5 6 7 8 1 2 3" )
            {
                for ( auto i = 1; i <= 5; ++i )
                {
                    CHECK( ring_buffer[ i - 1 ] == Approx( i + 3 ) );
                }
                for ( auto i = 6; i <= 8; ++i )
                {
                    CHECK( ring_buffer[ i - 1 ] == Approx( i - 5 ) );
                }
            }

            AND_WHEN( "buffer is moved by 3" )
            {
                ring_buffer.move( 3 );

                THEN( "entries of buffer are 7 8 1 2 3 4 5 6" )
                {
                    for ( auto i = 1; i <= 2; ++i )
                    {
                        CHECK( ring_buffer[ i - 1 ] == Approx( i + 6 ) );
                    }
                    for ( auto i = 3; i <= 8; ++i )
                    {
                        CHECK( ring_buffer[ i - 1 ] == Approx( i - 2 ) );
                    }
                }
            }
        }

        WHEN( "buffer is moved by 11" )
        {
            ring_buffer.move( 11 );

            THEN( "entries of buffer are 4 5 6 7 8 1 2 3" )
            {
                for ( auto i = 1; i <= 5; ++i )
                {
                    CHECK( ring_buffer[ i - 1 ] == Approx( i + 3 ) );
                }
                for ( auto i = 6; i <= 8; ++i )
                {
                    CHECK( ring_buffer[ i - 1 ] == Approx( i - 5 ) );
                }
            }
        }

        WHEN( "buffer is moved by 8" )
        {
            ring_buffer.move( 8 );

            THEN( "entries of buffer are 1 2 3 4 5 6 7 8" )
            {
                for ( auto i = 1; i <= 8; ++i )
                {
                    CHECK( ring_buffer[ i - 1 ] == Approx( i ) );
                }
            }
        }
    }
}

SCENARIO( "ring buffer clean", "[ring_buffer]" )
{
    using namespace sequencer::audio;

    GIVEN( "ring buffer with 8 entries 1 2 3 4 5 6 7 8" )
    {
        auto ring_buffer = ring_buffer_t< float >{8};
        auto init = 0;
        for ( auto& entry : ring_buffer )
        {
            entry = ++init;
        }

        WHEN( "clean(3) is called" )
        {
            ring_buffer.clean( 3 );

            THEN( "entries of buffer are 0 0 0 4 5 6 7 8" )
            {
                for ( auto i = 1; i <= 3; ++i )
                {
                    CHECK( ring_buffer[ i - 1 ] == Approx( 0 ) );
                }
                for ( auto i = 4; i <= 8; ++i )
                {
                    CHECK( ring_buffer[ i - 1 ] == Approx( i ) );
                }
            }
        }

        WHEN( "clean_and_move(3) is called" )
        {
            ring_buffer.clean_and_move( 3 );

            THEN( "entries of buffer are 4 5 6 7 8 0 0 0" )
            {
                for ( auto i = 1; i <= 5; ++i )
                {
                    CHECK( ring_buffer[ i - 1 ] == Approx( i + 3 ) );
                }
                for ( auto i = 6; i <= 8; ++i )
                {
                    CHECK( ring_buffer[ i - 1 ] == Approx( 0 ) );
                }
            }
        }
    }
}

SCENARIO( "ring buffer iterator access", "[ring_buffer]" )
{
    using namespace sequencer::audio;

    GIVEN( "ring buffer with 8 entries 1 2 3 4 5 6 7 8" )
    {
        auto ring_buffer = ring_buffer_t< float >{8};
        auto init = 0;
        for ( auto i = 0u; i < 8; ++i )
        {
            ring_buffer[ i ] = i + 1;
        }

        WHEN( "the begin iterator is retrieved" )
        {
            auto begin_iter = ring_buffer.begin();

            THEN( "iterator points to 1" )
            {
                CHECK( *begin_iter == Approx( 1 ) );
            }

            THEN( "next iterator points to 2" )
            {
                CHECK( *( begin_iter + 1 ) == Approx( 2 ) );
            }

            THEN( "second next iterator points to 3" )
            {
                CHECK( *( begin_iter + 2 ) == Approx( 3 ) );
            }

            THEN( "iterator + 7 points to 8" )
            {
                CHECK( *( begin_iter + 7 ) == Approx( 8 ) );
            }

            THEN( "iterator + 8 points to 1" )
            {
                CHECK( *( begin_iter + 8 ) == Approx( 1 ) );
            }
        }

        WHEN( "buffer is moved by 3" )
        {
            ring_buffer.move( 3 );

            WHEN( "the begin iterator is retrieved" )
            {
                auto begin_iter = ring_buffer.begin();

                THEN( "iterator points to 4" )
                {
                    CHECK( *begin_iter == Approx( 4 ) );
                }

                THEN( "next iterator points to 5" )
                {
                    CHECK( *( begin_iter + 1 ) == Approx( 5 ) );
                }

                THEN( "iterator + 7 points to 3" )
                {
                    CHECK( *( begin_iter + 7 ) == Approx( 3 ) );
                }

                THEN( "iterator + 8 points to 4" )
                {
                    CHECK( *( begin_iter + 8 ) == Approx( 4 ) );
                }
            }
        }
    }
}

SCENARIO( "ring buffer iterator comparison", "[ring_buffer]" )
{
    using namespace sequencer::audio;

    GIVEN( "ring buffer with 8 entries 1 2 3 4 5 6 7 8" )
    {
        auto ring_buffer = ring_buffer_t< float >{8};
        for ( auto i = 0u; i < 8; ++i )
        {
            ring_buffer[ i ] = i + 1;
        }

        WHEN( "two begin iterators are retrieved" )
        {
            auto first = ring_buffer.begin();
            auto second = ring_buffer.begin();

            THEN( "both iterators compare equal" )
            {
                CHECK( first == second );
                CHECK_FALSE( first != second );
            }

            AND_WHEN( "end iterator is retrieved" )
            {
                auto end = ring_buffer.end();

                THEN( "first and end compare not equal" )
                {
                    CHECK_FALSE( first == end );
                    CHECK( first != end );
                }

                AND_WHEN( "first is moved to end" )
                {
                    first += 7;
                    first += 1;

                    THEN( "first and end compare equal" )
                    {
                        CHECK( first == end );
                        CHECK_FALSE( first != end );
                    }
                }
            }
        }
    }
}
