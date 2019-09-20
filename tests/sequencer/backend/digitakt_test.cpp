#include <sequencer/backend/digitakt.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/realtime.hpp>

#include <catch2/catch.hpp>

SCENARIO( "set step in play mode", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{get_spec( "device_spec/elektron/digitakt.txt" )};
        REQUIRE( backend.mode() == digitakt_mode::play );

        WHEN( "set_step(2, true) is called" )
        {
            backend.set_step( 2, true );

            THEN( "the 3rd step of the current track gets a note" )
            {
                CHECK( backend.current_track()[ 2 ].is_active() );
            }

            AND_WHEN( "set_step(2, false) is called" )
            {
                backend.set_step( 2, false );

                THEN( "the 3rd step of the current track is set to no_note" )
                {
                    CHECK_FALSE( backend.current_track()[ 2 ].is_active() );
                }
            }
        }
    }
}

SCENARIO( "set step in mute mode", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{get_spec( "device_spec/elektron/digitakt.txt" )};
        backend.set_mode( digitakt_mode::track_select );
        backend.set_step( 2 );

        WHEN( "mode is set to mute" )
        {
            backend.set_mode( digitakt_mode::mute );
            CHECK( backend.mode() == digitakt_mode::mute );

            AND_WHEN( "mode is set to mute again" )
            {
                backend.set_mode( digitakt_mode::mute );

                THEN( "mode is back to play" )
                CHECK( backend.mode() == digitakt_mode::play );
            }

            AND_WHEN( "set_step(2, true) is called" )
            {
                backend.set_step( 2, true );

                THEN( "the 3rd step of the current track has no note" )
                {
                    CHECK_FALSE( backend.current_track()[ 2 ].is_active() );
                }

                THEN( "the current track is muted" )
                {
                    CHECK( backend.current_track().is_muted() );
                }

                AND_WHEN( "set_step(2, false) is called" )
                {
                    backend.set_step( 2, false );

                    THEN( "the current track is unmuted" )
                    {
                        CHECK_FALSE( backend.current_track().is_muted() );
                    }
                }
            }
        }
    }
}

