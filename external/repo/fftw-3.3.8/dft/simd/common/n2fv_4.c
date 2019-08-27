/*
 * Copyright (c) 2003, 2007-14 Matteo Frigo
 * Copyright (c) 2003, 2007-14 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Thu May 24 08:05:07 EDT 2018 */

#include "dft/codelet-dft.h"

#if defined( ARCH_PREFERS_FMA ) || defined( ISA_EXTENSION_PREFERS_FMA )

/* Generated by: ../../../genfft/gen_notw_c.native -fma -simd -compact -variables 4
 * -pipeline-latency 8 -n 4 -name n2fv_4 -with-ostride 2 -include dft/simd/n2f.h -store-multiple 2
 */

/*
 * This function contains 8 FP additions, 2 FP multiplications,
 * (or, 6 additions, 0 multiplications, 2 fused multiply/add),
 * 11 stack variables, 0 constants, and 10 memory accesses
 */
#include "dft/simd/n2f.h"

static void n2fv_4( const R* ri, const R* ii, R* ro, R* io, stride is, stride os, INT v, INT ivs,
                    INT ovs )
{
    {
        INT i;
        const R* xi;
        R* xo;
        xi = ri;
        xo = ro;
        for ( i = v; i > 0; i = i - VL, xi = xi + ( VL * ivs ), xo = xo + ( VL * ovs ),
              MAKE_VOLATILE_STRIDE( 8, is ), MAKE_VOLATILE_STRIDE( 8, os ) )
        {
            V T3, T7, T6, T8;
            {
                V T1, T2, T4, T5;
                T1 = LD( &( xi[ 0 ] ), ivs, &( xi[ 0 ] ) );
                T2 = LD( &( xi[ WS( is, 2 ) ] ), ivs, &( xi[ 0 ] ) );
                T3 = VSUB( T1, T2 );
                T7 = VADD( T1, T2 );
                T4 = LD( &( xi[ WS( is, 1 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                T5 = LD( &( xi[ WS( is, 3 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                T6 = VSUB( T4, T5 );
                T8 = VADD( T4, T5 );
            }
            {
                V T9, Ta, Tb, Tc;
                T9 = VFNMSI( T6, T3 );
                STM2( &( xo[ 2 ] ), T9, ovs, &( xo[ 2 ] ) );
                Ta = VADD( T7, T8 );
                STM2( &( xo[ 0 ] ), Ta, ovs, &( xo[ 0 ] ) );
                STN2( &( xo[ 0 ] ), Ta, T9, ovs );
                Tb = VFMAI( T6, T3 );
                STM2( &( xo[ 6 ] ), Tb, ovs, &( xo[ 2 ] ) );
                Tc = VSUB( T7, T8 );
                STM2( &( xo[ 4 ] ), Tc, ovs, &( xo[ 0 ] ) );
                STN2( &( xo[ 4 ] ), Tc, Tb, ovs );
            }
        }
    }
    VLEAVE();
}

static const kdft_desc desc = {4, XSIMD_STRING( "n2fv_4" ), {6, 0, 2, 0}, &GENUS, 0, 2, 0, 0};

void XSIMD( codelet_n2fv_4 )( planner* p )
{
    X( kdft_register )( p, n2fv_4, &desc );
}

#else

/* Generated by: ../../../genfft/gen_notw_c.native -simd -compact -variables 4 -pipeline-latency 8
 * -n 4 -name n2fv_4 -with-ostride 2 -include dft/simd/n2f.h -store-multiple 2 */

/*
 * This function contains 8 FP additions, 0 FP multiplications,
 * (or, 8 additions, 0 multiplications, 0 fused multiply/add),
 * 11 stack variables, 0 constants, and 10 memory accesses
 */
#include "dft/simd/n2f.h"

static void n2fv_4( const R* ri, const R* ii, R* ro, R* io, stride is, stride os, INT v, INT ivs,
                    INT ovs )
{
    {
        INT i;
        const R* xi;
        R* xo;
        xi = ri;
        xo = ro;
        for ( i = v; i > 0; i = i - VL, xi = xi + ( VL * ivs ), xo = xo + ( VL * ovs ),
              MAKE_VOLATILE_STRIDE( 8, is ), MAKE_VOLATILE_STRIDE( 8, os ) )
        {
            V T3, T7, T6, T8;
            {
                V T1, T2, T4, T5;
                T1 = LD( &( xi[ 0 ] ), ivs, &( xi[ 0 ] ) );
                T2 = LD( &( xi[ WS( is, 2 ) ] ), ivs, &( xi[ 0 ] ) );
                T3 = VSUB( T1, T2 );
                T7 = VADD( T1, T2 );
                T4 = LD( &( xi[ WS( is, 1 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                T5 = LD( &( xi[ WS( is, 3 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                T6 = VBYI( VSUB( T4, T5 ) );
                T8 = VADD( T4, T5 );
            }
            {
                V T9, Ta, Tb, Tc;
                T9 = VSUB( T3, T6 );
                STM2( &( xo[ 2 ] ), T9, ovs, &( xo[ 2 ] ) );
                Ta = VADD( T7, T8 );
                STM2( &( xo[ 0 ] ), Ta, ovs, &( xo[ 0 ] ) );
                STN2( &( xo[ 0 ] ), Ta, T9, ovs );
                Tb = VADD( T3, T6 );
                STM2( &( xo[ 6 ] ), Tb, ovs, &( xo[ 2 ] ) );
                Tc = VSUB( T7, T8 );
                STM2( &( xo[ 4 ] ), Tc, ovs, &( xo[ 0 ] ) );
                STN2( &( xo[ 4 ] ), Tc, Tb, ovs );
            }
        }
    }
    VLEAVE();
}

static const kdft_desc desc = {4, XSIMD_STRING( "n2fv_4" ), {8, 0, 0, 0}, &GENUS, 0, 2, 0, 0};

void XSIMD( codelet_n2fv_4 )( planner* p )
{
    X( kdft_register )( p, n2fv_4, &desc );
}

#endif
