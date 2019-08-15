#include <sequencer/midi/track.hpp>

#include <catch2/catch.hpp>

using sequencer::midi::make_message;
using sequencer::midi::track;
using sequencer::midi::track_for_step_sequencer;
using sequencer::midi::tracks_for_step_sequencer;
using sequencer::midi::channel::mode::all_notes_off;
using sequencer::midi::channel::voice::note_off;
using sequencer::midi::channel::voice::note_on;

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

SCENARIO( "track_for_step_sequencer", "[track]" )
{
    using namespace sequencer;
    constexpr auto number_of_steps = 16u;
    using track_t = track_for_step_sequencer< number_of_steps >;

    GIVEN( "track_for_step_sequencer with 4 steps" )
    {
        auto track = track_t{};
        REQUIRE( track.steps() == number_of_steps );
        REQUIRE( track.channel() == 0u );
        REQUIRE( track[ 0 ] == track_t::no_note );

        WHEN( "the 3rd step is set to note 1" )
        {
            const auto first_note = 1;
            track[ 2 ] = first_note;

            THEN( "the first step equals note 1" )
            {
                CHECK( track[ 2 ] == first_note );
            }

            THEN( "get_messages(1) returns empty message" )
            {
                CHECK( track.get_messages( 1 ).empty() );
            }

            THEN( "get_messages(2) returns note on message" )
            {
                CHECK( track.get_messages( 2 ) ==
                       make_message( note_on( track.channel(), first_note, 32 ) ) );

                WHEN( "the 5th step is set to note 2" )
                {
                    const auto second_note = 2;
                    track[ 4 ] = second_note;

                    THEN( "get_messages(4( returns note off and note on message" )
                    {
                        CHECK( track.get_messages( 4 ) ==
                               make_message( note_off( track.channel(), first_note, 32 ) )
                                   .append( make_message(
                                       note_on( track.channel(), second_note, 32 ) ) ) );
                    }
                }

                WHEN( "clear_last_note is called" )
                {
                    track.clear_last_note();

                    WHEN( "the 5th step is set to note 2" )
                    {
                        const auto second_note = 2;
                        track[ 4 ] = second_note;

                        THEN( "get_messages(4( returns note on message" )
                        {
                            CHECK( track.get_messages( 4 ) ==
                                   make_message( note_on( track.channel(), second_note, 32 ) ) );
                        }
                    }
                }
            }

            WHEN( "clear is called" )
            {
                track.clear();

                THEN( "the rd step equals no note" )
                {
                    CHECK( track[ 2 ] == track_t::no_note );
                }
            }
        }

        WHEN( "get_all_notes_off_message is called" )
        {
            const auto message = track.get_all_notes_off_message();

            THEN( "returns all notes off message for its channel" )
            {
                CHECK( message == make_message( all_notes_off( track.channel() ) ) );
            }
        }

        WHEN( "channel is channed to 1" )
        {
            track.set_channel( 1 );

            THEN( "channel is one" )
            {
                CHECK( track.channel() == 1u );
            }

            WHEN( "get_all_notes_off_message is called" )
            {
                const auto message = track.get_all_notes_off_message();

                THEN( "returns all notes off message for its channel" )
                {
                    CHECK( message == make_message( all_notes_off( track.channel() ) ) );
                }
            }
        }
    }
}

SCENARIO( "tracks_for_step_sequencer", "[track]" )
{
    using namespace sequencer;
    constexpr auto number_of_steps = 16u;
    constexpr auto number_of_tracks = 2u;
    using track_t = tracks_for_step_sequencer< number_of_steps, number_of_tracks >;

    GIVEN( "tracks_for_step_sequencer with 4 steps" )
    {
        auto tracks = track_t{};
        REQUIRE( tracks.steps() == number_of_steps );
        REQUIRE( tracks.track( 0 ).channel() == 0u );
        REQUIRE( tracks.track( 1 ).channel() == 1u );
        REQUIRE( tracks.track( 0 )[ 0 ] == track_t::no_note );
        REQUIRE( tracks.track( 1 )[ 0 ] == track_t::no_note );

        WHEN( "in the first track the 3rd step is set to note 1" )
        {
            const auto first_note = 1;
            tracks.track( 0 )[ 2 ] = first_note;

            THEN( "the first step equals note 1" )
            {
                CHECK( tracks.track( 0 )[ 2 ] == first_note );
            }

            THEN( "get_messages(1) returns empty message" )
            {
                CHECK( tracks.get_messages( 1 ).empty() );
            }

            THEN( "get_messages(2) returns note on message" )
            {
                CHECK( tracks.get_messages( 2 ) ==
                       make_message( note_on( tracks.track( 0 ).channel(), first_note, 32 ) ) );

                WHEN( "in the first track the 5th step is set to note 2" )
                {
                    const auto second_note = 2;
                    tracks.track( 0 )[ 4 ] = second_note;

                    THEN( "get_messages(4( returns note off and note on message" )
                    {
                        CHECK(
                            tracks.get_messages( 4 ) ==
                            make_message( note_off( tracks.track( 0 ).channel(), first_note, 32 ) )
                                .append( make_message(
                                    note_on( tracks.track( 0 ).channel(), second_note, 32 ) ) ) );
                    }
                }

                WHEN( "in the second track the 5th step is set to note 2" )
                {
                    const auto second_note = 2;
                    tracks.track( 1 )[ 4 ] = second_note;

                    THEN( "get_messages(4( returns note off and note on message" )
                    {
                        CHECK( tracks.get_messages( 4 ) ==
                               make_message(
                                   note_on( tracks.track( 1 ).channel(), second_note, 32 ) ) );
                    }
                }

                WHEN( "clear_last_note is called" )
                {
                    tracks.clear_last_note();

                    WHEN( "in the first track the 5th step is set to note 2" )
                    {
                        const auto second_note = 2;
                        tracks.track( 0 )[ 4 ] = second_note;

                        THEN( "get_messages(4( returns note on message" )
                        {
                            CHECK( tracks.get_messages( 4 ) ==
                                   make_message(
                                       note_on( tracks.track( 0 ).channel(), second_note, 32 ) ) );
                        }
                    }
                }
            }

            WHEN( "clear is called" )
            {
                tracks.clear();

                THEN( "the rd step equals no note" )
                {
                    CHECK( tracks.track( 0 )[ 2 ] == track_t::no_note );
                }
            }
        }

        WHEN( "get_all_notes_off_message is called" )
        {
            const auto message = tracks.get_all_notes_off_message();

            THEN( "returns all notes off messages with corresponding channels" )
            {
                CHECK( message == make_message( all_notes_off( tracks.track( 0 ).channel() ) )
                                      .append( all_notes_off( tracks.track( 1 ).channel() ) ) );
            }
        }

        WHEN( "channel is channed to 1" )
        {
            tracks.track( 1 ).set_channel( 2 );

            THEN( "channel is 2" )
            {
                CHECK( tracks.track( 1 ).channel() == 2u );
            }

            WHEN( "get_all_notes_off_message is called" )
            {
                const auto message = tracks.get_all_notes_off_message();

                THEN( "returns all notes off messages with corresponding channels" )
                {
                    CHECK( message == make_message( all_notes_off( tracks.track( 2 ).channel() ) )
                                          .append( all_notes_off( tracks.track( 1 ).channel() ) ) );
                }
            }
        }
    }
}
