#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/chrono/testing_clock.hpp>
#include <sequencer/midi/clock.hpp>
#include <sequencer/midi/track.hpp>
#include <sequencer/midi/util.hpp>

#include <catch2/catch.hpp>

#include <condition_variable>

using sequencer::beat_duration;
using sequencer::operator""_beats;
using sequencer::midi::make_midi_clock_raii_shutdown;
using sequencer::midi::message_t;
using sequencer::midi::no_note;
using sequencer::midi::note_t;
using sequencer::midi::step_t;
using sequencer::midi::track_t;
using sequencer::midi::channel::voice::all_notes_off;
using sequencer::midi::channel::voice::note_off;
using sequencer::midi::channel::voice::note_on;
using sequencer::midi::realtime::realtime_clock;
using sequencer::midi::realtime::realtime_continue;
using sequencer::midi::realtime::realtime_start;
using sequencer::midi::realtime::realtime_stop;

constexpr auto velocity = std::uint8_t{100};

namespace
{
    struct track_parameter_t
    {
        constexpr auto note_offset() const noexcept
        {
            return 0;
        }

        constexpr void set_velocity( int velocity ) noexcept
        {
            velocity_ = velocity;
        }

        constexpr auto velocity() const noexcept
        {
            return velocity_;
        }

        constexpr void set_note( note_t note ) noexcept
        {
            note_ = note;
        }

        constexpr auto note() const noexcept
        {
            return note_;
        }

        constexpr void set_note_length( beat_duration length ) noexcept
        {
            note_length_ = length;
        }

        constexpr beat_duration note_length() const noexcept
        {
            return 1_beats;
        }

        constexpr bool lfo_enabled() const noexcept
        {
            return false;
        }

    private:
        int velocity_ = 100;
        note_t note_{1};
        beat_duration note_length_{0.25};
    };
} // namespace

