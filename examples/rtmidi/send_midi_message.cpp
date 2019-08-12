#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/rtmidi/util.hpp>

#include <RtMidi.h>
#include <chrono>
#include <future>
#include <thread>
#include <vector>

using sequencer::rtmidi::cout_callback;
using sequencer::rtmidi::make_midi_port;

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
    std::promise< void > receiver_is_ready_promise;
    std::promise< void > sender_is_done;
    const auto receiver_is_ready = receiver_is_ready_promise.get_future();
    const auto receiver_is_done =
        std::async( std::launch::async, [&receiver_is_ready_promise, &sender_is_done] {
            receiver( receiver_is_ready_promise, sender_is_done.get_future() );
        } );
    receiver_is_ready.wait();

    auto midiout = make_midi_port< RtMidiOut >();
    if ( !midiout )
    {
        return 1;
    }

    const auto sender = [&midiout]( auto message ) {
        const std::vector< unsigned char > messages = {static_cast< unsigned char >( message )};
        midiout->sendMessage( &messages );
    };

    using sequencer::midi::real_time::message_type;
    sender( message_type::realtime_clock );
    sender( message_type::realtime_continue );
    sender( message_type::realtime_start );
    sender( message_type::realtime_stop );

    std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
    sender_is_done.set_value();
    return 0;
}
