#include <sequencer/midi/track.hpp>

#include <catch2/catch.hpp>

using sequencer::midi::track;

SCENARIO( "empty track has no notes", "[track]" )
{
    GIVEN( "an empty track" )
    {
        constexpr auto steps = 4u;
        const auto midi_track = track< steps >{};
        STATIC_REQUIRE( midi_track.steps() == steps );

        THEN( "no step has a note" )
        {
            for ( auto i = 0u; i < midi_track.steps(); ++i )
            {
                CHECK( midi_track[ i ] == track< steps >::no_note );
            }
        }
    }
}

SCENARIO( "change notes in track", "[track]" )
{
    GIVEN( "an empty track" )
    {
        constexpr auto steps = 4u;
        auto midi_track = track< steps >{};

        WHEN( "changing the second step to note value 3" )
        {
            midi_track[ 1 ] = 3;

            THEN( "all except the second step have no note and the second has note value 3" )
            {
                CHECK( midi_track[ 0 ] == track< steps >::no_note );
                CHECK( midi_track[ 1 ] == 3 );
                CHECK( midi_track[ 2 ] == track< steps >::no_note );
                CHECK( midi_track[ 3 ] == track< steps >::no_note );

                WHEN( "changing the first note to note value 42" )
                {
                    midi_track[ 0 ] = 42;

                    THEN( "track has the notes 42, 3, no_note, no_note" )
                    {
                        CHECK( midi_track[ 0 ] == 42 );
                        CHECK( midi_track[ 1 ] == 3 );
                        CHECK( midi_track[ 2 ] == track< steps >::no_note );
                        CHECK( midi_track[ 3 ] == track< steps >::no_note );
                    }

                    WHEN( "calling clear()" )
                    {
                        midi_track.clear();

                        THEN( "no step has a note" )
                        {
                            for ( auto i = 0u; i < midi_track.steps(); ++i )
                            {
                                CHECK( midi_track[ i ] == track< steps >::no_note );
                            }
                        }
                    }
                }
            }
        }
    }
}
