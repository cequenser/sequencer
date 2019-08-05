#include <sequencer/midi_clock.hpp>

#include <RtMidi.h>
#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include <vector>

void cout_callback( double time_delta, std::vector< unsigned char >* message, void* /*userData*/ )
{
    const auto nBytes = message->size();
    for ( decltype( message->size() ) i = 0; i < nBytes; ++i )
        std::cout << "Byte " << i << " = " << std::hex << static_cast< int >( message->at( i ) )
                  << ", ";
    if ( nBytes > 0 )
        std::cout << "expired since last message = " << time_delta << "s" << std::endl;
}

template < class RtMidi >
std::unique_ptr< RtMidi > make_unique()
{
    auto rtmidi = std::make_unique< RtMidi >();
    if ( rtmidi->getPortCount() == 0 )
    {
        std::cout << "No ports available!\n";
        return nullptr;
    }
    rtmidi->openPort( 0 );
    return rtmidi;
}

void receiver( std::promise< void >& receiver_is_ready, std::future< void > sender_is_done )
{
    auto midiin = make_unique< RtMidiIn >();
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

    const auto midiout = make_unique< RtMidiOut >();
    if ( !midiout )
    {
        return 1;
    }

    const auto sender = [&midiout]( sequencer::midi::message_type message ) {
        const std::vector< unsigned char > messages = {static_cast< unsigned char >( message )};
        midiout->sendMessage( &messages );
    };

    sender( sequencer::midi::message_type::realtime_clock );
    sender( sequencer::midi::message_type::realtime_continue );
    sender( sequencer::midi::message_type::realtime_start );
    sender( sequencer::midi::message_type::realtime_stop );

    std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
    sender_is_done.set_value();
    return 0;
}
