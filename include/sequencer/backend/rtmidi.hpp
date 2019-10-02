#pragma once

#include <sequencer/rtmidi/util.hpp>

#include <RtMidi.h>
#include <cassert>
#include <string>
#include <vector>

namespace sequencer::backend
{
    class rtmidi_sender_t
    {
    public:
        void select_output_port( int idx )
        {
            assert( idx >= 0 );
            assert( idx <= int( midiout_->getPortCount() ) + 1 );

            midiout_ = std::make_shared< RtMidiOut >();
            if ( idx == 0 )
            {
                return;
            }
            midiout_->openPort( unsigned( idx - 1 ) );
        }

        std::vector< std::string > available_output_ports()
        {
            std::vector< std::string > ports;
            for ( auto id = 0u; id < midiout_->getPortCount(); ++id )
            {
                ports.push_back( midiout_->getPortName( id ) );
            }
            return ports;
        }

        rtmidi::message_sender sender() noexcept
        {
            return rtmidi::message_sender{*midiout_};
        }

    private:
        std::shared_ptr< RtMidiOut > midiout_ = std::make_shared< RtMidiOut >();
    };
} // namespace sequencer::backend
