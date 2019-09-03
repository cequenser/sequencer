#pragma once

#include <sequencer/type_traits.hpp>
#include <sequencer/vector_view.hpp>

#include <array>
#include <cassert>
#include <cmath>
#include <complex>
#include <cstring>
#include <vector>

namespace sequencer::audio
{
    template < class T >
    constexpr std::complex< T > imag = std::complex< T >{0, 1};

    template < class T >
    auto create_table()
    {
        std::array< std::vector< std::complex< T > >, 16 > table;

        auto size = 2u;
        for ( auto size_exp = 0u; size_exp < table.size(); ++size_exp )
        {
            table[ size_exp ].resize( size );
            for ( auto k = 0u; k < table[ size_exp ].size(); ++k )
            {
                table[ size_exp ][ k ] =
                    std::exp( minus_two_pi< T > * imag< T > * ( T( k ) / size ) );
            }
            size *= 2;
        }

        return table;
    }

    template < class T >
    auto& gauss_table()
    {
        static auto table = create_table< T >();
        return table;
    }

    template < class T, class Size >
    std::complex< T > compute_gauss( Size k, Size size )
    {
        switch ( size )
        {
        case 2:
            return gauss_table< T >()[ 0 ][ k ];
        case 4:
            return gauss_table< T >()[ 1 ][ k ];
        case 8:
            return gauss_table< T >()[ 2 ][ k ];
        case 16:
            return gauss_table< T >()[ 3 ][ k ];
        case 32:
            return gauss_table< T >()[ 4 ][ k ];
        case 64:
            return gauss_table< T >()[ 5 ][ k ];
        case 128:
            return gauss_table< T >()[ 6 ][ k ];
        case 256:
            return gauss_table< T >()[ 7 ][ k ];
        case 512:
            return gauss_table< T >()[ 8 ][ k ];
        case 1024:
            return gauss_table< T >()[ 9 ][ k ];
        case 2048:
            return gauss_table< T >()[ 10 ][ k ];
        case 4096:
            return gauss_table< T >()[ 11 ][ k ];
        case 8192:
            return gauss_table< T >()[ 12 ][ k ];
        case 16384:
            return gauss_table< T >()[ 13 ][ k ];
        case 32768:
            return gauss_table< T >()[ 14 ][ k ];
        case 65536:
            return gauss_table< T >()[ 15 ][ k ];
        }

        return std::exp( minus_two_pi< T > * imag< T > * ( T( k ) / size ) );
    }

    template < class Container >
    auto radix2( const Container& x )
    {
        if ( x.size() == 1 )
        {
            return std::vector{x[ 0 ]};
        }

        auto y = radix2( const_vector_view{&x, 0, 2} );
        const auto z = radix2( const_vector_view{&x, 1, 2} );

        y.resize( x.size() );
        for ( typename Container::size_type k = 0; k < x.size() / 2; ++k )
        {
            using value_type = typename Container::value_type::value_type;
            const auto dy = compute_gauss< value_type >( k, x.size() ) * z[ k ];
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

        for ( typename Container::size_type k = 0; k < x.size() / 2; ++k )
        {
            using value_type = typename Container::value_type::value_type;
            const auto dy =
                compute_gauss< value_type >( k, x.size() ) * result[ result.size() / 2 + k ];
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

        std::vector< std::complex< T > > result( half_result.size() );
        for ( size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] =
                T( 0.5 ) * ( ( half_result[ k ] + z ) - imag< T > * ( half_result[ k ] - z ) *
                                                            compute_gauss< T >( k, x.size() ) );
        }

        return result;
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    void fft( const std::vector< T >& x, std::vector< std::complex< T > >& result )
    {
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        std::vector< std::complex< T > > half_result( x.size() / 2 );
        radix2( reinterpret_cast< const std::vector< std::complex< T > >& >( x ), half_result );

        for ( size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] =
                T( 0.5 ) * ( ( half_result[ k ] + z ) - imag< T > * ( half_result[ k ] - z ) *
                                                            compute_gauss< T >( k, x.size() ) );
        }
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    std::vector< T > fft_abs_scaled( const std::vector< T >& x )
    {
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        const auto half_result =
            radix2( reinterpret_cast< const std::vector< std::complex< T > >& >( x ) );

        std::vector< T > result( half_result.size() );
        const auto scale = T( 0.5 ) / result.size();
        for ( size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] =
                scale * abs( ( half_result[ k ] + z ) - imag< T > * ( half_result[ k ] - z ) *
                                                            compute_gauss< T >( k, x.size() ) );
        }

        return result;
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    std::vector< T > inverse_fft( const std::vector< std::complex< T > >& x )
    {
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        std::vector< std::complex< T > > y( x.size() );
        for ( size_type k = 0; k < y.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : y.size() - k;
            const auto z = conj( x[ s_k ] );
            y[ s_k ] =
                T( 0.5 ) * conj( ( x[ k ] + z ) - imag< T > * ( x[ k ] - z ) /
                                                      compute_gauss< T >( k, 2 * x.size() ) );
        }

        const auto complex_result = inverse_radix2( y );
        std::vector< T > result( 2 * complex_result.size() );
        std::memcpy( result.data(), complex_result.data(), result.size() * sizeof( T ) );
        return result;
    }
} // namespace sequencer::audio
