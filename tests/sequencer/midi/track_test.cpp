#include <sequencer/midi/track.hpp>

#include <catch2/catch.hpp>

using sequencer::midi::make_message;
using sequencer::midi::message_type;
using sequencer::midi::no_note;
using sequencer::midi::note_t;
using sequencer::midi::track_t;
using sequencer::midi::tracks_t;
using sequencer::midi::channel::mode::all_notes_off;
using sequencer::midi::channel::voice::note_off;
using sequencer::midi::channel::voice::note_on;

SCENARIO( "track_t", "[track]" )
{
    using namespace sequencer;
    constexpr auto number_of_steps = 16u;
    using track_t = track_t< number_of_steps >;

    GIVEN( "track_t with 4 steps" )
    {
        auto track = track_t{};
        REQUIRE( track.steps() == number_of_steps );
        REQUIRE( track.channel() == 0u );
        REQUIRE( track[ 0 ] == no_note() );

        WHEN( "the 3rd step is set to note 1" )
        {
            const auto first_note = note_t{1};
            track[ 2 ] = first_note;

            THEN( "the first step equals note 1" )
            {
                CHECK( track[ 2 ] == first_note );
            }

            THEN( "send_messages(1, sender) returns empty message" )
            {
                message_type received_message;
                const auto sender = [&received_message]( const auto& msg ) {
                    received_message = make_message( msg );
                };
                track.send_messages( 1, sender );
                CHECK( received_message.empty() );
            }

            THEN( "send_messages(2, sender) returns note on message" )
            {
                std::vector< message_type > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( make_message( msg ) );
                };
                track.send_messages( 2, sender );

                CHECK( received_messages.front() ==
                       make_message( note_on( track.channel(), to_uint8_t( first_note ), 32 ) ) );

                WHEN( "the 5th step is set to note 2" )
                {
                    const auto second_note = note_t{2};
                    track[ 4 ] = second_note;

                    THEN( "send_messages(4,sender) returns note off and note on message" )
                    {
                        track.send_messages( 4, sender );
                        REQUIRE( received_messages.size() == 3 );
                        CHECK( received_messages[ 1 ] ==
                               make_message(
                                   note_off( track.channel(), to_uint8_t( first_note ), 32 ) ) );
                        CHECK( received_messages[ 2 ] ==
                               make_message(
                                   note_on( track.channel(), to_uint8_t( second_note ), 32 ) ) );
                    }
                }

                WHEN( "send_all_notes_off_message is called" )
                {
                    track.send_all_notes_off_message( []( auto ) {} );

                    WHEN( "the 5th step is set to note 2" )
                    {
                        const auto second_note = note_t{2};
                        track[ 4 ] = second_note;

                        THEN( "send_messages(4,sender) returns note on message" )
                        {
                            track.send_messages( 4, sender );
                            REQUIRE( received_messages.size() == 2 );
                            CHECK( received_messages[ 1 ] ==
                                   make_message( note_on( track.channel(),
                                                          to_uint8_t( second_note ), 32 ) ) );
                        }
                    }
                }
            }

            WHEN( "clear is called" )
            {
                track.clear();

                THEN( "the 3rd step equals no note" )
                {
                    CHECK( track[ 2 ] == no_note() );
                }
            }
        }

        WHEN( "get_all_notes_off_message is called" )
        {
            message_type received_message;
            const auto sender = [&received_message]( const auto& msg ) {
                received_message = make_message( msg );
            };
            track.send_all_notes_off_message( sender );

            THEN( "returns all notes off message for its channel" )
            {
                CHECK( received_message == make_message( all_notes_off( track.channel() ) ) );
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
                message_type received_message;
                const auto sender = [&received_message]( const auto& msg ) {
                    received_message = make_message( msg );
                };
                track.send_all_notes_off_message( sender );

                THEN( "returns all notes off message for its channel" )
                {
                    CHECK( received_message == make_message( all_notes_off( track.channel() ) ) );
                }
            }
        }
    }
}