SCENARIO( "track select", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{get_spec( "device_spec/elektron/digitakt.txt" )};

        WHEN( "set_step(2, true) is called" )
        {
            backend.set_step( 2, true );

            AND_WHEN( "mode is set to track_select" )
            {
                backend.set_mode( digitakt_mode::track_select );
                CHECK( backend.mode() == digitakt_mode::track_select );

                AND_WHEN( "set_step(2) is called" )
                {
                    backend.set_step( 2 );

                    THEN( "the 3rd step of the current track is set to no_note" )
                    {
                        CHECK_FALSE( backend.current_track()[ 2 ].is_active() );
                    }

                    THEN( "mode is play" )
                    {
                        CHECK( backend.mode() == digitakt_mode::play );
                    }

                    AND_WHEN( "mode is set to track_select" )
                    {
                        backend.set_mode( digitakt_mode::track_select );
                        CHECK( backend.mode() == digitakt_mode::track_select );

                        AND_WHEN( "set_step(0) is called" )
                        {
                            backend.set_step( 0 );

                            THEN( "the 3rd step of the current track has a note" )
                            {
                                CHECK( backend.current_track()[ 2 ].is_active() );
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "pattern select", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{get_spec( "device_spec/elektron/digitakt.txt" )};

        WHEN( "set_step(2, true) is called" )
        {
            backend.set_step( 2, true );

            AND_WHEN( "mode is set to pattern_select" )
            {
                backend.set_mode( digitakt_mode::pattern_select );
                CHECK( backend.mode() == digitakt_mode::pattern_select );

                AND_WHEN( "set_step(2) is called" )
                {
                    backend.set_step( 2 );

                    THEN( "the 3rd step of the current track is set to no_note" )
                    {
                        CHECK_FALSE( backend.current_track()[ 2 ].is_active() );
                    }

                    THEN( "mode is play" )
                    {
                        CHECK( backend.mode() == digitakt_mode::play );
                    }

                    AND_WHEN( "mode is set to pattern_select" )
                    {
                        backend.set_mode( digitakt_mode::pattern_select );
                        CHECK( backend.mode() == digitakt_mode::pattern_select );

                        AND_WHEN( "set_step(0) is called" )
                        {
                            backend.set_step( 0 );

                            THEN( "the 3rd step of the current track has a note" )
                            {
                                CHECK( backend.current_track()[ 2 ].is_active() );
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "bank select", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{get_spec( "device_spec/elektron/digitakt.txt" )};
        backend.set_mode( digitakt_mode::pattern_select );
        backend.set_step( 2 );
        backend.set_mode( digitakt_mode::track_select );
        backend.set_step( 1 );

        WHEN( "set_step(2, true) is called" )
        {
            backend.set_step( 2, true );

            AND_WHEN( "mode is set to bank_select" )
            {
                backend.set_mode( digitakt_mode::bank_select );
                CHECK( backend.mode() == digitakt_mode::bank_select );

                AND_WHEN( "set_step(3) is called" )
                {
                    backend.set_step( 3 );

                    THEN( "mode is pattern_select" )
                    {
                        CHECK( backend.mode() == digitakt_mode::pattern_select );
                    }

                    AND_WHEN( "set_step(2) is called" )
                    {
                        backend.set_step( 2 );

                        THEN( "the 3rd step of the current track is set to no_note" )
                        {
                            CHECK_FALSE( backend.current_track()[ 2 ].is_active() );
                        }

                        THEN( "mode is play" )
                        {
                            CHECK( backend.mode() == digitakt_mode::play );
                        }

                        AND_WHEN( "mode is set to bank_select" )
                        {
                            backend.set_mode( digitakt_mode::bank_select );
                            CHECK( backend.mode() == digitakt_mode::bank_select );

                            AND_WHEN( "set_step(0) is called" )
                            {
                                backend.set_step( 0 );

                                THEN( "mode is pattern_select" )
                                {
                                    CHECK( backend.mode() == digitakt_mode::pattern_select );
                                }

                                AND_WHEN( "set_step(2) is called" )
                                {
                                    backend.set_step( 2 );

                                    THEN( "the 3rd step of the current track has a note" )
                                    {
                                        CHECK( backend.current_track()[ 2 ].is_active() );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "set step in play mode with different track notes", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;
    using namespace sequencer::midi::realtime;
    using namespace sequencer::midi::channel::voice;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{get_spec( "device_spec/elektron/digitakt.txt" )};
        REQUIRE( backend.mode() == digitakt_mode::play );

        AND_GIVEN( "with control mode trig" )
        {
            backend.set_control_mode( digitakt_control_mode::trig );
            CHECK( backend.control_mode() == digitakt_control_mode::trig );

            WHEN( "set_step(2, true) is called" )
            {
                backend.set_step( 2, true );

                AND_WHEN( "13 clock messages are sent" )
                {
                    std::vector< midi::message_t< 3 > > received_messages;
                    const auto sender = [&received_messages]( const auto& msg ) {
                        received_messages.push_back( msg );
                    };

                    backend.receive_clock_message( realtime_start(), sender );
                    for ( auto i = 0u; i < 13; ++i )
                    {
                        backend.receive_clock_message( realtime_clock(), sender );
                    }

                    THEN( "one message is received" )
                    {
                        REQUIRE( received_messages.size() == 1 );
                    }

                    const auto base_note =
                        static_cast< std::uint8_t >( received_messages.front()[ 1 ] );
                    AND_WHEN( "first control is changed to +3" )
                    {
                        backend.set_control( 0, 3, []( auto ) {} );

                        AND_WHEN( "13 clock messages are sent" )
                        {
                            received_messages.clear();
                            backend.receive_clock_message( realtime_start(), sender );
                            for ( auto i = 0u; i < 13; ++i )
                            {
                                backend.receive_clock_message( realtime_clock(), sender );
                            }

                            THEN( "two messages are received" )
                            {
                                REQUIRE( received_messages.size() == 2 );
                            }

                            THEN( "the note is three above the first received note" )
                            {
                                CHECK( base_note + 3 == static_cast< std::uint8_t >(
                                                            received_messages.back()[ 1 ] ) );
                            }

                            AND_WHEN( "first control is changed back to 0" )
                            {
                                backend.set_control( 0, 0, []( auto ) {} );

                                AND_WHEN( "13 clock messages are sent" )
                                {
                                    received_messages.clear();
                                    backend.receive_clock_message( realtime_start(), sender );
                                    for ( auto i = 0u; i < 13; ++i )
                                    {
                                        backend.receive_clock_message( realtime_clock(), sender );
                                    }

                                    THEN( "two messages are received" )
                                    {
                                        REQUIRE( received_messages.size() == 2 );
                                    }

                                    THEN( "the note is equal to the first received note" )
                                    {
                                        CHECK( base_note == static_cast< std::uint8_t >(
                                                                received_messages.back()[ 1 ] ) );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "set step in play mode with different track velocities", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;
    using namespace sequencer::midi::realtime;
    using namespace sequencer::midi::channel::voice;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{get_spec( "device_spec/elektron/digitakt.txt" )};
        REQUIRE( backend.mode() == digitakt_mode::play );

        AND_GIVEN( "with control mode trig" )
        {
            backend.set_control_mode( digitakt_control_mode::trig );
            CHECK( backend.control_mode() == digitakt_control_mode::trig );

            WHEN( "set_step(2, true) is called" )
            {
                backend.set_step( 2, true );

                AND_WHEN( "13 clock messages are sent" )
                {
                    std::vector< midi::message_t< 3 > > received_messages;
                    const auto sender = [&received_messages]( const auto& msg ) {
                        received_messages.push_back( msg );
                    };

                    backend.receive_clock_message( realtime_start(), sender );
                    for ( auto i = 0u; i < 13; ++i )
                    {
                        backend.receive_clock_message( realtime_clock(), sender );
                    }

                    THEN( "one message is received with velocity 100" )
                    {
                        REQUIRE( received_messages.size() == 1 );
                        CHECK( received_messages.front()[ 0 ] == midi::byte::note_on );
                        CHECK( static_cast< std::uint8_t >( received_messages.front()[ 2 ] ) ==
                               100 );
                    }

                    AND_WHEN( "second control is changed to +3" )
                    {
                        backend.set_control( 1, 3, []( auto ) {} );

                        AND_WHEN( "13 clock messages are sent" )
                        {
                            received_messages.clear();
                            backend.receive_clock_message( realtime_start(), sender );
                            for ( auto i = 0u; i < 13; ++i )
                            {
                                backend.receive_clock_message( realtime_clock(), sender );
                            }

                            THEN( "two messages are received" )
                            {
                                REQUIRE( received_messages.size() == 2 );
                            }

                            THEN( "the velocity is three" )
                            {
                                CHECK( static_cast< std::uint8_t >(
                                           received_messages.back()[ 2 ] ) == 3 );
                            }

                            AND_WHEN( "first control is changed back to 100" )
                            {
                                backend.set_control( 1, 100, []( auto ) {} );

                                AND_WHEN( "13 clock messages are sent" )
                                {
                                    received_messages.clear();
                                    backend.receive_clock_message( realtime_start(), sender );
                                    for ( auto i = 0u; i < 13; ++i )
                                    {
                                        backend.receive_clock_message( realtime_clock(), sender );
                                    }

                                    THEN( "two messages are received" )
                                    {
                                        REQUIRE( received_messages.size() == 2 );
                                    }

                                    THEN( "the velocity is 100" )
                                    {
                                        CHECK( static_cast< std::uint8_t >(
                                                   received_messages.back()[ 2 ] ) == 100 );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "set step in play mode with different note velocities", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;
    using namespace sequencer::midi::realtime;
    using namespace sequencer::midi::channel::voice;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{get_spec( "device_spec/elektron/digitakt.txt" )};
        REQUIRE( backend.mode() == digitakt_mode::play );

        AND_GIVEN( "with control mode trig" )
        {
            backend.set_control_mode( digitakt_control_mode::trig );
            CHECK( backend.control_mode() == digitakt_control_mode::trig );

            WHEN( "the 3rd step is set" )
            {
                backend.set_step( 2, true );

                AND_WHEN( "13 clock messages are sent" )
                {
                    std::vector< midi::message_t< 3 > > received_messages;
                    const auto sender = [&received_messages]( const auto& msg ) {
                        received_messages.push_back( msg );
                    };

                    backend.receive_clock_message( realtime_start(), sender );
                    for ( auto i = 0u; i < 13; ++i )
                    {
                        backend.receive_clock_message( realtime_clock(), sender );
                    }

                    THEN( "one message is received with velocity 100" )
                    {
                        REQUIRE( received_messages.size() == 1 );
                        CHECK( static_cast< std::uint8_t >( received_messages.front()[ 2 ] ) ==
                               100 );
                    }

                    AND_WHEN( "the fourth step is set" )
                    {
                        backend.set_step( 3 );

                        AND_WHEN( "the fourth step is selected" )
                        {
                            backend.set_mode( digitakt_mode::step_select );
                            backend.set_step( 3 );

                            AND_WHEN( "the velocity of the fourth step is changed to 80" )
                            {
                                backend.set_control( 1, 80, []( auto ) {} );

                                AND_WHEN( "19 clock messages are sent" )
                                {
                                    received_messages.clear();
                                    backend.receive_clock_message( realtime_start(), sender );
                                    for ( auto i = 0u; i < 19; ++i )
                                    {
                                        backend.receive_clock_message( realtime_clock(), sender );
                                    }

                                    THEN( "four messages are received" )
                                    {
                                        REQUIRE( received_messages.size() == 4 );
                                    }

                                    THEN( "the velocities are 100 and 80" )
                                    {
                                        CHECK( static_cast< std::uint8_t >(
                                                   received_messages[ 1 ][ 2 ] ) == 100 );
                                        CHECK( static_cast< std::uint8_t >(
                                                   received_messages[ 3 ][ 2 ] ) == 80 );
                                    }

                                    AND_WHEN( "19 clock messages are sent" )
                                    {
                                        received_messages.clear();
                                        backend.receive_clock_message( realtime_start(), sender );
                                        for ( auto i = 0u; i < 19; ++i )
                                        {
                                            backend.receive_clock_message( realtime_clock(),
                                                                           sender );
                                        }

                                        THEN( "four messages are received" )
                                        {
                                            REQUIRE( received_messages.size() == 4 );
                                        }

                                        THEN( "the velocities are 100 and 80" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 1 ][ 2 ] ) == 100 );
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 3 ][ 2 ] ) == 80 );
                                        }
                                    }

                                    AND_WHEN( "the fourth step is deselected" )
                                    {
                                        backend.set_step( 3 );
                                        CHECK( backend.mode() == digitakt_mode::play );

                                        AND_WHEN( "the velocity is changed to 90" )
                                        {
                                            CHECK_FALSE( backend.current_track()[ 2 ].velocity() );
                                            CHECK( *backend.current_track()[ 3 ].velocity() == 80 );
                                            backend.set_control( 1, 90, []( auto ) {} );
                                            CHECK_FALSE( backend.current_track()[ 2 ].velocity() );
                                            CHECK( *backend.current_track()[ 3 ].velocity() == 80 );

                                            AND_WHEN( "19 clock messages are sent" )
                                            {
                                                received_messages.clear();
                                                backend.receive_clock_message( realtime_start(),
                                                                               sender );
                                                for ( auto i = 0u; i < 19; ++i )
                                                {
                                                    backend.receive_clock_message( realtime_clock(),
                                                                                   sender );
                                                }

                                                THEN( "four messages are received" )
                                                {
                                                    REQUIRE( received_messages.size() == 4 );
                                                }

                                                THEN( "the velocities are 80 and 90" )
                                                {
                                                    CHECK( static_cast< std::uint8_t >(
                                                               received_messages[ 1 ][ 2 ] ) ==
                                                           90 );
                                                    CHECK( static_cast< std::uint8_t >(
                                                               received_messages[ 3 ][ 2 ] ) ==
                                                           80 );
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "set step in play mode with different note lengths", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;
    using namespace sequencer::midi::realtime;
    using namespace sequencer::midi::channel::voice;

    GIVEN( "a digitakt backend" )
    {
        auto backend = digitakt{get_spec( "device_spec/elektron/digitakt.txt" )};
        REQUIRE( backend.mode() == digitakt_mode::play );

        AND_GIVEN( "with control mode trig" )
        {
            backend.set_control_mode( digitakt_control_mode::trig );
            CHECK( backend.control_mode() == digitakt_control_mode::trig );

            WHEN( "the first step is set" )
            {
                backend.set_step( 0 );

                AND_WHEN( "six clock messages are send" )
                {
                    std::vector< midi::message_t< 3 > > received_messages;
                    const auto sender = [&received_messages]( const auto& msg ) {
                        received_messages.push_back( msg );
                    };

                    backend.receive_clock_message( realtime_start(), sender );
                    for ( auto i = 0u; i < 6; ++i )
                    {
                        backend.receive_clock_message( realtime_clock(), sender );
                    }

                    THEN( "one note on message is received" )
                    {
                        REQUIRE( received_messages.size() == 1 );
                        CHECK( received_messages[ 0 ][ 0 ] == midi::byte::note_on );
                    }

                    AND_WHEN( "another clock message is send" )
                    {
                        backend.receive_clock_message( realtime_clock(), sender );

                        THEN( "one note off message is received" )
                        {
                            REQUIRE( received_messages.size() == 2 );
                            CHECK( received_messages[ 1 ][ 0 ] == midi::byte::note_off );
                        }
                    }
                }

                AND_WHEN( "step length is set to 1/8 note" )
                {
                    backend.set_control( digitakt_track_parameter_t::length_idx, 30,
                                         []( auto ) {} );

                    AND_WHEN( "12 clock messages are send" )
                    {
                        std::vector< midi::message_t< 3 > > received_messages;
                        const auto sender = [&received_messages]( const auto& msg ) {
                            received_messages.push_back( msg );
                        };

                        backend.receive_clock_message( realtime_start(), sender );
                        for ( auto i = 0u; i < 12; ++i )
                        {
                            backend.receive_clock_message( realtime_clock(), sender );
                        }

                        THEN( "one note on message is received" )
                        {
                            REQUIRE( received_messages.size() == 1 );
                            CHECK( received_messages[ 0 ][ 0 ] == midi::byte::note_on );
                        }

                        AND_WHEN( "another clock message is send" )
                        {
                            backend.receive_clock_message( realtime_clock(), sender );

                            THEN( "one note off message is received" )
                            {
                                REQUIRE( received_messages.size() == 2 );
                                CHECK( received_messages[ 1 ][ 0 ] == midi::byte::note_off );
                            }
                        }
                    }
                }
            }
        }
    }
}
