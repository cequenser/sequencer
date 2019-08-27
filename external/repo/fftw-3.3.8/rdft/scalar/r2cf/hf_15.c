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
/* Generated on Thu May 24 08:06:30 EDT 2018 */

#include "rdft/codelet-rdft.h"

#if defined( ARCH_PREFERS_FMA ) || defined( ISA_EXTENSION_PREFERS_FMA )

/* Generated by: ../../../genfft/gen_hc2hc.native -fma -compact -variables 4 -pipeline-latency 4 -n
 * 15 -dit -name hf_15 -include rdft/scalar/hf.h */

/*
 * This function contains 184 FP additions, 140 FP multiplications,
 * (or, 72 additions, 28 multiplications, 112 fused multiply/add),
 * 51 stack variables, 6 constants, and 60 memory accesses
 */
#include "rdft/scalar/hf.h"

static void hf_15( R* cr, R* ci, const R* W, stride rs, INT mb, INT me, INT ms )
{
    DK( KP951056516, +0.951056516295153572116439333379382143405698634 );
    DK( KP559016994, +0.559016994374947424102293417182819058860154590 );
    DK( KP250000000, +0.250000000000000000000000000000000000000000000 );
    DK( KP618033988, +0.618033988749894848204586834365638117720309180 );
    DK( KP866025403, +0.866025403784438646763723170752936183471402627 );
    DK( KP500000000, +0.500000000000000000000000000000000000000000000 );
    {
        INT m;
        for ( m = mb, W = W + ( ( mb - 1 ) * 28 ); m < me;
              m = m + 1, cr = cr + ms, ci = ci - ms, W = W + 28, MAKE_VOLATILE_STRIDE( 30, rs ) )
        {
            E T1, T3i, T1G, T3l, Te, T1B, T3j, T3k, T1y, T2i, T2a, T2M, T37, T2Y, Tz;
            E T2e, T1O, T2t, T39, T2U, TT, T2f, T1V, T2z, T3a, T2V, T1e, T2h, T23, T2G;
            E T36, T2X;
            {
                E T7, T1D, Td, T1F;
                T1 = cr[ 0 ];
                T3i = ci[ 0 ];
                {
                    E T3, T6, T4, T1C, T2, T5;
                    T3 = cr[ WS( rs, 5 ) ];
                    T6 = ci[ WS( rs, 5 ) ];
                    T2 = W[ 8 ];
                    T4 = T2 * T3;
                    T1C = T2 * T6;
                    T5 = W[ 9 ];
                    T7 = FMA( T5, T6, T4 );
                    T1D = FNMS( T5, T3, T1C );
                }
                {
                    E T9, Tc, Ta, T1E, T8, Tb;
                    T9 = cr[ WS( rs, 10 ) ];
                    Tc = ci[ WS( rs, 10 ) ];
                    T8 = W[ 18 ];
                    Ta = T8 * T9;
                    T1E = T8 * Tc;
                    Tb = W[ 19 ];
                    Td = FMA( Tb, Tc, Ta );
                    T1F = FNMS( Tb, T9, T1E );
                }
                T1G = T1D - T1F;
                T3l = Td - T7;
                Te = T7 + Td;
                T1B = FNMS( KP500000000, Te, T1 );
                T3j = T1D + T1F;
                T3k = FNMS( KP500000000, T3j, T3i );
            }
            {
                E T1k, T2I, T1w, T28, T1q, T26;
                {
                    E T1g, T1j, T1h, T2H, T1f, T1i;
                    T1g = cr[ WS( rs, 9 ) ];
                    T1j = ci[ WS( rs, 9 ) ];
                    T1f = W[ 16 ];
                    T1h = T1f * T1g;
                    T2H = T1f * T1j;
                    T1i = W[ 17 ];
                    T1k = FMA( T1i, T1j, T1h );
                    T2I = FNMS( T1i, T1g, T2H );
                }
                {
                    E T1s, T1v, T1t, T27, T1r, T1u;
                    T1s = cr[ WS( rs, 4 ) ];
                    T1v = ci[ WS( rs, 4 ) ];
                    T1r = W[ 6 ];
                    T1t = T1r * T1s;
                    T27 = T1r * T1v;
                    T1u = W[ 7 ];
                    T1w = FMA( T1u, T1v, T1t );
                    T28 = FNMS( T1u, T1s, T27 );
                }
                {
                    E T1m, T1p, T1n, T25, T1l, T1o;
                    T1m = cr[ WS( rs, 14 ) ];
                    T1p = ci[ WS( rs, 14 ) ];
                    T1l = W[ 26 ];
                    T1n = T1l * T1m;
                    T25 = T1l * T1p;
                    T1o = W[ 27 ];
                    T1q = FMA( T1o, T1p, T1n );
                    T26 = FNMS( T1o, T1m, T25 );
                }
                {
                    E T29, T1x, T24, T2L, T2J, T2K;
                    T29 = T26 - T28;
                    T1x = T1q + T1w;
                    T24 = FNMS( KP500000000, T1x, T1k );
                    T1y = T1k + T1x;
                    T2i = FMA( KP866025403, T29, T24 );
                    T2a = FNMS( KP866025403, T29, T24 );
                    T2L = T1q - T1w;
                    T2J = T26 + T28;
                    T2K = FNMS( KP500000000, T2J, T2I );
                    T2M = FNMS( KP866025403, T2L, T2K );
                    T37 = T2I + T2J;
                    T2Y = FMA( KP866025403, T2L, T2K );
                }
            }
            {
                E Tl, T2p, Tx, T1M, Tr, T1K;
                {
                    E Th, Tk, Ti, T2o, Tg, Tj;
                    Th = cr[ WS( rs, 3 ) ];
                    Tk = ci[ WS( rs, 3 ) ];
                    Tg = W[ 4 ];
                    Ti = Tg * Th;
                    T2o = Tg * Tk;
                    Tj = W[ 5 ];
                    Tl = FMA( Tj, Tk, Ti );
                    T2p = FNMS( Tj, Th, T2o );
                }
                {
                    E Tt, Tw, Tu, T1L, Ts, Tv;
                    Tt = cr[ WS( rs, 13 ) ];
                    Tw = ci[ WS( rs, 13 ) ];
                    Ts = W[ 24 ];
                    Tu = Ts * Tt;
                    T1L = Ts * Tw;
                    Tv = W[ 25 ];
                    Tx = FMA( Tv, Tw, Tu );
                    T1M = FNMS( Tv, Tt, T1L );
                }
                {
                    E Tn, Tq, To, T1J, Tm, Tp;
                    Tn = cr[ WS( rs, 8 ) ];
                    Tq = ci[ WS( rs, 8 ) ];
                    Tm = W[ 14 ];
                    To = Tm * Tn;
                    T1J = Tm * Tq;
                    Tp = W[ 15 ];
                    Tr = FMA( Tp, Tq, To );
                    T1K = FNMS( Tp, Tn, T1J );
                }
                {
                    E T1N, Ty, T1I, T2s, T2q, T2r;
                    T1N = T1K - T1M;
                    Ty = Tr + Tx;
                    T1I = FNMS( KP500000000, Ty, Tl );
                    Tz = Tl + Ty;
                    T2e = FMA( KP866025403, T1N, T1I );
                    T1O = FNMS( KP866025403, T1N, T1I );
                    T2s = Tr - Tx;
                    T2q = T1K + T1M;
                    T2r = FNMS( KP500000000, T2q, T2p );
                    T2t = FNMS( KP866025403, T2s, T2r );
                    T39 = T2p + T2q;
                    T2U = FMA( KP866025403, T2s, T2r );
                }
            }
            {
                E TF, T2v, TR, T1T, TL, T1R;
                {
                    E TB, TE, TC, T2u, TA, TD;
                    TB = cr[ WS( rs, 12 ) ];
                    TE = ci[ WS( rs, 12 ) ];
                    TA = W[ 22 ];
                    TC = TA * TB;
                    T2u = TA * TE;
                    TD = W[ 23 ];
                    TF = FMA( TD, TE, TC );
                    T2v = FNMS( TD, TB, T2u );
                }
                {
                    E TN, TQ, TO, T1S, TM, TP;
                    TN = cr[ WS( rs, 7 ) ];
                    TQ = ci[ WS( rs, 7 ) ];
                    TM = W[ 12 ];
                    TO = TM * TN;
                    T1S = TM * TQ;
                    TP = W[ 13 ];
                    TR = FMA( TP, TQ, TO );
                    T1T = FNMS( TP, TN, T1S );
                }
                {
                    E TH, TK, TI, T1Q, TG, TJ;
                    TH = cr[ WS( rs, 2 ) ];
                    TK = ci[ WS( rs, 2 ) ];
                    TG = W[ 2 ];
                    TI = TG * TH;
                    T1Q = TG * TK;
                    TJ = W[ 3 ];
                    TL = FMA( TJ, TK, TI );
                    T1R = FNMS( TJ, TH, T1Q );
                }
                {
                    E T1U, TS, T1P, T2y, T2w, T2x;
                    T1U = T1R - T1T;
                    TS = TL + TR;
                    T1P = FNMS( KP500000000, TS, TF );
                    TT = TF + TS;
                    T2f = FMA( KP866025403, T1U, T1P );
                    T1V = FNMS( KP866025403, T1U, T1P );
                    T2y = TL - TR;
                    T2w = T1R + T1T;
                    T2x = FNMS( KP500000000, T2w, T2v );
                    T2z = FNMS( KP866025403, T2y, T2x );
                    T3a = T2v + T2w;
                    T2V = FMA( KP866025403, T2y, T2x );
                }
            }
            {
                E T10, T2C, T1c, T21, T16, T1Z;
                {
                    E TW, TZ, TX, T2B, TV, TY;
                    TW = cr[ WS( rs, 6 ) ];
                    TZ = ci[ WS( rs, 6 ) ];
                    TV = W[ 10 ];
                    TX = TV * TW;
                    T2B = TV * TZ;
                    TY = W[ 11 ];
                    T10 = FMA( TY, TZ, TX );
                    T2C = FNMS( TY, TW, T2B );
                }
                {
                    E T18, T1b, T19, T20, T17, T1a;
                    T18 = cr[ WS( rs, 1 ) ];
                    T1b = ci[ WS( rs, 1 ) ];
                    T17 = W[ 0 ];
                    T19 = T17 * T18;
                    T20 = T17 * T1b;
                    T1a = W[ 1 ];
                    T1c = FMA( T1a, T1b, T19 );
                    T21 = FNMS( T1a, T18, T20 );
                }
                {
                    E T12, T15, T13, T1Y, T11, T14;
                    T12 = cr[ WS( rs, 11 ) ];
                    T15 = ci[ WS( rs, 11 ) ];
                    T11 = W[ 20 ];
                    T13 = T11 * T12;
                    T1Y = T11 * T15;
                    T14 = W[ 21 ];
                    T16 = FMA( T14, T15, T13 );
                    T1Z = FNMS( T14, T12, T1Y );
                }
                {
                    E T22, T1d, T1X, T2F, T2D, T2E;
                    T22 = T1Z - T21;
                    T1d = T16 + T1c;
                    T1X = FNMS( KP500000000, T1d, T10 );
                    T1e = T10 + T1d;
                    T2h = FMA( KP866025403, T22, T1X );
                    T23 = FNMS( KP866025403, T22, T1X );
                    T2F = T16 - T1c;
                    T2D = T1Z + T21;
                    T2E = FNMS( KP500000000, T2D, T2C );
                    T2G = FNMS( KP866025403, T2F, T2E );
                    T36 = T2C + T2D;
                    T2X = FMA( KP866025403, T2F, T2E );
                }
            }
            {
                E T3c, T3e, Tf, T1A, T33, T34, T3d, T35;
                {
                    E T38, T3b, TU, T1z;
                    T38 = T36 - T37;
                    T3b = T39 - T3a;
                    T3c = FNMS( KP618033988, T3b, T38 );
                    T3e = FMA( KP618033988, T38, T3b );
                    Tf = T1 + Te;
                    TU = Tz + TT;
                    T1z = T1e + T1y;
                    T1A = TU + T1z;
                    T33 = FNMS( KP250000000, T1A, Tf );
                    T34 = TU - T1z;
                }
                cr[ 0 ] = Tf + T1A;
                T3d = FMA( KP559016994, T34, T33 );
                ci[ WS( rs, 5 ) ] = FNMS( KP951056516, T3e, T3d );
                cr[ WS( rs, 6 ) ] = FMA( KP951056516, T3e, T3d );
                T35 = FNMS( KP559016994, T34, T33 );
                ci[ WS( rs, 2 ) ] = FNMS( KP951056516, T3c, T35 );
                cr[ WS( rs, 3 ) ] = FMA( KP951056516, T3c, T35 );
            }
            {
                E T30, T32, T1H, T2c, T2R, T2S, T31, T2T;
                {
                    E T2W, T2Z, T1W, T2b;
                    T2W = T2U - T2V;
                    T2Z = T2X - T2Y;
                    T30 = FMA( KP618033988, T2Z, T2W );
                    T32 = FNMS( KP618033988, T2W, T2Z );
                    T1H = FNMS( KP866025403, T1G, T1B );
                    T1W = T1O + T1V;
                    T2b = T23 + T2a;
                    T2c = T1W + T2b;
                    T2R = FNMS( KP250000000, T2c, T1H );
                    T2S = T1W - T2b;
                }
                cr[ WS( rs, 5 ) ] = T1H + T2c;
                T31 = FNMS( KP559016994, T2S, T2R );
                cr[ WS( rs, 2 ) ] = FNMS( KP951056516, T32, T31 );
                ci[ WS( rs, 6 ) ] = FMA( KP951056516, T32, T31 );
                T2T = FMA( KP559016994, T2S, T2R );
                ci[ 0 ] = FNMS( KP951056516, T30, T2T );
                ci[ WS( rs, 3 ) ] = FMA( KP951056516, T30, T2T );
            }
            {
                E T2O, T2Q, T2d, T2k, T2l, T2m, T2n, T2P;
                {
                    E T2A, T2N, T2g, T2j;
                    T2A = T2t - T2z;
                    T2N = T2G - T2M;
                    T2O = FMA( KP618033988, T2N, T2A );
                    T2Q = FNMS( KP618033988, T2A, T2N );
                    T2d = FMA( KP866025403, T1G, T1B );
                    T2g = T2e + T2f;
                    T2j = T2h + T2i;
                    T2k = T2g + T2j;
                    T2l = FNMS( KP250000000, T2k, T2d );
                    T2m = T2g - T2j;
                }
                ci[ WS( rs, 4 ) ] = T2d + T2k;
                T2n = FMA( KP559016994, T2m, T2l );
                cr[ WS( rs, 4 ) ] = FNMS( KP951056516, T2O, T2n );
                cr[ WS( rs, 1 ) ] = FMA( KP951056516, T2O, T2n );
                T2P = FNMS( KP559016994, T2m, T2l );
                cr[ WS( rs, 7 ) ] = FNMS( KP951056516, T2Q, T2P );
                ci[ WS( rs, 1 ) ] = FMA( KP951056516, T2Q, T2P );
            }
            {
                E T3s, T3u, T3m, T3h, T3n, T3o, T3t, T3p;
                {
                    E T3q, T3r, T3f, T3g;
                    T3q = T2h - T2i;
                    T3r = T2e - T2f;
                    T3s = FNMS( KP618033988, T3r, T3q );
                    T3u = FMA( KP618033988, T3q, T3r );
                    T3m = FMA( KP866025403, T3l, T3k );
                    T3f = T2t + T2z;
                    T3g = T2G + T2M;
                    T3h = T3f + T3g;
                    T3n = FNMS( KP250000000, T3h, T3m );
                    T3o = T3f - T3g;
                }
                cr[ WS( rs, 10 ) ] = -( T3h + T3m );
                T3t = FMA( KP559016994, T3o, T3n );
                ci[ WS( rs, 10 ) ] = FMA( KP951056516, T3u, T3t );
                ci[ WS( rs, 13 ) ] = FNMS( KP951056516, T3u, T3t );
                T3p = FNMS( KP559016994, T3o, T3n );
                cr[ WS( rs, 13 ) ] = FMS( KP951056516, T3s, T3p );
                ci[ WS( rs, 7 ) ] = FMA( KP951056516, T3s, T3p );
            }
            {
                E T3Q, T3S, T3H, T3K, T3L, T3M, T3R, T3N;
                {
                    E T3O, T3P, T3I, T3J;
                    T3O = TT - Tz;
                    T3P = T1y - T1e;
                    T3Q = FMA( KP618033988, T3P, T3O );
                    T3S = FNMS( KP618033988, T3O, T3P );
                    T3H = T3j + T3i;
                    T3I = T39 + T3a;
                    T3J = T36 + T37;
                    T3K = T3I + T3J;
                    T3L = FNMS( KP250000000, T3K, T3H );
                    T3M = T3I - T3J;
                }
                ci[ WS( rs, 14 ) ] = T3K + T3H;
                T3R = FNMS( KP559016994, T3M, T3L );
                cr[ WS( rs, 12 ) ] = FMS( KP951056516, T3S, T3R );
                ci[ WS( rs, 11 ) ] = FMA( KP951056516, T3S, T3R );
                T3N = FMA( KP559016994, T3M, T3L );
                cr[ WS( rs, 9 ) ] = FMS( KP951056516, T3Q, T3N );
                ci[ WS( rs, 8 ) ] = FMA( KP951056516, T3Q, T3N );
            }
            {
                E T3E, T3G, T3v, T3y, T3z, T3A, T3F, T3B;
                {
                    E T3C, T3D, T3w, T3x;
                    T3C = T1O - T1V;
                    T3D = T23 - T2a;
                    T3E = FMA( KP618033988, T3D, T3C );
                    T3G = FNMS( KP618033988, T3C, T3D );
                    T3v = FNMS( KP866025403, T3l, T3k );
                    T3w = T2U + T2V;
                    T3x = T2X + T2Y;
                    T3y = T3w + T3x;
                    T3z = FNMS( KP250000000, T3y, T3v );
                    T3A = T3x - T3w;
                }
                ci[ WS( rs, 9 ) ] = T3y + T3v;
                T3F = FMA( KP559016994, T3A, T3z );
                cr[ WS( rs, 8 ) ] = FMS( KP951056516, T3G, T3F );
                ci[ WS( rs, 12 ) ] = FMA( KP951056516, T3G, T3F );
                T3B = FNMS( KP559016994, T3A, T3z );
                cr[ WS( rs, 11 ) ] = FMS( KP951056516, T3E, T3B );
                cr[ WS( rs, 14 ) ] = -( FMA( KP951056516, T3E, T3B ) );
            }
        }
    }
}

