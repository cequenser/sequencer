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

    template < class Container >
    class vector_view
    {
    public:
        using size_type = typename Container::size_type;

        constexpr explicit vector_view( const Container* v, size_type offset = 0 ) noexcept
            : v_( *v ), offset_( offset )
        {
        }
        auto& operator[]( size_type i )
        {
            return v_[ offset_ + 2 * i ];
        }

        const auto& operator[]( size_type i ) const
        {
            return v_[ offset_ + 2 * i ];
        }

        size_type size() const noexcept
        {
            return v_.size() / 2;
        }

    private:
        const Container& v_;
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
    auto to_complex_signal( const std::vector< T >& v )
    {
        std::vector< std::complex< T > > signal( v.size() / 2 );
        std::memcpy( signal.data(), v.data(), v.size() * sizeof( T ) );
        return signal;
    }

    template < class T >
    auto fft( const std::vector< T >& x )
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
    auto fft_abs_scaled( const std::vector< T >& x )
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
