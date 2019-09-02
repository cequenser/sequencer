#pragma once

#include <cassert>
#include <cmath>
#include <complex>
#include <cstring>
#include <vector>

namespace sequencer::audio
{
    template < class T >
    constexpr T two_pi = T( 2 * M_PI );

    template < class T >
    constexpr T minus_two_pi = -two_pi< T >;

    template < class T >
    class vector_view
    {
    public:
        using size_type = typename std::vector< T >::size_type;

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
            return v_ ? ( *v_ )[ offset_ + 2 * i ] : ( *view_ )[ offset_ + 2 * i ];
        }

        const T& operator[]( size_type i ) const
        {
            return v_ ? ( *v_ )[ offset_ + 2 * i ] : ( *view_ )[ offset_ + 2 * i ];
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
        if ( x.size() == 1 )
        {
            return std::vector{x[ 0 ]};
        }

        auto y = radix2( vector_view{&x, 0} );
        auto z = radix2( vector_view{&x, 1} );

        y.resize( x.size() );
        for ( typename Container::size_type k = 0; k < x.size() / 2; ++k )
        {
            using namespace std::complex_literals;
            const auto dy =
                std::exp( minus_two_pi< double > * 1i * ( double( k ) / x.size() ) ) * z[ k ];
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
            v = conj( v ) / double( y.size() );
        }

        return y;
    }

    template < class T >
    struct is_complex_t : std::false_type
    {
    };

    template < class T >
    struct is_complex_t< std::complex< T > > : std::true_type
    {
    };

    template < class T >
    constexpr bool is_complex_v = is_complex_t< T >::value;

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    auto to_complex_signal( const std::vector< T >& v )
    {
        std::vector< std::complex< T > > signal( v.size() / 2 );
        std::memcpy( signal.data(), v.data(), v.size() * sizeof( T ) );
        return signal;
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    std::vector< std::complex< T > > fft( const std::vector< T >& x )
    {
        using namespace std::complex_literals;
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        const auto half_result = radix2( to_complex_signal( x ) );

        std::vector< std::complex< T > > result( half_result.size() );
        for ( size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] = T( 0.5 ) * ( ( half_result[ k ] + z ) -
                                       1i * ( half_result[ k ] - z ) *
                                           exp( minus_two_pi< T > * 1i * ( T( k ) / x.size() ) ) );
        }

        return result;
    }

    template < class T >
    std::vector< T > fft_abs_scaled( const std::vector< T >& x )
    {
        using namespace std::complex_literals;
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        const auto half_result = radix2( to_complex_signal( x ) );

        std::vector< T > result( half_result.size() );
        const auto scale = T{0.5 / result.size()};
        for ( size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] = scale * abs( ( half_result[ k ] + z ) -
                                       1i * ( half_result[ k ] - z ) *
                                           exp( minus_two_pi< T > * 1i * ( T( k ) / x.size() ) ) );
        }

        return result;
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    std::vector< T > inverse_fft( const std::vector< std::complex< T > >& x )
    {
        using namespace std::complex_literals;
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        std::vector< std::complex< T > > y( x.size() );
        for ( size_type k = 0; k < y.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : y.size() - k;
            const auto z = conj( x[ s_k ] );
            y[ s_k ] = T( 0.5 ) * conj( ( x[ k ] + z ) - 1i * ( x[ k ] - z ) *
                                                             exp( two_pi< T > * 1i *
                                                                  ( T( k ) / ( 2 * x.size() ) ) ) );
        }

        const auto complex_result = inverse_radix2( y );
        std::vector< T > result( 2 * complex_result.size() );
        std::memcpy( result.data(), complex_result.data(), result.size() * sizeof( T ) );
        return result;
    }
} // namespace sequencer::audio
