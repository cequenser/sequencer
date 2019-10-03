#include <sequencer/audio/oscillator.hpp>

#include <catch2/catch.hpp>

SCENARIO( "oscillator with sine wave form", "[oscillator]" )
{
    using namespace sequencer::audio;

    GIVEN( "an oscillator with sine wave form and frequency 1" )
    {
        auto osc = oscillator_t{};
        osc.set_frequency( 1 );
        osc.set_wave_form( oscillator_wave_form::sine );

        THEN( "value at 0 is 0" )
        {
            CHECK( osc( 0 ) == Approx( 0 ) );
        }

        THEN( "value at 0.25 is 1" )
        {
            CHECK( osc( 0.25 ) == Approx( 1 ) );
        }

        THEN( "value at 0.5 is 0" )
        {
            CHECK( std::abs( osc( 0.5 ) ) < 1e-15 );
        }

        THEN( "value at 0.75 is -1" )
        {
            CHECK( osc( 0.75 ) == Approx( -1 ) );
        }

        THEN( "value at 1 is 0" )
        {
            CHECK( std::abs( osc( 1 ) ) < 1e-15 );
        }
    }

    GIVEN( "an oscillator with sine wave form, frequency 1 and phase 0.5" )
    {
        auto osc = oscillator_t{};
        osc.set_frequency( 1 );
        osc.set_phase( 0.5 );
        osc.set_wave_form( oscillator_wave_form::sine );

        THEN( "value at 0 is 0" )
        {
            CHECK( std::abs( osc( 0 ) ) < 1e-15 );
        }

        THEN( "value at 0.25 is -1" )
        {
            CHECK( osc( 0.25 ) == Approx( -1 ) );
        }

        THEN( "value at 0.5 is 0" )
        {
            CHECK( std::abs( osc( 0.5 ) ) < 1e-15 );
        }

        THEN( "value at 0.75 is 1" )
        {
            CHECK( osc( 0.75 ) == Approx( 1 ) );
        }

        THEN( "value at 1 is 0" )
        {
            CHECK( std::abs( osc( 1 ) ) < 1e-15 );
        }
    }

    GIVEN( "an oscillator with sine wave form and frequency 2" )
    {
        auto osc = oscillator_t{};
        osc.set_frequency( 2 );
        osc.set_wave_form( oscillator_wave_form::sine );

        THEN( "value at 0 is 0" )
        {
            CHECK( osc( 0 ) == Approx( 0 ) );
        }

        THEN( "value at 0.25 is 0" )
        {
            CHECK( std::abs( osc( 0.25 ) ) < 1e-15 );
        }

        THEN( "value at 0.5 is 0" )
        {
            CHECK( std::abs( osc( 0.5 ) ) < 1e-15 );
        }

        THEN( "value at 0.75 is 0" )
        {
            CHECK( std::abs( osc( 0.75 ) ) < 1e-15 );
        }

        THEN( "value at 1 is 0" )
        {
            CHECK( std::abs( osc( 1 ) ) < 1e-15 );
        }
    }
}
