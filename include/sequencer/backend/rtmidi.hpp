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
            SEQUENCER_ASSERT( idx >= 0 )
            SEQUENCER_ASSERT( idx <= int( midiout_->getPortCount() ) + 1 )

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

    class rtmidi_receiver_t
    {
    public:
        template < class Callback >
        explicit rtmidi_receiver_t( Callback callback )
        {
            midiin_.setCallback( callback );
            // Don't ignore sysex, timing, or active sensing messages.
            midiin_.ignoreTypes( false, false, false );
        }

        void select_input_port( int idx )
        {
            SEQUENCER_ASSERT( idx >= 0 )
            SEQUENCER_ASSERT( idx <= int( midiin_.getPortCount() ) + 1 )

            if ( midiin_.isPortOpen() )
            {
                midiin_.closePort();
            }
            if ( idx == 0 )
            {
                return;
            }
            midiin_.openPort( unsigned( idx - 1 ) );
        }

        std::vector< std::string > available_input_ports()
        {
            std::vector< std::string > ports;
            for ( auto id = 0u; id < midiin_.getPortCount(); ++id )
            {
                ports.push_back( midiin_.getPortName( id ) );
            }
            return ports;
        }

    private:
        RtMidiIn midiin_{};
    };
} // namespace sequencer::backend
