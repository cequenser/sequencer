#pragma once

#include <ios>
#include <iostream>
#include <memory>
#include <vector>

namespace sequencer::rtmidi
{
    void cout_callback( double time_delta, std::vector< unsigned char >* message,
                        void* /*userData*/ )
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
        std::cout << "Opening port " << rtmidi->getPortName( 0 ) << std::endl;
        rtmidi->openPort( 0 );
        return rtmidi;
    }
} // namespace sequencer::rtmidi
