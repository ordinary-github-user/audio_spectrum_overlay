#pragma once
#include "BasicTypes.h"
#include "ConceptFloating.h"
#include "ConceptIntegral.h"
#include "ConceptSame.h"
#include "Inline.h"
#include "Predict.h"
#include "Pure.h"
#include <emmintrin.h>
#include <limits>

namespace musl
{
	namespace sqrt_details
	{
		constexpr bool FENV_SUPPORT{ 1 };

		/* returns a*b*2^-32 - e, with error 0 <= e < 1.  */
		NO_SIDE_EFFECTS INLINE constexpr u32 mul32(u32 a, u32 b)noexcept
		{
			return (u64)a * b >> 32;
		}

		/* returns a*b*2^-64 - e, with error 0 <= e < 3.  */
		NO_SIDE_EFFECTS INLINE constexpr u64 mul64(u64 a, u64 b)noexcept
		{
			const u64 ahi{ a >> 32 };
			const u64 alo{ a & 0xffffffff };
			const u64 bhi{ b >> 32 };
			const u64 blo{ b & 0xffffffff };
			return ahi * bhi + (ahi * blo >> 32) + (alo * bhi >> 32);
		}

		constexpr u16 __rsqrt_tab[128]{
			0xb451,0xb2f0,0xb196,0xb044,0xaef9,0xadb6,0xac79,0xab43,
			0xaa14,0xa8eb,0xa7c8,0xa6aa,0xa592,0xa480,0xa373,0xa26b,
			0xa168,0xa06a,0x9f70,0x9e7b,0x9d8a,0x9c9d,0x9bb5,0x9ad1,
			0x99f0,0x9913,0x983a,0x9765,0x9693,0x95c4,0x94f8,0x9430,
			0x936b,0x92a9,0x91ea,0x912e,0x9075,0x8fbe,0x8f0a,0x8e59,
			0x8daa,0x8cfe,0x8c54,0x8bac,0x8b07,0x8a64,0x89c4,0x8925,
			0x8889,0x87ee,0x8756,0x86c0,0x862b,0x8599,0x8508,0x8479,
			0x83ec,0x8361,0x82d8,0x8250,0x81c9,0x8145,0x80c2,0x8040,
			0xff02,0xfd0e,0xfb25,0xf947,0xf773,0xf5aa,0xf3ea,0xf234,
			0xf087,0xeee3,0xed47,0xebb3,0xea27,0xe8a3,0xe727,0xe5b2,
			0xe443,0xe2dc,0xe17a,0xe020,0xdecb,0xdd7d,0xdc34,0xdaf1,
			0xd9b3,0xd87b,0xd748,0xd61a,0xd4f1,0xd3cd,0xd2ad,0xd192,
			0xd07b,0xcf69,0xce5b,0xcd51,0xcc4a,0xcb48,0xca4a,0xc94f,
			0xc858,0xc764,0xc674,0xc587,0xc49d,0xc3b7,0xc2d4,0xc1f4,
			0xc116,0xc03c,0xbf65,0xbe90,0xbdbe,0xbcef,0xbc23,0xbb59,
			0xba91,0xb9cc,0xb90a,0xb84a,0xb78c,0xb6d0,0xb617,0xb560,
		};
	}

