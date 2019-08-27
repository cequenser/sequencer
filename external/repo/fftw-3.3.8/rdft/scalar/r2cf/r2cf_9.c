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
/* Generated on Thu May 24 08:06:26 EDT 2018 */

#include "rdft/codelet-rdft.h"

#if defined( ARCH_PREFERS_FMA ) || defined( ISA_EXTENSION_PREFERS_FMA )

/* Generated by: ../../../genfft/gen_r2cf.native -fma -compact -variables 4 -pipeline-latency 4 -n 9
 * -name r2cf_9 -include rdft/scalar/r2cf.h */

/*
 * This function contains 38 FP additions, 30 FP multiplications,
 * (or, 12 additions, 4 multiplications, 26 fused multiply/add),
 * 48 stack variables, 18 constants, and 18 memory accesses
 */
#include "rdft/scalar/r2cf.h"

static void r2cf_9( R* R0, R* R1, R* Cr, R* Ci, stride rs, stride csr, stride csi, INT v, INT ivs,
                    INT ovs )
{
    DK( KP907603734, +0.907603734547952313649323976213898122064543220 );
    DK( KP347296355, +0.347296355333860697703433253538629592000751354 );
    DK( KP852868531, +0.852868531952443209628250963940074071936020296 );
    DK( KP666666666, +0.666666666666666666666666666666666666666666667 );
    DK( KP898197570, +0.898197570222573798468955502359086394667167570 );
    DK( KP673648177, +0.673648177666930348851716626769314796000375677 );
    DK( KP879385241, +0.879385241571816768108218554649462939872416269 );
    DK( KP984807753, +0.984807753012208059366743024589523013670643252 );
    DK( KP939692620, +0.939692620785908384054109277324731469936208134 );
    DK( KP394930843, +0.394930843634698457567117349190734585290304520 );
    DK( KP866025403, +0.866025403784438646763723170752936183471402627 );
    DK( KP586256827, +0.586256827714544512072145703099641959914944179 );
    DK( KP726681596, +0.726681596905677465811651808188092531873167623 );
    DK( KP968908795, +0.968908795874236621082202410917456709164223497 );
    DK( KP203604859, +0.203604859554852403062088995281827210665664861 );
    DK( KP152703644, +0.152703644666139302296566746461370407999248646 );
    DK( KP500000000, +0.500000000000000000000000000000000000000000000 );
    DK( KP184792530, +0.184792530904095372701352047572203755870913560 );
    {
        INT i;
        for ( i = v; i > 0; i = i - 1, R0 = R0 + ivs, R1 = R1 + ivs, Cr = Cr + ovs, Ci = Ci + ovs,
              MAKE_VOLATILE_STRIDE( 36, rs ), MAKE_VOLATILE_STRIDE( 36, csr ),
              MAKE_VOLATILE_STRIDE( 36, csi ) )
        {
            E T1, T4, To, Tk, Ta, Tu, Tf, Th, Tj, Tx, Tl, Tm, Ty, Tq, T2;
            E T3, T5, Tg;
            T1 = R0[ 0 ];
            T2 = R1[ WS( rs, 1 ) ];
            T3 = R0[ WS( rs, 3 ) ];
            T4 = T2 + T3;
            To = T3 - T2;
            {
                E T6, Tb, T9, Te, Ti;
                T6 = R1[ 0 ];
                Tb = R0[ WS( rs, 1 ) ];
                {
                    E T7, T8, Tc, Td;
                    T7 = R0[ WS( rs, 2 ) ];
                    T8 = R1[ WS( rs, 3 ) ];
                    T9 = T7 + T8;
                    Tk = T7 - T8;
                    Tc = R1[ WS( rs, 2 ) ];
                    Td = R0[ WS( rs, 4 ) ];
                    Te = Tc + Td;
                    Ti = Td - Tc;
                }
                Ta = T6 + T9;
                Tu = FMA( KP184792530, Tk, Ti );
                Tf = Tb + Te;
                Th = FNMS( KP500000000, Te, Tb );
                Tj = FNMS( KP152703644, Ti, Th );
                Tx = FMA( KP203604859, Th, Ti );
                Tl = FMS( KP500000000, T9, T6 );
                Tm = FNMS( KP968908795, Tl, Tk );
                Ty = FMA( KP726681596, Tk, Tl );
                Tq = FMA( KP586256827, Tl, Ti );
            }
            Ci[ WS( csi, 3 ) ] = KP866025403 * ( Tf - Ta );
            T5 = T1 + T4;
            Tg = Ta + Tf;
            Cr[ WS( csr, 3 ) ] = FNMS( KP500000000, Tg, T5 );
            Cr[ 0 ] = T5 + Tg;
            {
                E Tv, Tt, Tn, TC, TB;
                Tt = FMA( KP394930843, Th, To );
                Tv = FNMS( KP939692620, Tu, Tt );
                Ci[ WS( csi, 2 ) ] = KP984807753 * ( FNMS( KP879385241, Tv, Tl ) );
                Tn = FMA( KP673648177, Tm, Tj );
                TB = FMA( KP898197570, Ty, Tx );
                TC = FMA( KP666666666, Tn, TB );
                Ci[ WS( csi, 1 ) ] = -( KP984807753 * ( FNMS( KP879385241, To, Tn ) ) );
                Ci[ WS( csi, 4 ) ] = KP866025403 * ( FMA( KP852868531, TC, To ) );
                {
                    E Tp, Ts, Tz, TA, Tr, Tw;
                    Tp = FNMS( KP500000000, T4, T1 );
                    Tr = FNMS( KP347296355, Tq, Tk );
                    Ts = FNMS( KP907603734, Tr, Th );
                    Tw = FNMS( KP673648177, Tm, Tj );
                    Tz = FNMS( KP898197570, Ty, Tx );
                    TA = FNMS( KP500000000, Tz, Tw );
                    Cr[ WS( csr, 2 ) ] = FNMS( KP939692620, Ts, Tp );
                    Cr[ WS( csr, 1 ) ] = FMA( KP852868531, Tz, Tp );
                    Cr[ WS( csr, 4 ) ] = FMA( KP852868531, TA, Tp );
                }
            }
        }
    }
}

