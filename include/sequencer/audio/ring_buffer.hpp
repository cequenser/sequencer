#pragma once

#include <vector>

namespace sequencer::audio
{
    template < class T >
    class ring_buffer_iterator_t
    {
    public:
        using size_type = typename std::vector< T >::size_type;
        using difference_type = std::size_t;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using iterator_category = std::forward_iterator_tag;

        explicit ring_buffer_iterator_t( T* t, size_type size, size_type idx = 0,
                                         size_type increments = 0 ) noexcept
            : t_{t}, idx_{idx}, size_{size}, increments_{increments}
        {
        }

        ring_buffer_iterator_t& operator++() noexcept
        {
            return *this += 1;
        }
        ring_buffer_iterator_t operator++( int ) noexcept
        {
            auto other = *this;
            ++( *this );
            return other;
        }

        ring_buffer_iterator_t operator+( size_type n ) noexcept
        {
            return ring_buffer_iterator_t{*this} += n;
        }

        ring_buffer_iterator_t& operator+=( size_type n ) noexcept
        {
            n %= size_;
            idx_ += n;
            increments_ += n;
            if ( idx_ >= size_ )
            {
                t_ -= ( size_ - n );
                idx_ %= size_;
            }
            else
            {
                t_ += n;
            }
            return *this;
        }

        reference operator*() const noexcept
        {
            return *t_;
        }

        bool operator==( const ring_buffer_iterator_t& other ) const noexcept
        {
            return ( idx_ == other.idx_ && size_ == other.size_ ) ||
                   ( increments_ >= size_ && other.increments_ >= other.size_ );
        }

        bool operator!=( const ring_buffer_iterator_t& other ) const noexcept
        {
            return !( ( *this ) == other );
        }

    private:
        pointer t_{nullptr};
        size_type idx_{0};
        size_type size_;
        size_type increments_{0};
    };

    template < class T >
    class ring_buffer_t
    {
    public:
        using size_type = typename std::vector< T >::size_type;

        explicit ring_buffer_t( size_type size ) : buffer_( size, T() )
        {
        }

        T& operator[]( size_type i ) noexcept
        {
            return buffer_[ ( offset_ + i ) % buffer_.size() ];
        }

        const T& operator[]( size_type i ) const noexcept
        {
            return buffer_[ ( offset_ + i ) % buffer_.size() ];
        }

        void move( size_type offset ) noexcept
        {
            if ( buffer_.empty() )
            {
                return;
            }
            offset_ += offset;
            offset_ %= buffer_.size();
        }

        void clean( size_type N ) noexcept
        {
            std::fill( begin(), begin() + N, 0 );
        }

        void clean_and_move( size_type N ) noexcept
        {
            clean( N );
            move( N );
        }

        auto begin() noexcept
        {
            return ring_buffer_iterator_t{&*( buffer_.begin() + offset_ ), buffer_.size(), offset_};
        }

        auto end() noexcept
        {
            return ring_buffer_iterator_t< float >{nullptr, buffer_.size(), buffer_.size(),
                                                   buffer_.size()};
        }

    private:
        std::vector< T > buffer_;
        size_type offset_{0};
    };
} // namespace sequencer::audio
