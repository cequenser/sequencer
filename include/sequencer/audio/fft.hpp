#pragma once

#include <sequencer/type_traits.hpp>

#include <cassert>
#include <cmath>
#include <complex>
#include <cstring>
#include <vector>

namespace sequencer::audio
{
    template < class T >
    class const_vector_view
    {
    public:
        using size_type = typename std::vector< T >::size_type;
        using value_type = T;
        static constexpr size_type stride = 2;

        constexpr explicit const_vector_view( const std::vector< T >* v,
                                              size_type offset = 0 ) noexcept
            : v_( v ), offset_( offset )
        {
        }

        constexpr explicit const_vector_view( const const_vector_view< T >* view,
                                              size_type offset = 0 ) noexcept
            : view_( view ), offset_( offset )
        {
        }

        const T& operator[]( size_type i ) const
        {
            return v_ ? ( *v_ )[ offset_ + stride * i ] : ( *view_ )[ offset_ + stride * i ];
        }

        size_type size() const noexcept
        {
            const auto full_size = v_ ? v_->size() : view_->size();
            // NOTE: only multiples of stride in offset reduce size, therefore
            // the next line will not work properly if written as (full_size - offset_)/stride
            return full_size / stride - offset_ / stride;
        }

    private:
        const std::vector< T >* v_{nullptr};
        const const_vector_view< T >* view_{nullptr};
        const size_type offset_{0};
    };

    template < class T >
    class vector_view
    {
    public:
        using size_type = typename std::vector< T >::size_type;
        using value_type = T;

        constexpr explicit vector_view( std::vector< T >* v, size_type offset = 0 ) noexcept
            : v_( v ), offset_( offset )
        {
        }

        constexpr explicit vector_view( vector_view< T >* view, size_type offset = 0 ) noexcept
            : view_( view ), offset_( offset )
        {
        }

        T& operator[]( size_type i )
        {
            return v_ ? ( *v_ )[ offset_ + i ] : ( *view_ )[ offset_ + i ];
        }

        const T& operator[]( size_type i ) const
        {
            return v_ ? ( *v_ )[ offset_ + i ] : ( *view_ )[ offset_ + i ];
        }

        size_type size() const noexcept
        {
            const auto full_size = v_ ? v_->size() : view_->size();
            return full_size - offset_;
        }

    private:
        std::vector< T >* v_{nullptr};
        vector_view< T >* view_{nullptr};
        const size_type offset_{0};
    };

    template < class Container >
    auto radix2( const Container& x )
    {
        if ( x.size() == 1 )
        {
            return std::vector{x[ 0 ]};
        }

        auto y = radix2( const_vector_view{&x, 0} );
        const auto z = radix2( const_vector_view{&x, 1} );

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
        radix2( const_vector_view{&x, 0}, even_result_view );
        radix2( const_vector_view{&x, 1}, odd_result_view );

        for ( typename Container::size_type k = 0; k < x.size() / 2; ++k )
        {
            using namespace std::complex_literals;
            const auto dy = std::exp( minus_two_pi< double > * 1i * ( double( k ) / x.size() ) ) *
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

        const auto scale = 1.0 / y.size();
        for ( auto& v : y )
        {
            v = scale * conj( v );
        }

        return y;
    }

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    std::vector< std::complex< T > > fft( const std::vector< T >& x )
    {
        using namespace std::complex_literals;
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        const auto half_result =
            radix2( reinterpret_cast< const std::vector< std::complex< T > >& >( x ) );

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

    template < class T, std::enable_if_t< !is_complex_v< T > >* = nullptr >
    void fft( const std::vector< T >& x, std::vector< std::complex< T > >& result )
    {
        using namespace std::complex_literals;
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        std::vector< std::complex< T > > half_result( x.size() / 2 );
        radix2( reinterpret_cast< const std::vector< std::complex< T > >& >( x ), half_result );

        for ( size_type k = 0; k < result.size(); ++k )
        {
            const auto s_k = ( k == 0 ) ? 0 : result.size() - k;
            const auto z = conj( half_result[ s_k ] );
            result[ k ] = T( 0.5 ) * ( ( half_result[ k ] + z ) -
                                       1i * ( half_result[ k ] - z ) *
                                           exp( minus_two_pi< T > * 1i * ( T( k ) / x.size() ) ) );
        }
    }

    template < class T >
    std::vector< T > fft_abs_scaled( const std::vector< T >& x )
    {
        using namespace std::complex_literals;
        using size_type = typename std::vector< std::complex< T > >::size_type;

        assert( x.size() % 2 == 0 );
        const auto half_result =
            radix2( reinterpret_cast< const std::vector< std::complex< T > >& >( x ) );

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
