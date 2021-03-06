#include <sequencer/backend/digitakt_parameter.hpp>
#include <sequencer/midi/device_spec.hpp>

#include <catch2/catch.hpp>

#include <sstream>

SCENARIO( "read device spec filter parameters for analog rytm", "[device_spec][analog_rytm]" )
{
    using namespace sequencer::midi;
    using namespace sequencer::backend;

    GIVEN( "a file containing the control function values for the filter parameters" )
    {
        auto file = std::ifstream( "device_spec/elektron/analog_rytm_mkii.txt" );
        REQUIRE( file.is_open() );

        WHEN( "stream is read into a device_spec_cc_t" )
        {
            const auto device_spec = digitakt::read_device_spec_cc( file );

            THEN( "the control change function for attack time is 70" )
            {
                auto entry =
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 0 ];
                CHECK( entry.name == "attack" );
                CHECK( entry.min == 0 );
                CHECK( entry.max == 127 );
                CHECK( entry.value == 0 );
                CHECK( entry.cc_msb == 70 );
            }

            THEN( "the control change function for decay time is 71" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 1 ]
                        .cc_msb == 71 );
            }

            THEN( "the control change function for sustain level is 72" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 2 ]
                        .cc_msb == 72 );
            }

            THEN( "the control change function for release time is 73" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 3 ]
                        .cc_msb == 73 );
            }

            THEN( "the control change function for frequency is 74" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 4 ]
                        .cc_msb == 74 );
            }

            THEN( "the control change function for resonance is 75" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 5 ]
                        .cc_msb == 75 );
            }

            THEN( "the control change function for modeis 76" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 6 ]
                        .cc_msb == 76 );
            }

            THEN( "the control change function for env depth is 77" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 7 ]
                        .cc_msb == 77 );
            }

            THEN( "the control change function for lfo depth msb is 109" )
            {
                CHECK( device_spec.control_parameter[ digitakt::track_parameter_t::idx::lfo ][ 7 ]
                           .cc_msb == 109 );
            }

            THEN( "the control change function for lfo depth lsb is 118" )
            {
                CHECK( device_spec.control_parameter[ digitakt::track_parameter_t::idx::lfo ][ 7 ]
                           .cc_lsb == 118 );
            }

            THEN( "the decimals for lfo depth is 2" )
            {
                CHECK( device_spec.control_parameter[ digitakt::track_parameter_t::idx::lfo ][ 7 ]
                           .decimals == 2 );
            }

            THEN( "the decimals for lfo speed is 0" )
            {
                CHECK( device_spec.control_parameter[ digitakt::track_parameter_t::idx::lfo ][ 0 ]
                           .decimals == 0 );
            }
        }
    }
}

SCENARIO( "read device spec filter parameters for digitakt", "[device_spec][digitakt]" )
{
    using namespace sequencer::midi;
    using namespace sequencer::backend;

    GIVEN( "a file containing the control function values for the filter parameters" )
    {
        auto file = std::ifstream( "device_spec/elektron/digitakt.txt" );
        REQUIRE( file.is_open() );

        WHEN( "stream is read into a device_spec_cc_t" )
        {
            const auto device_spec = digitakt::read_device_spec_cc( file );

            THEN( "the control change function for filter attack time is 70" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 0 ]
                        .cc_msb == 70 );
            }

            THEN( "the control change function for filter decay time is 71" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 1 ]
                        .cc_msb == 71 );
            }

            THEN( "the control change function for filter sustain level is 72" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 2 ]
                        .cc_msb == 72 );
            }

            THEN( "the control change function for filter release time is 73" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 3 ]
                        .cc_msb == 73 );
            }

            THEN( "the control change function for filter frequency is 74" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 4 ]
                        .cc_msb == 74 );
            }

            THEN( "the control change function for filter resonance is 75" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 5 ]
                        .cc_msb == 75 );
            }

            THEN( "the control change function for filter modeis 76" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 6 ]
                        .cc_msb == 76 );
            }

            THEN( "the control change function for filter env depth is 77" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::filter ][ 7 ]
                        .cc_msb == 77 );
            }

            THEN( "the control change function for source tune is 16" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::source ][ 0 ]
                        .cc_msb == 16 );
            }

            THEN( "the control change function for source play mode is 17" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::source ][ 1 ]
                        .cc_msb == 17 );
            }

            THEN( "the control change function for source bit reduction is 18" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::source ][ 2 ]
                        .cc_msb == 18 );
            }

            THEN( "the control change function for source sample select is 19" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::source ][ 3 ]
                        .cc_msb == 19 );
            }

            THEN( "the control change function for source start is 20" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::source ][ 4 ]
                        .cc_msb == 20 );
            }

            THEN( "the control change function for source length is 21" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::source ][ 5 ]
                        .cc_msb == 21 );
            }

            THEN( "the control change function for source loop position is 22" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::source ][ 6 ]
                        .cc_msb == 22 );
            }

            THEN( "the control change function for source level is 23" )
            {
                CHECK(
                    device_spec.control_parameter[ digitakt::track_parameter_t::idx::source ][ 7 ]
                        .cc_msb == 23 );
            }
        }
    }
}
