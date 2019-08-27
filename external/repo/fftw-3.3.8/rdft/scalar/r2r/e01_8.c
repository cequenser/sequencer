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
/* Generated on Thu May 24 08:08:10 EDT 2018 */

#include "rdft/codelet-rdft.h"

#if defined( ARCH_PREFERS_FMA ) || defined( ISA_EXTENSION_PREFERS_FMA )

/* Generated by: ../../../genfft/gen_r2r.native -fma -compact -variables 4 -pipeline-latency 4
 * -redft01 -n 8 -name e01_8 -include rdft/scalar/r2r.h */

/*
 * This function contains 26 FP additions, 24 FP multiplications,
 * (or, 2 additions, 0 multiplications, 24 fused multiply/add),
 * 27 stack variables, 8 constants, and 16 memory accesses
 */
#include "rdft/scalar/r2r.h"

static void e01_8( const R* I, R* O, stride is, stride os, INT v, INT ivs, INT ovs )
{
    DK( KP1_662939224, +1.662939224605090474157576755235811513477121624 );
    DK( KP668178637, +0.668178637919298919997757686523080761552472251 );
    DK( KP1_961570560, +1.961570560806460898252364472268478073947867462 );
    DK( KP198912367, +0.198912367379658006911597622644676228597850501 );
    DK( KP1_847759065, +1.847759065022573512256366378793576573644833252 );
    DK( KP707106781, +0.707106781186547524400844362104849039284835938 );
    DK( KP414213562, +0.414213562373095048801688724209698078569671875 );
    DK( KP1_414213562, +1.414213562373095048801688724209698078569671875 );
    {
        INT i;
        for ( i = v; i > 0; i = i - 1, I = I + ivs, O = O + ovs, MAKE_VOLATILE_STRIDE( 16, is ),
              MAKE_VOLATILE_STRIDE( 16, os ) )
        {
            E T3, Tj, T6, Tk, Tc, Tn, Tf, Tm;
            {
                E T1, T2, T4, T5;
                T1 = I[ 0 ];
                T2 = I[ WS( is, 4 ) ];
                T3 = FMA( KP1_414213562, T2, T1 );
                Tj = FNMS( KP1_414213562, T2, T1 );
                T4 = I[ WS( is, 2 ) ];
                T5 = I[ WS( is, 6 ) ];
                T6 = FMA( KP414213562, T5, T4 );
                Tk = FMS( KP414213562, T4, T5 );
                {
                    E T8, Td, Tb, Te, T9, Ta;
                    T8 = I[ WS( is, 1 ) ];
                    Td = I[ WS( is, 7 ) ];
                    T9 = I[ WS( is, 5 ) ];
                    Ta = I[ WS( is, 3 ) ];
                    Tb = T9 + Ta;
                    Te = Ta - T9;
                    Tc = FMA( KP707106781, Tb, T8 );
                    Tn = FNMS( KP707106781, Te, Td );
                    Tf = FMA( KP707106781, Te, Td );
                    Tm = FNMS( KP707106781, Tb, T8 );
                }
            }
            {
                E T7, Tg, Tp, Tq;
                T7 = FMA( KP1_847759065, T6, T3 );
                Tg = FMA( KP198912367, Tf, Tc );
                O[ WS( os, 7 ) ] = FNMS( KP1_961570560, Tg, T7 );
                O[ 0 ] = FMA( KP1_961570560, Tg, T7 );
                Tp = FNMS( KP1_847759065, Tk, Tj );
                Tq = FMA( KP668178637, Tm, Tn );
                O[ WS( os, 5 ) ] = FNMS( KP1_662939224, Tq, Tp );
                O[ WS( os, 2 ) ] = FMA( KP1_662939224, Tq, Tp );
            }
            {
                E Th, Ti, Tl, To;
                Th = FNMS( KP1_847759065, T6, T3 );
                Ti = FNMS( KP198912367, Tc, Tf );
                O[ WS( os, 3 ) ] = FNMS( KP1_961570560, Ti, Th );
                O[ WS( os, 4 ) ] = FMA( KP1_961570560, Ti, Th );
                Tl = FMA( KP1_847759065, Tk, Tj );
                To = FNMS( KP668178637, Tn, Tm );
                O[ WS( os, 6 ) ] = FNMS( KP1_662939224, To, Tl );
                O[ WS( os, 1 ) ] = FMA( KP1_662939224, To, Tl );
            }
        }
    }
}

