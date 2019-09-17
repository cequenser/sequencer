#include <sequencer/backend/digitakt.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/realtime.hpp>

#include <catch2/catch.hpp>

// SCENARIO( "lfo for velocity with rectangular wave form", "[digitakt]" )
//{
//    using namespace sequencer;
//    using namespace sequencer::backend;
//    using namespace sequencer::midi::realtime;

//    GIVEN( "a digitakt backend" )
//    {
//        auto backend = digitakt{};
//        REQUIRE( backend.mode() == digitakt_mode::play );

//        AND_GIVEN("steps 1, 5, 9 and are set")
//        {
//            backend.set_step(1);
//            backend.set_step(5);
//            backend.set_step(9);
//            backend.set_step(13);

//            WHEN("lfo with square wave is set")
//            {
//                backend.set_control_mode(digitakt_control_mode::lfo);
//                backend.set_control(4, 2);

//                AND_WHEN("speed is set to 64")
//                {
//                    backend.set_control(0, 64);

//                    AND_WHEN("multplier is set to 1")
//                    {
//                        backend.set_control(1, 1);

//                        AND_WHEN("phase is set to 0")
//                        {
//                            backend.set_control(5, 0);

//                            AND_WHEN("depth is set to 128")
//                            {
//                                backend.set_control(7, 128);

//                                AND_WHEN( "2*4*24 clock messages are sent" )
//                                {
//                                    std::vector< midi::message_t< 3 > > received_messages;
//                                    const auto sender = [&received_messages]( const auto& msg ) {
//                                        received_messages.push_back( msg );
//                                    };

//                                    backend.receive_clock_message( realtime_start(), sender );
//                                    for ( auto i = 0u; i < 2*4*24; ++i )
//                                    {
//                                        backend.receive_clock_message( realtime_clock(), sender );
//                                    }

//                                    THEN( "the first 4 messages are note on/off with velocity 127"
//                                    )
//                                    {
//                                        REQUIRE( received_messages.size() == 15 );
//                                        CHECK( static_cast< std::uint8_t >(
//                                        received_messages.front()[ 0 ] ) ==
//                                               127 );
//                                        CHECK( static_cast< std::uint8_t >(
//                                        received_messages.front()[ 2 ] ) ==
//                                               127 );
//                                        CHECK( static_cast< std::uint8_t >(
//                                        received_messages.front()[ 4 ] ) ==
//                                               127 );
//                                        CHECK( static_cast< std::uint8_t >(
//                                        received_messages.front()[ 6 ] ) ==
//                                               127 );
//                                    }

//                                    THEN( "the last 4 messages are note on/off with velocity 0" )
//                                    {
//                                        REQUIRE( received_messages.size() == 15 );
//                                        CHECK( static_cast< std::uint8_t >(
//                                        received_messages.front()[ 8 ] ) ==
//                                               0 );
//                                        CHECK( static_cast< std::uint8_t >(
//                                        received_messages.front()[ 10 ] ) ==
//                                               0 );
//                                        CHECK( static_cast< std::uint8_t >(
//                                        received_messages.front()[ 12 ] ) ==
//                                               0 );
//                                        CHECK( static_cast< std::uint8_t >(
//                                        received_messages.front()[ 14 ] ) ==
//                                               0 );
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//}
