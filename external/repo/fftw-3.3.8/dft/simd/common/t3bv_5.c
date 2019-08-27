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
/* Generated on Thu May 24 08:06:09 EDT 2018 */

#include "dft/codelet-dft.h"

#if defined( ARCH_PREFERS_FMA ) || defined( ISA_EXTENSION_PREFERS_FMA )

/* Generated by: ../../../genfft/gen_twiddle_c.native -fma -simd -compact -variables 4
 * -pipeline-latency 8 -twiddle-log3 -precompute-twiddles -no-generate-bytw -n 5 -name t3bv_5
 * -include dft/simd/t3b.h -sign 1 */

/*
 * This function contains 22 FP additions, 23 FP multiplications,
 * (or, 13 additions, 14 multiplications, 9 fused multiply/add),
 * 24 stack variables, 4 constants, and 10 memory accesses
 */
#include "dft/simd/t3b.h"

static void t3bv_5( R* ri, R* ii, const R* W, stride rs, INT mb, INT me, INT ms )
{
    DVK( KP559016994, +0.559016994374947424102293417182819058860154590 );
    DVK( KP250000000, +0.250000000000000000000000000000000000000000000 );
    DVK( KP618033988, +0.618033988749894848204586834365638117720309180 );
    DVK( KP951056516, +0.951056516295153572116439333379382143405698634 );
    {
        INT m;
        R* x;
        x = ii;
        for ( m = mb, W = W + ( mb * ( ( TWVL / VL ) * 4 ) ); m < me;
              m = m + VL, x = x + ( VL * ms ), W = W + ( TWVL * 4 ), MAKE_VOLATILE_STRIDE( 5, rs ) )
        {
            V T2, T5, T6, Ta;
            T2 = LDW( &( W[ 0 ] ) );
            T5 = LDW( &( W[ TWVL * 2 ] ) );
            T6 = VZMUL( T2, T5 );
            Ta = VZMULJ( T2, T5 );
            {
                V T1, Tk, Tl, T9, Tf, Tg;
                T1 = LD( &( x[ 0 ] ), ms, &( x[ 0 ] ) );
                {
                    V T4, Te, T8, Tc;
                    {
                        V T3, Td, T7, Tb;
                        T3 = LD( &( x[ WS( rs, 1 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        T4 = VZMUL( T2, T3 );
                        Td = LD( &( x[ WS( rs, 3 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Te = VZMUL( T5, Td );
                        T7 = LD( &( x[ WS( rs, 4 ) ] ), ms, &( x[ 0 ] ) );
                        T8 = VZMUL( T6, T7 );
                        Tb = LD( &( x[ WS( rs, 2 ) ] ), ms, &( x[ 0 ] ) );
                        Tc = VZMUL( Ta, Tb );
                    }
                    Tk = VSUB( T4, T8 );
                    Tl = VSUB( Tc, Te );
                    T9 = VADD( T4, T8 );
                    Tf = VADD( Tc, Te );
                    Tg = VADD( T9, Tf );
                }
                ST( &( x[ 0 ] ), VADD( T1, Tg ), ms, &( x[ 0 ] ) );
                {
                    V Tm, To, Tj, Tn, Th, Ti;
                    Tm = VMUL( LDK( KP951056516 ), VFMA( LDK( KP618033988 ), Tl, Tk ) );
                    To = VMUL( LDK( KP951056516 ), VFNMS( LDK( KP618033988 ), Tk, Tl ) );
                    Th = VFNMS( LDK( KP250000000 ), Tg, T1 );
                    Ti = VSUB( T9, Tf );
                    Tj = VFMA( LDK( KP559016994 ), Ti, Th );
                    Tn = VFNMS( LDK( KP559016994 ), Ti, Th );
                    ST( &( x[ WS( rs, 1 ) ] ), VFMAI( Tm, Tj ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 3 ) ] ), VFMAI( To, Tn ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 4 ) ] ), VFNMSI( Tm, Tj ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 2 ) ] ), VFNMSI( To, Tn ), ms, &( x[ 0 ] ) );
                }
            }
        }
    }
    VLEAVE();
}

static const tw_instr twinstr[] = {VTW( 0, 1 ), VTW( 0, 3 ), {TW_NEXT, VL, 0}};

static const ct_desc desc = {5, XSIMD_STRING( "t3bv_5" ), twinstr, &GENUS, {13, 14, 9, 0}, 0, 0, 0};

void XSIMD( codelet_t3bv_5 )( planner* p )
{
    X( kdft_dit_register )( p, t3bv_5, &desc );
}
#else

/* Generated by: ../../../genfft/gen_twiddle_c.native -simd -compact -variables 4 -pipeline-latency
 * 8 -twiddle-log3 -precompute-twiddles -no-generate-bytw -n 5 -name t3bv_5 -include dft/simd/t3b.h
 * -sign 1 */

/*
 * This function contains 22 FP additions, 18 FP multiplications,
 * (or, 19 additions, 15 multiplications, 3 fused multiply/add),
 * 24 stack variables, 4 constants, and 10 memory accesses
 */
#include "dft/simd/t3b.h"

static void t3bv_5( R* ri, R* ii, const R* W, stride rs, INT mb, INT me, INT ms )
{
    DVK( KP250000000, +0.250000000000000000000000000000000000000000000 );
    DVK( KP559016994, +0.559016994374947424102293417182819058860154590 );
    DVK( KP587785252, +0.587785252292473129168705954639072768597652438 );
    DVK( KP951056516, +0.951056516295153572116439333379382143405698634 );
    {
        INT m;
        R* x;
        x = ii;
        for ( m = mb, W = W + ( mb * ( ( TWVL / VL ) * 4 ) ); m < me;
              m = m + VL, x = x + ( VL * ms ), W = W + ( TWVL * 4 ), MAKE_VOLATILE_STRIDE( 5, rs ) )
        {
            V T1, T4, T5, T9;
            T1 = LDW( &( W[ 0 ] ) );
            T4 = LDW( &( W[ TWVL * 2 ] ) );
            T5 = VZMUL( T1, T4 );
            T9 = VZMULJ( T1, T4 );
            {
                V Tj, T8, Te, Tg, Th, Tk;
                Tj = LD( &( x[ 0 ] ), ms, &( x[ 0 ] ) );
                {
                    V T3, Td, T7, Tb;
                    {
                        V T2, Tc, T6, Ta;
                        T2 = LD( &( x[ WS( rs, 1 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        T3 = VZMUL( T1, T2 );
                        Tc = LD( &( x[ WS( rs, 3 ) ] ), ms, &( x[ WS( rs, 1 ) ] ) );
                        Td = VZMUL( T4, Tc );
                        T6 = LD( &( x[ WS( rs, 4 ) ] ), ms, &( x[ 0 ] ) );
                        T7 = VZMUL( T5, T6 );
                        Ta = LD( &( x[ WS( rs, 2 ) ] ), ms, &( x[ 0 ] ) );
                        Tb = VZMUL( T9, Ta );
                    }
                    T8 = VSUB( T3, T7 );
                    Te = VSUB( Tb, Td );
                    Tg = VADD( T3, T7 );
                    Th = VADD( Tb, Td );
                    Tk = VADD( Tg, Th );
                }
                ST( &( x[ 0 ] ), VADD( Tj, Tk ), ms, &( x[ 0 ] ) );
                {
                    V Tf, Tn, Tm, To, Ti, Tl;
                    Tf = VBYI( VFMA( LDK( KP951056516 ), T8, VMUL( LDK( KP587785252 ), Te ) ) );
                    Tn = VBYI( VFNMS( LDK( KP951056516 ), Te, VMUL( LDK( KP587785252 ), T8 ) ) );
                    Ti = VMUL( LDK( KP559016994 ), VSUB( Tg, Th ) );
                    Tl = VFNMS( LDK( KP250000000 ), Tk, Tj );
                    Tm = VADD( Ti, Tl );
                    To = VSUB( Tl, Ti );
                    ST( &( x[ WS( rs, 1 ) ] ), VADD( Tf, Tm ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 3 ) ] ), VSUB( To, Tn ), ms, &( x[ WS( rs, 1 ) ] ) );
                    ST( &( x[ WS( rs, 4 ) ] ), VSUB( Tm, Tf ), ms, &( x[ 0 ] ) );
                    ST( &( x[ WS( rs, 2 ) ] ), VADD( Tn, To ), ms, &( x[ 0 ] ) );
                }
            }
        }
    }
    VLEAVE();
}

static const tw_instr twinstr[] = {VTW( 0, 1 ), VTW( 0, 3 ), {TW_NEXT, VL, 0}};

static const ct_desc desc = {5, XSIMD_STRING( "t3bv_5" ), twinstr, &GENUS, {19, 15, 3, 0}, 0, 0, 0};

void XSIMD( codelet_t3bv_5 )( planner* p )
{
    X( kdft_dit_register )( p, t3bv_5, &desc );
}
#endif
