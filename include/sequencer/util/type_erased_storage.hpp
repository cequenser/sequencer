#pragma once

#include <sequencer/assert.hpp>

#include <cassert>
#include <type_traits>
#include <utility>

namespace sequencer
{
    class type_erased_storage_t final
    {
        template < class T >
        static void delete_data( void* data ) noexcept
        {
            SEQUENCER_ASSERT( data )
            delete static_cast< T* >( data );
        }

        template < class T >
        static void* copy_data( void* data )
        {
            return data ? new T( *static_cast< T* >( data ) ) : nullptr;
        }

    public:
        type_erased_storage_t() noexcept = default;

        template < class T, std::enable_if_t< !std::is_constructible<
                                type_erased_storage_t, std::decay_t< T > >::value >* = nullptr >
        // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
        explicit type_erased_storage_t( T&& value )
            : del_( &delete_data< std::decay_t< T > > ),
              copy_data_( &copy_data< std::decay_t< T > > ),
              data_( new std::decay_t< T >( std::forward< T >( value ) ) )
        {
        }

        template < class T, std::enable_if_t< !std::is_constructible<
                                type_erased_storage_t, std::decay_t< T > >::value >* = nullptr >
        type_erased_storage_t& operator=( T&& value )
        {
            // NOLINTNEXTLINE(cppcoreguidelines-c-copy-assignment-signature)
            return *this = Storage( std::forward< T >( value ) );
        }

        ~type_erased_storage_t()
        {
            reset();
        }

        type_erased_storage_t( const type_erased_storage_t& other )
            : del_( other.del_ ), copy_data_( other.copy_data_ ),
              data_( other.data_ == nullptr ? nullptr : other.copy() )
        {
        }

        type_erased_storage_t( type_erased_storage_t&& other ) noexcept
            : del_( other.del_ ), copy_data_( other.copy_data_ ), data_( other.data_ )
        {
            other.data_ = nullptr;
        }

        type_erased_storage_t& operator=( const type_erased_storage_t& other )
        {
            reset();
            del_ = other.del_;
            copy_data_ = other.copy_data_;
            data_ = ( other.data_ == nullptr ? nullptr : other.copy() );
            return *this;
        }

        type_erased_storage_t& operator=( type_erased_storage_t&& other ) noexcept
        {
            reset();
            del_ = other.del_;
            copy_data_ = other.copy_data_;
            data_ = other.data_;
            other.data_ = nullptr;
            return *this;
        }

        template < class T >
        T& get() noexcept
        {
            SEQUENCER_ASSERT( data_ )
            return *static_cast< T* >( data_ );
        }

        template < class T >
        const T& get() const noexcept
        {
            SEQUENCER_ASSERT( data_ )
            return *static_cast< const T* >( data_ );
        }

        explicit operator bool() const noexcept
        {
            return data_ != nullptr;
        }

    private:
        void reset() noexcept
        {
            if ( data_ != nullptr )
            {
                del_( data_ );
            }
        }

        void* copy() const
        {
            SEQUENCER_ASSERT( data_ )
            return copy_data_( data_ );
        }

        using delete_fn = void ( * )( void* );
        using copy_fn = void* (*)( void* );
        delete_fn del_ = nullptr;
        copy_fn copy_data_ = nullptr;
        void* data_ = nullptr;
    };
} // namespace sequencer