SCENARIO( "track_t", "[track]" )
{
    constexpr auto number_of_steps = 16u;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        REQUIRE( track.steps() == number_of_steps );
        REQUIRE( track.channel() == 0u );
        REQUIRE( track[ 0 ] == step_t{} );

        WHEN( "the 3rd step is activated" )
        {
            const auto first_note = note_t{1};
            const auto first_velocity = std::uint8_t{80};
            track[ 2 ] = step_t{first_note, first_velocity};

            THEN( "the 3rd step is active" )
            {
                CHECK( track[ 2 ].is_active() );
            }

            THEN( "send_messages(1, sender) returns no message" )
            {
                auto received_message = false;
                const auto sender = [&received_message]( const auto& ) { received_message = true; };
                track.send_messages( 1, sender );
                CHECK_FALSE( received_message );
            }

            THEN( "send_messages(2, sender) returns note on message" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };
                track.send_messages( 2, sender );

                CHECK( received_messages.front() ==
                       note_on( track.channel(), to_uint8_t( first_note ), first_velocity ) );

                WHEN( "the 5th step is set to note 2" )
                {
                    const auto second_note = note_t{2};
                    const auto second_velocity = std::uint8_t{42};
                    track[ 4 ] = step_t{second_note, second_velocity};

                    THEN( "send_messages(4,sender) returns note off and note on message" )
                    {
                        track.send_messages( 4, sender );
                        REQUIRE( received_messages.size() == 2 );
                        CHECK( received_messages[ 1 ] == note_on( track.channel(),
                                                                  to_uint8_t( second_note ),
                                                                  second_velocity ) );
                    }

                    AND_WHEN( "the track is muted" )
                    {
                        track.mute();

                        THEN( "track is muted" )
                        {
                            CHECK( track.is_muted() );
                        }

                        THEN( "send_messages(4,sender) returns no messages" )
                        {
                            track.send_messages( 4, sender );
                            REQUIRE( received_messages.size() == 1 );
                        }

                        AND_WHEN( "track is unmuted" )
                        {
                            track.mute( false );

                            THEN( "track is not muted" )
                            {
                                CHECK_FALSE( track.is_muted() );
                            }

                            THEN( "send_messages(4,sender) returns note off and note on message" )
                            {
                                track.send_messages( 4, sender );
                                REQUIRE( received_messages.size() == 2 );
                                CHECK( received_messages[ 1 ] == note_on( track.channel(),
                                                                          to_uint8_t( second_note ),
                                                                          second_velocity ) );
                            }
                        }
                    }
                }

                WHEN( "send_all_notes_off_message is called" )
                {
                    track.send_all_notes_off_message( []( auto ) {} );

                    WHEN( "the 5th step is set to note 2" )
                    {
                        const auto second_note = note_t{2};
                        const auto second_velocity = std::uint8_t{42};
                        track[ 4 ] = step_t{second_note, second_velocity};

                        THEN( "send_messages(4,sender) returns note on message" )
                        {
                            track.send_messages( 4, sender );
                            REQUIRE( received_messages.size() == 2 );
                            CHECK( received_messages[ 1 ] == note_on( track.channel(),
                                                                      to_uint8_t( second_note ),
                                                                      second_velocity ) );
                        }
                    }
                }
            }

            WHEN( "clear is called" )
            {
                track.clear();

                THEN( "the 3rd step is empty" )
                {
                    CHECK( track[ 2 ] == step_t{} );
                }
            }
        }

        WHEN( "get_all_notes_off_message is called" )
        {
            message_t< 3 > received_message = {std::byte{0x00}};
            const auto sender = [&received_message]( const auto& msg ) { received_message = msg; };
            track.send_all_notes_off_message( sender );

            THEN( "returns all notes off message for its channel" )
            {
                CHECK( received_message == all_notes_off( track.channel() ) );
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
                message_t< 3 > received_message;
                const auto sender = [&received_message]( const auto& msg ) {
                    received_message = msg;
                };
                track.send_all_notes_off_message( sender );

                THEN( "returns all notes off message for its channel" )
                {
                    CHECK( received_message == all_notes_off( track.channel() ) );
                }
            }
        }
    }
}

SCENARIO( "track_t with different speed multipliers", "[track]" )
{
    constexpr auto number_of_steps = 16u;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        const auto first_note = note_t{1};
        const auto first_velocity = std::uint8_t{80};
        const auto first_step = step_t{first_note, first_velocity};
        track[ 4 ] = first_step;

        AND_WHEN( "a start and 24 clock messages are send" )
        {
            std::vector< message_t< 3 > > received_messages;
            const auto sender = [&received_messages]( const auto& msg ) {
                received_messages.push_back( msg );
            };

            track.send_messages( realtime_start(), sender );
            for ( auto i = 0; i < 24; ++i )
            {
                track.send_messages( realtime_clock(), sender );
            }

            THEN( "no messages are received" )
            {
                REQUIRE( received_messages.empty() );
            }
        }

        AND_WHEN( "speed multiplier is set to 2" )
        {
            track.set_speed_multiplier( 2 );

            AND_WHEN( "a start and 24 clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                for ( auto i = 0; i < 24; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "two messages are received" )
                {
                    REQUIRE( received_messages.size() == 2 );
                }
            }
        }
    }
}

SCENARIO( "track_t with trig_condition::deterministic<1,2>", "[track]" )
{
    namespace trig_condition = sequencer::midi::trig_condition;
    constexpr auto number_of_steps = 16u;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        const auto first_note = note_t{1};
        const auto first_velocity = std::uint8_t{80};
        const auto first_step = step_t{first_note, first_velocity};
        track[ 4 ] = first_step;
        track[ 4 ].set_trig_condition( trig_condition::deterministic< 1, 2 >{} );

        AND_WHEN( "a start and 4*24 clock messages are send" )
        {
            std::vector< message_t< 3 > > received_messages;
            const auto sender = [&received_messages]( const auto& msg ) {
                received_messages.push_back( msg );
            };

            track.send_messages( realtime_start(), sender );
            for ( auto i = 0; i < 4 * 24; ++i )
            {
                track.send_messages( realtime_clock(), sender );
            }

            THEN( "two messages are received" )
            {
                REQUIRE( received_messages.size() == 2 );
            }

            AND_WHEN( "another 4*24 clock messages are send" )
            {
                received_messages.clear();
                for ( auto i = 0; i < 4 * 24; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "no messages are received" )
                {
                    REQUIRE( received_messages.empty() );
                }
            }
        }
    }
}

