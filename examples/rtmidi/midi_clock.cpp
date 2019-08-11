#include "util.hpp"

#include <sequencer/chrono/chrono_adapter.hpp>
#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/midi/clock.hpp>

#include <RtMidi.h>
#include <chrono>
#include <future>
#include <memory>
#include <vector>

using sequencer::beats_per_minute;
using sequencer::midi::message_type;
using sequencer::rtmidi::cout_callback;
using sequencer::rtmidi::make_midi_port;
using sequencer::rtmidi::wait_for_press_enter;

using underlying_clock_type = sequencer::chrono::clock_object_adapter< std::chrono::steady_clock >;
using sequencer_clock_type = sequencer::chrono::sequencer_clock< underlying_clock_type >;

auto make_clock( const std::unique_ptr< RtMidiOut >& midiout )
{
    auto sender = [&midiout]( message_type message ) {
        const std::vector< unsigned char > messages = {static_cast< unsigned char >( message )};
        midiout->sendMessage( &messages );
    };
    sequencer_clock_type sequencer_clock{underlying_clock_type{}};
    return sequencer::midi::clock{std::move( sequencer_clock ), std::move( sender )};
}

void log_file_callback( double time_delta, std::vector< unsigned char >* message,
                        void* /*userData*/ )
{
    static std::ofstream log_file( "midi_messages.log" );
    const auto number_of_bytes = message->size();
    for ( decltype( message->size() ) i = 0; i < number_of_bytes; ++i )
        log_file << "Byte " << i << " = " << std::hex << static_cast< int >( message->at( i ) )
                 << ", ";
    if ( number_of_bytes > 0 )
        log_file << "expired since last message = " << time_delta << "s" << std::endl;
}

int main()
{
    const auto midiout = make_midi_port< RtMidiOut >();
    if ( !midiout )
    {
        std::cout << "Failed to create MIDI out" << std::endl;
        return 1;
    }

    auto midiin = make_midi_port< RtMidiIn >();
    if ( !midiin )
    {
        std::cout << "Failed to create MIDI in" << std::endl;
        return 1;
    }
    midiin->setCallback( &log_file_callback );
    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );

    wait_for_press_enter( "Connect MIDI signals then press <Enter> to continue." );

    auto midi_clock = make_clock( midiout );

    std::promise< void > controller_ready_promise;
    const auto controller_ready = controller_ready_promise.get_future();
    const auto clock_done =
        std::async( std::launch::async, [&midi_clock, &controller_ready_promise] {
            controller_ready_promise.set_value();
            midi_clock.run();
            // wait a bit to make sure that the last messages where sent
            // TODO solve without sleep if possible
            std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
        } );
    controller_ready.wait();

    midi_clock.start();

    auto bpm = 120.0;
    while ( bpm > 0 )
    {
        std::cout << "Enter a tempo in bpm or -1 to quit." << std::endl;
        std::cin >> bpm;
        midi_clock.set_tempo( beats_per_minute{bpm} );
    }
    midi_clock.shut_down();
    clock_done.wait();

    return 0;
}
