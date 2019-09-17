#pragma once

#include <sequencer/rtmidi/util.hpp>

#include <RtMidi.h>
#include <cassert>
#include <string>
#include <vector>

namespace sequencer::backend
{
    class rtmidi_t
    {
    public:
        void select_port( int idx )
        {
            assert( idx >= 0 );
            assert( idx <= int( midiout_.getPortCount() ) + 1 );

            if ( midiout_.isPortOpen() )
            {
                midiout_.closePort();
            }
            if ( idx == 0 )
            {
                return;
            }
            midiout_.openPort( unsigned( idx - 1 ) );
        }

        std::vector< std::string > available_ports()
        {
            std::vector< std::string > ports( midiout_.getPortCount() );
            for ( auto id = 0u; id < midiout_.getPortCount(); ++id )
            {
                ports.push_back( midiout_.getPortName( id ).c_str() );
            }
            return ports;
        }

        rtmidi::message_sender sender() noexcept
        {
            return rtmidi::message_sender{midiout_};
        }

    private:
        RtMidiOut midiout_;
    };
} // namespace sequencer::backend
