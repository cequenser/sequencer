#pragma once

#include <atomic>

namespace sequencer
{
    template < class T >
    class copyable_atomic
    {
    public:
        copyable_atomic() = default;



        constexpr explicit copyable_atomic( T value ) noexcept : value_{value}
        {
        }

        copyable_atomic& operator=( T value ) noexcept
        {
            value_ = value;
            return *this;
        }

        /// @warning not atomic
        copyable_atomic( const copyable_atomic& other ) : value_{other.load()}
        {
        }

        /// @warning not atomic
        copyable_atomic& operator=( const copyable_atomic& other )
        {
            value_ = other.load();
            return *this;
        }

        T load( std::memory_order order = std::memory_order_seq_cst ) const noexcept
        {
            return value_.load( order );
        }

        void store( T value, std::memory_order order = std::memory_order_seq_cst ) const noexcept
        {
            value_.store( value, order );
        }

        operator T() const noexcept
        {
            return load();
        }

    private:
        std::atomic< T > value_;
    };
} // namespace sequencer