SCENARIO( "track_t with trig_condition::deterministic<2,2>", "[track2]" )
{
    namespace trig_condition = sequencer::midi::trig_condition;
    constexpr auto number_of_steps = 16u;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        const auto first_note = note_t{1};
        const auto first_velocity = std::uint8_t{80};
        const auto first_step = step_t{first_note, first_velocity};
        track[ 4 ] = first_step;
        track[ 4 ].set_trig_condition( trig_condition::deterministic< 2, 2 >{} );

        AND_WHEN( "a start and 4*24 clock messages are send" )
        {
            std::vector< message_t< 3 > > received_messages;
            const auto sender = [&received_messages]( const auto& msg ) {
                received_messages.push_back( msg );
            };

            track.send_messages( realtime_start(), sender );
            for ( auto i = 0; i < 4 * 24; ++i )
            {
                track.send_messages( realtime_clock(), sender );
            }

            THEN( "no messages are received" )
            {
                REQUIRE( received_messages.empty() );
            }

            AND_WHEN( "another 4*24 clock messages are send" )
            {
                for ( auto i = 0; i < 4 * 24; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "two messages are received" )
                {
                    REQUIRE( received_messages.size() == 2 );
                }
            }
        }
    }
}

SCENARIO( "track_t set_steps", "[track]" )
{
    GIVEN( "an empty track" )
    {
        auto track = track_t< track_parameter_t >{};

        WHEN( "number of steps is changed to 3" )
        {
            track.set_steps( 3 );

            THEN( "track has 3 steps with no note" )
            {
                REQUIRE( track.steps() == 3 );
                REQUIRE( track[ 0 ] == step_t{} );
                REQUIRE( track[ 1 ] == step_t{} );
                REQUIRE( track[ 2 ] == step_t{} );
            }
        }
    }

    GIVEN( "a track with 4 steps and second step set to note 42" )
    {
        auto track = track_t< track_parameter_t >{4};
        const auto first_note = note_t{42};
        const auto first_velocity = std::uint8_t{80};
        const auto step = step_t{first_note, first_velocity};
        track[ 1 ] = step;

        WHEN( "number of steps is changed to 3" )
        {
            track.set_steps( 3 );

            THEN( "track has 3 steps with note 42 on second step" )
            {
                REQUIRE( track.steps() == 3 );
                REQUIRE( track[ 0 ] == step_t{} );
                REQUIRE( track[ 1 ] == step );
                REQUIRE( track[ 2 ] == step_t{} );
            }
        }

        WHEN( "number of steps is changed to 6" )
        {
            track.set_steps( 6 );

            THEN( "track has 6 steps with note 42 on second step" )
            {
                REQUIRE( track.steps() == 6 );
                REQUIRE( track[ 0 ] == step_t{} );
                REQUIRE( track[ 1 ] == step );
                REQUIRE( track[ 2 ] == step_t{} );
                REQUIRE( track[ 3 ] == step_t{} );
                REQUIRE( track[ 4 ] == step_t{} );
                REQUIRE( track[ 5 ] == step_t{} );
            }
        }
    }
}

