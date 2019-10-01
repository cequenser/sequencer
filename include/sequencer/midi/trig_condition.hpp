#pragma once

#include <cassert>
#include <chrono>
#include <random>
#include <type_traits>
#include <utility>

namespace sequencer::midi
{
    class type_erased_storage_t
    {
        template < class T >
        static void delete_data( void* data ) noexcept
        {
            assert( data );
            delete static_cast< T* >( data );
        }

        template < class T >
        static void* copy_data( void* data )
        {
            return data ? new T( *static_cast< T* >( data ) ) : nullptr;
        }

    public:
        type_erased_storage_t() noexcept = default;

        template < class T, std::enable_if_t< !std::is_base_of<
                                type_erased_storage_t, std::decay_t< T > >::value >* = nullptr >
        explicit type_erased_storage_t( T&& value )
            : del_( &delete_data< std::decay_t< T > > ),
              copy_data_( &copy_data< std::decay_t< T > > ),
              data_( new std::decay_t< T >( std::forward< T >( value ) ) )
        {
        }

        template < class T, std::enable_if_t< !std::is_base_of<
                                type_erased_storage_t, std::decay_t< T > >::value >* = nullptr >
        type_erased_storage_t& operator=( T&& value )
        {
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
            assert( data_ );
            return *static_cast< T* >( data_ );
        }

        template < class T >
        const T& get() const noexcept
        {
            assert( data_ );
            return *static_cast< const T* >( data_ );
        }

        explicit operator bool() const noexcept
        {
            return data_ != nullptr;
        }

    private:
        void reset() noexcept
        {
            if ( data_ )
            {
                del_( data_ );
            }
        }

        void* copy() const
        {
            assert( data_ );
            return copy_data_( data_ );
        }

        using delete_fn = void ( * )( void* );
        using copy_fn = void* (*)( void* );
        delete_fn del_ = nullptr;
        copy_fn copy_data_ = nullptr;
        void* data_ = nullptr;
    };

    class trig_condition_t
    {
        template < class Impl >
        static bool call( const type_erased_storage_t& data )
        {
            return data.template get< Impl >()();
        }

    public:
        trig_condition_t() noexcept = default;

        template < class T, std::enable_if_t<
                                !std::is_constructible< trig_condition_t, T >::value >* = nullptr >
        trig_condition_t( T&& value )
            : call_( &call< std::decay_t< T > > ), impl_( std::forward< T >( value ) )
        {
        }

        template < class T, std::enable_if_t<
                                !std::is_constructible< trig_condition_t, T >::value >* = nullptr >
        trig_condition_t& operator=( T&& value )
        {
            return *this = trig_condition_t( std::forward< T >( value ) );
        }

        explicit operator bool() const noexcept
        {
            return bool( impl_ );
        }

        bool operator()() const
        {
            assert( call_ );
            assert( impl_ );
            return call_( impl_ );
        }

    private:
        using call_t = bool ( * )( const type_erased_storage_t& );
        call_t call_{nullptr};
        type_erased_storage_t impl_{};
    };

    namespace trig_condition
    {
        template < int trig, int size >
        class deterministic
        {
        public:
            bool operator()() const noexcept
            {
                const auto result = ( current_++ == trig );
                current_ %= size;
                return result;
            }

        private:
            mutable int current_{0};
        };

        class random
        {
        public:
            explicit random( int percent ) : percent_( percent )
            {
            }

            bool operator()() const
            {
                using clock = std::chrono::system_clock;

                static auto seed =
                    clock::from_time_t( std::time( nullptr ) ).time_since_epoch().count();
                static std::minstd_rand engine( seed );
                static std::uniform_int_distribution< int > distribution{};

                return distribution( engine, std::uniform_int_distribution< int >::param_type{
                                                 0, 99} ) < percent_;
            }

        private:
            int percent_{0};
        };
    } // namespace trig_condition
} // namespace sequencer::midi
