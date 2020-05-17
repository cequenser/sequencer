#pragma once

#include <sequencer/assert.hpp>
#include <sequencer/util/type_erased_storage.hpp>

namespace sequencer
{
    template < class ReturnType >
    class callable_t
    {
        template < class Impl >
        static ReturnType call( const type_erased_storage_t& data )
        {
            return data.template get< Impl >()();
        }

    public:
        callable_t() noexcept = default;

        template < class T,
                   std::enable_if_t< !std::is_constructible< callable_t, T >::value >* = nullptr >
        // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
        callable_t( T&& value )
            : call_( &call< std::decay_t< T > > ), impl_( std::forward< T >( value ) )
        {
        }

        template < class T,
                   std::enable_if_t< !std::is_constructible< callable_t, T >::value >* = nullptr >
        callable_t& operator=( T&& value )
        {
            // NOLINTNEXTLINE(cppcoreguidelines-c-copy-assignment-signature)
            return *this = callable_t( std::forward< T >( value ) );
        }

        explicit operator bool() const noexcept
        {
            return bool( impl_ );
        }

        ReturnType operator()() const
        {
            SEQUENCER_ASSERT( call_ )
            SEQUENCER_ASSERT( impl_ )
            return call_( impl_ );
        }

    private:
        using call_t = ReturnType ( * )( const type_erased_storage_t& );
        call_t call_{nullptr};
        type_erased_storage_t impl_{};
    };
} // namespace sequencer
