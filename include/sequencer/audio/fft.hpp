#pragma once

#include <sequencer/type_traits.hpp>
#include <sequencer/vector_view.hpp>

#include <cassert>
#include <cmath>
#include <complex>
#include <cstring>
#include <vector>

namespace sequencer::audio
{
    template < class Container >
    auto radix2( const Container& x )
    {
        if ( x.size() == 1 )
        {
            return std::vector{x[ 0 ]};
        }

        auto y = radix2( const_vector_view{&x, 0, 2} );
        const auto z = radix2( const_vector_view{&x, 1, 2} );

        static constexpr auto i = typename Container::value_type{0, 1};
        y.resize( x.size() );
        for ( typename Container::size_type k = 0; k < x.size() / 2; ++k )
        {
            using value_type = typename Container::value_type::value_type;
            const auto dy =
                std::exp( minus_two_pi< value_type > * i * ( value_type( k ) / x.size() ) ) *
                z[ k ];
            y[ x.size() / 2 + k ] = y[ k ] - dy;
            y[ k ] += dy;
        }

        return y;
    }

    template < class Container, class Result >
    void radix2( const Container& x, Result& result )
    {
        if ( x.size() == 1 )
        {
            result[ 0 ] = x[ 0 ];
            return;
        }

        auto even_result_view = vector_view{&result, 0};
        auto odd_result_view = vector_view{&result, result.size() / 2};
        radix2( const_vector_view{&x, 0, 2}, even_result_view );
        radix2( const_vector_view{&x, 1, 2}, odd_result_view );

        static constexpr auto i = typename Container::value_type{0, 1};
        for ( typename Container::size_type k = 0; k < x.size() / 2; ++k )
        {
            using value_type = typename Container::value_type::value_type;
            const auto dy =
                std::exp( minus_two_pi< value_type > * i * ( value_type( k ) / x.size() ) ) *
                result[ result.size() / 2 + k ];
            result[ x.size() / 2 + k ] = result[ k ] - dy;
            result[ k ] += dy;
        }
    }

    template < class Container >
    auto inverse_radix2( Container x )
    {
        for ( auto& v : x )
        {
            v = conj( v );
        }

        auto y = radix2( x );

        using value_type = typename Container::value_type::value_type;
        const auto scale = value_type( 1.0 ) / y.size();
        for ( auto& v : y )
        {
            v = scale * conj( v );
        }

        return y;
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    std::vector< std::complex< T > > fft( const std::vector< T >& x )
    {
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        const auto half_result =
            radix2( reinterpret_cast< const std::vector< std::complex< T > >& >( x ) );

        static constexpr auto i = std::complex< T >{0, 1};
        std::vector< std::complex< T > > result( half_result.size() );
        for ( size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] = T( 0.5 ) * ( ( half_result[ k ] + z ) -
                                       i * ( half_result[ k ] - z ) *
                                           exp( minus_two_pi< T > * i * ( T( k ) / x.size() ) ) );
        }

        return result;
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    void fft( const std::vector< T >& x, std::vector< std::complex< T > >& result )
    {
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        static constexpr auto i = std::complex< T >{0, 1};
        std::vector< std::complex< T > > half_result( x.size() / 2 );
        radix2( reinterpret_cast< const std::vector< std::complex< T > >& >( x ), half_result );

        for ( size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] = T( 0.5 ) * ( ( half_result[ k ] + z ) -
                                       i * ( half_result[ k ] - z ) *
                                           exp( minus_two_pi< T > * i * ( T( k ) / x.size() ) ) );
        }
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    std::vector< T > fft_abs_scaled( const std::vector< T >& x )
    {
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        const auto half_result =
            radix2( reinterpret_cast< const std::vector< std::complex< T > >& >( x ) );

        static constexpr auto i = std::complex< T >{0, 1};
        std::vector< T > result( half_result.size() );
        const auto scale = T( 0.5 ) / result.size();
        for ( size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] = scale * abs( ( half_result[ k ] + z ) -
                                       i * ( half_result[ k ] - z ) *
                                           exp( minus_two_pi< T > * i * ( T( k ) / x.size() ) ) );
        }

        return result;
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    std::vector< T > inverse_fft( const std::vector< std::complex< T > >& x )
    {
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        static constexpr auto i = std::complex< T >{0, 1};
        std::vector< std::complex< T > > y( x.size() );
        for ( size_type k = 0; k < y.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : y.size() - k;
            const auto z = conj( x[ s_k ] );
            y[ s_k ] =
                T( 0.5 ) *
                conj( ( x[ k ] + z ) -
                      i * ( x[ k ] - z ) * exp( two_pi< T > * i * ( T( k ) / ( 2 * x.size() ) ) ) );
        }

        const auto complex_result = inverse_radix2( y );
        std::vector< T > result( 2 * complex_result.size() );
        std::memcpy( result.data(), complex_result.data(), result.size() * sizeof( T ) );
        return result;
    }
} // namespace sequencer::audio
