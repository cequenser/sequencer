#include <sequencer/backend/digitakt.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/realtime.hpp>

#include <catch2/catch.hpp>

SCENARIO( "lfo for velocity with rectangular wave form", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;
    using namespace sequencer::midi::realtime;

    GIVEN( "a digitakt backend" )
    {
        auto backend =
            digitakt::backend_impl{digitakt::get_spec( "device_spec/elektron/digitakt.txt" )};
        REQUIRE( backend.mode() == digitakt::mode_t::play );

        WHEN( "lfo with square wave is set" )
        {
            backend.set_control_mode( digitakt::control_mode_t::lfo );
            backend.set_control( digitakt::track_parameter_t::lfo_idx::wave_form,
                                 digitakt::track_parameter_t::wave_from_idx::square,
                                 []( auto ) {} );

            AND_WHEN( "speed is set to 32" )
            {
                backend.set_control( digitakt::track_parameter_t::lfo_idx::speed, 32,
                                     []( auto ) {} );

                AND_WHEN( "phase is set to 0" )
                {
                    backend.set_control( digitakt::track_parameter_t::lfo_idx::phase, 0,
                                         []( auto ) {} );

                    AND_WHEN( "depth is set to 128" )
                    {
                        backend.set_control( digitakt::track_parameter_t::lfo_idx::depth, 128,
                                             []( auto ) {} );

                        AND_WHEN( "destination is set to velocity" )
                        {
                            backend.set_control( digitakt::track_parameter_t::lfo_idx::destination,
                                                 2, []( auto ) {} );

                            AND_WHEN( "multiplier is set to 2" )
                            {
                                backend.set_control(
                                    digitakt::track_parameter_t::lfo_idx::multiplier, 1,
                                    []( auto ) {} );

                                AND_WHEN( "2*4*24 clock messages are sent" )
                                {
                                    std::vector< midi::message_t< 3 > > received_messages;
                                    const auto sender = [&received_messages]( const auto& msg ) {
                                        received_messages.push_back( msg );
                                    };

                                    backend.receive_clock_message( realtime_start(), sender );
                                    for ( auto i = 0u; i < 2 * 4 * 24; ++i )
                                    {
                                        backend.receive_clock_message( realtime_clock(), sender );
                                    }

                                    THEN( "for each pulse one message is received" )
                                    {
                                        REQUIRE( received_messages.size() == 2 * 4 * 24 );

                                        AND_THEN( "the first 4*24 messages have velocity 127" )
                                        {
                                            for ( auto i = 0u; i < 4 * 24; ++i )
                                            {
                                                CHECK( static_cast< std::uint8_t >(
                                                           received_messages[ i ][ 2 ] ) == 127 );
                                            }
                                        }

                                        AND_THEN( "the second 4*24 messages have velocity 0" )
                                        {
                                            const auto offset = 4u * 24u;
                                            for ( auto i = offset; i < offset + 4 * 24; ++i )
                                            {
                                                CHECK( static_cast< std::uint8_t >(
                                                           received_messages[ i ][ 2 ] ) == 0 );
                                            }
                                        }
                                    }
                                }

                                AND_WHEN( "phase is set to 64" )
                                {
                                    backend.set_control(
                                        digitakt::track_parameter_t::lfo_idx::phase, 64,
                                        []( auto ) {} );

                                    AND_WHEN( "2*4*24 clock messages are sent" )
                                    {
                                        std::vector< midi::message_t< 3 > > received_messages;
                                        const auto sender =
                                            [&received_messages]( const auto& msg ) {
                                                received_messages.push_back( msg );
                                            };

                                        backend.receive_clock_message( realtime_start(), sender );
                                        for ( auto i = 0u; i < 2 * 4 * 24; ++i )
                                        {
                                            backend.receive_clock_message( realtime_clock(),
                                                                           sender );
                                        }

                                        THEN( "for each pulse one message is received" )
                                        {
                                            REQUIRE( received_messages.size() == 2 * 4 * 24 );

                                            AND_THEN( "the first 4*24 messages have velocity 0" )
                                            {
                                                for ( auto i = 0u; i < 4 * 24; ++i )
                                                {
                                                    CHECK( static_cast< std::uint8_t >(
                                                               received_messages[ i ][ 2 ] ) == 0 );
                                                }
                                            }

                                            AND_THEN( "the second 4*24 messages have velocity 127" )
                                            {
                                                const auto offset = 4u * 24u;
                                                for ( auto i = offset; i < offset + 4 * 24; ++i )
                                                {
                                                    CHECK( static_cast< std::uint8_t >(
                                                               received_messages[ i ][ 2 ] ) ==
                                                           127 );
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            AND_WHEN( "multiplier is set to 4" )
                            {
                                backend.set_control(
                                    digitakt::track_parameter_t::lfo_idx::multiplier, 2,
                                    []( auto ) {} );

                                AND_WHEN( "2*4*24 clock messages are sent" )
                                {
                                    std::vector< midi::message_t< 3 > > received_messages;
                                    const auto sender = [&received_messages]( const auto& msg ) {
                                        received_messages.push_back( msg );
                                    };

                                    backend.receive_clock_message( realtime_start(), sender );
                                    for ( auto i = 0u; i < 2 * 4 * 24; ++i )
                                    {
                                        backend.receive_clock_message( realtime_clock(), sender );
                                    }

                                    THEN( "for each pulse one message is received" )
                                    {
                                        REQUIRE( received_messages.size() == 2 * 4 * 24 );

                                        AND_THEN( "the first 2*24 messages have velocity 127" )
                                        {
                                            for ( auto i = 0u; i < 2 * 24; ++i )
                                            {
                                                CHECK( static_cast< std::uint8_t >(
                                                           received_messages[ i ][ 2 ] ) == 127 );
                                            }
                                        }

                                        AND_THEN( "the second 2*24 messages have velocity 0" )
                                        {
                                            const auto offset = 2u * 24u;
                                            for ( auto i = offset; i < offset + 2 * 24; ++i )
                                            {
                                                CHECK( static_cast< std::uint8_t >(
                                                           received_messages[ i ][ 2 ] ) == 0 );
                                            }
                                        }

                                        AND_THEN( "the third 2*24 messages have velocity 127" )
                                        {
                                            const auto offset = 4u * 24u;
                                            for ( auto i = offset; i < offset + 2 * 24; ++i )
                                            {
                                                CHECK( static_cast< std::uint8_t >(
                                                           received_messages[ i ][ 2 ] ) == 127 );
                                            }
                                        }

                                        AND_THEN( "the fourth 2*24 messages have velocity 0" )
                                        {
                                            const auto offset = 6u * 24u;
                                            for ( auto i = offset; i < offset + 2 * 24; ++i )
                                            {
                                                CHECK( static_cast< std::uint8_t >(
                                                           received_messages[ i ][ 2 ] ) == 0 );
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

SCENARIO( "lfo for velocity with triangular wave form", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;
    using namespace sequencer::midi::realtime;

    GIVEN( "a digitakt backend" )
    {
        auto backend =
            digitakt::backend_impl{digitakt::get_spec( "device_spec/elektron/digitakt.txt" )};
        REQUIRE( backend.mode() == digitakt::mode_t::play );

        WHEN( "lfo with triangular wave is set" )
        {
            backend.set_control_mode( digitakt::control_mode_t::lfo );
            backend.set_control( digitakt::track_parameter_t::lfo_idx::wave_form, 0,
                                 []( auto ) {} );

            AND_WHEN( "speed is set to 32" )
            {
                backend.set_control( digitakt::track_parameter_t::lfo_idx::speed, 32,
                                     []( auto ) {} );

                AND_WHEN( "phase is set to 0" )
                {
                    backend.set_control( digitakt::track_parameter_t::lfo_idx::phase, 0,
                                         []( auto ) {} );

                    AND_WHEN( "depth is set to 128" )
                    {
                        backend.set_control( digitakt::track_parameter_t::lfo_idx::depth, 128,
                                             []( auto ) {} );

                        AND_WHEN( "destination is set to velocity" )
                        {
                            backend.set_control( digitakt::track_parameter_t::lfo_idx::destination,
                                                 2, []( auto ) {} );

                            AND_WHEN( "multiplier is set to 2" )
                            {
                                backend.set_control(
                                    digitakt::track_parameter_t::lfo_idx::multiplier, 1,
                                    []( auto ) {} );

                                AND_WHEN( "2*4*24 clock messages are sent" )
                                {
                                    std::vector< midi::message_t< 3 > > received_messages;
                                    const auto sender = [&received_messages]( const auto& msg ) {
                                        received_messages.push_back( msg );
                                    };

                                    backend.receive_clock_message( realtime_start(), sender );
                                    for ( auto i = 0u; i < 2 * 4 * 24; ++i )
                                    {
                                        backend.receive_clock_message( realtime_clock(), sender );
                                    }

                                    THEN( "for each pulse one message is received" )
                                    {
                                        REQUIRE( received_messages.size() == 2 * 4 * 24 );

                                        AND_THEN( "the first messages has velocity 64" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 0 ][ 2 ] ) == 63 );
                                        }

                                        AND_THEN( "the 24th messages has velocity 96" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 24 ][ 2 ] ) == 95 );
                                        }

                                        AND_THEN( "the 48th messages has velocity 127" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 48 ][ 2 ] ) == 127 );
                                        }

                                        AND_THEN( "the 72th messages has velocity 95" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 72 ][ 2 ] ) == 95 );
                                        }

                                        AND_THEN( "the 96th messages has velocity 63" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 96 ][ 2 ] ) == 63 );
                                        }

                                        AND_THEN( "the 120th messages has velocity 31" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 120 ][ 2 ] ) == 31 );
                                        }

                                        AND_THEN( "the 144th messages has velocity 0" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 144 ][ 2 ] ) == 0 );
                                        }

                                        AND_THEN( "the 168th messages has velocity 32" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 168 ][ 2 ] ) == 31 );
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

SCENARIO( "lfo for velocity with sine wave form", "[digitakt]" )
{
    using namespace sequencer;
    using namespace sequencer::backend;
    using namespace sequencer::midi::realtime;

    GIVEN( "a digitakt backend" )
    {
        auto backend =
            digitakt::backend_impl{digitakt::get_spec( "device_spec/elektron/digitakt.txt" )};
        REQUIRE( backend.mode() == digitakt::mode_t::play );

        WHEN( "lfo with sine wave is set" )
        {
            backend.set_control_mode( digitakt::control_mode_t::lfo );
            backend.set_control( digitakt::track_parameter_t::lfo_idx::wave_form, 1,
                                 []( auto ) {} );

            AND_WHEN( "speed is set to 32" )
            {
                backend.set_control( digitakt::track_parameter_t::lfo_idx::speed, 32,
                                     []( auto ) {} );

                AND_WHEN( "phase is set to 0" )
                {
                    backend.set_control( digitakt::track_parameter_t::lfo_idx::phase, 0,
                                         []( auto ) {} );

                    AND_WHEN( "depth is set to 128" )
                    {
                        backend.set_control( digitakt::track_parameter_t::lfo_idx::depth, 128,
                                             []( auto ) {} );

                        AND_WHEN( "destination is set to velocity" )
                        {
                            backend.set_control( digitakt::track_parameter_t::lfo_idx::destination,
                                                 2, []( auto ) {} );

                            AND_WHEN( "multiplier is set to 2" )
                            {
                                backend.set_control(
                                    digitakt::track_parameter_t::lfo_idx::multiplier, 1,
                                    []( auto ) {} );

                                AND_WHEN( "2*4*24 clock messages are sent" )
                                {
                                    std::vector< midi::message_t< 3 > > received_messages;
                                    const auto sender = [&received_messages]( const auto& msg ) {
                                        received_messages.push_back( msg );
                                    };

                                    backend.receive_clock_message( realtime_start(), sender );
                                    for ( auto i = 0u; i < 2 * 4 * 24; ++i )
                                    {
                                        backend.receive_clock_message( realtime_clock(), sender );
                                    }

                                    THEN( "for each pulse one message is received" )
                                    {
                                        REQUIRE( received_messages.size() == 2 * 4 * 24 );

                                        AND_THEN( "the first messages has velocity 64" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 0 ][ 2 ] ) == 63 );
                                        }

                                        AND_THEN( "the 24th messages has velocity 96" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 24 ][ 2 ] ) == 108 );
                                        }

                                        AND_THEN( "the 48th messages has velocity 127" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 48 ][ 2 ] ) == 127 );
                                        }

                                        AND_THEN( "the 72th messages has velocity 95" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 72 ][ 2 ] ) == 108 );
                                        }

                                        AND_THEN( "the 96th messages has velocity 63" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 96 ][ 2 ] ) == 63 );
                                        }

                                        AND_THEN( "the 120th messages has velocity 31" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 120 ][ 2 ] ) == 18 );
                                        }

                                        AND_THEN( "the 144th messages has velocity 0" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 144 ][ 2 ] ) == 0 );
                                        }

                                        AND_THEN( "the 168th messages has velocity 32" )
                                        {
                                            CHECK( static_cast< std::uint8_t >(
                                                       received_messages[ 168 ][ 2 ] ) == 18 );
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