	NO_SIDE_EFFECTS INLINE constexpr f32 sqrtf(f32 x)noexcept
	{
		if !consteval
		{
			return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
		}
		else
		{
			using namespace sqrt_details;

			u32 ix, m, m1, m0, even, ey;

			ix = __builtin_bit_cast(u32, x);
			if (predict_false(ix - 0x00800000 >= 0x7f800000 - 0x00800000)) {
				/* x < 0x1p-126 or inf or nan.  */
				if (ix * 2 == 0)
					return x;
				if (ix == 0x7f800000)
					return x;
				if (ix > 0x7f800000)
					return std::numeric_limits<f32>::quiet_NaN();
				/* x is subnormal, normalize it.  */
				ix = __builtin_bit_cast(u32, x * 0x1p23f);
				ix -= 23 << 23;
			}

			/* x = 4^e m; with i32 e and m in [1, 4).  */
			even = ix & 0x00800000;
			m1 = (ix << 8) | 0x80000000;
			m0 = (ix << 7) & 0x7fffffff;
			m = even ? m0 : m1;

			/* 2^e is the exponent part of the return value.  */
			ey = ix >> 1;
			ey += 0x3f800000 >> 1;
			ey &= 0x7f800000;

			/* compute r ~ 1/sqrt(m), s ~ sqrt(m) with 2 goldschmidt iterations.  */
			static constexpr u32 three{ 0xc0000000 };
			u32 r, s, d, u, i;
			i = (ix >> 17) % 128;
			r = (u32)__rsqrt_tab[i] << 16;
			/* |r*sqrt(m) - 1| < 0x1p-8 */
			s = mul32(m, r);
			/* |s/sqrt(m) - 1| < 0x1p-8 */
			d = mul32(s, r);
			u = three - d;
			r = mul32(r, u) << 1;
			/* |r*sqrt(m) - 1| < 0x1.7bp-16 */
			s = mul32(s, u) << 1;
			/* |s/sqrt(m) - 1| < 0x1.7bp-16 */
			d = mul32(s, r);
			u = three - d;
			s = mul32(s, u);
			/* -0x1.03p-28 < s/sqrt(m) - 1 < 0x1.fp-31 */
			s = (s - 1) >> 6;
			/* s < sqrt(m) < s + 0x1.08p-23 */

			/* compute nearest rounded result.  */
			u32 d0, d1, d2;
			f32 y, t;
			d0 = (m << 16) - s * s;
			d1 = s - d0;
			d2 = d1 + s + 1;
			s += d1 >> 31;
			s &= 0x007fffff;
			s |= ey;
			y = __builtin_bit_cast(f32,s);
			if constexpr (FENV_SUPPORT)
			{
				/* handle rounding and inexact exception. */
				u32 tiny = predict_false(d2 == 0) ? 0 : 0x01000000;
				tiny |= (d1 ^ d2) & 0x80000000;
				t = __builtin_bit_cast(f32,tiny);
				y = y + t;
			}
			return y;
		}
	}