static const tw_instr twinstr[] = {{TW_FULL, 1, 15}, {TW_NEXT, 1, 0}};

static const hc2hc_desc desc = {15, "hf_15", twinstr, &GENUS, {72, 28, 112, 0}};

void X( codelet_hf_15 )( planner* p )
{
    X( khc2hc_register )( p, hf_15, &desc );
}
#else

/* Generated by: ../../../genfft/gen_hc2hc.native -compact -variables 4 -pipeline-latency 4 -n 15
 * -dit -name hf_15 -include rdft/scalar/hf.h */

/*
 * This function contains 184 FP additions, 112 FP multiplications,
 * (or, 128 additions, 56 multiplications, 56 fused multiply/add),
 * 65 stack variables, 6 constants, and 60 memory accesses
 */
#include "rdft/scalar/hf.h"

static void hf_15( R* cr, R* ci, const R* W, stride rs, INT mb, INT me, INT ms )
{
    DK( KP587785252, +0.587785252292473129168705954639072768597652438 );
    DK( KP951056516, +0.951056516295153572116439333379382143405698634 );
    DK( KP250000000, +0.250000000000000000000000000000000000000000000 );
    DK( KP559016994, +0.559016994374947424102293417182819058860154590 );
    DK( KP500000000, +0.500000000000000000000000000000000000000000000 );
    DK( KP866025403, +0.866025403784438646763723170752936183471402627 );
    {
        INT m;
        for ( m = mb, W = W + ( ( mb - 1 ) * 28 ); m < me;
              m = m + 1, cr = cr + ms, ci = ci - ms, W = W + 28, MAKE_VOLATILE_STRIDE( 30, rs ) )
        {
            E T1q, T2Q, Td, T1n, T2T, T3l, T13, T1k, T1l, T2E, T2F, T3j, T1H, T1T, T2k;
            E T2w, T2f, T2v, T1M, T1U, Tu, TL, TM, T2H, T2I, T3i, T1w, T1Q, T29, T2t;
            E T24, T2s, T1B, T1R;
            {
                E T1, T2R, T6, T1o, Tb, T1p, Tc, T2S;
                T1 = cr[ 0 ];
                T2R = ci[ 0 ];
                {
                    E T3, T5, T2, T4;
                    T3 = cr[ WS( rs, 5 ) ];
                    T5 = ci[ WS( rs, 5 ) ];
                    T2 = W[ 8 ];
                    T4 = W[ 9 ];
                    T6 = FMA( T2, T3, T4 * T5 );
                    T1o = FNMS( T4, T3, T2 * T5 );
                }
                {
                    E T8, Ta, T7, T9;
                    T8 = cr[ WS( rs, 10 ) ];
                    Ta = ci[ WS( rs, 10 ) ];
                    T7 = W[ 18 ];
                    T9 = W[ 19 ];
                    Tb = FMA( T7, T8, T9 * Ta );
                    T1p = FNMS( T9, T8, T7 * Ta );
                }
                T1q = KP866025403 * ( T1o - T1p );
                T2Q = KP866025403 * ( Tb - T6 );
                Tc = T6 + Tb;
                Td = T1 + Tc;
                T1n = FNMS( KP500000000, Tc, T1 );
                T2S = T1o + T1p;
                T2T = FNMS( KP500000000, T2S, T2R );
                T3l = T2S + T2R;
            }
            {
                E TR, T2c, T18, T2h, TW, T1E, T11, T1F, T12, T2d, T1d, T1J, T1i, T1K, T1j;
                E T2i;
                {
                    E TO, TQ, TN, TP;
                    TO = cr[ WS( rs, 6 ) ];
                    TQ = ci[ WS( rs, 6 ) ];
                    TN = W[ 10 ];
                    TP = W[ 11 ];
                    TR = FMA( TN, TO, TP * TQ );
                    T2c = FNMS( TP, TO, TN * TQ );
                }
                {
                    E T15, T17, T14, T16;
                    T15 = cr[ WS( rs, 9 ) ];
                    T17 = ci[ WS( rs, 9 ) ];
                    T14 = W[ 16 ];
                    T16 = W[ 17 ];
                    T18 = FMA( T14, T15, T16 * T17 );
                    T2h = FNMS( T16, T15, T14 * T17 );
                }
                {
                    E TT, TV, TS, TU;
                    TT = cr[ WS( rs, 11 ) ];
                    TV = ci[ WS( rs, 11 ) ];
                    TS = W[ 20 ];
                    TU = W[ 21 ];
                    TW = FMA( TS, TT, TU * TV );
                    T1E = FNMS( TU, TT, TS * TV );
                }
                {
                    E TY, T10, TX, TZ;
                    TY = cr[ WS( rs, 1 ) ];
                    T10 = ci[ WS( rs, 1 ) ];
                    TX = W[ 0 ];
                    TZ = W[ 1 ];
                    T11 = FMA( TX, TY, TZ * T10 );
                    T1F = FNMS( TZ, TY, TX * T10 );
                }
                T12 = TW + T11;
                T2d = T1E + T1F;
                {
                    E T1a, T1c, T19, T1b;
                    T1a = cr[ WS( rs, 14 ) ];
                    T1c = ci[ WS( rs, 14 ) ];
                    T19 = W[ 26 ];
                    T1b = W[ 27 ];
                    T1d = FMA( T19, T1a, T1b * T1c );
                    T1J = FNMS( T1b, T1a, T19 * T1c );
                }
                {
                    E T1f, T1h, T1e, T1g;
                    T1f = cr[ WS( rs, 4 ) ];
                    T1h = ci[ WS( rs, 4 ) ];
                    T1e = W[ 6 ];
                    T1g = W[ 7 ];
                    T1i = FMA( T1e, T1f, T1g * T1h );
                    T1K = FNMS( T1g, T1f, T1e * T1h );
                }
                T1j = T1d + T1i;
                T2i = T1J + T1K;
                {
                    E T1D, T1G, T2g, T2j;
                    T13 = TR + T12;
                    T1k = T18 + T1j;
                    T1l = T13 + T1k;
                    T2E = T2c + T2d;
                    T2F = T2h + T2i;
                    T3j = T2E + T2F;
                    T1D = FNMS( KP500000000, T12, TR );
                    T1G = KP866025403 * ( T1E - T1F );
                    T1H = T1D - T1G;
                    T1T = T1D + T1G;
                    T2g = KP866025403 * ( T1d - T1i );
                    T2j = FNMS( KP500000000, T2i, T2h );
                    T2k = T2g - T2j;
                    T2w = T2g + T2j;
                    {
                        E T2b, T2e, T1I, T1L;
                        T2b = KP866025403 * ( T11 - TW );
                        T2e = FNMS( KP500000000, T2d, T2c );
                        T2f = T2b + T2e;
                        T2v = T2e - T2b;
                        T1I = FNMS( KP500000000, T1j, T18 );
                        T1L = KP866025403 * ( T1J - T1K );
                        T1M = T1I - T1L;
                        T1U = T1I + T1L;
                    }
                }
            }
            {
                E Ti, T21, Tz, T26, Tn, T1t, Ts, T1u, Tt, T22, TE, T1y, TJ, T1z, TK;
                E T27;
                {
                    E Tf, Th, Te, Tg;
                    Tf = cr[ WS( rs, 3 ) ];
                    Th = ci[ WS( rs, 3 ) ];
                    Te = W[ 4 ];
                    Tg = W[ 5 ];
                    Ti = FMA( Te, Tf, Tg * Th );
                    T21 = FNMS( Tg, Tf, Te * Th );
                }
                {
                    E Tw, Ty, Tv, Tx;
                    Tw = cr[ WS( rs, 12 ) ];
                    Ty = ci[ WS( rs, 12 ) ];
                    Tv = W[ 22 ];
                    Tx = W[ 23 ];
                    Tz = FMA( Tv, Tw, Tx * Ty );
                    T26 = FNMS( Tx, Tw, Tv * Ty );
                }
                {
                    E Tk, Tm, Tj, Tl;
                    Tk = cr[ WS( rs, 8 ) ];
                    Tm = ci[ WS( rs, 8 ) ];
                    Tj = W[ 14 ];
                    Tl = W[ 15 ];
                    Tn = FMA( Tj, Tk, Tl * Tm );
                    T1t = FNMS( Tl, Tk, Tj * Tm );
                }
                {
                    E Tp, Tr, To, Tq;
                    Tp = cr[ WS( rs, 13 ) ];
                    Tr = ci[ WS( rs, 13 ) ];
                    To = W[ 24 ];
                    Tq = W[ 25 ];
                    Ts = FMA( To, Tp, Tq * Tr );
                    T1u = FNMS( Tq, Tp, To * Tr );
                }
                Tt = Tn + Ts;
                T22 = T1t + T1u;
                {
                    E TB, TD, TA, TC;
                    TB = cr[ WS( rs, 2 ) ];
                    TD = ci[ WS( rs, 2 ) ];
                    TA = W[ 2 ];
                    TC = W[ 3 ];
                    TE = FMA( TA, TB, TC * TD );
                    T1y = FNMS( TC, TB, TA * TD );
                }
                {
                    E TG, TI, TF, TH;
                    TG = cr[ WS( rs, 7 ) ];
                    TI = ci[ WS( rs, 7 ) ];
                    TF = W[ 12 ];
                    TH = W[ 13 ];
                    TJ = FMA( TF, TG, TH * TI );
                    T1z = FNMS( TH, TG, TF * TI );
                }
                TK = TE + TJ;
                T27 = T1y + T1z;
                {
                    E T1s, T1v, T25, T28;
                    Tu = Ti + Tt;
                    TL = Tz + TK;
                    TM = Tu + TL;
                    T2H = T21 + T22;
                    T2I = T26 + T27;
                    T3i = T2H + T2I;
                    T1s = FNMS( KP500000000, Tt, Ti );
                    T1v = KP866025403 * ( T1t - T1u );
                    T1w = T1s - T1v;
                    T1Q = T1s + T1v;
                    T25 = KP866025403 * ( TJ - TE );
                    T28 = FNMS( KP500000000, T27, T26 );
                    T29 = T25 + T28;
                    T2t = T28 - T25;
                    {
                        E T20, T23, T1x, T1A;
                        T20 = KP866025403 * ( Ts - Tn );
                        T23 = FNMS( KP500000000, T22, T21 );
                        T24 = T20 + T23;
                        T2s = T23 - T20;
                        T1x = FNMS( KP500000000, TK, Tz );
                        T1A = KP866025403 * ( T1y - T1z );
                        T1B = T1x - T1A;
                        T1R = T1x + T1A;
                    }
                }
            }
            {
                E T2C, T1m, T2B, T2K, T2M, T2G, T2J, T2L, T2D;
                T2C = KP559016994 * ( TM - T1l );
                T1m = TM + T1l;
                T2B = FNMS( KP250000000, T1m, Td );
                T2G = T2E - T2F;
                T2J = T2H - T2I;
                T2K = FNMS( KP587785252, T2J, KP951056516 * T2G );
                T2M = FMA( KP951056516, T2J, KP587785252 * T2G );
                cr[ 0 ] = Td + T1m;
                T2L = T2C + T2B;
                ci[ WS( rs, 5 ) ] = T2L - T2M;
                cr[ WS( rs, 6 ) ] = T2L + T2M;
                T2D = T2B - T2C;
                ci[ WS( rs, 2 ) ] = T2D - T2K;
                cr[ WS( rs, 3 ) ] = T2D + T2K;
            }
            {
                E T3k, T3m, T3n, T3h, T3p, T3f, T3g, T3q, T3o;
                T3k = KP559016994 * ( T3i - T3j );
                T3m = T3i + T3j;
                T3n = FNMS( KP250000000, T3m, T3l );
                T3f = T1k - T13;
                T3g = Tu - TL;
                T3h = FNMS( KP951056516, T3g, KP587785252 * T3f );
                T3p = FMA( KP587785252, T3g, KP951056516 * T3f );
                ci[ WS( rs, 14 ) ] = T3m + T3l;
                T3q = T3n - T3k;
                cr[ WS( rs, 12 ) ] = T3p - T3q;
                ci[ WS( rs, 11 ) ] = T3p + T3q;
                T3o = T3k + T3n;
                cr[ WS( rs, 9 ) ] = T3h - T3o;
                ci[ WS( rs, 8 ) ] = T3h + T3o;
            }
            {
                E T2y, T2A, T1r, T1O, T2p, T2q, T2z, T2r;
                {
                    E T2u, T2x, T1C, T1N;
                    T2u = T2s - T2t;
                    T2x = T2v - T2w;
                    T2y = FMA( KP951056516, T2u, KP587785252 * T2x );
                    T2A = FNMS( KP587785252, T2u, KP951056516 * T2x );
                    T1r = T1n - T1q;
                    T1C = T1w + T1B;
                    T1N = T1H + T1M;
                    T1O = T1C + T1N;
                    T2p = KP559016994 * ( T1C - T1N );
                    T2q = FNMS( KP250000000, T1O, T1r );
                }
                cr[ WS( rs, 5 ) ] = T1r + T1O;
                T2z = T2q - T2p;
                cr[ WS( rs, 2 ) ] = T2z - T2A;
                ci[ WS( rs, 6 ) ] = T2z + T2A;
                T2r = T2p + T2q;
                ci[ 0 ] = T2r - T2y;
                ci[ WS( rs, 3 ) ] = T2r + T2y;
            }
            {
                E T35, T3d, T39, T3a, T38, T3b, T3e, T3c;
                {
                    E T33, T34, T36, T37;
                    T33 = T1w - T1B;
                    T34 = T1H - T1M;
                    T35 = FMA( KP951056516, T33, KP587785252 * T34 );
                    T3d = FNMS( KP587785252, T33, KP951056516 * T34 );
                    T39 = T2T - T2Q;
                    T36 = T2v + T2w;
                    T37 = T2s + T2t;
                    T3a = T37 + T36;
                    T38 = KP559016994 * ( T36 - T37 );
                    T3b = FNMS( KP250000000, T3a, T39 );
                }
                ci[ WS( rs, 9 ) ] = T3a + T39;
                T3e = T38 + T3b;
                cr[ WS( rs, 8 ) ] = T3d - T3e;
                ci[ WS( rs, 12 ) ] = T3d + T3e;
                T3c = T38 - T3b;
                cr[ WS( rs, 11 ) ] = T35 + T3c;
                cr[ WS( rs, 14 ) ] = T3c - T35;
            }
            {
                E T2X, T31, T2U, T2P, T2Y, T2Z, T32, T30;
                {
                    E T2V, T2W, T2N, T2O;
                    T2V = T1T - T1U;
                    T2W = T1Q - T1R;
                    T2X = FNMS( KP587785252, T2W, KP951056516 * T2V );
                    T31 = FMA( KP951056516, T2W, KP587785252 * T2V );
                    T2U = T2Q + T2T;
                    T2N = T2k - T2f;
                    T2O = T24 + T29;
                    T2P = T2N - T2O;
                    T2Y = FMA( KP250000000, T2P, T2U );
                    T2Z = KP559016994 * ( T2O + T2N );
                }
                cr[ WS( rs, 10 ) ] = T2P - T2U;
                T32 = T2Z + T2Y;
                ci[ WS( rs, 10 ) ] = T31 + T32;
                ci[ WS( rs, 13 ) ] = T32 - T31;
                T30 = T2Y - T2Z;
                cr[ WS( rs, 13 ) ] = T2X - T30;
                ci[ WS( rs, 7 ) ] = T2X + T30;
            }
            {
                E T2m, T2o, T1P, T1W, T1X, T1Y, T1Z, T2n;
                {
                    E T2a, T2l, T1S, T1V;
                    T2a = T24 - T29;
                    T2l = T2f + T2k;
                    T2m = FMA( KP951056516, T2a, KP587785252 * T2l );
                    T2o = FNMS( KP587785252, T2a, KP951056516 * T2l );
                    T1P = T1n + T1q;
                    T1S = T1Q + T1R;
                    T1V = T1T + T1U;
                    T1W = T1S + T1V;
                    T1X = KP559016994 * ( T1S - T1V );
                    T1Y = FNMS( KP250000000, T1W, T1P );
                }
                ci[ WS( rs, 4 ) ] = T1P + T1W;
                T1Z = T1X + T1Y;
                cr[ WS( rs, 4 ) ] = T1Z - T2m;
                cr[ WS( rs, 1 ) ] = T1Z + T2m;
                T2n = T1Y - T1X;
                cr[ WS( rs, 7 ) ] = T2n - T2o;
                ci[ WS( rs, 1 ) ] = T2n + T2o;
            }
        }
    }
}

static const tw_instr twinstr[] = {{TW_FULL, 1, 15}, {TW_NEXT, 1, 0}};

static const hc2hc_desc desc = {15, "hf_15", twinstr, &GENUS, {128, 56, 56, 0}};

void X( codelet_hf_15 )( planner* p )
{
    X( khc2hc_register )( p, hf_15, &desc );
}
#endif
