#include "util.hpp"

#include <sequencer/midi/clock.hpp>

#include <RtMidi.h>
#include <chrono>
#include <future>
#include <memory>
#include <vector>

template < class Sender >
using midi_clock = sequencer::midi::clock< Sender >;
using sequencer::midi::clock_controller;
using sequencer::midi::get_controller;
using sequencer::midi::message_type;
using sequencer::rtmidi::cout_callback;
using sequencer::rtmidi::make_unique;

int main()
{
    const auto midiout = make_unique< RtMidiOut >();
    if ( !midiout )
    {
        return 1;
    }

    const auto sender = [&midiout]( message_type message ) {
        const std::vector< unsigned char > messages = {static_cast< unsigned char >( message )};
        midiout->sendMessage( &messages );
    };

    clock_controller< decltype( sender ) > clock_controller;
    auto controller_ready_promise = std::make_shared< std::promise< void > >();
    const auto controller_ready = controller_ready_promise->get_future();
    const auto clock_done =
        std::async( std::launch::async, [&clock_controller, sender,
                                         controller_ready = std::move( controller_ready_promise )] {
            const auto clock = std::make_shared< midi_clock< decltype( sender ) > >( sender );
            clock_controller = get_controller( clock );
            controller_ready->set_value();
            clock->run();
        } );
    controller_ready.wait();

    auto midiin = make_unique< RtMidiIn >();
    if ( !midiin )
    {
        return 1;
    }
    midiin->setCallback( &cout_callback );
    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );

    clock_controller.start_clock();

    std::cout << "\nReading MIDI input ... press <Enter> to quit.\n";
    char input;
    std::cin.get( input );
    clock_controller.shut_down_clock();

    clock_done.wait();

    return 0;
}
