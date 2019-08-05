#pragma once

#include <sequencer/midi_clock.hpp>

#include <cstddef>
#include <utility>

namespace sequencer::midi
{
    template < class sender_impl >
    class sender
    {
    public:
        constexpr explicit sender( const sender_impl& sender ) : sender_( sender )
        {
        }

        constexpr explicit sender( sender_impl&& sender ) : sender_( std::move( sender ) )
        {
        }

        constexpr void operator()( message_type message ) const
        {
            sender_( static_cast< std::byte >( message ) );
        }

    private:
        sender_impl sender_;
    };
} // namespace sequencer::midi
