#pragma once

#include <cassert>
#include <cmath>
#include <complex>
#include <cstring>
#include <vector>

namespace sequencer::audio
{
    template < class T >
    constexpr T minus_two_pi = T( -2 * M_PI );

    template < class T >
    class vector_view
    {
    public:
        using size_type = typename std::vector< T >::size_type;
        using value_type = T;

        constexpr explicit vector_view( const std::vector< T >* v, size_type offset = 0 ) noexcept
            : v_( v ), offset_( offset )
        {
        }

        constexpr explicit vector_view( const vector_view< T >* view,
                                        size_type offset = 0 ) noexcept
            : view_( view ), offset_( offset )
        {
        }

        T& operator[]( size_type i )
        {
            return ( v_ ) ? ( *v_ )[ offset_ + 2 * i ] : ( *view_ )[ offset_ + 2 * i ];
        }

        const T& operator[]( size_type i ) const
        {
            return ( v_ ) ? ( *v_ )[ offset_ + 2 * i ] : ( *view_ )[ offset_ + 2 * i ];
        }

        size_type size() const noexcept
        {
            return v_ ? v_->size() / 2 : view_->size() / 2;
        }

    private:
        const std::vector< T >* v_{nullptr};
        const vector_view< T >* view_{nullptr};
        const size_type offset_{0};
    };

    template < class Container >
    auto radix2( const Container& x )
    {
        using value_type = typename Container::value_type::value_type;
        if ( x.size() == 1 )
        {
            return std::vector{x[ 0 ]};
        }

        auto y = radix2( vector_view{&x, 0} );
        auto z = radix2( vector_view{&x, 1} );

        y.resize( x.size() );
        using namespace std::complex_literals;
        for ( typename Container::size_type k = 0; k < x.size() / 2; ++k )
        {
            const auto dy =
                std::exp( minus_two_pi< value_type > * 1i * ( value_type( k ) / x.size() ) ) *
                z[ k ];
            y[ x.size() / 2 + k ] = y[ k ] - dy;
            y[ k ] += dy;
        }

        return y;
    }

    template < class Container >
    auto inverse_radix2( Container x )
    {
        for ( auto& v : x )
        {
            v = conj( v );
        }

        auto y = radix2( x );
        for ( auto& v : y )
        {
            using value_type = typename Container::value_type::value_type;
            v = conj( v ) / value_type( y.size() );
        }

        return y;
    }

    template < class T >
    std::vector< std::complex< T > > to_complex_signal( const std::vector< T >& v )
    {
        std::vector< std::complex< T > > signal( v.size() / 2 );
        std::memcpy( signal.data(), v.data(), v.size() * sizeof( T ) );
        return signal;
    }

    template < class T >
    std::vector< std::complex< T > > fft( const std::vector< T >& x )
    {
        using namespace std::complex_literals;
        assert( x.size() % 2 == 0 );
        const auto half_result = radix2( to_complex_signal( x ) );

        std::vector< std::complex< T > > result( half_result.size() );
        const auto scale = T{0.5};
        for ( typename std::vector< std::complex< T > >::size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] =
                scale * ( ( half_result[ k ] + z ) -
                          1i * ( half_result[ k ] - z ) *
                              std::exp( minus_two_pi< T > * 1i * ( T( k ) / x.size() ) ) );
        }

        return result;
    }

    template < class T >
    std::vector< T > fft_abs_scaled( const std::vector< T >& x )
    {
        using namespace std::complex_literals;
        assert( x.size() % 2 == 0 );
        const auto half_result = radix2( to_complex_signal( x ) );

        std::vector< T > result( half_result.size() );
        const auto scale = T{0.5 / result.size()};
        for ( typename std::vector< T >::size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] =
                scale * abs( ( half_result[ k ] + z ) -
                             1i * ( half_result[ k ] - z ) *
                                 std::exp( minus_two_pi< T > * 1i * ( T( k ) / x.size() ) ) );
        }

        return result;
    }
} // namespace sequencer::audio
