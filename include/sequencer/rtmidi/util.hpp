#pragma once

#include <sequencer/chrono/chrono_adapter.hpp>
#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/midi/clock.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
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

    class message_sender
    {
    public:
        explicit message_sender( RtMidiOut& rtmidiout ) : rtmidiout_( rtmidiout )
        {
        }

        template < class Message >
        void operator()( const Message& msg ) const
        {
            rtmidiout_.sendMessage(
                static_cast< const unsigned char* >( static_cast< const void* >( msg.data() ) ),
                msg.size() );
        }

    private:
        RtMidiOut& rtmidiout_;
    };

    inline auto make_clock()
    {
        using underlying_clock_type =
            sequencer::chrono::clock_object_adapter< std::chrono::steady_clock >;
        using sequencer_clock_type = sequencer::chrono::sequencer_clock< underlying_clock_type >;

        return sequencer::midi::clock{sequencer_clock_type{underlying_clock_type{}}};
    }
} // namespace sequencer::rtmidi
