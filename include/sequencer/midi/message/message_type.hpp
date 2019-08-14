#pragma once

#include <sequencer/midi/message/util.hpp>

#include <cstddef>
#include <vector>

namespace sequencer::midi
{
    class message_type
    {
    public:
        using size_type = std::vector< std::byte >::size_type;

        message_type() = default;

        explicit message_type( const std::vector< std::byte >& message ) : message_( message )
        {
        }

        explicit message_type( std::vector< std::byte >&& message )
            : message_( std::move( message ) )
        {
        }

        bool empty() const noexcept
        {
            return message_.empty();
        }

        const std::byte& front() const
        {
            return message_.front();
        }

        size_type size() const noexcept
        {
            return message_.size();
        }

        std::byte& operator[]( size_type i )
        {
            return message_[ i ];
        }

        const std::byte& operator[]( size_type i ) const
        {
            return message_[ i ];
        }

        auto begin() const
        {
            return message_.begin();
        }

        auto end() const
        {
            return message_.end();
        }

        const std::vector< std::byte >& message() const noexcept
        {
            return message_;
        }

        const unsigned char* data() const
        {
            return static_cast< const unsigned char* >(
                static_cast< const void* >( message_.data() ) );
        }

        bool operator==( const message_type& other ) const noexcept
        {
            if ( size() != other.size() )
            {
                return false;
            }

            for ( auto i = 0u; i < size(); ++i )
            {
                if ( ( *this )[ i ] != other[ i ] )
                {
                    return false;
                }
            }

            return true;
        }

    private:
        std::vector< std::byte > message_;
    };

    template < class... Args >
    message_type make_message( Args&&... args )
    {
        return message_type{{std::forward< Args >( args )...}};
    }
} // namespace sequencer::midi
