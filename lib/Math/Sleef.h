#pragma once
//https://github.com/shibatch/sleef/blob/7df8e36/src/libm/sleefsp.c
//https://github.com/shibatch/sleef/blob/7df8e36/src/libm/sleefdp.c

// Copyright Naoki Shibata and contributors 2010 - 2021. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Always use -ffp-contract=off option to compile SLEEF.

#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include "../Util/Assert.h"
#include "../Util/BitCast.h"
#include "../Util/Inline.h"
#include "../Util/LikelyUnlikely.h"
#include "../Util/Pure.h"
#include "MathConstant.h"
#include "Numeric/Signbit.h"
#include "Numeric/Sqrt.h"
#include "SinCos.h"

#if !defined(SLEEF_GENHEADER)
	#ifndef SLEEF_FP_ILOGB0
		#define SLEEF_FP_ILOGB0 ((i32)0x80000000)
	#endif

	#ifndef SLEEF_FP_ILOGBNAN
		#define SLEEF_FP_ILOGBNAN ((i32)2147483647)
	#endif
#endif

#define SLEEF_FLT_MIN 0x1p-126
#define SLEEF_DBL_MIN 0x1p-1022
#define SLEEF_INT_MAX 2147483647
#define SLEEF_DBL_DENORM_MIN 4.9406564584124654e-324
#define SLEEF_FLT_DENORM_MIN 1.40129846e-45F

//PI_A to PI_D are constants that satisfy the following two conditions.
//* For PI_A, PI_B and PI_C, the last 28 bits are zero.
//* PI_A + PI_B + PI_C + PI_D is close to PI as much as possible.
//The argument of a trig function is multiplied by 1/PI, and the
//integral part is divided into two parts, each has at most 28
//bits. So, the maximum argument that could be correctly reduced
//should be 2^(28*2-1) PI = 1.1e+17. However, due to internal
//f64 precision calculation, the actual maximum argument that can
//be correctly reduced is around 2^47.

#define PI_A 3.1415926218032836914
#define PI_B 3.1786509424591713469e-08
#define PI_C 1.2246467864107188502e-16
#define PI_D 1.2736634327021899816e-24
#define TRIGRANGEMAX 1e+14

//PI_A2 and PI_B2 are constants that satisfy the following two conditions.
//* The last 3 bits of PI_A2 are zero.
//* PI_A2 + PI_B2 is close to PI as much as possible.
//The argument of a trig function is multiplied by 1/PI, and the
//integral part is multiplied by PI_A2. So, the maximum argument that
//could be correctly reduced should be 2^(3-1) PI = 12.6. By testing,
//we confirmed that it correctly reduces the argument up to around 15.

#define PI_A2 3.141592653589793116
#define PI_B2 1.2246467991473532072e-16
#define TRIGRANGEMAX2 15

#define M_2_PI_H 0.63661977236758138243
#define M_2_PI_L -3.9357353350364971764e-17

#define SQRT_DBL_MAX 1.3407807929942596355e+154

#define TRIGRANGEMAX3 1e+9

#define M_4_PI 1.273239544735162542821171882678754627704620361328125

#define L2U .69314718055966295651160180568695068359375
#define L2L .28235290563031577122588448175013436025525412068e-12
#define R_LN2 1.442695040888963407359924681001892137426645954152985934135449406931

#define L10U 0.30102999566383914498 // log 2 / log 10
#define L10L 1.4205023227266099418e-13
#define LOG10_2 3.3219280948873623478703194294893901758648313930

#define L10Uf 0.3010253906f
#define L10Lf 4.605038981e-06f

#define PI_Af 3.140625f
#define PI_Bf 0.0009670257568359375f
#define PI_Cf 6.2771141529083251953e-07f
#define PI_Df 1.2154201256553420762e-10f
#define TRIGRANGEMAXf 39000

#define PI_A2f 3.1414794921875f
#define PI_B2f 0.00011315941810607910156f
#define PI_C2f 1.9841872589410058936e-09f
#define TRIGRANGEMAX2f 125.0f

#define TRIGRANGEMAX4f 8e+6f

#define SQRT_FLT_MAX 18446743523953729536.0

#define L2Uf 0.693145751953125f
#define L2Lf 1.428606765330187045e-06f

#define R_LN2f 1.442695040888963407359924681001892137426645954152985934135449406931f
#if defined(M_PIf)
	#undef M_PIf
#endif
#define M_PIf ((f32)Math::pi<f64>)

#if (defined (__GNUC__) || defined (__clang__) || defined(__INTEL_COMPILER)) && !defined(_MSC_VER)
	#define SLEEF_NAN __builtin_nan("")
	#define SLEEF_NANf __builtin_nanf("")
	#define SLEEF_NANl __builtin_nanl("")
	#define SLEEF_INFINITY __builtin_inf()
	#define SLEEF_INFINITYf __builtin_inff()
	#define SLEEF_INFINITYl __builtin_infl()

	#if defined(__INTEL_COMPILER) || defined (__clang__)
		#define SLEEF_INFINITYq __builtin_inf()
		#define SLEEF_NANq __builtin_nan("")
	#else
		#define SLEEF_INFINITYq __builtin_infq()
		#define SLEEF_NANq (SLEEF_INFINITYq - SLEEF_INFINITYq)
	#endif
#elif defined(_MSC_VER) // #if (defined (__GNUC__) || defined (__clang__) || defined(__INTEL_COMPILER)) && !defined(_MSC_VER)
	#if (defined(__GNUC__) || defined(__CLANG__)) && (defined(__i386__) || defined(__x86_64__)) && !defined(SLEEF_GENHEADER)
		#include <x86intrin.h>
	#endif

	#define SLEEF_INFINITY (1e+300 * 1e+300)
	#define SLEEF_NAN (SLEEF_INFINITY - SLEEF_INFINITY)
	#define SLEEF_INFINITYf ((f32)SLEEF_INFINITY)
	#define SLEEF_NANf ((f32)SLEEF_NAN)
	#define SLEEF_INFINITYl ((f128)SLEEF_INFINITY)
	#define SLEEF_NANl ((f128)SLEEF_NAN)
#endif // #elif defined(_MSC_VER) // #if (defined (__GNUC__) || defined (__clang__) || defined(__INTEL_COMPILER)) && !defined(_MSC_VER)

#if defined (__GNUC__) && !defined(__INTEL_COMPILER)
	#pragma GCC diagnostic ignored "-Wpragmas"
	#pragma GCC diagnostic ignored "-Wunknown-pragmas"
	#if !defined (__clang__)
		#pragma GCC diagnostic ignored "-Wattribute-alias"
		#pragma GCC diagnostic ignored "-Wlto-type-mismatch"
		#pragma GCC diagnostic ignored "-Wstringop-overflow"
	#endif
#endif

struct Sleef_double2 { f64 x, y; };
struct Sleef_float2 { f32 x, y; };

alignas(64) constexpr f32 Sleef_rempitabsp[] = {
 0.159154892f, 5.112411827e-08f, 3.626141271e-15f, -2.036222915e-22f,
 0.03415493667f, 6.420638243e-09f, 7.342738037e-17f, 8.135951656e-24f,
 0.03415493667f, 6.420638243e-09f, 7.342738037e-17f, 8.135951656e-24f,
 0.002904943191f, -9.861969574e-11f, -9.839336547e-18f, -1.790215892e-24f,
 0.002904943191f, -9.861969574e-11f, -9.839336547e-18f, -1.790215892e-24f,
 0.002904943191f, -9.861969574e-11f, -9.839336547e-18f, -1.790215892e-24f,
 0.002904943191f, -9.861969574e-11f, -9.839336547e-18f, -1.790215892e-24f,
 0.0009518179577f, 1.342109202e-10f, 1.791623576e-17f, 1.518506657e-24f,
 0.0009518179577f, 1.342109202e-10f, 1.791623576e-17f, 1.518506657e-24f,
 0.0004635368241f, 1.779561221e-11f, 4.038449606e-18f, -1.358546052e-25f,
 0.0002193961991f, 1.779561221e-11f, 4.038449606e-18f, -1.358546052e-25f,
 9.73258866e-05f, 1.779561221e-11f, 4.038449606e-18f, -1.358546052e-25f,
 3.62907449e-05f, 3.243700447e-12f, 5.690024473e-19f, 7.09405479e-26f,
 5.773168596e-06f, 1.424711477e-12f, 1.3532163e-19f, 1.92417627e-26f,
 5.773168596e-06f, 1.424711477e-12f, 1.3532163e-19f, 1.92417627e-26f,
 5.773168596e-06f, 1.424711477e-12f, 1.3532163e-19f, 1.92417627e-26f,
 1.958472239e-06f, 5.152167755e-13f, 1.3532163e-19f, 1.92417627e-26f,
 5.112411827e-08f, 3.626141271e-15f, -2.036222915e-22f, 6.177847236e-30f,
 5.112411827e-08f, 3.626141271e-15f, -2.036222915e-22f, 6.177847236e-30f,
 5.112411827e-08f, 3.626141271e-15f, -2.036222915e-22f, 6.177847236e-30f,
 5.112411827e-08f, 3.626141271e-15f, -2.036222915e-22f, 6.177847236e-30f,
 5.112411827e-08f, 3.626141271e-15f, -2.036222915e-22f, 6.177847236e-30f,
 5.112411827e-08f, 3.626141271e-15f, -2.036222915e-22f, 6.177847236e-30f,
 2.132179588e-08f, 3.626141271e-15f, -2.036222915e-22f, 6.177847236e-30f,
 6.420638243e-09f, 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f,
 6.420638243e-09f, 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f,
 2.695347945e-09f, 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f,
 8.327027956e-10f, 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f,
 8.327027956e-10f, 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f,
 3.670415083e-10f, 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f,
 1.342109202e-10f, 1.791623576e-17f, 1.518506361e-24f, 2.613904e-31f,
 1.779561221e-11f, 4.038449606e-18f, -1.358545683e-25f, -3.443243946e-32f,
 1.779561221e-11f, 4.038449606e-18f, -1.358545683e-25f, -3.443243946e-32f,
 1.779561221e-11f, 4.038449606e-18f, -1.358545683e-25f, -3.443243946e-32f,
 3.243700447e-12f, 5.690024473e-19f, 7.094053557e-26f, 1.487136711e-32f,
 3.243700447e-12f, 5.690024473e-19f, 7.094053557e-26f, 1.487136711e-32f,
 3.243700447e-12f, 5.690024473e-19f, 7.094053557e-26f, 1.487136711e-32f,
 1.424711477e-12f, 1.3532163e-19f, 1.924175961e-26f, 2.545416018e-33f,
 5.152167755e-13f, 1.3532163e-19f, 1.924175961e-26f, 2.545416018e-33f,
 6.046956013e-14f, -2.036222915e-22f, 6.177846108e-30f, 1.082084378e-36f,
 6.046956013e-14f, -2.036222915e-22f, 6.177846108e-30f, 1.082084378e-36f,
 6.046956013e-14f, -2.036222915e-22f, 6.177846108e-30f, 1.082084378e-36f,
 3.626141271e-15f, -2.036222915e-22f, 6.177846108e-30f, 1.082084378e-36f,
 3.626141271e-15f, -2.036222915e-22f, 6.177846108e-30f, 1.082084378e-36f,
 3.626141271e-15f, -2.036222915e-22f, 6.177846108e-30f, 1.082084378e-36f,
 3.626141271e-15f, -2.036222915e-22f, 6.177846108e-30f, 1.082084378e-36f,
 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f, 6.296048013e-40f,
 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f, 6.296048013e-40f,
 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f, 6.296048013e-40f,
 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f, 6.296048013e-40f,
 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f, 6.296048013e-40f,
 7.342738037e-17f, 8.135951656e-24f, -1.330400526e-31f, 6.296048013e-40f,
 1.791623576e-17f, 1.518506361e-24f, 2.61390353e-31f, 4.764937743e-38f,
 1.791623576e-17f, 1.518506361e-24f, 2.61390353e-31f, 4.764937743e-38f,
 4.038449606e-18f, -1.358545683e-25f, -3.443243946e-32f, 6.296048013e-40f,
 4.038449606e-18f, -1.358545683e-25f, -3.443243946e-32f, 6.296048013e-40f,
 5.690024473e-19f, 7.094053557e-26f, 1.487136711e-32f, 6.296048013e-40f,
 5.690024473e-19f, 7.094053557e-26f, 1.487136711e-32f, 6.296048013e-40f,
 5.690024473e-19f, 7.094053557e-26f, 1.487136711e-32f, 6.296048013e-40f,
 1.3532163e-19f, 1.924175961e-26f, 2.545415467e-33f, 6.296048013e-40f,
 1.3532163e-19f, 1.924175961e-26f, 2.545415467e-33f, 6.296048013e-40f,
 2.690143217e-20f, -1.452834402e-28f, -6.441077673e-36f, -1.764234767e-42f,
 2.690143217e-20f, -1.452834402e-28f, -6.441077673e-36f, -1.764234767e-42f,
 2.690143217e-20f, -1.452834402e-28f, -6.441077673e-36f, -1.764234767e-42f,
 1.334890502e-20f, -1.452834402e-28f, -6.441077673e-36f, -1.764234767e-42f,
 6.572641438e-21f, -1.452834402e-28f, -6.441077673e-36f, -1.764234767e-42f,
 0.05874381959f, 1.222115387e-08f, 7.693612965e-16f, 1.792054435e-22f,
 0.02749382704f, 4.77057327e-09f, 7.693612965e-16f, 1.792054435e-22f,
 0.01186883077f, 1.045283415e-09f, 3.252721926e-16f, 7.332633139e-23f,
 0.00405633077f, 1.045283415e-09f, 3.252721926e-16f, 7.332633139e-23f,
 0.000150081818f, -2.454155802e-12f, 1.161414894e-20f, 1.291319272e-27f,
 0.000150081818f, -2.454155802e-12f, 1.161414894e-20f, 1.291319272e-27f,
 0.000150081818f, -2.454155802e-12f, 1.161414894e-20f, 1.291319272e-27f,
 0.000150081818f, -2.454155802e-12f, 1.161414894e-20f, 1.291319272e-27f,
 0.000150081818f, -2.454155802e-12f, 1.161414894e-20f, 1.291319272e-27f,
 2.801149822e-05f, 4.821800945e-12f, 8.789757674e-19f, 1.208447639e-25f,
 2.801149822e-05f, 4.821800945e-12f, 8.789757674e-19f, 1.208447639e-25f,
 2.801149822e-05f, 4.821800945e-12f, 8.789757674e-19f, 1.208447639e-25f,
 1.275271279e-05f, 1.183823005e-12f, 1.161414894e-20f, 1.291319272e-27f,
 5.12331826e-06f, 1.183823005e-12f, 1.161414894e-20f, 1.291319272e-27f,
 1.308621904e-06f, 2.743283031e-13f, 1.161414894e-20f, 1.291319272e-27f,
 1.308621904e-06f, 2.743283031e-13f, 1.161414894e-20f, 1.291319272e-27f,
 3.549478151e-07f, 4.695462769e-14f, 1.161414894e-20f, 1.291319272e-27f,
 3.549478151e-07f, 4.695462769e-14f, 1.161414894e-20f, 1.291319272e-27f,
 1.165292645e-07f, 1.853292503e-14f, 4.837885366e-21f, 1.291319272e-27f,
 1.165292645e-07f, 1.853292503e-14f, 4.837885366e-21f, 1.291319272e-27f,
 5.69246339e-08f, 4.322073705e-15f, 1.449754789e-21f, 7.962890365e-29f,
 2.712231151e-08f, 4.322073705e-15f, 1.449754789e-21f, 7.962890365e-29f,
 1.222115387e-08f, 7.693612965e-16f, 1.792054182e-22f, 2.91418027e-29f,
 4.77057327e-09f, 7.693612965e-16f, 1.792054182e-22f, 2.91418027e-29f,
 1.045283415e-09f, 3.252721926e-16f, 7.332632508e-23f, 3.898253736e-30f,
 1.045283415e-09f, 3.252721926e-16f, 7.332632508e-23f, 3.898253736e-30f,
 1.139611461e-10f, 1.996093359e-17f, 5.344349223e-25f, 1.511644828e-31f,
 1.139611461e-10f, 1.996093359e-17f, 5.344349223e-25f, 1.511644828e-31f,
 1.139611461e-10f, 1.996093359e-17f, 5.344349223e-25f, 1.511644828e-31f,
 1.139611461e-10f, 1.996093359e-17f, 5.344349223e-25f, 1.511644828e-31f,
 5.575349904e-11f, 6.083145782e-18f, 5.344349223e-25f, 1.511644828e-31f,
 2.664967552e-11f, -8.557475018e-19f, -8.595036458e-26f, -2.139883875e-32f,
 1.209775682e-11f, 2.61369883e-18f, 5.344349223e-25f, 1.511644828e-31f,
 4.821800945e-12f, 8.789757674e-19f, 1.208447639e-25f, 3.253064536e-33f,
 1.183823005e-12f, 1.161414894e-20f, 1.29131908e-27f, 1.715766248e-34f,
 1.183823005e-12f, 1.161414894e-20f, 1.29131908e-27f, 1.715766248e-34f,
 2.743283031e-13f, 1.161414894e-20f, 1.29131908e-27f, 1.715766248e-34f,
};


#if defined(_MSC_VER) && !defined (__clang__)
#pragma fp_contract (off)
#endif

//#pragma STDC FP_CONTRACT OFF

#define C2V(x) (x)
// Copyright Naoki Shibata and contributors 2010 - 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// These are macros for evaluating polynomials using Estrin's method

#define POLY2(x, c1, c0) mla(x, C2V(c1), C2V(c0))
#define POLY3(x, x2, c2, c1, c0) mla(x2, C2V(c2), mla(x, C2V(c1), C2V(c0)))
#define POLY4(x, x2, c3, c2, c1, c0) mla(x2, mla(x, C2V(c3), C2V(c2)), mla(x, C2V(c1), C2V(c0)))
#define POLY5(x, x2, x4, c4, c3, c2, c1, c0) mla(x4, C2V(c4), POLY4(x, x2, c3, c2, c1, c0))
#define POLY6(x, x2, x4, c5, c4, c3, c2, c1, c0) mla(x4, POLY2(x, c5, c4), POLY4(x, x2, c3, c2, c1, c0))
#define POLY7(x, x2, x4, c6, c5, c4, c3, c2, c1, c0) mla(x4, POLY3(x, x2, c6, c5, c4), POLY4(x, x2, c3, c2, c1, c0))
#define POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0) mla(x4, POLY4(x, x2, c7, c6, c5, c4), POLY4(x, x2, c3, c2, c1, c0))
#define POLY9(x, x2, x4, x8, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x8, C2V(c8), POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY10(x, x2, x4, x8, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x8, POLY2(x, c9, c8), POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY11(x, x2, x4, x8, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x8, POLY3(x, x2, ca, c9, c8), POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY12(x, x2, x4, x8, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x8, POLY4(x, x2, cb, ca, c9, c8), POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY13(x, x2, x4, x8, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x8, POLY5(x, x2, x4, cc, cb, ca, c9, c8), POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY14(x, x2, x4, x8, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x8, POLY6(x, x2, x4, cd, cc, cb, ca, c9, c8), POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY15(x, x2, x4, x8, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x8, POLY7(x, x2, x4, ce, cd, cc, cb, ca, c9, c8), POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY16(x, x2, x4, x8, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x8, POLY8(x, x2, x4, cf, ce, cd, cc, cb, ca, c9, c8), POLY8(x, x2, x4, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY17(x, x2, x4, x8, x16, d0, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x16, C2V(d0), POLY16(x, x2, x4, x8, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY18(x, x2, x4, x8, x16, d1, d0, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x16, POLY2(x, d1, d0), POLY16(x, x2, x4, x8, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY19(x, x2, x4, x8, x16, d2, d1, d0, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x16, POLY3(x, x2, d2, d1, d0), POLY16(x, x2, x4, x8, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY20(x, x2, x4, x8, x16, d3, d2, d1, d0, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x16, POLY4(x, x2, d3, d2, d1, d0), POLY16(x, x2, x4, x8, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0))
#define POLY21(x, x2, x4, x8, x16, d4, d3, d2, d1, d0, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0) mla(x16, POLY5(x, x2, x4, d4, d3, d2, d1, d0), POLY16(x, x2, x4, x8, cf, ce, cd, cc, cb, ca, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0))

NO_SIDE_EFFECTS INLINE constexpr f32 fabsfk(f32 x) { return __builtin_bit_cast(f32,0x7fffffff & BITCAST(i32,x)); }

NO_SIDE_EFFECTS INLINE constexpr f32 mulsignf(f32 x, f32 y) { return __builtin_bit_cast(f32,BITCAST(i32,x) ^ (BITCAST(i32,y) & (1 << 31))); }

NO_SIDE_EFFECTS INLINE constexpr f32 copysignfk(f32 x, f32 y) { return __builtin_bit_cast(f32,(BITCAST(i32,x) & ~(1 << 31)) ^ (BITCAST(i32,y) & (1 << 31))); }

NO_SIDE_EFFECTS INLINE constexpr f32 signf(f32 d) { return mulsignf(1, d); }
NO_SIDE_EFFECTS INLINE constexpr f32 mla(f32 x, f32 y, f32 z) { return x * y + z; }
NO_SIDE_EFFECTS INLINE constexpr f64 mla(f64 x, f64 y, f64 z) { return x * y + z; }
NO_SIDE_EFFECTS INLINE constexpr f32 rintfk(f32 x) { return static_cast<f32>(x < 0.f ? (i32)(x - 0.5f) : (i32)(x + 0.5f)); }
NO_SIDE_EFFECTS INLINE constexpr i32 ceilfk(f32 x) { return (i32)x + (x < 0 ? 0 : 1); }
NO_SIDE_EFFECTS INLINE constexpr f32 fminfk(f32 x, f32 y) { return x < y ? x : y; }
NO_SIDE_EFFECTS INLINE constexpr f32 fmaxfk(f32 x, f32 y) { return x > y ? x : y; }
NO_SIDE_EFFECTS INLINE constexpr i32 xisintf(f32 x) { return (x == (i32)x); }

//NO_SIDE_EFFECTS INLINE constexpr i32 signbit(f64 d) { return (BITCAST(i32,d) & BITCAST(i32,-0.0)) == BITCAST(i32,-0.0); }
NO_SIDE_EFFECTS INLINE constexpr i32 xisnanf(f32 x) { return x != x; }
NO_SIDE_EFFECTS INLINE constexpr i32 xisinff(f32 x) { return x == SLEEF_INFINITYf || x == -SLEEF_INFINITYf; }
NO_SIDE_EFFECTS INLINE constexpr i32 xisminff(f32 x) { return x == -SLEEF_INFINITYf; }
NO_SIDE_EFFECTS INLINE constexpr i32 xispinff(f32 x) { return x == SLEEF_INFINITYf; }
NO_SIDE_EFFECTS INLINE constexpr i32 xisnegzerof(f32 x) { return BITCAST(i32,x) == BITCAST(i32,-0.f); }
NO_SIDE_EFFECTS INLINE constexpr i32 xisnumberf(f32 x) { return !xisinff(x) && !xisnanf(x); }

NO_SIDE_EFFECTS INLINE constexpr i32 ilogbkf(f32 d) {
	i32 m = d < 5.421010862427522E-20f;
	d = m ? 1.8446744073709552E19f * d : d;
	i32 q = (BITCAST(i32,d) >> 23) & 0xff;
	q = m ? q - (64 + 0x7f) : q - 0x7f;
	return q;
}

// vilogb2kf is similar to ilogbkf, but the argument has to be a
// normalized FP value.
NO_SIDE_EFFECTS INLINE constexpr i32 ilogb2kf(f32 d) {
	return ((BITCAST(i32,d) >> 23) & 0xff) - 0x7f;
}

NO_SIDE_EFFECTS constexpr i32 xilogbf(f32 d) {
	i32 e = ilogbkf(fabsfk(d));
	e = d == 0.0f ? SLEEF_FP_ILOGB0 : e;
	e = xisnanf(d) ? SLEEF_FP_ILOGBNAN : e;
	e = xisinff(d) ? INT_MAX : e;
	return e;
}

NO_SIDE_EFFECTS INLINE constexpr f32 pow2if(i32 q) {
	return __builtin_bit_cast(f32,((i32)(q + 0x7f)) << 23);
}

NO_SIDE_EFFECTS INLINE constexpr f32 ldexpkf(f32 x, i32 q) {
	i32 m = q >> 31;
	m = (((m + q) >> 6) - m) << 4;
	q = q - (m << 2);
	m += 127;
	m = m < 0 ? 0 : m;
	m = m > 255 ? 255 : m;
	f32 u = __builtin_bit_cast(f32,((i32)m) << 23);
	x = x * u * u * u * u;
	u = __builtin_bit_cast(f32,((i32)(q + 0x7f)) << 23);
	return x * u;
}
// faster than ldexpkf, short reach
NO_SIDE_EFFECTS INLINE constexpr f32 ldexp2kf(f32 d, i32 e) { return d * pow2if(e >> 1) * pow2if(e - (e >> 1)); }

NO_SIDE_EFFECTS INLINE constexpr f32 ldexp3kf(f32 d, i32 e) { // very fast, no denormal
	return __builtin_bit_cast(f32,BITCAST(i32,d) + (e << 23));
}

NO_SIDE_EFFECTS INLINE constexpr f32 upperf(f32 d) {
	return __builtin_bit_cast(f32,BITCAST(i32,d) & 0xfffff000);
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 df(f32 h, f32 l) {
	Sleef_float2 ret;
	ret.x = h; ret.y = l;
	return ret;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfx(f32 d) {
	Sleef_float2 ret;
	ret.x = d; ret.y = d - ret.x;
	return ret;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfnormalize_f2_f2(Sleef_float2 t) {
	Sleef_float2 s;

	s.x = t.x + t.y;
	s.y = t.x - s.x + t.y;

	return s;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfscale_f2_f2_f(Sleef_float2 d, f32 s) {
	Sleef_float2 r;

	r.x = d.x * s;
	r.y = d.y * s;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfneg_f2_f2(Sleef_float2 d) {
	Sleef_float2 r;

	r.x = -d.x;
	r.y = -d.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfabs_f2_f2(Sleef_float2 x) {
	return df(x.x < 0 ? -x.x : x.x, x.x < 0 ? -x.y : x.y);
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfadd_f2_f_f(f32 x, f32 y) {
	// |x| >= |y|

	Sleef_float2 r;

	r.x = x + y;
	r.y = x - r.x + y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfadd2_f2_f_f(f32 x, f32 y) {
	Sleef_float2 r;

	r.x = x + y;
	f32 v = r.x - x;
	r.y = (x - (r.x - v)) + (y - v);

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfadd_f2_f2_f(Sleef_float2 x, f32 y) {
	// |x| >= |y|

	Sleef_float2 r;

	r.x = x.x + y;
	r.y = x.x - r.x + y + x.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfadd_f2_f_f2(f32 x, Sleef_float2 y) {
	// |x| >= |y|

	Sleef_float2 r;

	r.x = x + y.x;
	r.y = x - r.x + y.x + y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfadd2_f2_f2_f(Sleef_float2 x, f32 y) {
	// |x| >= |y|

	Sleef_float2 r;

	r.x = x.x + y;
	f32 v = r.x - x.x;
	r.y = (x.x - (r.x - v)) + (y - v);
	r.y += x.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfadd2_f2_f_f2(f32 x, Sleef_float2 y) {
	Sleef_float2 r;

	r.x = x + y.x;
	f32 v = r.x - x;
	r.y = (x - (r.x - v)) + (y.x - v) + y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfadd_f2_f2_f2(Sleef_float2 x, Sleef_float2 y) {
	// |x| >= |y|

	Sleef_float2 r;

	r.x = x.x + y.x;
	r.y = x.x - r.x + y.x + x.y + y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfadd2_f2_f2_f2(Sleef_float2 x, Sleef_float2 y) {
	Sleef_float2 r;

	r.x = x.x + y.x;
	f32 v = r.x - x.x;
	r.y = (x.x - (r.x - v)) + (y.x - v);
	r.y += x.y + y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfsub_f2_f2_f2(Sleef_float2 x, Sleef_float2 y) {
	// |x| >= |y|

	Sleef_float2 r;

	r.x = x.x - y.x;
	r.y = x.x - r.x - y.x + x.y - y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfdiv_f2_f2_f2(Sleef_float2 n, Sleef_float2 d) {
	f32 t = 1.0f / d.x;
	f32 dh = upperf(d.x), dl = d.x - dh;
	f32 th = upperf(t), tl = t - th;
	f32 nhh = upperf(n.x), nhl = n.x - nhh;

	Sleef_float2 q;

	q.x = n.x * t;

	f32 u = -q.x + nhh * th + nhh * tl + nhl * th + nhl * tl +
		q.x * (1 - dh * th - dh * tl - dl * th - dl * tl);

	q.y = t * (n.y - q.x * d.y) + u;

	return q;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfmul_f2_f_f(f32 x, f32 y) {
	f32 xh = upperf(x), xl = x - xh;
	f32 yh = upperf(y), yl = y - yh;
	Sleef_float2 r;

	r.x = x * y;
	r.y = xh * yh - r.x + xl * yh + xh * yl + xl * yl;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfmul_f2_f2_f(Sleef_float2 x, f32 y) {
	f32 xh = upperf(x.x), xl = x.x - xh;
	f32 yh = upperf(y), yl = y - yh;
	Sleef_float2 r;

	r.x = x.x * y;
	r.y = xh * yh - r.x + xl * yh + xh * yl + xl * yl + x.y * y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfmul_f2_f2_f2(Sleef_float2 x, Sleef_float2 y) {
	f32 xh = upperf(x.x), xl = x.x - xh;
	f32 yh = upperf(y.x), yl = y.x - yh;
	Sleef_float2 r;

	r.x = x.x * y.x;
	r.y = xh * yh - r.x + xl * yh + xh * yl + xl * yl + x.x * y.y + x.y * y.x;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr f32 dfmul_f_f2_f2(Sleef_float2 x, Sleef_float2 y) {
	f32 xh = upperf(x.x), xl = x.x - xh;
	f32 yh = upperf(y.x), yl = y.x - yh;

	return x.y * yh + xh * y.y + xl * yl + xh * yl + xl * yh + xh * yh;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfsqu_f2_f2(Sleef_float2 x) {
	f32 xh = upperf(x.x), xl = x.x - xh;
	Sleef_float2 r;

	r.x = x.x * x.x;
	r.y = xh * xh - r.x + (xh + xh) * xl + xl * xl + x.x * (x.y + x.y);

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr f32 dfsqu_f_f2(Sleef_float2 x) {
	f32 xh = upperf(x.x), xl = x.x - xh;

	return xh * x.y + xh * x.y + xl * xl + (xh * xl + xh * xl) + xh * xh;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfrec_f2_f(f32 d) {
	f32 t = 1.0f / d;
	f32 dh = upperf(d), dl = d - dh;
	f32 th = upperf(t), tl = t - th;
	Sleef_float2 q;

	q.x = t;
	q.y = t * (1 - dh * th - dh * tl - dl * th - dl * tl);

	return q;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfrec_f2_f2(Sleef_float2 d) {
	f32 t = 1.0f / d.x;
	f32 dh = upperf(d.x), dl = d.x - dh;
	f32 th = upperf(t), tl = t - th;
	Sleef_float2 q;

	q.x = t;
	q.y = t * (1 - dh * th - dh * tl - dl * th - dl * tl - d.y * t);

	return q;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfsqrt_f2_f2(Sleef_float2 d) {
	f32 t = Math::sqrt(d.x + d.y);
	return dfscale_f2_f2_f(dfmul_f2_f2_f2(dfadd2_f2_f2_f2(d, dfmul_f2_f_f(t, t)), dfrec_f2_f(t)), 0.5f);
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfsqrt_f2_f(f32 d) {
	f32 t = Math::sqrt(d);
	return dfscale_f2_f2_f(dfmul_f2_f2_f2(dfadd2_f2_f_f2(d, dfmul_f2_f_f(t, t)), dfrec_f2_f(t)), 0.5);
}

//

typedef struct {
	f32 d;
	i32 i;
} fi_t;

typedef struct {
	Sleef_float2 df;
	i32 i;
} dfi_t;

NO_SIDE_EFFECTS constexpr fi_t rempisubf(f32 x) {
	fi_t ret;
	f32 fr = x - (f32)(INT64_C(1) << 10) * (i32)(x * (1.0f / (INT64_C(1) << 10)));
	ret.i = ((7 & ((x > 0 ? 4 : 3) + (i32)(fr * 8))) - 3) >> 1;
	fr = fr - 0.25f * (i32)(fr * 4 + mulsignf(0.5f, x));
	fr = fabsfk(fr) > 0.125f ? (fr - mulsignf(0.5f, x)) : fr;
	fr = fabsfk(fr) > 1e+10f ? 0 : fr;
	if (fabsfk(x) == 0.12499999254941940308f) { fr = x; ret.i = 0; }
	ret.d = fr;
	return ret;
}

NO_SIDE_EFFECTS constexpr dfi_t rempif(f32 a) {
	Sleef_float2 x, y;
	fi_t di;
	i32 ex = ilogb2kf(a) - 25, q = ex > (90 - 25) ? -64 : 0;
	a = ldexp3kf(a, q);
	if (ex < 0) ex = 0;
	ex *= 4;
	x = dfmul_f2_f_f(a, Sleef_rempitabsp[ex]);
	di = rempisubf(x.x);
	q = di.i;
	x.x = di.d;
	x = dfnormalize_f2_f2(x);
	y = dfmul_f2_f_f(a, Sleef_rempitabsp[ex + 1]);
	x = dfadd2_f2_f2_f2(x, y);
	di = rempisubf(x.x);
	q += di.i;
	x.x = di.d;
	x = dfnormalize_f2_f2(x);
	y = dfmul_f2_f2_f(df(Sleef_rempitabsp[ex + 2], Sleef_rempitabsp[ex + 3]), a);
	x = dfadd2_f2_f2_f2(x, y);
	x = dfnormalize_f2_f2(x);
	x = dfmul_f2_f2_f2(x, df(3.1415927410125732422f * 2, -8.7422776573475857731e-08f * 2));
	dfi_t ret = { fabsfk(a) < 0.7f ? df(a, 0) : x, q };
	return ret;
}

NO_SIDE_EFFECTS constexpr f32 xsinf(f32 d) {
	i32 q;
	f32 u, s, t = d;

	if (fabsfk(d) < TRIGRANGEMAX2f) {
		q = (i32)rintfk(d * (f32)(1 / Math::pi<f64>));
		d = mla(static_cast<f32>(q), -PI_A2f, d);
		d = mla(static_cast<f32>(q), -PI_B2f, d);
		d = mla(static_cast<f32>(q), -PI_C2f, d);
	}
	else if (fabsfk(d) < TRIGRANGEMAXf) {
		q = (i32)rintfk(d * (f32)(1 / Math::pi<f64>));
		d = mla(static_cast<f32>(q), -PI_Af, d);
		d = mla(static_cast<f32>(q), -PI_Bf, d);
		d = mla(static_cast<f32>(q), -PI_Cf, d);
		d = mla(static_cast<f32>(q), -PI_Df, d);
	}
	else {
		dfi_t dfi = rempif(t);
		q = ((dfi.i & 3) * 2 + (dfi.df.x > 0) + 1) >> 2;
		if ((dfi.i & 1) != 0) {
			dfi.df = dfadd2_f2_f2_f2(dfi.df, df(mulsignf(3.1415927410125732422f * -0.5, dfi.df.x),
												mulsignf(-8.7422776573475857731e-08f * -0.5, dfi.df.x)));
		}
		d = dfi.df.x + dfi.df.y;
		if (xisinff(t) || xisnanf(t)) d = SLEEF_NANf;
	}

	s = d * d;

	if ((q & 1) != 0) d = -d;

	u = 2.6083159809786593541503e-06f;
	u = mla(u, s, -0.0001981069071916863322258f);
	u = mla(u, s, 0.00833307858556509017944336f);
	u = mla(u, s, -0.166666597127914428710938f);

	u = mla(s, u * d, d);

	if (xisnegzerof(t)) u = -0.0f;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xsinf_u1(f32 d) {
	i32 q;
	f32 u;
	Sleef_float2 s, t, x;

	if (fabsfk(d) < TRIGRANGEMAX2f) {
		q = (i32)rintfk(d * (f32)(1 / Math::pi<f64>));
		u = mla(static_cast<f32>(q), -PI_A2f, d);
		s = dfadd2_f2_f_f(u, q * (-PI_B2f));
		s = dfadd_f2_f2_f(s, q * (-PI_C2f));
	}
	else {
		dfi_t dfi = rempif(d);
		q = ((dfi.i & 3) * 2 + (dfi.df.x > 0) + 1) >> 2;
		if ((dfi.i & 1) != 0) {
			dfi.df = dfadd2_f2_f2_f2(dfi.df, df(mulsignf(3.1415927410125732422f * -0.5, dfi.df.x),
												mulsignf(-8.7422776573475857731e-08f * -0.5, dfi.df.x)));
		}
		s = dfnormalize_f2_f2(dfi.df);
		if (xisinff(d) || xisnanf(d)) s.x = SLEEF_NANf;
	}

	t = s;
	s = dfsqu_f2_f2(s);

	u = 2.6083159809786593541503e-06f;
	u = mla(u, s.x, -0.0001981069071916863322258f);
	u = mla(u, s.x, 0.00833307858556509017944336f);

	x = dfadd_f2_f_f2(1, dfmul_f2_f2_f2(dfadd_f2_f_f(-0.166666597127914428710938f, u * s.x), s));

	u = dfmul_f_f2_f2(t, x);

	if ((q & 1) != 0) u = -u;
	if (xisnegzerof(d)) u = d;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xcosf(f32 d) {
	i32 q;
	f32 u, s, t = d;

	if (fabsfk(d) < TRIGRANGEMAX2f) {
		q = 1 + 2 * (i32)rintfk(d * (f32)(1 / Math::pi<f64>) - 0.5f);
		d = mla(static_cast<f32>(q), -PI_A2f * 0.5f, d);
		d = mla(static_cast<f32>(q), -PI_B2f * 0.5f, d);
		d = mla(static_cast<f32>(q), -PI_C2f * 0.5f, d);
	}
	else if (fabsfk(d) < TRIGRANGEMAXf) {
		q = 1 + 2 * (i32)rintfk(d * (f32)(1 / Math::pi<f64>) - 0.5f);
		d = mla(static_cast<f32>(q), -PI_Af * 0.5f, d);
		d = mla(static_cast<f32>(q), -PI_Bf * 0.5f, d);
		d = mla(static_cast<f32>(q), -PI_Cf * 0.5f, d);
		d = mla(static_cast<f32>(q), -PI_Df * 0.5f, d);
	}
	else {
		dfi_t dfi = rempif(t);
		q = ((dfi.i & 3) * 2 + (dfi.df.x > 0) + 7) >> 1;
		if ((dfi.i & 1) == 0) {
			dfi.df = dfadd2_f2_f2_f2(dfi.df, df(mulsignf(3.1415927410125732422f * -0.5, dfi.df.x > 0 ? 1.f : -1.f),
												mulsignf(-8.7422776573475857731e-08f * -0.5, dfi.df.x > 0 ? 1.f : -1.f)));
		}
		d = dfi.df.x + dfi.df.y;
		if (xisinff(t) || xisnanf(t)) d = SLEEF_NANf;
	}

	s = d * d;

	if ((q & 2) == 0) d = -d;

	u = 2.6083159809786593541503e-06f;
	u = mla(u, s, -0.0001981069071916863322258f);
	u = mla(u, s, 0.00833307858556509017944336f);
	u = mla(u, s, -0.166666597127914428710938f);

	u = mla(s, u * d, d);

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xcosf_u1(f32 d) {
	f32 u;
	Sleef_float2 s, t, x;
	i32 q;

	if (fabsfk(d) < TRIGRANGEMAX2f) {
		d = fabsfk(d);
		f32 dq = mla(rintfk(d * (f32)(1 / Math::pi<f64>) - 0.5f), 2, 1);
		q = (i32)dq;
		s = dfadd2_f2_f_f(d, dq * (-PI_A2f * 0.5f));
		s = dfadd2_f2_f2_f(s, dq * (-PI_B2f * 0.5f));
		s = dfadd2_f2_f2_f(s, dq * (-PI_C2f * 0.5f));
	}
	else {
		dfi_t dfi = rempif(d);
		q = ((dfi.i & 3) * 2 + (dfi.df.x > 0) + 7) >> 1;
		if ((dfi.i & 1) == 0) {
			dfi.df = dfadd2_f2_f2_f2(dfi.df, df(mulsignf(3.1415927410125732422f * -0.5, dfi.df.x > 0 ? 1.f : -1.f),
												mulsignf(-8.7422776573475857731e-08f * -0.5, dfi.df.x > 0 ? 1.f : -1.f)));
		}
		s = dfnormalize_f2_f2(dfi.df);
		if (xisinff(d) || xisnanf(d)) s.x = SLEEF_NANf;
	}

	t = s;
	s = dfsqu_f2_f2(s);

	u = 2.6083159809786593541503e-06f;
	u = mla(u, s.x, -0.0001981069071916863322258f);
	u = mla(u, s.x, 0.00833307858556509017944336f);

	x = dfadd_f2_f_f2(1, dfmul_f2_f2_f2(dfadd_f2_f_f(-0.166666597127914428710938f, u * s.x), s));

	u = dfmul_f_f2_f2(t, x);

	if ((((i32)q) & 2) == 0) u = -u;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xfastsinf_u3500(f32 d) {
	i32 q;
	f32 u, s, t = d;

	q = static_cast<i32>(rintfk(d * (f32)(1 / Math::pi<f32>)));
	d = mla(static_cast<f32>(q), -(f32)Math::pi<f32>, d);

	s = d * d;

	u = -0.1881748176e-3f;
	u = mla(u, s, +0.8323502727e-2f);
	u = mla(u, s, -0.1666651368e+0f);
	u = mla(s * d, u, d);

	if ((q & 1) != 0) u = -u;

	if (UNLIKELY(fabsfk(t) > 30.0f)) return xsinf(t);

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xfastcosf_u3500(f32 d) {
	i32 q;
	f32 u, s, t = d;

	q = static_cast<i32>(rintfk(mla(d, (f32)(1 / Math::pi<f64>), -0.5f)));
	d = mla(static_cast<f32>(q), -(f32)Math::pi<f64>, d - (f32)Math::pi<f64> *0.5f);

	s = d * d;

	u = -0.1881748176e-3f;
	u = mla(u, s, +0.8323502727e-2f);
	u = mla(u, s, -0.1666651368e+0f);
	u = mla(s * d, u, d);

	if ((q & 1) == 0) u = -u;

	if (UNLIKELY(fabsfk(t) > 30.0f)) return xcosf(t);

	return u;
}

NO_SIDE_EFFECTS constexpr SinCos<f32> xsincosf(f32 d) {
	i32 q;
	f32 u, s, t;
	SinCos<f32> r;

	s = d;

	if (fabsfk(d) < TRIGRANGEMAX2f) {
		q = (i32)rintfk(d * ((f32)(2 * (1 / Math::pi<f64>))));
		s = mla(static_cast<f32>(q), -PI_A2f * 0.5f, s);
		s = mla(static_cast<f32>(q), -PI_B2f * 0.5f, s);
		s = mla(static_cast<f32>(q), -PI_C2f * 0.5f, s);
	}
	else if (fabsfk(d) < TRIGRANGEMAXf) {
		q = (i32)rintfk(d * ((f32)(2 * (1 / Math::pi<f64>))));
		s = mla(static_cast<f32>(q), -PI_Af * 0.5f, s);
		s = mla(static_cast<f32>(q), -PI_Bf * 0.5f, s);
		s = mla(static_cast<f32>(q), -PI_Cf * 0.5f, s);
		s = mla(static_cast<f32>(q), -PI_Df * 0.5f, s);
	}
	else {
		dfi_t dfi = rempif(d);
		q = dfi.i;
		s = dfi.df.x + dfi.df.y;
		if (xisinff(d) || xisnanf(d)) s = SLEEF_NANf;
	}

	t = s;

	s = s * s;

	u = -0.000195169282960705459117889f;
	u = mla(u, s, 0.00833215750753879547119141f);
	u = mla(u, s, -0.166666537523269653320312f);
	u = u * s * t;

	r.s = t + u;

	if (xisnegzerof(d)) r.s = -0.0f;

	u = -2.71811842367242206819355e-07f;
	u = mla(u, s, 2.47990446951007470488548e-05f);
	u = mla(u, s, -0.00138888787478208541870117f);
	u = mla(u, s, 0.0416666641831398010253906f);
	u = mla(u, s, -0.5f);

	r.c = u * s + 1;

	if ((q & 1) != 0) { s = r.c; r.c = r.s; r.s = s; }
	if ((q & 2) != 0) { r.s = -r.s; }
	if (((q + 1) & 2) != 0) { r.c = -r.c; }

	return r;
}

NO_SIDE_EFFECTS constexpr Sleef_float2 xsincosf_u1(f32 d) {
	i32 q;
	f32 u;
	Sleef_float2 r, s, t, x;

	if (fabsfk(d) < TRIGRANGEMAX2f) {
		q = (i32)rintfk(d * (f32)(2 * (1 / Math::pi<f64>)));
		u = mla(static_cast<f32>(q), -PI_A2f * 0.5f, d);
		s = dfadd2_f2_f_f(u, q * (-PI_B2f * 0.5f));
		s = dfadd_f2_f2_f(s, q * (-PI_C2f * 0.5f));
	}
	else {
		dfi_t dfi = rempif(d);
		q = dfi.i;
		s = dfi.df;
		if (xisinff(d) || xisnanf(d)) s.x = SLEEF_NANf;
	}

	t = s;
	s.x = dfsqu_f_f2(s);

	u = -0.000195169282960705459117889f;
	u = mla(u, s.x, 0.00833215750753879547119141f);
	u = mla(u, s.x, -0.166666537523269653320312f);

	u *= s.x * t.x;

	x = dfadd_f2_f2_f(t, u);
	r.x = x.x + x.y;
	if (xisnegzerof(d)) r.x = -0.0f;

	u = -2.71811842367242206819355e-07f;
	u = mla(u, s.x, 2.47990446951007470488548e-05f);
	u = mla(u, s.x, -0.00138888787478208541870117f);
	u = mla(u, s.x, 0.0416666641831398010253906f);
	u = mla(u, s.x, -0.5f);

	x = dfadd_f2_f_f2(1, dfmul_f2_f_f(s.x, u));
	r.y = x.x + x.y;

	if ((q & 1) != 0) { u = r.y; r.y = r.x; r.x = u; }
	if ((q & 2) != 0) { r.x = -r.x; }
	if (((q + 1) & 2) != 0) { r.y = -r.y; }

	return r;
}

NO_SIDE_EFFECTS constexpr Sleef_float2 xsincospif_u05(f32 d) {
	f32 u, s, t;
	Sleef_float2 r, x, s2;

	u = d * 4;
	i32 q = ceilfk(u) & ~(i32)1;

	s = u - (f32)q;
	t = s;
	s = s * s;
	s2 = dfmul_f2_f_f(t, t);

	//

	u = +0.3093842054e-6f;
	u = mla(u, s, -0.3657307388e-4f);
	u = mla(u, s, +0.2490393585e-2f);
	x = dfadd2_f2_f_f2(u * s, df(-0.080745510756969451904f, -1.3373665339076936258e-09f));
	x = dfadd2_f2_f2_f2(dfmul_f2_f2_f2(s2, x), df(0.78539818525314331055f, -2.1857338617566484855e-08f));

	x = dfmul_f2_f2_f(x, t);
	r.x = x.x + x.y;
	if (xisnegzerof(d)) r.x = -0.0f;

	u = -0.2430611801e-7f;
	u = mla(u, s, +0.3590577080e-5f);
	u = mla(u, s, -0.3259917721e-3f);
	x = dfadd2_f2_f_f2(u * s, df(0.015854343771934509277f, 4.4940051354032242811e-10f));
	x = dfadd2_f2_f2_f2(dfmul_f2_f2_f2(s2, x), df(-0.30842512845993041992f, -9.0728339030733922277e-09f));

	x = dfadd2_f2_f2_f(dfmul_f2_f2_f2(x, s2), 1);
	r.y = x.x + x.y;

	if ((q & 2) != 0) { s = r.y; r.y = r.x; r.x = s; }
	if ((q & 4) != 0) { r.x = -r.x; }
	if (((q + 2) & 4) != 0) { r.y = -r.y; }

	if (fabsfk(d) > 1e+7f) { r.x = 0; r.y = 1; }
	if (xisinff(d)) { r.x = r.y = SLEEF_NANf; }

	return r;
}

NO_SIDE_EFFECTS constexpr Sleef_float2 xsincospif_u35(f32 d) {
	f32 u, s, t;
	Sleef_float2 r;

	u = d * 4;
	i32 q = ceilfk(u) & ~(i32)1;

	s = u - (f32)q;
	t = s;
	s = s * s;

	//

	u = -0.3600925265e-4f;
	u = mla(u, s, +0.2490088111e-2f);
	u = mla(u, s, -0.8074551076e-1f);
	u = mla(u, s, +0.7853981853e+0f);

	r.x = u * t;

	u = +0.3539815225e-5f;
	u = mla(u, s, -0.3259574005e-3f);
	u = mla(u, s, +0.1585431583e-1f);
	u = mla(u, s, -0.3084251285e+0f);
	u = mla(u, s, 1);

	r.y = u;

	if ((q & 2) != 0) { s = r.y; r.y = r.x; r.x = s; }
	if ((q & 4) != 0) { r.x = -r.x; }
	if (((q + 2) & 4) != 0) { r.y = -r.y; }

	if (fabsfk(d) > 1e+7f) { r.x = 0; r.y = 1; }
	if (xisinff(d)) { r.x = r.y = SLEEF_NANf; }

	return r;
}

NO_SIDE_EFFECTS constexpr f32 xtanf(f32 d) {
	i32 q;
	f32 u, s, x;

	x = d;

	if (fabsfk(d) < TRIGRANGEMAX2f * 0.5f) {
		q = (i32)rintfk(d * (f32)(2 * (1 / Math::pi<f64>)));
		x = mla(static_cast<f32>(q), -PI_A2f * 0.5f, x);
		x = mla(static_cast<f32>(q), -PI_B2f * 0.5f, x);
		x = mla(static_cast<f32>(q), -PI_C2f * 0.5f, x);
	}
	else if (fabsfk(d) < TRIGRANGEMAXf) {
		q = (i32)rintfk(d * (f32)(2 * (1 / Math::pi<f64>)));
		x = mla(static_cast<f32>(q), -PI_Af * 0.5f, x);
		x = mla(static_cast<f32>(q), -PI_Bf * 0.5f, x);
		x = mla(static_cast<f32>(q), -PI_Cf * 0.5f, x);
		x = mla(static_cast<f32>(q), -PI_Df * 0.5f, x);
	}
	else {
		dfi_t dfi = rempif(d);
		q = dfi.i;
		x = dfi.df.x + dfi.df.y;
		if (xisinff(d) || xisnanf(d)) x = SLEEF_NANf;
	}

	s = x * x;

	if ((q & 1) != 0) x = -x;

	f32 s2 = s * s, s4 = s2 * s2;
	u = POLY6(s, s2, s4,
			  0.00927245803177356719970703f,
			  0.00331984995864331722259521f,
			  0.0242998078465461730957031f,
			  0.0534495301544666290283203f,
			  0.133383005857467651367188f,
			  0.333331853151321411132812f);

	u = mla(s, u * x, x);

	if ((q & 1) != 0) u = 1.0f / u;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xtanf_u1(f32 d) {
	i32 q;
	f32 u;
	Sleef_float2 s, t, x;

	if (fabsfk(d) < TRIGRANGEMAX2f) {
		q = (i32)rintfk(d * (f32)(2 * (1 / Math::pi<f64>)));
		u = mla(static_cast<f32>(q), -PI_A2f * 0.5f, d);
		s = dfadd2_f2_f_f(u, q * (-PI_B2f * 0.5f));
		s = dfadd_f2_f2_f(s, q * (-PI_C2f * 0.5f));
	}
	else {
		dfi_t dfi = rempif(d);
		q = dfi.i;
		s = dfi.df;
		if (xisinff(d) || xisnanf(d)) s.x = SLEEF_NANf;
	}

	if ((q & 1) != 0) s = dfneg_f2_f2(s);

	t = s;
	s = dfsqu_f2_f2(s);
	s = dfnormalize_f2_f2(s);

	u = 0.00446636462584137916564941f;
	u = mla(u, s.x, -8.3920182078145444393158e-05f);
	u = mla(u, s.x, 0.0109639242291450500488281f);
	u = mla(u, s.x, 0.0212360303848981857299805f);
	u = mla(u, s.x, 0.0540687143802642822265625f);

	x = dfadd_f2_f_f(0.133325666189193725585938f, u * s.x);
	x = dfadd_f2_f_f2(1, dfmul_f2_f2_f2(dfadd_f2_f_f2(0.33333361148834228515625f, dfmul_f2_f2_f2(s, x)), s));
	x = dfmul_f2_f2_f2(t, x);

	if ((q & 1) != 0) x = dfrec_f2_f2(x);

	u = x.x + x.y;

	if (xisnegzerof(d)) u = -0.0f;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xatanf(f32 s) {
	f32 t, u;
	i32 q = 0;

	if (signf(s) == -1) { s = -s; q = 2; }
	if (s > 1) { s = 1.0f / s; q |= 1; }

	t = s * s;

	f32 t2 = t * t, t4 = t2 * t2;
	u = POLY8(t, t2, t4,
			  0.00282363896258175373077393f,
			  -0.0159569028764963150024414f,
			  0.0425049886107444763183594f,
			  -0.0748900920152664184570312f,
			  0.106347933411598205566406f,
			  -0.142027363181114196777344f,
			  0.199926957488059997558594f,
			  -0.333331018686294555664062f);

	t = s + s * (t * u);

	if ((q & 1) != 0) t = 1.570796326794896557998982f - t;
	if ((q & 2) != 0) t = -t;

	return t;
}

NO_SIDE_EFFECTS INLINE constexpr f32 atan2kf(f32 y, f32 x) {
	f32 s, t, u;
	i32 q = 0;

	if (x < 0) { x = -x; q = -2; }
	if (y > x) { t = x; x = y; y = -t; q += 1; }

	s = y / x;
	t = s * s;

	f32 t2 = t * t, t4 = t2 * t2;
	u = POLY8(t, t2, t4,
			  0.00282363896258175373077393f,
			  -0.0159569028764963150024414f,
			  0.0425049886107444763183594f,
			  -0.0748900920152664184570312f,
			  0.106347933411598205566406f,
			  -0.142027363181114196777344f,
			  0.199926957488059997558594f,
			  -0.333331018686294555664062f);

	t = u * t * s + s;
	t = q * (f32)(Math::pi<f64> / 2) + t;

	return t;
}

NO_SIDE_EFFECTS constexpr f32 xatan2f(f32 y, f32 x) {
	f32 r = atan2kf(fabsfk(y), x);

	r = mulsignf(r, x);
	if (xisinff(x) || x == 0) r = M_PIf / 2 - (xisinff(x) ? (signf(x) * (f32)(Math::pi<f64> / 2)) : 0);
	if (xisinff(y)) r = M_PIf / 2 - (xisinff(x) ? (signf(x) * (f32)(Math::pi<f64> *1 / 4)) : 0);
	if (y == 0) r = (signf(x) == -1 ? M_PIf : 0);

	return xisnanf(x) || xisnanf(y) ? SLEEF_NANf : mulsignf(r, y);
}

NO_SIDE_EFFECTS constexpr f32 xasinf(f32 d) {
	i32 o = fabsfk(d) < 0.5f;
	f32 x2 = o ? (d * d) : ((1 - fabsfk(d)) * 0.5f), x = o ? fabsfk(d) : Math::sqrt(x2), u;

	u = +0.4197454825e-1f;
	u = mla(u, x2, +0.2424046025e-1f);
	u = mla(u, x2, +0.4547423869e-1f);
	u = mla(u, x2, +0.7495029271e-1f);
	u = mla(u, x2, +0.1666677296e+0f);
	u = mla(u, x * x2, x);

	f32 r = o ? u : (M_PIf / 2 - 2 * u);
	r = mulsignf(r, d);

	return r;
}

NO_SIDE_EFFECTS constexpr f32 xacosf(f32 d) {
	i32 o = fabsfk(d) < 0.5f;
	f32 x2 = o ? (d * d) : ((1 - fabsfk(d)) * 0.5f), u;
	f32 x = o ? fabsfk(d) : Math::sqrt(x2);
	x = fabsfk(d) == 1.0 ? 0 : x;

	u = +0.4197454825e-1f;
	u = mla(u, x2, +0.2424046025e-1f);
	u = mla(u, x2, +0.4547423869e-1f);
	u = mla(u, x2, +0.7495029271e-1f);
	u = mla(u, x2, +0.1666677296e+0f);

	u *= x * x2;

	f32 y = 3.1415926535897932f / 2 - (mulsignf(x, d) + mulsignf(u, d));
	x += u;
	f32 r = o ? y : (x * 2);
	if (!o && d < 0) r = dfadd_f2_f2_f(df(3.1415927410125732422f, -8.7422776573475857731e-08f), -r).x;

	return r;
}

constexpr Sleef_float2 atan2kf_u1(Sleef_float2 y, Sleef_float2 x) {
	f32 u;
	Sleef_float2 s, t;
	i32 q = 0;

	if (x.x < 0) { x.x = -x.x; x.y = -x.y; q = -2; }
	if (y.x > x.x) { t = x; x = y; y.x = -t.x; y.y = -t.y; q += 1; }

	s = dfdiv_f2_f2_f2(y, x);
	t = dfsqu_f2_f2(s);
	t = dfnormalize_f2_f2(t);

	u = -0.00176397908944636583328247f;
	u = mla(u, t.x, 0.0107900900766253471374512f);
	u = mla(u, t.x, -0.0309564601629972457885742f);
	u = mla(u, t.x, 0.0577365085482597351074219f);
	u = mla(u, t.x, -0.0838950723409652709960938f);
	u = mla(u, t.x, 0.109463557600975036621094f);
	u = mla(u, t.x, -0.142626821994781494140625f);
	u = mla(u, t.x, 0.199983194470405578613281f);

	t = dfmul_f2_f2_f2(t, dfadd_f2_f_f(-0.333332866430282592773438f, u * t.x));
	t = dfmul_f2_f2_f2(s, dfadd_f2_f_f2(1, t));
	t = dfadd2_f2_f2_f2(dfmul_f2_f2_f(df(1.5707963705062866211f, -4.3711388286737928865e-08f), static_cast<f32>(q)), t);

	return t;
}

NO_SIDE_EFFECTS constexpr f32 xatan2f_u1(f32 y, f32 x) {
	if (fabsfk(x) < 2.9387372783541830947e-39f) { y *= (UINT64_C(1) << 24); x *= (UINT64_C(1) << 24); } // nexttowardf((1.0 / FLT_MAX), 1)
	Sleef_float2 d = atan2kf_u1(df(fabsfk(y), 0), df(x, 0));
	f32 r = d.x + d.y;

	r = mulsignf(r, x);
	if (xisinff(x) || x == 0) r = (f32)Math::pi<f64> / 2 - (xisinff(x) ? (signf(x) * (f32)(Math::pi<f64> / 2)) : 0.0f);
	if (xisinff(y)) r = (f32)Math::pi<f64> / 2 - (xisinff(x) ? (signf(x) * (f32)(Math::pi<f64> *1 / 4)) : 0.0f);
	if (y == 0) r = (signf(x) == -1 ? (f32)Math::pi<f64> : 0.0f);

	return xisnanf(x) || xisnanf(y) ? SLEEF_NANf : mulsignf(r, y);
}

NO_SIDE_EFFECTS constexpr f32 xasinf_u1(f32 d) {
	i32 o = fabsfk(d) < 0.5f;
	f32 x2 = o ? (d * d) : ((1 - fabsfk(d)) * 0.5f), u;
	Sleef_float2 x = o ? df(fabsfk(d), 0) : dfsqrt_f2_f(x2);
	x = fabsfk(d) == 1.0f ? df(0, 0) : x;

	u = +0.4197454825e-1f;
	u = mla(u, x2, +0.2424046025e-1f);
	u = mla(u, x2, +0.4547423869e-1f);
	u = mla(u, x2, +0.7495029271e-1f);
	u = mla(u, x2, +0.1666677296e+0f);
	u *= x2 * x.x;

	Sleef_float2 y = dfadd_f2_f2_f(dfsub_f2_f2_f2(df(3.1415927410125732422f / 4, -8.7422776573475857731e-08f / 4), x), -u);
	f32 r = o ? (u + x.x) : ((y.x + y.y) * 2);
	r = mulsignf(r, d);

	return r;
}

NO_SIDE_EFFECTS constexpr f32 xacosf_u1(f32 d) {
	i32 o = fabsfk(d) < 0.5f;
	f32 x2 = o ? (d * d) : ((1 - fabsfk(d)) * 0.5f), u;
	Sleef_float2 x = o ? df(fabsfk(d), 0) : dfsqrt_f2_f(x2);
	x = fabsfk(d) == 1.0 ? df(0, 0) : x;

	u = +0.4197454825e-1f;
	u = mla(u, x2, +0.2424046025e-1f);
	u = mla(u, x2, +0.4547423869e-1f);
	u = mla(u, x2, +0.7495029271e-1f);
	u = mla(u, x2, +0.1666677296e+0f);

	u = u * x.x * x2;

	Sleef_float2 y = dfsub_f2_f2_f2(df(3.1415927410125732422f / 2, -8.7422776573475857731e-08f / 2),
									dfadd_f2_f_f(mulsignf(x.x, d), mulsignf(u, d)));
	x = dfadd_f2_f2_f(x, u);
	y = o ? y : dfscale_f2_f2_f(x, 2);
	if (!o && d < 0) y = dfsub_f2_f2_f2(df(3.1415927410125732422f, -8.7422776573475857731e-08f), y);

	return y.x + y.y;
}

NO_SIDE_EFFECTS constexpr f32 xatanf_u1(f32 d) {
	Sleef_float2 d2 = atan2kf_u1(df(fabsfk(d), 0.0f), df(1.0f, 0.0f));
	f32 r = d2.x + d2.y;
	if (xisinff(d)) r = 1.570796326794896557998982f;
	return mulsignf(r, d);
}

NO_SIDE_EFFECTS constexpr f32 xlogf(f32 d) {
	f32 x, x2, t, m;
	i32 e;

	i32 o = d < FLT_MIN;
	if (o) d *= (f32)(INT64_C(1) << 32) * (f32)(INT64_C(1) << 32);

	e = ilogb2kf(d * (1.0f / 0.75f));
	m = ldexp3kf(d, -e);

	if (o) e -= 64;

	x = (m - 1.0f) / (m + 1.0f);
	x2 = x * x;

	t = 0.2392828464508056640625f;
	t = mla(t, x2, 0.28518211841583251953125f);
	t = mla(t, x2, 0.400005877017974853515625f);
	t = mla(t, x2, 0.666666686534881591796875f);
	t = mla(t, x2, 2.0f);

	x = x * t + 0.693147180559945286226764f * e;

	if (xisinff(d)) x = SLEEF_INFINITYf;
	if (d < 0 || xisnanf(d)) x = SLEEF_NANf;
	if (d == 0) x = -SLEEF_INFINITYf;

	return x;
}

NO_SIDE_EFFECTS constexpr f32 xexpf(f32 d) {
	i32 q = (i32)rintfk(d * R_LN2f);
	f32 s, u;

	s = mla(static_cast<f32>(q), -L2Uf, d);
	s = mla(static_cast<f32>(q), -L2Lf, s);

	u = 0.000198527617612853646278381f;
	u = mla(u, s, 0.00139304355252534151077271f);
	u = mla(u, s, 0.00833336077630519866943359f);
	u = mla(u, s, 0.0416664853692054748535156f);
	u = mla(u, s, 0.166666671633720397949219f);
	u = mla(u, s, 0.5f);

	u = s * s * u + s + 1.0f;
	u = ldexp2kf(u, q);

	if (d < -104) u = 0;
	if (d > 104) u = SLEEF_INFINITYf;

	return u;
}

NO_SIDE_EFFECTS INLINE constexpr f32 expkf(Sleef_float2 d) {
	i32 q = (i32)rintfk((d.x + d.y) * R_LN2f);
	Sleef_float2 s, t;
	f32 u;

	s = dfadd2_f2_f2_f(d, q * -L2Uf);
	s = dfadd2_f2_f2_f(s, q * -L2Lf);

	s = dfnormalize_f2_f2(s);

	u = 0.00136324646882712841033936f;
	u = mla(u, s.x, 0.00836596917361021041870117f);
	u = mla(u, s.x, 0.0416710823774337768554688f);
	u = mla(u, s.x, 0.166665524244308471679688f);
	u = mla(u, s.x, 0.499999850988388061523438f);

	t = dfadd_f2_f2_f2(s, dfmul_f2_f2_f(dfsqu_f2_f2(s), u));

	t = dfadd_f2_f_f2(1, t);

	u = ldexpkf(t.x + t.y, q);

	if (d.x < -104) u = 0;

	return u;
}

NO_SIDE_EFFECTS INLINE constexpr f32 expm1kf(f32 d) {
	i32 q = (i32)rintfk(d * R_LN2f);
	f32 s, u;

	s = mla(static_cast<f32>(q), -L2Uf, d);
	s = mla(static_cast<f32>(q), -L2Lf, s);

	f32 s2 = s * s, s4 = s2 * s2;
	u = POLY6(s, s2, s4,
			  0.000198527617612853646278381f,
			  0.00139304355252534151077271f,
			  0.00833336077630519866943359f,
			  0.0416664853692054748535156f,
			  0.166666671633720397949219f,
			  0.5f);

	u = s * s * u + s;

	if (q != 0) u = ldexp2kf(u + 1, q) - 1;

	return u;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 logkf(f32 d) {
	Sleef_float2 x, x2, s;
	f32 m, t;
	i32 e;

	i32 o = d < FLT_MIN;
	if (o) d *= (f32)(INT64_C(1) << 32) * (f32)(INT64_C(1) << 32);

	e = ilogb2kf(d * (1.0f / 0.75f));
	m = ldexp3kf(d, -e);

	if (o) e -= 64;

	x = dfdiv_f2_f2_f2(dfadd2_f2_f_f(-1, m), dfadd2_f2_f_f(1, m));
	x2 = dfsqu_f2_f2(x);

	t = 0.240320354700088500976562f;
	t = mla(t, x2.x, 0.285112679004669189453125f);
	t = mla(t, x2.x, 0.400007992982864379882812f);
	Sleef_float2 c = df(0.66666662693023681640625f, 3.69183861259614332084311e-09f);

	s = dfmul_f2_f2_f(df(0.69314718246459960938f, -1.904654323148236017e-09f), static_cast<f32>(e));
	s = dfadd_f2_f2_f2(s, dfscale_f2_f2_f(x, 2));
	s = dfadd_f2_f2_f2(s, dfmul_f2_f2_f2(dfmul_f2_f2_f2(x2, x),
										 dfadd2_f2_f2_f2(dfmul_f2_f2_f(x2, t), c)));
	return s;
}

NO_SIDE_EFFECTS constexpr f32 xlogf_u1(f32 d) {
	Sleef_float2 x, s;
	f32 m, t, x2;
	i32 e;

	i32 o = d < FLT_MIN;
	if (o) d *= (f32)(INT64_C(1) << 32) * (f32)(INT64_C(1) << 32);

	e = ilogb2kf(d * (1.0f / 0.75f));
	m = ldexp3kf(d, -e);

	if (o) e -= 64;

	x = dfdiv_f2_f2_f2(dfadd2_f2_f_f(-1, m), dfadd2_f2_f_f(1, m));
	x2 = x.x * x.x;

	t = +0.3027294874e+0f;
	t = mla(t, x2, +0.3996108174e+0f);
	t = mla(t, x2, +0.6666694880e+0f);

	s = dfmul_f2_f2_f(df(0.69314718246459960938f, -1.904654323148236017e-09f), (f32)e);
	s = dfadd_f2_f2_f2(s, dfscale_f2_f2_f(x, 2));
	s = dfadd_f2_f2_f(s, x2 * x.x * t);

	f32 r = s.x + s.y;

	if (xisinff(d)) r = SLEEF_INFINITYf;
	if (d < 0 || xisnanf(d)) r = SLEEF_NANf;
	if (d == 0) r = -SLEEF_INFINITYf;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 expk2f(Sleef_float2 d) {
	i32 q = (i32)rintfk((d.x + d.y) * R_LN2f);
	Sleef_float2 s, t;
	f32 u;

	s = dfadd2_f2_f2_f(d, q * -L2Uf);
	s = dfadd2_f2_f2_f(s, q * -L2Lf);

	u = +0.1980960224e-3f;
	u = mla(u, s.x, +0.1394256484e-2f);
	u = mla(u, s.x, +0.8333456703e-2f);
	u = mla(u, s.x, +0.4166637361e-1f);

	t = dfadd2_f2_f2_f(dfmul_f2_f2_f(s, u), +0.166666659414234244790680580464e+0f);
	t = dfadd2_f2_f2_f(dfmul_f2_f2_f2(s, t), 0.5);
	t = dfadd2_f2_f2_f2(s, dfmul_f2_f2_f2(dfsqu_f2_f2(s), t));

	t = dfadd2_f2_f_f2(1, t);

	t.x = ldexp2kf(t.x, q);
	t.y = ldexp2kf(t.y, q);

	return d.x < -104 ? df(0, 0) : t;
}

NO_SIDE_EFFECTS constexpr f32 xpowf(f32 x, f32 y) {
	i32 yisint = (y == (i32)y) || (fabsfk(y) >= (f32)(INT64_C(1) << 24));
	i32 yisodd = (1 & (i32)y) != 0 && yisint && fabsfk(y) < (f32)(INT64_C(1) << 24);

	f32 result = expkf(dfmul_f2_f2_f(logkf(fabsfk(x)), y));

	result = xisnanf(result) ? SLEEF_INFINITYf : result;
	result *= (x >= 0 ? 1 : (yisint ? (yisodd ? -1 : 1) : SLEEF_NANf));

	f32 efx = mulsignf(fabsfk(x) - 1, y);
	if (xisinff(y)) result = efx < 0 ? 0.0f : (efx == 0 ? 1.0f : SLEEF_INFINITYf);
	if (xisinff(x) || x == 0) result = mulsignf((Math::signbit(y) ^ (x == 0)) ? 0 : SLEEF_INFINITYf, yisodd ? x : 1);
	if (xisnanf(x) || xisnanf(y)) result = SLEEF_NANf;
	if (y == 0 || x == 1) result = 1;

	return result;
}

NO_SIDE_EFFECTS INLINE constexpr f32 logk3f(f32 d) {
	f32 x, x2, t, m;
	i32 e;

	i32 o = d < FLT_MIN;
	if (o) d *= (f32)(INT64_C(1) << 32) * (f32)(INT64_C(1) << 32);

	e = ilogb2kf(d * (1.0f / 0.75f));
	m = ldexp3kf(d, -e);

	if (o) e -= 64;

	x = (m - 1) / (m + 1);
	x2 = x * x;

	t = 0.2392828464508056640625f;
	t = mla(t, x2, 0.28518211841583251953125f);
	t = mla(t, x2, 0.400005877017974853515625f);
	t = mla(t, x2, 0.666666686534881591796875f);
	t = mla(t, x2, 2.0f);

	x = mla(x, t, 0.693147180559945286226764f * e);

	return x;
}

NO_SIDE_EFFECTS INLINE constexpr f32 expk3f(f32 d) {
	i32 q = (i32)rintfk(d * R_LN2f);
	f32 s, u;

	s = mla(static_cast<f32>(q), -L2Uf, d);
	s = mla(static_cast<f32>(q), -L2Lf, s);

	u = 0.000198527617612853646278381f;
	u = mla(u, s, 0.00139304355252534151077271f);
	u = mla(u, s, 0.00833336077630519866943359f);
	u = mla(u, s, 0.0416664853692054748535156f);
	u = mla(u, s, 0.166666671633720397949219f);
	u = mla(u, s, 0.5f);

	u = mla(s * s, u, s + 1.0f);
	u = ldexpkf(u, q);

	if (d < -104) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xfastpowf_u3500(f32 x, f32 y) {
	f32 result = expk3f(logk3f(fabsfk(x)) * y);

	i32 yisint = (y == (i32)y) || (fabsfk(y) >= (f32)(INT64_C(1) << 24));
	i32 yisodd = (1 & (i32)y) != 0 && yisint && fabsfk(y) < (f32)(INT64_C(1) << 24);

	result *= (x < 0 && yisodd) ? -1 : 1;
	if (x == 0) result = 0;
	if (y == 0) result = 1;

	return result;
}

NO_SIDE_EFFECTS constexpr f32 xsinhf(f32 x) {
	f32 y = fabsfk(x);
	Sleef_float2 d = expk2f(df(y, 0));
	d = dfsub_f2_f2_f2(d, dfrec_f2_f2(d));
	y = (d.x + d.y) * 0.5f;

	y = fabsfk(x) > 89 ? SLEEF_INFINITYf : y;
	y = xisnanf(y) ? SLEEF_INFINITYf : y;
	y = mulsignf(y, x);
	y = xisnanf(x) ? SLEEF_NANf : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f32 xcoshf(f32 x) {
	f32 y = fabsfk(x);
	Sleef_float2 d = expk2f(df(y, 0));
	d = dfadd_f2_f2_f2(d, dfrec_f2_f2(d));
	y = (d.x + d.y) * 0.5f;

	y = fabsfk(x) > 89 ? SLEEF_INFINITYf : y;
	y = xisnanf(y) ? SLEEF_INFINITYf : y;
	y = xisnanf(x) ? SLEEF_NANf : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f32 xtanhf(f32 x) {
	f32 y = fabsfk(x);
	Sleef_float2 d = expk2f(df(y, 0));
	Sleef_float2 e = dfrec_f2_f2(d);
	d = dfdiv_f2_f2_f2(dfsub_f2_f2_f2(d, e), dfadd_f2_f2_f2(d, e));
	y = d.x + d.y;

	y = fabsfk(x) > 18.714973875f ? 1.0f : y;
	y = xisnanf(y) ? 1.0f : y;
	y = mulsignf(y, x);
	y = xisnanf(x) ? SLEEF_NANf : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f32 xsinhf_u35(f32 x) {
	f32 e = expm1kf(fabsfk(x));
	f32 y = (e + 2) / (e + 1) * (0.5f * e);

	y = fabsfk(x) > 88 ? SLEEF_INFINITYf : y;
	y = xisnanf(y) ? SLEEF_INFINITYf : y;
	y = mulsignf(y, x);
	y = xisnanf(x) ? SLEEF_NANf : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f32 xcoshf_u35(f32 x) {
	f32 e = xexpf(fabsfk(x));
	f32 y = 0.5f * e + 0.5f / e;

	y = fabsfk(x) > 88 ? SLEEF_INFINITYf : y;
	y = xisnanf(y) ? SLEEF_INFINITYf : y;
	y = xisnanf(x) ? SLEEF_NANf : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f32 xtanhf_u35(f32 x) {
	f32 y = fabsfk(x);
	f32 d = expm1kf(2 * y);
	y = d / (d + 2);

	y = fabsfk(x) > 18.714973875f ? 1.0f : y;
	y = xisnanf(y) ? 1.0f : y;
	y = mulsignf(y, x);
	y = xisnanf(x) ? SLEEF_NANf : y;

	return y;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 logk2f(Sleef_float2 d) {
	Sleef_float2 x, x2, m, s;
	f32 t;
	i32 e;

	e = ilogbkf(d.x * (1.0f / 0.75f));
	m = dfscale_f2_f2_f(d, pow2if(-e));

	x = dfdiv_f2_f2_f2(dfadd2_f2_f2_f(m, -1), dfadd2_f2_f2_f(m, 1));
	x2 = dfsqu_f2_f2(x);

	t = 0.2392828464508056640625f;
	t = mla(t, x2.x, 0.28518211841583251953125f);
	t = mla(t, x2.x, 0.400005877017974853515625f);
	t = mla(t, x2.x, 0.666666686534881591796875f);

	s = dfmul_f2_f2_f(df(0.69314718246459960938f, -1.904654323148236017e-09f), static_cast<f32>(e));
	s = dfadd_f2_f2_f2(s, dfscale_f2_f2_f(x, 2));
	s = dfadd_f2_f2_f2(s, dfmul_f2_f2_f(dfmul_f2_f2_f2(x2, x), t));

	return s;
}

NO_SIDE_EFFECTS constexpr f32 xasinhf(f32 x) {
	f32 y = fabsfk(x);
	Sleef_float2 d;

	d = y > 1 ? dfrec_f2_f(x) : df(y, 0);
	d = dfsqrt_f2_f2(dfadd2_f2_f2_f(dfsqu_f2_f2(d), 1));
	d = y > 1 ? dfmul_f2_f2_f(d, y) : d;

	d = logk2f(dfnormalize_f2_f2(dfadd_f2_f2_f(d, x)));
	y = d.x + d.y;

	y = (fabsfk(x) > SQRT_FLT_MAX || xisnanf(y)) ? mulsignf(SLEEF_INFINITYf, x) : y;
	y = xisnanf(x) ? SLEEF_NANf : y;
	y = xisnegzerof(x) ? -0.0f : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f32 xacoshf(f32 x) {
	Sleef_float2 d = logk2f(dfadd2_f2_f2_f(dfmul_f2_f2_f2(dfsqrt_f2_f2(dfadd2_f2_f_f(x, 1)), dfsqrt_f2_f2(dfadd2_f2_f_f(x, -1))), x));
	f32 y = d.x + d.y;

	y = (x > SQRT_FLT_MAX || xisnanf(y)) ? SLEEF_INFINITYf : y;
	y = x == 1.0f ? 0.0f : y;
	y = x < 1.0f ? SLEEF_NANf : y;
	y = xisnanf(x) ? SLEEF_NANf : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f32 xatanhf(f32 x) {
	f32 y = fabsfk(x);
	Sleef_float2 d = logk2f(dfdiv_f2_f2_f2(dfadd2_f2_f_f(1, y), dfadd2_f2_f_f(1, -y)));
	y = y > 1.0f ? SLEEF_NANf : (y == 1.0f ? SLEEF_INFINITYf : (d.x + d.y) * 0.5f);

	y = xisinff(x) || xisnanf(y) ? SLEEF_NANf : y;
	y = mulsignf(y, x);
	y = xisnanf(x) ? SLEEF_NANf : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f32 xexp2f(f32 d) {
	i32 q = (i32)rintfk(d);
	f32 s, u;

	s = d - q;

	u = +0.1535920892e-3f;
	u = mla(u, s, +0.1339262701e-2f);
	u = mla(u, s, +0.9618384764e-2f);
	u = mla(u, s, +0.5550347269e-1f);
	u = mla(u, s, +0.2402264476e+0f);
	u = mla(u, s, +0.6931471825e+0f);
	u = dfnormalize_f2_f2(dfadd_f2_f_f2(1, dfmul_f2_f_f(u, s))).x;

	u = ldexp2kf(u, q);

	if (d >= 128) u = SLEEF_INFINITYf;
	if (d < -150) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xexp2f_u35(f32 d) {
	i32 q = (i32)rintfk(d);
	f32 s, u;

	s = d - q;

	u = +0.1535920892e-3f;
	u = mla(u, s, +0.1339262701e-2f);
	u = mla(u, s, +0.9618384764e-2f);
	u = mla(u, s, +0.5550347269e-1f);
	u = mla(u, s, +0.2402264476e+0f);
	u = mla(u, s, +0.6931471825e+0f);
	u = mla(u, s, +0.1000000000e+1f);

	u = ldexp2kf(u, q);

	if (d >= 128) u = SLEEF_INFINITYf;
	if (d < -150) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xexp10f(f32 d) {
	i32 q = (i32)rintfk(d * (f32)LOG10_2);
	f32 s, u;

	s = mla(static_cast<f32>(q), -L10Uf, d);
	s = mla(static_cast<f32>(q), -L10Lf, s);

	u = +0.6802555919e-1f;
	u = mla(u, s, +0.2078080326e+0f);
	u = mla(u, s, +0.5393903852e+0f);
	u = mla(u, s, +0.1171245337e+1f);
	u = mla(u, s, +0.2034678698e+1f);
	u = mla(u, s, +0.2650949001e+1f);
	Sleef_float2 x = dfadd_f2_f2_f(df(2.3025851249694824219f, -3.1705172516493593157e-08f), u * s);
	u = dfnormalize_f2_f2(dfadd_f2_f_f2(1, dfmul_f2_f2_f(x, s))).x;

	u = ldexp2kf(u, q);

	if (d > 38.5318394191036238941387f) u = SLEEF_INFINITYf; // log10(FLT_MAX)
	if (d < -50) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xexp10f_u35(f32 d) {
	i32 q = (i32)rintfk(d * (f32)LOG10_2);
	f32 s, u;

	s = mla(static_cast<f32>(q), -L10Uf, d);
	s = mla(static_cast<f32>(q), -L10Lf, s);

	u = +0.2064004987e+0f;
	u = mla(u, s, +0.5417877436e+0f);
	u = mla(u, s, +0.1171286821e+1f);
	u = mla(u, s, +0.2034656048e+1f);
	u = mla(u, s, +0.2650948763e+1f);
	u = mla(u, s, +0.2302585125e+1f);
	u = mla(u, s, +0.1000000000e+1f);

	u = ldexp2kf(u, q);

	if (d > 38.5318394191036238941387f) u = SLEEF_INFINITYf; // log10(FLT_MAX)
	if (d < -50) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f32 xexpm1f(f32 a) {
	Sleef_float2 d = dfadd2_f2_f2_f(expk2f(df(a, 0)), -1.0f);
	f32 x = d.x + d.y;
	if (a > 88.72283172607421875f) x = SLEEF_INFINITYf;
	if (a < -16.635532333438687426013570f) x = -1;
	if (xisnegzerof(a)) x = -0.0f;
	return x;
}

NO_SIDE_EFFECTS constexpr f32 xlog10f(f32 d) {
	Sleef_float2 x, s;
	f32 m, t, x2;
	i32 e;

	i32 o = d < FLT_MIN;
	if (o) d *= (f32)(INT64_C(1) << 32) * (f32)(INT64_C(1) << 32);

	e = ilogb2kf(d * (1.0f / 0.75f));
	m = ldexp3kf(d, -e);

	if (o) e -= 64;

	x = dfdiv_f2_f2_f2(dfadd2_f2_f_f(-1, m), dfadd2_f2_f_f(1, m));
	x2 = x.x * x.x;

	t = +0.1314289868e+0f;
	t = mla(t, x2, +0.1735493541e+0f);
	t = mla(t, x2, +0.2895309627e+0f);

	s = dfmul_f2_f2_f(df(0.30103001f, -1.432098889e-08f), (f32)e);
	s = dfadd_f2_f2_f2(s, dfmul_f2_f2_f2(x, df(0.868588984f, -2.170757285e-08f)));
	s = dfadd_f2_f2_f(s, x2 * x.x * t);

	f32 r = s.x + s.y;

	if (xisinff(d)) r = SLEEF_INFINITYf;
	if (d < 0 || xisnanf(d)) r = SLEEF_NANf;
	if (d == 0) r = -SLEEF_INFINITYf;

	return r;
}

NO_SIDE_EFFECTS constexpr f32 xlog2f(f32 d) {
	Sleef_float2 x, s;
	f32 m, t, x2;
	i32 e;

	i32 o = d < FLT_MIN;
	if (o) d *= (f32)(INT64_C(1) << 32) * (f32)(INT64_C(1) << 32);

	e = ilogb2kf(d * (1.0f / 0.75f));
	m = ldexp3kf(d, -e);

	if (o) e -= 64;

	x = dfdiv_f2_f2_f2(dfadd2_f2_f_f(-1, m), dfadd2_f2_f_f(1, m));
	x2 = x.x * x.x;

	t = +0.4374550283e+0f;
	t = mla(t, x2, +0.5764790177e+0f);
	t = mla(t, x2, +0.9618012905120f);

	s = dfadd2_f2_f_f2(static_cast<f32>(e), dfmul_f2_f2_f2(x, df(2.8853900432586669922f, 3.2734474483568488616e-08f)));
	s = dfadd2_f2_f2_f(s, x2 * x.x * t);

	f32 r = s.x + s.y;

	if (xisinff(d)) r = SLEEF_INFINITYf;
	if (d < 0 || xisnanf(d)) r = SLEEF_NANf;
	if (d == 0) r = -SLEEF_INFINITYf;

	return r;
}

NO_SIDE_EFFECTS constexpr f32 xlog2f_u35(f32 d) {
	f32 m, t, x, x2;
	i32 e;

	i32 o = d < FLT_MIN;
	if (o) d *= (f32)(INT64_C(1) << 32) * (f32)(INT64_C(1) << 32);

	e = ilogb2kf(d * (1.0f / 0.75f));
	m = ldexp3kf(d, -e);

	if (o) e -= 64;

	x = (m - 1) / (m + 1);
	x2 = x * x;

	t = +0.4374088347e+0f;
	t = mla(t, x2, +0.5764843822e+0f);
	t = mla(t, x2, +0.9618024230e+0f);

	f32 r = mla(x2 * x, t, mla(x, +0.2885390043e+1f, static_cast<f32>(e)));

	if (xisinff(d)) r = SLEEF_INFINITYf;
	if (d < 0 || xisnanf(d)) r = SLEEF_NANf;
	if (d == 0) r = -SLEEF_INFINITYf;

	return r;
}

NO_SIDE_EFFECTS constexpr f32 xlog1pf(f32 d) {
	Sleef_float2 x, s;
	f32 m, t, x2;
	i32 e;

	f32 dp1 = d + 1;

	i32 o = dp1 < FLT_MIN;
	if (o) dp1 *= (f32)(INT64_C(1) << 32) * (f32)(INT64_C(1) << 32);

	e = ilogb2kf(dp1 * (1.0f / 0.75f));

	t = ldexp3kf(1, -e);
	m = mla(d, t, t - 1);

	if (o) e -= 64;

	x = dfdiv_f2_f2_f2(df(m, 0), dfadd_f2_f_f(2, m));
	x2 = x.x * x.x;

	t = +0.3027294874e+0f;
	t = mla(t, x2, +0.3996108174e+0f);
	t = mla(t, x2, +0.6666694880e+0f);

	s = dfmul_f2_f2_f(df(0.69314718246459960938f, -1.904654323148236017e-09f), (f32)e);
	s = dfadd_f2_f2_f2(s, dfscale_f2_f2_f(x, 2));
	s = dfadd_f2_f2_f(s, x2 * x.x * t);

	f32 r = s.x + s.y;

	if (d > 1e+38) r = SLEEF_INFINITYf;
	if (d < -1) r = SLEEF_NANf;
	if (d == -1) r = -SLEEF_INFINITYf;
	if (xisnegzerof(d)) r = -0.0f;

	return r;
}

NO_SIDE_EFFECTS constexpr f32 xcbrtf(f32 d) {
	f32 x, y, q = 1.0f;
	i32 e, r;

	e = ilogbkf(fabsfk(d)) + 1;
	d = ldexp2kf(d, -e);
	r = (e + 6144) % 3;
	q = (r == 1) ? 1.2599210498948731647672106f : q;
	q = (r == 2) ? 1.5874010519681994747517056f : q;
	q = ldexp2kf(q, (e + 6144) / 3 - 2048);

	q = mulsignf(q, d);
	d = fabsfk(d);

	x = -0.601564466953277587890625f;
	x = mla(x, d, 2.8208892345428466796875f);
	x = mla(x, d, -5.532182216644287109375f);
	x = mla(x, d, 5.898262500762939453125f);
	x = mla(x, d, -3.8095417022705078125f);
	x = mla(x, d, 2.2241256237030029296875f);

	y = d * x * x;
	y = (y - (2.0f / 3.0f) * y * (y * x - 1.0f)) * q;

	return y;
}

NO_SIDE_EFFECTS constexpr f32 xcbrtf_u1(f32 d) {
	f32 x, y, z;
	Sleef_float2 q2 = df(1, 0), u, v;
	i32 e, r;

	e = ilogbkf(fabsfk(d)) + 1;
	d = ldexp2kf(d, -e);
	r = (e + 6144) % 3;
	q2 = (r == 1) ? df(1.2599210739135742188, -2.4018701694217270415e-08) : q2;
	q2 = (r == 2) ? df(1.5874010324478149414, 1.9520385308169352356e-08) : q2;

	q2.x = mulsignf(q2.x, d); q2.y = mulsignf(q2.y, d);
	d = fabsfk(d);

	x = -0.601564466953277587890625f;
	x = mla(x, d, 2.8208892345428466796875f);
	x = mla(x, d, -5.532182216644287109375f);
	x = mla(x, d, 5.898262500762939453125f);
	x = mla(x, d, -3.8095417022705078125f);
	x = mla(x, d, 2.2241256237030029296875f);

	y = x * x; y = y * y; x -= (d * y - x) * (1.0f / 3.0f);

	z = x;

	u = dfmul_f2_f_f(x, x);
	u = dfmul_f2_f2_f2(u, u);
	u = dfmul_f2_f2_f(u, d);
	u = dfadd2_f2_f2_f(u, -x);
	y = u.x + u.y;

	y = -2.0f / 3.0f * y * z;
	v = dfadd2_f2_f2_f(dfmul_f2_f_f(z, z), y);
	v = dfmul_f2_f2_f(v, d);
	v = dfmul_f2_f2_f2(v, q2);
	z = ldexp2kf(v.x + v.y, (e + 6144) / 3 - 2048);

	if (xisinff(d)) { z = mulsignf(SLEEF_INFINITYf, q2.x); }
	if (d == 0) { z = mulsignf(0, q2.x); }

	return z;
}

NO_SIDE_EFFECTS constexpr f32 xfabsf(f32 x) { return fabsfk(x); }

NO_SIDE_EFFECTS constexpr f32 xcopysignf(f32 x, f32 y) { return copysignfk(x, y); }

NO_SIDE_EFFECTS constexpr f32 xfmaxf(f32 x, f32 y) {
	return y != y ? x : (x > y ? x : y);
}

NO_SIDE_EFFECTS constexpr f32 xfminf(f32 x, f32 y) {
	return y != y ? x : (x < y ? x : y);
}

NO_SIDE_EFFECTS constexpr f32 xfdimf(f32 x, f32 y) {
	f32 ret = x - y;
	if (ret < 0 || x == y) ret = 0;
	return ret;
}

NO_SIDE_EFFECTS constexpr f32 xtruncf(f32 x) {
	f32 fr = x - (i32)x;
	return (xisinff(x) || fabsfk(x) >= (f32)(INT64_C(1) << 23)) ? x : copysignfk(x - fr, x);
}

NO_SIDE_EFFECTS constexpr f32 xfloorf(f32 x) {
	f32 fr = x - (i32)x;
	fr = fr < 0 ? fr + 1.0f : fr;
	return (xisinff(x) || fabsfk(x) >= (f32)(INT64_C(1) << 23)) ? x : copysignfk(x - fr, x);
}

NO_SIDE_EFFECTS constexpr f32 xceilf(f32 x) {
	f32 fr = x - (i32)x;
	fr = fr <= 0 ? fr : fr - 1.0f;
	return (xisinff(x) || fabsfk(x) >= (f32)(INT64_C(1) << 23)) ? x : copysignfk(x - fr, x);
}

NO_SIDE_EFFECTS constexpr f32 xroundf(f32 d) {
	f32 x = d + 0.5f;
	f32 fr = x - (i32)x;
	if (fr == 0 && x <= 0) x--;
	fr = fr < 0 ? fr + 1.0f : fr;
	x = d == 0.4999999701976776123f ? 0 : x; // nextafterf(0.5, 0)
	return (xisinff(d) || fabsfk(d) >= (f32)(INT64_C(1) << 23)) ? d : copysignfk(x - fr, d);
}

NO_SIDE_EFFECTS constexpr f32 xrintf(f32 d) {
	f32 x = d + 0.5f;
	i32 isodd = (1 & (i32)x) != 0;
	f32 fr = x - (i32)x;
	fr = (fr < 0 || (fr == 0 && isodd)) ? fr + 1.0f : fr;
	x = d == 0.50000005960464477539f ? 0 : x; // nextafterf(0.5, 1)
	return (xisinff(d) || fabsfk(d) >= (f32)(INT64_C(1) << 23)) ? d : copysignfk(x - fr, d);
}

NO_SIDE_EFFECTS constexpr Sleef_float2 xmodff(f32 x) {
	f32 fr = x - (i32)x;
	fr = fabsfk(x) > (f32)(INT64_C(1) << 23) ? 0 : fr;
	Sleef_float2 ret = { copysignfk(fr, x), copysignfk(x - fr, x) };
	return ret;
}

NO_SIDE_EFFECTS constexpr f32 xldexpf(f32 x, i32 exp) {
	if (exp > 300) exp = 300;
	if (exp < -300) exp = -300;

	i32 e0 = exp >> 2;
	if (exp < 0) e0++;
	if (-50 < exp && exp < 50) e0 = 0;
	i32 e1 = exp - (e0 << 2);

	f32 p = pow2if(e0);
	f32 ret = x * pow2if(e1) * p * p * p * p;

	return ret;
}

NO_SIDE_EFFECTS constexpr f32 xnextafterf(f32 x, f32 y) {
	f32 cxf = x == 0 ? mulsignf(0, y) : x;
	i32 cxi = BITCAST(i32,cxf);
	//memcpy(&cxi, &cxf, sizeof(cxi));
	i32 c = (cxi < 0) == (y < x);
	if (c) cxi = -(cxi ^ (1 << 31));

	if (x != y) cxi--;

	if (c) cxi = -(cxi ^ (1 << 31));

	cxf = __builtin_bit_cast(f32,cxi);
	//memcpy(&cxf, &cxi, sizeof(cxf));
	if (cxf == 0 && x != 0) cxf = mulsignf(0, x);
	if (x == 0 && y == 0) cxf = y;
	if (xisnanf(x) || xisnanf(y)) cxf = SLEEF_NANf;

	return cxf;
}

NO_SIDE_EFFECTS constexpr f32 xfrfrexpf(f32 x) {

	if (fabsfk(x) < FLT_MIN) x *= (1 << 30);

	f32 cxf = x;
	i32 cxu = BITCAST(i32,cxf);

	cxu &= ~0x7f800000U;
	cxu |= 0x3f000000U;

	//memcpy(&cxf, &cxu, sizeof(cxf));
	cxf = __builtin_bit_cast(f32,cxu);
	if (xisinff(x)) cxf = mulsignf(SLEEF_INFINITYf, x);
	if (x == 0) cxf = x;

	return cxf;
}

NO_SIDE_EFFECTS constexpr i32 xexpfrexpf(f32 x) {
	i32 ret = 0;

	if (fabsfk(x) < FLT_MIN) { x *= (1 << 30); ret = -30; }

	f32 cxf = x;
	u32 cxu = BITCAST(u32,cxf);

	//memcpy(&cxu, &cxf, sizeof(cxu));
	ret += (i32)(((cxu >> 23) & 0xff)) - 0x7e;

	if (x == 0 || xisnanf(x) || xisinff(x)) ret = 0;

	return ret;
}

NO_SIDE_EFFECTS constexpr f32 xhypotf_u05(f32 x, f32 y) {
	x = fabsfk(x);
	y = fabsfk(y);
	f32 min = fminfk(x, y), n = min;
	f32 max = fmaxfk(x, y), d = max;

	if (max < FLT_MIN) { n *= UINT64_C(1) << 24; d *= UINT64_C(1) << 24; }
	Sleef_float2 t = dfdiv_f2_f2_f2(df(n, 0), df(d, 0));
	t = dfmul_f2_f2_f(dfsqrt_f2_f2(dfadd2_f2_f2_f(dfsqu_f2_f2(t), 1)), max);
	f32 ret = t.x + t.y;
	if (xisnanf(ret)) ret = SLEEF_INFINITYf;
	if (min == 0) ret = max;
	if (xisnanf(x) || xisnanf(y)) ret = SLEEF_NANf;
	if (x == SLEEF_INFINITYf || y == SLEEF_INFINITYf) ret = SLEEF_INFINITYf;
	return ret;
}

NO_SIDE_EFFECTS constexpr f32 xhypotf_u35(f32 x, f32 y) {
	x = fabsfk(x);
	y = fabsfk(y);
	f32 min = fminfk(x, y);
	f32 max = fmaxfk(x, y);

	f32 t = min / max;
	f32 ret = max * Math::sqrt(1 + t * t);
	if (min == 0) ret = max;
	if (xisnanf(x) || xisnanf(y)) ret = SLEEF_NANf;
	if (x == SLEEF_INFINITYf || y == SLEEF_INFINITYf) ret = SLEEF_INFINITYf;
	return ret;
}

NO_SIDE_EFFECTS INLINE constexpr f32 toward0f(f32 d) {
	return d == 0 ? 0 : __builtin_bit_cast(f32,BITCAST(i32,d) - 1);
}

NO_SIDE_EFFECTS INLINE constexpr f32 ptruncf(f32 x) {
	return fabsfk(x) >= (f32)(INT64_C(1) << 23) ? x : (x - (x - (i32)x));
}

NO_SIDE_EFFECTS constexpr f32 xfmodf(f32 x, f32 y) {
	f32 nu = fabsfk(x), de = fabsfk(y), s = 1, q;
	if (de < FLT_MIN) { nu *= UINT64_C(1) << 25; de *= UINT64_C(1) << 25; s = 1.0f / (UINT64_C(1) << 25); }
	Sleef_float2 r = df(nu, 0);
	f32 rde = toward0f(1.0f / de);

	for (i32 i = 0; i < 8; i++) { // ceil(log2(FLT_MAX) / 22)+1
		q = ptruncf(toward0f(r.x) * rde);
		q = (3 * de > r.x && r.x >= de) ? 2 : q;
		q = (2 * de > r.x && r.x >= de) ? 1 : q;
		r = dfnormalize_f2_f2(dfadd2_f2_f2_f2(r, dfmul_f2_f_f(q, -de)));
		if (r.x < de) break;
	}

	f32 ret = (r.x + r.y) * s;
	if (r.x + r.y == de) ret = 0;
	ret = mulsignf(ret, x);
	if (nu < de) ret = x;
	if (de == 0) ret = SLEEF_NANf;

	return ret;
}

NO_SIDE_EFFECTS INLINE constexpr f32 rintfk2(f32 d) {
	f32 x = d + 0.5f;
	i32 isodd = (1 & (i32)x) != 0;
	f32 fr = x - (i32)x;
	fr = (fr < 0 || (fr == 0 && isodd)) ? fr + 1.0f : fr;
	return (fabsfk(d) >= (f32)(INT64_C(1) << 23)) ? d : copysignfk(x - fr, d);
}

NO_SIDE_EFFECTS constexpr f32 xremainderf(f32 x, f32 y) {
	f32 n = fabsfk(x), d = fabsfk(y), s = 1, q;
	if (d < FLT_MIN * 2) { n *= UINT64_C(1) << 25; d *= UINT64_C(1) << 25; s = 1.0f / (UINT64_C(1) << 25); }
	f32 rd = 1.0f / d;
	Sleef_float2 r = df(n, 0);
	i32 qisodd = 0;

	for (i32 i = 0; i < 8; i++) { // ceil(log2(FLT_MAX) / 22)+1
		q = rintfk2(r.x * rd);
		if (fabsfk(r.x) < 1.5f * d) q = r.x < 0 ? -1.f : 1.f;
		if (fabsfk(r.x) < 0.5f * d || (fabsfk(r.x) == 0.5f * d && !qisodd)) q = 0;
		if (q == 0) break;
		if (xisinff(q * -d)) q = q + mulsignf(-1, r.x);
		qisodd ^= (1 & (i32)q) != 0 && fabsfk(q) < (f32)(INT64_C(1) << 24);
		r = dfnormalize_f2_f2(dfadd2_f2_f2_f2(r, dfmul_f2_f_f(q, -d)));
	}

	f32 ret = r.x * s;
	ret = mulsignf(ret, x);
	if (xisinff(y)) ret = xisinff(x) ? SLEEF_NANf : x;
	if (d == 0) ret = SLEEF_NANf;

	return ret;
}

NO_SIDE_EFFECTS constexpr f32 xsqrtf_u05(f32 d) {
	f32 q = 0.5f;

	d = d < 0 ? SLEEF_NANf : d;

	if (d < 5.2939559203393770e-23f) {
		d *= 1.8889465931478580e+22f;
		q = 7.2759576141834260e-12f * 0.5f;
	}

	if (d > 1.8446744073709552e+19f) {
		d *= 5.4210108624275220e-20f;
		q = 4294967296.0f * 0.5f;
	}

	// http://en.wikipedia.org/wiki/Fast_inverse_square_root
	f32 x = __builtin_bit_cast(f32,0x5f375a86 - (BITCAST(i32,d + 1e-45f) >> 1));

	x = x * (1.5f - 0.5f * d * x * x);
	x = x * (1.5f - 0.5f * d * x * x);
	x = x * (1.5f - 0.5f * d * x * x) * d;

	Sleef_float2 d2 = dfmul_f2_f2_f2(dfadd2_f2_f_f2(d, dfmul_f2_f_f(x, x)), dfrec_f2_f(x));

	f32 ret = (d2.x + d2.y) * q;

	ret = d == SLEEF_INFINITYf ? SLEEF_INFINITYf : ret;
	ret = d == 0 ? d : ret;

	return ret;
}

NO_SIDE_EFFECTS constexpr f32 xsqrtf_u35(f32 d) {
	f32 q = 1.0f;

	d = d < 0 ? SLEEF_NANf : d;

	if (d < 5.2939559203393770e-23f) {
		d *= 1.8889465931478580e+22f;
		q = 7.2759576141834260e-12f;
	}

	if (d > 1.8446744073709552e+19f) {
		d *= 5.4210108624275220e-20f;
		q = 4294967296.0f;
	}

	// http://en.wikipedia.org/wiki/Fast_inverse_square_root
	f32 x = __builtin_bit_cast(f32,0x5f375a86 - (BITCAST(i32,d + 1e-45f) >> 1));

	x = x * (1.5f - 0.5f * d * x * x);
	x = x * (1.5f - 0.5f * d * x * x);
	x = x * (1.5f - 0.5f * d * x * x);
	x = x * (1.5f - 0.5f * d * x * x);

	return d == SLEEF_INFINITYf ? SLEEF_INFINITYf : (x * d * q);
}

NO_SIDE_EFFECTS constexpr f32 xsqrtf(f32 d) { return Math::sqrt(d); }

NO_SIDE_EFFECTS constexpr f32 xfmaf(f32 x, f32 y, f32 z) {
	f32 h2 = x * y + z, q = 1;
	if (fabsfk(h2) < 1e-38f) {
		const f32 c0 = 1 << 25, c1 = c0 * c0, c2 = c1 * c1;
		x *= c1;
		y *= c1;
		z *= c2;
		q = 1.0f / c2;
	}
	if (fabsfk(h2) > 1e+38f) {
		const f32 c0 = 1 << 25, c1 = c0 * c0, c2 = c1 * c1;
		x *= 1.0 / c1;
		y *= 1.0 / c1;
		z *= 1.0 / c2;
		q = c2;
	}
	Sleef_float2 d = dfmul_f2_f_f(x, y);
	d = dfadd2_f2_f2_f(d, z);
	f32 ret = (x == 0 || y == 0) ? z : (d.x + d.y);
	if (xisinff(z) && !xisinff(x) && !xisnanf(x) && !xisinff(y) && !xisnanf(y)) h2 = z;
	return (xisinff(h2) || xisnanf(h2)) ? h2 : ret * q;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 sinpifk(f32 d) {
	f32 u, s, t;
	Sleef_float2 x, s2;

	u = d * 4;
	i32 q = ceilfk(u) & ~1;
	i32 o = (q & 2) != 0;

	s = u - (f32)q;
	t = s;
	s = s * s;
	s2 = dfmul_f2_f_f(t, t);

	//

	u = o ? -0.2430611801e-7f : +0.3093842054e-6f;
	u = mla(u, s, o ? +0.3590577080e-5f : -0.3657307388e-4f);
	u = mla(u, s, o ? -0.3259917721e-3f : +0.2490393585e-2f);
	x = dfadd2_f2_f_f2(u * s, o ? df(0.015854343771934509277f, 4.4940051354032242811e-10f) :
					   df(-0.080745510756969451904f, -1.3373665339076936258e-09f));
	x = dfadd2_f2_f2_f2(dfmul_f2_f2_f2(s2, x), o ? df(-0.30842512845993041992f, -9.0728339030733922277e-09f) :
						df(0.78539818525314331055f, -2.1857338617566484855e-08f));

	x = dfmul_f2_f2_f2(x, o ? s2 : df(t, 0));
	x = o ? dfadd2_f2_f2_f(x, 1) : x;

	//

	if ((q & 4) != 0) { x.x = -x.x; x.y = -x.y; }

	return x;
}

NO_SIDE_EFFECTS constexpr f32 xsinpif_u05(f32 d) {
	Sleef_float2 x = sinpifk(d);
	f32 r = x.x + x.y;

	if (xisnegzerof(d)) r = -0.0;
	if (fabsfk(d) > TRIGRANGEMAX4f) r = 0;
	if (xisinff(d)) r = SLEEF_NANf;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 cospifk(f32 d) {
	f32 u, s, t;
	Sleef_float2 x, s2;

	u = d * 4;
	i32 q = ceilfk(u) & ~1;
	i32 o = (q & 2) == 0;

	s = u - (f32)q;
	t = s;
	s = s * s;
	s2 = dfmul_f2_f_f(t, t);

	//

	u = o ? -0.2430611801e-7f : +0.3093842054e-6f;
	u = mla(u, s, o ? +0.3590577080e-5f : -0.3657307388e-4f);
	u = mla(u, s, o ? -0.3259917721e-3f : +0.2490393585e-2f);
	x = dfadd2_f2_f_f2(u * s, o ? df(0.015854343771934509277f, 4.4940051354032242811e-10f) :
					   df(-0.080745510756969451904f, -1.3373665339076936258e-09f));
	x = dfadd2_f2_f2_f2(dfmul_f2_f2_f2(s2, x), o ? df(-0.30842512845993041992f, -9.0728339030733922277e-09f) :
						df(0.78539818525314331055f, -2.1857338617566484855e-08f));

	x = dfmul_f2_f2_f2(x, o ? s2 : df(t, 0));
	x = o ? dfadd2_f2_f2_f(x, 1) : x;

	//

	if (((q + 2) & 4) != 0) { x.x = -x.x; x.y = -x.y; }

	return x;
}

NO_SIDE_EFFECTS constexpr f32 xcospif_u05(f32 d) {
	Sleef_float2 x = cospifk(d);
	f32 r = x.x + x.y;

	if (fabsfk(d) > TRIGRANGEMAX4f) r = 1;
	if (xisinff(d)) r = SLEEF_NANf;

	return r;
}

typedef struct {
	Sleef_float2 a, b;
} df2;

NO_SIDE_EFFECTS constexpr df2 gammafk(f32 a) {
	Sleef_float2 clc = df(0, 0), clln = df(1, 0), clld = df(1, 0), x, y, z;
	f32 t, u;

	i32 otiny = fabsfk(a) < 1e-30f, oref = a < 0.5f;

	x = otiny ? df(0, 0) : (oref ? dfadd2_f2_f_f(1, -a) : df(a, 0));

	i32 o0 = (0.5f <= x.x && x.x <= 1.2), o2 = 2.3 < x.x;

	y = dfnormalize_f2_f2(dfmul_f2_f2_f2(dfadd2_f2_f2_f(x, 1), x));
	y = dfnormalize_f2_f2(dfmul_f2_f2_f2(dfadd2_f2_f2_f(x, 2), y));

	clln = (o2 && x.x <= 7) ? y : clln;

	x = (o2 && x.x <= 7) ? dfadd2_f2_f2_f(x, 3) : x;
	t = o2 ? (1.0f / x.x) : dfnormalize_f2_f2(dfadd2_f2_f2_f(x, o0 ? -1.f : -2.f)).x;

	u = o2 ? +0.000839498720672087279971000786f : (o0 ? +0.9435157776e+0f : +0.1102489550e-3f);
	u = mla(u, t, o2 ? -5.17179090826059219329394422e-05f : (o0 ? +0.8670063615e+0f : +0.8160019934e-4f));
	u = mla(u, t, o2 ? -0.000592166437353693882857342347f : (o0 ? +0.4826702476e+0f : +0.1528468856e-3f));
	u = mla(u, t, o2 ? +6.97281375836585777403743539e-05f : (o0 ? -0.8855129778e-1f : -0.2355068718e-3f));
	u = mla(u, t, o2 ? +0.000784039221720066627493314301f : (o0 ? +0.1013825238e+0f : +0.4962242092e-3f));
	u = mla(u, t, o2 ? -0.000229472093621399176949318732f : (o0 ? -0.1493408978e+0f : -0.1193488017e-2f));
	u = mla(u, t, o2 ? -0.002681327160493827160473958490f : (o0 ? +0.1697509140e+0f : +0.2891599433e-2f));
	u = mla(u, t, o2 ? +0.003472222222222222222175164840f : (o0 ? -0.2072454542e+0f : -0.7385451812e-2f));
	u = mla(u, t, o2 ? +0.083333333333333333335592087900f : (o0 ? +0.2705872357e+0f : +0.2058077045e-1f));

	y = dfmul_f2_f2_f2(dfadd2_f2_f2_f(x, -0.5), logk2f(x));
	y = dfadd2_f2_f2_f2(y, dfneg_f2_f2(x));
	y = dfadd2_f2_f2_f2(y, dfx(0.91893853320467278056f)); // 0.5*log(2*Math::pi<f64>)

	z = dfadd2_f2_f2_f(dfmul_f2_f_f(u, t), o0 ? -0.400686534596170958447352690395e+0f : -0.673523028297382446749257758235e-1f);
	z = dfadd2_f2_f2_f(dfmul_f2_f2_f(z, t), o0 ? +0.822466960142643054450325495997e+0f : +0.322467033928981157743538726901e+0f);
	z = dfadd2_f2_f2_f(dfmul_f2_f2_f(z, t), o0 ? -0.577215665946766039837398973297e+0f : +0.422784335087484338986941629852e+0f);
	z = dfmul_f2_f2_f(z, t);

	clc = o2 ? y : z;

	clld = o2 ? dfadd2_f2_f2_f(dfmul_f2_f_f(u, t), 1) : clld;

	y = clln;

	clc = otiny ? dfx(41.58883083359671856503f) : // log(2^60)
		(oref ? dfadd2_f2_f2_f2(dfx(1.1447298858494001639f), dfneg_f2_f2(clc)) : clc); // log(Math::pi<f64>)
	clln = otiny ? df(1, 0) : (oref ? clln : clld);

	if (oref) x = dfmul_f2_f2_f2(clld, sinpifk(a - (f32)(INT64_C(1) << 12) * (i32)(a * (1.0 / (INT64_C(1) << 12)))));

	clld = otiny ? df(a * ((INT64_C(1) << 30) * (f32)(INT64_C(1) << 30)), 0) : (oref ? x : y);

	df2 ret = { clc, dfdiv_f2_f2_f2(clln, clld) };

	return ret;
}

NO_SIDE_EFFECTS constexpr f32 xtgammaf_u1(f32 a) {
	df2 d = gammafk(a);
	Sleef_float2 y = dfmul_f2_f2_f2(expk2f(d.a), d.b);
	f32 r = y.x + y.y;
	r = (a == -SLEEF_INFINITYf || (a < 0 && xisintf(a)) || (xisnumberf(a) && a < 0 && xisnanf(r))) ? SLEEF_NANf : r;
	r = ((a == SLEEF_INFINITYf || xisnumberf(a)) && a >= -FLT_MIN && (a == 0 || a > 36 || xisnanf(r))) ? mulsignf(SLEEF_INFINITYf, a) : r;
	return r;
}

NO_SIDE_EFFECTS constexpr f32 xlgammaf_u1(f32 a) {
	df2 d = gammafk(a);
	Sleef_float2 y = dfadd2_f2_f2_f2(d.a, logk2f(dfabs_f2_f2(d.b)));
	f32 r = y.x + y.y;
	r = (xisinff(a) || (a <= 0 && xisintf(a)) || (xisnumberf(a) && xisnanf(r))) ? SLEEF_INFINITYf : r;
	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 dfmla(f32 x, Sleef_float2 y, Sleef_float2 z) {
	return dfadd2_f2_f2_f2(z, dfmul_f2_f2_f(y, x));
}
NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 poly2df_b(f32 x, Sleef_float2 c1, Sleef_float2 c0) { return dfmla(x, c1, c0); }
NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 poly2df(f32 x, f32 c1, Sleef_float2 c0) { return dfmla(x, df(c1, 0), c0); }
NO_SIDE_EFFECTS INLINE constexpr Sleef_float2 poly4df(f32 x, f32 c3, Sleef_float2 c2, Sleef_float2 c1, Sleef_float2 c0) {
	return dfmla(x * x, poly2df(x, c3, c2), poly2df_b(x, c1, c0));
}

NO_SIDE_EFFECTS constexpr f32 xerff_u1(f32 a) {
	f32 t, x = fabsfk(a);
	Sleef_float2 t2;
	f32 x2 = x * x, x4 = x2 * x2;

	if (x < 2.5) {
		// Abramowitz and Stegun
		t = POLY6(x, x2, x4,
				  -0.4360447008e-6f,
				  +0.6867515367e-5f,
				  -0.3045156700e-4f,
				  +0.9808536561e-4f,
				  +0.2395523916e-3f,
				  +0.1459901541e-3f);
		t2 = poly4df(x, t,
					 df(0.0092883445322513580322f, -2.7863745897025330755e-11f),
					 df(0.042275499552488327026f, 1.3461399289988106057e-09f),
					 df(0.070523701608180999756f, -3.6616309318707365163e-09f));
		t2 = dfadd_f2_f_f2(1, dfmul_f2_f2_f(t2, x));
		t2 = dfsqu_f2_f2(t2);
		t2 = dfsqu_f2_f2(t2);
		t2 = dfsqu_f2_f2(t2);
		t2 = dfsqu_f2_f2(t2);
		t2 = dfrec_f2_f2(t2);
	}
	else if (x > 4.0) {
		t2 = df(0, 0);
	}
	else {
		t = POLY6(x, x2, x4,
				  -0.1130012848e-6f,
				  +0.4115272986e-5f,
				  -0.6928304356e-4f,
				  +0.7172692567e-3f,
				  -0.5131045356e-2f,
				  +0.2708637156e-1f);
		t2 = poly4df(x, t,
					 df(-0.11064319312572479248f, 3.7050452777225283007e-09f),
					 df(-0.63192230463027954102f, -2.0200432585073177859e-08f),
					 df(-1.1296638250350952148f, 2.5515120196453259252e-08f));
		t2 = dfmul_f2_f2_f(t2, x);
		t2 = df(expkf(t2), 0);
	}

	t2 = dfadd2_f2_f2_f(t2, -1);

	if (x < 1e-4) t2 = dfmul_f2_f2_f(df(-1.1283792257308959961f, 5.8635383422197591097e-08f), x);
	return mulsignf(a == 0 ? 0 : (xisinff(a) ? 1 : (-t2.x - t2.y)), a);
}

NO_SIDE_EFFECTS constexpr f32 xerfcf_u15(f32 a) {
	f32 s = a, r = 0, t;
	Sleef_float2 u, d, x;
	a = fabsfk(a);
	i32 o0 = a < 1.0f, o1 = a < 2.2f, o2 = a < 4.3f, o3 = a < 10.1f;
	u = o1 ? df(a, 0) : dfdiv_f2_f2_f2(df(1, 0), df(a, 0));

	t = o0 ? -0.8638041618e-4f : o1 ? -0.6236977242e-5f : o2 ? -0.3869504035e+0f : +0.1115344167e+1f;
	t = mla(t, u.x, o0 ? +0.6000166177e-3f : o1 ? +0.5749821503e-4f : o2 ? +0.1288077235e+1f : -0.9454904199e+0f);
	t = mla(t, u.x, o0 ? -0.1665703603e-2f : o1 ? +0.6002851478e-5f : o2 ? -0.1816803217e+1f : -0.3667259514e+0f);
	t = mla(t, u.x, o0 ? +0.1795156277e-3f : o1 ? -0.2851036377e-2f : o2 ? +0.1249150872e+1f : +0.7155663371e+0f);
	t = mla(t, u.x, o0 ? +0.1914106123e-1f : o1 ? +0.2260518074e-1f : o2 ? -0.1328857988e+0f : -0.1262947265e-1f);

	d = dfmul_f2_f2_f(u, t);
	d = dfadd2_f2_f2_f2(d, o0 ? dfx(-0.102775359343930288081655368891e+0f) :
						o1 ? dfx(-0.105247583459338632253369014063e+0f) :
						o2 ? dfx(-0.482365310333045318680618892669e+0f) :
						dfx(-0.498961546254537647970305302739e+0f));
	d = dfmul_f2_f2_f2(d, u);
	d = dfadd2_f2_f2_f2(d, o0 ? dfx(-0.636619483208481931303752546439e+0f) :
						o1 ? dfx(-0.635609463574589034216723775292e+0f) :
						o2 ? dfx(-0.134450203224533979217859332703e-2f) :
						dfx(-0.471199543422848492080722832666e-4f));
	d = dfmul_f2_f2_f2(d, u);
	d = dfadd2_f2_f2_f2(d, o0 ? dfx(-0.112837917790537404939545770596e+1f) :
						o1 ? dfx(-0.112855987376668622084547028949e+1f) :
						o2 ? dfx(-0.572319781150472949561786101080e+0f) :
						dfx(-0.572364030327966044425932623525e+0f));

	x = dfmul_f2_f2_f(o1 ? d : df(-a, 0), a);
	x = o1 ? x : dfadd2_f2_f2_f2(x, d);

	x = expk2f(x);
	x = o1 ? x : dfmul_f2_f2_f2(x, u);

	r = o3 ? (x.x + x.y) : 0;
	if (s < 0) r = 2 - r;
	r = xisnanf(s) ? SLEEF_NANf : r;
	return r;
}

constexpr f64 Sleef_rempitabdp[] = {
 0.15915494309189531785, 1.7916237278037667488e-17, 2.5454160968749269937e-33, 2.1132476107887107169e-49,
 0.03415494309189533173, 4.0384494702232122736e-18, 1.0046721413651383112e-33, 2.1132476107887107169e-49,
 0.03415494309189533173, 4.0384494702232122736e-18, 1.0046721413651383112e-33, 2.1132476107887107169e-49,
 0.0029049430918953351999, 5.6900251826959904774e-19, 4.1707169171520598517e-35, -2.496415728504571394e-51,
 0.0029049430918953351999, 5.6900251826959904774e-19, 4.1707169171520598517e-35, -2.496415728504571394e-51,
 0.0029049430918953351999, 5.6900251826959904774e-19, 4.1707169171520598517e-35, -2.496415728504571394e-51,
 0.0029049430918953351999, 5.6900251826959904774e-19, 4.1707169171520598517e-35, -2.496415728504571394e-51,
 0.00095181809189533563356, 1.3532164927539732229e-19, -6.4410794381603004826e-36, 1.7634898158762436344e-52,
 0.00095181809189533563356, 1.3532164927539732229e-19, -6.4410794381603004826e-36, 1.7634898158762436344e-52,
 0.00046353684189533574198, 2.6901432026846872871e-20, -4.2254836195018827479e-37, 9.301187206862134399e-54,
 0.00021939621689533574198, 2.6901432026846872871e-20, -4.2254836195018827479e-37, 9.301187206862134399e-54,
 9.7325904395335769087e-05, -2.0362228529073840241e-22, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 3.6290748145335769087e-05, -2.0362228529073840241e-22, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 5.7731700203357690874e-06, -2.0362228529073840241e-22, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 5.7731700203357690874e-06, -2.0362228529073840241e-22, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 5.7731700203357690874e-06, -2.0362228529073840241e-22, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 1.9584727547107690874e-06, -2.0362228529073840241e-22, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 5.1124121898268875627e-08, 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 5.1124121898268875627e-08, 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 5.1124121898268875627e-08, 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 5.1124121898268875627e-08, 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 5.1124121898268875627e-08, 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 5.1124121898268875627e-08, 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369025999e-57,
 2.1321799510573569745e-08, 1.5185066224124613304e-24, 2.6226236120327253511e-40, 2.6283399642369025999e-57,
 6.4206383167259151492e-09, -1.3585460269359374382e-25, -1.3244127270701094468e-41, -2.4695541513869446866e-57,
 6.4206383167259151492e-09, -1.3585460269359374382e-25, -1.3244127270701094468e-41, -2.4695541513869446866e-57,
 2.6953480182640010867e-09, -1.3585460269359374382e-25, -1.3244127270701094468e-41, -2.4695541513869446866e-57,
 8.3270286903304384868e-10, 7.0940550444663151936e-26, 9.7147467687967058732e-42, 7.9392906424978921242e-59,
 8.3270286903304384868e-10, 7.0940550444663151936e-26, 9.7147467687967058732e-42, 7.9392906424978921242e-59,
 3.6704158172530459087e-10, 7.0940550444663151936e-26, 9.7147467687967058732e-42, 7.9392906424978921242e-59,
 1.3421093807143501366e-10, 1.9241762160098927996e-26, 3.9750282589222551507e-42, 7.9392906424978921242e-59,
 1.7795616244500218596e-11, -1.452834466126541428e-28, -1.5869767474823787636e-44, -2.6168913164368963837e-61,
 1.7795616244500218596e-11, -1.452834466126541428e-28, -1.5869767474823787636e-44, -2.6168913164368963837e-61,
 1.7795616244500218596e-11, -1.452834466126541428e-28, -1.5869767474823787636e-44, -2.6168913164368963837e-61,
 3.2437010161333667893e-12, -1.452834466126541428e-28, -1.5869767474823787636e-44, -2.6168913164368963837e-61,
 3.2437010161333667893e-12, -1.452834466126541428e-28, -1.5869767474823787636e-44, -2.6168913164368963837e-61,
 3.2437010161333667893e-12, -1.452834466126541428e-28, -1.5869767474823787636e-44, -2.6168913164368963837e-61,
 1.4247116125875099096e-12, 2.5861333686050385673e-28, 2.8971783383570358633e-44, -2.6168913164368963837e-61,
 5.1521691081458187359e-13, 5.6664945123924856962e-29, 6.5510079543732854985e-45, -2.6168913164368963837e-61,
 6.0469559928117805118e-14, 6.1778471897801070206e-30, 9.4581409707401690366e-46, 4.9461632249367446986e-62,
 6.0469559928117805118e-14, 6.1778471897801070206e-30, 9.4581409707401690366e-46, 4.9461632249367446986e-62,
 6.0469559928117805118e-14, 6.1778471897801070206e-30, 9.4581409707401690366e-46, 4.9461632249367446986e-62,
 3.6261410673097965595e-15, -1.3304005198798645927e-31, -1.7578597149294783985e-47, 8.4432539107728104262e-64,
 3.6261410673097965595e-15, -1.3304005198798645927e-31, -1.7578597149294783985e-47, 8.4432539107728104262e-64,
 3.6261410673097965595e-15, -1.3304005198798645927e-31, -1.7578597149294783985e-47, 8.4432539107728104262e-64,
 3.6261410673097965595e-15, -1.3304005198798645927e-31, -1.7578597149294783985e-47, 8.4432539107728104262e-64,
 7.3427388509295482183e-17, 1.4871367740953237822e-32, -1.1571307704883330232e-48, -6.7249112515659578102e-65,
 7.3427388509295482183e-17, 1.4871367740953237822e-32, -1.1571307704883330232e-48, -6.7249112515659578102e-65,
 7.3427388509295482183e-17, 1.4871367740953237822e-32, -1.1571307704883330232e-48, -6.7249112515659578102e-65,
 7.3427388509295482183e-17, 1.4871367740953237822e-32, -1.1571307704883330232e-48, -6.7249112515659578102e-65,
 7.3427388509295482183e-17, 1.4871367740953237822e-32, -1.1571307704883330232e-48, -6.7249112515659578102e-65,
 7.3427388509295482183e-17, 1.4871367740953237822e-32, -1.1571307704883330232e-48, -6.7249112515659578102e-65,
 1.7916237278037667488e-17, 2.5454160968749269937e-33, 2.1132476107887107169e-49, 8.7154294504188129325e-66,
 1.7916237278037667488e-17, 2.5454160968749269937e-33, 2.1132476107887107169e-49, 8.7154294504188129325e-66,
 4.0384494702232122736e-18, 1.0046721413651383112e-33, 2.1132476107887107169e-49, 8.7154294504188129325e-66,
 4.0384494702232122736e-18, 1.0046721413651383112e-33, 2.1132476107887107169e-49, 8.7154294504188129325e-66,
 5.6900251826959904774e-19, 4.1707169171520598517e-35, -2.4964157285045710972e-51, -1.866653112309982615e-67,
 5.6900251826959904774e-19, 4.1707169171520598517e-35, -2.4964157285045710972e-51, -1.866653112309982615e-67,
 5.6900251826959904774e-19, 4.1707169171520598517e-35, -2.4964157285045710972e-51, -1.866653112309982615e-67,
 1.3532164927539732229e-19, -6.4410794381603004826e-36, 1.7634898158762432635e-52, 3.5887057810247033998e-68,
 1.3532164927539732229e-19, -6.4410794381603004826e-36, 1.7634898158762432635e-52, 3.5887057810247033998e-68,
 2.6901432026846872871e-20, -4.2254836195018827479e-37, 9.3011872068621332399e-54, 1.113250147552460308e-69,
 2.6901432026846872871e-20, -4.2254836195018827479e-37, 9.3011872068621332399e-54, 1.113250147552460308e-69,
 2.6901432026846872871e-20, -4.2254836195018827479e-37, 9.3011872068621332399e-54, 1.113250147552460308e-69,
 1.3348904870778067446e-20, -4.2254836195018827479e-37, 9.3011872068621332399e-54, 1.113250147552460308e-69,
 6.5726412927436632287e-21, 1.0820844071023395684e-36, 1.7634898158762432635e-52, 3.5887057810247033998e-68,
 3.1845095037264626247e-21, 3.2976802257607573031e-37, 9.3011872068621332399e-54, 1.113250147552460308e-69,
 1.4904436092178623228e-21, -4.6390169687056261795e-38, -1.1392999419355048437e-54, -4.587677453735884283e-71,
 6.4341066196356198368e-22, -4.6390169687056261795e-38, -1.1392999419355048437e-54, -4.587677453735884283e-71,
 2.1989418833641172011e-22, 4.7649378378726728402e-38, 9.3011872068621332399e-54, 1.113250147552460308e-69,
 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73,
 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73,
 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73,
 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73,
 8.135951522836682362e-24, 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73,
 1.5185066224124613304e-24, 2.6226236120327253511e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73,
 1.5185066224124613304e-24, 2.6226236120327253511e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73,
 1.5185066224124613304e-24, 2.6226236120327253511e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73,
 6.9132600985943383921e-25, 7.8591368887290111994e-41, 2.6283399642369020339e-57, 5.3358074162805516304e-73,
 2.7773570358292009361e-25, -1.3244127270701094468e-41, -2.4695541513869446866e-57, -3.2399200798614356002e-74,
 7.0940550444663151936e-26, 9.7147467687967058732e-42, 7.9392906424978921242e-59, 2.9745456030524896742e-75,
 7.0940550444663151936e-26, 9.7147467687967058732e-42, 7.9392906424978921242e-59, 2.9745456030524896742e-75,
 1.9241762160098927996e-26, 3.9750282589222551507e-42, 7.9392906424978921242e-59, 2.9745456030524896742e-75,
 1.9241762160098927996e-26, 3.9750282589222551507e-42, 7.9392906424978921242e-59, 2.9745456030524896742e-75,
 6.317065088957874881e-27, -3.2976062348358281152e-43, -2.6168913164368963837e-61, 3.7036201000008290615e-78,
 6.317065088957874881e-27, -3.2976062348358281152e-43, -2.6168913164368963837e-61, 3.7036201000008290615e-78,
 3.0858908211726098086e-27, 3.8770419025072344914e-43, 7.9392906424978921242e-59, 2.9745456030524896742e-75,
 1.4703036872799779898e-27, 2.8971783383570358633e-44, -2.6168913164368963837e-61, 3.7036201000008290615e-78,
 6.625101203336619011e-28, 2.8971783383570358633e-44, -2.6168913164368963837e-61, 3.7036201000008290615e-78,
 2.5861333686050385673e-28, 2.8971783383570358633e-44, -2.6168913164368963837e-61, 3.7036201000008290615e-78,
 5.6664945123924856962e-29, 6.5510079543732854985e-45, -2.6168913164368963837e-61, 3.7036201000008290615e-78,
 5.6664945123924856962e-29, 6.5510079543732854985e-45, -2.6168913164368963837e-61, 3.7036201000008290615e-78,
 6.1778471897801070206e-30, 9.4581409707401690366e-46, 4.9461632249367446986e-62, 3.7036201000008290615e-78,
 6.1778471897801070206e-30, 9.4581409707401690366e-46, 4.9461632249367446986e-62, 3.7036201000008290615e-78,
 6.1778471897801070206e-30, 9.4581409707401690366e-46, 4.9461632249367446986e-62, 3.7036201000008290615e-78,
 6.1778471897801070206e-30, 9.4581409707401690366e-46, 4.9461632249367446986e-62, 3.7036201000008290615e-78,
 3.0224035688960604996e-30, 2.451648649116083682e-46, 4.9461632249367446986e-62, 3.7036201000008290615e-78,
 1.4446817584540368888e-30, 2.451648649116083682e-46, 4.9461632249367446986e-62, 3.7036201000008290615e-78,
 6.5582085323302525856e-31, 7.0002556871006273225e-47, 1.0567786762735315635e-62, -6.1446417754639313137e-79,
 2.6139040062251944343e-31, -1.7578597149294783985e-47, 8.4432539107728090768e-64, 1.9517662449371102229e-79,
 6.4175174317266470186e-32, 4.3166913557804827486e-48, 8.4432539107728090768e-64, 1.9517662449371102229e-79,
 6.4175174317266470186e-32, 4.3166913557804827486e-48, 8.4432539107728090768e-64, 1.9517662449371102229e-79,
 1.4871367740953237822e-32, -1.1571307704883330232e-48, -6.7249112515659569668e-65, -7.2335760163150273591e-81,
 1.4871367740953237822e-32, -1.1571307704883330232e-48, -6.7249112515659569668e-65, -7.2335760163150273591e-81,
 2.5454160968749269937e-33, 2.1132476107887107169e-49, 8.7154294504188118783e-66, 1.2001823382693912203e-81,
 2.5454160968749269937e-33, 2.1132476107887107169e-49, 8.7154294504188118783e-66, 1.2001823382693912203e-81,
 2.5454160968749269937e-33, 2.1132476107887107169e-49, 8.7154294504188118783e-66, 1.2001823382693912203e-81,
 1.0046721413651383112e-33, 2.1132476107887107169e-49, 8.7154294504188118783e-66, 1.2001823382693912203e-81,
 2.3430016361024414106e-34, 4.0267819632970559834e-50, -7.8013829534098555144e-67, -1.1759240463442418271e-82,
 2.3430016361024414106e-34, 4.0267819632970559834e-50, -7.8013829534098555144e-67, -1.1759240463442418271e-82,
 4.1707169171520598517e-35, -2.4964157285045710972e-51, -1.866653112309982615e-67, 1.4185069655957361252e-83,
 4.1707169171520598517e-35, -2.4964157285045710972e-51, -1.866653112309982615e-67, 1.4185069655957361252e-83,
 4.1707169171520598517e-35, -2.4964157285045710972e-51, -1.866653112309982615e-67, 1.4185069655957361252e-83,
 1.7633044866680145008e-35, 2.8491136916798196016e-51, 4.0680767287898916022e-67, 1.4185069655957361252e-83,
 5.595982714259923599e-36, 1.7634898158762432635e-52, 3.588705781024702988e-68, 5.9489775128085140685e-84,
 5.595982714259923599e-36, 1.7634898158762432635e-52, 3.588705781024702988e-68, 5.9489775128085140685e-84,
 2.5867171761548675786e-36, 1.7634898158762432635e-52, 3.588705781024702988e-68, 5.9489775128085140685e-84,
 1.0820844071023395684e-36, 1.7634898158762432635e-52, 3.588705781024702988e-68, 5.9489775128085140685e-84,
 3.2976802257607573031e-37, 9.3011872068621332399e-54, 1.113250147552460308e-69, 2.9286284920280944778e-86,
 3.2976802257607573031e-37, 9.3011872068621332399e-54, 1.113250147552460308e-69, 2.9286284920280944778e-86,
 1.4168892644450972904e-37, 9.3011872068621332399e-54, 1.113250147552460308e-69, 2.9286284920280944778e-86,
 4.7649378378726728402e-38, 9.3011872068621332399e-54, 1.113250147552460308e-69, 2.9286284920280944778e-86,
 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 6.2960434583523738135e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 2.6226236120327253511e-40, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 7.8591368887290111994e-41, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 7.8591368887290111994e-41, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 3.2673620808294506214e-41, 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.524218473063975309e-90,
 9.7147467687967058732e-42, 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257943935e-91,
 9.7147467687967058732e-42, 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257943935e-91,
 3.9750282589222551507e-42, 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257943935e-91,
 1.1051690039850297894e-42, 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257943935e-91,
 1.1051690039850297894e-42, 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257943935e-91,
 3.8770419025072344914e-43, 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257943935e-91,
 2.8971783383570358633e-44, -2.6168913164368963837e-61, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 2.8971783383570358633e-44, -2.6168913164368963837e-61, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 2.8971783383570358633e-44, -2.6168913164368963837e-61, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 2.8971783383570358633e-44, -2.6168913164368963837e-61, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 6.5510079543732854985e-45, -2.6168913164368963837e-61, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 6.5510079543732854985e-45, -2.6168913164368963837e-61, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 9.4581409707401690366e-46, 4.9461632249367446986e-62, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 9.4581409707401690366e-46, 4.9461632249367446986e-62, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 9.4581409707401690366e-46, 4.9461632249367446986e-62, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 2.451648649116083682e-46, 4.9461632249367446986e-62, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 2.451648649116083682e-46, 4.9461632249367446986e-62, 3.7036201000008285821e-78, 5.6554937751584084315e-94,
 7.0002556871006273225e-47, 1.0567786762735315635e-62, -6.1446417754639301152e-79, -1.5355611056488084652e-94,
 7.0002556871006273225e-47, 1.0567786762735315635e-62, -6.1446417754639301152e-79, -1.5355611056488084652e-94,
 2.6211979860855749482e-47, 8.4432539107728090768e-64, 1.9517662449371099233e-79, 2.62202614552995759e-95,
 4.3166913557804827486e-48, 8.4432539107728090768e-64, 1.9517662449371099233e-79, 2.62202614552995759e-95,
 4.3166913557804827486e-48, 8.4432539107728090768e-64, 1.9517662449371099233e-79, 2.62202614552995759e-95,
 4.3166913557804827486e-48, 8.4432539107728090768e-64, 1.9517662449371099233e-79, 2.62202614552995759e-95,
 1.5797802926460750146e-48, 2.3660905534865399025e-64, -7.2335760163150273591e-81, 2.8738690232659205689e-99,
 2.1132476107887107169e-49, 8.7154294504188118783e-66, 1.2001823382693912203e-81, 2.8738690232659205689e-99,
 2.1132476107887107169e-49, 8.7154294504188118783e-66, 1.2001823382693912203e-81, 2.8738690232659205689e-99,
 2.1132476107887107169e-49, 8.7154294504188118783e-66, 1.2001823382693912203e-81, 2.8738690232659205689e-99,
 4.0267819632970559834e-50, -7.8013829534098555144e-67, -1.1759240463442418271e-82, 2.8738690232659205689e-99,
 4.0267819632970559834e-50, -7.8013829534098555144e-67, -1.1759240463442418271e-82, 2.8738690232659205689e-99,
 4.0267819632970559834e-50, -7.8013829534098555144e-67, -1.1759240463442418271e-82, 2.8738690232659205689e-99,
 1.8885701952232994665e-50, -7.8013829534098555144e-67, -1.1759240463442418271e-82, 2.8738690232659205689e-99,
 8.1946431118642097069e-51, 1.5937536410989638719e-66, 1.459625439463388979e-82, 2.8738690232659205689e-99,
 2.8491136916798196016e-51, 4.0680767287898916022e-67, 1.4185069655957361252e-83, -7.8369062883735917115e-100,
 1.7634898158762432635e-52, 3.588705781024702988e-68, 5.9489775128085131541e-84, 1.0450891972142808004e-99,
 1.7634898158762432635e-52, 3.588705781024702988e-68, 5.9489775128085131541e-84, 1.0450891972142808004e-99,
 1.7634898158762432635e-52, 3.588705781024702988e-68, 5.9489775128085131541e-84, 1.0450891972142808004e-99,
 1.7634898158762432635e-52, 3.588705781024702988e-68, 5.9489775128085131541e-84, 1.0450891972142808004e-99,
 9.3011872068621332399e-54, 1.113250147552460308e-69, 2.9286284920280941206e-86, 2.1132026692048600853e-102,
 9.3011872068621332399e-54, 1.113250147552460308e-69, 2.9286284920280941206e-86, 2.1132026692048600853e-102,
 9.3011872068621332399e-54, 1.113250147552460308e-69, 2.9286284920280941206e-86, 2.1132026692048600853e-102,
 9.3011872068621332399e-54, 1.113250147552460308e-69, 2.9286284920280941206e-86, 2.1132026692048600853e-102,
 9.3011872068621332399e-54, 1.113250147552460308e-69, 2.9286284920280941206e-86, 2.1132026692048600853e-102,
 4.0809436324633147776e-54, -4.587677453735884283e-71, -2.8859500138942368532e-87, -5.6567402911297190423e-103,
 1.470821845263904967e-54, -4.587677453735884283e-71, -2.8859500138942368532e-87, -5.6567402911297190423e-103,
 1.6576095166419998917e-55, 2.6568658093254848067e-71, 5.1571087196495574384e-87, 3.2728487032630537605e-103,
 1.6576095166419998917e-55, 2.6568658093254848067e-71, 5.1571087196495574384e-87, 3.2728487032630537605e-103,
 1.6576095166419998917e-55, 2.6568658093254848067e-71, 5.1571087196495574384e-87, 3.2728487032630537605e-103,
 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.5242184730639744369e-90, 1.145584788913072936e-105,
 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.5242184730639744369e-90, 1.145584788913072936e-105,
 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.5242184730639744369e-90, 1.145584788913072936e-105,
 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.5242184730639744369e-90, 1.145584788913072936e-105,
 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.5242184730639744369e-90, 1.145584788913072936e-105,
 2.6283399642369020339e-57, 5.3358074162805516304e-73, 4.5242184730639744369e-90, 1.145584788913072936e-105,
 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.554706987098633963e-107,
 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.554706987098633963e-107,
 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.554706987098633963e-107,
 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.554706987098633963e-107,
 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.554706987098633963e-107,
 7.9392906424978921242e-59, 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.554706987098633963e-107,
 3.9565608646667614317e-59, 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.554706987098633963e-107,
 1.9651959757511960854e-59, 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.554706987098633963e-107,
 9.6951353129341363331e-60, 7.6368645294831185015e-76, 1.0603435429602168369e-91, 1.0451839188820145747e-108,
 4.7167230906452229674e-60, 7.6368645294831185015e-76, 1.0603435429602168369e-91, 1.0451839188820145747e-108,
 2.2275169795007668372e-60, 2.1097166542226745549e-76, 4.4670685979800101779e-92, 1.0451839188820145747e-108,
 9.8291392392853877215e-61, -6.5385728340754726503e-77, -1.3520652573660833788e-93, -2.3220403312043059402e-109,
 3.6061239614242446325e-61, 7.2792968540756372162e-77, 1.3988851821689310822e-92, 1.0451839188820145747e-108,
 4.9461632249367446986e-62, 3.7036201000008285821e-78, 5.6554937751584084315e-94, -1.9306041120023063932e-110,
 4.9461632249367446986e-62, 3.7036201000008285821e-78, 5.6554937751584084315e-94, -1.9306041120023063932e-110,
 4.9461632249367446986e-62, 3.7036201000008285821e-78, 5.6554937751584084315e-94, -1.9306041120023063932e-110,
 1.0567786762735315635e-62, -6.1446417754639301152e-79, -1.535561105648808199e-94, -1.9306041120023063932e-110,
 1.0567786762735315635e-62, -6.1446417754639301152e-79, -1.535561105648808199e-94, -1.9306041120023063932e-110,
 8.4432539107728090768e-64, 1.9517662449371099233e-79, 2.62202614552995759e-95, 6.5314563001514358328e-112,
 8.4432539107728090768e-64, 1.9517662449371099233e-79, 2.62202614552995759e-95, 6.5314563001514358328e-112,
 8.4432539107728090768e-64, 1.9517662449371099233e-79, 2.62202614552995759e-95, 6.5314563001514358328e-112,
 8.4432539107728090768e-64, 1.9517662449371099233e-79, 2.62202614552995759e-95, 6.5314563001514358328e-112,
 2.3660905534865399025e-64, -7.2335760163150273591e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115,
 2.3660905534865399025e-64, -7.2335760163150273591e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115,
 8.4679971416497210292e-65, -7.2335760163150273591e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115,
 8.7154294504188118783e-66, 1.2001823382693912203e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115,
 8.7154294504188118783e-66, 1.2001823382693912203e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115,
 8.7154294504188118783e-66, 1.2001823382693912203e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115,
 8.7154294504188118783e-66, 1.2001823382693912203e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115,
 3.9676455775389135587e-66, 1.459625439463388979e-82, 2.8738690232659205689e-99, 1.8395411057335783574e-115,
 1.5937536410989638719e-66, 1.459625439463388979e-82, 2.8738690232659205689e-99, 1.8395411057335783574e-115,
 4.0680767287898916022e-67, 1.4185069655957361252e-83, -7.8369062883735917115e-100, -1.9081236411894110579e-116,
 4.0680767287898916022e-67, 1.4185069655957361252e-83, -7.8369062883735917115e-100, -1.9081236411894110579e-116,
 1.1007118082399544936e-67, 1.4185069655957361252e-83, -7.8369062883735917115e-100, -1.9081236411894110579e-116,
 1.1007118082399544936e-67, 1.4185069655957361252e-83, -7.8369062883735917115e-100, -1.9081236411894110579e-116,
 3.588705781024702988e-68, 5.9489775128085131541e-84, 1.0450891972142805974e-99, 1.8395411057335783574e-115,
 3.588705781024702988e-68, 5.9489775128085131541e-84, 1.0450891972142805974e-99, 1.8395411057335783574e-115,
 1.7341027056809927069e-68, 1.830931441234090934e-84, 1.3069928418846076386e-100, 3.1677600334418876704e-116,
 8.0680116800913756637e-69, -2.2809159455312046184e-85, -4.0748824503880445403e-101, -6.3915272253158644628e-117,
 3.4315039917320989315e-69, -2.2809159455312046184e-85, -4.0748824503880445403e-101, -6.3915272253158644628e-117,
 1.113250147552460308e-69, 2.9286284920280941206e-86, 2.1132026692048600853e-102, -4.6672632026740766185e-119,
 1.113250147552460308e-69, 2.9286284920280941206e-86, 2.1132026692048600853e-102, -4.6672632026740766185e-119,
 5.3368668650755071652e-70, 2.9286284920280941206e-86, 2.1132026692048600853e-102, -4.6672632026740766185e-119,
 2.4390495598509592076e-70, 2.9286284920280941206e-86, 2.1132026692048600853e-102, -4.6672632026740766185e-119,
 9.901409072386855505e-71, -2.8859500138942368532e-87, -5.6567402911297190423e-103, -4.6672632026740766185e-119,
 2.6568658093254848067e-71, 5.1571087196495574384e-87, 3.2728487032630532648e-103, 5.2465720993401781599e-119,
 2.6568658093254848067e-71, 5.1571087196495574384e-87, 3.2728487032630532648e-103, 5.2465720993401781599e-119,
 8.4572999356014273536e-72, 1.1355793528776598461e-87, 3.2728487032630532648e-103, 5.2465720993401781599e-119,
 8.4572999356014273536e-72, 1.1355793528776598461e-87, 3.2728487032630532648e-103, 5.2465720993401781599e-119,
 3.9294603961880721752e-72, 1.3019701118468578292e-88, -7.5747169634236195447e-105, -2.0152904854894729832e-121,
 1.6655406264813940833e-72, 1.3019701118468578292e-88, -7.5747169634236195447e-105, -2.0152904854894729832e-121,
 5.3358074162805516304e-73, 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598455046e-121,
 5.3358074162805516304e-73, 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598455046e-121,
 2.5059077041472040156e-73, 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598455046e-121,
 1.0909578480805302081e-73, 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598455046e-121,
 3.8348292004719330442e-74, 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598455046e-121,
 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.5547069870986327528e-107, 1.6304246661326865276e-122,
 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.5547069870986327528e-107, 1.6304246661326865276e-122,
 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.5547069870986327528e-107, 1.6304246661326865276e-122,
 2.9745456030524891833e-75, 5.969437008257942845e-91, 5.5547069870986327528e-107, 1.6304246661326865276e-122,
 7.6368645294831185015e-76, 1.0603435429602168369e-91, 1.0451839188820145747e-108, 4.2386081393205242443e-125,
 7.6368645294831185015e-76, 1.0603435429602168369e-91, 1.0451839188820145747e-108, 4.2386081393205242443e-125,
 2.1097166542226745549e-76, 4.4670685979800101779e-92, 1.0451839188820145747e-108, 4.2386081393205242443e-125,
 2.1097166542226745549e-76, 4.4670685979800101779e-92, 1.0451839188820145747e-108, 4.2386081393205242443e-125,
 7.2792968540756372162e-77, 1.3988851821689310822e-92, 1.0451839188820145747e-108, 4.2386081393205242443e-125,
 3.7036201000008285821e-78, 5.6554937751584084315e-94, -1.9306041120023063932e-110, 1.0223371855251472933e-126,
 3.7036201000008285821e-78, 5.6554937751584084315e-94, -1.9306041120023063932e-110, 1.0223371855251472933e-126,
 3.7036201000008285821e-78, 5.6554937751584084315e-94, -1.9306041120023063932e-110, 1.0223371855251472933e-126,
 3.7036201000008285821e-78, 5.6554937751584084315e-94, -1.9306041120023063932e-110, 1.0223371855251472933e-126,
 3.7036201000008285821e-78, 5.6554937751584084315e-94, -1.9306041120023063932e-110, 1.0223371855251472933e-126,
 1.5445779612272179051e-78, 8.6145718795359707834e-95, 7.3062078800278780675e-111, 1.0223371855251472933e-126,
 4.6505689184041232695e-79, 8.6145718795359707834e-95, 7.3062078800278780675e-111, 1.0223371855251472933e-126,
 4.6505689184041232695e-79, 8.6145718795359707834e-95, 7.3062078800278780675e-111, 1.0223371855251472933e-126,
 1.9517662449371099233e-79, 2.62202614552995759e-95, 6.5314563001514349095e-112, 9.9039323746573674262e-128,
 6.0236490820360325022e-80, -3.7424672147304925625e-96, -1.784871512364483542e-112, 6.7095375687163151728e-129,
 6.0236490820360325022e-80, -3.7424672147304925625e-96, -1.784871512364483542e-112, 6.7095375687163151728e-129,
 2.6501457402022643213e-80, 3.7482149527770239293e-96, 6.5314563001514349095e-112, 9.9039323746573674262e-128,
 9.6339406928538097998e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132,
 1.2001823382693912203e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132,
 1.2001823382693912203e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132,
 1.2001823382693912203e-81, 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132,
 1.459625439463388979e-82, 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132,
 1.459625439463388979e-82, 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132,
 1.459625439463388979e-82, 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132,
 1.4185069655957361252e-83, -7.8369062883735917115e-100, -1.9081236411894107761e-116, -2.1796760241698337334e-132,
 1.4185069655957361252e-83, -7.8369062883735917115e-100, -1.9081236411894107761e-116, -2.1796760241698337334e-132,
 1.4185069655957361252e-83, -7.8369062883735917115e-100, -1.9081236411894107761e-116, -2.1796760241698337334e-132,
 1.4185069655957361252e-83, -7.8369062883735917115e-100, -1.9081236411894107761e-116, -2.1796760241698337334e-132,
 5.9489775128085131541e-84, 1.0450891972142805974e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132,
 1.830931441234090934e-84, 1.3069928418846076386e-100, 3.1677600334418871069e-116, 3.4556869017247800778e-132,
 1.830931441234090934e-84, 1.3069928418846076386e-100, 3.1677600334418871069e-116, 3.4556869017247800778e-132,
 8.0141992334048515034e-85, 1.3069928418846076386e-100, 3.1677600334418871069e-116, 3.4556869017247800778e-132,
 2.8666416439368237283e-85, 1.6400545060233297363e-101, -4.6672632026740766185e-119, -3.755176715260116501e-136,
 2.9286284920280941206e-86, 2.1132026692048600853e-102, -4.6672632026740766185e-119, -3.755176715260116501e-136,
 2.9286284920280941206e-86, 2.1132026692048600853e-102, -4.6672632026740766185e-119, -3.755176715260116501e-136,
 2.9286284920280941206e-86, 2.1132026692048600853e-102, -4.6672632026740766185e-119, -3.755176715260116501e-136,
 2.9286284920280941206e-86, 2.1132026692048600853e-102, -4.6672632026740766185e-119, -3.755176715260116501e-136,
 1.3200167453193350837e-86, 2.1132026692048600853e-102, -4.6672632026740766185e-119, -3.755176715260116501e-136,
 5.1571087196495574384e-87, 3.2728487032630532648e-103, 5.2465720993401781599e-119, -3.755176715260116501e-136,
 1.1355793528776598461e-87, 3.2728487032630532648e-103, 5.2465720993401781599e-119, -3.755176715260116501e-136,
 1.1355793528776598461e-87, 3.2728487032630532648e-103, 5.2465720993401781599e-119, -3.755176715260116501e-136,
 1.3019701118468578292e-88, -7.5747169634236195447e-105, -2.0152904854894725532e-121, -3.1562414818576682143e-137,
 1.3019701118468578292e-88, -7.5747169634236195447e-105, -2.0152904854894725532e-121, -3.1562414818576682143e-137,
 1.3019701118468578292e-88, -7.5747169634236195447e-105, -2.0152904854894725532e-121, -3.1562414818576682143e-137,
 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598452896e-121, 1.1431992269852683481e-137,
 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598452896e-121, 1.1431992269852683481e-137,
 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598452896e-121, 1.1431992269852683481e-137,
 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598452896e-121, 1.1431992269852683481e-137,
 4.5242184730639744369e-90, 1.1455847889130727424e-105, 1.8573014293598452896e-121, 1.1431992269852683481e-137,
 5.969437008257942845e-91, 5.5547069870986327528e-107, 1.6304246661326865276e-122, 6.8339049774534162772e-139,
 5.969437008257942845e-91, 5.5547069870986327528e-107, 1.6304246661326865276e-122, 6.8339049774534162772e-139,
 5.969437008257942845e-91, 5.5547069870986327528e-107, 1.6304246661326865276e-122, 6.8339049774534162772e-139,
 1.0603435429602168369e-91, 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591188256e-141,
 1.0603435429602168369e-91, 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591188256e-141,
 1.0603435429602168369e-91, 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591188256e-141,
 4.4670685979800101779e-92, 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591188256e-141,
 1.3988851821689310822e-92, 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591188256e-141,
 1.3988851821689310822e-92, 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591188256e-141,
 6.3183932821616130831e-93, 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591188256e-141,
 2.4831640123977650651e-93, 1.9359195088038447797e-109, -4.8867691298577234423e-126, -2.0587960670007823264e-142,
 5.6554937751584084315e-94, -1.9306041120023063932e-110, 1.0223371855251471293e-126, 1.2214168761472102282e-142,
 5.6554937751584084315e-94, -1.9306041120023063932e-110, 1.0223371855251471293e-126, 1.2214168761472102282e-142,
 8.6145718795359707834e-95, 7.3062078800278780675e-111, 1.0223371855251471293e-126, 1.2214168761472102282e-142,
 8.6145718795359707834e-95, 7.3062078800278780675e-111, 1.0223371855251471293e-126, 1.2214168761472102282e-142,
 8.6145718795359707834e-95, 7.3062078800278780675e-111, 1.0223371855251471293e-126, 1.2214168761472102282e-142,
 2.62202614552995759e-95, 6.5314563001514349095e-112, 9.9039323746573674262e-128, -8.6629775332868987041e-145,
 2.62202614552995759e-95, 6.5314563001514349095e-112, 9.9039323746573674262e-128, -8.6629775332868987041e-145,
 1.1238897120284541253e-95, 6.5314563001514349095e-112, 9.9039323746573674262e-128, -8.6629775332868987041e-145,
 3.7482149527770239293e-96, 6.5314563001514349095e-112, 9.9039323746573674262e-128, -8.6629775332868987041e-145,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 2.8738690232659205689e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 1.0450891972142805974e-99, 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148,
 1.3069928418846076386e-100, 3.1677600334418871069e-116, 3.4556869017247794521e-132, 8.5448727249069983612e-148,
 1.3069928418846076386e-100, 3.1677600334418871069e-116, 3.4556869017247794521e-132, 8.5448727249069983612e-148,
 1.3069928418846076386e-100, 3.1677600334418871069e-116, 3.4556869017247794521e-132, 8.5448727249069983612e-148,
 1.6400545060233297363e-101, -4.6672632026740766185e-119, -3.755176715260116501e-136, 2.1571619860435652883e-152,
 1.6400545060233297363e-101, -4.6672632026740766185e-119, -3.755176715260116501e-136, 2.1571619860435652883e-152,
 1.6400545060233297363e-101, -4.6672632026740766185e-119, -3.755176715260116501e-136, 2.1571619860435652883e-152,
 2.1132026692048600853e-102, -4.6672632026740766185e-119, -3.755176715260116501e-136, 2.1571619860435652883e-152,
 2.1132026692048600853e-102, -4.6672632026740766185e-119, -3.755176715260116501e-136, 2.1571619860435652883e-152,
 2.1132026692048600853e-102, -4.6672632026740766185e-119, -3.755176715260116501e-136, 2.1571619860435652883e-152,
 3.2728487032630532648e-103, 5.2465720993401781599e-119, -3.755176715260116501e-136, 2.1571619860435652883e-152,
 3.2728487032630532648e-103, 5.2465720993401781599e-119, -3.755176715260116501e-136, 2.1571619860435652883e-152,
 3.2728487032630532648e-103, 5.2465720993401781599e-119, -3.755176715260116501e-136, 2.1571619860435652883e-152,
 1.0404514546648604359e-103, 2.896544483330507019e-120, 3.1239284188885823808e-136, 2.1571619860435652883e-152,
 1.0404514546648604359e-103, 2.896544483330507019e-120, 3.1239284188885823808e-136, 2.1571619860435652883e-152,
 4.8235214251531210473e-104, 2.896544483330507019e-120, 3.1239284188885823808e-136, 2.1571619860435652883e-152,
 2.0330248644053793915e-104, 2.896544483330507019e-120, 3.1239284188885823808e-136, 2.1571619860435652883e-152,
 6.3777658403150887343e-105, -2.0152904854894725532e-121, -3.156241481857667737e-137, -7.0684085473731388916e-153,
 6.3777658403150887343e-105, -2.0152904854894725532e-121, -3.156241481857667737e-137, -7.0684085473731388916e-153,
 2.88964513938041089e-105, 5.7298933442091639924e-121, -3.156241481857667737e-137, -7.0684085473731388916e-153,
 1.1455847889130727424e-105, 1.8573014293598452896e-121, 1.1431992269852681095e-137, 2.4782675885631257398e-153,
 2.7355461367940366859e-106, -7.8994528064813712419e-123, -2.0037599452814940222e-138, 9.1598554579059548847e-155,
 2.7355461367940366859e-106, -7.8994528064813712419e-123, -2.0037599452814940222e-138, 9.1598554579059548847e-155,
 5.5547069870986327528e-107, 1.6304246661326865276e-122, 6.8339049774534147855e-139, 9.1598554579059548847e-155,
 5.5547069870986327528e-107, 1.6304246661326865276e-122, 6.8339049774534147855e-139, 9.1598554579059548847e-155,
 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157,
 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157,
 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157,
 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157,
 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157,
 1.0451839188820145747e-108, 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157,
 1.9359195088038447797e-109, -4.8867691298577234423e-126, -2.0587960670007819622e-142, -2.8326669474241479263e-158,
 1.9359195088038447797e-109, -4.8867691298577234423e-126, -2.0587960670007819622e-142, -2.8326669474241479263e-158,
 1.9359195088038447797e-109, -4.8867691298577234423e-126, -2.0587960670007819622e-142, -2.8326669474241479263e-158,
 8.7142954880180709975e-110, -4.8867691298577234423e-126, -2.0587960670007819622e-142, -2.8326669474241479263e-158,
 3.3918456880078814158e-110, 6.931443500908017045e-126, 1.1062055705591186799e-141, 1.1734404793201255869e-157,
 7.3062078800278780675e-111, 1.0223371855251471293e-126, 1.2214168761472102282e-142, 8.0910098773220312367e-159,
 7.3062078800278780675e-111, 1.0223371855251471293e-126, 1.2214168761472102282e-142, 8.0910098773220312367e-159,
 6.5314563001514349095e-112, 9.9039323746573674262e-128, -8.6629775332868972816e-145, -1.5987060076657616072e-160,
 6.5314563001514349095e-112, 9.9039323746573674262e-128, -8.6629775332868972816e-145, -1.5987060076657616072e-160,
 6.5314563001514349095e-112, 9.9039323746573674262e-128, -8.6629775332868972816e-145, -1.5987060076657616072e-160,
 6.5314563001514349095e-112, 9.9039323746573674262e-128, -8.6629775332868972816e-145, -1.5987060076657616072e-160,
 2.3732923938934761454e-112, 6.7095375687163138915e-129, 1.6963686085056791706e-144, 1.2464251916751375716e-160,
 2.9421044076449630171e-113, 6.7095375687163138915e-129, 1.6963686085056791706e-144, 1.2464251916751375716e-160,
 2.9421044076449630171e-113, 6.7095375687163138915e-129, 1.6963686085056791706e-144, 1.2464251916751375716e-160,
 2.9421044076449630171e-113, 6.7095375687163138915e-129, 1.6963686085056791706e-144, 1.2464251916751375716e-160,
 3.4325196623373878948e-114, 9.3892593260023063019e-130, 9.4702132359198537748e-146, 1.7950099192230045857e-161,
 3.4325196623373878948e-114, 9.3892593260023063019e-130, 9.4702132359198537748e-146, 1.7950099192230045857e-161,
 3.4325196623373878948e-114, 9.3892593260023063019e-130, 9.4702132359198537748e-146, 1.7950099192230045857e-161,
 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148, 2.9106774506606945839e-164,
 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148, 2.9106774506606945839e-164,
 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148, 2.9106774506606945839e-164,
 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148, 2.9106774506606945839e-164,
 1.8395411057335783574e-115, -7.8150389500644475446e-132, -3.9681466199873824165e-148, 2.9106774506606945839e-164,
 8.2436437080731844263e-116, 1.4726412753514008951e-131, -3.9681466199873824165e-148, 2.9106774506606945839e-164,
 3.1677600334418871069e-116, 3.4556869017247794521e-132, 8.544872724906996972e-148, 1.6802919634942429241e-163,
 6.2981819612623816536e-117, 6.3800543877747317218e-133, 7.2423563434801054878e-149, 1.1741471776254779927e-164,
 6.2981819612623816536e-117, 6.3800543877747317218e-133, 7.2423563434801054878e-149, 1.1741471776254779927e-164,
 6.2981819612623816536e-117, 6.3800543877747317218e-133, 7.2423563434801054878e-149, 1.1741471776254779927e-164,
 3.1257546646178208289e-117, -6.6414926959353515111e-134, -5.7828074707888119584e-150, -1.2825052715093464343e-165,
 1.5395410162955400644e-117, -6.6414926959353515111e-134, -5.7828074707888119584e-150, -1.2825052715093464343e-165,
 7.4643419213439950602e-118, 1.0969016447485317626e-133, -5.7828074707888119584e-150, -1.2825052715093464343e-165,
 3.4988078005382940294e-118, 2.1637618757749825688e-134, -8.9490928918944555247e-151, -1.9717385086233606481e-166,
 1.5160407401354430737e-118, 2.1637618757749825688e-134, -8.9490928918944555247e-151, -1.9717385086233606481e-166,
 5.2465720993401781599e-119, -3.755176715260116501e-136, 2.1571619860435648643e-152, 6.3257905089784152346e-168,
 2.896544483330507019e-120, 3.1239284188885823808e-136, 2.1571619860435648643e-152, 6.3257905089784152346e-168,
 2.896544483330507019e-120, 3.1239284188885823808e-136, 2.1571619860435648643e-152, 6.3257905089784152346e-168,
 2.896544483330507019e-120, 3.1239284188885823808e-136, 2.1571619860435648643e-152, 6.3257905089784152346e-168,
 2.896544483330507019e-120, 3.1239284188885823808e-136, 2.1571619860435648643e-152, 6.3257905089784152346e-168,
 2.896544483330507019e-120, 3.1239284188885823808e-136, 2.1571619860435648643e-152, 6.3257905089784152346e-168,
 1.3475077173907800538e-120, -3.156241481857667737e-137, -7.0684085473731388916e-153, -3.3573283875161501977e-170,
 5.7298933442091639924e-121, -3.156241481857667737e-137, -7.0684085473731388916e-153, -3.3573283875161501977e-170,
 1.8573014293598452896e-121, 1.1431992269852681095e-137, 2.4782675885631257398e-153, -3.3573283875161501977e-170,
 1.8573014293598452896e-121, 1.1431992269852681095e-137, 2.4782675885631257398e-153, -3.3573283875161501977e-170,
 8.8915345064751572143e-122, 1.1431992269852681095e-137, 2.4782675885631257398e-153, -3.3573283875161501977e-170,
 4.0507946129135104481e-122, 6.8339049774534147855e-139, 9.1598554579059548847e-155, -4.5159745404911825673e-172,
 1.6304246661326865276e-122, 6.8339049774534147855e-139, 9.1598554579059548847e-155, -4.5159745404911825673e-172,
 4.2023969274227456735e-123, 6.8339049774534147855e-139, 9.1598554579059548847e-155, -4.5159745404911825673e-172,
 4.2023969274227456735e-123, 6.8339049774534147855e-139, 9.1598554579059548847e-155, -4.5159745404911825673e-172,
 1.1769344939467164447e-123, 1.1602886988632691941e-140, 3.0307583960570927356e-156, 5.8345524661064369683e-172,
 1.1769344939467164447e-123, 1.1602886988632691941e-140, 3.0307583960570927356e-156, 5.8345524661064369683e-172,
 4.2056888557770896953e-124, 1.1602886988632691941e-140, 3.0307583960570927356e-156, 5.8345524661064369683e-172,
 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174,
 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174,
 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174,
 4.2386081393205242443e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174,
 1.8749656131673758844e-125, 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174,
 6.931443500908017045e-126, 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174,
 1.0223371855251471293e-126, 1.2214168761472102282e-142, 8.0910098773220302259e-159, 1.2381024895275844856e-174,
 1.0223371855251471293e-126, 1.2214168761472102282e-142, 8.0910098773220302259e-159, 1.2381024895275844856e-174,
 1.0223371855251471293e-126, 1.2214168761472102282e-142, 8.0910098773220302259e-159, 1.2381024895275844856e-174,
 2.8369889610228834887e-127, 4.0136364036021218058e-143, -1.0134099605688458828e-159, -2.5389576707476506925e-176,
 2.8369889610228834887e-127, 4.0136364036021218058e-143, -1.0134099605688458828e-159, -2.5389576707476506925e-176,
 9.9039323746573674262e-128, -8.6629775332868972816e-145, -1.5987060076657612913e-160, -2.5389576707476506925e-176,
 6.7095375687163138915e-129, 1.6963686085056791706e-144, 1.2464251916751375716e-160, 6.197724948400014906e-177,
 6.7095375687163138915e-129, 1.6963686085056791706e-144, 1.2464251916751375716e-160, 6.197724948400014906e-177,
 6.7095375687163138915e-129, 1.6963686085056791706e-144, 1.2464251916751375716e-160, 6.197724948400014906e-177,
 6.7095375687163138915e-129, 1.6963686085056791706e-144, 1.2464251916751375716e-160, 6.197724948400014906e-177,
 9.3892593260023063019e-130, 9.4702132359198537748e-146, 1.7950099192230045857e-161, -1.6991004655691155518e-177,
 9.3892593260023063019e-130, 9.4702132359198537748e-146, 1.7950099192230045857e-161, -1.6991004655691155518e-177,
 9.3892593260023063019e-130, 9.4702132359198537748e-146, 1.7950099192230045857e-161, -1.6991004655691155518e-177,
 2.175994780857201024e-130, 1.4618808551874518553e-146, 1.6802919634942426156e-163, 2.8330093736631818036e-179,
 2.175994780857201024e-130, 1.4618808551874518553e-146, 1.6802919634942426156e-163, 2.8330093736631818036e-179,
 3.7267864457092460442e-131, 4.6083930759590139305e-147, 1.6802919634942426156e-163, 2.8330093736631818036e-179,
 3.7267864457092460442e-131, 4.6083930759590139305e-147, 1.6802919634942426156e-163, 2.8330093736631818036e-179,
 3.7267864457092460442e-131, 4.6083930759590139305e-147, 1.6802919634942426156e-163, 2.8330093736631818036e-179,
 1.4726412753514008951e-131, -3.9681466199873824165e-148, 2.9106774506606941983e-164, 5.1948630316441296498e-180,
 3.4556869017247794521e-132, 8.544872724906996972e-148, 1.6802919634942426156e-163, 2.8330093736631818036e-179,
 3.4556869017247794521e-132, 8.544872724906996972e-148, 1.6802919634942426156e-163, 2.8330093736631818036e-179,
 6.3800543877747317218e-133, 7.2423563434801054878e-149, 1.1741471776254777999e-164, 1.3389912474795152755e-180,
 6.3800543877747317218e-133, 7.2423563434801054878e-149, 1.1741471776254777999e-164, 1.3389912474795152755e-180,
 6.3800543877747317218e-133, 7.2423563434801054878e-149, 1.1741471776254777999e-164, 1.3389912474795152755e-180,
 2.8579525590905986764e-133, -5.7828074707888119584e-150, -1.2825052715093464343e-165, -1.0696067158221530218e-181,
 1.0969016447485317626e-133, -5.7828074707888119584e-150, -1.2825052715093464343e-165, -1.0696067158221530218e-181,
 2.1637618757749825688e-134, -8.9490928918944555247e-151, -1.9717385086233606481e-166, 1.3535321672928907047e-182,
 2.1637618757749825688e-134, -8.9490928918944555247e-151, -1.9717385086233606481e-166, 1.3535321672928907047e-182,
 2.1637618757749825688e-134, -8.9490928918944555247e-151, -1.9717385086233606481e-166, 1.3535321672928907047e-182,
 1.0631050543111905033e-134, 1.5490398016102376505e-150, 3.4549185946116918017e-166, 1.3535321672928907047e-182,
 5.1277664357929471499e-135, 3.2706525621039604902e-151, 7.4159004299416557678e-167, 1.3535321672928907047e-182,
 2.3761243821334675971e-135, 3.2706525621039604902e-151, 7.4159004299416557678e-167, 1.3535321672928907047e-182,
 1.0003033553037281263e-135, 2.1571619860435648643e-152, 6.3257905089784152346e-168, 3.5607241064750984115e-184,
 3.1239284188885823808e-136, 2.1571619860435648643e-152, 6.3257905089784152346e-168, 3.5607241064750984115e-184,
 3.1239284188885823808e-136, 2.1571619860435648643e-152, 6.3257905089784152346e-168, 3.5607241064750984115e-184,
 1.4041521353514076604e-136, 2.1571619860435648643e-152, 6.3257905089784152346e-168, 3.5607241064750984115e-184,
 5.4426399358282049106e-137, 2.4782675885631257398e-153, -3.3573283875161501977e-170, 3.0568054078295488291e-186,
 1.1431992269852681095e-137, 2.4782675885631257398e-153, -3.3573283875161501977e-170, 3.0568054078295488291e-186,
 1.1431992269852681095e-137, 2.4782675885631257398e-153, -3.3573283875161501977e-170, 3.0568054078295488291e-186,
 6.8339049774534147855e-139, 9.1598554579059548847e-155, -4.5159745404911819927e-172, -4.5870810097328578981e-188,
 6.8339049774534147855e-139, 9.1598554579059548847e-155, -4.5159745404911819927e-172, -4.5870810097328578981e-188,
 6.8339049774534147855e-139, 9.1598554579059548847e-155, -4.5159745404911819927e-172, -4.5870810097328578981e-188,
 6.8339049774534147855e-139, 9.1598554579059548847e-155, -4.5159745404911819927e-172, -4.5870810097328578981e-188,
 1.1602886988632691941e-140, 3.0307583960570927356e-156, 5.8345524661064358191e-172, 6.9043123899963188689e-188,
 1.1602886988632691941e-140, 3.0307583960570927356e-156, 5.8345524661064358191e-172, 6.9043123899963188689e-188,
 1.1602886988632691941e-140, 3.0307583960570927356e-156, 5.8345524661064358191e-172, 6.9043123899963188689e-188,
 1.1602886988632691941e-140, 3.0307583960570927356e-156, 5.8345524661064358191e-172, 6.9043123899963188689e-188,
 1.1602886988632691941e-140, 3.0307583960570927356e-156, 5.8345524661064358191e-172, 6.9043123899963188689e-188,
 1.1602886988632691941e-140, 3.0307583960570927356e-156, 5.8345524661064358191e-172, 6.9043123899963188689e-188,
 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174, -8.4789520282639751913e-191,
 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174, -8.4789520282639751913e-191,
 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174, -8.4789520282639751913e-191,
 1.1062055705591186799e-141, 1.1734404793201255869e-157, 1.2381024895275844856e-174, -8.4789520282639751913e-191,
 4.5016298192952031469e-142, -2.8326669474241479263e-158, 1.2381024895275844856e-174, -8.4789520282639751913e-191,
 1.2214168761472102282e-142, 8.0910098773220302259e-159, 1.2381024895275844856e-174, -8.4789520282639751913e-191,
 1.2214168761472102282e-142, 8.0910098773220302259e-159, 1.2381024895275844856e-174, -8.4789520282639751913e-191,
 4.0136364036021218058e-143, -1.0134099605688458828e-159, -2.5389576707476506925e-176, -6.2404128071707654958e-193,
 4.0136364036021218058e-143, -1.0134099605688458828e-159, -2.5389576707476506925e-176, -6.2404128071707654958e-193,
 1.9635033141346264592e-143, -1.0134099605688458828e-159, -2.5389576707476506925e-176, -6.2404128071707654958e-193,
 9.3843676940087855824e-144, 1.2626949989038732076e-159, 2.2730883653953564668e-175, 2.7431118386590483722e-191,
 4.2590349703400483539e-144, 1.2464251916751375716e-160, 6.1977249484000140293e-177, 1.1294061984896458822e-192,
 1.6963686085056791706e-144, 1.2464251916751375716e-160, 6.1977249484000140293e-177, 1.1294061984896458822e-192,
 4.1503542758849472122e-145, -1.7614040799531193879e-161, -1.6991004655691153326e-177, -1.856794109153959173e-193,
 4.1503542758849472122e-145, -1.7614040799531193879e-161, -1.6991004655691153326e-177, -1.856794109153959173e-193,
 9.4702132359198537748e-146, 1.7950099192230045857e-161, -1.6991004655691153326e-177, -1.856794109153959173e-193,
 9.4702132359198537748e-146, 1.7950099192230045857e-161, -1.6991004655691153326e-177, -1.856794109153959173e-193,
 1.4618808551874518553e-146, 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196,
 1.4618808551874518553e-146, 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196,
 1.4618808551874518553e-146, 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196,
 4.6083930759590139305e-147, 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196,
 4.6083930759590139305e-147, 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196,
 2.105789206980137775e-147, 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196,
 8.544872724906996972e-148, 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196,
 2.2883630524598079723e-148, 2.9106774506606941983e-164, 5.1948630316441287936e-180, 9.6685396110091032843e-196,
 2.2883630524598079723e-148, 2.9106774506606941983e-164, 5.1948630316441287936e-180, 9.6685396110091032843e-196,
 7.2423563434801054878e-149, 1.1741471776254777999e-164, 1.3389912474795150614e-180, 1.1067843414450286726e-196,
 7.2423563434801054878e-149, 1.1741471776254777999e-164, 1.3389912474795150614e-180, 1.1067843414450286726e-196,
 3.3320377982006123631e-149, 3.0588204110786950436e-165, 3.7502330143836152136e-181, 3.6564932749519464998e-198,
 1.3768785255608653665e-149, 3.0588204110786950436e-165, 3.7502330143836152136e-181, 3.6564932749519464998e-198,
 3.9929888924099219388e-150, -1.9717385086233606481e-166, 1.3535321672928907047e-182, 3.1205762277848031878e-199,
 3.9929888924099219388e-150, -1.9717385086233606481e-166, 1.3535321672928907047e-182, 3.1205762277848031878e-199,
 1.5490398016102376505e-150, 3.4549185946116918017e-166, 1.3535321672928907047e-182, 3.1205762277848031878e-199,
 3.2706525621039604902e-151, 7.4159004299416557678e-167, 1.3535321672928907047e-182, 3.1205762277848031878e-199,
 3.2706525621039604902e-151, 7.4159004299416557678e-167, 1.3535321672928907047e-182, 3.1205762277848031878e-199,
 2.1571619860435648643e-152, 6.3257905089784152346e-168, 3.5607241064750984115e-184, -1.4832196127821708615e-201,
 2.1571619860435648643e-152, 6.3257905089784152346e-168, 3.5607241064750984115e-184, -1.4832196127821708615e-201,
 2.1571619860435648643e-152, 6.3257905089784152346e-168, 3.5607241064750984115e-184, -1.4832196127821708615e-201,
 2.1571619860435648643e-152, 6.3257905089784152346e-168, 3.5607241064750984115e-184, -1.4832196127821708615e-201,
 2.4782675885631257398e-153, -3.3573283875161501977e-170, 3.0568054078295488291e-186, 1.4980560800565462618e-202,
 2.4782675885631257398e-153, -3.3573283875161501977e-170, 3.0568054078295488291e-186, 1.4980560800565462618e-202,
 2.4782675885631257398e-153, -3.3573283875161501977e-170, 3.0568054078295488291e-186, 1.4980560800565462618e-202,
 9.1598554579059548847e-155, -4.5159745404911819927e-172, -4.5870810097328572602e-188, -3.2905064432040069127e-204,
 9.1598554579059548847e-155, -4.5159745404911819927e-172, -4.5870810097328572602e-188, -3.2905064432040069127e-204,
 9.1598554579059548847e-155, -4.5159745404911819927e-172, -4.5870810097328572602e-188, -3.2905064432040069127e-204,
 9.1598554579059548847e-155, -4.5159745404911819927e-172, -4.5870810097328572602e-188, -3.2905064432040069127e-204,
 9.1598554579059548847e-155, -4.5159745404911819927e-172, -4.5870810097328572602e-188, -3.2905064432040069127e-204,
 1.7015147267057481414e-155, -4.5159745404911819927e-172, -4.5870810097328572602e-188, -3.2905064432040069127e-204,
 1.7015147267057481414e-155, -4.5159745404911819927e-172, -4.5870810097328572602e-188, -3.2905064432040069127e-204,
 1.7015147267057481414e-155, -4.5159745404911819927e-172, -4.5870810097328572602e-188, -3.2905064432040069127e-204,
 7.6922213530572229852e-156, -4.5159745404911819927e-172, -4.5870810097328572602e-188, -3.2905064432040069127e-204,
 3.0307583960570927356e-156, 5.8345524661064358191e-172, 6.9043123899963188689e-188, -3.2905064432040069127e-204,
 7.0002691755702864582e-157, 6.5928896280762691321e-173, 1.1586156901317304854e-188, -1.0100405885278530137e-205,
 7.0002691755702864582e-157, 6.5928896280762691321e-173, 1.1586156901317304854e-188, -1.0100405885278530137e-205,
 1.1734404793201255869e-157, 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.3321093418096261919e-207,
 1.1734404793201255869e-157, 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.3321093418096261919e-207,
 1.1734404793201255869e-157, 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.3321093418096261919e-207,
 4.4508689228885539715e-158, 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.3321093418096261919e-207,
 8.0910098773220302259e-159, 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.3321093418096261919e-207,
 8.0910098773220302259e-159, 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.3321093418096261919e-207,
 8.0910098773220302259e-159, 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.3321093418096261919e-207,
 3.5387999583765925506e-159, 2.2730883653953564668e-175, 2.7431118386590483722e-191, -1.3321093418096261919e-207,
 1.2626949989038732076e-159, 2.2730883653953564668e-175, 2.7431118386590483722e-191, -1.3321093418096261919e-207,
 1.2464251916751375716e-160, 6.1977249484000140293e-177, 1.1294061984896456875e-192, 2.2526486929936882202e-208,
 1.2464251916751375716e-160, 6.1977249484000140293e-177, 1.1294061984896456875e-192, 2.2526486929936882202e-208,
 1.2464251916751375716e-160, 6.1977249484000140293e-177, 1.1294061984896456875e-192, 2.2526486929936882202e-208,
 1.2464251916751375716e-160, 6.1977249484000140293e-177, 1.1294061984896456875e-192, 2.2526486929936882202e-208,
 5.3514239183991277695e-161, 6.1977249484000140293e-177, 1.1294061984896456875e-192, 2.2526486929936882202e-208,
 1.7950099192230045857e-161, -1.6991004655691153326e-177, -1.8567941091539589297e-193, -1.8074851186411640793e-209,
 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212,
 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212,
 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212,
 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212,
 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212,
 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212,
 1.6802919634942426156e-163, 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212,
 2.9106774506606941983e-164, 5.1948630316441287936e-180, 9.6685396110091013832e-196, 1.7562785002189357559e-211,
 2.9106774506606941983e-164, 5.1948630316441287936e-180, 9.6685396110091013832e-196, 1.7562785002189357559e-211,
 2.9106774506606941983e-164, 5.1948630316441287936e-180, 9.6685396110091013832e-196, 1.7562785002189357559e-211,
 1.1741471776254777999e-164, 1.3389912474795150614e-180, 1.106784341445028435e-196, 3.3045982549756583552e-212,
 3.0588204110786950436e-165, 3.7502330143836152136e-181, 3.6564932749519464998e-198, 3.7097125405852507464e-214,
 3.0588204110786950436e-165, 3.7502330143836152136e-181, 3.6564932749519464998e-198, 3.7097125405852507464e-214,
 8.8815756978467430465e-166, 1.3403131492807310959e-181, 3.6564932749519464998e-198, 3.7097125405852507464e-214,
 8.8815756978467430465e-166, 1.3403131492807310959e-181, 3.6564932749519464998e-198, 3.7097125405852507464e-214,
 3.4549185946116918017e-166, 1.3535321672928907047e-182, 3.1205762277848031878e-199, -3.3569248349832580936e-217,
 7.4159004299416557678e-167, 1.3535321672928907047e-182, 3.1205762277848031878e-199, -3.3569248349832580936e-217,
 7.4159004299416557678e-167, 1.3535321672928907047e-182, 3.1205762277848031878e-199, -3.3569248349832580936e-217,
 6.3257905089784152346e-168, 3.5607241064750984115e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218,
 6.3257905089784152346e-168, 3.5607241064750984115e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218,
 6.3257905089784152346e-168, 3.5607241064750984115e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218,
 6.3257905089784152346e-168, 3.5607241064750984115e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218,
 2.0862146470760309789e-168, -1.146150630053972131e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218,
 2.0862146470760309789e-168, -1.146150630053972131e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218,
 1.026320681600434562e-168, 1.2072867382105631402e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218,
 4.9637369886263658882e-169, 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218,
 2.3140020749373754342e-169, 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218,
 9.8913461809288020723e-170, 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218,
 3.2670088967063259373e-170, 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218,
 3.2670088967063259373e-170, 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218,
 1.6109245756507072713e-170, -6.2044048008378732802e-187, -5.4322544592823556944e-203, 4.2491789852161138683e-219,
 7.8288241512289757055e-171, 1.2181824638728806485e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218,
 3.6886133485899290404e-171, 2.9887099189454666024e-187, 4.774153170641553462e-203, 4.2491789852161138683e-219,
 1.6185079472704052482e-171, 2.9887099189454666024e-187, 4.774153170641553462e-203, 4.2491789852161138683e-219,
 5.8345524661064358191e-172, 6.9043123899963188689e-188, -3.2905064432040069127e-204, -9.1795828160190082842e-224,
 6.5928896280762691321e-173, 1.1586156901317304854e-188, -1.0100405885278530137e-205, -9.1795828160190082842e-224,
 6.5928896280762691321e-173, 1.1586156901317304854e-188, -1.0100405885278530137e-205, -9.1795828160190082842e-224,
 6.5928896280762691321e-173, 1.1586156901317304854e-188, -1.0100405885278530137e-205, -9.1795828160190082842e-224,
 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.332109341809626019e-207, -9.1795828160190082842e-224,
 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.332109341809626019e-207, -9.1795828160190082842e-224,
 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.332109341809626019e-207, -9.1795828160190082842e-224,
 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.332109341809626019e-207, -9.1795828160190082842e-224,
 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.332109341809626019e-207, -9.1795828160190082842e-224,
 1.2381024895275844856e-174, -8.4789520282639751913e-191, -1.332109341809626019e-207, -9.1795828160190082842e-224,
 2.2730883653953564668e-175, 2.7431118386590483722e-191, -1.332109341809626019e-207, -9.1795828160190082842e-224,
 2.2730883653953564668e-175, 2.7431118386590483722e-191, -1.332109341809626019e-207, -9.1795828160190082842e-224,
 2.2730883653953564668e-175, 2.7431118386590483722e-191, -1.332109341809626019e-207, -9.1795828160190082842e-224,
 1.0095962991602958391e-175, -6.2404128071707654958e-193, 3.0593092910744445285e-209, 5.4622616159087170031e-225,
 3.7785026604276538491e-176, -6.2404128071707654958e-193, 3.0593092910744445285e-209, 5.4622616159087170031e-225,
 6.1977249484000140293e-177, 1.1294061984896456875e-192, 2.2526486929936882202e-208, -5.3441928036578162463e-225,
 6.1977249484000140293e-177, 1.1294061984896456875e-192, 2.2526486929936882202e-208, -5.3441928036578162463e-225,
 6.1977249484000140293e-177, 1.1294061984896456875e-192, 2.2526486929936882202e-208, -5.3441928036578162463e-225,
 2.2493122414154495675e-177, 2.5268245888628466632e-193, 3.0593092910744445285e-209, 5.4622616159087170031e-225,
 2.7510588792316711745e-178, 3.3501523985444386676e-194, 6.2591208621664049475e-210, 5.9034406125450500218e-227,
 2.7510588792316711745e-178, 3.3501523985444386676e-194, 6.2591208621664049475e-210, 5.9034406125450500218e-227,
 2.7510588792316711745e-178, 3.3501523985444386676e-194, 6.2591208621664049475e-210, 5.9034406125450500218e-227,
 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212, 9.9192633285681635836e-229,
 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212, 9.9192633285681635836e-229,
 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212, 9.9192633285681635836e-229,
 2.8330093736631818036e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212, 9.9192633285681635836e-229,
 1.2906606599973359683e-179, -7.4549709281190454638e-196, -1.4481306607622412036e-212, 9.9192633285681635836e-229,
 5.1948630316441287936e-180, 9.6685396110091013832e-196, 1.7562785002189355449e-211, 1.6821693549018732055e-227,
 1.3389912474795150614e-180, 1.106784341445028435e-196, 3.3045982549756578275e-212, 6.2685154049107876715e-228,
 1.3389912474795150614e-180, 1.106784341445028435e-196, 3.3045982549756578275e-212, 6.2685154049107876715e-228,
 3.7502330143836152136e-181, 3.6564932749519464998e-198, 3.7097125405852507464e-214, 2.5658818466966882188e-231,
 3.7502330143836152136e-181, 3.6564932749519464998e-198, 3.7097125405852507464e-214, 2.5658818466966882188e-231,
 1.3403131492807310959e-181, 3.6564932749519464998e-198, 3.7097125405852507464e-214, 2.5658818466966882188e-231,
 1.3535321672928907047e-182, 3.1205762277848031878e-199, -3.3569248349832580936e-217, -1.0577661142165146927e-233,
 1.3535321672928907047e-182, 3.1205762277848031878e-199, -3.3569248349832580936e-217, -1.0577661142165146927e-233,
 1.3535321672928907047e-182, 3.1205762277848031878e-199, -3.3569248349832580936e-217, -1.0577661142165146927e-233,
 1.3535321672928907047e-182, 3.1205762277848031878e-199, -3.3569248349832580936e-217, -1.0577661142165146927e-233,
 6.0043220944823941786e-183, 3.1205762277848031878e-199, -3.3569248349832580936e-217, -1.0577661142165146927e-233,
 2.2388223052591377446e-183, 3.1205762277848031878e-199, -3.3569248349832580936e-217, -1.0577661142165146927e-233,
 3.5607241064750984115e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 3.5607241064750984115e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 3.5607241064750984115e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 1.2072867382105631402e-184, -1.4832196127821708615e-201, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 3.0568054078295488291e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 1.2181824638728806485e-186, 1.4980560800565460352e-202, 2.6911956484118910092e-218, -5.1336618966962585332e-235,
 2.9887099189454666024e-187, 4.774153170641553462e-203, 4.2491789852161132393e-219, 7.4467067939231424594e-235,
 2.9887099189454666024e-187, 4.774153170641553462e-203, 4.2491789852161132393e-219, 7.4467067939231424594e-235,
 6.9043123899963188689e-188, -3.2905064432040069127e-204, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 6.9043123899963188689e-188, -3.2905064432040069127e-204, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 1.1586156901317304854e-188, -1.0100405885278530137e-205, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 1.1586156901317304854e-188, -1.0100405885278530137e-205, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 1.1586156901317304854e-188, -1.0100405885278530137e-205, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 4.4040360264865697732e-189, -1.0100405885278530137e-205, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 8.129755890712020335e-190, 9.8339840169166049336e-206, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 8.129755890712020335e-190, 9.8339840169166049336e-206, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 8.129755890712020335e-190, 9.8339840169166049336e-206, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 3.6409303439428119063e-190, -1.332109341809626019e-207, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 1.3965175705582071936e-190, -1.332109341809626019e-207, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 2.7431118386590483722e-191, -1.332109341809626019e-207, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 2.7431118386590483722e-191, -1.332109341809626019e-207, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 2.7431118386590483722e-191, -1.332109341809626019e-207, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 1.3403538552936701153e-191, 1.7826390804083638359e-207, -9.1795828160190063645e-224, -2.3569545504732004486e-239,
 6.389748636109812983e-192, 2.2526486929936882202e-208, -5.3441928036578156465e-225, -7.741539335184153052e-241,
 2.8828536776963681193e-192, 2.2526486929936882202e-208, -5.3441928036578156465e-225, -7.741539335184153052e-241,
 1.1294061984896456875e-192, 2.2526486929936882202e-208, -5.3441928036578156465e-225, -7.741539335184153052e-241,
 2.5268245888628466632e-193, 3.0593092910744445285e-209, 5.4622616159087170031e-225, 4.2560351759808952526e-241,
 2.5268245888628466632e-193, 3.0593092910744445285e-209, 5.4622616159087170031e-225, 4.2560351759808952526e-241,
 3.3501523985444386676e-194, 6.2591208621664049475e-210, 5.9034406125450490845e-227, 1.3186893776791012681e-242,
 3.3501523985444386676e-194, 6.2591208621664049475e-210, 5.9034406125450490845e-227, 1.3186893776791012681e-242,
 3.3501523985444386676e-194, 6.2591208621664049475e-210, 5.9034406125450490845e-227, 1.3186893776791012681e-242,
 6.1039071228393547627e-195, 1.7562785002189355449e-211, 1.6821693549018732055e-227, -8.7276385348052817035e-244,
 6.1039071228393547627e-195, 1.7562785002189355449e-211, 1.6821693549018732055e-227, -8.7276385348052817035e-244,
 6.1039071228393547627e-195, 1.7562785002189355449e-211, 1.6821693549018732055e-227, -8.7276385348052817035e-244,
 2.6792050150137250131e-195, 1.7562785002189355449e-211, 1.6821693549018732055e-227, -8.7276385348052817035e-244,
 9.6685396110091013832e-196, 1.7562785002189355449e-211, 1.6821693549018732055e-227, -8.7276385348052817035e-244,
 2.0416567491425607157e-177, 6.0959078275963141821e-193, 1.156336993964950812e-208, 2.7126166236326293347e-224,
 2.0416567491425607157e-177, 6.0959078275963141821e-193, 1.156336993964950812e-208, 2.7126166236326293347e-224,
 2.0416567491425607157e-177, 6.0959078275963141821e-193, 1.156336993964950812e-208, 2.7126166236326293347e-224,
 6.7450395650278649168e-179, 6.8432117823206978686e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228,
 6.7450395650278649168e-179, 6.8432117823206978686e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228,
 6.7450395650278649168e-179, 6.8432117823206978686e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228,
 6.7450395650278649168e-179, 6.8432117823206978686e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228,
 6.7450395650278649168e-179, 6.8432117823206978686e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228,
 5.756447103644822603e-180, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 5.756447103644822603e-180, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 5.756447103644822603e-180, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 5.756447103644822603e-180, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 1.9005753194802080146e-180, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 1.9005753194802080146e-180, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 9.3660737343905436753e-181, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 4.5462340041847754398e-181, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 2.1363141390818913221e-181, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 9.3135420653044926323e-182, -6.1924333305615830735e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 3.2887424025472810002e-182, 7.185309278132283136e-198, -1.9512340798794268979e-214, -3.6162764918921697356e-230,
 2.7634257116867652192e-183, 4.9643797378534984559e-199, -9.4699347169310243473e-216, -9.2331809177749095611e-233,
 2.7634257116867652192e-183, 4.9643797378534984559e-199, -9.4699347169310243473e-216, -9.2331809177749095611e-233,
 2.7634257116867652192e-183, 4.9643797378534984559e-199, -9.4699347169310243473e-216, -9.2331809177749095611e-233,
 2.7634257116867652192e-183, 4.9643797378534984559e-199, -9.4699347169310243473e-216, -9.2331809177749095611e-233,
 8.806758170751374203e-184, 7.8383517263666503337e-200, 1.3736749441945438342e-215, -9.2331809177749095611e-233,
 8.806758170751374203e-184, 7.8383517263666503337e-200, 1.3736749441945438342e-215, -9.2331809177749095611e-233,
 4.0998834342223036605e-184, 7.8383517263666503337e-200, 1.3736749441945438342e-215, -9.2331809177749095611e-233,
 1.7464460659577689118e-184, 2.612671019845610006e-200, 2.1334073625072069974e-216, -9.2331809177749095611e-233,
 5.697273818255015375e-185, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 5.697273818255015375e-185, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 2.755477107924346286e-185, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 1.2845787527590117414e-185, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 5.4912957517634446918e-186, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 1.8140498638501083305e-186, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 1.8140498638501083305e-186, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 8.9473839187177424013e-187, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 4.3508265588260719497e-187, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 2.0525478788802367239e-187, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 9.0340853890731911095e-188, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 3.288388689208603045e-188, -1.6933341491052464293e-204, -4.3478137385944270631e-220, -2.3353910329236990725e-236,
 4.1554033927630885323e-189, -9.8582956929636044137e-206, -1.4280619485269765742e-221, 1.2171222696290252021e-237,
 4.1554033927630885323e-189, -9.8582956929636044137e-206, -1.4280619485269765742e-221, 1.2171222696290252021e-237,
 4.1554033927630885323e-189, -9.8582956929636044137e-206, -1.4280619485269765742e-221, 1.2171222696290252021e-237,
 5.643429553477207926e-190, 1.0076094209231528444e-205, 7.8509991660024955813e-222, 1.2171222696290252021e-237,
 5.643429553477207926e-190, 1.0076094209231528444e-205, 7.8509991660024955813e-222, 1.2171222696290252021e-237,
 5.643429553477207926e-190, 1.0076094209231528444e-205, 7.8509991660024955813e-222, 1.2171222696290252021e-237,
 1.1546040067079994973e-190, 1.0889925813396166947e-207, 2.4325525462765697993e-223, -1.1429360314275701698e-239,
 1.1546040067079994973e-190, 1.0889925813396166947e-207, 2.4325525462765697993e-223, -1.1429360314275701698e-239,
 3.2397620015697148712e-192, 3.1030547578511949035e-208, -1.609965144193984205e-224, -1.8313007053436627876e-240,
 3.2397620015697148712e-192, 3.1030547578511949035e-208, -1.609965144193984205e-224, -1.8313007053436627876e-240,
 3.2397620015697148712e-192, 3.1030547578511949035e-208, -1.609965144193984205e-224, -1.8313007053436627876e-240,
 3.2397620015697148712e-192, 3.1030547578511949035e-208, -1.609965144193984205e-224, -1.8313007053436627876e-240,
 3.2397620015697148712e-192, 3.1030547578511949035e-208, -1.609965144193984205e-224, -1.8313007053436627876e-240,
 3.2397620015697148712e-192, 3.1030547578511949035e-208, -1.609965144193984205e-224, -1.8313007053436627876e-240,
 1.4863145223629928288e-192, -7.9038076992129241506e-209, -1.609965144193984205e-224, -1.8313007053436627876e-240,
 6.0959078275963141821e-193, 1.156336993964950812e-208, 2.7126166236326293347e-224, -1.8313007053436627876e-240,
 1.712289129579509076e-193, 1.8297811202182925249e-209, 1.1003018740995688645e-226, 5.827891678485165325e-243,
 1.712289129579509076e-193, 1.8297811202182925249e-209, 1.1003018740995688645e-226, 5.827891678485165325e-243,
 6.1638445507530779946e-194, -6.0361608463951204924e-210, 1.1003018740995688645e-226, 5.827891678485165325e-243,
 6.8432117823206978686e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.029900079464340522e-245,
 6.8432117823206978686e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.029900079464340522e-245,
 6.8432117823206978686e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.029900079464340522e-245,
 6.8432117823206978686e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.029900079464340522e-245,
 3.418509674495068119e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.029900079464340522e-245,
 1.7061586205822532442e-195, 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.029900079464340522e-245,
 8.499830936258458068e-196, 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.029900079464340522e-245,
 4.218953301476420881e-196, 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.029900079464340522e-245,
 2.0785144840854027628e-196, -1.9512340798794268979e-214, -3.6162764918921692779e-230, -2.8387319855193022476e-246,
 1.008295075389893466e-196, -1.9512340798794268979e-214, -3.6162764918921692779e-230, -2.8387319855193022476e-246,
 4.7318537104213881764e-197, -1.9512340798794268979e-214, -3.6162764918921692779e-230, -2.8387319855193022476e-246,
 2.0563051886826149345e-197, -1.9512340798794268979e-214, -3.6162764918921692779e-230, -2.8387319855193022476e-246,
 7.185309278132283136e-198, -1.9512340798794268979e-214, -3.6162764918921692779e-230, -2.8387319855193022476e-246,
 4.9643797378534984559e-199, -9.4699347169310243473e-216, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 4.9643797378534984559e-199, -9.4699347169310243473e-216, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 4.9643797378534984559e-199, -9.4699347169310243473e-216, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 4.9643797378534984559e-199, -9.4699347169310243473e-216, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 7.8383517263666503337e-200, 1.3736749441945438342e-215, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 7.8383517263666503337e-200, 1.3736749441945438342e-215, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 7.8383517263666503337e-200, 1.3736749441945438342e-215, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 2.612671019845610006e-200, 2.1334073625072069974e-216, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 2.612671019845610006e-200, 2.1334073625072069974e-216, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 1.306250843215349634e-200, 2.1334073625072069974e-216, -9.2331809177749077733e-233, -1.4042876247421728101e-248,
 6.5304075490021959302e-201, 6.8298960257742791824e-217, 6.8696910062179237095e-233, 3.8349029251851101018e-249,
 3.2643571074265457254e-201, -4.2219277387461470355e-218, -1.753154605289404553e-234, -7.5861268822635538093e-251,
 1.6313318866387202604e-201, -4.2219277387461470355e-218, -1.753154605289404553e-234, -7.5861268822635538093e-251,
 8.1481927624480752786e-202, -4.2219277387461470355e-218, -1.753154605289404553e-234, -7.5861268822635538093e-251,
 4.0656297104785107096e-202, 4.8431832608149701961e-218, 8.3111403472061145651e-234, 1.6001805286092554504e-249,
 2.0243481844937293316e-202, 3.1062776103441183191e-219, 7.6291913283447536617e-235, 2.0347903074934629333e-250,
 1.0037074215013384159e-202, 3.1062776103441183191e-219, 7.6291913283447536617e-235, 2.0347903074934629333e-250,
 4.9338704000514295811e-203, 3.1062776103441183191e-219, 7.6291913283447536617e-235, 2.0347903074934629333e-250,
 2.3822684925704522921e-203, 3.1062776103441183191e-219, 7.6291913283447536617e-235, 2.0347903074934629333e-250,
 1.1064675388299639308e-203, 2.7343042298126957741e-220, 5.5273393987134252385e-236, 1.1432574793608782288e-251,
 4.6856706195971960852e-204, 2.7343042298126957741e-220, 5.5273393987134252385e-236, 1.1432574793608782288e-251,
 1.4961682352459748279e-204, -8.0675475439086544798e-221, -3.6970842501441777651e-237, -5.7032870362481275794e-253,
 1.4961682352459748279e-204, -8.0675475439086544798e-221, -3.6970842501441777651e-237, -5.7032870362481275794e-253,
 6.9879263915816924805e-205, 9.6377473771091526132e-221, 1.5959741828948633012e-236, 2.7031904319843495713e-252,
 3.0010484111426663515e-205, 7.8509991660024955813e-222, 1.2171222696290252021e-237, -2.4742181023285720738e-254,
 1.0076094209231528444e-205, 7.8509991660024955813e-222, 1.2171222696290252021e-237, -2.4742181023285720738e-254,
 1.0889925813396166947e-207, 2.4325525462765697993e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256,
 1.0889925813396166947e-207, 2.4325525462765697993e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256,
 1.0889925813396166947e-207, 2.4325525462765697993e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256,
 1.0889925813396166947e-207, 2.4325525462765697993e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256,
 1.0889925813396166947e-207, 2.4325525462765697993e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256,
 1.0889925813396166947e-207, 2.4325525462765697993e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256,
 1.0889925813396166947e-207, 2.4325525462765697993e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256,
 3.1030547578511949035e-208, -1.609965144193984205e-224, -1.8313007053436625212e-240, -2.3341145329525059632e-256,
 3.1030547578511949035e-208, -1.609965144193984205e-224, -1.8313007053436625212e-240, -2.3341145329525059632e-256,
 1.156336993964950812e-208, 2.7126166236326293347e-224, -1.8313007053436625212e-240, -2.3341145329525059632e-256,
 1.8297811202182925249e-209, 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.1174271110208206547e-259,
 1.8297811202182925249e-209, 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.1174271110208206547e-259,
 1.8297811202182925249e-209, 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.1174271110208206547e-259,
 6.1308251778939023781e-210, 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.1174271110208206547e-259,
 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 4.7332165749391048364e-212, 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 2.3568521170701555846e-212, -7.7818310317651142243e-229, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 1.1686698881356804311e-212, 1.8601114328504743806e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 5.7457877366844311816e-213, 5.409641648369814791e-229, -3.0299000794643401155e-245, -2.8075477999879273582e-261,
 2.7753321643482446169e-213, -1.1860946916976500828e-229, 6.3146909508553973881e-246, 1.2573885592501532045e-261,
 1.290104378180150675e-213, 2.1117734783360818049e-229, 4.2928382696354204061e-245, -2.8075477999879273582e-261,
 5.4749048509610403382e-214, 4.6283939331921604413e-230, 6.3146909508553973881e-246, 1.2573885592501532045e-261,
 1.7618353855408067201e-214, 5.060587206499956961e-231, 5.9380161562121075096e-247, -1.2904053011746964278e-263,
 1.7618353855408067201e-214, 5.060587206499956961e-231, 5.9380161562121075096e-247, -1.2904053011746964278e-263,
 8.3356801918574821257e-215, 5.060587206499956961e-231, 5.9380161562121075096e-247, -1.2904053011746964278e-263,
 3.6943433600821895879e-215, 5.060587206499956961e-231, 5.9380161562121075096e-247, -1.2904053011746964278e-263,
 1.3736749441945438342e-215, -9.2331809177749077733e-233, -1.4042876247421726117e-248, -9.9505977179164858712e-265,
 2.1334073625072069974e-216, -9.2331809177749077733e-233, -1.4042876247421726117e-248, -9.9505977179164858712e-265,
 2.1334073625072069974e-216, -9.2331809177749077733e-233, -1.4042876247421726117e-248, -9.9505977179164858712e-265,
 2.1334073625072069974e-216, -9.2331809177749077733e-233, -1.4042876247421726117e-248, -9.9505977179164858712e-265,
 6.8298960257742791824e-217, 6.8696910062179237095e-233, 3.8349029251851101018e-249, -2.6436684620390282645e-267,
 6.8298960257742791824e-217, 6.8696910062179237095e-233, 3.8349029251851101018e-249, -2.6436684620390282645e-267,
 3.2038516259498326923e-217, -1.1817449557784924788e-233, -6.3454186796659920093e-250, -2.6436684620390282645e-267,
 1.3908294260376086421e-217, 2.8439730252197153919e-233, 3.8349029251851101018e-249, -2.6436684620390282645e-267,
 4.8431832608149701961e-218, 8.3111403472061145651e-234, 1.6001805286092554504e-249, -2.6436684620390282645e-267,
 3.1062776103441183191e-219, 7.6291913283447536617e-235, 2.0347903074934629333e-250, -2.6436684620390282645e-267,
 3.1062776103441183191e-219, 7.6291913283447536617e-235, 2.0347903074934629333e-250, -2.6436684620390282645e-267,
 3.1062776103441183191e-219, 7.6291913283447536617e-235, 2.0347903074934629333e-250, -2.6436684620390282645e-267,
 3.1062776103441183191e-219, 7.6291913283447536617e-235, 2.0347903074934629333e-250, -2.6436684620390282645e-267,
 2.7343042298126957741e-220, 5.5273393987134252385e-236, 1.1432574793608780349e-251, 1.2329569415922591084e-267,
 2.7343042298126957741e-220, 5.5273393987134252385e-236, 1.1432574793608780349e-251, 1.2329569415922591084e-267,
 2.7343042298126957741e-220, 5.5273393987134252385e-236, 1.1432574793608780349e-251, 1.2329569415922591084e-267,
 2.7343042298126957741e-220, 5.5273393987134252385e-236, 1.1432574793608780349e-251, 1.2329569415922591084e-267,
 9.6377473771091526132e-221, 1.5959741828948633012e-236, 2.7031904319843490867e-252, 2.638005906844372114e-268,
 7.8509991660024955813e-222, 1.2171222696290252021e-237, -2.4742181023285720738e-254, -1.2030990169203137715e-270,
 7.8509991660024955813e-222, 1.2171222696290252021e-237, -2.4742181023285720738e-254, -1.2030990169203137715e-270,
 7.8509991660024955813e-222, 1.2171222696290252021e-237, -2.4742181023285720738e-254, -1.2030990169203137715e-270,
 7.8509991660024955813e-222, 1.2171222696290252021e-237, -2.4742181023285720738e-254, -1.2030990169203137715e-270,
 2.318094503184431479e-222, -1.1429360314275701698e-239, 8.3218722366085688343e-256, -2.0046830753539155726e-272,
 2.318094503184431479e-222, -1.1429360314275701698e-239, 8.3218722366085688343e-256, -2.0046830753539155726e-272,
 9.3486833747991514629e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256, -2.0046830753539155726e-272,
 2.4325525462765697993e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256, -2.0046830753539155726e-272,
 2.4325525462765697993e-223, -1.1429360314275701698e-239, 8.3218722366085688343e-256, -2.0046830753539155726e-272,
 7.0351983914592419146e-224, 7.766758903588374524e-240, 8.3218722366085688343e-256, -2.0046830753539155726e-272,
 7.0351983914592419146e-224, 7.766758903588374524e-240, 8.3218722366085688343e-256, -2.0046830753539155726e-272,
 2.7126166236326293347e-224, -1.8313007053436625212e-240, -2.3341145329525056675e-256, -2.0046830753539155726e-272,
 5.5132573971932232487e-225, 5.6821419688934674008e-241, 3.2988215943776273615e-257, 2.1353977370878701046e-273,
 5.5132573971932232487e-225, 5.6821419688934674008e-241, 3.2988215943776273615e-257, 2.1353977370878701046e-273,
 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275,
 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275,
 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275,
 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275,
 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275,
 1.1003018740995688645e-226, 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275,
 2.560476225709334075e-227, 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275,
 2.560476225709334075e-227, 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275,
 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261, -1.472095602234059958e-277,
 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261, -1.472095602234059958e-277,
 4.4984059688774601837e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261, -1.472095602234059958e-277,
 1.8601114328504743806e-228, -3.0299000794643401155e-245, -2.8075477999879273582e-261, -1.472095602234059958e-277,
 5.409641648369814791e-229, -3.0299000794643401155e-245, -2.8075477999879273582e-261, -1.472095602234059958e-277,
 5.409641648369814791e-229, -3.0299000794643401155e-245, -2.8075477999879273582e-261, -1.472095602234059958e-277,
 2.1117734783360818049e-229, 4.2928382696354204061e-245, -2.8075477999879273582e-261, -1.472095602234059958e-277,
 4.6283939331921604413e-230, 6.3146909508553973881e-246, 1.2573885592501529789e-261, 3.0408903374280139822e-277,
 4.6283939331921604413e-230, 6.3146909508553973881e-246, 1.2573885592501529789e-261, 3.0408903374280139822e-277,
 5.060587206499956961e-231, 5.9380161562121075096e-247, -1.2904053011746964278e-263, 8.7279092175580820317e-280,
 5.060587206499956961e-231, 5.9380161562121075096e-247, -1.2904053011746964278e-263, 8.7279092175580820317e-280,
 5.060587206499956961e-231, 5.9380161562121075096e-247, -1.2904053011746964278e-263, 8.7279092175580820317e-280,
 5.060587206499956961e-231, 5.9380161562121075096e-247, -1.2904053011746964278e-263, 8.7279092175580820317e-280,
 2.4841276986611042098e-231, 2.1712682097791944335e-248, 2.9746046415267896827e-264, -8.6516445844406224413e-282,
 1.1958979447416775482e-231, 2.1712682097791944335e-248, 2.9746046415267896827e-264, -8.6516445844406224413e-282,
 5.5178306778196421733e-232, 2.1712682097791944335e-248, 2.9746046415267896827e-264, -8.6516445844406224413e-282,
 2.2972562930210755192e-232, 2.1712682097791944335e-248, 2.9746046415267896827e-264, -8.6516445844406224413e-282,
 6.8696910062179237095e-233, 3.8349029251851101018e-249, -2.6436684620390282645e-267, -4.3807022524130141006e-284,
 6.8696910062179237095e-233, 3.8349029251851101018e-249, -2.6436684620390282645e-267, -4.3807022524130141006e-284,
 2.8439730252197153919e-233, 3.8349029251851101018e-249, -2.6436684620390282645e-267, -4.3807022524130141006e-284,
 8.3111403472061145651e-234, 1.6001805286092554504e-249, -2.6436684620390282645e-267, -4.3807022524130141006e-284,
 8.3111403472061145651e-234, 1.6001805286092554504e-249, -2.6436684620390282645e-267, -4.3807022524130141006e-284,
 3.2789928709583552854e-234, 4.8281933032132812475e-250, -2.6436684620390282645e-267, -4.3807022524130141006e-284,
 7.6291913283447536617e-235, 2.0347903074934629333e-250, -2.6436684620390282645e-267, -4.3807022524130141006e-284,
 7.6291913283447536617e-235, 2.0347903074934629333e-250, -2.6436684620390282645e-267, -4.3807022524130141006e-284,
 1.3390069830350552605e-235, -6.026193929640082176e-252, -7.0535576022338457803e-268, -4.3807022524130141006e-284,
 1.3390069830350552605e-235, -6.026193929640082176e-252, -7.0535576022338457803e-268, -4.3807022524130141006e-284,
 1.3390069830350552605e-235, -6.026193929640082176e-252, -7.0535576022338457803e-268, -4.3807022524130141006e-284,
 5.5273393987134252385e-236, 1.1432574793608780349e-251, 1.2329569415922591084e-267, -4.3807022524130141006e-284,
 1.5959741828948633012e-236, 2.7031904319843490867e-252, 2.638005906844371576e-268, 6.3790946999826013345e-284,
 1.5959741828948633012e-236, 2.7031904319843490867e-252, 2.638005906844371576e-268, 6.3790946999826013345e-284,
 6.1313287894022281692e-237, 5.2084434157824127104e-253, 2.1511502957481757317e-269, 3.2670891426006739096e-285,
 1.2171222696290252021e-237, -2.4742181023285720738e-254, -1.2030990169203137715e-270, -9.5347405022956042207e-287,
 1.2171222696290252021e-237, -2.4742181023285720738e-254, -1.2030990169203137715e-270, -9.5347405022956042207e-287,
 1.2171222696290252021e-237, -2.4742181023285720738e-254, -1.2030990169203137715e-270, -9.5347405022956042207e-287,
 6.0284645465737476297e-238, -2.4742181023285720738e-254, -1.2030990169203137715e-270, -9.5347405022956042207e-287,
 2.9570854717154947523e-238, 4.3456134301905148502e-254, 6.3684349745470443788e-270, -9.5347405022956042207e-287,
 1.4213959342863689955e-238, 9.3569766393097138822e-255, 2.5826679788133653036e-270, -9.5347405022956042207e-287,
 6.5355116557180594664e-239, 9.3569766393097138822e-255, 2.5826679788133653036e-270, -9.5347405022956042207e-287,
 2.6962878121452450746e-239, 8.3218722366085688343e-256, -2.0046830753539152442e-272, -3.4057806738724185961e-288,
 7.766758903588374524e-240, 8.3218722366085688343e-256, -2.0046830753539152442e-272, -3.4057806738724185961e-288,
 7.766758903588374524e-240, 8.3218722366085688343e-256, -2.0046830753539152442e-272, -3.4057806738724185961e-288,
 2.9677290991223565342e-240, -2.3341145329525056675e-256, -2.0046830753539152442e-272, -3.4057806738724185961e-288,
 5.6821419688934674008e-241, 3.2988215943776273615e-257, 2.1353977370878701046e-273, -1.2215123283371736879e-289,
 5.6821419688934674008e-241, 3.2988215943776273615e-257, 2.1353977370878701046e-273, -1.2215123283371736879e-289,
 5.6821419688934674008e-241, 3.2988215943776273615e-257, 2.1353977370878701046e-273, -1.2215123283371736879e-289,
 2.6827483411022054912e-241, 3.2988215943776273615e-257, 2.1353977370878701046e-273, -1.2215123283371736879e-289,
 1.1830515272065748694e-241, -3.117427111020820077e-259, -5.9718623963762788119e-275, 6.1155422068568954053e-291,
 4.3320312025875939195e-242, -3.117427111020820077e-259, -5.9718623963762788119e-275, 6.1155422068568954053e-291,
 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275, 6.1155422068568954053e-291,
 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275, 6.1155422068568954053e-291,
 5.827891678485165325e-243, -3.117427111020820077e-259, -5.9718623963762788119e-275, 6.1155422068568954053e-291,
 1.1413391350613183311e-243, -5.1586784110844895013e-260, -1.9524039360882352712e-276, -2.9779654517181717279e-292,
 1.1413391350613183311e-243, -5.1586784110844895013e-260, -1.9524039360882352712e-276, -2.9779654517181717279e-292,
 1.1413391350613183311e-243, -5.1586784110844895013e-260, -1.9524039360882352712e-276, -2.9779654517181717279e-292,
 5.5552006713333735927e-244, 7.8491179384773690214e-260, -1.9524039360882352712e-276, -2.9779654517181717279e-292,
 2.6261053316934700345e-244, 1.345219763696439399e-260, 1.6579848156414234801e-276, 1.0303712682997740506e-292,
 1.1615576618735179302e-244, 1.345219763696439399e-260, 1.6579848156414234801e-276, 1.0303712682997740506e-292,
 4.2928382696354204061e-245, -2.8075477999879273582e-261, -1.472095602234059958e-277, 2.8287088295287585094e-294,
 6.3146909508553973881e-246, 1.2573885592501529789e-261, 3.0408903374280139822e-277, 2.8287088295287585094e-294,
 6.3146909508553973881e-246, 1.2573885592501529789e-261, 3.0408903374280139822e-277, 2.8287088295287585094e-294,
 6.3146909508553973881e-246, 1.2573885592501529789e-261, 3.0408903374280139822e-277, 2.8287088295287585094e-294,
 1.7379794826680480784e-246, 2.4115446944063306384e-262, 2.202741251392177696e-278, 2.8287088295287585094e-294,
 1.7379794826680480784e-246, 2.4115446944063306384e-262, 2.202741251392177696e-278, 2.8287088295287585094e-294,
 5.9380161562121075096e-247, -1.2904053011746964278e-263, 8.7279092175580810531e-280, 8.8634899828990930877e-296,
 2.1712682097791944335e-248, 2.9746046415267896827e-264, -8.6516445844406224413e-282, -5.0528699238150276549e-299,
 2.1712682097791944335e-248, 2.9746046415267896827e-264, -8.6516445844406224413e-282, -5.0528699238150276549e-299,
 2.1712682097791944335e-248, 2.9746046415267896827e-264, -8.6516445844406224413e-282, -5.0528699238150276549e-299,
 2.1712682097791944335e-248, 2.9746046415267896827e-264, -8.6516445844406224413e-282, -5.0528699238150276549e-299,
 2.1712682097791944335e-248, 2.9746046415267896827e-264, -8.6516445844406224413e-282, -5.0528699238150276549e-299,
 3.8349029251851101018e-249, -2.6436684620390282645e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 3.8349029251851101018e-249, -2.6436684620390282645e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 3.8349029251851101018e-249, -2.6436684620390282645e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 1.6001805286092554504e-249, -2.6436684620390282645e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 4.8281933032132812475e-250, -2.6436684620390282645e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 4.8281933032132812475e-250, -2.6436684620390282645e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 2.0347903074934629333e-250, -2.6436684620390282645e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 6.3808880963355377617e-251, -2.6436684620390282645e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 6.3808880963355377617e-251, -2.6436684620390282645e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 2.8891343516857640937e-251, 5.1095823452235464813e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 1.1432574793608780349e-251, 1.2329569415922591084e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300,
 2.7031904319843490867e-252, 2.638005906844371576e-268, 6.3790946999826013345e-284, -2.7456019707854725967e-300,
 2.7031904319843490867e-252, 2.638005906844371576e-268, 6.3790946999826013345e-284, -2.7456019707854725967e-300,
 5.2084434157824127104e-253, 2.1511502957481757317e-269, 3.2670891426006735363e-285, 2.4084160842482777461e-301,
 5.2084434157824127104e-253, 2.1511502957481757317e-269, 3.2670891426006735363e-285, 2.4084160842482777461e-301,
 5.2084434157824127104e-253, 2.1511502957481757317e-269, 3.2670891426006735363e-285, 2.4084160842482777461e-301,
 2.4805108027747776379e-253, 2.1511502957481757317e-269, 3.2670891426006735363e-285, 2.4084160842482777461e-301,
 1.1165444962709601017e-253, 2.1511502957481757317e-269, 3.2670891426006735363e-285, 2.4084160842482777461e-301,
 4.3456134301905148502e-254, 6.3684349745470443788e-270, -9.5347405022956030541e-287, -1.5805886663557401565e-302,
 9.3569766393097138822e-255, 2.5826679788133653036e-270, -9.5347405022956030541e-287, -1.5805886663557401565e-302,
 9.3569766393097138822e-255, 2.5826679788133653036e-270, -9.5347405022956030541e-287, -1.5805886663557401565e-302,
 8.3218722366085688343e-256, -2.0046830753539152442e-272, -3.4057806738724185961e-288, 2.3458177946667328156e-304,
 8.3218722366085688343e-256, -2.0046830753539152442e-272, -3.4057806738724185961e-288, 2.3458177946667328156e-304,
 8.3218722366085688343e-256, -2.0046830753539152442e-272, -3.4057806738724185961e-288, 2.3458177946667328156e-304,
 8.3218722366085688343e-256, -2.0046830753539152442e-272, -3.4057806738724185961e-288, 2.3458177946667328156e-304,
 2.9938788518280315834e-256, -2.0046830753539152442e-272, -3.4057806738724185961e-288, 2.3458177946667328156e-304,
 3.2988215943776273615e-257, 2.1353977370878701046e-273, -1.2215123283371736879e-289, 6.7342163555358599277e-306,
 3.2988215943776273615e-257, 2.1353977370878701046e-273, -1.2215123283371736879e-289, 6.7342163555358599277e-306,
 3.2988215943776273615e-257, 2.1353977370878701046e-273, -1.2215123283371736879e-289, 6.7342163555358599277e-306,
 3.2988215943776273615e-257, 2.1353977370878701046e-273, -1.2215123283371736879e-289, 6.7342163555358599277e-306,
 1.6338236616337094706e-257, 2.1353977370878701046e-273, -1.2215123283371736879e-289, 6.7342163555358599277e-306,
 8.0132469526175071002e-258, 2.8687869620228451614e-274, -1.9537812801257956865e-290, 1.0380272777574237546e-306,
 3.850752120757712373e-258, 2.8687869620228451614e-274, -1.9537812801257956865e-290, 1.0380272777574237546e-306,
 1.7695047048278150093e-258, 2.8687869620228451614e-274, -1.9537812801257956865e-290, 1.0380272777574237546e-306,
 7.2888099686286655858e-259, 5.581381609158630475e-275, 6.1155422068568946933e-291, 1.0380272777574237546e-306,
 2.0856914288039227544e-259, -1.9524039360882352712e-276, -2.9779654517181712829e-292, -3.000817432603284506e-308,
 2.0856914288039227544e-259, -1.9524039360882352712e-276, -2.9779654517181712829e-292, -3.000817432603284506e-308,
 7.8491179384773690214e-260, -1.9524039360882352712e-276, -2.9779654517181712829e-292, -3.000817432603284506e-308,
 1.345219763696439399e-260, 1.6579848156414234801e-276, 1.0303712682997738281e-292, 1.4493302844111182601e-308,
 1.345219763696439399e-260, 1.6579848156414234801e-276, 1.0303712682997738281e-292, 1.4493302844111182601e-308,
 1.345219763696439399e-260, 1.6579848156414234801e-276, 1.0303712682997738281e-292, 1.4493302844111182601e-308,
 5.3223249184882342185e-261, -1.472095602234059958e-277, 2.8287088295287585094e-294, -1.0874435234232647519e-310,
 1.2573885592501529789e-261, 3.0408903374280139822e-277, 2.8287088295287585094e-294, -1.0874435234232647519e-310,
 1.2573885592501529789e-261, 3.0408903374280139822e-277, 2.8287088295287585094e-294, -1.0874435234232647519e-310,
 2.4115446944063306384e-262, 2.202741251392177696e-278, 2.8287088295287585094e-294, -1.0874435234232647519e-310,
 2.4115446944063306384e-262, 2.202741251392177696e-278, 2.8287088295287585094e-294, -1.0874435234232647519e-310,
 2.4115446944063306384e-262, 2.202741251392177696e-278, 2.8287088295287585094e-294, -1.0874435234232647519e-310,
 1.1412520821444306741e-262, -6.1787496089661820348e-279, -3.028042329852615431e-295, -2.182740474438892116e-311,
 5.0610577601348040988e-263, 7.9243314524777990283e-279, -3.028042329852615431e-295, -2.182740474438892116e-311,
 1.8853262294800541881e-263, 8.7279092175580810531e-280, 8.8634899828990930877e-296, -9.8167844904532653004e-314,
 2.9746046415267896827e-264, -8.6516445844406224413e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 2.9746046415267896827e-264, -8.6516445844406224413e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 2.9746046415267896827e-264, -8.6516445844406224413e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 9.8977243486757054781e-265, -8.6516445844406224413e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 9.8977243486757054781e-265, -8.6516445844406224413e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 4.9356438320276576408e-265, -8.6516445844406224413e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 2.4546035737036337221e-265, -8.6516445844406224413e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 1.2140834445416214873e-265, 1.8893435613692150014e-281, 3.0075895258731974416e-297, -9.8167844904532653004e-314,
 5.9382337996061564537e-266, 5.1208955146257653156e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 2.8369334767011265554e-266, 5.1208955146257653156e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 1.2862833152486119506e-266, 1.6777604898591683764e-282, -5.0528699238150265939e-299, -1.3288013265921760399e-314,
 5.1095823452235464813e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300, -2.5539572388808429997e-317,
 1.2329569415922591084e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300, -2.5539572388808429997e-317,
 1.2329569415922591084e-267, -4.3807022524130141006e-284, -2.7456019707854725967e-300, -2.5539572388808429997e-317,
 2.638005906844371576e-268, 6.3790946999826013345e-284, -2.7456019707854725967e-300, -2.5539572388808429997e-317,
 2.638005906844371576e-268, 6.3790946999826013345e-284, -2.7456019707854725967e-300, -2.5539572388808429997e-317,
 2.1511502957481757317e-269, 3.2670891426006735363e-285, 2.4084160842482773317e-301, 5.7350888195772519812e-317,
 2.1511502957481757317e-269, 3.2670891426006735363e-285, 2.4084160842482773317e-301, 5.7350888195772519812e-317,
 2.1511502957481757317e-269, 3.2670891426006735363e-285, 2.4084160842482773317e-301, 5.7350888195772519812e-317,
 2.1511502957481757317e-269, 3.2670891426006735363e-285, 2.4084160842482773317e-301, 5.7350888195772519812e-317,
 6.3684349745470443788e-270, -9.5347405022956030541e-287, -1.5805886663557401565e-302, 3.6369654387311681856e-319,
 6.3684349745470443788e-270, -9.5347405022956030541e-287, -1.5805886663557401565e-302, 3.6369654387311681856e-319,
 2.5826679788133653036e-270, -9.5347405022956030541e-287, -1.5805886663557401565e-302, 3.6369654387311681856e-319,
 6.8978448094652555593e-271, 1.1480487920352081009e-286, 7.5257037990230704094e-303, 3.6369654387311681856e-319,
 6.8978448094652555593e-271, 1.1480487920352081009e-286, 7.5257037990230704094e-303, 3.6369654387311681856e-319,
 2.1656360647981577662e-271, 9.7287370902823839435e-288, 1.6928061833779524157e-303, 3.6369654387311681856e-319,
 2.1656360647981577662e-271, 9.7287370902823839435e-288, 1.6928061833779524157e-303, 3.6369654387311681856e-319,
 9.825838786313830552e-272, 9.7287370902823839435e-288, 1.6928061833779524157e-303, 3.6369654387311681856e-319,
 3.9105778554799569972e-272, 9.7287370902823839435e-288, 1.6928061833779524157e-303, 3.6369654387311681856e-319,
 9.5294739006302120482e-273, -1.2215123283371736879e-289, 6.7342163555358599277e-306, -5.681754927174335258e-322,
 9.5294739006302120482e-273, -1.2215123283371736879e-289, 6.7342163555358599277e-306, -5.681754927174335258e-322,
 2.1353977370878701046e-273, -1.2215123283371736879e-289, 6.7342163555358599277e-306, -5.681754927174335258e-322,
 2.1353977370878701046e-273, -1.2215123283371736879e-289, 6.7342163555358599277e-306, -5.681754927174335258e-322,
 2.8687869620228451614e-274, -1.9537812801257956865e-290, 1.0380272777574237546e-306, 6.4228533959362050743e-323,
};


#if defined(_MSC_VER) && !defined (__clang__)
#pragma fp_contract (off)
#endif

NO_SIDE_EFFECTS INLINE constexpr f64 fabsk(f64 x) {
	return __builtin_bit_cast(f64,INT64_C(0x7fffffffffffffff) & __builtin_bit_cast(i64 ,x));
}

NO_SIDE_EFFECTS INLINE constexpr f64 mulsign(f64 x, f64 y) {
	return __builtin_bit_cast(f64,__builtin_bit_cast(i64 ,x) ^ (__builtin_bit_cast(i64 ,y) & (INT64_C(1) << 63)));
}

NO_SIDE_EFFECTS INLINE constexpr f64 copysignk(f64 x, f64 y) {
	return __builtin_bit_cast(f64,(__builtin_bit_cast(i64 ,x) & ~(INT64_C(1) << 63)) ^ (__builtin_bit_cast(i64 ,y) & (INT64_C(1) << 63)));
}

NO_SIDE_EFFECTS INLINE constexpr f64 sign(f64 d) { return mulsign(1, d); }
//NO_SIDE_EFFECTS INLINE constexpr f64 mla(f64 x, f64 y, f64 z) { return x * y + z; }
NO_SIDE_EFFECTS INLINE constexpr f64 rintk(f64 x) { return x < 0 ? (i32)(x - 0.5) : (i32)(x + 0.5); }
NO_SIDE_EFFECTS INLINE constexpr i32 ceilk(f64 x) { return (i32)x + (x < 0 ? 0 : 1); }
NO_SIDE_EFFECTS INLINE constexpr f64 trunck(f64 x) { return (f64)(i32)x; }
NO_SIDE_EFFECTS INLINE constexpr f64 fmink(f64 x, f64 y) { return x < y ? x : y; }
NO_SIDE_EFFECTS INLINE constexpr f64 fmaxk(f64 x, f64 y) { return x > y ? x : y; }

NO_SIDE_EFFECTS INLINE constexpr i32 xisnan(f64 x) { return x != x; }
NO_SIDE_EFFECTS INLINE constexpr i32 xisinf(f64 x) { return x == SLEEF_INFINITY || x == -SLEEF_INFINITY; }
NO_SIDE_EFFECTS INLINE constexpr i32 xisminf(f64 x) { return x == -SLEEF_INFINITY; }
NO_SIDE_EFFECTS INLINE constexpr i32 xispinf(f64 x) { return x == SLEEF_INFINITY; }
NO_SIDE_EFFECTS INLINE constexpr i32 xisnegzero(f64 x) { return __builtin_bit_cast(i64 ,x) == __builtin_bit_cast(i64 ,-0.0); }
NO_SIDE_EFFECTS INLINE constexpr i32 xisnumber(f64 x) { return !xisinf(x) && !xisnan(x); }

NO_SIDE_EFFECTS INLINE constexpr i32 xisint(f64 d) {
	f64 x = d - (f64)(INT64_C(1) << 31) * (i32)(d * (1.0 / (INT64_C(1) << 31)));
	return (x == (i32)x) || (fabsk(d) >= (f64)(INT64_C(1) << 53));
}

NO_SIDE_EFFECTS INLINE constexpr i32 xisodd(f64 d) {
	f64 x = d - (f64)(INT64_C(1) << 31) * (i32)(d * (1.0 / (INT64_C(1) << 31)));
	return (1 & (i32)x) != 0 && fabsk(d) < (f64)(INT64_C(1) << 53);
}

NO_SIDE_EFFECTS INLINE constexpr f64 pow2i(i32 q) {
	return __builtin_bit_cast(f64,((i64)(q + 0x3ff)) << 52);
}

NO_SIDE_EFFECTS INLINE constexpr f64 ldexpk(f64 x, i32 q) {
	f64 u;
	i32 m;
	m = q >> 31;
	m = (((m + q) >> 9) - m) << 7;
	q = q - (m << 2);
	m += 0x3ff;
	m = m < 0 ? 0 : m;
	m = m > 0x7ff ? 0x7ff : m;
	u = __builtin_bit_cast(f64,((i64)m) << 52);
	x = x * u * u * u * u;
	u = __builtin_bit_cast(f64,((i64)(q + 0x3ff)) << 52);
	return x * u;
}

NO_SIDE_EFFECTS INLINE constexpr f64 ldexp2k(f64 d, i32 e) { // faster than ldexpk, short reach
	return d * pow2i(e >> 1) * pow2i(e - (e >> 1));
}

NO_SIDE_EFFECTS INLINE constexpr f64 ldexp3k(f64 d, i32 e) { // very fast, no denormal
	return __builtin_bit_cast(f64,__builtin_bit_cast(i64 ,d) + (((i64)e) << 52));
}

NO_SIDE_EFFECTS constexpr f64 xldexp(f64 x, i32 exp) {
	if (exp > 2100) exp = 2100;
	if (exp < -2100) exp = -2100;

	i32 e0 = exp >> 2;
	if (exp < 0) e0++;
	if (-100 < exp && exp < 100) e0 = 0;
	i32 e1 = exp - (e0 << 2);

	f64 p = pow2i(e0);
	f64 ret = x * pow2i(e1) * p * p * p * p;

	return ret;
}

NO_SIDE_EFFECTS INLINE constexpr i32 ilogbk(f64 d) {
	i32 m = d < 4.9090934652977266E-91;
	d = m ? 2.037035976334486E90 * d : d;
	i32 q = (__builtin_bit_cast(i64 ,d) >> 52) & 0x7ff;
	q = m ? q - (300 + 0x03ff) : q - 0x03ff;
	return q;
}

// ilogb2k is similar to ilogbk, but the argument has to be a
// normalized FP value.
NO_SIDE_EFFECTS INLINE constexpr i32 ilogb2k(f64 d) {
	return ((__builtin_bit_cast(i64 ,d) >> 52) & 0x7ff) - 0x3ff;
}

NO_SIDE_EFFECTS constexpr i32 xilogb(f64 d) {
	i32 e = ilogbk(fabsk(d));
	e = d == 0.0 ? SLEEF_FP_ILOGB0 : e;
	e = xisnan(d) ? SLEEF_FP_ILOGBNAN : e;
	e = xisinf(d) ? INT_MAX : e;
	return e;
}

NO_SIDE_EFFECTS INLINE constexpr f64 upper(f64 d) {
	return __builtin_bit_cast(f64,__builtin_bit_cast(i64 ,d) & INT64_C(0xfffffffff8000000));
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 dd(f64 h, f64 l) {
	Sleef_double2 ret;
	ret.x = h; ret.y = l;
	return ret;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddnormalize_d2_d2(Sleef_double2 t) {
	Sleef_double2 s;

	s.x = t.x + t.y;
	s.y = t.x - s.x + t.y;

	return s;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddscale_d2_d2_d(Sleef_double2 d, f64 s) {
	Sleef_double2 r;

	r.x = d.x * s;
	r.y = d.y * s;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddneg_d2_d2(Sleef_double2 d) {
	Sleef_double2 r;

	r.x = -d.x;
	r.y = -d.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddabs_d2_d2(Sleef_double2 x) {
	return dd(x.x < 0 ? -x.x : x.x, x.x < 0 ? -x.y : x.y);
}

/*
 * ddadd and ddadd2 are functions for f64-f64 addition. ddadd
 * is simpler and faster than ddadd2, but it requires the absolute
 * value of first argument to be larger than the second argument. The
 * exact condition that should be met is checked if NDEBUG macro is
 * not defined.
 *
 * Please note that if the results won't be used, it is no problem to
 * feed arguments that do not meet this condition. You will see
 * warning messages if you turn off NDEBUG macro and run tester2, but
 * this is normal.
 *
 * Please see :
 * Jonathan Richard Shewchuk, Adaptive Precision Floating-Point
 * Arithmetic and Fast Robust Geometric Predicates, Discrete &
 * Computational Geometry 18:305-363, 1997.
 */

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddadd_d2_d_d(f64 x, f64 y) {
	// |x| >= |y|

	Sleef_double2 r;

	r.x = x + y;
	r.y = x - r.x + y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddadd2_d2_d_d(f64 x, f64 y) {
	Sleef_double2 r;

	r.x = x + y;
	f64 v = r.x - x;
	r.y = (x - (r.x - v)) + (y - v);

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddadd_d2_d2_d(Sleef_double2 x, f64 y) {
	// |x| >= |y|

	Sleef_double2 r;

	r.x = x.x + y;
	r.y = x.x - r.x + y + x.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddadd2_d2_d2_d(Sleef_double2 x, f64 y) {
	Sleef_double2 r;

	r.x = x.x + y;
	f64 v = r.x - x.x;
	r.y = (x.x - (r.x - v)) + (y - v);
	r.y += x.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddadd_d2_d_d2(f64 x, Sleef_double2 y) {
	// |x| >= |y|

	Sleef_double2 r;

	r.x = x + y.x;
	r.y = x - r.x + y.x + y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddadd2_d2_d_d2(f64 x, Sleef_double2 y) {
	Sleef_double2 r;

	r.x = x + y.x;
	f64 v = r.x - x;
	r.y = (x - (r.x - v)) + (y.x - v) + y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr f64 ddadd2_d_d_d2(f64 x, Sleef_double2 y) { return y.y + y.x + x; }

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddadd_d2_d2_d2(Sleef_double2 x, Sleef_double2 y) {
	// |x| >= |y|

	Sleef_double2 r;

	r.x = x.x + y.x;
	r.y = x.x - r.x + y.x + x.y + y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddadd2_d2_d2_d2(Sleef_double2 x, Sleef_double2 y) {
	Sleef_double2 r;

	r.x = x.x + y.x;
	f64 v = r.x - x.x;
	r.y = (x.x - (r.x - v)) + (y.x - v);
	r.y += x.y + y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddsub_d2_d2_d2(Sleef_double2 x, Sleef_double2 y) {
	// |x| >= |y|

	Sleef_double2 r;

	r.x = x.x - y.x;
	r.y = x.x - r.x - y.x + x.y - y.y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 dddiv_d2_d2_d2(Sleef_double2 n, Sleef_double2 d) {
	f64 t = 1.0 / d.x;
	f64 dh = upper(d.x), dl = d.x - dh;
	f64 th = upper(t), tl = t - th;
	f64 nhh = upper(n.x), nhl = n.x - nhh;

	Sleef_double2 q;

	q.x = n.x * t;

	f64 u = -q.x + nhh * th + nhh * tl + nhl * th + nhl * tl +
		q.x * (1 - dh * th - dh * tl - dl * th - dl * tl);

	q.y = t * (n.y - q.x * d.y) + u;

	return q;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddmul_d2_d_d(f64 x, f64 y) {
	f64 xh = upper(x), xl = x - xh;
	f64 yh = upper(y), yl = y - yh;
	Sleef_double2 r;

	r.x = x * y;
	r.y = xh * yh - r.x + xl * yh + xh * yl + xl * yl;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddmul_d2_d2_d(Sleef_double2 x, f64 y) {
	f64 xh = upper(x.x), xl = x.x - xh;
	f64 yh = upper(y), yl = y - yh;
	Sleef_double2 r;

	r.x = x.x * y;
	r.y = xh * yh - r.x + xl * yh + xh * yl + xl * yl + x.y * y;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddmul_d2_d2_d2(Sleef_double2 x, Sleef_double2 y) {
	f64 xh = upper(x.x), xl = x.x - xh;
	f64 yh = upper(y.x), yl = y.x - yh;
	Sleef_double2 r;

	r.x = x.x * y.x;
	r.y = xh * yh - r.x + xl * yh + xh * yl + xl * yl + x.x * y.y + x.y * y.x;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr f64 ddmul_d_d2_d2(Sleef_double2 x, Sleef_double2 y) {
	f64 xh = upper(x.x), xl = x.x - xh;
	f64 yh = upper(y.x), yl = y.x - yh;

	return x.y * yh + xh * y.y + xl * yl + xh * yl + xl * yh + xh * yh;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddsqu_d2_d2(Sleef_double2 x) {
	f64 xh = upper(x.x), xl = x.x - xh;
	Sleef_double2 r;

	r.x = x.x * x.x;
	r.y = xh * xh - r.x + (xh + xh) * xl + xl * xl + x.x * (x.y + x.y);

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr f64 ddsqu_d_d2(Sleef_double2 x) {
	f64 xh = upper(x.x), xl = x.x - xh;

	return xh * x.y + xh * x.y + xl * xl + (xh * xl + xh * xl) + xh * xh;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddrec_d2_d(f64 d) {
	f64 t = 1.0 / d;
	f64 dh = upper(d), dl = d - dh;
	f64 th = upper(t), tl = t - th;
	Sleef_double2 q;

	q.x = t;
	q.y = t * (1 - dh * th - dh * tl - dl * th - dl * tl);

	return q;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddrec_d2_d2(Sleef_double2 d) {
	f64 t = 1.0 / d.x;
	f64 dh = upper(d.x), dl = d.x - dh;
	f64 th = upper(t), tl = t - th;
	Sleef_double2 q;

	q.x = t;
	q.y = t * (1 - dh * th - dh * tl - dl * th - dl * tl - d.y * t);

	return q;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddsqrt_d2_d2(Sleef_double2 d) {
	f64 t = Math::sqrt(d.x + d.y);
	return ddscale_d2_d2_d(ddmul_d2_d2_d2(ddadd2_d2_d2_d2(d, ddmul_d2_d_d(t, t)), ddrec_d2_d(t)), 0.5);
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddsqrt_d2_d(f64 d) {
	f64 t = Math::sqrt(d);
	return ddscale_d2_d2_d(ddmul_d2_d2_d2(ddadd2_d2_d_d2(d, ddmul_d2_d_d(t, t)), ddrec_d2_d(t)), 0.5);
}

NO_SIDE_EFFECTS INLINE constexpr f64 atan2k(f64 y, f64 x) {
	f64 s, t, u;
	i32 q = 0;

	if (x < 0) { x = -x; q = -2; }
	if (y > x) { t = x; x = y; y = -t; q += 1; }

	s = y / x;
	t = s * s;

	f64 t2 = t * t, t4 = t2 * t2, t8 = t4 * t4, t16 = t8 * t8;
	u = POLY19(t, t2, t4, t8, t16,
			   -1.88796008463073496563746e-05,
			   0.000209850076645816976906797,
			   -0.00110611831486672482563471,
			   0.00370026744188713119232403,
			   -0.00889896195887655491740809,
			   0.016599329773529201970117,
			   -0.0254517624932312641616861,
			   0.0337852580001353069993897,
			   -0.0407629191276836500001934,
			   0.0466667150077840625632675,
			   -0.0523674852303482457616113,
			   0.0587666392926673580854313,
			   -0.0666573579361080525984562,
			   0.0769219538311769618355029,
			   -0.090908995008245008229153,
			   0.111111105648261418443745,
			   -0.14285714266771329383765,
			   0.199999999996591265594148,
			   -0.333333333333311110369124);

	t = u * t * s + s;
	t = q * (Math::pi<f64> / 2) + t;

	return t;
}

NO_SIDE_EFFECTS constexpr f64 xatan2(f64 y, f64 x) {
	f64 r = atan2k(fabsk(y), x);

	r = mulsign(r, x);
	if (xisinf(x) || x == 0) r = Math::pi<f64> / 2 - (xisinf(x) ? (sign(x) * (Math::pi<f64> / 2)) : 0);
	if (xisinf(y)) r = Math::pi<f64> / 2 - (xisinf(x) ? (sign(x) * (Math::pi<f64> *1 / 4)) : 0);
	if (y == 0) r = (sign(x) == -1 ? Math::pi<f64> : 0);

	return xisnan(x) || xisnan(y) ? SLEEF_NAN : mulsign(r, y);
}

NO_SIDE_EFFECTS constexpr f64 xasin(f64 d) {
	i32 o = fabsk(d) < 0.5;
	f64 x2 = o ? (d * d) : ((1 - fabsk(d)) * 0.5), x = o ? fabsk(d) : Math::sqrt(x2), u;

	f64 x4 = x2 * x2, x8 = x4 * x4, x16 = x8 * x8;
	u = POLY12(x2, x4, x8, x16,
			   +0.3161587650653934628e-1,
			   -0.1581918243329996643e-1,
			   +0.1929045477267910674e-1,
			   +0.6606077476277170610e-2,
			   +0.1215360525577377331e-1,
			   +0.1388715184501609218e-1,
			   +0.1735956991223614604e-1,
			   +0.2237176181932048341e-1,
			   +0.3038195928038132237e-1,
			   +0.4464285681377102438e-1,
			   +0.7500000000378581611e-1,
			   +0.1666666666666497543e+0);

	u = mla(u, x * x2, x);

	f64 r = o ? u : (Math::pi<f64> / 2 - 2 * u);
	r = mulsign(r, d);

	return r;
}

NO_SIDE_EFFECTS constexpr f64 xacos(f64 d) {
	i32 o = fabsk(d) < 0.5;
	f64 x2 = o ? (d * d) : ((1 - fabsk(d)) * 0.5), u;
	f64 x = o ? fabsk(d) : Math::sqrt(x2);
	x = fabsk(d) == 1.0 ? 0 : x;

	f64 x4 = x2 * x2, x8 = x4 * x4, x16 = x8 * x8;
	u = POLY12(x2, x4, x8, x16,
			   +0.3161587650653934628e-1,
			   -0.1581918243329996643e-1,
			   +0.1929045477267910674e-1,
			   +0.6606077476277170610e-2,
			   +0.1215360525577377331e-1,
			   +0.1388715184501609218e-1,
			   +0.1735956991223614604e-1,
			   +0.2237176181932048341e-1,
			   +0.3038195928038132237e-1,
			   +0.4464285681377102438e-1,
			   +0.7500000000378581611e-1,
			   +0.1666666666666497543e+0);

	u *= x * x2;

	f64 y = 3.1415926535897932 / 2 - (mulsign(x, d) + mulsign(u, d));
	x += u;
	f64 r = o ? y : (x * 2);
	if (!o && d < 0) r = ddadd_d2_d2_d(dd(3.141592653589793116, 1.2246467991473532072e-16), -r).x;

	return r;
}

NO_SIDE_EFFECTS constexpr f64 xatan(f64 s) {
	f64 t, u;
	i32 q = 0;

	if (sign(s) == -1) { s = -s; q = 2; }
	if (s > 1) { s = 1.0 / s; q |= 1; }

	t = s * s;

	f64 t2 = t * t, t4 = t2 * t2, t8 = t4 * t4, t16 = t8 * t8;
	u = POLY19(t, t2, t4, t8, t16,
			   -1.88796008463073496563746e-05,
			   0.000209850076645816976906797,
			   -0.00110611831486672482563471,
			   0.00370026744188713119232403,
			   -0.00889896195887655491740809,
			   0.016599329773529201970117,
			   -0.0254517624932312641616861,
			   0.0337852580001353069993897,
			   -0.0407629191276836500001934,
			   0.0466667150077840625632675,
			   -0.0523674852303482457616113,
			   0.0587666392926673580854313,
			   -0.0666573579361080525984562,
			   0.0769219538311769618355029,
			   -0.090908995008245008229153,
			   0.111111105648261418443745,
			   -0.14285714266771329383765,
			   0.199999999996591265594148,
			   -0.333333333333311110369124);

	t = s + s * (t * u);

	if ((q & 1) != 0) t = 1.570796326794896557998982 - t;
	if ((q & 2) != 0) t = -t;

	return t;
}

constexpr Sleef_double2 atan2k_u1(Sleef_double2 y, Sleef_double2 x) {
	f64 u;
	Sleef_double2 s, t;
	i32 q = 0;

	if (x.x < 0) { x.x = -x.x; x.y = -x.y; q = -2; }
	if (y.x > x.x) { t = x; x = y; y.x = -t.x; y.y = -t.y; q += 1; }

	s = dddiv_d2_d2_d2(y, x);
	t = ddsqu_d2_d2(s);
	t = ddnormalize_d2_d2(t);

	f64 t2 = t.x * t.x, t4 = t2 * t2, t8 = t4 * t4;
	u = POLY16(t.x, t2, t4, t8,
			   1.06298484191448746607415e-05,
			   -0.000125620649967286867384336,
			   0.00070557664296393412389774,
			   -0.00251865614498713360352999,
			   0.00646262899036991172313504,
			   -0.0128281333663399031014274,
			   0.0208024799924145797902497,
			   -0.0289002344784740315686289,
			   0.0359785005035104590853656,
			   -0.041848579703592507506027,
			   0.0470843011653283988193763,
			   -0.0524914210588448421068719,
			   0.0587946590969581003860434,
			   -0.0666620884778795497194182,
			   0.0769225330296203768654095,
			   -0.0909090442773387574781907);
	u = mla(u, t.x, 0.111111108376896236538123);
	u = mla(u, t.x, -0.142857142756268568062339);
	u = mla(u, t.x, 0.199999999997977351284817);
	u = mla(u, t.x, -0.333333333333317605173818);

	t = ddadd_d2_d2_d2(s, ddmul_d2_d2_d(ddmul_d2_d2_d2(s, t), u));

	if (fabsk(s.x) < 1e-200) t = s;
	t = ddadd2_d2_d2_d2(ddmul_d2_d2_d(dd(1.570796326794896557998982, 6.12323399573676603586882e-17), q), t);

	return t;
}

NO_SIDE_EFFECTS constexpr f64 xatan2_u1(f64 y, f64 x) {
	if (fabsk(x) < 5.5626846462680083984e-309) { y *= (UINT64_C(1) << 53); x *= (UINT64_C(1) << 53); } // nexttoward((1.0 / DBL_MAX), 1)
	Sleef_double2 d = atan2k_u1(dd(fabsk(y), 0), dd(x, 0));
	f64 r = d.x + d.y;

	r = mulsign(r, x);
	if (xisinf(x) || x == 0) r = Math::pi<f64> / 2 - (xisinf(x) ? (sign(x) * (Math::pi<f64> / 2)) : 0);
	if (xisinf(y)) r = Math::pi<f64> / 2 - (xisinf(x) ? (sign(x) * (Math::pi<f64> *1 / 4)) : 0);
	if (y == 0) r = (sign(x) == -1 ? Math::pi<f64> : 0);

	return xisnan(x) || xisnan(y) ? SLEEF_NAN : mulsign(r, y);
}

NO_SIDE_EFFECTS constexpr f64 xasin_u1(f64 d) {
	i32 o = fabsk(d) < 0.5;
	f64 x2 = o ? (d * d) : ((1 - fabsk(d)) * 0.5), u;
	Sleef_double2 x = o ? dd(fabsk(d), 0) : ddsqrt_d2_d(x2);
	x = fabsk(d) == 1.0 ? dd(0, 0) : x;

	f64 x4 = x2 * x2, x8 = x4 * x4, x16 = x8 * x8;
	u = POLY12(x2, x4, x8, x16,
			   +0.3161587650653934628e-1,
			   -0.1581918243329996643e-1,
			   +0.1929045477267910674e-1,
			   +0.6606077476277170610e-2,
			   +0.1215360525577377331e-1,
			   +0.1388715184501609218e-1,
			   +0.1735956991223614604e-1,
			   +0.2237176181932048341e-1,
			   +0.3038195928038132237e-1,
			   +0.4464285681377102438e-1,
			   +0.7500000000378581611e-1,
			   +0.1666666666666497543e+0);

	u *= x2 * x.x;

	Sleef_double2 y = ddadd_d2_d2_d(ddsub_d2_d2_d2(dd(3.141592653589793116 / 4, 1.2246467991473532072e-16 / 4), x), -u);
	f64 r = o ? (u + x.x) : ((y.x + y.y) * 2);
	r = mulsign(r, d);

	return r;
}

NO_SIDE_EFFECTS constexpr f64 xacos_u1(f64 d) {
	i32 o = fabsk(d) < 0.5;
	f64 x2 = o ? (d * d) : ((1 - fabsk(d)) * 0.5), u;
	Sleef_double2 x = o ? dd(fabsk(d), 0) : ddsqrt_d2_d(x2);
	x = fabsk(d) == 1.0 ? dd(0, 0) : x;

	f64 x4 = x2 * x2, x8 = x4 * x4, x16 = x8 * x8;
	u = POLY12(x2, x4, x8, x16,
			   +0.3161587650653934628e-1,
			   -0.1581918243329996643e-1,
			   +0.1929045477267910674e-1,
			   +0.6606077476277170610e-2,
			   +0.1215360525577377331e-1,
			   +0.1388715184501609218e-1,
			   +0.1735956991223614604e-1,
			   +0.2237176181932048341e-1,
			   +0.3038195928038132237e-1,
			   +0.4464285681377102438e-1,
			   +0.7500000000378581611e-1,
			   +0.1666666666666497543e+0);

	u *= x.x * x2;

	Sleef_double2 y = ddsub_d2_d2_d2(dd(3.141592653589793116 / 2, 1.2246467991473532072e-16 / 2),
									 ddadd_d2_d_d(mulsign(x.x, d), mulsign(u, d)));
	x = ddadd_d2_d2_d(x, u);
	y = o ? y : ddscale_d2_d2_d(x, 2);
	if (!o && d < 0) y = ddsub_d2_d2_d2(dd(3.141592653589793116, 1.2246467991473532072e-16), y);

	return y.x + y.y;
}

NO_SIDE_EFFECTS constexpr f64 xatan_u1(f64 d) {
	Sleef_double2 d2 = atan2k_u1(dd(fabsk(d), 0), dd(1, 0));
	f64 r = d2.x + d2.y;
	if (xisinf(d)) r = 1.570796326794896557998982;
	return mulsign(r, d);
}

typedef struct {
	f64 d;
	i32 i;
} di_t;

typedef struct {
	Sleef_double2 dd;
	i32 i;
} ddi_t;

NO_SIDE_EFFECTS INLINE constexpr f64 orsign(f64 x, f64 y) {
	return __builtin_bit_cast(f64,__builtin_bit_cast(i64 ,x) | (__builtin_bit_cast(i64 ,y) & (INT64_C(1) << 63)));
}

NO_SIDE_EFFECTS constexpr di_t rempisub(f64 x) {
	// This function is equivalent to :
	// di_t ret = { x - rint(4 * x) * 0.25, (i32)(rint(4 * x) - rint(x) * 4) };
	di_t ret;
	f64 c = mulsign(INT64_C(1) << 52, x);
	f64 rint4x = fabsk(4 * x) > INT64_C(1) << 52 ? (4 * x) : orsign(mla(4, x, c) - c, x);
	f64 rintx = fabsk(x) > INT64_C(1) << 52 ? x : orsign(x + c - c, x);
	ret.d = mla(-0.25, rint4x, x);
	ret.i = static_cast<i32>(mla(-4, rintx, rint4x));
	return ret;
}

// Payne-Hanek like argument reduction
NO_SIDE_EFFECTS constexpr ddi_t rempi(f64 a) {
	Sleef_double2 x, y;
	di_t di;
	i32 ex = ilogb2k(a) - 55, q = ex > (700 - 55) ? -64 : 0;
	a = ldexp3k(a, q);
	if (ex < 0) ex = 0;
	ex *= 4;
	x = ddmul_d2_d_d(a, Sleef_rempitabdp[ex]);
	di = rempisub(x.x);
	q = di.i;
	x.x = di.d;
	x = ddnormalize_d2_d2(x);
	y = ddmul_d2_d_d(a, Sleef_rempitabdp[ex + 1]);
	x = ddadd2_d2_d2_d2(x, y);
	di = rempisub(x.x);
	q += di.i;
	x.x = di.d;
	x = ddnormalize_d2_d2(x);
	y = ddmul_d2_d2_d(dd(Sleef_rempitabdp[ex + 2], Sleef_rempitabdp[ex + 3]), a);
	x = ddadd2_d2_d2_d2(x, y);
	x = ddnormalize_d2_d2(x);
	x = ddmul_d2_d2_d2(x, dd(3.141592653589793116 * 2, 1.2246467991473532072e-16 * 2));
	ddi_t ret = { fabsk(a) < 0.7 ? dd(a, 0) : x, q };
	return ret;
}

NO_SIDE_EFFECTS constexpr f64 xsin(f64 d) {
	f64 u, s, t = d;
	i32 ql;

	if (fabsk(d) < TRIGRANGEMAX2) {
		ql = static_cast<i32>(rintk(d * (1 / Math::pi<f64>)));
		d = mla(ql, -PI_A2, d);
		d = mla(ql, -PI_B2, d);
	}
	else if (fabsk(d) < TRIGRANGEMAX) {
		f64 dqh = trunck(d * ((1 / Math::pi<f64>) / (1 << 24))) * (f64)(1 << 24);
		ql = static_cast<i32>(rintk(mla(d, (1 / Math::pi<f64>), -dqh)));

		d = mla(dqh, -PI_A, d);
		d = mla(ql, -PI_A, d);
		d = mla(dqh, -PI_B, d);
		d = mla(ql, -PI_B, d);
		d = mla(dqh, -PI_C, d);
		d = mla(ql, -PI_C, d);
		d = mla(dqh + ql, -PI_D, d);
	}
	else {
		ddi_t ddi = rempi(t);
		ql = ((ddi.i & 3) * 2 + (ddi.dd.x > 0) + 1) >> 2;
		if ((ddi.i & 1) != 0) {
			ddi.dd = ddadd2_d2_d2_d2(ddi.dd, dd(mulsign(3.141592653589793116 * -0.5, ddi.dd.x),
												mulsign(1.2246467991473532072e-16 * -0.5, ddi.dd.x)));
		}
		d = ddi.dd.x + ddi.dd.y;
		if (xisinf(t) || xisnan(t)) d = SLEEF_NAN;
	}

	s = d * d;

	if ((ql & 1) != 0) d = -d;

	f64 s2 = s * s, s4 = s2 * s2;
	u = POLY8(s, s2, s4,
			  -7.97255955009037868891952e-18,
			  2.81009972710863200091251e-15,
			  -7.64712219118158833288484e-13,
			  1.60590430605664501629054e-10,
			  -2.50521083763502045810755e-08,
			  2.75573192239198747630416e-06,
			  -0.000198412698412696162806809,
			  0.00833333333333332974823815);
	u = mla(u, s, -0.166666666666666657414808);

	u = mla(s, u * d, d);

	if (xisnegzero(t)) u = t;

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xsin_u1(f64 d) {
	f64 u;
	Sleef_double2 s, t, x;
	i32 ql;

	if (fabsk(d) < TRIGRANGEMAX2) {
		ql = static_cast<i32>(rintk(d * (1 / Math::pi<f64>)));
		u = mla(ql, -PI_A2, d);
		s = ddadd_d2_d_d(u, ql * -PI_B2);
	}
	else if (fabsk(d) < TRIGRANGEMAX) {
		const f64 dqh = trunck(d * ((1 / Math::pi<f64>) / (1 << 24))) * (f64)(1 << 24);
		ql = static_cast<i32>(rintk(mla(d, (1 / Math::pi<f64>), -dqh)));

		u = mla(dqh, -PI_A, d);
		s = ddadd_d2_d_d(u, ql * -PI_A);
		s = ddadd2_d2_d2_d(s, dqh * -PI_B);
		s = ddadd2_d2_d2_d(s, ql * -PI_B);
		s = ddadd2_d2_d2_d(s, dqh * -PI_C);
		s = ddadd2_d2_d2_d(s, ql * -PI_C);
		s = ddadd_d2_d2_d(s, (dqh + ql) * -PI_D);
	}
	else {
		ddi_t ddi = rempi(d);
		ql = ((ddi.i & 3) * 2 + (ddi.dd.x > 0) + 1) >> 2;
		if ((ddi.i & 1) != 0) {
			ddi.dd = ddadd2_d2_d2_d2(ddi.dd, dd(mulsign(3.141592653589793116 * -0.5, ddi.dd.x),
												mulsign(1.2246467991473532072e-16 * -0.5, ddi.dd.x)));
		}
		s = ddnormalize_d2_d2(ddi.dd);
		if (xisinf(d) || xisnan(d)) s.x = SLEEF_NAN;
	}

	t = s;
	s = ddsqu_d2_d2(s);

	f64 s2 = s.x * s.x, s4 = s2 * s2;
	u = POLY6(s.x, s2, s4,
			  2.72052416138529567917983e-15,
			  -7.6429259411395447190023e-13,
			  1.60589370117277896211623e-10,
			  -2.5052106814843123359368e-08,
			  2.75573192104428224777379e-06,
			  -0.000198412698412046454654947);
	u = mla(u, s.x, 0.00833333333333318056201922);

	x = ddadd_d2_d_d2(1, ddmul_d2_d2_d2(ddadd_d2_d_d(-0.166666666666666657414808, u * s.x), s));
	u = ddmul_d_d2_d2(t, x);

	if ((ql & 1) != 0) u = -u;
	if (xisnegzero(d)) u = d;

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xcos(f64 d) {
	f64 u, s, t = d;
	i32 ql;

	if (fabsk(d) < TRIGRANGEMAX2) {
		ql = static_cast<i32>(mla(2., rintk(d * (1 / Math::pi<f64>) - 0.5), 1));
		d = mla(ql, -PI_A2 * 0.5, d);
		d = mla(ql, -PI_B2 * 0.5, d);
	}
	else if (fabsk(d) < TRIGRANGEMAX) {
		f64 dqh = trunck(d * ((1 / Math::pi<f64>) / (INT64_C(1) << 23)) - 0.5 * ((1 / Math::pi<f64>) / (INT64_C(1) << 23)));
		ql = static_cast<i32>(2 * rintk(d * (1 / Math::pi<f64>) - 0.5 - dqh * (f64)(INT64_C(1) << 23)) + 1);
		dqh *= 1 << 24;

		d = mla(dqh, -PI_A * 0.5, d);
		d = mla(ql, -PI_A * 0.5, d);
		d = mla(dqh, -PI_B * 0.5, d);
		d = mla(ql, -PI_B * 0.5, d);
		d = mla(dqh, -PI_C * 0.5, d);
		d = mla(ql, -PI_C * 0.5, d);
		d = mla(dqh + ql, -PI_D * 0.5, d);
	}
	else {
		ddi_t ddi = rempi(t);
		ql = ((ddi.i & 3) * 2 + (ddi.dd.x > 0) + 7) >> 1;
		if ((ddi.i & 1) == 0) {
			ddi.dd = ddadd2_d2_d2_d2(ddi.dd, dd(mulsign(3.141592653589793116 * -0.5, ddi.dd.x > 0 ? 1 : -1),
												mulsign(1.2246467991473532072e-16 * -0.5, ddi.dd.x > 0 ? 1 : -1)));
		}
		d = ddi.dd.x + ddi.dd.y;
		if (xisinf(t) || xisnan(t)) d = SLEEF_NAN;
	}

	s = d * d;

	if ((ql & 2) == 0) d = -d;

	f64 s2 = s * s, s4 = s2 * s2;
	u = POLY8(s, s2, s4,
			  -7.97255955009037868891952e-18,
			  2.81009972710863200091251e-15,
			  -7.64712219118158833288484e-13,
			  1.60590430605664501629054e-10,
			  -2.50521083763502045810755e-08,
			  2.75573192239198747630416e-06,
			  -0.000198412698412696162806809,
			  0.00833333333333332974823815);
	u = mla(u, s, -0.166666666666666657414808);

	u = mla(s, u * d, d);

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xcos_u1(f64 d) {
	f64 u;
	Sleef_double2 s, t, x;
	i32 ql;

	d = fabsk(d);

	if (d < TRIGRANGEMAX2) {
		ql = static_cast<i32>(mla(2, rintk(d * (1 / Math::pi<f64>) - 0.5), 1));
		s = ddadd2_d2_d_d(d, ql * (-PI_A2 * 0.5));
		s = ddadd_d2_d2_d(s, ql * (-PI_B2 * 0.5));
	}
	else if (d < TRIGRANGEMAX) {
		f64 dqh = trunck(d * ((1 / Math::pi<f64>) / (INT64_C(1) << 23)) - 0.5 * ((1 / Math::pi<f64>) / (INT64_C(1) << 23)));
		ql = static_cast<i32>(2 * rintk(d * (1 / Math::pi<f64>) - 0.5 - dqh * (f64)(INT64_C(1) << 23)) + 1);
		dqh *= 1 << 24;

		u = mla(dqh, -PI_A * 0.5, d);
		s = ddadd2_d2_d_d(u, ql * (-PI_A * 0.5));
		s = ddadd2_d2_d2_d(s, dqh * (-PI_B * 0.5));
		s = ddadd2_d2_d2_d(s, ql * (-PI_B * 0.5));
		s = ddadd2_d2_d2_d(s, dqh * (-PI_C * 0.5));
		s = ddadd2_d2_d2_d(s, ql * (-PI_C * 0.5));
		s = ddadd_d2_d2_d(s, (dqh + ql) * (-PI_D * 0.5));
	}
	else {
		ddi_t ddi = rempi(d);
		ql = ((ddi.i & 3) * 2 + (ddi.dd.x > 0) + 7) >> 1;
		if ((ddi.i & 1) == 0) {
			ddi.dd = ddadd2_d2_d2_d2(ddi.dd, dd(mulsign(3.141592653589793116 * -0.5, ddi.dd.x > 0 ? 1 : -1),
												mulsign(1.2246467991473532072e-16 * -0.5, ddi.dd.x > 0 ? 1 : -1)));
		}
		s = ddnormalize_d2_d2(ddi.dd);
		if (xisinf(d) || xisnan(d)) s.x = SLEEF_NAN;
	}

	t = s;
	s = ddsqu_d2_d2(s);

	f64 s2 = s.x * s.x, s4 = s2 * s2;
	u = POLY6(s.x, s2, s4,
			  2.72052416138529567917983e-15,
			  -7.6429259411395447190023e-13,
			  1.60589370117277896211623e-10,
			  -2.5052106814843123359368e-08,
			  2.75573192104428224777379e-06,
			  -0.000198412698412046454654947);
	u = mla(u, s.x, 0.00833333333333318056201922);

	x = ddadd_d2_d_d2(1, ddmul_d2_d2_d2(ddadd_d2_d_d(-0.166666666666666657414808, u * s.x), s));
	u = ddmul_d_d2_d2(t, x);

	if ((((i32)ql) & 2) == 0) u = -u;

	return u;
}

NO_SIDE_EFFECTS constexpr SinCos<f64> xsincos(f64 d) {
	f64 u, s, t;
	SinCos<f64> r;
	i32 ql;

	s = d;

	if (fabsk(d) < TRIGRANGEMAX2) {
		ql = static_cast<i32>(rintk(s * (2 * (1 / Math::pi<f64>))));
		s = mla(ql, -PI_A2 * 0.5, s);
		s = mla(ql, -PI_B2 * 0.5, s);
	}
	else if (fabsk(d) < TRIGRANGEMAX) {
		f64 dqh = trunck(d * ((2 * (1 / Math::pi<f64>)) / (1 << 24))) * (f64)(1 << 24);
		ql = static_cast<i32>(rintk(d * (2 * (1 / Math::pi<f64>)) - dqh));

		s = mla(dqh, -PI_A * 0.5, s);
		s = mla(ql, -PI_A * 0.5, s);
		s = mla(dqh, -PI_B * 0.5, s);
		s = mla(ql, -PI_B * 0.5, s);
		s = mla(dqh, -PI_C * 0.5, s);
		s = mla(ql, -PI_C * 0.5, s);
		s = mla(dqh + ql, -PI_D * 0.5, s);
	}
	else {
		ddi_t ddi = rempi(d);
		ql = ddi.i;
		s = ddi.dd.x + ddi.dd.y;
		if (xisinf(d) || xisnan(d)) s = SLEEF_NAN;
	}

	t = s;

	s = s * s;

	u = 1.58938307283228937328511e-10;
	u = mla(u, s, -2.50506943502539773349318e-08);
	u = mla(u, s, 2.75573131776846360512547e-06);
	u = mla(u, s, -0.000198412698278911770864914);
	u = mla(u, s, 0.0083333333333191845961746);
	u = mla(u, s, -0.166666666666666130709393);
	u = u * s * t;

	r.s = t + u;

	if (xisnegzero(d)) r.s = -0.0;

	u = -1.13615350239097429531523e-11;
	u = mla(u, s, 2.08757471207040055479366e-09);
	u = mla(u, s, -2.75573144028847567498567e-07);
	u = mla(u, s, 2.48015872890001867311915e-05);
	u = mla(u, s, -0.00138888888888714019282329);
	u = mla(u, s, 0.0416666666666665519592062);
	u = mla(u, s, -0.5);

	r.c = u * s + 1;

	if ((ql & 1) != 0) { s = r.c; r.c = r.s; r.s = s; }
	if ((ql & 2) != 0) { r.s = -r.s; }
	if (((ql + 1) & 2) != 0) { r.c = -r.c; }

	return r;
}

NO_SIDE_EFFECTS constexpr Sleef_double2 xsincos_u1(f64 d) {
	f64 u;
	Sleef_double2 r, s, t, x;
	i32 ql;

	if (fabsk(d) < TRIGRANGEMAX2) {
		ql = static_cast<i32>(rintk(d * (2 * (1 / Math::pi<f64>))));
		u = mla(ql, -PI_A2 * 0.5, d);
		s = ddadd_d2_d_d(u, ql * (-PI_B2 * 0.5));
	}
	else if (fabsk(d) < TRIGRANGEMAX) {
		const f64 dqh = trunck(d * ((2 * (1 / Math::pi<f64>)) / (1 << 24))) * (f64)(1 << 24);
		ql = static_cast<i32>(rintk(d * (2 * (1 / Math::pi<f64>)) - dqh));

		u = mla(dqh, -PI_A * 0.5, d);
		s = ddadd_d2_d_d(u, ql * (-PI_A * 0.5));
		s = ddadd2_d2_d2_d(s, dqh * (-PI_B * 0.5));
		s = ddadd2_d2_d2_d(s, ql * (-PI_B * 0.5));
		s = ddadd2_d2_d2_d(s, dqh * (-PI_C * 0.5));
		s = ddadd2_d2_d2_d(s, ql * (-PI_C * 0.5));
		s = ddadd_d2_d2_d(s, (dqh + ql) * (-PI_D * 0.5));
	}
	else {
		ddi_t ddi = rempi(d);
		ql = ddi.i;
		s = ddi.dd;
		if (xisinf(d) || xisnan(d)) s = dd(SLEEF_NAN, SLEEF_NAN);
	}

	t = s;

	s.x = ddsqu_d_d2(s);

	u = 1.58938307283228937328511e-10;
	u = mla(u, s.x, -2.50506943502539773349318e-08);
	u = mla(u, s.x, 2.75573131776846360512547e-06);
	u = mla(u, s.x, -0.000198412698278911770864914);
	u = mla(u, s.x, 0.0083333333333191845961746);
	u = mla(u, s.x, -0.166666666666666130709393);

	u *= s.x * t.x;

	x = ddadd_d2_d2_d(t, u);
	r.x = x.x + x.y;

	if (xisnegzero(d)) r.x = -0.0;

	u = -1.13615350239097429531523e-11;
	u = mla(u, s.x, 2.08757471207040055479366e-09);
	u = mla(u, s.x, -2.75573144028847567498567e-07);
	u = mla(u, s.x, 2.48015872890001867311915e-05);
	u = mla(u, s.x, -0.00138888888888714019282329);
	u = mla(u, s.x, 0.0416666666666665519592062);
	u = mla(u, s.x, -0.5);

	x = ddadd_d2_d_d2(1, ddmul_d2_d_d(s.x, u));
	r.y = x.x + x.y;

	if ((ql & 1) != 0) { u = r.y; r.y = r.x; r.x = u; }
	if ((ql & 2) != 0) { r.x = -r.x; }
	if (((ql + 1) & 2) != 0) { r.y = -r.y; }

	return r;
}

NO_SIDE_EFFECTS constexpr Sleef_double2 xsincospi_u05(f64 d) {
	f64 u, s, t;
	Sleef_double2 r, x, s2;

	u = d * 4;
	i32 q = ceilk(u) & ~(i32)1;

	s = u - (f64)q;
	t = s;
	s = s * s;
	s2 = ddmul_d2_d_d(t, t);

	//

	u = -2.02461120785182399295868e-14;
	u = mla(u, s, 6.94821830580179461327784e-12);
	u = mla(u, s, -1.75724749952853179952664e-09);
	u = mla(u, s, 3.13361688966868392878422e-07);
	u = mla(u, s, -3.6576204182161551920361e-05);
	u = mla(u, s, 0.00249039457019271850274356);
	x = ddadd2_d2_d_d2(u * s, dd(-0.0807455121882807852484731, 3.61852475067037104849987e-18));
	x = ddadd2_d2_d2_d2(ddmul_d2_d2_d2(s2, x), dd(0.785398163397448278999491, 3.06287113727155002607105e-17));

	x = ddmul_d2_d2_d(x, t);
	r.x = x.x + x.y;

	if (xisnegzero(d)) r.x = -0.0;

	//

	u = 9.94480387626843774090208e-16;
	u = mla(u, s, -3.89796226062932799164047e-13);
	u = mla(u, s, 1.15011582539996035266901e-10);
	u = mla(u, s, -2.4611369501044697495359e-08);
	u = mla(u, s, 3.59086044859052754005062e-06);
	u = mla(u, s, -0.000325991886927389905997954);
	x = ddadd2_d2_d_d2(u * s, dd(0.0158543442438155018914259, -1.04693272280631521908845e-18));
	x = ddadd2_d2_d2_d2(ddmul_d2_d2_d2(s2, x), dd(-0.308425137534042437259529, -1.95698492133633550338345e-17));

	x = ddadd2_d2_d2_d(ddmul_d2_d2_d2(x, s2), 1);
	r.y = x.x + x.y;

	//

	if ((q & 2) != 0) { s = r.y; r.y = r.x; r.x = s; }
	if ((q & 4) != 0) { r.x = -r.x; }
	if (((q + 2) & 4) != 0) { r.y = -r.y; }

	if (fabsk(d) > TRIGRANGEMAX3 / 4) { r.x = 0; r.y = 1; }
	if (xisinf(d)) { r.x = r.y = SLEEF_NAN; }

	return r;
}

NO_SIDE_EFFECTS constexpr Sleef_double2 xsincospi_u35(f64 d) {
	f64 u, s, t;
	Sleef_double2 r;

	u = d * 4;
	i32 q = ceilk(u) & ~(i32)1;

	s = u - (f64)q;
	t = s;
	s = s * s;

	//

	u = +0.6880638894766060136e-11;
	u = mla(u, s, -0.1757159564542310199e-8);
	u = mla(u, s, +0.3133616327257867311e-6);
	u = mla(u, s, -0.3657620416388486452e-4);
	u = mla(u, s, +0.2490394570189932103e-2);
	u = mla(u, s, -0.8074551218828056320e-1);
	u = mla(u, s, +0.7853981633974482790e+0);

	r.x = u * t;

	//

	u = -0.3860141213683794352e-12;
	u = mla(u, s, +0.1150057888029681415e-9);
	u = mla(u, s, -0.2461136493006663553e-7);
	u = mla(u, s, +0.3590860446623516713e-5);
	u = mla(u, s, -0.3259918869269435942e-3);
	u = mla(u, s, +0.1585434424381541169e-1);
	u = mla(u, s, -0.3084251375340424373e+0);
	u = mla(u, s, 1);

	r.y = u;

	//

	if ((q & 2) != 0) { s = r.y; r.y = r.x; r.x = s; }
	if ((q & 4) != 0) { r.x = -r.x; }
	if (((q + 2) & 4) != 0) { r.y = -r.y; }

	if (fabsk(d) > TRIGRANGEMAX3 / 4) { r.x = 0; r.y = 1; }
	if (xisinf(d)) { r.x = r.y = SLEEF_NAN; }

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 sinpik(f64 d) {
	f64 u, s, t;
	Sleef_double2 x, s2;

	u = d * 4;
	i32 q = ceilk(u) & ~1;
	i32 o = (q & 2) != 0;

	s = u - (f64)q;
	t = s;
	s = s * s;
	s2 = ddmul_d2_d_d(t, t);

	//

	u = o ? 9.94480387626843774090208e-16 : -2.02461120785182399295868e-14;
	u = mla(u, s, o ? -3.89796226062932799164047e-13 : 6.94821830580179461327784e-12);
	u = mla(u, s, o ? 1.15011582539996035266901e-10 : -1.75724749952853179952664e-09);
	u = mla(u, s, o ? -2.4611369501044697495359e-08 : 3.13361688966868392878422e-07);
	u = mla(u, s, o ? 3.59086044859052754005062e-06 : -3.6576204182161551920361e-05);
	u = mla(u, s, o ? -0.000325991886927389905997954 : 0.00249039457019271850274356);
	x = ddadd2_d2_d_d2(u * s, o ? dd(0.0158543442438155018914259, -1.04693272280631521908845e-18) :
					   dd(-0.0807455121882807852484731, 3.61852475067037104849987e-18));
	x = ddadd2_d2_d2_d2(ddmul_d2_d2_d2(s2, x), o ? dd(-0.308425137534042437259529, -1.95698492133633550338345e-17) :
						dd(0.785398163397448278999491, 3.06287113727155002607105e-17));

	x = ddmul_d2_d2_d2(x, o ? s2 : dd(t, 0));
	x = o ? ddadd2_d2_d2_d(x, 1) : x;

	//

	if ((q & 4) != 0) { x.x = -x.x; x.y = -x.y; }

	return x;
}

NO_SIDE_EFFECTS constexpr f64 xsinpi_u05(f64 d) {
	Sleef_double2 x = sinpik(d);
	f64 r = x.x + x.y;

	if (xisnegzero(d)) r = -0.0;
	if (fabsk(d) > TRIGRANGEMAX3 / 4) r = 0;
	if (xisinf(d)) r = SLEEF_NAN;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 cospik(f64 d) {
	f64 u, s, t;
	Sleef_double2 x, s2;

	u = d * 4;
	i32 q = ceilk(u) & ~1;
	i32 o = (q & 2) == 0;

	s = u - (f64)q;
	t = s;
	s = s * s;
	s2 = ddmul_d2_d_d(t, t);

	//

	u = o ? 9.94480387626843774090208e-16 : -2.02461120785182399295868e-14;
	u = mla(u, s, o ? -3.89796226062932799164047e-13 : 6.94821830580179461327784e-12);
	u = mla(u, s, o ? 1.15011582539996035266901e-10 : -1.75724749952853179952664e-09);
	u = mla(u, s, o ? -2.4611369501044697495359e-08 : 3.13361688966868392878422e-07);
	u = mla(u, s, o ? 3.59086044859052754005062e-06 : -3.6576204182161551920361e-05);
	u = mla(u, s, o ? -0.000325991886927389905997954 : 0.00249039457019271850274356);
	x = ddadd2_d2_d_d2(u * s, o ? dd(0.0158543442438155018914259, -1.04693272280631521908845e-18) :
					   dd(-0.0807455121882807852484731, 3.61852475067037104849987e-18));
	x = ddadd2_d2_d2_d2(ddmul_d2_d2_d2(s2, x), o ? dd(-0.308425137534042437259529, -1.95698492133633550338345e-17) :
						dd(0.785398163397448278999491, 3.06287113727155002607105e-17));

	x = ddmul_d2_d2_d2(x, o ? s2 : dd(t, 0));
	x = o ? ddadd2_d2_d2_d(x, 1) : x;

	//

	if (((q + 2) & 4) != 0) { x.x = -x.x; x.y = -x.y; }

	return x;
}

NO_SIDE_EFFECTS constexpr f64 xcospi_u05(f64 d) {
	Sleef_double2 x = cospik(d);
	f64 r = x.x + x.y;

	if (fabsk(d) > TRIGRANGEMAX3 / 4) r = 1;
	if (xisinf(d)) r = SLEEF_NAN;

	return r;
}

NO_SIDE_EFFECTS constexpr f64 xtan(f64 d) {
	f64 u, s, x, y;
	i32 ql;

	if (fabsk(d) < TRIGRANGEMAX2) {
		ql = static_cast<i32>(rintk(d * (2 * (1 / Math::pi<f64>))));
		x = mla(ql, -PI_A2 * 0.5, d);
		x = mla(ql, -PI_B2 * 0.5, x);
	}
	else if (fabsk(d) < 1e+6) {
		f64 dqh = trunck(d * ((2 * (1 / Math::pi<f64>)) / (1 << 24))) * (f64)(1 << 24);
		ql = static_cast<i32>(rintk(d * (2 * (1 / Math::pi<f64>)) - dqh));

		x = mla(dqh, -PI_A * 0.5, d);
		x = mla(ql, -PI_A * 0.5, x);
		x = mla(dqh, -PI_B * 0.5, x);
		x = mla(ql, -PI_B * 0.5, x);
		x = mla(dqh, -PI_C * 0.5, x);
		x = mla(ql, -PI_C * 0.5, x);
		x = mla(dqh + ql, -PI_D * 0.5, x);
	}
	else {
		ddi_t ddi = rempi(d);
		ql = ddi.i;
		x = ddi.dd.x + ddi.dd.y;
		if (xisinf(d) || xisnan(d)) x = SLEEF_NAN;
	}

	x *= 0.5;
	s = x * x;

	f64 s2 = s * s, s4 = s2 * s2;
	u = POLY8(s, s2, s4,
			  +0.3245098826639276316e-3,
			  +0.5619219738114323735e-3,
			  +0.1460781502402784494e-2,
			  +0.3591611540792499519e-2,
			  +0.8863268409563113126e-2,
			  +0.2186948728185535498e-1,
			  +0.5396825399517272970e-1,
			  +0.1333333333330500581e+0);

	u = mla(u, s, +0.3333333333333343695e+0);
	u = mla(s, u * x, x);

	y = mla(u, u, -1);
	x = -2 * u;

	if ((ql & 1) != 0) { f64 t = x; x = y; y = -t; }

	u = x / y;

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xtan_u1(f64 d) {
	f64 u;
	Sleef_double2 s, t, x, y;
	i32 ql;

	if (fabsk(d) < TRIGRANGEMAX2) {
		ql = static_cast<i32>(rintk(d * (2 * (1 / Math::pi<f64>))));
		u = mla(ql, -PI_A2 * 0.5, d);
		s = ddadd_d2_d_d(u, ql * (-PI_B2 * 0.5));
	}
	else if (fabsk(d) < TRIGRANGEMAX) {
		const f64 dqh = trunck(d * ((2 / Math::pi<f64>) / (1 << 24))) * (f64)(1 << 24);
		s = ddadd2_d2_d2_d(ddmul_d2_d2_d(dd(M_2_PI_H, M_2_PI_L), d), (d < 0 ? -0.5 : 0.5) - dqh);
		ql = static_cast<i32>(s.x + s.y);

		u = mla(dqh, -PI_A * 0.5, d);
		s = ddadd_d2_d_d(u, ql * (-PI_A * 0.5));
		s = ddadd2_d2_d2_d(s, dqh * (-PI_B * 0.5));
		s = ddadd2_d2_d2_d(s, ql * (-PI_B * 0.5));
		s = ddadd2_d2_d2_d(s, dqh * (-PI_C * 0.5));
		s = ddadd2_d2_d2_d(s, ql * (-PI_C * 0.5));
		s = ddadd_d2_d2_d(s, (dqh + ql) * (-PI_D * 0.5));
	}
	else {
		ddi_t ddi = rempi(d);
		ql = ddi.i;
		s = ddi.dd;
		if (xisinf(d) || xisnan(d)) s.x = SLEEF_NAN;
	}

	t = ddscale_d2_d2_d(s, 0.5);
	s = ddsqu_d2_d2(t);

	f64 s2 = s.x * s.x, s4 = s2 * s2;
	u = POLY8(s.x, s2, s4,
			  +0.3245098826639276316e-3,
			  +0.5619219738114323735e-3,
			  +0.1460781502402784494e-2,
			  +0.3591611540792499519e-2,
			  +0.8863268409563113126e-2,
			  +0.2186948728185535498e-1,
			  +0.5396825399517272970e-1,
			  +0.1333333333330500581e+0);

	u = mla(u, s.x, +0.3333333333333343695e+0);
	x = ddadd_d2_d2_d2(t, ddmul_d2_d2_d(ddmul_d2_d2_d2(s, t), u));

	y = ddadd_d2_d_d2(-1, ddsqu_d2_d2(x));
	x = ddscale_d2_d2_d(x, -2);

	if ((ql & 1) != 0) { t = x; x = y; y = ddneg_d2_d2(t); }

	x = dddiv_d2_d2_d2(x, y);

	u = x.x + x.y;

	if (xisnegzero(d)) u = d;

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xlog(f64 d) {
	f64 x, x2, t, m;
	i32 e;

	i32 o = d < DBL_MIN;
	if (o) d *= (f64)(INT64_C(1) << 32) * (f64)(INT64_C(1) << 32);

	e = ilogb2k(d * (1.0 / 0.75));
	m = ldexp3k(d, -e);

	if (o) e -= 64;

	x = (m - 1) / (m + 1);
	x2 = x * x;

	f64 x4 = x2 * x2, x8 = x4 * x4;

	t = POLY7(x2, x4, x8,
			  0.153487338491425068243146,
			  0.152519917006351951593857,
			  0.181863266251982985677316,
			  0.222221366518767365905163,
			  0.285714294746548025383248,
			  0.399999999950799600689777,
			  0.6666666666667778740063);

	x = x * 2 + 0.693147180559945286226764 * e + x * x2 * t;

	if (xisinf(d)) x = SLEEF_INFINITY;
	if (d < 0 || xisnan(d)) x = SLEEF_NAN;
	if (d == 0) x = -SLEEF_INFINITY;

	return x;
}

NO_SIDE_EFFECTS constexpr f64 xexp(f64 d) {
	i32 q = (i32)rintk(d * R_LN2);
	f64 s, u;

	s = mla(static_cast<f64>(q), -L2U, d);
	s = mla(static_cast<f64>(q), -L2L, s);

	f64 s2 = s * s, s4 = s2 * s2, s8 = s4 * s4;
	u = POLY10(s, s2, s4, s8,
			   2.08860621107283687536341e-09,
			   2.51112930892876518610661e-08,
			   2.75573911234900471893338e-07,
			   2.75572362911928827629423e-06,
			   2.4801587159235472998791e-05,
			   0.000198412698960509205564975,
			   0.00138888888889774492207962,
			   0.00833333333331652721664984,
			   0.0416666666666665047591422,
			   0.166666666666666851703837);
	u = mla(u, s, +0.5);

	u = s * s * u + s + 1;
	u = ldexp2k(u, q);

	if (d > 709.78271114955742909217217426) u = SLEEF_INFINITY;
	if (d < -1000) u = 0;

	return u;
}

NO_SIDE_EFFECTS INLINE constexpr f64 expm1k(f64 d) {
	i32 q = (i32)rintk(d * R_LN2);
	f64 s, u;

	s = mla(static_cast<f64>(q), -L2U, d);
	s = mla(static_cast<f64>(q), -L2L, s);

	f64 s2 = s * s, s4 = s2 * s2, s8 = s4 * s4;
	u = POLY10(s, s2, s4, s8,
			   2.08860621107283687536341e-09,
			   2.51112930892876518610661e-08,
			   2.75573911234900471893338e-07,
			   2.75572362911928827629423e-06,
			   2.4801587159235472998791e-05,
			   0.000198412698960509205564975,
			   0.00138888888889774492207962,
			   0.00833333333331652721664984,
			   0.0416666666666665047591422,
			   0.166666666666666851703837);

	u = mla(s2, 0.5, s2 * s * u) + s;

	if (q != 0) u = ldexp2k(u + 1, q) - 1;

	return u;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 logk(f64 d) {
	Sleef_double2 x, x2, s;
	f64 m, t;
	i32 e;

	i32 o = d < DBL_MIN;
	if (o) d *= (f64)(INT64_C(1) << 32) * (f64)(INT64_C(1) << 32);

	e = ilogb2k(d * (1.0 / 0.75));
	m = ldexp3k(d, -e);

	if (o) e -= 64;

	x = dddiv_d2_d2_d2(ddadd2_d2_d_d(-1, m), ddadd2_d2_d_d(1, m));
	x2 = ddsqu_d2_d2(x);

	f64 x4 = x2.x * x2.x, x8 = x4 * x4, x16 = x8 * x8;
	t = POLY9(x2.x, x4, x8, x16,
			  0.116255524079935043668677,
			  0.103239680901072952701192,
			  0.117754809412463995466069,
			  0.13332981086846273921509,
			  0.153846227114512262845736,
			  0.181818180850050775676507,
			  0.222222222230083560345903,
			  0.285714285714249172087875,
			  0.400000000000000077715612);

	Sleef_double2 c = dd(0.666666666666666629659233, 3.80554962542412056336616e-17);
	s = ddmul_d2_d2_d(dd(0.693147180559945286226764, 2.319046813846299558417771e-17), e);
	s = ddadd_d2_d2_d2(s, ddscale_d2_d2_d(x, 2));
	x = ddmul_d2_d2_d2(x2, x);
	s = ddadd_d2_d2_d2(s, ddmul_d2_d2_d2(x, c));
	x = ddmul_d2_d2_d2(x2, x);
	s = ddadd_d2_d2_d2(s, ddmul_d2_d2_d(x, t));

	return s;
}

NO_SIDE_EFFECTS constexpr f64 xlog_u1(f64 d) {
	Sleef_double2 x, s;
	f64 m, t, x2;
	i32 e;

	i32 o = d < DBL_MIN;
	if (o) d *= (f64)(INT64_C(1) << 32) * (f64)(INT64_C(1) << 32);

	e = ilogb2k(d * (1.0 / 0.75));
	m = ldexp3k(d, -e);

	if (o) e -= 64;

	x = dddiv_d2_d2_d2(ddadd2_d2_d_d(-1, m), ddadd2_d2_d_d(1, m));
	x2 = x.x * x.x;

	f64 x4 = x2 * x2, x8 = x4 * x4;
	t = POLY7(x2, x4, x8,
			  0.1532076988502701353e+0,
			  0.1525629051003428716e+0,
			  0.1818605932937785996e+0,
			  0.2222214519839380009e+0,
			  0.2857142932794299317e+0,
			  0.3999999999635251990e+0,
			  0.6666666666667333541e+0);

	s = ddmul_d2_d2_d(dd(0.693147180559945286226764, 2.319046813846299558417771e-17), (f64)e);
	s = ddadd_d2_d2_d2(s, ddscale_d2_d2_d(x, 2));
	s = ddadd_d2_d2_d(s, x2 * x.x * t);

	f64 r = s.x + s.y;

	if (xisinf(d)) r = SLEEF_INFINITY;
	if (d < 0 || xisnan(d)) r = SLEEF_NAN;
	if (d == 0) r = -SLEEF_INFINITY;

	return r;
}

NO_SIDE_EFFECTS INLINE constexpr f64 expk(Sleef_double2 d) {
	i32 q = (i32)rintk((d.x + d.y) * R_LN2);
	Sleef_double2 s, t;
	f64 u;

	s = ddadd2_d2_d2_d(d, q * -L2U);
	s = ddadd2_d2_d2_d(s, q * -L2L);

	s = ddnormalize_d2_d2(s);

	f64 s2 = s.x * s.x, s4 = s2 * s2, s8 = s4 * s4;
	u = POLY10(s.x, s2, s4, s8,
			   2.51069683420950419527139e-08,
			   2.76286166770270649116855e-07,
			   2.75572496725023574143864e-06,
			   2.48014973989819794114153e-05,
			   0.000198412698809069797676111,
			   0.0013888888939977128960529,
			   0.00833333333332371417601081,
			   0.0416666666665409524128449,
			   0.166666666666666740681535,
			   0.500000000000000999200722);

	t = ddadd_d2_d_d2(1, s);
	t = ddadd_d2_d2_d2(t, ddmul_d2_d2_d(ddsqu_d2_d2(s), u));

	u = ldexpk(t.x + t.y, q);

	if (d.x < -1000) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xpow(f64 x, f64 y) {
	i32 yisint = xisint(y);
	i32 yisodd = yisint && xisodd(y);

	Sleef_double2 d = ddmul_d2_d2_d(logk(fabsk(x)), y);
	f64 result = expk(d);

	result = (d.x > 709.78271114955742909217217426 || xisnan(result)) ? SLEEF_INFINITY : result;
	result *= (x > 0 ? 1 : (yisint ? (yisodd ? -1 : 1) : SLEEF_NAN));

	f64 efx = mulsign(fabsk(x) - 1, y);
	if (xisinf(y)) result = efx < 0 ? 0.0 : (efx == 0 ? 1.0 : SLEEF_INFINITY);
	if (xisinf(x) || x == 0) result = mulsign((Math::signbit(y) ^ (x == 0)) ? 0 : SLEEF_INFINITY, yisodd ? x : 1);
	if (xisnan(x) || xisnan(y)) result = SLEEF_NAN;
	if (y == 0 || x == 1) result = 1;

	return result;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 expk2(Sleef_double2 d) {
	i32 q = (i32)rintk((d.x + d.y) * R_LN2);
	Sleef_double2 s, t;
	f64 u;

	s = ddadd2_d2_d2_d(d, q * -L2U);
	s = ddadd2_d2_d2_d(s, q * -L2L);

	u = +0.1602472219709932072e-9;
	u = mla(u, s.x, +0.2092255183563157007e-8);
	u = mla(u, s.x, +0.2505230023782644465e-7);
	u = mla(u, s.x, +0.2755724800902135303e-6);
	u = mla(u, s.x, +0.2755731892386044373e-5);
	u = mla(u, s.x, +0.2480158735605815065e-4);
	u = mla(u, s.x, +0.1984126984148071858e-3);
	u = mla(u, s.x, +0.1388888888886763255e-2);
	u = mla(u, s.x, +0.8333333333333347095e-2);
	u = mla(u, s.x, +0.4166666666666669905e-1);

	t = ddadd2_d2_d2_d(ddmul_d2_d2_d(s, u), +0.1666666666666666574e+0);
	t = ddadd2_d2_d2_d(ddmul_d2_d2_d2(s, t), 0.5);
	t = ddadd2_d2_d2_d2(s, ddmul_d2_d2_d2(ddsqu_d2_d2(s), t));

	t = ddadd2_d2_d_d2(1, t);

	t.x = ldexp2k(t.x, q);
	t.y = ldexp2k(t.y, q);

	return d.x < -1000 ? dd(0, 0) : t;
}

NO_SIDE_EFFECTS constexpr f64 xsinh(f64 x) {
	f64 y = fabsk(x);
	Sleef_double2 d = expk2(dd(y, 0));
	d = ddsub_d2_d2_d2(d, ddrec_d2_d2(d));
	y = (d.x + d.y) * 0.5;

	y = fabsk(x) > 710 ? SLEEF_INFINITY : y;
	y = xisnan(y) ? SLEEF_INFINITY : y;
	y = mulsign(y, x);
	y = xisnan(x) ? SLEEF_NAN : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f64 xcosh(f64 x) {
	f64 y = fabsk(x);
	Sleef_double2 d = expk2(dd(y, 0));
	d = ddadd_d2_d2_d2(d, ddrec_d2_d2(d));
	y = (d.x + d.y) * 0.5;

	y = fabsk(x) > 710 ? SLEEF_INFINITY : y;
	y = xisnan(y) ? SLEEF_INFINITY : y;
	y = xisnan(x) ? SLEEF_NAN : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f64 xtanh(f64 x) {
	f64 y = fabsk(x);
	Sleef_double2 d = expk2(dd(y, 0));
	Sleef_double2 e = ddrec_d2_d2(d);
	d = dddiv_d2_d2_d2(ddsub_d2_d2_d2(d, e), ddadd_d2_d2_d2(d, e));
	y = d.x + d.y;

	y = fabsk(x) > 18.714973875 ? 1.0 : y;
	y = xisnan(y) ? 1.0 : y;
	y = mulsign(y, x);
	y = xisnan(x) ? SLEEF_NAN : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f64 xsinh_u35(f64 x) {
	f64 e = expm1k(fabsk(x));
	f64 y = (e + 2) / (e + 1) * (0.5 * e);

	y = fabsk(x) > 709 ? SLEEF_INFINITY : y;
	y = xisnan(y) ? SLEEF_INFINITY : y;
	y = mulsign(y, x);
	y = xisnan(x) ? SLEEF_NAN : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f64 xcosh_u35(f64 x) {
	f64 e = xexp(fabsk(x));
	f64 y = 0.5 / e + 0.5 * e;

	y = fabsk(x) > 709 ? SLEEF_INFINITY : y;
	y = xisnan(y) ? SLEEF_INFINITY : y;
	y = xisnan(x) ? SLEEF_NAN : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f64 xtanh_u35(f64 x) {
	f64 y = fabsk(x);
	f64 d = expm1k(2 * y);
	y = d / (d + 2);

	y = fabsk(x) > 18.714973875 ? 1.0 : y;
	y = xisnan(y) ? 1.0 : y;
	y = mulsign(y, x);
	y = xisnan(x) ? SLEEF_NAN : y;

	return y;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 logk2(Sleef_double2 d) {
	Sleef_double2 x, x2, m, s;
	f64 t;
	i32 e;

	e = ilogbk(d.x * (1.0 / 0.75));

	m.x = ldexp2k(d.x, -e);
	m.y = ldexp2k(d.y, -e);

	x = dddiv_d2_d2_d2(ddadd2_d2_d2_d(m, -1), ddadd2_d2_d2_d(m, 1));
	x2 = ddsqu_d2_d2(x);

	f64 x4 = x2.x * x2.x, x8 = x4 * x4;
	t = POLY7(x2.x, x4, x8,
			  0.13860436390467167910856,
			  0.131699838841615374240845,
			  0.153914168346271945653214,
			  0.181816523941564611721589,
			  0.22222224632662035403996,
			  0.285714285511134091777308,
			  0.400000000000914013309483);
	t = mla(t, x2.x, 0.666666666666664853302393);

	s = ddmul_d2_d2_d(dd(0.693147180559945286226764, 2.319046813846299558417771e-17), e);
	s = ddadd_d2_d2_d2(s, ddscale_d2_d2_d(x, 2));
	s = ddadd_d2_d2_d2(s, ddmul_d2_d2_d(ddmul_d2_d2_d2(x2, x), t));

	return s;
}

NO_SIDE_EFFECTS constexpr f64 xasinh(f64 x) {
	f64 y = fabsk(x);
	Sleef_double2 d;

	d = y > 1 ? ddrec_d2_d(x) : dd(y, 0);
	d = ddsqrt_d2_d2(ddadd2_d2_d2_d(ddsqu_d2_d2(d), 1));
	d = y > 1 ? ddmul_d2_d2_d(d, y) : d;

	d = logk2(ddnormalize_d2_d2(ddadd_d2_d2_d(d, x)));
	y = d.x + d.y;

	y = (fabsk(x) > SQRT_DBL_MAX || xisnan(y)) ? mulsign(SLEEF_INFINITY, x) : y;
	y = xisnan(x) ? SLEEF_NAN : y;
	y = xisnegzero(x) ? -0.0 : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f64 xacosh(f64 x) {
	Sleef_double2 d = logk2(ddadd2_d2_d2_d(ddmul_d2_d2_d2(ddsqrt_d2_d2(ddadd2_d2_d_d(x, 1)), ddsqrt_d2_d2(ddadd2_d2_d_d(x, -1))), x));
	f64 y = d.x + d.y;

	y = (x > SQRT_DBL_MAX || xisnan(y)) ? SLEEF_INFINITY : y;
	y = x == 1.0 ? 0.0 : y;
	y = x < 1.0 ? SLEEF_NAN : y;
	y = xisnan(x) ? SLEEF_NAN : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f64 xatanh(f64 x) {
	f64 y = fabsk(x);
	Sleef_double2 d = logk2(dddiv_d2_d2_d2(ddadd2_d2_d_d(1, y), ddadd2_d2_d_d(1, -y)));
	y = y > 1.0 ? SLEEF_NAN : (y == 1.0 ? SLEEF_INFINITY : (d.x + d.y) * 0.5);

	y = mulsign(y, x);
	y = (xisinf(x) || xisnan(y)) ? SLEEF_NAN : y;

	return y;
}

NO_SIDE_EFFECTS constexpr f64 xcbrt(f64 d) { // max error : 2 ulps
	f64 x, y, q = 1.0;
	i32 e, r;

	e = ilogbk(fabsk(d)) + 1;
	d = ldexp2k(d, -e);
	r = (e + 6144) % 3;
	q = (r == 1) ? 1.2599210498948731647672106 : q;
	q = (r == 2) ? 1.5874010519681994747517056 : q;
	q = ldexp2k(q, (e + 6144) / 3 - 2048);

	q = mulsign(q, d);
	d = fabsk(d);

	x = -0.640245898480692909870982;
	x = mla(x, d, 2.96155103020039511818595);
	x = mla(x, d, -5.73353060922947843636166);
	x = mla(x, d, 6.03990368989458747961407);
	x = mla(x, d, -3.85841935510444988821632);
	x = mla(x, d, 2.2307275302496609725722);

	y = x * x; y = y * y; x -= (d * y - x) * (1.0 / 3.0);
	y = d * x * x;
	y = (y - (2.0 / 3.0) * y * (y * x - 1)) * q;

	return y;
}

NO_SIDE_EFFECTS constexpr f64 xcbrt_u1(f64 d) {
	f64 x, y, z;
	Sleef_double2 q2 = dd(1, 0), u, v;
	i32 e, r;

	e = ilogbk(fabsk(d)) + 1;
	d = ldexp2k(d, -e);
	r = (e + 6144) % 3;
	q2 = (r == 1) ? dd(1.2599210498948731907, -2.5899333753005069177e-17) : q2;
	q2 = (r == 2) ? dd(1.5874010519681995834, -1.0869008194197822986e-16) : q2;

	q2.x = mulsign(q2.x, d); q2.y = mulsign(q2.y, d);
	d = fabsk(d);

	x = -0.640245898480692909870982;
	x = mla(x, d, 2.96155103020039511818595);
	x = mla(x, d, -5.73353060922947843636166);
	x = mla(x, d, 6.03990368989458747961407);
	x = mla(x, d, -3.85841935510444988821632);
	x = mla(x, d, 2.2307275302496609725722);

	y = x * x; y = y * y; x -= (d * y - x) * (1.0 / 3.0);

	z = x;

	u = ddmul_d2_d_d(x, x);
	u = ddmul_d2_d2_d2(u, u);
	u = ddmul_d2_d2_d(u, d);
	u = ddadd2_d2_d2_d(u, -x);
	y = u.x + u.y;

	y = -2.0 / 3.0 * y * z;
	v = ddadd2_d2_d2_d(ddmul_d2_d_d(z, z), y);
	v = ddmul_d2_d2_d(v, d);
	v = ddmul_d2_d2_d2(v, q2);
	z = ldexp2k(v.x + v.y, (e + 6144) / 3 - 2048);

	if (xisinf(d)) { z = mulsign(SLEEF_INFINITY, q2.x); }
	if (d == 0) { z = mulsign(0, q2.x); }

	return z;
}

NO_SIDE_EFFECTS constexpr f64 xexp2(f64 d) {
	i32 q = (i32)rintk(d);
	f64 s, u;

	s = d - q;

	f64 s2 = s * s, s4 = s2 * s2, s8 = s4 * s4;
	u = POLY10(s, s2, s4, s8,
			   +0.4434359082926529454e-9,
			   +0.7073164598085707425e-8,
			   +0.1017819260921760451e-6,
			   +0.1321543872511327615e-5,
			   +0.1525273353517584730e-4,
			   +0.1540353045101147808e-3,
			   +0.1333355814670499073e-2,
			   +0.9618129107597600536e-2,
			   +0.5550410866482046596e-1,
			   +0.2402265069591012214e+0);
	u = mla(u, s, +0.6931471805599452862e+0);

	u = ddnormalize_d2_d2(ddadd_d2_d_d2(1, ddmul_d2_d_d(u, s))).x;

	u = ldexp2k(u, q);

	if (d >= 1024) u = SLEEF_INFINITY;
	if (d < -2000) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xexp2_u35(f64 d) {
	i32 q = (i32)rintk(d);
	f64 s, u;

	s = d - q;

	u = +0.4434359082926529454e-9;
	u = mla(u, s, +0.7073164598085707425e-8);
	u = mla(u, s, +0.1017819260921760451e-6);
	u = mla(u, s, +0.1321543872511327615e-5);
	u = mla(u, s, +0.1525273353517584730e-4);
	u = mla(u, s, +0.1540353045101147808e-3);
	u = mla(u, s, +0.1333355814670499073e-2);
	u = mla(u, s, +0.9618129107597600536e-2);
	u = mla(u, s, +0.5550410866482046596e-1);
	u = mla(u, s, +0.2402265069591012214e+0);
	u = mla(u, s, +0.6931471805599452862e+0);
	u = mla(u, s, +0.1000000000000000000e+1);

	u = ldexp2k(u, q);

	if (d >= 1024) u = SLEEF_INFINITY;
	if (d < -2000) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xexp10(f64 d) {
	i32 q = (i32)rintk(d * LOG10_2);
	f64 s, u;

	s = mla(static_cast<f64>(q), -L10U, d);
	s = mla(static_cast<f64>(q), -L10L, s);

	u = +0.2411463498334267652e-3;
	u = mla(u, s, +0.1157488415217187375e-2);
	u = mla(u, s, +0.5013975546789733659e-2);
	u = mla(u, s, +0.1959762320720533080e-1);
	u = mla(u, s, +0.6808936399446784138e-1);
	u = mla(u, s, +0.2069958494722676234e+0);
	u = mla(u, s, +0.5393829292058536229e+0);
	u = mla(u, s, +0.1171255148908541655e+1);
	u = mla(u, s, +0.2034678592293432953e+1);
	u = mla(u, s, +0.2650949055239205876e+1);
	u = mla(u, s, +0.2302585092994045901e+1);

	u = ddnormalize_d2_d2(ddadd_d2_d_d2(1, ddmul_d2_d_d(u, s))).x;

	u = ldexp2k(u, q);

	if (d > 308.25471555991671) u = SLEEF_INFINITY; // log10(DBL_MAX)
	if (d < -350) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xexp10_u35(f64 d) {
	i32 q = (i32)rintk(d * LOG10_2);
	f64 s, u;

	s = mla(static_cast<f64>(q), -L10U, d);
	s = mla(static_cast<f64>(q), -L10L, s);

	u = +0.2411463498334267652e-3;
	u = mla(u, s, +0.1157488415217187375e-2);
	u = mla(u, s, +0.5013975546789733659e-2);
	u = mla(u, s, +0.1959762320720533080e-1);
	u = mla(u, s, +0.6808936399446784138e-1);
	u = mla(u, s, +0.2069958494722676234e+0);
	u = mla(u, s, +0.5393829292058536229e+0);
	u = mla(u, s, +0.1171255148908541655e+1);
	u = mla(u, s, +0.2034678592293432953e+1);
	u = mla(u, s, +0.2650949055239205876e+1);
	u = mla(u, s, +0.2302585092994045901e+1);
	u = mla(u, s, +0.1000000000000000000e+1);

	u = ldexp2k(u, q);

	if (d > 308.25471555991671) u = SLEEF_INFINITY;
	if (d < -350) u = 0;

	return u;
}

NO_SIDE_EFFECTS constexpr f64 xexpm1(f64 a) {
	Sleef_double2 d = ddadd2_d2_d2_d(expk2(dd(a, 0)), -1.0);
	f64 x = d.x + d.y;
	if (a > 709.782712893383996732223) x = SLEEF_INFINITY; // log(DBL_MAX)
	if (a < -36.736800569677101399113302437) x = -1; // log(1 - nexttoward(1, 0))
	if (xisnegzero(a)) x = -0.0;
	return x;
}

NO_SIDE_EFFECTS constexpr f64 xlog10(f64 d) {
	Sleef_double2 x, s;
	f64 m, t, x2;
	i32 e;

	i32 o = d < DBL_MIN;
	if (o) d *= (f64)(INT64_C(1) << 32) * (f64)(INT64_C(1) << 32);

	e = ilogb2k(d * (1.0 / 0.75));
	m = ldexp3k(d, -e);

	if (o) e -= 64;

	x = dddiv_d2_d2_d2(ddadd2_d2_d_d(-1, m), ddadd2_d2_d_d(1, m));
	x2 = x.x * x.x;

	f64 x4 = x2 * x2, x8 = x4 * x4;
	t = POLY7(x2, x4, x8,
			  +0.6653725819576758460e-1,
			  +0.6625722782820833712e-1,
			  +0.7898105214313944078e-1,
			  +0.9650955035715275132e-1,
			  +0.1240841409721444993e+0,
			  +0.1737177927454605086e+0,
			  +0.2895296546021972617e+0);

	s = ddmul_d2_d2_d(dd(0.30102999566398119802, -2.803728127785170339e-18), (f64)e);
	s = ddadd_d2_d2_d2(s, ddmul_d2_d2_d2(x, dd(0.86858896380650363334, 1.1430059694096389311e-17)));
	s = ddadd_d2_d2_d(s, x2 * x.x * t);

	f64 r = s.x + s.y;

	if (xisinf(d)) r = SLEEF_INFINITY;
	if (d < 0 || xisnan(d)) r = SLEEF_NAN;
	if (d == 0) r = -SLEEF_INFINITY;

	return r;
}

NO_SIDE_EFFECTS constexpr f64 xlog2(f64 d) {
	Sleef_double2 x, s;
	f64 m, t, x2;
	i32 e;

	i32 o = d < DBL_MIN;
	if (o) d *= (f64)(INT64_C(1) << 32) * (f64)(INT64_C(1) << 32);

	e = ilogb2k(d * (1.0 / 0.75));
	m = ldexp3k(d, -e);

	if (o) e -= 64;

	x = dddiv_d2_d2_d2(ddadd2_d2_d_d(-1, m), ddadd2_d2_d_d(1, m));
	x2 = x.x * x.x;

	f64 x4 = x2 * x2, x8 = x4 * x4;
	t = POLY7(x2, x4, x8,
			  +0.2211941750456081490e+0,
			  +0.2200768693152277689e+0,
			  +0.2623708057488514656e+0,
			  +0.3205977477944495502e+0,
			  +0.4121985945485324709e+0,
			  +0.5770780162997058982e+0,
			  +0.96179669392608091449);

	s = ddadd2_d2_d_d2(e, ddmul_d2_d2_d2(x, dd(2.885390081777926774, 6.0561604995516736434e-18)));
	s = ddadd2_d2_d2_d(s, x2 * x.x * t);

	f64 r = s.x + s.y;

	if (xisinf(d)) r = SLEEF_INFINITY;
	if (d < 0 || xisnan(d)) r = SLEEF_NAN;
	if (d == 0) r = -SLEEF_INFINITY;

	return r;
}

NO_SIDE_EFFECTS constexpr f64 xlog2_u35(f64 d) {
	f64 m, t, x, x2;
	i32 e;

	i32 o = d < DBL_MIN;
	if (o) d *= (f64)(INT64_C(1) << 32) * (f64)(INT64_C(1) << 32);

	e = ilogb2k(d * (1.0 / 0.75));
	m = ldexp3k(d, -e);

	if (o) e -= 64;

	x = (m - 1) / (m + 1);
	x2 = x * x;

	t = +0.2211941750456081490e+0;
	t = mla(t, x2, +0.2200768693152277689e+0);
	t = mla(t, x2, +0.2623708057488514656e+0);
	t = mla(t, x2, +0.3205977477944495502e+0);
	t = mla(t, x2, +0.4121985945485324709e+0);
	t = mla(t, x2, +0.5770780162997058982e+0);
	t = mla(t, x2, +0.96179669392608091449);

	Sleef_double2 s = ddadd_d2_d_d2(e, ddmul_d2_d_d(2.885390081777926774, x));
	f64 r = mla(t, x * x2, s.x + s.y);

	if (xisinf(d)) r = SLEEF_INFINITY;
	if (d < 0 || xisnan(d)) r = SLEEF_NAN;
	if (d == 0) r = -SLEEF_INFINITY;

	return r;
}

NO_SIDE_EFFECTS constexpr f64 xlog1p(f64 d) {
	Sleef_double2 x, s;
	f64 m, t, x2;
	i32 e;

	f64 dp1 = d + 1;

	i32 o = dp1 < DBL_MIN;
	if (o) dp1 *= (f64)(INT64_C(1) << 32) * (f64)(INT64_C(1) << 32);

	e = ilogb2k(dp1 * (1.0 / 0.75));

	t = ldexp3k(1, -e);
	m = mla(d, t, t - 1);

	if (o) e -= 64;

	x = dddiv_d2_d2_d2(dd(m, 0), ddadd_d2_d_d(2, m));
	x2 = x.x * x.x;

	f64 x4 = x2 * x2, x8 = x4 * x4;
	t = POLY7(x2, x4, x8,
			  0.1532076988502701353e+0,
			  0.1525629051003428716e+0,
			  0.1818605932937785996e+0,
			  0.2222214519839380009e+0,
			  0.2857142932794299317e+0,
			  0.3999999999635251990e+0,
			  0.6666666666667333541e+0);

	s = ddmul_d2_d2_d(dd(0.693147180559945286226764, 2.319046813846299558417771e-17), (f64)e);
	s = ddadd_d2_d2_d2(s, ddscale_d2_d2_d(x, 2));
	s = ddadd_d2_d2_d(s, x2 * x.x * t);

	f64 r = s.x + s.y;

	if (d > 1e+307) r = SLEEF_INFINITY;
	if (d < -1 || xisnan(d)) r = SLEEF_NAN;
	if (d == -1) r = -SLEEF_INFINITY;
	if (xisnegzero(d)) r = -0.0;

	return r;
}

NO_SIDE_EFFECTS constexpr f64 xfma(f64 x, f64 y, f64 z) {
	f64 h2 = x * y + z, q = 1;
	if (fabsk(h2) < 1e-300) {
		const f64 c0 = UINT64_C(1) << 54, c1 = c0 * c0, c2 = c1 * c1;
		x *= c1;
		y *= c1;
		z *= c2;
		q = 1.0 / c2;
	}
	if (fabsk(h2) > 1e+299) {
		const f64 c0 = UINT64_C(1) << 54, c1 = c0 * c0, c2 = c1 * c1;
		x *= 1.0 / c1;
		y *= 1.0 / c1;
		z *= 1. / c2;
		q = c2;
	}
	Sleef_double2 d = ddmul_d2_d_d(x, y);
	d = ddadd2_d2_d2_d(d, z);
	f64 ret = (x == 0 || y == 0) ? z : (d.x + d.y);
	if ((xisinf(z) && !xisinf(x) && !xisnan(x) && !xisinf(y) && !xisnan(y))) h2 = z;
	return (xisinf(h2) || xisnan(h2)) ? h2 : ret * q;
}

NO_SIDE_EFFECTS constexpr f64 xsqrt_u05(f64 d) {
	f64 q = 0.5;

	d = d < 0 ? SLEEF_NAN : d;

	if (d < 8.636168555094445E-78) {
		d *= 1.157920892373162E77;
		q = 2.9387358770557188E-39 * 0.5;
	}

	if (d > 1.3407807929942597e+154) {
		d *= 7.4583407312002070e-155;
		q = 1.1579208923731620e+77 * 0.5;
	}

	// http://en.wikipedia.org/wiki/Fast_inverse_square_root
	f64 x = __builtin_bit_cast(f64,0x5fe6ec85e7de30da - (__builtin_bit_cast(i64 ,d + 1e-320) >> 1));

	x = x * (1.5 - 0.5 * d * x * x);
	x = x * (1.5 - 0.5 * d * x * x);
	x = x * (1.5 - 0.5 * d * x * x) * d;

	Sleef_double2 d2 = ddmul_d2_d2_d2(ddadd2_d2_d_d2(d, ddmul_d2_d_d(x, x)), ddrec_d2_d(x));

	f64 ret = (d2.x + d2.y) * q;

	ret = d == SLEEF_INFINITY ? SLEEF_INFINITY : ret;
	ret = d == 0 ? d : ret;

	return ret;
}

NO_SIDE_EFFECTS constexpr f64 xsqrt_u35(f64 d) { return xsqrt_u05(d); }
NO_SIDE_EFFECTS constexpr f64 xsqrt(f64 d) { return Math::sqrt(d); }

NO_SIDE_EFFECTS constexpr f64 xfabs(f64 x) { return fabsk(x); }

NO_SIDE_EFFECTS constexpr f64 xcopysign(f64 x, f64 y) { return copysignk(x, y); }

NO_SIDE_EFFECTS constexpr f64 xfmax(f64 x, f64 y) {
	return y != y ? x : (x > y ? x : y);
}

NO_SIDE_EFFECTS constexpr f64 xfmin(f64 x, f64 y) {
	return y != y ? x : (x < y ? x : y);
}

NO_SIDE_EFFECTS constexpr f64 xfdim(f64 x, f64 y) {
	f64 ret = x - y;
	if (ret < 0 || x == y) ret = 0;
	return ret;
}

NO_SIDE_EFFECTS constexpr f64 xtrunc(f64 x) {
	f64 fr = x - (f64)(INT64_C(1) << 31) * (i32)(x * (1.0 / (INT64_C(1) << 31)));
	fr = fr - (i32)fr;
	return (xisinf(x) || fabsk(x) >= (f64)(INT64_C(1) << 52)) ? x : copysignk(x - fr, x);
}

NO_SIDE_EFFECTS constexpr f64 xfloor(f64 x) {
	f64 fr = x - (f64)(INT64_C(1) << 31) * (i32)(x * (1.0 / (INT64_C(1) << 31)));
	fr = fr - (i32)fr;
	fr = fr < 0 ? fr + 1.0 : fr;
	return (xisinf(x) || fabsk(x) >= (f64)(INT64_C(1) << 52)) ? x : copysignk(x - fr, x);
}

NO_SIDE_EFFECTS constexpr f64 xceil(f64 x) {
	f64 fr = x - (f64)(INT64_C(1) << 31) * (i32)(x * (1.0 / (INT64_C(1) << 31)));
	fr = fr - (i32)fr;
	fr = fr <= 0 ? fr : fr - 1.0;
	return (xisinf(x) || fabsk(x) >= (f64)(INT64_C(1) << 52)) ? x : copysignk(x - fr, x);
}

NO_SIDE_EFFECTS constexpr f64 xround(f64 d) {
	f64 x = d + 0.5;
	f64 fr = x - (f64)(INT64_C(1) << 31) * (i32)(x * (1.0 / (INT64_C(1) << 31)));
	fr = fr - (i32)fr;
	if (fr == 0 && x <= 0) x--;
	fr = fr < 0 ? fr + 1.0 : fr;
	x = d == 0.49999999999999994449 ? 0 : x; // nextafter(0.5, 0)
	return (xisinf(d) || fabsk(d) >= (f64)(INT64_C(1) << 52)) ? d : copysignk(x - fr, d);
}

NO_SIDE_EFFECTS constexpr f64 xrint(f64 d) {
	f64 c = mulsign(INT64_C(1) << 52, d);
	return fabsk(d) > INT64_C(1) << 52 ? d : orsign(d + c - c, d);
}

NO_SIDE_EFFECTS constexpr f64 xhypot_u05(f64 x, f64 y) {
	x = fabsk(x);
	y = fabsk(y);
	f64 min = fmink(x, y), n = min;
	f64 max = fmaxk(x, y), d = max;

	if (max < DBL_MIN) { n *= UINT64_C(1) << 54; d *= UINT64_C(1) << 54; }
	Sleef_double2 t = dddiv_d2_d2_d2(dd(n, 0), dd(d, 0));
	t = ddmul_d2_d2_d(ddsqrt_d2_d2(ddadd2_d2_d2_d(ddsqu_d2_d2(t), 1)), max);
	f64 ret = t.x + t.y;
	if (xisnan(ret)) ret = SLEEF_INFINITY;
	if (min == 0) ret = max;
	if (xisnan(x) || xisnan(y)) ret = SLEEF_NAN;
	if (x == SLEEF_INFINITY || y == SLEEF_INFINITY) ret = SLEEF_INFINITY;
	return ret;
}

NO_SIDE_EFFECTS constexpr f64 xhypot_u35(f64 x, f64 y) {
	x = fabsk(x);
	y = fabsk(y);
	f64 min = fmink(x, y);
	f64 max = fmaxk(x, y);

	f64 t = min / max;
	f64 ret = max * Math::sqrt(1 + t * t);
	if (min == 0) ret = max;
	if (xisnan(x) || xisnan(y)) ret = SLEEF_NAN;
	if (x == SLEEF_INFINITY || y == SLEEF_INFINITY) ret = SLEEF_INFINITY;
	return ret;
}

NO_SIDE_EFFECTS constexpr f64 xnextafter(f64 x, f64 y) {

	x = x == 0 ? mulsign(0, y) : x;
	f64 cxf = x;
	i64 cxi = __builtin_bit_cast(i64 ,cxf);

	i32 c = (cxi < 0) == (y < x);
	if (c) cxi = -(cxi ^ (i64)(UINT64_C(1) << 63));

	if (x != y) cxi--;

	if (c) cxi = -(cxi ^ (i64)(UINT64_C(1) << 63));

	//memcpy(&cxf, &cxi, sizeof(cxf));
	cxf = __builtin_bit_cast(f64,cxi);
	if (cxf == 0 && x != 0) cxf = mulsign(0, x);
	if (x == 0 && y == 0) cxf = y;
	if (xisnan(x) || xisnan(y)) cxf = SLEEF_NAN;

	return cxf;
}

NO_SIDE_EFFECTS constexpr f64 xfrfrexp(f64 x) {

	if (fabsk(x) < DBL_MIN) x *= (UINT64_C(1) << 63);

	f64 cxf = x;
	u64 cxu = BITCAST(u64,cxf);
	//memcpy(&cxu, &cxf, sizeof(cxu));

	cxu &= ~UINT64_C(0x7ff0000000000000);
	cxu |= UINT64_C(0x3fe0000000000000);

	//memcpy(&cxf, &cxu, sizeof(cxf));
	cxf = __builtin_bit_cast(f64,cxu);
	if (xisinf(x)) cxf = mulsign(SLEEF_INFINITY, x);
	if (x == 0) cxf = x;

	return cxf;
}

NO_SIDE_EFFECTS constexpr i32 xexpfrexp(f64 x) {
	i32 ret = 0;

	if (fabsk(x) < DBL_MIN) { x *= (UINT64_C(1) << 63); ret = -63; }

	f64 cxf = x;
	u64 cxu = BITCAST(u64,cxf);

	ret += (i32)(((cxu >> 52) & 0x7ff)) - 0x3fe;

	if (x == 0 || xisnan(x) || xisinf(x)) ret = 0;

	return ret;
}

NO_SIDE_EFFECTS INLINE constexpr f64 toward0(f64 d) {
	return d == 0 ? 0 : __builtin_bit_cast(f64,__builtin_bit_cast(i64 ,d) - 1);
}

NO_SIDE_EFFECTS INLINE constexpr f64 removelsb(f64 d) {
	return __builtin_bit_cast(f64,__builtin_bit_cast(i64 ,d) & INT64_C(0xfffffffffffffffe));
}

NO_SIDE_EFFECTS INLINE constexpr f64 ptrunc(f64 x) {
	f64 fr = mla(-(f64)(INT64_C(1) << 31), (i32)(x * (1.0 / (INT64_C(1) << 31))), x);
	return fabsk(x) >= (f64)(INT64_C(1) << 52) ? x : (x - (fr - (i32)fr));
}

NO_SIDE_EFFECTS constexpr f64 xfmod(f64 x, f64 y) {
	f64 n = fabsk(x), d = fabsk(y), s = 1, q;
	if (d < DBL_MIN) { n *= UINT64_C(1) << 54; d *= UINT64_C(1) << 54; s = 1.0 / (UINT64_C(1) << 54); }
	Sleef_double2 r = dd(n, 0);
	f64 rd = toward0(1.0 / d);

	for (i32 i = 0; i < 21; i++) { // ceil(log2(DBL_MAX) / 52)
		q = removelsb(ptrunc(toward0(r.x) * rd));
		q = (3 * d > r.x && r.x > d) ? 2 : q;
		q = (2 * d > r.x && r.x > d) ? 1 : q;
		q = r.x == d ? (r.y >= 0 ? 1 : 0) : q;
		r = ddnormalize_d2_d2(ddadd2_d2_d2_d2(r, ddmul_d2_d_d(q, -d)));
		if (r.x < d) break;
	}

	f64 ret = r.x * s;
	if (r.x + r.y == d) ret = 0;
	ret = mulsign(ret, x);
	if (n < d) ret = x;
	if (d == 0) ret = SLEEF_NAN;

	return ret;
}

NO_SIDE_EFFECTS INLINE constexpr f64 rintk2(f64 d) {
	f64 c = mulsign(INT64_C(1) << 52, d);
	return fabsk(d) > INT64_C(1) << 52 ? d : orsign(d + c - c, d);
}

NO_SIDE_EFFECTS constexpr f64 xremainder(f64 x, f64 y) {
	f64 n = fabsk(x), d = fabsk(y), s = 1, q;
	if (d < DBL_MIN * 2) { n *= UINT64_C(1) << 54; d *= UINT64_C(1) << 54; s = 1.0 / (UINT64_C(1) << 54); }
	f64 rd = 1.0 / d;
	Sleef_double2 r = dd(n, 0);
	i32 qisodd = 0;

	for (i32 i = 0; i < 21; i++) { // ceil(log2(DBL_MAX) / 52)
		q = removelsb(rintk2(r.x * rd));
		if (fabsk(r.x) < 1.5 * d) q = r.x < 0 ? -1 : 1;
		if (fabsk(r.x) < 0.5 * d || (fabsk(r.x) == 0.5 * d && !qisodd)) q = 0;
		if (q == 0) break;
		if (xisinf(q * -d)) q = q + mulsign(-1, r.x);
		qisodd ^= xisodd(q);
		r = ddnormalize_d2_d2(ddadd2_d2_d2_d2(r, ddmul_d2_d_d(q, -d)));
	}

	f64 ret = r.x * s;
	ret = mulsign(ret, x);
	if (xisinf(y)) ret = xisinf(x) ? SLEEF_NAN : x;
	if (d == 0) ret = SLEEF_NAN;

	return ret;
}

NO_SIDE_EFFECTS constexpr Sleef_double2 xmodf(f64 x) {
	f64 fr = x - (f64)(INT64_C(1) << 31) * (i32)(x * (1.0 / (INT64_C(1) << 31)));
	fr = fr - (i32)fr;
	fr = fabsk(x) >= (f64)(INT64_C(1) << 52) ? 0 : fr;
	Sleef_double2 ret = { copysignk(fr, x), copysignk(x - fr, x) };
	return ret;
}

typedef struct {
	Sleef_double2 a, b;
} dd2;

NO_SIDE_EFFECTS constexpr dd2 gammak(f64 a) {
	Sleef_double2 clc = dd(0, 0), clln = dd(1, 0), clld = dd(1, 0), x, y, z;
	f64 t, u;

	i32 otiny = fabsk(a) < 1e-306, oref = a < 0.5;

	x = otiny ? dd(0, 0) : (oref ? ddadd2_d2_d_d(1, -a) : dd(a, 0));

	i32 o0 = (0.5 <= x.x && x.x <= 1.1), o2 = 2.3 < x.x;

	y = ddnormalize_d2_d2(ddmul_d2_d2_d2(ddadd2_d2_d2_d(x, 1), x));
	y = ddnormalize_d2_d2(ddmul_d2_d2_d2(ddadd2_d2_d2_d(x, 2), y));
	y = ddnormalize_d2_d2(ddmul_d2_d2_d2(ddadd2_d2_d2_d(x, 3), y));
	y = ddnormalize_d2_d2(ddmul_d2_d2_d2(ddadd2_d2_d2_d(x, 4), y));

	clln = (o2 && x.x <= 7) ? y : clln;

	x = (o2 && x.x <= 7) ? ddadd2_d2_d2_d(x, 5) : x;
	t = o2 ? (1.0 / x.x) : ddnormalize_d2_d2(ddadd2_d2_d2_d(x, o0 ? -1 : -2)).x;

	u = o2 ? -156.801412704022726379848862 : (o0 ? +0.2947916772827614196e+2 : +0.7074816000864609279e-7);
	u = mla(u, t, o2 ? +1.120804464289911606838558160000 : (o0 ? +0.1281459691827820109e+3 : +0.4009244333008730443e-6));
	u = mla(u, t, o2 ? +13.39798545514258921833306020000 : (o0 ? +0.2617544025784515043e+3 : +0.1040114641628246946e-5));
	u = mla(u, t, o2 ? -0.116546276599463200848033357000 : (o0 ? +0.3287022855685790432e+3 : +0.1508349150733329167e-5));
	u = mla(u, t, o2 ? -1.391801093265337481495562410000 : (o0 ? +0.2818145867730348186e+3 : +0.1288143074933901020e-5));
	u = mla(u, t, o2 ? +0.015056113040026424412918973400 : (o0 ? +0.1728670414673559605e+3 : +0.4744167749884993937e-6));
	u = mla(u, t, o2 ? +0.179540117061234856098844714000 : (o0 ? +0.7748735764030416817e+2 : -0.6554816306542489902e-7));
	u = mla(u, t, o2 ? -0.002481743600264997730942489280 : (o0 ? +0.2512856643080930752e+2 : -0.3189252471452599844e-6));
	u = mla(u, t, o2 ? -0.029527880945699120504851034100 : (o0 ? +0.5766792106140076868e+1 : +0.1358883821470355377e-6));
	u = mla(u, t, o2 ? +0.000540164767892604515196325186 : (o0 ? +0.7270275473996180571e+0 : -0.4343931277157336040e-6));
	u = mla(u, t, o2 ? +0.006403362833808069794787256200 : (o0 ? +0.8396709124579147809e-1 : +0.9724785897406779555e-6));
	u = mla(u, t, o2 ? -0.000162516262783915816896611252 : (o0 ? -0.8211558669746804595e-1 : -0.2036886057225966011e-5));
	u = mla(u, t, o2 ? -0.001914438498565477526465972390 : (o0 ? +0.6828831828341884458e-1 : +0.4373363141819725815e-5));
	u = mla(u, t, o2 ? +7.20489541602001055898311517e-05 : (o0 ? -0.7712481339961671511e-1 : -0.9439951268304008677e-5));
	u = mla(u, t, o2 ? +0.000839498720672087279971000786 : (o0 ? +0.8337492023017314957e-1 : +0.2050727030376389804e-4));
	u = mla(u, t, o2 ? -5.17179090826059219329394422e-05 : (o0 ? -0.9094964931456242518e-1 : -0.4492620183431184018e-4));
	u = mla(u, t, o2 ? -0.000592166437353693882857342347 : (o0 ? +0.1000996313575929358e+0 : +0.9945751236071875931e-4));
	u = mla(u, t, o2 ? +6.97281375836585777403743539e-05 : (o0 ? -0.1113342861544207724e+0 : -0.2231547599034983196e-3));
	u = mla(u, t, o2 ? +0.000784039221720066627493314301 : (o0 ? +0.1255096673213020875e+0 : +0.5096695247101967622e-3));
	u = mla(u, t, o2 ? -0.000229472093621399176949318732 : (o0 ? -0.1440498967843054368e+0 : -0.1192753911667886971e-2));
	u = mla(u, t, o2 ? -0.002681327160493827160473958490 : (o0 ? +0.1695571770041949811e+0 : +0.2890510330742210310e-2));
	u = mla(u, t, o2 ? +0.003472222222222222222175164840 : (o0 ? -0.2073855510284092762e+0 : -0.7385551028674461858e-2));
	u = mla(u, t, o2 ? +0.083333333333333333335592087900 : (o0 ? +0.2705808084277815939e+0 : +0.2058080842778455335e-1));

	y = ddmul_d2_d2_d2(ddadd2_d2_d2_d(x, -0.5), logk2(x));
	y = ddadd2_d2_d2_d2(y, ddneg_d2_d2(x));
	y = ddadd2_d2_d2_d2(y, dd(0.91893853320467278056, -3.8782941580672414498e-17)); // 0.5*log(2*Math::pi<f64>)

	z = ddadd2_d2_d2_d(ddmul_d2_d_d(u, t), o0 ? -0.4006856343865314862e+0 : -0.6735230105319810201e-1);
	z = ddadd2_d2_d2_d(ddmul_d2_d2_d(z, t), o0 ? +0.8224670334241132030e+0 : +0.3224670334241132030e+0);
	z = ddadd2_d2_d2_d(ddmul_d2_d2_d(z, t), o0 ? -0.5772156649015328655e+0 : +0.4227843350984671345e+0);
	z = ddmul_d2_d2_d(z, t);

	clc = o2 ? y : z;

	clld = o2 ? ddadd2_d2_d2_d(ddmul_d2_d_d(u, t), 1) : clld;

	y = clln;

	clc = otiny ? dd(83.1776616671934334590333, 3.67103459631568507221878e-15) : // log(2^120)
		(oref ? ddadd2_d2_d2_d2(dd(1.1447298858494001639, 1.026595116270782638e-17), ddneg_d2_d2(clc)) : clc); // log(Math::pi<f64>)
	clln = otiny ? dd(1, 0) : (oref ? clln : clld);

	if (oref) x = ddmul_d2_d2_d2(clld, sinpik(a - (f64)(INT64_C(1) << 28) * (i32)(a * (1.0 / (INT64_C(1) << 28)))));

	clld = otiny ? dd(a * ((INT64_C(1) << 60) * (f64)(INT64_C(1) << 60)), 0) : (oref ? x : y);

	dd2 ret = { clc, dddiv_d2_d2_d2(clln, clld) };

	return ret;
}

NO_SIDE_EFFECTS constexpr f64 xtgamma_u1(f64 a) {
	dd2 d = gammak(a);
	Sleef_double2 y = ddmul_d2_d2_d2(expk2(d.a), d.b);
	f64 r = y.x + y.y;
	r = (a == -SLEEF_INFINITY || (a < 0 && xisint(a)) || (xisnumber(a) && a < 0 && xisnan(r))) ? SLEEF_NAN : r;
	r = ((a == SLEEF_INFINITY || xisnumber(a)) && a >= -DBL_MIN && (a == 0 || a > 200 || xisnan(r))) ? mulsign(SLEEF_INFINITY, a) : r;
	return r;
}

NO_SIDE_EFFECTS constexpr f64 xlgamma_u1(f64 a) {
	dd2 d = gammak(a);
	Sleef_double2 y = ddadd2_d2_d2_d2(d.a, logk2(ddabs_d2_d2(d.b)));
	f64 r = y.x + y.y;
	r = (xisinf(a) || (a <= 0 && xisint(a)) || (xisnumber(a) && xisnan(r))) ? SLEEF_INFINITY : r;
	return r;
}

NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 ddmla(f64 x, Sleef_double2 y, Sleef_double2 z) {
	return ddadd2_d2_d2_d2(z, ddmul_d2_d2_d(y, x));
}
NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 poly2dd_b(f64 x, Sleef_double2 c1, Sleef_double2 c0) { return ddmla(x, c1, c0); }
NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 poly2dd(f64 x, f64 c1, Sleef_double2 c0) { return ddmla(x, dd(c1, 0), c0); }
NO_SIDE_EFFECTS INLINE constexpr Sleef_double2 poly4dd(f64 x, f64 c3, Sleef_double2 c2, Sleef_double2 c1, Sleef_double2 c0) {
	return ddmla(x * x, poly2dd(x, c3, c2), poly2dd_b(x, c1, c0));
}

NO_SIDE_EFFECTS constexpr f64 xerf_u1(f64 a) {
	f64 t, x = fabsk(a);
	Sleef_double2 t2;
	f64 x2 = x * x, x4 = x2 * x2, x8 = x4 * x4, x16 = x8 * x8;

	if (x < 2.5) {
		// Abramowitz and Stegun
		t = POLY21(x, x2, x4, x8, x16,
				   -0.2083271002525222097e-14,
				   +0.7151909970790897009e-13,
				   -0.1162238220110999364e-11,
				   +0.1186474230821585259e-10,
				   -0.8499973178354613440e-10,
				   +0.4507647462598841629e-9,
				   -0.1808044474288848915e-8,
				   +0.5435081826716212389e-8,
				   -0.1143939895758628484e-7,
				   +0.1215442362680889243e-7,
				   +0.1669878756181250355e-7,
				   -0.9808074602255194288e-7,
				   +0.1389000557865837204e-6,
				   +0.2945514529987331866e-6,
				   -0.1842918273003998283e-5,
				   +0.3417987836115362136e-5,
				   +0.3860236356493129101e-5,
				   -0.3309403072749947546e-4,
				   +0.1060862922597579532e-3,
				   +0.2323253155213076174e-3,
				   +0.1490149719145544729e-3);
		t2 = poly4dd(x, t,
					 dd(0.0092877958392275604405, 7.9287559463961107493e-19),
					 dd(0.042275531758784692937, 1.3785226620501016138e-19),
					 dd(0.07052369794346953491, 9.5846628070792092842e-19));
		t2 = ddadd_d2_d_d2(1, ddmul_d2_d2_d(t2, x));
		t2 = ddsqu_d2_d2(t2);
		t2 = ddsqu_d2_d2(t2);
		t2 = ddsqu_d2_d2(t2);
		t2 = ddsqu_d2_d2(t2);
		t2 = ddrec_d2_d2(t2);
	}
	else if (x > 6.0) {
		t2 = dd(0, 0);
	}
	else {
		t = POLY21(x, x2, x4, x8, x16,
				   -0.4024015130752621932e-18,
				   +0.3847193332817048172e-16,
				   -0.1749316241455644088e-14,
				   +0.5029618322872872715e-13,
				   -0.1025221466851463164e-11,
				   +0.1573695559331945583e-10,
				   -0.1884658558040203709e-9,
				   +0.1798167853032159309e-8,
				   -0.1380745342355033142e-7,
				   +0.8525705726469103499e-7,
				   -0.4160448058101303405e-6,
				   +0.1517272660008588485e-5,
				   -0.3341634127317201697e-5,
				   -0.2515023395879724513e-5,
				   +0.6539731269664907554e-4,
				   -0.3551065097428388658e-3,
				   +0.1210736097958368864e-2,
				   -0.2605566912579998680e-2,
				   +0.1252823202436093193e-2,
				   +0.1820191395263313222e-1,
				   -0.1021557155453465954e+0);
		t2 = poly4dd(x, t,
					 dd(-0.63691044383641748361, -2.4249477526539431839e-17),
					 dd(-1.1282926061803961737, -6.2970338860410996505e-17),
					 dd(-1.2261313785184804967e-05, -5.5329707514490107044e-22));
		t2 = dd(expk(t2), 0);
	}

	t2 = ddadd2_d2_d2_d(t2, -1);

	if (x < 1e-8) t2 = dd(-1.12837916709551262756245475959 * x, 0);
	return mulsign(a == 0 ? 0 : (xisinf(a) ? 1 : (-t2.x - t2.y)), a);
}

NO_SIDE_EFFECTS constexpr f64 xerfc_u15(f64 a) {
	f64 s = a, r = 0, t;
	Sleef_double2 u, d, x;
	a = fabsk(a);
	i32 o0 = a < 1.0, o1 = a < 2.2, o2 = a < 4.2, o3 = a < 27.3;
	u = o0 ? ddmul_d2_d_d(a, a) : o1 ? dd(a, 0) : dddiv_d2_d2_d2(dd(1, 0), dd(a, 0));

	t = o0 ? +0.6801072401395386139e-20 : o1 ? +0.3438010341362585303e-12 : o2 ? -0.5757819536420710449e+2 : +0.2334249729638701319e+5;
	t = mla(t, u.x, o0 ? -0.2161766247570055669e-18 : o1 ? -0.1237021188160598264e-10 : o2 ? +0.4669289654498104483e+3 : -0.4695661044933107769e+5);
	t = mla(t, u.x, o0 ? +0.4695919173301595670e-17 : o1 ? +0.2117985839877627852e-09 : o2 ? -0.1796329879461355858e+4 : +0.3173403108748643353e+5);
	t = mla(t, u.x, o0 ? -0.9049140419888007122e-16 : o1 ? -0.2290560929177369506e-08 : o2 ? +0.4355892193699575728e+4 : +0.3242982786959573787e+4);
	t = mla(t, u.x, o0 ? +0.1634018903557410728e-14 : o1 ? +0.1748931621698149538e-07 : o2 ? -0.7456258884965764992e+4 : -0.2014717999760347811e+5);
	t = mla(t, u.x, o0 ? -0.2783485786333451745e-13 : o1 ? -0.9956602606623249195e-07 : o2 ? +0.9553977358167021521e+4 : +0.1554006970967118286e+5);
	t = mla(t, u.x, o0 ? +0.4463221276786415752e-12 : o1 ? +0.4330010240640327080e-06 : o2 ? -0.9470019905444229153e+4 : -0.6150874190563554293e+4);
	t = mla(t, u.x, o0 ? -0.6711366622850136563e-11 : o1 ? -0.1435050600991763331e-05 : o2 ? +0.7387344321849855078e+4 : +0.1240047765634815732e+4);
	t = mla(t, u.x, o0 ? +0.9422759050232662223e-10 : o1 ? +0.3460139479650695662e-05 : o2 ? -0.4557713054166382790e+4 : -0.8210325475752699731e+2);
	t = mla(t, u.x, o0 ? -0.1229055530100229098e-08 : o1 ? -0.4988908180632898173e-05 : o2 ? +0.2207866967354055305e+4 : +0.3242443880839930870e+2);
	t = mla(t, u.x, o0 ? +0.1480719281585086512e-07 : o1 ? -0.1308775976326352012e-05 : o2 ? -0.8217975658621754746e+3 : -0.2923418863833160586e+2);
	t = mla(t, u.x, o0 ? -0.1636584469123399803e-06 : o1 ? +0.2825086540850310103e-04 : o2 ? +0.2268659483507917400e+3 : +0.3457461732814383071e+0);
	t = mla(t, u.x, o0 ? +0.1646211436588923575e-05 : o1 ? -0.6393913713069986071e-04 : o2 ? -0.4633361260318560682e+2 : +0.5489730155952392998e+1);
	t = mla(t, u.x, o0 ? -0.1492565035840623511e-04 : o1 ? -0.2566436514695078926e-04 : o2 ? +0.9557380123733945965e+1 : +0.1559934132251294134e-2);
	t = mla(t, u.x, o0 ? +0.1205533298178967851e-03 : o1 ? +0.5895792375659440364e-03 : o2 ? -0.2958429331939661289e+1 : -0.1541741566831520638e+1);
	t = mla(t, u.x, o0 ? -0.8548327023450850081e-03 : o1 ? -0.1695715579163588598e-02 : o2 ? +0.1670329508092765480e+0 : +0.2823152230558364186e-5);
	t = mla(t, u.x, o0 ? +0.5223977625442187932e-02 : o1 ? +0.2089116434918055149e-03 : o2 ? +0.6096615680115419211e+0 : +0.6249999184195342838e+0);
	t = mla(t, u.x, o0 ? -0.2686617064513125222e-01 : o1 ? +0.1912855949584917753e-01 : o2 ? +0.1059212443193543585e-2 : +0.1741749416408701288e-8);

	d = ddmul_d2_d2_d(u, t);
	d = ddadd2_d2_d2_d2(d, o0 ? dd(0.11283791670955126141, -4.0175691625932118483e-18) :
						o1 ? dd(-0.10277263343147646779, -6.2338714083404900225e-18) :
						o2 ? dd(-0.50005180473999022439, 2.6362140569041995803e-17) :
						dd(-0.5000000000258444377, -4.0074044712386992281e-17));
	d = ddmul_d2_d2_d2(d, u);
	d = ddadd2_d2_d2_d2(d, o0 ? dd(-0.37612638903183753802, 1.3391897206042552387e-17) :
						o1 ? dd(-0.63661976742916359662, 7.6321019159085724662e-18) :
						o2 ? dd(1.601106273924963368e-06, 1.1974001857764476775e-23) :
						dd(2.3761973137523364792e-13, -1.1670076950531026582e-29));
	d = ddmul_d2_d2_d2(d, u);
	d = ddadd2_d2_d2_d2(d, o0 ? dd(1.1283791670955125586, 1.5335459613165822674e-17) :
						o1 ? dd(-1.1283791674717296161, 8.0896847755965377194e-17) :
						o2 ? dd(-0.57236496645145429341, 3.0704553245872027258e-17) :
						dd(-0.57236494292470108114, -2.3984352208056898003e-17));

	x = ddmul_d2_d2_d(o1 ? d : dd(-a, 0), a);
	x = o1 ? x : ddadd2_d2_d2_d2(x, d);
	x = o0 ? ddsub_d2_d2_d2(dd(1, 0), x) : expk2(x);
	x = o1 ? x : ddmul_d2_d2_d2(x, u);

	r = o3 ? (x.x + x.y) : 0;
	if (s < 0) r = 2 - r;
	r = xisnan(s) ? SLEEF_NAN : r;
	return r;
}

#undef SLEEF_FP_ILOGB0 
#undef SLEEF_FP_ILOGBNAN 
#undef SLEEF_FLT_MIN 
#undef SLEEF_DBL_MIN 
#undef SLEEF_INT_MAX 
#undef SLEEF_DBL_DENORM_MIN 
#undef SLEEF_FLT_DENORM_MIN 
#undef PI_A 
#undef PI_B 
#undef PI_C 
#undef PI_D 
#undef TRIGRANGEMAX 
#undef PI_A2 
#undef PI_B2 
#undef TRIGRANGEMAX2 
#undef M_2_PI_H 
#undef M_2_PI_L 
#undef SQRT_DBL_MAX 
#undef TRIGRANGEMAX3 
#undef M_4_PI 
#undef L2U 
#undef L2L 
#undef R_LN2 
#undef L10U 
#undef L10L 
#undef LOG10_2 
#undef L10Uf 
#undef L10Lf 
#undef PI_Af 
#undef PI_Bf 
#undef PI_Cf 
#undef PI_Df 
#undef TRIGRANGEMAXf 
#undef PI_A2f 
#undef PI_B2f 
#undef PI_C2f 
#undef TRIGRANGEMAX2f 
#undef TRIGRANGEMAX4f 
#undef SQRT_FLT_MAX 
#undef L2Uf 
#undef L2Lf 
#undef R_LN2f 
#undef M_PIf 
#undef SLEEF_NAN 
#undef SLEEF_NANf 
#undef SLEEF_NANl 
#undef SLEEF_INFINITY 
#undef SLEEF_INFINITYf 
#undef SLEEF_INFINITYl 
#undef SLEEF_INFINITYq 
#undef SLEEF_NANq 
#undef SLEEF_INFINITYq 
#undef SLEEF_NANq 
#undef SLEEF_INFINITY 
#undef SLEEF_NAN 
#undef SLEEF_INFINITYf 
#undef SLEEF_NANf 
#undef SLEEF_INFINITYl 
#undef SLEEF_NANl 