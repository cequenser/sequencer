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
            backend.set_control( 4, 2, []( auto ) {} );

            AND_WHEN( "speed is set to 32" )
            {
                backend.set_control( 0, 23, []( auto ) {} );

                AND_WHEN( "multplier is set to 2" )
                {
                    backend.set_control( 1, 2, []( auto ) {} );

                    AND_WHEN( "phase is set to 0" )
                    {
                        backend.set_control( 5, 0, []( auto ) {} );

                        AND_WHEN( "depth is set to 128" )
                        {
                            backend.set_control( 7, 128, []( auto ) {} );

                            AND_WHEN( "destination is set to 1" )
                            {
                                backend.set_control( 3, 1, []( auto ) {} );

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

                                        //                                THEN( "the last 4 messages
                                        //                                are note on/off with
                                        //                                velocity 0" )
                                        //                                {
                                        //                                    REQUIRE(
                                        //                                    received_messages.size()
                                        //                                    == 16 ); CHECK(
                                        //                                    static_cast<
                                        //                                    std::uint8_t >(
                                        //                                    received_messages[ 8
                                        //                                    ][ 2 ] ) ==
                                        //                                            0 );
                                        //                                    CHECK( static_cast<
                                        //                                    std::uint8_t >(
                                        //                                    received_messages[ 10
                                        //                                    ][ 2 ] ) ==
                                        //                                            0 );
                                        //                                    CHECK( static_cast<
                                        //                                    std::uint8_t >(
                                        //                                    received_messages[ 12
                                        //                                    ][ 2 ] ) ==
                                        //                                            0 );
                                        //                                    CHECK( static_cast<
                                        //                                    std::uint8_t >(
                                        //                                    received_messages[ 14
                                        //                                    ][ 2 ] ) ==
                                        //                                            0 );
                                        //                                }
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
