#pragma once

#include <sequencer/midi_clock.hpp>

#include <cstddef>

namespace sequencer
{
    namespace midi
    {
        template < class sender_impl >
        class sender
        {
        public:
            explicit sender( const sender_impl& sender ) : sender_( sender )
            {
            }

            void operator()( message_type message ) const
            {
                sender_( static_cast< std::byte >( message ) );
            }

        private:
            sender_impl sender_;
        };
    } // namespace midi
} // namespace sequencer
