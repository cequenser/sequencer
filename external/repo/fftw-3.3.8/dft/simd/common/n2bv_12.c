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
/* Generated on Thu May 24 08:05:11 EDT 2018 */

#include "dft/codelet-dft.h"

#if defined( ARCH_PREFERS_FMA ) || defined( ISA_EXTENSION_PREFERS_FMA )

/* Generated by: ../../../genfft/gen_notw_c.native -fma -simd -compact -variables 4
 * -pipeline-latency 8 -sign 1 -n 12 -name n2bv_12 -with-ostride 2 -include dft/simd/n2b.h
 * -store-multiple 2 */

/*
 * This function contains 48 FP additions, 20 FP multiplications,
 * (or, 30 additions, 2 multiplications, 18 fused multiply/add),
 * 33 stack variables, 2 constants, and 30 memory accesses
 */
#include "dft/simd/n2b.h"

static void n2bv_12( const R* ri, const R* ii, R* ro, R* io, stride is, stride os, INT v, INT ivs,
                     INT ovs )
{
    DVK( KP866025403, +0.866025403784438646763723170752936183471402627 );
    DVK( KP500000000, +0.500000000000000000000000000000000000000000000 );
    {
        INT i;
        const R* xi;
        R* xo;
        xi = ii;
        xo = io;
        for ( i = v; i > 0; i = i - VL, xi = xi + ( VL * ivs ), xo = xo + ( VL * ovs ),
              MAKE_VOLATILE_STRIDE( 24, is ), MAKE_VOLATILE_STRIDE( 24, os ) )
        {
            V T5, Ta, TJ, TB, Tq, Tp, Tg, Tl, TG, Ty, Tt, Ts;
            {
                V T1, T6, T4, Tz, T9, TA;
                T1 = LD( &( xi[ 0 ] ), ivs, &( xi[ 0 ] ) );
                T6 = LD( &( xi[ WS( is, 6 ) ] ), ivs, &( xi[ 0 ] ) );
                {
                    V T2, T3, T7, T8;
                    T2 = LD( &( xi[ WS( is, 4 ) ] ), ivs, &( xi[ 0 ] ) );
                    T3 = LD( &( xi[ WS( is, 8 ) ] ), ivs, &( xi[ 0 ] ) );
                    T4 = VADD( T2, T3 );
                    Tz = VSUB( T2, T3 );
                    T7 = LD( &( xi[ WS( is, 10 ) ] ), ivs, &( xi[ 0 ] ) );
                    T8 = LD( &( xi[ WS( is, 2 ) ] ), ivs, &( xi[ 0 ] ) );
                    T9 = VADD( T7, T8 );
                    TA = VSUB( T7, T8 );
                }
                T5 = VADD( T1, T4 );
                Ta = VADD( T6, T9 );
                TJ = VSUB( Tz, TA );
                TB = VADD( Tz, TA );
                Tq = VFNMS( LDK( KP500000000 ), T9, T6 );
                Tp = VFNMS( LDK( KP500000000 ), T4, T1 );
            }
            {
                V Tc, Th, Tf, Tw, Tk, Tx;
                Tc = LD( &( xi[ WS( is, 3 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                Th = LD( &( xi[ WS( is, 9 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                {
                    V Td, Te, Ti, Tj;
                    Td = LD( &( xi[ WS( is, 7 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                    Te = LD( &( xi[ WS( is, 11 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                    Tf = VADD( Td, Te );
                    Tw = VSUB( Td, Te );
                    Ti = LD( &( xi[ WS( is, 1 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                    Tj = LD( &( xi[ WS( is, 5 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                    Tk = VADD( Ti, Tj );
                    Tx = VSUB( Tj, Ti );
                }
                Tg = VADD( Tc, Tf );
                Tl = VADD( Th, Tk );
                TG = VADD( Tw, Tx );
                Ty = VSUB( Tw, Tx );
                Tt = VFNMS( LDK( KP500000000 ), Tk, Th );
                Ts = VFNMS( LDK( KP500000000 ), Tf, Tc );
            }
            {
                V TN, TO, TP, TQ, TR, TS;
                {
                    V Tb, Tm, Tn, To;
                    Tb = VSUB( T5, Ta );
                    Tm = VSUB( Tg, Tl );
                    TN = VFNMSI( Tm, Tb );
                    STM2( &( xo[ 6 ] ), TN, ovs, &( xo[ 2 ] ) );
                    TO = VFMAI( Tm, Tb );
                    STM2( &( xo[ 18 ] ), TO, ovs, &( xo[ 2 ] ) );
                    Tn = VADD( T5, Ta );
                    To = VADD( Tg, Tl );
                    TP = VSUB( Tn, To );
                    STM2( &( xo[ 12 ] ), TP, ovs, &( xo[ 0 ] ) );
                    TQ = VADD( Tn, To );
                    STM2( &( xo[ 0 ] ), TQ, ovs, &( xo[ 0 ] ) );
                }
                {
                    V TC, TE, Tv, TD, Tr, Tu, TT, TU;
                    TC = VMUL( LDK( KP866025403 ), VSUB( Ty, TB ) );
                    TE = VMUL( LDK( KP866025403 ), VADD( TB, Ty ) );
                    Tr = VADD( Tp, Tq );
                    Tu = VADD( Ts, Tt );
                    Tv = VSUB( Tr, Tu );
                    TD = VADD( Tr, Tu );
                    TR = VFNMSI( TC, Tv );
                    STM2( &( xo[ 20 ] ), TR, ovs, &( xo[ 0 ] ) );
                    TS = VFMAI( TE, TD );
                    STM2( &( xo[ 8 ] ), TS, ovs, &( xo[ 0 ] ) );
                    TT = VFMAI( TC, Tv );
                    STM2( &( xo[ 4 ] ), TT, ovs, &( xo[ 0 ] ) );
                    STN2( &( xo[ 4 ] ), TT, TN, ovs );
                    TU = VFNMSI( TE, TD );
                    STM2( &( xo[ 16 ] ), TU, ovs, &( xo[ 0 ] ) );
                    STN2( &( xo[ 16 ] ), TU, TO, ovs );
                }
                {
                    V TH, TL, TK, TM, TF, TI;
                    TF = VSUB( Tp, Tq );
                    TH = VFNMS( LDK( KP866025403 ), TG, TF );
                    TL = VFMA( LDK( KP866025403 ), TG, TF );
                    TI = VSUB( Ts, Tt );
                    TK = VFMA( LDK( KP866025403 ), TJ, TI );
                    TM = VFNMS( LDK( KP866025403 ), TJ, TI );
                    {
                        V TV, TW, TX, TY;
                        TV = VFMAI( TK, TH );
                        STM2( &( xo[ 2 ] ), TV, ovs, &( xo[ 2 ] ) );
                        STN2( &( xo[ 0 ] ), TQ, TV, ovs );
                        TW = VFNMSI( TM, TL );
                        STM2( &( xo[ 14 ] ), TW, ovs, &( xo[ 2 ] ) );
                        STN2( &( xo[ 12 ] ), TP, TW, ovs );
                        TX = VFNMSI( TK, TH );
                        STM2( &( xo[ 22 ] ), TX, ovs, &( xo[ 2 ] ) );
                        STN2( &( xo[ 20 ] ), TR, TX, ovs );
                        TY = VFMAI( TM, TL );
                        STM2( &( xo[ 10 ] ), TY, ovs, &( xo[ 2 ] ) );
                        STN2( &( xo[ 8 ] ), TS, TY, ovs );
                    }
                }
            }
        }
    }
    VLEAVE();
}

static const kdft_desc desc = {12, XSIMD_STRING( "n2bv_12" ), {30, 2, 18, 0}, &GENUS, 0, 2, 0, 0};

void XSIMD( codelet_n2bv_12 )( planner* p )
{
    X( kdft_register )( p, n2bv_12, &desc );
}

#else

/* Generated by: ../../../genfft/gen_notw_c.native -simd -compact -variables 4 -pipeline-latency 8
 * -sign 1 -n 12 -name n2bv_12 -with-ostride 2 -include dft/simd/n2b.h -store-multiple 2 */

/*
 * This function contains 48 FP additions, 8 FP multiplications,
 * (or, 44 additions, 4 multiplications, 4 fused multiply/add),
 * 33 stack variables, 2 constants, and 30 memory accesses
 */
#include "dft/simd/n2b.h"

static void n2bv_12( const R* ri, const R* ii, R* ro, R* io, stride is, stride os, INT v, INT ivs,
                     INT ovs )
{
    DVK( KP866025403, +0.866025403784438646763723170752936183471402627 );
    DVK( KP500000000, +0.500000000000000000000000000000000000000000000 );
    {
        INT i;
        const R* xi;
        R* xo;
        xi = ii;
        xo = io;
        for ( i = v; i > 0; i = i - VL, xi = xi + ( VL * ivs ), xo = xo + ( VL * ovs ),
              MAKE_VOLATILE_STRIDE( 24, is ), MAKE_VOLATILE_STRIDE( 24, os ) )
        {
            V T5, Ta, TG, TF, Ty, Tm, Ti, Tp, TJ, TI, Tx, Ts;
            {
                V T1, T6, T4, Tk, T9, Tl;
                T1 = LD( &( xi[ 0 ] ), ivs, &( xi[ 0 ] ) );
                T6 = LD( &( xi[ WS( is, 6 ) ] ), ivs, &( xi[ 0 ] ) );
                {
                    V T2, T3, T7, T8;
                    T2 = LD( &( xi[ WS( is, 4 ) ] ), ivs, &( xi[ 0 ] ) );
                    T3 = LD( &( xi[ WS( is, 8 ) ] ), ivs, &( xi[ 0 ] ) );
                    T4 = VADD( T2, T3 );
                    Tk = VSUB( T2, T3 );
                    T7 = LD( &( xi[ WS( is, 10 ) ] ), ivs, &( xi[ 0 ] ) );
                    T8 = LD( &( xi[ WS( is, 2 ) ] ), ivs, &( xi[ 0 ] ) );
                    T9 = VADD( T7, T8 );
                    Tl = VSUB( T7, T8 );
                }
                T5 = VFNMS( LDK( KP500000000 ), T4, T1 );
                Ta = VFNMS( LDK( KP500000000 ), T9, T6 );
                TG = VADD( T6, T9 );
                TF = VADD( T1, T4 );
                Ty = VADD( Tk, Tl );
                Tm = VMUL( LDK( KP866025403 ), VSUB( Tk, Tl ) );
            }
            {
                V Tn, Tq, Te, To, Th, Tr;
                Tn = LD( &( xi[ WS( is, 3 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                Tq = LD( &( xi[ WS( is, 9 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                {
                    V Tc, Td, Tf, Tg;
                    Tc = LD( &( xi[ WS( is, 7 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                    Td = LD( &( xi[ WS( is, 11 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                    Te = VSUB( Tc, Td );
                    To = VADD( Tc, Td );
                    Tf = LD( &( xi[ WS( is, 1 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                    Tg = LD( &( xi[ WS( is, 5 ) ] ), ivs, &( xi[ WS( is, 1 ) ] ) );
                    Th = VSUB( Tf, Tg );
                    Tr = VADD( Tf, Tg );
                }
                Ti = VMUL( LDK( KP866025403 ), VSUB( Te, Th ) );
                Tp = VFNMS( LDK( KP500000000 ), To, Tn );
                TJ = VADD( Tq, Tr );
                TI = VADD( Tn, To );
                Tx = VADD( Te, Th );
                Ts = VFNMS( LDK( KP500000000 ), Tr, Tq );
            }
            {
                V TN, TO, TP, TQ, TR, TS;
                {
                    V TH, TK, TL, TM;
                    TH = VSUB( TF, TG );
                    TK = VBYI( VSUB( TI, TJ ) );
                    TN = VSUB( TH, TK );
                    STM2( &( xo[ 6 ] ), TN, ovs, &( xo[ 2 ] ) );
                    TO = VADD( TH, TK );
                    STM2( &( xo[ 18 ] ), TO, ovs, &( xo[ 2 ] ) );
                    TL = VADD( TF, TG );
                    TM = VADD( TI, TJ );
                    TP = VSUB( TL, TM );
                    STM2( &( xo[ 12 ] ), TP, ovs, &( xo[ 0 ] ) );
                    TQ = VADD( TL, TM );
                    STM2( &( xo[ 0 ] ), TQ, ovs, &( xo[ 0 ] ) );
                }
                {
                    V Tj, Tv, Tu, Tw, Tb, Tt, TT, TU;
                    Tb = VSUB( T5, Ta );
                    Tj = VSUB( Tb, Ti );
                    Tv = VADD( Tb, Ti );
                    Tt = VSUB( Tp, Ts );
                    Tu = VBYI( VADD( Tm, Tt ) );
                    Tw = VBYI( VSUB( Tt, Tm ) );
                    TR = VSUB( Tj, Tu );
                    STM2( &( xo[ 22 ] ), TR, ovs, &( xo[ 2 ] ) );
                    TS = VADD( Tv, Tw );
                    STM2( &( xo[ 10 ] ), TS, ovs, &( xo[ 2 ] ) );
                    TT = VADD( Tj, Tu );
                    STM2( &( xo[ 2 ] ), TT, ovs, &( xo[ 2 ] ) );
                    STN2( &( xo[ 0 ] ), TQ, TT, ovs );
                    TU = VSUB( Tv, Tw );
                    STM2( &( xo[ 14 ] ), TU, ovs, &( xo[ 2 ] ) );
                    STN2( &( xo[ 12 ] ), TP, TU, ovs );
                }
                {
                    V Tz, TD, TC, TE, TA, TB;
                    Tz = VBYI( VMUL( LDK( KP866025403 ), VSUB( Tx, Ty ) ) );
                    TD = VBYI( VMUL( LDK( KP866025403 ), VADD( Ty, Tx ) ) );
                    TA = VADD( T5, Ta );
                    TB = VADD( Tp, Ts );
                    TC = VSUB( TA, TB );
                    TE = VADD( TA, TB );
                    {
                        V TV, TW, TX, TY;
                        TV = VADD( Tz, TC );
                        STM2( &( xo[ 4 ] ), TV, ovs, &( xo[ 0 ] ) );
                        STN2( &( xo[ 4 ] ), TV, TN, ovs );
                        TW = VSUB( TE, TD );
                        STM2( &( xo[ 16 ] ), TW, ovs, &( xo[ 0 ] ) );
                        STN2( &( xo[ 16 ] ), TW, TO, ovs );
                        TX = VSUB( TC, Tz );
                        STM2( &( xo[ 20 ] ), TX, ovs, &( xo[ 0 ] ) );
                        STN2( &( xo[ 20 ] ), TX, TR, ovs );
                        TY = VADD( TD, TE );
                        STM2( &( xo[ 8 ] ), TY, ovs, &( xo[ 0 ] ) );
                        STN2( &( xo[ 8 ] ), TY, TS, ovs );
                    }
                }
            }
        }
    }
    VLEAVE();
}

static const kdft_desc desc = {12, XSIMD_STRING( "n2bv_12" ), {44, 4, 4, 0}, &GENUS, 0, 2, 0, 0};

void XSIMD( codelet_n2bv_12 )( planner* p )
{
    X( kdft_register )( p, n2bv_12, &desc );
}

#endif
