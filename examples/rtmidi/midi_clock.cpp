#include "util.hpp"

#include <sequencer/chrono/chrono_adapter.hpp>
#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/midi/clock.hpp>

#include <RtMidi.h>
#include <chrono>
#include <future>
#include <memory>
#include <vector>

using sequencer::midi::message_type;
using sequencer::rtmidi::cout_callback;
using sequencer::rtmidi::make_midi_port;
using sequencer::rtmidi::wait_for_press_enter;

using underlying_clock_type = sequencer::chrono::clock_object_adapter< std::chrono::steady_clock >;
using sequencer_clock_type = sequencer::chrono::sequencer_clock< underlying_clock_type >;

int main()
{
    const auto midiout = make_midi_port< RtMidiOut >();
    if ( !midiout )
    {
        std::cout << "Failed to create MIDI out" << std::endl;
        return 1;
    }

    wait_for_press_enter( "Connect MIDI signals then press <Enter> to continue." );

    const auto sender = [&midiout]( message_type message ) {
        const std::vector< unsigned char > messages = {static_cast< unsigned char >( message )};
        midiout->sendMessage( &messages );
    };
    sequencer_clock_type sequencer_clock{underlying_clock_type{}};
    auto midi_clock = sequencer::midi::clock{sequencer_clock, sender};

    std::promise< void > controller_ready_promise;
    const auto controller_ready = controller_ready_promise.get_future();
    const auto clock_done =
        std::async( std::launch::async, [&midi_clock, &controller_ready_promise] {
            controller_ready_promise.set_value();
            midi_clock.run();
        } );
    controller_ready.wait();

    auto midiin = make_midi_port< RtMidiIn >();
    if ( !midiin )
    {
        std::cout << "Failed to create MIDI in" << std::endl;
        return 1;
    }
    midiin->setCallback( &cout_callback );
    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );

    midi_clock.start();

    wait_for_press_enter( "Reading MIDI input ... press <Enter> to quit." );
    midi_clock.shut_down();

    clock_done.wait();

    return 0;
}
