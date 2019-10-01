#include <sequencer/backend/digitakt_parameter.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/midi/track.hpp>

#include <catch2/catch.hpp>

using sequencer::backend::digitakt::track_parameter_t;
using sequencer::midi::message_t;
using sequencer::midi::note_t;
using sequencer::midi::step_t;
using sequencer::midi::track_t;
using sequencer::midi::channel::voice::control_change;
using sequencer::midi::channel::voice::note_off;
using sequencer::midi::channel::voice::note_on;
using sequencer::midi::realtime::realtime_clock;
using sequencer::midi::realtime::realtime_start;

SCENARIO( "track_t lfo", "[track]" )
{
    constexpr auto number_of_steps = 16u;
    constexpr auto lfo_control_byte = 0x14;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        track.parameter().set_velocity( 100 );
        track.parameter().set_note_length_idx( 14 );
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::speed ] = 128;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::phase ] = 0;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::multiplier ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::destination ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::wave_form ] =
            track_parameter_t::wave_from_idx::square;

        WHEN( "squared lfo-filter with speed 128 for velocity is set" )
        {
            track.set_lfo( 0, 127, []( std::uint8_t value ) {
                return control_change( 0, lfo_control_byte, value );
            } );

            AND_WHEN( "a start and 96 clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                for ( auto i = 0; i < 96; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "96 messages are received" )
                {
                    REQUIRE( received_messages.size() == 96 );

                    AND_THEN( "the first half has velocity 127" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 0 ][ 2 ] ) == 127 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 47 ][ 2 ] ) == 127 );
                    }

                    AND_THEN( "the second half has velocity 0" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 48 ][ 2 ] ) == 0 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 95 ][ 2 ] ) == 0 );
                    }
                }
            }

            AND_WHEN( "speed is changed to -128" )
            {
                track.parameter()
                    .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::speed ] =
                    -128;

                AND_WHEN( "a start and 96 clock messages are send" )
                {
                    std::vector< message_t< 3 > > received_messages;
                    const auto sender = [&received_messages]( const auto& msg ) {
                        received_messages.push_back( msg );
                    };

                    track.send_messages( realtime_start(), sender );
                    for ( auto i = 0; i < 96; ++i )
                    {
                        track.send_messages( realtime_clock(), sender );
                    }

                    THEN( "96 messages are received" )
                    {
                        REQUIRE( received_messages.size() == 96 );

                        AND_THEN( "the first half has velocity 0" )
                        {
                            CHECK( static_cast< std::uint8_t >( received_messages[ 0 ][ 2 ] ) ==
                                   0 );
                            CHECK( static_cast< std::uint8_t >( received_messages[ 47 ][ 2 ] ) ==
                                   0 );
                        }

                        AND_THEN( "the second half has velocity 127" )
                        {
                            CHECK( static_cast< std::uint8_t >( received_messages[ 48 ][ 2 ] ) ==
                                   127 );
                            CHECK( static_cast< std::uint8_t >( received_messages[ 95 ][ 2 ] ) ==
                                   127 );
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "track_t lfo in trig mode", "[track]" )
{
    constexpr auto number_of_steps = 16u;
    constexpr auto lfo_control_byte = 0x14;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        track.parameter().set_velocity( 100 );
        track.parameter().set_note_length_idx( 14 );
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::speed ] = 128;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::phase ] = 0;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::multiplier ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::destination ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::wave_form ] =
            track_parameter_t::wave_from_idx::square;
        track.parameter().values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::mod ] =
            1;
        const auto first_note = note_t{1};
        const auto first_velocity = std::uint8_t{80};
        const auto first_step = step_t{first_note, first_velocity};
        track[ 12 ] = first_step;

        WHEN( "squared lfo-filter with speed 128 for velocity is set" )
        {
            track.set_lfo( 0, 127, []( std::uint8_t value ) {
                return control_change( 0, lfo_control_byte, value );
            } );

            AND_WHEN( "a start and 144 clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                for ( auto i = 0; i < 144; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "146 messages are received" )
                {
                    REQUIRE( received_messages.size() == 146 );

                    AND_THEN( "the first half has velocity 127" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 0 ][ 2 ] ) == 127 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 47 ][ 2 ] ) == 127 );
                    }

                    AND_THEN( "followed by 24 messages with velocity 0" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 48 ][ 2 ] ) == 0 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 71 ][ 2 ] ) == 0 );
                    }

                    AND_THEN( "followed by a note on messages" )
                    {
                        CHECK( received_messages[ 72 ] ==
                               note_on( 0, to_uint8_t( first_note ), first_velocity ) );
                    }

                    AND_THEN( "followed by 6 messages with velocity 127" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 73 ][ 2 ] ) == 127 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 78 ][ 2 ] ) == 127 );
                    }

                    AND_THEN( "followed by a note off messages" )
                    {
                        CHECK( received_messages[ 79 ] ==
                               note_off( 0, to_uint8_t( first_note ), 0 ) );
                    }

                    AND_THEN( "followed by 42 messages with velocity 127" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 80 ][ 2 ] ) == 127 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 121 ][ 2 ] ) ==
                               127 );
                    }

                    AND_THEN( "followed by 22 messages with velocity 0" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 122 ][ 2 ] ) == 0 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 143 ][ 2 ] ) == 0 );
                    }
                }
            }
        }
    }
}

