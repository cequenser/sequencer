#pragma once

#include <sequencer/chrono/chrono_adapter.hpp>
#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/midi/clock.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/midi/message/system_common.hpp>

#include <RtMidi.h>
#include <ios>
#include <iostream>
#include <memory>
#include <vector>

namespace sequencer::rtmidi
{
    inline void cout_callback( double time_delta, std::vector< unsigned char >* message,
                               void* /*userData*/ )
    {
        const auto number_of_bytes = message->size();
        for ( decltype( message->size() ) i = 0; i < number_of_bytes; ++i )
            std::cout << "Byte " << i << " = " << std::hex << static_cast< int >( message->at( i ) )
                      << ", ";
        if ( number_of_bytes > 0 )
            std::cout << "expired since last message = " << time_delta << "s" << std::endl;
    }

    template < class RtMidi >
    std::unique_ptr< RtMidi > make_midi_port( unsigned port_number = 0 )
    {
        auto rtmidi = std::make_unique< RtMidi >();
        if ( rtmidi->getPortCount() < port_number + 1u )
        {
            std::cout << "Requested port: " << port_number
                      << ". Available number of ports: " << rtmidi->getPortCount() << std::endl;
            return {};
        }
        std::cout << "Opening port " << rtmidi->getPortName( port_number ) << std::endl;
        rtmidi->openPort( port_number );
        return rtmidi;
    }

    inline void wait_for_press_enter( const std::string& message )
    {
        std::cout << message << std::endl;
        char input;
        std::cin.get( input );
    }

    class realtime_message_sender
    {
    public:
        explicit realtime_message_sender( RtMidiOut& rtmidiout ) : rtmidiout_( rtmidiout )
        {
        }

        void operator()( midi::realtime::message_type message )
        {
            const std::vector< unsigned char > messages = {static_cast< unsigned char >( message )};
            rtmidiout_.sendMessage( &messages );
        }

    private:
        RtMidiOut& rtmidiout_;
    };

    class message_sender
    {
    public:
        explicit message_sender( RtMidiOut& rtmidiout ) : rtmidiout_( rtmidiout )
        {
        }

        template < class Message >
        void operator()( const Message& msg )
        {
            rtmidiout_.sendMessage(
                static_cast< const unsigned char* >( static_cast< const void* >( msg.data() ) ),
                msg.size() );
        }

    private:
        RtMidiOut& rtmidiout_;
    };

    inline auto make_clock( RtMidiOut& midiout )
    {
        using underlying_clock_type =
            sequencer::chrono::clock_object_adapter< std::chrono::steady_clock >;
        using sequencer_clock_type = sequencer::chrono::sequencer_clock< underlying_clock_type >;

        sequencer_clock_type sequencer_clock{underlying_clock_type{}};
        auto sender = realtime_message_sender{midiout};
        return sequencer::midi::clock{std::move( sequencer_clock ), std::move( sender )};
    }

    template < class Sender >
    inline auto make_clock( RtMidiOut& midiout, Sender other_sender )
    {
        using underlying_clock_type =
            sequencer::chrono::clock_object_adapter< std::chrono::steady_clock >;
        using sequencer_clock_type = sequencer::chrono::sequencer_clock< underlying_clock_type >;

        sequencer_clock_type sequencer_clock{underlying_clock_type{}};
        auto sender = [&other_sender, &midiout]( midi::realtime::message_type message ) {
            const std::vector< unsigned char > messages = {static_cast< unsigned char >( message )};
            midiout.sendMessage( &messages );
            other_sender( message );
        };
        return sequencer::midi::clock{std::move( sequencer_clock ), std::move( sender )};
    }

} // namespace sequencer::rtmidi
