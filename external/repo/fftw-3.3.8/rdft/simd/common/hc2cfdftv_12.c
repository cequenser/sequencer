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
/* Generated on Thu May 24 08:08:11 EDT 2018 */

#include "rdft/codelet-rdft.h"

#if defined( ARCH_PREFERS_FMA ) || defined( ISA_EXTENSION_PREFERS_FMA )

/* Generated by: ../../../genfft/gen_hc2cdft_c.native -fma -simd -compact -variables 4
 * -pipeline-latency 8 -trivial-stores -variables 32 -no-generate-bytw -n 12 -dit -name hc2cfdftv_12
 * -include rdft/simd/hc2cfv.h */

/*
 * This function contains 71 FP additions, 66 FP multiplications,
 * (or, 41 additions, 36 multiplications, 30 fused multiply/add),
 * 86 stack variables, 2 constants, and 24 memory accesses
 */
#include "rdft/simd/hc2cfv.h"

static void hc2cfdftv_12( R* Rp, R* Ip, R* Rm, R* Im, const R* W, stride rs, INT mb, INT me,
                          INT ms )
{
    DVK( KP866025403, +0.866025403784438646763723170752936183471402627 );
    DVK( KP500000000, +0.500000000000000000000000000000000000000000000 );
    {
        INT m;
        for ( m = mb, W = W + ( ( mb - 1 ) * ( ( TWVL / VL ) * 22 ) ); m < me; m = m + VL,
              Rp = Rp + ( VL * ms ), Ip = Ip + ( VL * ms ), Rm = Rm - ( VL * ms ),
              Im = Im - ( VL * ms ), W = W + ( TWVL * 22 ), MAKE_VOLATILE_STRIDE( 48, rs ) )
        {
            V Td, TQ, Tr, TR, TI, TY, TA, TX, T12, T1e, TV, T1d, TK, TL, Ts;
            V TJ, TO, TP, TM, TN, TW, T16, T13, T17, TS, TZ, T14, T19, T15, T18;
            V T1f, T1j, T1c, T1i, T1a, T1b, T1g, T1l, T1h, T1k;
            {
                V T3, Tu, T7, Tw, Tp, TH, Tl, TE, Th, TC, Tb, Tz, T1, T2, Tt;
                V T5, T6, T4, Tv, Tn, To, Tm, TG, Tj, Tk, Ti, TD, Tf, Tg, Te;
                V TB, T9, Ta, T8, Ty, Tc, Tq, TF, Tx, T10, T11, TT, TU;
                T1 = LD( &( Rp[ 0 ] ), ms, &( Rp[ 0 ] ) );
                T2 = LD( &( Rm[ 0 ] ), -ms, &( Rm[ 0 ] ) );
                T3 = VFMACONJ( T2, T1 );
                Tt = LDW( &( W[ 0 ] ) );
                Tu = VZMULIJ( Tt, VFNMSCONJ( T2, T1 ) );
                T5 = LD( &( Rp[ WS( rs, 2 ) ] ), ms, &( Rp[ 0 ] ) );
                T6 = LD( &( Rm[ WS( rs, 2 ) ] ), -ms, &( Rm[ 0 ] ) );
                T4 = LDW( &( W[ TWVL * 6 ] ) );
                T7 = VZMULJ( T4, VFMACONJ( T6, T5 ) );
                Tv = LDW( &( W[ TWVL * 8 ] ) );
                Tw = VZMULIJ( Tv, VFNMSCONJ( T6, T5 ) );
                Tn = LD( &( Rp[ WS( rs, 1 ) ] ), ms, &( Rp[ WS( rs, 1 ) ] ) );
                To = LD( &( Rm[ WS( rs, 1 ) ] ), -ms, &( Rm[ WS( rs, 1 ) ] ) );
                Tm = LDW( &( W[ TWVL * 2 ] ) );
                Tp = VZMULJ( Tm, VFMACONJ( To, Tn ) );
                TG = LDW( &( W[ TWVL * 4 ] ) );
                TH = VZMULIJ( TG, VFNMSCONJ( To, Tn ) );
                Tj = LD( &( Rp[ WS( rs, 5 ) ] ), ms, &( Rp[ WS( rs, 1 ) ] ) );
                Tk = LD( &( Rm[ WS( rs, 5 ) ] ), -ms, &( Rm[ WS( rs, 1 ) ] ) );
                Ti = LDW( &( W[ TWVL * 18 ] ) );
                Tl = VZMULJ( Ti, VFMACONJ( Tk, Tj ) );
                TD = LDW( &( W[ TWVL * 20 ] ) );
                TE = VZMULIJ( TD, VFNMSCONJ( Tk, Tj ) );
                Tf = LD( &( Rp[ WS( rs, 3 ) ] ), ms, &( Rp[ WS( rs, 1 ) ] ) );
                Tg = LD( &( Rm[ WS( rs, 3 ) ] ), -ms, &( Rm[ WS( rs, 1 ) ] ) );
                Te = LDW( &( W[ TWVL * 10 ] ) );
                Th = VZMULJ( Te, VFMACONJ( Tg, Tf ) );
                TB = LDW( &( W[ TWVL * 12 ] ) );
                TC = VZMULIJ( TB, VFNMSCONJ( Tg, Tf ) );
                T9 = LD( &( Rp[ WS( rs, 4 ) ] ), ms, &( Rp[ 0 ] ) );
                Ta = LD( &( Rm[ WS( rs, 4 ) ] ), -ms, &( Rm[ 0 ] ) );
                T8 = LDW( &( W[ TWVL * 14 ] ) );
                Tb = VZMULJ( T8, VFMACONJ( Ta, T9 ) );
                Ty = LDW( &( W[ TWVL * 16 ] ) );
                Tz = VZMULIJ( Ty, VFNMSCONJ( Ta, T9 ) );
                Tc = VADD( T7, Tb );
                Td = VADD( T3, Tc );
                TQ = VFNMS( LDK( KP500000000 ), Tc, T3 );
                Tq = VADD( Tl, Tp );
                Tr = VADD( Th, Tq );
                TR = VFNMS( LDK( KP500000000 ), Tq, Th );
                TF = VADD( TC, TE );
                TI = VADD( TF, TH );
                TY = VFNMS( LDK( KP500000000 ), TF, TH );
                Tx = VADD( Tu, Tw );
                TA = VADD( Tx, Tz );
                TX = VFNMS( LDK( KP500000000 ), Tx, Tz );
                T10 = VSUB( Tb, T7 );
                T11 = VSUB( Tp, Tl );
                T12 = VSUB( T10, T11 );
                T1e = VADD( T10, T11 );
                TT = VSUB( TC, TE );
                TU = VSUB( Tu, Tw );
                TV = VSUB( TT, TU );
                T1d = VADD( TU, TT );
            }
            Ts = VSUB( Td, Tr );
            TJ = VSUB( TA, TI );
            TK = VMUL( LDK( KP500000000 ), VFMAI( TJ, Ts ) );
            TL = VCONJ( VMUL( LDK( KP500000000 ), VFNMSI( TJ, Ts ) ) );
            ST( &( Rp[ WS( rs, 3 ) ] ), TK, ms, &( Rp[ WS( rs, 1 ) ] ) );
            ST( &( Rm[ WS( rs, 2 ) ] ), TL, -ms, &( Rm[ 0 ] ) );
            TM = VADD( Td, Tr );
            TN = VADD( TA, TI );
            TO = VMUL( LDK( KP500000000 ), VSUB( TM, TN ) );
            TP = VCONJ( VMUL( LDK( KP500000000 ), VADD( TN, TM ) ) );
            ST( &( Rp[ 0 ] ), TO, ms, &( Rp[ 0 ] ) );
            ST( &( Rm[ WS( rs, 5 ) ] ), TP, -ms, &( Rm[ WS( rs, 1 ) ] ) );
            TS = VSUB( TQ, TR );
            TW = VFMA( LDK( KP866025403 ), TV, TS );
            T16 = VFNMS( LDK( KP866025403 ), TV, TS );
            TZ = VSUB( TX, TY );
            T13 = VFNMS( LDK( KP866025403 ), T12, TZ );
            T17 = VFMA( LDK( KP866025403 ), T12, TZ );
            T14 = VMUL( LDK( KP500000000 ), VFNMSI( T13, TW ) );
            ST( &( Rp[ WS( rs, 1 ) ] ), T14, ms, &( Rp[ WS( rs, 1 ) ] ) );
            T19 = VCONJ( VMUL( LDK( KP500000000 ), VFMAI( T17, T16 ) ) );
            ST( &( Rm[ WS( rs, 4 ) ] ), T19, -ms, &( Rm[ 0 ] ) );
            T15 = VCONJ( VMUL( LDK( KP500000000 ), VFMAI( T13, TW ) ) );
            ST( &( Rm[ 0 ] ), T15, -ms, &( Rm[ 0 ] ) );
            T18 = VMUL( LDK( KP500000000 ), VFNMSI( T17, T16 ) );
            ST( &( Rp[ WS( rs, 5 ) ] ), T18, ms, &( Rp[ WS( rs, 1 ) ] ) );
            T1f = VMUL( LDK( KP866025403 ), VSUB( T1d, T1e ) );
            T1j = VMUL( LDK( KP866025403 ), VADD( T1d, T1e ) );
            T1a = VADD( TX, TY );
            T1b = VADD( TQ, TR );
            T1c = VADD( T1a, T1b );
            T1i = VSUB( T1b, T1a );
            T1g = VCONJ( VMUL( LDK( KP500000000 ), VFNMSI( T1f, T1c ) ) );
            ST( &( Rm[ WS( rs, 1 ) ] ), T1g, -ms, &( Rm[ WS( rs, 1 ) ] ) );
            T1l = VMUL( LDK( KP500000000 ), VFMAI( T1j, T1i ) );
            ST( &( Rp[ WS( rs, 4 ) ] ), T1l, ms, &( Rp[ 0 ] ) );
            T1h = VMUL( LDK( KP500000000 ), VFMAI( T1f, T1c ) );
            ST( &( Rp[ WS( rs, 2 ) ] ), T1h, ms, &( Rp[ 0 ] ) );
            T1k = VCONJ( VMUL( LDK( KP500000000 ), VFNMSI( T1j, T1i ) ) );
            ST( &( Rm[ WS( rs, 3 ) ] ), T1k, -ms, &( Rm[ WS( rs, 1 ) ] ) );
        }
    }
    VLEAVE();
}