static const kr2r_desc desc = {8, "e01_8", {2, 0, 24, 0}, &GENUS, REDFT01};

void X( codelet_e01_8 )( planner* p )
{
    X( kr2r_register )( p, e01_8, &desc );
}

#else

/* Generated by: ../../../genfft/gen_r2r.native -compact -variables 4 -pipeline-latency 4 -redft01
 * -n 8 -name e01_8 -include rdft/scalar/r2r.h */

/*
 * This function contains 26 FP additions, 15 FP multiplications,
 * (or, 20 additions, 9 multiplications, 6 fused multiply/add),
 * 28 stack variables, 8 constants, and 16 memory accesses
 */
#include "rdft/scalar/r2r.h"

static void e01_8( const R* I, R* O, stride is, stride os, INT v, INT ivs, INT ovs )
{
    DK( KP1_662939224, +1.662939224605090474157576755235811513477121624 );
    DK( KP1_111140466, +1.111140466039204449485661627897065748749874382 );
    DK( KP390180644, +0.390180644032256535696569736954044481855383236 );
    DK( KP1_961570560, +1.961570560806460898252364472268478073947867462 );
    DK( KP707106781, +0.707106781186547524400844362104849039284835938 );
    DK( KP1_414213562, +1.414213562373095048801688724209698078569671875 );
    DK( KP765366864, +0.765366864730179543456919968060797733522689125 );
    DK( KP1_847759065, +1.847759065022573512256366378793576573644833252 );
    {
        INT i;
        for ( i = v; i > 0; i = i - 1, I = I + ivs, O = O + ovs, MAKE_VOLATILE_STRIDE( 16, is ),
              MAKE_VOLATILE_STRIDE( 16, os ) )
        {
            E T7, Tl, T4, Tk, Td, To, Tg, Tn;
            {
                E T5, T6, T1, T3, T2;
                T5 = I[ WS( is, 2 ) ];
                T6 = I[ WS( is, 6 ) ];
                T7 = FMA( KP1_847759065, T5, KP765366864 * T6 );
                Tl = FNMS( KP1_847759065, T6, KP765366864 * T5 );
                T1 = I[ 0 ];
                T2 = I[ WS( is, 4 ) ];
                T3 = KP1_414213562 * T2;
                T4 = T1 + T3;
                Tk = T1 - T3;
                {
                    E T9, Tf, Tc, Te, Ta, Tb;
                    T9 = I[ WS( is, 1 ) ];
                    Tf = I[ WS( is, 7 ) ];
                    Ta = I[ WS( is, 5 ) ];
                    Tb = I[ WS( is, 3 ) ];
                    Tc = KP707106781 * ( Ta + Tb );
                    Te = KP707106781 * ( Ta - Tb );
                    Td = T9 + Tc;
                    To = Te + Tf;
                    Tg = Te - Tf;
                    Tn = T9 - Tc;
                }
            }
            {
                E T8, Th, Tq, Tr;
                T8 = T4 + T7;
                Th = FNMS( KP390180644, Tg, KP1_961570560 * Td );
                O[ WS( os, 7 ) ] = T8 - Th;
                O[ 0 ] = T8 + Th;
                Tq = Tk - Tl;
                Tr = FMA( KP1_111140466, Tn, KP1_662939224 * To );
                O[ WS( os, 5 ) ] = Tq - Tr;
                O[ WS( os, 2 ) ] = Tq + Tr;
            }
            {
                E Ti, Tj, Tm, Tp;
                Ti = T4 - T7;
                Tj = FMA( KP390180644, Td, KP1_961570560 * Tg );
                O[ WS( os, 4 ) ] = Ti - Tj;
                O[ WS( os, 3 ) ] = Ti + Tj;
                Tm = Tk + Tl;
                Tp = FNMS( KP1_111140466, To, KP1_662939224 * Tn );
                O[ WS( os, 6 ) ] = Tm - Tp;
                O[ WS( os, 1 ) ] = Tm + Tp;
            }
        }
    }
}

static const kr2r_desc desc = {8, "e01_8", {20, 9, 6, 0}, &GENUS, REDFT01};

void X( codelet_e01_8 )( planner* p )
{
    X( kr2r_register )( p, e01_8, &desc );
}

#endif