SCENARIO( "track_t lfo in hold mode", "[track]" )
{
    constexpr auto number_of_steps = 16u;
    constexpr auto lfo_control_byte = 0x14;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        track.parameter().set_velocity( 100 );
        track.parameter().set_note_length_idx( 14 );
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::speed ] = 128;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::phase ] = 0;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::multiplier ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::destination ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::wave_form ] =
            track_parameter_t::wave_from_idx::square;
        track.parameter().values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::mod ] =
            2;
        const auto first_note = note_t{1};
        const auto first_velocity = std::uint8_t{80};
        const auto first_step = step_t{first_note, first_velocity};
        track[ 6 ] = first_step;
        track[ 12 ] = first_step;

        WHEN( "squared lfo-filter with speed 128 for velocity is set" )
        {
            track.set_lfo( 0, 127, []( std::uint8_t value ) {
                return control_change( 0, lfo_control_byte, value );
            } );

            AND_WHEN( "a start and 96 clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                for ( auto i = 0; i < 96; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "7 messages are received" )
                {
                    REQUIRE( received_messages.size() == 6 );

                    AND_THEN( "note on" )
                    {
                        CHECK( received_messages[ 0 ] ==
                               note_on( 0, to_uint8_t( first_note ), first_velocity ) );
                    }

                    AND_THEN( "control change for velocity 217" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 1 ][ 2 ] ) == 127 );
                    }

                    AND_THEN( "note off" )
                    {
                        CHECK( received_messages[ 2 ] ==
                               note_off( 0, to_uint8_t( first_note ), 0 ) );
                    }

                    AND_THEN( "note on" )
                    {
                        CHECK( received_messages[ 3 ] ==
                               note_on( 0, to_uint8_t( first_note ), first_velocity ) );
                    }

                    AND_THEN( "control change for velocity 0" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 4 ][ 2 ] ) == 0 );
                    }

                    AND_THEN( "note off" )
                    {
                        CHECK( received_messages[ 5 ] ==
                               note_off( 0, to_uint8_t( first_note ), 0 ) );
                    }
                }
            }
        }

        WHEN( "triangular lfo-filter with speed 128 for velocity is set" )
        {
            track.parameter()
                .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::wave_form ] =
                track_parameter_t::wave_from_idx::triangular;
            track.set_lfo( 0, 127, []( std::uint8_t value ) {
                return control_change( 0, lfo_control_byte, value );
            } );

            AND_WHEN( "a start and 96 clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                for ( auto i = 0; i < 96; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "one message is received" )
                {
                    REQUIRE( received_messages.size() == 6 );

                    AND_THEN( "note on" )
                    {
                        CHECK( received_messages[ 0 ] ==
                               note_on( 0, to_uint8_t( first_note ), first_velocity ) );
                    }

                    AND_THEN( "control change for velocity 217" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 1 ][ 2 ] ) == 95 );
                    }

                    AND_THEN( "note off" )
                    {
                        CHECK( received_messages[ 2 ] ==
                               note_off( 0, to_uint8_t( first_note ), 0 ) );
                    }

                    AND_THEN( "note on" )
                    {
                        CHECK( received_messages[ 3 ] ==
                               note_on( 0, to_uint8_t( first_note ), first_velocity ) );
                    }

                    AND_THEN( "control change for velocity 0" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 4 ][ 2 ] ) == 0 );
                    }

                    AND_THEN( "note off" )
                    {
                        CHECK( received_messages[ 5 ] ==
                               note_off( 0, to_uint8_t( first_note ), 0 ) );
                    }
                }
            }
        }
    }
}

