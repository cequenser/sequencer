#pragma once

#include <sequencer/midi/message/invalid_string.hpp>

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

        auto size() const noexcept
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

    private:
        std::vector< std::byte > message_;
    };
} // namespace sequencer::midi