	NO_SIDE_EFFECTS INLINE constexpr f64 sqrt(f64 x)noexcept
	{
		if !consteval
		{
#if GCC
			asm("sqrtsd %0, %0" : "+x" (x));
			return x;
#else
			return _mm_cvtsd_f64(_mm_sqrt_sd({}, _mm_set_sd(x)));
#endif
		}
		else
		{
			using namespace sqrt_details;

			u64 ix, top, m;

			/* special case handling.  */
			ix = __builtin_bit_cast(u64, x);
			top = ix >> 52;
			if (predict_false(top - 0x001 >= 0x7ff - 0x001))
			{
				/* x < 0x1p-1022 or inf or nan.  */
				if (ix * 2 == 0)
					return x;
				if (ix == 0x7ff0000000000000)
					return x;
				if (ix > 0x7ff0000000000000)
					return std::numeric_limits<f64>::quiet_NaN();
				/* x is subnormal, normalize it.  */
				ix = __builtin_bit_cast(u64,x * 0x1p52);
				top = ix >> 52;
				top -= 52;
			}

			/* argument reduction:
			   x = 4^e m; with integer e, and m in [1, 4)
			   m: fixed point representation [2.62]
			   2^e is the exponent part of the result.  */
			i32 even = top & 1;
			m = (ix << 11) | 0x8000000000000000;
			if (even) m >>= 1;
			top = (top + 0x3ff) >> 1;

			/* approximate r ~ 1/sqrt(m) and s ~ sqrt(m) when m in [1,4)

			   initial estimate:
			   7bit table lookup (1bit exponent and 6bit significand).

			   iterative approximation:
			   using 2 goldschmidt iterations with 32bit i32 arithmetics
			   and a final iteration with 64bit i32 arithmetics.

			   details:

			   the relative error (e = r0 sqrt(m)-1) of a linear estimate
			   (r0 = a m + b) is |e| < 0.085955 ~ 0x1.6p-4 at best,
			   a table lookup is faster and needs one less iteration
			   6 bit lookup table (128b) gives |e| < 0x1.f9p-8
			   7 bit lookup table (256b) gives |e| < 0x1.fdp-9
			   for single and f64 prec 6bit is enough but for quad
			   prec 7bit is needed (or modified iterations). to avoid
			   one more iteration >=13bit table would be needed (16k).

			   a newton-raphson iteration for r is
				 w = r*r
				 u = 3 - m*w
				 r = r*u/2
			   can use a goldschmidt iteration for s at the end or
				 s = m*r

			   first goldschmidt iteration is
				 s = m*r
				 u = 3 - s*r
				 r = r*u/2
				 s = s*u/2
			   next goldschmidt iteration is
				 u = 3 - s*r
				 r = r*u/2
				 s = s*u/2
			   and at the end r is not computed only s.

			   they use the same amount of operations and converge at the
			   same quadratic rate, i.e. if
				 r1 sqrt(m) - 1 = e, then
				 r2 sqrt(m) - 1 = -3/2 e^2 - 1/2 e^3
			   the advantage of goldschmidt is that the mul for s and r
			   are independent (computed in parallel), however it is not
			   "self synchronizing": it only uses the input m in the
			   first iteration so rounding errors accumulate. at the end
			   or when switching to larger precision arithmetics rounding
			   errors dominate so the first iteration should be used.

			   the fixed point representations are
				 m: 2.30 r: 0.32, s: 2.30, d: 2.30, u: 2.30, three: 2.30
			   and after switching to 64 bit
				 m: 2.62 r: 0.64, s: 2.62, d: 2.62, u: 2.62, three: 2.62  */

			static constexpr u64 three{ 0xc0000000 };
			u64 r, s, d, u, i;

			i = (ix >> 46) % 128;
			r = (u32)__rsqrt_tab[i] << 16;
			/* |r sqrt(m) - 1| < 0x1.fdp-9 */
			s = mul32(static_cast<u32>(m >> 32), static_cast<u32>(r));
			/* |s/sqrt(m) - 1| < 0x1.fdp-9 */
			d = mul32(static_cast<u32>(s), static_cast<u32>(r));
			u = three - d;
			r = mul32(static_cast<u32>(r), static_cast<u32>(u)) << 1;
			/* |r sqrt(m) - 1| < 0x1.7bp-16 */
			s = mul32(static_cast<u32>(s), static_cast<u32>(u)) << 1;
			/* |s/sqrt(m) - 1| < 0x1.7bp-16 */
			d = mul32(static_cast<u32>(s), static_cast<u32>(r));
			u = three - d;
			r = mul32(static_cast<u32>(r), static_cast<u32>(u)) << 1;
			/* |r sqrt(m) - 1| < 0x1.3704p-29 (measured worst-case) */
			r = r << 32;
			s = mul64(m, r);
			d = mul64(s, r);
			u = (three << 32) - d;
			s = mul64(s, u);  /* repr: 3.61 */
			/* -0x1p-57 < s - sqrt(m) < 0x1.8001p-61 */
			s = (s - 2) >> 9; /* repr: 12.52 */
			/* -0x1.09p-52 < s - sqrt(m) < -0x1.fffcp-63 */

			/* s < sqrt(m) < s + 0x1.09p-52,
			   compute nearest rounded result:
			   the nearest result to 52 bits is either s or s+0x1p-52,
			   we can decide by comparing (2^52 s + 0.5)^2 to 2^104 m.  */
			u64 d0, d1, d2;
			f64 y, t;
			d0 = (m << 42) - s * s;
			d1 = s - d0;
			d2 = d1 + s + 1;
			s += d1 >> 63;
			s &= 0x000fffffffffffff;
			s |= top << 52;
			y = __builtin_bit_cast(f64,s);
			if constexpr (FENV_SUPPORT)
			{
				/* handle rounding modes and inexact exception:
				   only (s+1)^2 == 2^42 m case is exact otherwise
				   add a tiny value to cause the fenv effects.  */
				u64 tiny = predict_false(d2 == 0) ? 0 : 0x0010000000000000;
				tiny |= (d1 ^ d2) & 0x8000000000000000;
				t = __builtin_bit_cast(f64,tiny);
				y = y + t;
			}
			return y;
		}
	}
}
namespace Math
{
	NO_SIDE_EFFECTS INLINE constexpr f32 sqrt(f32 x)noexcept { return musl::sqrtf(x); }
	NO_SIDE_EFFECTS INLINE constexpr f64 sqrt(f64 x)noexcept { return musl::sqrt(x); }
	NO_SIDE_EFFECTS INLINE constexpr f32 rsqrt(f32 x)noexcept
	{
		if consteval
		{
			return 1.f / sqrt(x);
		}
		else
		{
			return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
		}
	}
	NO_SIDE_EFFECTS INLINE constexpr f64 rsqrt(f64 x)noexcept = delete;
	//NO_SIDE_EFFECTS INLINE constexpr f64 rsqrt(f64 x)noexcept { return 1. / sqrt(x); }
}