static const tw_instr twinstr[] = {VTW( 1, 1 ), VTW( 1, 2 ),  VTW( 1, 3 ),  VTW( 1, 4 ),
                                   VTW( 1, 5 ), VTW( 1, 6 ),  VTW( 1, 7 ),  VTW( 1, 8 ),
                                   VTW( 1, 9 ), VTW( 1, 10 ), VTW( 1, 11 ), {TW_NEXT, VL, 0}};

static const hc2c_desc desc = {
    12, XSIMD_STRING( "hc2cfdftv_12" ), twinstr, &GENUS, {41, 36, 30, 0}};

void XSIMD( codelet_hc2cfdftv_12 )( planner* p )
{
    X( khc2c_register )( p, hc2cfdftv_12, &desc, HC2C_VIA_DFT );
}
#else

/* Generated by: ../../../genfft/gen_hc2cdft_c.native -simd -compact -variables 4 -pipeline-latency
 * 8 -trivial-stores -variables 32 -no-generate-bytw -n 12 -dit -name hc2cfdftv_12 -include
 * rdft/simd/hc2cfv.h */

/*
 * This function contains 71 FP additions, 41 FP multiplications,
 * (or, 67 additions, 37 multiplications, 4 fused multiply/add),
 * 58 stack variables, 4 constants, and 24 memory accesses
 */
