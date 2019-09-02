#pragma once

#include <complex>
#include <type_traits>

namespace sequencer
{
    template < class T >
    constexpr T two_pi = T( 2 * M_PI );

    template < class T >
    constexpr T minus_two_pi = -two_pi< T >;

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
} // namespace sequencer