SCENARIO( "track_t copy", "[track]" )
{
    GIVEN( "an empty track" )
    {
        auto track = track_t< track_parameter_t >{64, 0};

        WHEN( "track is copy-constructed" )
        {
            const auto other = track;

            THEN( "other track has 0 steps" )
            {
                REQUIRE( other.steps() == std::size_t{0} );
            }
        }

        WHEN( "track is copy-assigned to empty track" )
        {
            auto other = track_t< track_parameter_t >{64, 0};
            other = track;

            THEN( "other track has 0 steps" )
            {

                REQUIRE( other.steps() == std::size_t{0} );
            }
        }

        WHEN( "track is copy-assigned to track with 4 steps" )
        {
            auto other = track_t< track_parameter_t >{4};
            other = track;

            THEN( "other track has 0 steps" )
            {

                REQUIRE( other.steps() == std::size_t{0} );
            }
        }
    }

    GIVEN( "a track_t with 4 steps and note on step 1" )
    {
        auto track = track_t< track_parameter_t >{4};
        const auto first_note = note_t{42};
        const auto first_velocity = std::uint8_t{80};
        const auto step = step_t{first_note, first_velocity};
        track[ 1 ] = step;

        WHEN( "track is copy-constructed" )
        {
            const auto other = track;

            THEN( "other track has also one note on step 1" )
            {
                REQUIRE( other[ 0 ] == step_t{} );
                REQUIRE( other[ 1 ] == step );
                REQUIRE( other[ 2 ] == step_t{} );
                REQUIRE( other[ 3 ] == step_t{} );
            }
        }

        WHEN( "track is copy-assigned to empty track" )
        {
            auto other = track_t< track_parameter_t >{};
            other = track;

            THEN( "other track has 4 steps" )
            {
                REQUIRE( other.steps() == std::size_t{4} );
            }

            THEN( "other track has also one note on step 1" )
            {
                REQUIRE( other[ 0 ] == step_t{} );
                REQUIRE( other[ 1 ] == step );
                REQUIRE( other[ 2 ] == step_t{} );
                REQUIRE( other[ 3 ] == step_t{} );
            }
        }

        WHEN( "track is copy-assigned to track with 5 steps" )
        {
            auto other = track_t< track_parameter_t >{5};
            other = track;

            THEN( "other track has 4 steps" )
            {
                REQUIRE( other.steps() == std::size_t{4} );
            }

            THEN( "other track has also one note on step 1" )
            {
                REQUIRE( other[ 0 ] == step_t{} );
                REQUIRE( other[ 1 ] == step );
                REQUIRE( other[ 2 ] == step_t{} );
                REQUIRE( other[ 3 ] == step_t{} );
            }
        }

        WHEN( "track is copy-assigned to itself" )
        {
            track = track;

            THEN( "other track has 4 steps" )
            {

                REQUIRE( track.steps() == std::size_t{4} );
            }

            THEN( "track has still one note on step 1" )
            {
                REQUIRE( track[ 0 ] == step_t{} );
                REQUIRE( track[ 1 ] == step );
                REQUIRE( track[ 2 ] == step_t{} );
                REQUIRE( track[ 3 ] == step_t{} );
            }
        }
    }
}

