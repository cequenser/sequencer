#pragma once

#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <vector>

namespace sequencer::rtmidi
{
    void cout_callback( double time_delta, std::vector< unsigned char >* message,
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
    std::unique_ptr< RtMidi > make_midi_port( int port_number = 0 )
    {
        auto rtmidi = std::make_unique< RtMidi >();
        if ( rtmidi->getPortCount() < port_number + 1 )
        {
            std::cout << "Requested port: " << port_number
                      << ". Available number of ports: " << rtmidi->getPortCount() << std::endl;
            return nullptr;
        }
        std::cout << "Opening port " << rtmidi->getPortName( port_number ) << std::endl;
        rtmidi->openPort( 0 );
        return rtmidi;
    }

    void wait_for_press_enter( const std::string& message )
    {
        std::cout << message << std::endl;
        char input;
        std::cin.get( input );
    }
} // namespace sequencer::rtmidi
