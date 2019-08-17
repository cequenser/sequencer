#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/rtmidi/util.hpp>

#include <RtMidi.h>
#include <chrono>
#include <future>
#include <ios>
#include <thread>
#include <vector>

using sequencer::rtmidi::cout_callback;
using sequencer::rtmidi::make_midi_port;
using sequencer::rtmidi::message_sender;

void receiver( std::promise< void >& receiver_is_ready, std::future< void > sender_is_done )
{
    auto midiin = make_midi_port< RtMidiIn >();
    if ( !midiin )
    {
        return;
    }
    midiin->setCallback( &cout_callback );
    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );

    receiver_is_ready.set_value();
    sender_is_done.wait();
}

int main()
{
    //    std::promise< void > receiver_is_ready_promise;
    //    std::promise< void > sender_is_done;
    //    const auto receiver_is_ready = receiver_is_ready_promise.get_future();
    //    const auto receiver_is_done =
    //        std::async( std::launch::async, [&receiver_is_ready_promise, &sender_is_done] {
    //            receiver( receiver_is_ready_promise, sender_is_done.get_future() );
    //        } );
    //    receiver_is_ready.wait();

    auto midiout = make_midi_port< RtMidiOut >( 1 );
    if ( !midiout )
    {
        return 1;
    }

    const auto sender = message_sender{*midiout};

    char c;
    std::cin.get( c );

    using namespace sequencer::midi::realtime;
    sender( realtime_clock() );
    sender( realtime_continue() );
    sender( realtime_start() );
    sender( realtime_stop() );

    using namespace sequencer::midi::channel::voice;
    sender( note_on( 1, 40, 73 ) );
    sender( note_off( 1, 40, 73 ) );
    sender( polymorphic_key_pressure( 1, 47, 3 ) );
    sender( control_change( 1, 47, 42 ) );
    sender( program_change( 1, 73 ) );
    sender( channel_pressure( 1, 73 ) );
    sender( pitch_bend_change( 1, 3 ) );
    sender( effects_1_depth( 1, 17 ) );
    sender( effects_2_depth( 1, 17 ) );
    sender( effects_3_depth( 1, 17 ) );
    sender( effects_4_depth( 1, 17 ) );
    sender( effects_5_depth( 1, 17 ) );
    sender( all_sounds_off( 2 ) );
    sender( reset_all_controllers( 2 ) );
    sender( local_control( 3, true ) );
    sender( local_control( 3, false ) );
    sender( omni_mode_off( 4 ) );
    sender( omni_mode_on( 4 ) );
    sender( poly_mode_on( 4 ) );
    sender( damper_pedal( 5, true ) );
    sender( damper_pedal( 5, false ) );
    sender( portamento( 5, true ) );
    sender( portamento( 5, false ) );
    sender( sostenuto( 5, true ) );
    sender( sostenuto( 5, false ) );
    sender( soft_pedal( 5, true ) );
    sender( soft_pedal( 5, false ) );
    sender( hold_2( 5, true ) );
    sender( hold_2( 5, false ) );

    std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
    //    sender_is_done.set_value();
    return 0;
}