SCENARIO( "track_t triggered by clock", "[track]" )
{
    constexpr auto number_of_steps = 16u;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        const auto first_note = note_t{1};
        const auto first_velocity = std::uint8_t{80};
        const auto first_step = step_t{first_note, first_velocity};
        const auto second_note = note_t{2};
        const auto second_velocity = std::uint8_t{50};
        const auto second_step = step_t{second_note, second_velocity};
        track[ 0 ] = first_step;
        track[ 2 ] = second_step;

        WHEN( "pulses per quarter note are 24 and steps per beat are 4" )
        {
            AND_WHEN( "a start and 6 clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                for ( auto i = 0; i < 6; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "a note on message is send" )
                {

                    REQUIRE( received_messages.size() == 1 );
                    REQUIRE( received_messages.front() ==
                             note_on( 0, to_uint8_t( first_note ), first_velocity ) );

                    WHEN( "another clock message is send" )
                    {
                        track.send_messages( realtime_clock(), sender );

                        THEN( "a note off message is send" )
                        {
                            REQUIRE( received_messages.size() == 2 );
                            REQUIRE( received_messages[ 1 ] ==
                                     note_off( 0, to_uint8_t( first_note ), 0 ) );
                        }

                        WHEN( "another 6 clock messages are send" )
                        {
                            for ( auto i = 0; i < 6; ++i )
                            {
                                track.send_messages( realtime_clock(), sender );
                            }

                            THEN( "a note on message is send" )
                            {
                                REQUIRE( received_messages.size() == 3 );
                                REQUIRE( received_messages[ 2 ] ==
                                         note_on( 0, to_uint8_t( second_note ), second_velocity ) );
                            }
                        }
                    }
                }
            }
        }

        WHEN( "pulses per quarter note are 8 and steps per beat are 4" )
        {
            track.set_pulses_per_quarter_note( 8 );

            AND_WHEN( "a start and one clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                track.send_messages( realtime_clock(), sender );
                track.send_messages( realtime_clock(), sender );

                THEN( "a note on message is send" )
                {
                    REQUIRE( received_messages.size() == 1 );
                    REQUIRE( received_messages.front() ==
                             note_on( 0, to_uint8_t( first_note ), first_velocity ) );

                    WHEN( "another clock message is send" )
                    {
                        track.send_messages( realtime_clock(), sender );

                        THEN( "a note off and note on message are send" )
                        {
                            REQUIRE( received_messages.size() == 2 );
                            REQUIRE( received_messages[ 1 ] ==
                                     note_off( 0, to_uint8_t( first_note ), 0 ) );
                        }

                        WHEN( "another 3 clock messages are send" )
                        {
                            track.send_messages( realtime_clock(), sender );
                            track.send_messages( realtime_clock(), sender );
                            track.send_messages( realtime_clock(), sender );

                            THEN( "a note off and note on message are send" )
                            {
                                REQUIRE( received_messages.size() == 3 );
                                REQUIRE( received_messages[ 2 ] ==
                                         note_on( 0, to_uint8_t( second_note ), second_velocity ) );
                            }
                        }
                    }
                }
            }
        }

        WHEN( "pulses per quarter note are 8 and steps per beat are 2" )
        {
            track.set_pulses_per_quarter_note( 8 );
            track.set_steps_per_beat( 2 );

            AND_WHEN( "a start and four clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                for ( auto i = 0; i < 4; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "a note on message is send" )
                {
                    REQUIRE( received_messages.size() == 1 );
                    REQUIRE( received_messages.front() ==
                             note_on( 0, to_uint8_t( first_note ), first_velocity ) );

                    WHEN( "another clock message is send" )
                    {
                        track.send_messages( realtime_clock(), sender );

                        THEN( "a note off and note on message are send" )
                        {
                            REQUIRE( received_messages.size() == 2 );
                            REQUIRE( received_messages[ 1 ] ==
                                     note_off( 0, to_uint8_t( first_note ), 0 ) );
                        }

                        WHEN( "another 4 clock messages are send" )
                        {
                            for ( auto i = 0; i < 4; ++i )
                            {
                                track.send_messages( realtime_clock(), sender );
                            }

                            THEN( "a note off and note on message are send" )
                            {
                                REQUIRE( received_messages.size() == 3 );
                                REQUIRE( received_messages[ 2 ] ==
                                         note_on( 0, to_uint8_t( second_note ), second_velocity ) );
                            }
                        }
                    }
                }
            }
        }
    }
}

namespace
{
    struct clock_message_count_t
    {
        explicit clock_message_count_t( int count ) noexcept : count( count )
        {
        }

        clock_message_count_t& operator++() noexcept
        {
            std::unique_lock lock{mutex};
            ++count;
            lock.unlock();
            message_received.notify_one();
            return *this;
        }

