#include <sequencer/chrono/chrono_adapter.hpp>
#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/midi/clock.hpp>
#include <sequencer/rtmidi/util.hpp>

#include <RtMidi.h>
#include <chrono>
#include <fstream>
#include <future>
#include <vector>

using sequencer::beats_per_minute;
using sequencer::midi::start_clock_in_thread;
using sequencer::midi::realtime::message_type;
using sequencer::rtmidi::cout_callback;
using sequencer::rtmidi::make_clock;
using sequencer::rtmidi::make_midi_port;
using sequencer::rtmidi::realtime_message_sender;
using sequencer::rtmidi::wait_for_press_enter;

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
    auto midiout = make_midi_port< RtMidiOut >();
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

    auto midi_clock = make_clock();
    const auto clock_done = start_clock_in_thread( midi_clock, realtime_message_sender{*midiout} );

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