SCENARIO( "tracks_t", "[track]" )
{
    using namespace sequencer;
    constexpr auto number_of_steps = 16u;
    constexpr auto number_of_tracks = 2u;
    using track_t = tracks_t< number_of_steps, number_of_tracks >;

    GIVEN( "tracks_t with 4 steps" )
    {
        auto tracks = track_t{};
        REQUIRE( tracks.steps() == number_of_steps );
        REQUIRE( tracks.track( 0 ).channel() == 0u );
        REQUIRE( tracks.track( 1 ).channel() == 1u );
        REQUIRE( tracks.track( 0 )[ 0 ] == no_note() );
        REQUIRE( tracks.track( 1 )[ 0 ] == no_note() );

        WHEN( "in the first track the 3rd step is set to note 1" )
        {
            const auto first_note = note_t{1};
            tracks.track( 0 )[ 2 ] = first_note;

            THEN( "the first step equals note 1" )
            {
                CHECK( tracks.track( 0 )[ 2 ] == first_note );
            }

            THEN( "send_messages(1, sender) returns empty message" )
            {
                message_type received_message;
                const auto sender = [&received_message]( const auto& msg ) {
                    received_message = make_message( msg );
                };
                tracks.send_messages( 1, sender );
                CHECK( received_message.empty() );
            }

            THEN( "send_messages(2, sender) returns note on message" )
            {
                std::vector< message_type > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( make_message( msg ) );
                };
                tracks.send_messages( 2, sender );

                CHECK( received_messages.front() ==
                       make_message(
                           note_on( tracks.track( 0 ).channel(), to_uint8_t( first_note ), 32 ) ) );

                WHEN( "in the first track the 5th step is set to note 2" )
                {
                    const auto second_note = note_t{2};
                    tracks.track( 0 )[ 4 ] = second_note;

                    THEN( "send_messages(4, sender) returns note off and note on message" )
                    {
                        tracks.send_messages( 4, sender );
                        REQUIRE( received_messages.size() == 3 );
                        CHECK( received_messages[ 1 ] ==
                               make_message( note_off( tracks.track( 0 ).channel(),
                                                       to_uint8_t( first_note ), 32 ) ) );
                        CHECK( received_messages[ 2 ] ==
                               make_message( note_on( tracks.track( 0 ).channel(),
                                                      to_uint8_t( second_note ), 32 ) ) );
                    }
                }

                WHEN( "in the second track the 5th step is set to note 2" )
                {
                    const auto second_note = note_t{2};
                    tracks.track( 1 )[ 4 ] = second_note;

                    THEN( "send_messages(4,sender) returns note off and note on message" )
                    {
                        tracks.send_messages( 4, sender );
                        REQUIRE( received_messages.size() == 2 );
                        CHECK( received_messages[ 1 ] ==
                               make_message( note_on( tracks.track( 1 ).channel(),
                                                      to_uint8_t( second_note ), 32 ) ) );
                    }
                }

                WHEN( "send_all_notes_off_message is called" )
                {
                    tracks.send_all_notes_off_message( []( auto ) {} );

                    WHEN( "in the first track the 5th step is set to note 2" )
                    {
                        const auto second_note = note_t{2};
                        tracks.track( 0 )[ 4 ] = second_note;

                        THEN( "get_messages(4( returns note on message" )
                        {
                            tracks.send_messages( 4, sender );
                            REQUIRE( received_messages.size() == 2 );
                            CHECK( received_messages[ 1 ] ==
                                   make_message( note_on( tracks.track( 0 ).channel(),
                                                          to_uint8_t( second_note ), 32 ) ) );
                        }
                    }
                }
            }

            WHEN( "clear is called" )
            {
                tracks.clear();

                THEN( "the 3rd step equals no note" )
                {
                    CHECK( tracks.track( 0 )[ 2 ] == no_note() );
                }
            }
        }

        WHEN( "get_all_notes_off_message is called" )
        {
            std::vector< message_type > received_messages;
            const auto sender = [&received_messages]( const auto& msg ) {
                received_messages.push_back( make_message( msg ) );
            };
            tracks.send_all_notes_off_message( sender );

            THEN( "returns all notes off message for its channel" )
            {
                REQUIRE( received_messages.size() == 2 );
                CHECK( received_messages[ 0 ] ==
                       make_message( all_notes_off( tracks.track( 0 ).channel() ) ) );
                CHECK( received_messages[ 1 ] ==
                       make_message( all_notes_off( tracks.track( 1 ).channel() ) ) );
            }
        }

        WHEN( "channel is set to 2" )
        {
            tracks.track( 1 ).set_channel( 2 );

            THEN( "channel is 2" )
            {
                CHECK( tracks.track( 1 ).channel() == 2u );
            }

            WHEN( "get_all_notes_off_message is called" )
            {
                std::vector< message_type > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( make_message( msg ) );
                };
                tracks.send_all_notes_off_message( sender );

                THEN( "returns all notes off messages with corresponding channels" )
                {
                    REQUIRE( received_messages.size() == 2 );
                    CHECK( received_messages[ 0 ] ==
                           make_message( all_notes_off( tracks.track( 0 ).channel() ) ) );
                    CHECK( received_messages[ 1 ] ==
                           make_message( all_notes_off( tracks.track( 1 ).channel() ) ) );
                }
            }
        }
    }
}
