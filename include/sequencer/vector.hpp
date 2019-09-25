#pragma once

#include <vector>

namespace sequencer
{
    template < class T >
    class vector
    {
    public:
        using vec_t = std::vector< T >;
        using size_type = typename vec_t::size_type;
        using value_type = typename vec_t::value_type;

        vector( size_type max_size, size_type initial_size, T default_value = T{} )
            : vec_( max_size, default_value ), size_{initial_size}
        {
        }

        explicit vector( size_type max_size, T default_value = T{} )
            : vector{max_size, max_size, default_value}
        {
        }

        bool resize( size_type size ) noexcept
        {
            if ( size > max_size() )
            {
                return false;
            }

            size_ = size;
        }

        size_type size() const noexcept
        {
            return size_;
        }

        constexpr size_type max_size() const noexcept
        {
            return vec_.size();
        }

        auto& operator[]( size_type i ) noexcept
        {
            return vec_[ i ];
        }

        const auto& operator[]( size_type i ) const noexcept
        {
            return vec_[ i ];
        }

        auto begin() noexcept
        {
            return vec_.begin();
        }

        auto begin() const noexcept
        {
            return vec_.begin();
        }

        auto end() noexcept
        {
            return begin() + size();
        }

        auto end() const noexcept
        {
            return begin() + size();
        }

    private:
        vec_t vec_;
        size_type size_{0};
    };
} // namespace sequencer
