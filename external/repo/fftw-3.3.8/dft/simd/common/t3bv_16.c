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
/* Generated on Thu May 24 08:06:06 EDT 2018 */

#include "dft/codelet-dft.h"

#if defined( ARCH_PREFERS_FMA ) || defined( ISA_EXTENSION_PREFERS_FMA )

/* Generated by: ../../../genfft/gen_twiddle_c.native -fma -simd -compact -variables 4
 * -pipeline-latency 8 -twiddle-log3 -precompute-twiddles -no-generate-bytw -n 16 -name t3bv_16
 * -include dft/simd/t3b.h -sign 1 */

/*
 * This function contains 98 FP additions, 86 FP multiplications,
 * (or, 64 additions, 52 multiplications, 34 fused multiply/add),
 * 51 stack variables, 3 constants, and 32 memory accesses
 */
#include "dft/simd/t3b.h"

static void t3bv_16( R* ri, R* ii, const R* W, stride rs, INT mb, INT me, INT ms )
{
    DVK( KP923879532, +0.923879532511286756128183189396788286822416626 );
    DVK( KP707106781, +0.707106781186547524400844362104849039284835938 );
    DVK( KP414213562, +0.414213562373095048801688724209698078569671875 );
    {
        INT m;
        R* x;
        x = ii;
        for ( m = mb, W = W + ( mb * ( ( TWVL / VL ) * 8 ) ); m < me; m = m + VL,
              x = x + ( VL * ms ), W = W + ( TWVL * 8 ), MAKE_VOLATILE_STRIDE( 16, rs ) )
        {
            V T2, T8, T9, Tx, Tu, TR, T3, T4, TN, TU, Tc, Tm, Ty, TE, Tp;
            T2 = LDW( &( W[ 0 ] ) );
            T8 = LDW( &( W[ TWVL * 2 ] ) );
            T9 = VZMUL( T2, T8 );
            Tx = VZMULJ( T2, T8 );
            Tu = LDW( &( W[ TWVL * 6 ] ) );
            TR = VZMULJ( T2, Tu );
            T3 = LDW( &( W[ TWVL * 4 ] ) );
            T4 = VZMULJ( T2, T3 );
            TN = VZMUL( T2, T3 );
            TU = VZMULJ( T8, T3 );
            Tc = VZMUL( T8, T3 );
            Tm = VZMULJ( T9, T3 );
            Ty = VZMULJ( Tx, T3 );
            TE = VZMUL( Tx, T3 );
            Tp = VZMUL( T9, T3 );
            {
                V T7, T1b, Tf, T1o, TQ, TX, T1e, T1p, Tl, Ts, Tt, T1i, T1r, TB, TH;
                V TI, T1l, T1s, T1, T6, T5;
                T1 = LD( &( x[ 0 ] ), ms, &( x[ 0 ] ) );
                T5 = LD( &( x[ WS( rs, 8 ) ] ), ms, &( x[ 0 ] ) );
                T6 = VZMUL( T4, T5 );
                T7 = VADD( T1, T6 );
                T1b = VSUB( T1, T6 );
                {
                    V Tb, Te, Ta, Td;
                    Ta = LD( &( x[ WS( rs, 4 ) ] ), ms, &( x[ 0 ] ) );
                    Tb = VZMUL( T9, Ta );
                    Td = LD( &( x[ WS( rs, 12 ) ] ), ms, &( x[ 0 ] ) );
                    Te = VZMUL( Tc, Td );
                    Tf = VADD( Tb, Te );
                    T1o = VSUB( Tb, Te );
                }
                {
                    V TM, TW, TP, TT, T1c, T1d;
                    {
                        V TL, TV, TO, TS;
                        TL = LD( &( x[ WS( rs, 2 ) ] ), ms, &( x[ 0 ] ) );
                        TM = VZMUL( Tx, TL );
                        TV = LD( &( x[ WS( rs, 6 ) ] ), ms, &( x[ 0 ] ) );
                        TW = VZMUL( TU, TV );
                        TO = LD( &( x[ WS( rs, 10 ) ] ), ms, &( x[ 0 ] ) );
                        TP = VZMUL( TN, TO );
                        TS = LD( &( x[ WS( rs, 14 ) ] ), ms, &( x[ 0 ] ) );
                        TT = VZMUL( TR, TS );
                    }
                    TQ = VADD( TM, TP );
                    TX = VADD( TT, TW );
                    T1c = VSUB( TM, TP );
                    T1d = VSUB( TT, TW );
                    T1e = VADD( T1c, T1d );
                    T1p = VSUB( T1c, T1d );
                }
                {
                    V Ti, Tr, Tk, To, T1g, T1h;
                    {
                        V Th, Tq, Tj, Tn;
                        Th = LD( &( x[ WS( rs, 1 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Ti = VZMUL( T2, Th );
                        Tq = LD( &( x[ WS( rs, 13 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Tr = VZMUL( Tp, Tq );
                        Tj = LD( &( x[ WS( rs, 9 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Tk = VZMUL( T3, Tj );
                        Tn = LD( &( x[ WS( rs, 5 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        To = VZMUL( Tm, Tn );
                    }
                    Tl = VADD( Ti, Tk );
                    Ts = VADD( To, Tr );
                    Tt = VSUB( Tl, Ts );
                    T1g = VSUB( Ti, Tk );
                    T1h = VSUB( To, Tr );
                    T1i = VFNMS( LDK( KP414213562 ), T1h, T1g );
                    T1r = VFMA( LDK( KP414213562 ), T1g, T1h );
                }
                {
                    V Tw, TG, TA, TD, T1j, T1k;
                    {
                        V Tv, TF, Tz, TC;
                        Tv = LD( &( x[ WS( rs, 15 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Tw = VZMUL( Tu, Tv );
                        TF = LD( &( x[ WS( rs, 11 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        TG = VZMUL( TE, TF );
                        Tz = LD( &( x[ WS( rs, 7 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        TA = VZMUL( Ty, Tz );
                        TC = LD( &( x[ WS( rs, 3 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        TD = VZMUL( T8, TC );
                    }
                    TB = VADD( Tw, TA );
                    TH = VADD( TD, TG );
                    TI = VSUB( TB, TH );
                    T1j = VSUB( Tw, TA );
                    T1k = VSUB( TG, TD );
                    T1l = VFNMS( LDK( KP414213562 ), T1k, T1j );
                    T1s = VFMA( LDK( KP414213562 ), T1j, T1k );
                }
                {
                    V TK, T11, T10, T12;
                    {
                        V Tg, TJ, TY, TZ;
                        Tg = VSUB( T7, Tf );
                        TJ = VADD( Tt, TI );
                        TK = VFNMS( LDK( KP707106781 ), TJ, Tg );
                        T11 = VFMA( LDK( KP707106781 ), TJ, Tg );
                        TY = VSUB( TQ, TX );
                        TZ = VSUB( Tt, TI );
                        T10 = VFNMS( LDK( KP707106781 ), TZ, TY );
                        T12 = VFMA( LDK( KP707106781 ), TZ, TY );
                    }
                    ST( &( x[ WS( rs, 6 ) ] ), VFNMSI( T10, TK ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 14 ) ] ), VFNMSI( T12, T11 ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 10 ) ] ), VFMAI( T10, TK ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 2 ) ] ), VFMAI( T12, T11 ), ms, &( x[ 0 ] ) );
                }
                {
                    V T1z, T1D, T1C, T1E;
                    {
                        V T1x, T1y, T1A, T1B;
                        T1x = VFNMS( LDK( KP707106781 ), T1e, T1b );
                        T1y = VADD( T1r, T1s );
                        T1z = VFNMS( LDK( KP923879532 ), T1y, T1x );
                        T1D = VFMA( LDK( KP923879532 ), T1y, T1x );
                        T1A = VFNMS( LDK( KP707106781 ), T1p, T1o );
                        T1B = VSUB( T1i, T1l );
                        T1C = VFMA( LDK( KP923879532 ), T1B, T1A );
                        T1E = VFNMS( LDK( KP923879532 ), T1B, T1A );
                    }
                    ST( &( x[ WS( rs, 5 ) ] ), VFMAI( T1C, T1z ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 13 ) ] ), VFMAI( T1E, T1D ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 11 ) ] ), VFNMSI( T1C, T1z ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 3 ) ] ), VFNMSI( T1E, T1D ), ms, &( x[ WS( rs, 1 ) ] ) );
                }
                {
                    V T15, T19, T18, T1a;
                    {
                        V T13, T14, T16, T17;
                        T13 = VADD( T7, Tf );
                        T14 = VADD( TQ, TX );
                        T15 = VSUB( T13, T14 );
                        T19 = VADD( T13, T14 );
                        T16 = VADD( Tl, Ts );
                        T17 = VADD( TB, TH );
                        T18 = VSUB( T16, T17 );
                        T1a = VADD( T16, T17 );
                    }
                    ST( &( x[ WS( rs, 12 ) ] ), VFNMSI( T18, T15 ), ms, &( x[ 0 ] ) );
                    ST( &( x[ 0 ] ), VADD( T19, T1a ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 4 ) ] ), VFMAI( T18, T15 ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 8 ) ] ), VSUB( T19, T1a ), ms, &( x[ 0 ] ) );
                }
                {
                    V T1n, T1v, T1u, T1w;
                    {
                        V T1f, T1m, T1q, T1t;
                        T1f = VFMA( LDK( KP707106781 ), T1e, T1b );
                        T1m = VADD( T1i, T1l );
                        T1n = VFNMS( LDK( KP923879532 ), T1m, T1f );
                        T1v = VFMA( LDK( KP923879532 ), T1m, T1f );
                        T1q = VFMA( LDK( KP707106781 ), T1p, T1o );
                        T1t = VSUB( T1r, T1s );
                        T1u = VFNMS( LDK( KP923879532 ), T1t, T1q );
                        T1w = VFMA( LDK( KP923879532 ), T1t, T1q );
                    }
                    ST( &( x[ WS( rs, 7 ) ] ), VFNMSI( T1u, T1n ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 1 ) ] ), VFMAI( T1w, T1v ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 9 ) ] ), VFMAI( T1u, T1n ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 15 ) ] ), VFNMSI( T1w, T1v ), ms, &( x[ WS( rs, 1 ) ] ) );
                }
            }
        }
    }
    VLEAVE();
}

static const tw_instr twinstr[] = {
    VTW( 0, 1 ), VTW( 0, 3 ), VTW( 0, 9 ), VTW( 0, 15 ), {TW_NEXT, VL, 0}};

static const ct_desc desc = {16, XSIMD_STRING( "t3bv_16" ), twinstr, &GENUS, {64, 52, 34, 0}, 0, 0,
                             0};

void XSIMD( codelet_t3bv_16 )( planner* p )
{
    X( kdft_dit_register )( p, t3bv_16, &desc );
}
#else

/* Generated by: ../../../genfft/gen_twiddle_c.native -simd -compact -variables 4 -pipeline-latency
 * 8 -twiddle-log3 -precompute-twiddles -no-generate-bytw -n 16 -name t3bv_16 -include
 * dft/simd/t3b.h -sign 1 */

/*
 * This function contains 98 FP additions, 64 FP multiplications,
 * (or, 94 additions, 60 multiplications, 4 fused multiply/add),
 * 51 stack variables, 3 constants, and 32 memory accesses
 */
#include "dft/simd/t3b.h"

static void t3bv_16( R* ri, R* ii, const R* W, stride rs, INT mb, INT me, INT ms )
{
    DVK( KP382683432, +0.382683432365089771728459984030398866761344562 );
    DVK( KP923879532, +0.923879532511286756128183189396788286822416626 );
    DVK( KP707106781, +0.707106781186547524400844362104849039284835938 );
    {
        INT m;
        R* x;
        x = ii;
        for ( m = mb, W = W + ( mb * ( ( TWVL / VL ) * 8 ) ); m < me; m = m + VL,
              x = x + ( VL * ms ), W = W + ( TWVL * 8 ), MAKE_VOLATILE_STRIDE( 16, rs ) )
        {
            V T1, T8, T9, Tl, Ti, TE, T4, Ta, TO, TV, Td, Tm, TA, TH, Ts;
            T1 = LDW( &( W[ 0 ] ) );
            T8 = LDW( &( W[ TWVL * 2 ] ) );
            T9 = VZMUL( T1, T8 );
            Tl = VZMULJ( T1, T8 );
            Ti = LDW( &( W[ TWVL * 6 ] ) );
            TE = VZMULJ( T1, Ti );
            T4 = LDW( &( W[ TWVL * 4 ] ) );
            Ta = VZMULJ( T9, T4 );
            TO = VZMUL( T8, T4 );
            TV = VZMULJ( T1, T4 );
            Td = VZMUL( T9, T4 );
            Tm = VZMULJ( Tl, T4 );
            TA = VZMUL( T1, T4 );
            TH = VZMULJ( T8, T4 );
            Ts = VZMUL( Tl, T4 );
            {
                V TY, T1q, TR, T1r, T1m, T1n, TL, TZ, T1f, T1g, T1h, Th, T11, T1i, T1j;
                V T1k, Tw, T12, TU, TX, TW;
                TU = LD( &( x[ 0 ] ), ms, &( x[ 0 ] ) );
                TW = LD( &( x[ WS( rs, 8 ) ] ), ms, &( x[ 0 ] ) );
                TX = VZMUL( TV, TW );
                TY = VSUB( TU, TX );
                T1q = VADD( TU, TX );
                {
                    V TN, TQ, TM, TP;
                    TM = LD( &( x[ WS( rs, 4 ) ] ), ms, &( x[ 0 ] ) );
                    TN = VZMUL( T9, TM );
                    TP = LD( &( x[ WS( rs, 12 ) ] ), ms, &( x[ 0 ] ) );
                    TQ = VZMUL( TO, TP );
                    TR = VSUB( TN, TQ );
                    T1r = VADD( TN, TQ );
                }
                {
                    V Tz, TJ, TC, TG, TD, TK;
                    {
                        V Ty, TI, TB, TF;
                        Ty = LD( &( x[ WS( rs, 2 ) ] ), ms, &( x[ 0 ] ) );
                        Tz = VZMUL( Tl, Ty );
                        TI = LD( &( x[ WS( rs, 6 ) ] ), ms, &( x[ 0 ] ) );
                        TJ = VZMUL( TH, TI );
                        TB = LD( &( x[ WS( rs, 10 ) ] ), ms, &( x[ 0 ] ) );
                        TC = VZMUL( TA, TB );
                        TF = LD( &( x[ WS( rs, 14 ) ] ), ms, &( x[ 0 ] ) );
                        TG = VZMUL( TE, TF );
                    }
                    T1m = VADD( Tz, TC );
                    T1n = VADD( TG, TJ );
                    TD = VSUB( Tz, TC );
                    TK = VSUB( TG, TJ );
                    TL = VMUL( LDK( KP707106781 ), VSUB( TD, TK ) );
                    TZ = VMUL( LDK( KP707106781 ), VADD( TD, TK ) );
                }
                {
                    V T3, Tf, T6, Tc, T7, Tg;
                    {
                        V T2, Te, T5, Tb;
                        T2 = LD( &( x[ WS( rs, 1 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        T3 = VZMUL( T1, T2 );
                        Te = LD( &( x[ WS( rs, 13 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Tf = VZMUL( Td, Te );
                        T5 = LD( &( x[ WS( rs, 9 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        T6 = VZMUL( T4, T5 );
                        Tb = LD( &( x[ WS( rs, 5 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Tc = VZMUL( Ta, Tb );
                    }
                    T1f = VADD( T3, T6 );
                    T1g = VADD( Tc, Tf );
                    T1h = VSUB( T1f, T1g );
                    T7 = VSUB( T3, T6 );
                    Tg = VSUB( Tc, Tf );
                    Th = VFNMS( LDK( KP382683432 ), Tg, VMUL( LDK( KP923879532 ), T7 ) );
                    T11 = VFMA( LDK( KP382683432 ), T7, VMUL( LDK( KP923879532 ), Tg ) );
                }
                {
                    V Tk, Tu, To, Tr, Tp, Tv;
                    {
                        V Tj, Tt, Tn, Tq;
                        Tj = LD( &( x[ WS( rs, 15 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Tk = VZMUL( Ti, Tj );
                        Tt = LD( &( x[ WS( rs, 11 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Tu = VZMUL( Ts, Tt );
                        Tn = LD( &( x[ WS( rs, 7 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        To = VZMUL( Tm, Tn );
                        Tq = LD( &( x[ WS( rs, 3 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Tr = VZMUL( T8, Tq );
                    }
                    T1i = VADD( Tk, To );
                    T1j = VADD( Tr, Tu );
                    T1k = VSUB( T1i, T1j );
                    Tp = VSUB( Tk, To );
                    Tv = VSUB( Tr, Tu );
                    Tw = VFMA( LDK( KP923879532 ), Tp, VMUL( LDK( KP382683432 ), Tv ) );
                    T12 = VFNMS( LDK( KP382683432 ), Tp, VMUL( LDK( KP923879532 ), Tv ) );
                }
                {
                    V T1p, T1v, T1u, T1w;
                    {
                        V T1l, T1o, T1s, T1t;
                        T1l = VMUL( LDK( KP707106781 ), VSUB( T1h, T1k ) );
                        T1o = VSUB( T1m, T1n );
                        T1p = VBYI( VSUB( T1l, T1o ) );
                        T1v = VBYI( VADD( T1o, T1l ) );
                        T1s = VSUB( T1q, T1r );
                        T1t = VMUL( LDK( KP707106781 ), VADD( T1h, T1k ) );
                        T1u = VSUB( T1s, T1t );
                        T1w = VADD( T1s, T1t );
                    }
                    ST( &( x[ WS( rs, 6 ) ] ), VADD( T1p, T1u ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 14 ) ] ), VSUB( T1w, T1v ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 10 ) ] ), VSUB( T1u, T1p ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 2 ) ] ), VADD( T1v, T1w ), ms, &( x[ 0 ] ) );
                }
                {
                    V T1z, T1D, T1C, T1E;
                    {
                        V T1x, T1y, T1A, T1B;
                        T1x = VADD( T1q, T1r );
                        T1y = VADD( T1m, T1n );
                        T1z = VSUB( T1x, T1y );
                        T1D = VADD( T1x, T1y );
                        T1A = VADD( T1f, T1g );
                        T1B = VADD( T1i, T1j );
                        T1C = VBYI( VSUB( T1A, T1B ) );
                        T1E = VADD( T1A, T1B );
                    }
                    ST( &( x[ WS( rs, 12 ) ] ), VSUB( T1z, T1C ), ms, &( x[ 0 ] ) );
                    ST( &( x[ 0 ] ), VADD( T1D, T1E ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 4 ) ] ), VADD( T1z, T1C ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 8 ) ] ), VSUB( T1D, T1E ), ms, &( x[ 0 ] ) );
                }
                {
                    V TT, T15, T14, T16;
                    {
                        V Tx, TS, T10, T13;
                        Tx = VSUB( Th, Tw );
                        TS = VSUB( TL, TR );
                        TT = VBYI( VSUB( Tx, TS ) );
                        T15 = VBYI( VADD( TS, Tx ) );
                        T10 = VSUB( TY, TZ );
                        T13 = VSUB( T11, T12 );
                        T14 = VSUB( T10, T13 );
                        T16 = VADD( T10, T13 );
                    }
                    ST( &( x[ WS( rs, 5 ) ] ), VADD( TT, T14 ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 13 ) ] ), VSUB( T16, T15 ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 11 ) ] ), VSUB( T14, TT ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 3 ) ] ), VADD( T15, T16 ), ms, &( x[ WS( rs, 1 ) ] ) );
                }
                {
                    V T19, T1d, T1c, T1e;
                    {
                        V T17, T18, T1a, T1b;
                        T17 = VADD( TY, TZ );
                        T18 = VADD( Th, Tw );
                        T19 = VADD( T17, T18 );
                        T1d = VSUB( T17, T18 );
                        T1a = VADD( TR, TL );
                        T1b = VADD( T11, T12 );
                        T1c = VBYI( VADD( T1a, T1b ) );
                        T1e = VBYI( VSUB( T1b, T1a ) );
                    }
                    ST( &( x[ WS( rs, 15 ) ] ), VSUB( T19, T1c ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 7 ) ] ), VADD( T1d, T1e ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 1 ) ] ), VADD( T19, T1c ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 9 ) ] ), VSUB( T1d, T1e ), ms, &( x[ WS( rs, 1 ) ] ) );
                }
            }
        }
    }
    VLEAVE();
}

static const tw_instr twinstr[] = {
    VTW( 0, 1 ), VTW( 0, 3 ), VTW( 0, 9 ), VTW( 0, 15 ), {TW_NEXT, VL, 0}};

static const ct_desc desc = {16, XSIMD_STRING( "t3bv_16" ), twinstr, &GENUS, {94, 60, 4, 0}, 0, 0,
                             0};

void XSIMD( codelet_t3bv_16 )( planner* p )
{
    X( kdft_dit_register )( p, t3bv_16, &desc );
}
#endif