static const kr2c_desc desc = {9, "r2cf_9", {12, 4, 26, 0}, &GENUS};

void X( codelet_r2cf_9 )( planner* p )
{
    X( kr2c_register )( p, r2cf_9, &desc );
}

#else

/* Generated by: ../../../genfft/gen_r2cf.native -compact -variables 4 -pipeline-latency 4 -n 9
 * -name r2cf_9 -include rdft/scalar/r2cf.h */

/*
 * This function contains 38 FP additions, 26 FP multiplications,
 * (or, 21 additions, 9 multiplications, 17 fused multiply/add),
 * 36 stack variables, 14 constants, and 18 memory accesses
 */
#include "rdft/scalar/r2cf.h"

static void r2cf_9( R* R0, R* R1, R* Cr, R* Ci, stride rs, stride csr, stride csi, INT v, INT ivs,
                    INT ovs )
{
    DK( KP939692620, +0.939692620785908384054109277324731469936208134 );
    DK( KP296198132, +0.296198132726023843175338011893050938967728390 );
    DK( KP342020143, +0.342020143325668733044099614682259580763083368 );
    DK( KP813797681, +0.813797681349373692844693217248393223289101568 );
    DK( KP984807753, +0.984807753012208059366743024589523013670643252 );
    DK( KP150383733, +0.150383733180435296639271897612501926072238258 );
    DK( KP642787609, +0.642787609686539326322643409907263432907559884 );
    DK( KP663413948, +0.663413948168938396205421319635891297216863310 );
    DK( KP852868531, +0.852868531952443209628250963940074071936020296 );
    DK( KP173648177, +0.173648177666930348851716626769314796000375677 );
    DK( KP556670399, +0.556670399226419366452912952047023132968291906 );
    DK( KP766044443, +0.766044443118978035202392650555416673935832457 );
    DK( KP866025403, +0.866025403784438646763723170752936183471402627 );
    DK( KP500000000, +0.500000000000000000000000000000000000000000000 );
    {
        INT i;
        for ( i = v; i > 0; i = i - 1, R0 = R0 + ivs, R1 = R1 + ivs, Cr = Cr + ovs, Ci = Ci + ovs,
              MAKE_VOLATILE_STRIDE( 36, rs ), MAKE_VOLATILE_STRIDE( 36, csr ),
              MAKE_VOLATILE_STRIDE( 36, csi ) )
        {
            E T1, T4, Tr, Ta, Tl, Ti, Tf, Tk, Tj, T2, T3, T5, Tg;
            T1 = R0[ 0 ];
            T2 = R1[ WS( rs, 1 ) ];
            T3 = R0[ WS( rs, 3 ) ];
            T4 = T2 + T3;
            Tr = T3 - T2;
            {
                E T6, T7, T8, T9;
                T6 = R1[ 0 ];
                T7 = R0[ WS( rs, 2 ) ];
                T8 = R1[ WS( rs, 3 ) ];
                T9 = T7 + T8;
                Ta = T6 + T9;
                Tl = T8 - T7;
                Ti = FNMS( KP500000000, T9, T6 );
            }
            {
                E Tb, Tc, Td, Te;
                Tb = R0[ WS( rs, 1 ) ];
                Tc = R1[ WS( rs, 2 ) ];
                Td = R0[ WS( rs, 4 ) ];
                Te = Tc + Td;
                Tf = Tb + Te;
                Tk = FNMS( KP500000000, Te, Tb );
                Tj = Td - Tc;
            }
            Ci[ WS( csi, 3 ) ] = KP866025403 * ( Tf - Ta );
            T5 = T1 + T4;
            Tg = Ta + Tf;
            Cr[ WS( csr, 3 ) ] = FNMS( KP500000000, Tg, T5 );
            Cr[ 0 ] = T5 + Tg;
            {
                E Tt, Th, Tm, Tn, To, Tp, Tq, Ts;
                Tt = KP866025403 * Tr;
                Th = FNMS( KP500000000, T4, T1 );
                Tm = FMA( KP766044443, Ti, KP556670399 * Tl );
                Tn = FMA( KP173648177, Tk, KP852868531 * Tj );
                To = Tm + Tn;
                Tp = FNMS( KP642787609, Ti, KP663413948 * Tl );
                Tq = FNMS( KP984807753, Tk, KP150383733 * Tj );
                Ts = Tp + Tq;
                Cr[ WS( csr, 1 ) ] = Th + To;
                Ci[ WS( csi, 1 ) ] = Tt + Ts;
                Cr[ WS( csr, 4 ) ] = FMA( KP866025403, Tp - Tq, Th ) - ( KP500000000 * To );
                Ci[ WS( csi, 4 ) ] = FNMS( KP500000000, Ts, KP866025403 * ( Tr + ( Tn - Tm ) ) );
                Ci[ WS( csi, 2 ) ] = FNMS( KP342020143, Tk, KP813797681 * Tj ) +
                                     FNMA( KP150383733, Tl, KP984807753 * Ti ) - Tt;
                Cr[ WS( csr, 2 ) ] = FMA( KP173648177, Ti, Th ) +
                                     FNMA( KP296198132, Tj, KP939692620 * Tk ) -
                                     ( KP852868531 * Tl );
            }
        }
    }
}

static const kr2c_desc desc = {9, "r2cf_9", {21, 9, 17, 0}, &GENUS};

void X( codelet_r2cf_9 )( planner* p )
{
    X( kr2c_register )( p, r2cf_9, &desc );
}

#endif
