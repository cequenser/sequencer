#pragma once

#include <sequencer/midi/message/util.hpp>

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <vector>

namespace sequencer::midi
{
    class message_type
    {
    public:
        using container = std::vector< std::byte >;
        using size_type = container::size_type;

        message_type() = default;

        explicit message_type( const std::vector< std::byte >& message ) : message_( message )
        {
        }

        template <
            class... Args,
            std::enable_if_t< !std::is_constructible_v< message_type, Args... > >* = nullptr >
        explicit message_type( Args&&... args ) : message_{{std::forward< Args >( args )...}}
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

        const std::byte& back() const
        {
            return message_.back();
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

        const container& message() const noexcept
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

            for ( auto i = size_type{0}; i < size(); ++i )
            {
                if ( ( *this )[ i ] != other[ i ] )
                {
                    return false;
                }
            }

            return true;
        }

    private:
        container message_;
    };

    template < std::size_t n >
    message_type make_message( const std::array< std::byte, n >& message )
    {
        return std::make_from_tuple< message_type >( message );
    }
} // namespace sequencer::midi