        void wait_for_count( int expected_count )
        {
            std::unique_lock lock( mutex );
            message_received.wait( lock,
                                   [this, expected_count] { return count == expected_count; } );
        }

        std::mutex mutex;
        std::condition_variable message_received;
        int count{0};
    };
} // namespace

SCENARIO( "tracks_t, that is triggered by a midi clock, plays 4 beats", "[track]" )
{
    using namespace std::literals::chrono_literals;

    using underlying_clock_type = sequencer::chrono::steady_testing_clock<>;
    using sequencer_clock_type = sequencer::chrono::sequencer_clock< const underlying_clock_type& >;

    GIVEN( "an empty track" )
    {
        std::vector< message_t< 3 > > received_messages;
        const auto midi_sender = [&received_messages]( const auto& message ) {
            received_messages.push_back( message );
        };

        constexpr auto steps = 4u;
        auto midi_track = std::vector( 1, track_t< track_parameter_t >{steps} );
        underlying_clock_type testing_clock;
        sequencer_clock_type sequencer_clock{testing_clock};

        clock_message_count_t clock_message_count{0};
        const auto clock_sender = [&midi_track, &clock_message_count,
                                   &midi_sender]( auto message ) {
            send_messages( midi_track, message, midi_sender );
            ++clock_message_count;
        };
        auto midi_clock = sequencer::midi::clock{sequencer_clock};

        auto thread_ready_promise = std::make_shared< std::promise< void > >();
        const auto thread_ready = thread_ready_promise->get_future();
        const auto clock_done =
            std::async( std::launch::async, [&clock_sender, &midi_clock,
                                             thread_ready = std::move( thread_ready_promise )] {
                thread_ready->set_value();
                midi_clock.run( clock_sender );
            } );
        // make sure that midi_clock.shut_down() is called before the blocking destructor of
        // clock_done is called
        const auto midi_clock_raii_shutdown =
            make_midi_clock_raii_shutdown( midi_clock, clock_done );
        thread_ready.wait();
        midi_clock.start();

        WHEN( "receives start message and 96 clock messages (i.e. 4 beats @ 24 pulses "
              "per beat)" )
        {
            testing_clock.set( testing_clock.now() + 1999ms );
            clock_message_count.wait_for_count( 97 );

            THEN( "no message is sent" )
            {
                CHECK( received_messages.empty() );
            }
        }
    }

    GIVEN( "a track with notes on first two quarters" )
    {
        std::vector< message_t< 3 > > received_messages;
        const auto midi_sender = [&received_messages]( const auto& message ) {
            received_messages.push_back( message );
        };

        constexpr auto steps = 16u;
        auto midi_track = std::vector( 1, track_t< track_parameter_t >{steps} );
        const auto first_note = note_t{1};
        const auto first_velocity = std::uint8_t{80};
        const auto first_step = step_t{first_note, first_velocity};
        const auto second_note = note_t{2};
        const auto second_velocity = std::uint8_t{50};
        const auto second_step = step_t{second_note, second_velocity};
        midi_track[ 0 ][ 0 ] = first_step;
        midi_track[ 0 ][ 4 ] = second_step;
        underlying_clock_type testing_clock;
        sequencer_clock_type sequencer_clock{testing_clock};

        clock_message_count_t clock_message_count{0};
        const auto clock_sender = [&midi_track, &clock_message_count,
                                   &midi_sender]( auto message ) {
            send_messages( midi_track, message, midi_sender );
            ++clock_message_count;
        };
        auto midi_clock = sequencer::midi::clock{sequencer_clock};

        auto thread_ready_promise = std::make_shared< std::promise< void > >();
        const auto thread_ready = thread_ready_promise->get_future();
        const auto clock_done =
            std::async( std::launch::async, [&clock_sender, &midi_clock,
                                             thread_ready = std::move( thread_ready_promise )] {
                thread_ready->set_value();
                midi_clock.run( clock_sender );
            } );
        // make sure that midi_clock.shut_down() is called before the blocking destructor of
        // clock_done is called
        const auto midi_clock_raii_shutdown =
            make_midi_clock_raii_shutdown( midi_clock, clock_done );
        thread_ready.wait();
        midi_clock.start();

        WHEN( "testing clock runs for 10 ms" )
        {
            testing_clock.set( testing_clock.now() + 10ms );
            clock_message_count.wait_for_count( 2 );

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 1 );
                CHECK( received_messages.front() ==
                       note_on( 0, to_uint8_t( first_note ), first_velocity ) );
            }
        }

        WHEN( "testing clock runs for 501 ms" )
        {
            testing_clock.set( testing_clock.now() + 501ms );
            clock_message_count.wait_for_count( 26 );

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 3 );
                CHECK( received_messages[ 0 ] ==
                       note_on( 0, to_uint8_t( first_note ), first_velocity ) );

                AND_THEN( "one note off message is send" )
                {
                    CHECK( received_messages[ 1 ] == note_off( 0, to_uint8_t( first_note ), 0 ) );

                    AND_THEN( "one note on message is send" )
                    {
                        CHECK( received_messages[ 2 ] ==
                               note_on( 0, to_uint8_t( second_note ), second_velocity ) );
                    }
                }
            }
        }

        WHEN( "testing clock runs for 499 ms" )
        {
            testing_clock.set( testing_clock.now() + 499ms );
            clock_message_count.wait_for_count( 25 );

            THEN( "one note on and one note off message are send" )
            {
                REQUIRE( received_messages.size() == 2 );
                CHECK( received_messages[ 0 ] ==
                       note_on( 0, to_uint8_t( first_note ), first_velocity ) );
                CHECK( received_messages[ 1 ] == note_off( 0, to_uint8_t( first_note ), 0 ) );
            }

            WHEN( "midi clock is stopped" )
            {
                midi_clock.stop();
                clock_message_count.wait_for_count( 26 );

                THEN( "all notes off message is send" )
                {
                    REQUIRE( received_messages.size() == 3 );
                    CHECK( received_messages[ 2 ] == all_notes_off( 0 ) );
                }

                AND_WHEN( "midi clock is started again and testing clock runs for 499 ms" )
                {
                    midi_clock.start();
                    testing_clock.set( testing_clock.now() + 499ms );
                    clock_message_count.wait_for_count( 51 );

                    THEN( "one note on and one note off message are send" )
                    {
                        REQUIRE( received_messages.size() == 5 );
                        CHECK( received_messages[ 3 ] ==
                               note_on( 0, to_uint8_t( second_note ), second_velocity ) );
                        CHECK( received_messages[ 4 ] ==
                               note_off( 0, to_uint8_t( second_note ), 0 ) );
                    }
                }

                AND_WHEN(
                    "midi clock is reset and then started again and testing clock runs for 501 ms" )
                {
                    midi_clock.reset();
                    midi_clock.start();
                    testing_clock.set( testing_clock.now() + 10ms );
                    clock_message_count.wait_for_count( 28 );
                    testing_clock.set( testing_clock.now() + 491ms );
                    clock_message_count.wait_for_count( 52 );

                    THEN( "one note on and one note off message is send" )
                    {
                        REQUIRE( received_messages.size() == 6 );
                        CHECK( received_messages[ 3 ] ==
                               note_on( 0, to_uint8_t( first_note ), first_velocity ) );

                        AND_THEN( "one note off message is send" )
                        {
                            CHECK( received_messages[ 4 ] ==
                                   note_off( 0, to_uint8_t( first_note ), 0 ) );

                            AND_THEN( "one note on message is send" )
                            {
                                CHECK( received_messages[ 5 ] ==
                                       note_on( 0, to_uint8_t( second_note ), second_velocity ) );
                            }
                        }
                    }
                }
            }
        }
    }
}