#include "rdft/simd/hc2cfv.h"

static void hc2cfdftv_12( R* Rp, R* Ip, R* Rm, R* Im, const R* W, stride rs, INT mb, INT me,
                          INT ms )
{
    DVK( KP433012701, +0.433012701892219323381861585376468091735701313 );
    DVK( KP866025403, +0.866025403784438646763723170752936183471402627 );
    DVK( KP250000000, +0.250000000000000000000000000000000000000000000 );
    DVK( KP500000000, +0.500000000000000000000000000000000000000000000 );
    {
        INT m;
        for ( m = mb, W = W + ( ( mb - 1 ) * ( ( TWVL / VL ) * 22 ) ); m < me; m = m + VL,
              Rp = Rp + ( VL * ms ), Ip = Ip + ( VL * ms ), Rm = Rm - ( VL * ms ),
              Im = Im - ( VL * ms ), W = W + ( TWVL * 22 ), MAKE_VOLATILE_STRIDE( 48, rs ) )
        {
            V TX, T13, T4, Tf, TZ, TD, TF, T17, TW, T14, Tw, Tl, T10, TL, TN;
            V T16;
            {
                V T1, T3, TA, Tb, Td, Te, T9, TC, T2, Tz, Tc, Ta, T6, T8, T7;
                V T5, TB, TE, Ti, Tk, TI, Ts, Tu, Tv, Tq, TK, Tj, TH, Tt, Tr;
                V Tn, Tp, To, Tm, TJ, Th, TM;
                T1 = LD( &( Rp[ 0 ] ), ms, &( Rp[ 0 ] ) );
                T2 = LD( &( Rm[ 0 ] ), -ms, &( Rm[ 0 ] ) );
                T3 = VCONJ( T2 );
                Tz = LDW( &( W[ 0 ] ) );
                TA = VZMULIJ( Tz, VSUB( T3, T1 ) );
                Tb = LD( &( Rp[ WS( rs, 4 ) ] ), ms, &( Rp[ 0 ] ) );
                Tc = LD( &( Rm[ WS( rs, 4 ) ] ), -ms, &( Rm[ 0 ] ) );
                Td = VCONJ( Tc );
                Ta = LDW( &( W[ TWVL * 14 ] ) );
                Te = VZMULJ( Ta, VADD( Tb, Td ) );
                T6 = LD( &( Rp[ WS( rs, 2 ) ] ), ms, &( Rp[ 0 ] ) );
                T7 = LD( &( Rm[ WS( rs, 2 ) ] ), -ms, &( Rm[ 0 ] ) );
                T8 = VCONJ( T7 );
                T5 = LDW( &( W[ TWVL * 6 ] ) );
                T9 = VZMULJ( T5, VADD( T6, T8 ) );
                TB = LDW( &( W[ TWVL * 8 ] ) );
                TC = VZMULIJ( TB, VSUB( T8, T6 ) );
                TX = VSUB( TC, TA );
                T13 = VSUB( Te, T9 );
                T4 = VADD( T1, T3 );
                Tf = VADD( T9, Te );
                TZ = VFNMS( LDK( KP250000000 ), Tf, VMUL( LDK( KP500000000 ), T4 ) );
                TD = VADD( TA, TC );
                TE = LDW( &( W[ TWVL * 16 ] ) );
                TF = VZMULIJ( TE, VSUB( Td, Tb ) );
                T17 = VFNMS( LDK( KP500000000 ), TD, TF );
                Ti = LD( &( Rp[ WS( rs, 3 ) ] ), ms, &( Rp[ WS( rs, 1 ) ] ) );
                Tj = LD( &( Rm[ WS( rs, 3 ) ] ), -ms, &( Rm[ WS( rs, 1 ) ] ) );
                Tk = VCONJ( Tj );
                TH = LDW( &( W[ TWVL * 12 ] ) );
                TI = VZMULIJ( TH, VSUB( Tk, Ti ) );
                Ts = LD( &( Rp[ WS( rs, 1 ) ] ), ms, &( Rp[ WS( rs, 1 ) ] ) );
                Tt = LD( &( Rm[ WS( rs, 1 ) ] ), -ms, &( Rm[ WS( rs, 1 ) ] ) );
                Tu = VCONJ( Tt );
                Tr = LDW( &( W[ TWVL * 2 ] ) );
                Tv = VZMULJ( Tr, VADD( Ts, Tu ) );
                Tn = LD( &( Rp[ WS( rs, 5 ) ] ), ms, &( Rp[ WS( rs, 1 ) ] ) );
                To = LD( &( Rm[ WS( rs, 5 ) ] ), -ms, &( Rm[ WS( rs, 1 ) ] ) );
                Tp = VCONJ( To );
                Tm = LDW( &( W[ TWVL * 18 ] ) );
                Tq = VZMULJ( Tm, VADD( Tn, Tp ) );
                TJ = LDW( &( W[ TWVL * 20 ] ) );
                TK = VZMULIJ( TJ, VSUB( Tp, Tn ) );
                TW = VSUB( TK, TI );
                T14 = VSUB( Tv, Tq );
                Tw = VADD( Tq, Tv );
                Th = LDW( &( W[ TWVL * 10 ] ) );
                Tl = VZMULJ( Th, VADD( Ti, Tk ) );
                T10 = VFNMS( LDK( KP250000000 ), Tw, VMUL( LDK( KP500000000 ), Tl ) );
                TL = VADD( TI, TK );
                TM = LDW( &( W[ TWVL * 4 ] ) );
                TN = VZMULIJ( TM, VSUB( Tu, Ts ) );
                T16 = VFNMS( LDK( KP500000000 ), TL, TN );
            }
            {
                V Ty, TS, TP, TT, Tg, Tx, TG, TO, TQ, TV, TR, TU, T1i, T1o, T1l;
                V T1p, T1g, T1h, T1j, T1k, T1m, T1r, T1n, T1q, T12, T1c, T19, T1d, TY, T11;
                V T15, T18, T1a, T1f, T1b, T1e;
                Tg = VADD( T4, Tf );
                Tx = VADD( Tl, Tw );
                Ty = VADD( Tg, Tx );
                TS = VSUB( Tg, Tx );
                TG = VADD( TD, TF );
                TO = VADD( TL, TN );
                TP = VADD( TG, TO );
                TT = VBYI( VSUB( TO, TG ) );
                TQ = VCONJ( VMUL( LDK( KP500000000 ), VSUB( Ty, TP ) ) );
                ST( &( Rm[ WS( rs, 5 ) ] ), TQ, -ms, &( Rm[ WS( rs, 1 ) ] ) );
                TV = VMUL( LDK( KP500000000 ), VADD( TS, TT ) );
                ST( &( Rp[ WS( rs, 3 ) ] ), TV, ms, &( Rp[ WS( rs, 1 ) ] ) );
                TR = VMUL( LDK( KP500000000 ), VADD( Ty, TP ) );
                ST( &( Rp[ 0 ] ), TR, ms, &( Rp[ 0 ] ) );
                TU = VCONJ( VMUL( LDK( KP500000000 ), VSUB( TS, TT ) ) );
                ST( &( Rm[ WS( rs, 2 ) ] ), TU, -ms, &( Rm[ 0 ] ) );
                T1g = VADD( TX, TW );
                T1h = VADD( T13, T14 );
                T1i = VMUL( LDK( KP500000000 ),
                            VBYI( VMUL( LDK( KP866025403 ), VSUB( T1g, T1h ) ) ) );
                T1o = VMUL( LDK( KP500000000 ),
                            VBYI( VMUL( LDK( KP866025403 ), VADD( T1g, T1h ) ) ) );
                T1j = VADD( TZ, T10 );
                T1k = VMUL( LDK( KP500000000 ), VADD( T17, T16 ) );
                T1l = VSUB( T1j, T1k );
                T1p = VADD( T1j, T1k );
                T1m = VADD( T1i, T1l );
                ST( &( Rp[ WS( rs, 2 ) ] ), T1m, ms, &( Rp[ 0 ] ) );
                T1r = VCONJ( VSUB( T1p, T1o ) );
                ST( &( Rm[ WS( rs, 3 ) ] ), T1r, -ms, &( Rm[ WS( rs, 1 ) ] ) );
                T1n = VCONJ( VSUB( T1l, T1i ) );
                ST( &( Rm[ WS( rs, 1 ) ] ), T1n, -ms, &( Rm[ WS( rs, 1 ) ] ) );
                T1q = VADD( T1o, T1p );
                ST( &( Rp[ WS( rs, 4 ) ] ), T1q, ms, &( Rp[ 0 ] ) );
                TY = VMUL( LDK( KP433012701 ), VSUB( TW, TX ) );
                T11 = VSUB( TZ, T10 );
                T12 = VADD( TY, T11 );
                T1c = VSUB( T11, TY );
                T15 = VMUL( LDK( KP866025403 ), VSUB( T13, T14 ) );
                T18 = VSUB( T16, T17 );
                T19 = VMUL( LDK( KP500000000 ), VBYI( VSUB( T15, T18 ) ) );
                T1d = VMUL( LDK( KP500000000 ), VBYI( VADD( T15, T18 ) ) );
                T1a = VCONJ( VSUB( T12, T19 ) );
                ST( &( Rm[ 0 ] ), T1a, -ms, &( Rm[ 0 ] ) );
                T1f = VCONJ( VADD( T1c, T1d ) );
                ST( &( Rm[ WS( rs, 4 ) ] ), T1f, -ms, &( Rm[ 0 ] ) );
                T1b = VADD( T12, T19 );
                ST( &( Rp[ WS( rs, 1 ) ] ), T1b, ms, &( Rp[ WS( rs, 1 ) ] ) );
                T1e = VSUB( T1c, T1d );
                ST( &( Rp[ WS( rs, 5 ) ] ), T1e, ms, &( Rp[ WS( rs, 1 ) ] ) );
            }
        }
    }
    VLEAVE();
}

static const tw_instr twinstr[] = {VTW( 1, 1 ), VTW( 1, 2 ),  VTW( 1, 3 ),  VTW( 1, 4 ),
                                   VTW( 1, 5 ), VTW( 1, 6 ),  VTW( 1, 7 ),  VTW( 1, 8 ),
                                   VTW( 1, 9 ), VTW( 1, 10 ), VTW( 1, 11 ), {TW_NEXT, VL, 0}};

static const hc2c_desc desc = {12, XSIMD_STRING( "hc2cfdftv_12" ), twinstr, &GENUS, {67, 37, 4, 0}};

void XSIMD( codelet_hc2cfdftv_12 )( planner* p )
{
    X( khc2c_register )( p, hc2cfdftv_12, &desc, HC2C_VIA_DFT );
}
#endif