SCENARIO( "track_t lfo in one mode", "[track]" )
{
    constexpr auto number_of_steps = 16u;
    constexpr auto lfo_control_byte = 0x14;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        track.parameter().set_velocity( 100 );
        track.parameter().set_note_length_idx( 14 );
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::speed ] = 128;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::phase ] = 0;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::multiplier ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::destination ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::wave_form ] =
            track_parameter_t::wave_from_idx::square;
        track.parameter().values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::mod ] =
            3;

        WHEN( "squared lfo-filter with speed 128 for velocity is set" )
        {
            track.set_lfo( 0, 127, []( std::uint8_t value ) {
                return control_change( 0, lfo_control_byte, value );
            } );

            AND_WHEN( "a start and 144 clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                for ( auto i = 0; i < 144; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "97 messages are received" )
                {
                    REQUIRE( received_messages.size() == 97 );

                    AND_THEN( "48 messages with velocity 127" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 0 ][ 2 ] ) == 127 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 47 ][ 2 ] ) == 127 );
                    }

                    AND_THEN( "48 messages with velocity 0" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 48 ][ 2 ] ) == 0 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 95 ][ 2 ] ) == 0 );
                    }

                    AND_THEN( "one message with velocity 64" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 96 ][ 2 ] ) == 64 );
                    }
                }
            }
        }
    }
}

SCENARIO( "track_t lfo in half mode", "[track]" )
{
    constexpr auto number_of_steps = 16u;
    constexpr auto lfo_control_byte = 0x14;

    GIVEN( "track_t with 16 steps" )
    {
        auto track = track_t< track_parameter_t >{number_of_steps};
        track.parameter().set_velocity( 100 );
        track.parameter().set_note_length_idx( 14 );
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::speed ] = 128;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::phase ] = 0;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::multiplier ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::destination ] = 1;
        track.parameter()
            .values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::wave_form ] =
            track_parameter_t::wave_from_idx::square;
        track.parameter().values[ track_parameter_t::idx::lfo ][ track_parameter_t::lfo_idx::mod ] =
            4;

        WHEN( "squared lfo-filter with speed 128 for velocity is set" )
        {
            track.set_lfo( 0, 127, []( std::uint8_t value ) {
                return control_change( 0, lfo_control_byte, value );
            } );

            AND_WHEN( "a start and 144 clock messages are send" )
            {
                std::vector< message_t< 3 > > received_messages;
                const auto sender = [&received_messages]( const auto& msg ) {
                    received_messages.push_back( msg );
                };

                track.send_messages( realtime_start(), sender );
                for ( auto i = 0; i < 144; ++i )
                {
                    track.send_messages( realtime_clock(), sender );
                }

                THEN( "49 messages are received" )
                {
                    REQUIRE( received_messages.size() == 49 );

                    AND_THEN( "24 messages with velocity 127" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 0 ][ 2 ] ) == 127 );
                        CHECK( static_cast< std::uint8_t >( received_messages[ 47 ][ 2 ] ) == 127 );
                    }

                    AND_THEN( "one message with velocity 64" )
                    {
                        CHECK( static_cast< std::uint8_t >( received_messages[ 48 ][ 2 ] ) == 64 );
                    }
                }
            }
        }
    }
}
