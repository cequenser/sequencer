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
/* Generated on Thu May 24 08:06:28 EDT 2018 */

#include "rdft/codelet-rdft.h"

#if defined( ARCH_PREFERS_FMA ) || defined( ISA_EXTENSION_PREFERS_FMA )

/* Generated by: ../../../genfft/gen_hc2hc.native -fma -compact -variables 4 -pipeline-latency 4 -n
 * 5 -dit -name hf_5 -include rdft/scalar/hf.h */

/*
 * This function contains 40 FP additions, 34 FP multiplications,
 * (or, 14 additions, 8 multiplications, 26 fused multiply/add),
 * 31 stack variables, 4 constants, and 20 memory accesses
 */
#include "rdft/scalar/hf.h"

static void hf_5( R* cr, R* ci, const R* W, stride rs, INT mb, INT me, INT ms )
{
    DK( KP951056516, +0.951056516295153572116439333379382143405698634 );
    DK( KP559016994, +0.559016994374947424102293417182819058860154590 );
    DK( KP618033988, +0.618033988749894848204586834365638117720309180 );
    DK( KP250000000, +0.250000000000000000000000000000000000000000000 );
    {
        INT m;
        for ( m = mb, W = W + ( ( mb - 1 ) * 8 ); m < me;
              m = m + 1, cr = cr + ms, ci = ci - ms, W = W + 8, MAKE_VOLATILE_STRIDE( 10, rs ) )
        {
            E T1, TJ, T7, Tx, Td, Tz, Te, TK, Tk, TC, Tq, TE, Tr, TL;
            T1 = cr[ 0 ];
            TJ = ci[ 0 ];
            {
                E T3, T6, T4, Tw, T9, Tc, Ta, Ty, T2, T8, T5, Tb;
                T3 = cr[ WS( rs, 1 ) ];
                T6 = ci[ WS( rs, 1 ) ];
                T2 = W[ 0 ];
                T4 = T2 * T3;
                Tw = T2 * T6;
                T9 = cr[ WS( rs, 4 ) ];
                Tc = ci[ WS( rs, 4 ) ];
                T8 = W[ 6 ];
                Ta = T8 * T9;
                Ty = T8 * Tc;
                T5 = W[ 1 ];
                T7 = FMA( T5, T6, T4 );
                Tx = FNMS( T5, T3, Tw );
                Tb = W[ 7 ];
                Td = FMA( Tb, Tc, Ta );
                Tz = FNMS( Tb, T9, Ty );
                Te = T7 + Td;
                TK = Tx + Tz;
            }
            {
                E Tg, Tj, Th, TB, Tm, Tp, Tn, TD, Tf, Tl, Ti, To;
                Tg = cr[ WS( rs, 2 ) ];
                Tj = ci[ WS( rs, 2 ) ];
                Tf = W[ 2 ];
                Th = Tf * Tg;
                TB = Tf * Tj;
                Tm = cr[ WS( rs, 3 ) ];
                Tp = ci[ WS( rs, 3 ) ];
                Tl = W[ 4 ];
                Tn = Tl * Tm;
                TD = Tl * Tp;
                Ti = W[ 3 ];
                Tk = FMA( Ti, Tj, Th );
                TC = FNMS( Ti, Tg, TB );
                To = W[ 5 ];
                Tq = FMA( To, Tp, Tn );
                TE = FNMS( To, Tm, TD );
                Tr = Tk + Tq;
                TL = TC + TE;
            }
            {
                E Tu, Ts, Tt, TG, TI, TA, TF, Tv, TH;
                Tu = Te - Tr;
                Ts = Te + Tr;
                Tt = FNMS( KP250000000, Ts, T1 );
                TA = Tx - Tz;
                TF = TC - TE;
                TG = FMA( KP618033988, TF, TA );
                TI = FNMS( KP618033988, TA, TF );
                cr[ 0 ] = T1 + Ts;
                Tv = FMA( KP559016994, Tu, Tt );
                ci[ 0 ] = FNMS( KP951056516, TG, Tv );
                cr[ WS( rs, 1 ) ] = FMA( KP951056516, TG, Tv );
                TH = FNMS( KP559016994, Tu, Tt );
                cr[ WS( rs, 2 ) ] = FNMS( KP951056516, TI, TH );
                ci[ WS( rs, 1 ) ] = FMA( KP951056516, TI, TH );
            }
            {
                E TO, TM, TN, TS, TU, TQ, TR, TT, TP;
                TO = TK - TL;
                TM = TK + TL;
                TN = FNMS( KP250000000, TM, TJ );
                TQ = Tk - Tq;
                TR = Td - T7;
                TS = FMA( KP618033988, TR, TQ );
                TU = FNMS( KP618033988, TQ, TR );
                ci[ WS( rs, 4 ) ] = TM + TJ;
                TT = FMA( KP559016994, TO, TN );
                cr[ WS( rs, 4 ) ] = FMS( KP951056516, TU, TT );
                ci[ WS( rs, 3 ) ] = FMA( KP951056516, TU, TT );
                TP = FNMS( KP559016994, TO, TN );
                cr[ WS( rs, 3 ) ] = FMS( KP951056516, TS, TP );
                ci[ WS( rs, 2 ) ] = FMA( KP951056516, TS, TP );
            }
        }
    }
}

static const tw_instr twinstr[] = {{TW_FULL, 1, 5}, {TW_NEXT, 1, 0}};

static const hc2hc_desc desc = {5, "hf_5", twinstr, &GENUS, {14, 8, 26, 0}};

void X( codelet_hf_5 )( planner* p )
{
    X( khc2hc_register )( p, hf_5, &desc );
}
#else

/* Generated by: ../../../genfft/gen_hc2hc.native -compact -variables 4 -pipeline-latency 4 -n 5
 * -dit -name hf_5 -include rdft/scalar/hf.h */

/*
 * This function contains 40 FP additions, 28 FP multiplications,
 * (or, 26 additions, 14 multiplications, 14 fused multiply/add),
 * 29 stack variables, 4 constants, and 20 memory accesses
 */
#include "rdft/scalar/hf.h"

static void hf_5( R* cr, R* ci, const R* W, stride rs, INT mb, INT me, INT ms )
{
    DK( KP250000000, +0.250000000000000000000000000000000000000000000 );
    DK( KP559016994, +0.559016994374947424102293417182819058860154590 );
    DK( KP587785252, +0.587785252292473129168705954639072768597652438 );
    DK( KP951056516, +0.951056516295153572116439333379382143405698634 );
    {
        INT m;
        for ( m = mb, W = W + ( ( mb - 1 ) * 8 ); m < me;
              m = m + 1, cr = cr + ms, ci = ci - ms, W = W + 8, MAKE_VOLATILE_STRIDE( 10, rs ) )
        {
            E T1, TE, Tu, Tx, TC, TB, TF, TG, TH, Tc, Tn, To;
            T1 = cr[ 0 ];
            TE = ci[ 0 ];
            {
                E T6, Ts, Tm, Tw, Tb, Tt, Th, Tv;
                {
                    E T3, T5, T2, T4;
                    T3 = cr[ WS( rs, 1 ) ];
                    T5 = ci[ WS( rs, 1 ) ];
                    T2 = W[ 0 ];
                    T4 = W[ 1 ];
                    T6 = FMA( T2, T3, T4 * T5 );
                    Ts = FNMS( T4, T3, T2 * T5 );
                }
                {
                    E Tj, Tl, Ti, Tk;
                    Tj = cr[ WS( rs, 3 ) ];
                    Tl = ci[ WS( rs, 3 ) ];
                    Ti = W[ 4 ];
                    Tk = W[ 5 ];
                    Tm = FMA( Ti, Tj, Tk * Tl );
                    Tw = FNMS( Tk, Tj, Ti * Tl );
                }
                {
                    E T8, Ta, T7, T9;
                    T8 = cr[ WS( rs, 4 ) ];
                    Ta = ci[ WS( rs, 4 ) ];
                    T7 = W[ 6 ];
                    T9 = W[ 7 ];
                    Tb = FMA( T7, T8, T9 * Ta );
                    Tt = FNMS( T9, T8, T7 * Ta );
                }
                {
                    E Te, Tg, Td, Tf;
                    Te = cr[ WS( rs, 2 ) ];
                    Tg = ci[ WS( rs, 2 ) ];
                    Td = W[ 2 ];
                    Tf = W[ 3 ];
                    Th = FMA( Td, Te, Tf * Tg );
                    Tv = FNMS( Tf, Te, Td * Tg );
                }
                Tu = Ts - Tt;
                Tx = Tv - Tw;
                TC = Th - Tm;
                TB = Tb - T6;
                TF = Ts + Tt;
                TG = Tv + Tw;
                TH = TF + TG;
                Tc = T6 + Tb;
                Tn = Th + Tm;
                To = Tc + Tn;
            }
            cr[ 0 ] = T1 + To;
            {
                E Ty, TA, Tr, Tz, Tp, Tq;
                Ty = FMA( KP951056516, Tu, KP587785252 * Tx );
                TA = FNMS( KP587785252, Tu, KP951056516 * Tx );
                Tp = KP559016994 * ( Tc - Tn );
                Tq = FNMS( KP250000000, To, T1 );
                Tr = Tp + Tq;
                Tz = Tq - Tp;
                ci[ 0 ] = Tr - Ty;
                ci[ WS( rs, 1 ) ] = Tz + TA;
                cr[ WS( rs, 1 ) ] = Tr + Ty;
                cr[ WS( rs, 2 ) ] = Tz - TA;
            }
            ci[ WS( rs, 4 ) ] = TH + TE;
            {
                E TD, TL, TK, TM, TI, TJ;
                TD = FMA( KP587785252, TB, KP951056516 * TC );
                TL = FNMS( KP587785252, TC, KP951056516 * TB );
                TI = FNMS( KP250000000, TH, TE );
                TJ = KP559016994 * ( TF - TG );
                TK = TI - TJ;
                TM = TJ + TI;
                cr[ WS( rs, 3 ) ] = TD - TK;
                ci[ WS( rs, 3 ) ] = TL + TM;
                ci[ WS( rs, 2 ) ] = TD + TK;
                cr[ WS( rs, 4 ) ] = TL - TM;
            }
        }
    }
}

static const tw_instr twinstr[] = {{TW_FULL, 1, 5}, {TW_NEXT, 1, 0}};

static const hc2hc_desc desc = {5, "hf_5", twinstr, &GENUS, {26, 14, 14, 0}};

void X( codelet_hf_5 )( planner* p )
{
    X( khc2hc_register )( p, hf_5, &desc );
}
#endif