#pragma once
//https://github.com/ifduyue/musl/tree/master/src/math
#include <bit>
#include <float.h>
#include <math.h>
#include "../Util/BasicTypes.h"
#include "../Util/BitCast.h"
#include "../Util/DisableWarning.h"
#include "../Util/Inline.h"
#include "../Util/Macro/Pragma.h"
#include "../Util/Predict.h"
#include "../Util/Pure.h"
#include "MathConstant.h"
#include "Numeric/Abs.h"
#include "Numeric/IsNaN.h"
#include "Numeric/Sqrt.h"
PRAGMA_MSVC(warning(push)); PRAGMA_MSVC(warning(disable : 4146));
#define FORCE_EVAL(x) ((void)0)

#define EXTRACT_WORDS(hi,lo,d) do { u64 __u = BITCAST(u64,d); (hi) = __u >> 32; (lo) = (u32)__u; } while (0)

#define GET_HIGH_WORD(hi,d) do { (hi) = BITCAST(u64,d) >> 32; } while (0)

#define GET_LOW_WORD(lo,d) do { (lo) = (u32)BITCAST(u64,d); } while (0)

#define INSERT_WORDS(d,hi,lo) do { (d) = BITCAST(f64, (u64(hi)<<32) | u32(lo)); } while (0)

#define SET_HIGH_WORD(d,hi) INSERT_WORDS(d, hi, (u32)BITCAST(u64,d))

#define SET_LOW_WORD(d,lo) INSERT_WORDS(d, BITCAST(u64,d)>>32, lo)

#define GET_FLOAT_WORD(w,d) do { (w) = BITCAST(u32,d); } while (0)

#define SET_FLOAT_WORD(d,w) do { (d) = BITCAST(f32,w); } while (0)

#define TOINT_INTRINSICS 0
#define EXP_USE_TOINT_NARROW 0
#define WANT_ROUNDING 1

namespace musl
{
	PRAGMA_MSVC(warning(push)); PRAGMA_MSVC(warning(disable : 4723)); PRAGMA_MSVC(warning(disable : 4756));
	NO_SIDE_EFFECTS INLINE constexpr f32 __math_invalidf(f32 x)noexcept
	{
		return (x - x) / (x - x);
	}
	NO_SIDE_EFFECTS INLINE constexpr f64 __math_invalid(f64 x)noexcept
	{
		return (x - x) / (x - x);
	}
	NO_SIDE_EFFECTS INLINE constexpr f32 __math_divzerof(u32 sign)noexcept
	{
		return (sign ? -1.0f : 1.0f) / 0.0f;
	}
	NO_SIDE_EFFECTS INLINE constexpr f64 __math_divzero(u32 sign)noexcept
	{
		return (sign ? -1.0 : 1.0) / 0.0;
	}
	NO_SIDE_EFFECTS INLINE constexpr f64 __math_xflow(u32 sign, f64 y) noexcept { return (sign ? -y : y) * y; }
	NO_SIDE_EFFECTS INLINE constexpr f64 __math_uflow(u32 sign)noexcept { return __math_xflow(sign, 0x1p-767); }
	NO_SIDE_EFFECTS INLINE constexpr f64 __math_oflow(u32 sign)noexcept { return __math_xflow(sign, 0x1p769); }
	NO_SIDE_EFFECTS INLINE constexpr f32 __math_xflowf(u32 sign, f32 y)noexcept { return (sign ? -y : y) * y; }
	NO_SIDE_EFFECTS INLINE constexpr f32 __math_oflowf(u32 sign)noexcept { return __math_xflowf(sign, 0x1p97f); }
	NO_SIDE_EFFECTS INLINE constexpr f32 __math_uflowf(u32 sign)noexcept { return __math_xflowf(sign, 0x1p-95f); }
	PRAGMA_MSVC(warning(pop));

	NO_SIDE_EFFECTS INLINE constexpr u32 top12(f64 x)noexcept { return __builtin_bit_cast(u64, x) >> 52; }
	NO_SIDE_EFFECTS INLINE constexpr u32 top12(f32 x)noexcept { return __builtin_bit_cast(u32, x) >> 20; }

	NO_SIDE_EFFECTS INLINE constexpr f32 roundf(f32 x)noexcept
	{
		static constexpr f32 EPS =
#if FLT_EVAL_METHOD==0
			FLT_EPSILON;
#elif FLT_EVAL_METHOD==1
			DBL_EPSILON;
#elif FLT_EVAL_METHOD==2
			LDBL_EPSILON;
#endif

		static constexpr f32 toint = 1 / EPS;

		union { f32 f; u32 i; } u = { x };
		i32 e = u.i >> 23 & 0xff;
		f32 y;

		if (e >= 0x7f + 23)
			return x;
		if (u.i >> 31)
			x = -x;
		if (e < 0x7f - 1) {
			FORCE_EVAL(x + toint);
			return 0 * u.f;
		}
		y = x + toint - toint - x;
		if (y > 0.5f)
			y = y + x - 1;
		else if (y <= -0.5f)
			y = y + x + 1;
		else
			y = y + x;
		if (u.i >> 31)
			y = -y;
		return y;
	}

	NO_SIDE_EFFECTS INLINE constexpr f64 round(f64 x)noexcept
	{
		static constexpr f64 EPS =
#if FLT_EVAL_METHOD==0 || FLT_EVAL_METHOD==1
			DBL_EPSILON;
#elif FLT_EVAL_METHOD==2
			LDBL_EPSILON;
#endif

		static constexpr f64 toint = 1 / EPS;

		union { f64 f; u64 i; } u = { x };
		i32 e = u.i >> 52 & 0x7ff;
		f64 y;

		if (e >= 0x3ff + 52)
			return x;
		if (u.i >> 63)
			x = -x;
		if (e < 0x3ff - 1) {
			/* raise inexact if x!=0 */
			FORCE_EVAL(x + toint);
			return 0 * u.f;
		}
		y = x + toint - toint - x;
		if (y > 0.5)
			y = y + x - 1;
		else if (y <= -0.5)
			y = y + x + 1;
		else
			y = y + x;
		if (u.i >> 63)
			y = -y;
		return y;
	}

	NO_SIDE_EFFECTS INLINE constexpr f32 ceilf(f32 x)noexcept
	{
		union { f32 f; u32 i; } u = { x };
		i32 e = (i32)(u.i >> 23 & 0xff) - 0x7f;
		u32 m;

		if (e >= 23)
			return x;
		if (e >= 0) {
			m = 0x007fffff >> e;
			if ((u.i & m) == 0)
				return x;
			FORCE_EVAL(x + 0x1p120f);
			if (u.i >> 31 == 0)
				u.i += m;
			u.i &= ~m;
		}
		else {
			FORCE_EVAL(x + 0x1p120f);
			if (u.i >> 31)
				u.f = -0.0;
			else if (u.i << 1)
				u.f = 1.0;
		}
		return u.f;
	}

	NO_SIDE_EFFECTS INLINE constexpr f64 ceil(f64 x)noexcept
	{
		static constexpr f64 EPS =
#if FLT_EVAL_METHOD==0 || FLT_EVAL_METHOD==1
			DBL_EPSILON;
#elif FLT_EVAL_METHOD==2
			LDBL_EPSILON;
#endif

		static constexpr f64 toint = 1 / EPS;

		union { f64 f; u64 i; } u = { x };
		i32 e = u.i >> 52 & 0x7ff;
		f64 y;

		if (e >= 0x3ff + 52 || x == 0)
			return x;
		/* y = i32(x) - x, where i32(x) is an integer neighbor of x */
		if (u.i >> 63)
			y = x - toint + toint - x;
		else
			y = x + toint - toint - x;
		/* special case because of non-nearest rounding modes */
		if (e <= 0x3ff - 1) {
			FORCE_EVAL(y);
			return u.i >> 63 ? -0.0 : 1;
		}
		if (y < 0)
			return x + y + 1;
		return x + y;
	}

	NO_SIDE_EFFECTS INLINE constexpr f32 floorf(f32 x)noexcept
	{
		union { f32 f; u32 i; } u = { x };
		i32 e = (i32)(u.i >> 23 & 0xff) - 0x7f;
		u32 m;

		if (e >= 23)
			return x;
		if (e >= 0) {
			m = 0x007fffff >> e;
			if ((u.i & m) == 0)
				return x;
			FORCE_EVAL(x + 0x1p120f);
			if (u.i >> 31)
				u.i += m;
			u.i &= ~m;
		}
		else {
			FORCE_EVAL(x + 0x1p120f);
			if (u.i >> 31 == 0)
				u.i = 0;
			else if (u.i << 1)
				u.f = -1.0;
		}
		return u.f;
	}

	NO_SIDE_EFFECTS INLINE constexpr f64 floor(f64 x)noexcept
	{
		static constexpr f64 EPS =
#if FLT_EVAL_METHOD==0 || FLT_EVAL_METHOD==1
			DBL_EPSILON;
#elif FLT_EVAL_METHOD==2
			LDBL_EPSILON;
#endif
		static constexpr f64 toint = 1 / EPS;

		union { f64 f; u64 i; } u = { x };
		i32 e = u.i >> 52 & 0x7ff;
		f64 y;

		if (e >= 0x3ff + 52 || x == 0)
			return x;
		/* y = i32(x) - x, where i32(x) is an integer neighbor of x */
		if (u.i >> 63)
			y = x - toint + toint - x;
		else
			y = x + toint - toint - x;
		/* special case because of non-nearest rounding modes */
		if (e <= 0x3ff - 1) {
			FORCE_EVAL(y);
			return u.i >> 63 ? -1 : 0;
		}
		if (y > 0)
			return x + y - 1;
		return x + y;
	}

	namespace acos_details
	{
		constexpr f64
			pio2_hi = 1.57079632679489655800e+00, /* 0x3FF921FB, 0x54442D18 */
			pio2_lo = 6.12323399573676603587e-17, /* 0x3C91A626, 0x33145C07 */
			pS0 = 1.66666666666666657415e-01, /* 0x3FC55555, 0x55555555 */
			pS1 = -3.25565818622400915405e-01, /* 0xBFD4D612, 0x03EB6F7D */
			pS2 = 2.01212532134862925881e-01, /* 0x3FC9C155, 0x0E884455 */
			pS3 = -4.00555345006794114027e-02, /* 0xBFA48228, 0xB5688F3B */
			pS4 = 7.91534994289814532176e-04, /* 0x3F49EFE0, 0x7501B288 */
			pS5 = 3.47933107596021167570e-05, /* 0x3F023DE1, 0x0DFDF709 */
			qS1 = -2.40339491173441421878e+00, /* 0xC0033A27, 0x1C8A2D4B */
			qS2 = 2.02094576023350569471e+00, /* 0x40002AE5, 0x9C598AC8 */
			qS3 = -6.88283971605453293030e-01, /* 0xBFE6066C, 0x1B8D0159 */
			qS4 = 7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */

		NO_SIDE_EFFECTS INLINE constexpr f64 R(f64 z)noexcept
		{
			f64 p, q;
			p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
			q = 1.0 + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
			return p / q;
		}
	}

	NO_SIDE_EFFECTS constexpr f64 acos(f64 x)noexcept
	{
		using namespace acos_details;
		f64 z, w, s, c, df;
		u32 hx, ix;

		GET_HIGH_WORD(hx, x);
		ix = hx & 0x7fffffff;
		/* |x| >= 1 or nan */
		if (ix >= 0x3ff00000) {
			u32 lx;

			GET_LOW_WORD(lx, x);
			if (((ix - 0x3ff00000) | lx) == 0) {
				/* acos(1)=0, acos(-1)=pi */
				if (hx >> 31)
					return 2 * pio2_hi + 0x1p-120f;
				return 0;
			}
			return 0 / (x - x);
		}
		/* |x| < 0.5 */
		if (ix < 0x3fe00000) {
			if (ix <= 0x3c600000)  /* |x| < 2**-57 */
				return pio2_hi + 0x1p-120f;
			return pio2_hi - (x - (pio2_lo - x * R(x * x)));
		}
		/* x < -0.5 */
		if (hx >> 31) {
			z = (1.0 + x) * 0.5;
			s = sqrt(z);
			w = R(z) * s - pio2_lo;
			return 2 * (pio2_hi - (s + w));
		}
		/* x > 0.5 */
		z = (1.0 - x) * 0.5;
		s = sqrt(z);
		df = s;
		SET_LOW_WORD(df, 0);
		c = (z - df * df) / (s + df);
		w = R(z) * s + c;
		return 2 * (df + w);
	}

	namespace acosf_details
	{
		constexpr f32
			pio2_hi = 1.5707962513e+00f, /* 0x3fc90fda */
			pio2_lo = 7.5497894159e-08f, /* 0x33a22168 */
			pS0 = 1.6666586697e-01f,
			pS1 = -4.2743422091e-02f,
			pS2 = -8.6563630030e-03f,
			qS1 = -7.0662963390e-01f;
		NO_SIDE_EFFECTS INLINE f32 R(f32 z)noexcept
		{
			f32 p, q;
			p = z * (pS0 + z * (pS1 + z * pS2));
			q = 1.0f + z * qS1;
			return p / q;
		}
	}

	NO_SIDE_EFFECTS constexpr f32 acosf(f32 x)noexcept
	{
		using namespace acosf_details;
		f32 z, w, s, c, df;
		u32 hx, ix;

		GET_FLOAT_WORD(hx, x);
		ix = hx & 0x7fffffff;
		/* |x| >= 1 or nan */
		if (ix >= 0x3f800000) {
			if (ix == 0x3f800000) {
				if (hx >> 31)
					return 2 * pio2_hi + 0x1p-120f;
				return 0;
			}
			return 0 / (x - x);
		}
		/* |x| < 0.5 */
		if (ix < 0x3f000000) {
			if (ix <= 0x32800000) /* |x| < 2**-26 */
				return pio2_hi + 0x1p-120f;
			return pio2_hi - (x - (pio2_lo - x * R(x * x)));
		}
		/* x < -0.5 */
		if (hx >> 31) {
			z = (1 + x) * 0.5f;
			s = sqrtf(z);
			w = R(z) * s - pio2_lo;
			return 2 * (pio2_hi - (s + w));
		}
		/* x > 0.5 */
		z = (1 - x) * 0.5f;
		s = sqrtf(z);
		GET_FLOAT_WORD(hx, s);
		SET_FLOAT_WORD(df, hx & 0xfffff000);
		c = (z - df * df) / (s + df);
		w = R(z) * s + c;
		return 2 * (df + w);
	}

	namespace asin_details
	{
		constexpr f64
			pio2_hi = 1.57079632679489655800e+00, /* 0x3FF921FB, 0x54442D18 */
			pio2_lo = 6.12323399573676603587e-17, /* 0x3C91A626, 0x33145C07 */
			/* coefficients for R(x^2) */
			pS0 = 1.66666666666666657415e-01, /* 0x3FC55555, 0x55555555 */
			pS1 = -3.25565818622400915405e-01, /* 0xBFD4D612, 0x03EB6F7D */
			pS2 = 2.01212532134862925881e-01, /* 0x3FC9C155, 0x0E884455 */
			pS3 = -4.00555345006794114027e-02, /* 0xBFA48228, 0xB5688F3B */
			pS4 = 7.91534994289814532176e-04, /* 0x3F49EFE0, 0x7501B288 */
			pS5 = 3.47933107596021167570e-05, /* 0x3F023DE1, 0x0DFDF709 */
			qS1 = -2.40339491173441421878e+00, /* 0xC0033A27, 0x1C8A2D4B */
			qS2 = 2.02094576023350569471e+00, /* 0x40002AE5, 0x9C598AC8 */
			qS3 = -6.88283971605453293030e-01, /* 0xBFE6066C, 0x1B8D0159 */
			qS4 = 7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */

		NO_SIDE_EFFECTS INLINE constexpr f64 R(f64 z)noexcept
		{
			f64 p, q;
			p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
			q = 1.0 + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
			return p / q;
		}
	}

	NO_SIDE_EFFECTS constexpr f64 asin(f64 x)noexcept
	{
		using namespace asin_details;
		f64 z, r, s;
		u32 hx, ix;

		GET_HIGH_WORD(hx, x);
		ix = hx & 0x7fffffff;
		/* |x| >= 1 or nan */
		if (ix >= 0x3ff00000) {
			u32 lx;
			GET_LOW_WORD(lx, x);
			if (((ix - 0x3ff00000) | lx) == 0)
				/* asin(1) = +-pi/2 with inexact */
				return x * pio2_hi + 0x1p-120f;
			return 0 / (x - x);
		}
		/* |x| < 0.5 */
		if (ix < 0x3fe00000) {
			/* if 0x1p-1022 <= |x| < 0x1p-26, avoid raising underflow */
			if (ix < 0x3e500000 && ix >= 0x00100000)
				return x;
			return x + x * R(x * x);
		}
		/* 1 > |x| >= 0.5 */
		z = (1 - Math::abs(x)) * 0.5;
		s = sqrt(z);
		r = R(z);
		if (ix >= 0x3fef3333) {  /* if |x| > 0.975 */
			x = pio2_hi - (2 * (s + s * r) - pio2_lo);
		}
		else {
			f64 f, c;
			/* f+c = sqrt(z) */
			f = s;
			SET_LOW_WORD(f, 0);
			c = (z - f * f) / (s + f);
			x = 0.5 * pio2_hi - (2 * s * r - (pio2_lo - 2 * c) - (0.5 * pio2_hi - 2 * f));
		}
		if (hx >> 31)
			return -x;
		return x;
	}

	namespace asinf_details
	{
		constexpr f64
			pio2 = 1.570796326794896558e+00;

		constexpr f32
			/* coefficients for R(x^2) */
			pS0 = 1.6666586697e-01f,
			pS1 = -4.2743422091e-02f,
			pS2 = -8.6563630030e-03f,
			qS1 = -7.0662963390e-01f;

		NO_SIDE_EFFECTS INLINE constexpr f32 R(f32 z)noexcept
		{
			f32 p, q;
			p = z * (pS0 + z * (pS1 + z * pS2));
			q = 1.0f + z * qS1;
			return p / q;
		}
	}

	NO_SIDE_EFFECTS constexpr f32 asinf(f32 x)noexcept
	{
		using namespace asinf_details;
		f64 s;
		f32 z;
		u32 hx, ix;

		GET_FLOAT_WORD(hx, x);
		ix = hx & 0x7fffffff;
		if (ix >= 0x3f800000) {  /* |x| >= 1 */
			if (ix == 0x3f800000)  /* |x| == 1 */
				return static_cast<f32>(x * pio2 + 0x1p-120f);  /* asin(+-1) = +-pi/2 with inexact */
			return 0 / (x - x);  /* asin(|x|>1) is NaN */
		}
		if (ix < 0x3f000000) {  /* |x| < 0.5 */
			/* if 0x1p-126 <= |x| < 0x1p-12, avoid raising underflow */
			if (ix < 0x39800000 && ix >= 0x00800000)
				return x;
			return x + x * R(x * x);
		}
		/* 1 > |x| >= 0.5 */
		z = (1 - Math::abs(x)) * 0.5f;
		s = sqrtf(z);
		x = static_cast<f32>(pio2 - 2 * (s + s * R(z)));
		if (hx >> 31)
			return -x;
		return x;
	}

	NO_SIDE_EFFECTS constexpr f64 atan(f64 x)noexcept
	{
		static constexpr f64 atanhi[]{
		  4.63647609000806093515e-01, /* atan(0.5)hi 0x3FDDAC67, 0x0561BB4F */
		  7.85398163397448278999e-01, /* atan(1.0)hi 0x3FE921FB, 0x54442D18 */
		  9.82793723247329054082e-01, /* atan(1.5)hi 0x3FEF730B, 0xD281F69B */
		  1.57079632679489655800e+00, /* atan(inf)hi 0x3FF921FB, 0x54442D18 */
		};

		static constexpr f64 atanlo[]{
		  2.26987774529616870924e-17, /* atan(0.5)lo 0x3C7A2B7F, 0x222F65E2 */
		  3.06161699786838301793e-17, /* atan(1.0)lo 0x3C81A626, 0x33145C07 */
		  1.39033110312309984516e-17, /* atan(1.5)lo 0x3C700788, 0x7AF0CBBD */
		  6.12323399573676603587e-17, /* atan(inf)lo 0x3C91A626, 0x33145C07 */
		};

		static constexpr f64 aT[]{
		  3.33333333333329318027e-01, /* 0x3FD55555, 0x5555550D */
		 -1.99999999998764832476e-01, /* 0xBFC99999, 0x9998EBC4 */
		  1.42857142725034663711e-01, /* 0x3FC24924, 0x920083FF */
		 -1.11111104054623557880e-01, /* 0xBFBC71C6, 0xFE231671 */
		  9.09088713343650656196e-02, /* 0x3FB745CD, 0xC54C206E */
		 -7.69187620504482999495e-02, /* 0xBFB3B0F2, 0xAF749A6D */
		  6.66107313738753120669e-02, /* 0x3FB10D66, 0xA0D03D51 */
		 -5.83357013379057348645e-02, /* 0xBFADDE2D, 0x52DEFD9A */
		  4.97687799461593236017e-02, /* 0x3FA97B4B, 0x24760DEB */
		 -3.65315727442169155270e-02, /* 0xBFA2B444, 0x2C6A6C2F */
		  1.62858201153657823623e-02, /* 0x3F90AD3A, 0xE322DA11 */
		};

		f64 w, s1, s2, z;
		u32 ix, sign;
		i32 id;

		GET_HIGH_WORD(ix, x);
		sign = ix >> 31;
		ix &= 0x7fffffff;
		if (ix >= 0x44100000) {   /* if |x| >= 2^66 */
			if (Math::isNaN(x))
				return x;
			z = atanhi[3] + 0x1p-120f;
			return sign ? -z : z;
		}
		if (ix < 0x3fdc0000) {    /* |x| < 0.4375 */
			if (ix < 0x3e400000) {  /* |x| < 2^-27 */
				if (ix < 0x00100000)
					/* raise underflow for subnormal x */
					FORCE_EVAL((f32)x);
				return x;
			}
			id = -1;
		}
		else {
			x = Math::abs(x);
			if (ix < 0x3ff30000) {  /* |x| < 1.1875 */
				if (ix < 0x3fe60000) {  /*  7/16 <= |x| < 11/16 */
					id = 0;
					x = (2.0 * x - 1.0) / (2.0 + x);
				}
				else {                /* 11/16 <= |x| < 19/16 */
					id = 1;
					x = (x - 1.0) / (x + 1.0);
				}
			}
			else {
				if (ix < 0x40038000) {  /* |x| < 2.4375 */
					id = 2;
					x = (x - 1.5) / (1.0 + 1.5 * x);
				}
				else {                /* 2.4375 <= |x| < 2^66 */
					id = 3;
					x = -1.0 / x;
				}
			}
		}
		/* end of argument reduction */
		z = x * x;
		w = z * z;
		/* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
		s1 = z * (aT[0] + w * (aT[2] + w * (aT[4] + w * (aT[6] + w * (aT[8] + w * aT[10])))));
		s2 = w * (aT[1] + w * (aT[3] + w * (aT[5] + w * (aT[7] + w * aT[9]))));
		if (id < 0)
			return x - x * (s1 + s2);
		z = atanhi[id] - (x * (s1 + s2) - atanlo[id] - x);
		return sign ? -z : z;
	}

	NO_SIDE_EFFECTS constexpr f32 atanf(f32 x)noexcept
	{
		static constexpr f32 atanhi[]{
		  4.6364760399e-01f, /* atan(0.5)hi 0x3eed6338 */
		  7.8539812565e-01f, /* atan(1.0)hi 0x3f490fda */
		  9.8279368877e-01f, /* atan(1.5)hi 0x3f7b985e */
		  1.5707962513e+00f, /* atan(inf)hi 0x3fc90fda */
		};

		static constexpr f32 atanlo[]{
		  5.0121582440e-09f, /* atan(0.5)lo 0x31ac3769 */
		  3.7748947079e-08f, /* atan(1.0)lo 0x33222168 */
		  3.4473217170e-08f, /* atan(1.5)lo 0x33140fb4 */
		  7.5497894159e-08f, /* atan(inf)lo 0x33a22168 */
		};

		static constexpr f32 aT[]{
		  3.3333328366e-01f,
		 -1.9999158382e-01f,
		  1.4253635705e-01f,
		 -1.0648017377e-01f,
		  6.1687607318e-02f,
		};

		f32 w, s1, s2, z;
		u32 ix, sign;
		i32 id;

		GET_FLOAT_WORD(ix, x);
		sign = ix >> 31;
		ix &= 0x7fffffff;
		if (ix >= 0x4c800000) {  /* if |x| >= 2**26 */
			if (Math::isNaN(x))
				return x;
			z = atanhi[3] + 0x1p-120f;
			return sign ? -z : z;
		}
		if (ix < 0x3ee00000) {   /* |x| < 0.4375 */
			if (ix < 0x39800000) {  /* |x| < 2**-12 */
				if (ix < 0x00800000)
					/* raise underflow for subnormal x */
					FORCE_EVAL(x * x);
				return x;
			}
			id = -1;
		}
		else {
			x = Math::abs(x);
			if (ix < 0x3f980000) {  /* |x| < 1.1875 */
				if (ix < 0x3f300000) {  /*  7/16 <= |x| < 11/16 */
					id = 0;
					x = (2.0f * x - 1.0f) / (2.0f + x);
				}
				else {                /* 11/16 <= |x| < 19/16 */
					id = 1;
					x = (x - 1.0f) / (x + 1.0f);
				}
			}
			else {
				if (ix < 0x401c0000) {  /* |x| < 2.4375 */
					id = 2;
					x = (x - 1.5f) / (1.0f + 1.5f * x);
				}
				else {                /* 2.4375 <= |x| < 2**26 */
					id = 3;
					x = -1.0f / x;
				}
			}
		}
		/* end of argument reduction */
		z = x * x;
		w = z * z;
		/* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
		s1 = z * (aT[0] + w * (aT[2] + w * aT[4]));
		s2 = w * (aT[1] + w * aT[3]);
		if (id < 0)
			return x - x * (s1 + s2);
		z = atanhi[id] - ((x * (s1 + s2) - atanlo[id]) - x);
		return sign ? -z : z;
	}

	NO_SIDE_EFFECTS constexpr f64 atan2(f64 y, f64 x)noexcept
	{
		static constexpr f64
			pi = 3.1415926535897931160E+00, /* 0x400921FB, 0x54442D18 */
			pi_lo = 1.2246467991473531772E-16; /* 0x3CA1A626, 0x33145C07 */

		f64 z;
		u32 m, lx, ly, ix, iy;

		if (Math::isNaN(x) || Math::isNaN(y))
			return x + y;
		EXTRACT_WORDS(ix, lx, x);
		EXTRACT_WORDS(iy, ly, y);
		if (((ix - 0x3ff00000) | lx) == 0)  /* x = 1.0 */
			return atan(y);
		m = ((iy >> 31) & 1) | ((ix >> 30) & 2);  /* 2*sign(x)+sign(y) */
		ix = ix & 0x7fffffff;
		iy = iy & 0x7fffffff;

		/* when y = 0 */
		if ((iy | ly) == 0) {
			switch (m) {
				case 0:
				case 1: return y;   /* atan(+-0,+anything)=+-0 */
				case 2: return  pi; /* atan(+0,-anything) = pi */
				case 3: return -pi; /* atan(-0,-anything) =-pi */
			}
		}
		/* when x = 0 */
		if ((ix | lx) == 0)
			return m & 1 ? -pi / 2 : pi / 2;
		/* when x is INF */
		if (ix == 0x7ff00000) {
			if (iy == 0x7ff00000) {
				switch (m) {
					case 0: return  pi / 4;   /* atan(+INF,+INF) */
					case 1: return -pi / 4;   /* atan(-INF,+INF) */
					case 2: return  3 * pi / 4; /* atan(+INF,-INF) */
					case 3: return -3 * pi / 4; /* atan(-INF,-INF) */
				}
			}
			else {
				switch (m) {
					case 0: return  0.0; /* atan(+...,+INF) */
					case 1: return -0.0; /* atan(-...,+INF) */
					case 2: return  pi;  /* atan(+...,-INF) */
					case 3: return -pi;  /* atan(-...,-INF) */
				}
			}
		}
		/* |y/x| > 0x1p64 */
		if (ix + (64 << 20) < iy || iy == 0x7ff00000)
			return m & 1 ? -pi / 2 : pi / 2;

		/* z = atan(|y/x|) without spurious underflow */
		if ((m & 2) && iy + (64 << 20) < ix)  /* |y/x| < 0x1p-64, x<0 */
			z = 0;
		else
			z = atan(Math::abs(y / x));
		switch (m) {
			case 0: return z;              /* atan(+,+) */
			case 1: return -z;             /* atan(-,+) */
			case 2: return pi - (z - pi_lo); /* atan(+,-) */
			default: /* case 3 */
				return (z - pi_lo) - pi; /* atan(-,-) */
		}
	}

	NO_SIDE_EFFECTS constexpr f32 atan2f(f32 y, f32 x)noexcept
	{
		static constexpr f32
			pi = 3.1415927410e+00f, /* 0x40490fdb */
			pi_lo = -8.7422776573e-08f; /* 0xb3bbbd2e */

		f32 z;
		u32 m, ix, iy;

		if (Math::isNaN(x) || Math::isNaN(y))
			return x + y;
		GET_FLOAT_WORD(ix, x);
		GET_FLOAT_WORD(iy, y);
		if (ix == 0x3f800000)  /* x=1.0 */
			return atanf(y);
		m = ((iy >> 31) & 1) | ((ix >> 30) & 2);  /* 2*sign(x)+sign(y) */
		ix &= 0x7fffffff;
		iy &= 0x7fffffff;

		/* when y = 0 */
		if (iy == 0) {
			switch (m) {
				case 0:
				case 1: return y;   /* atan(+-0,+anything)=+-0 */
				case 2: return  pi; /* atan(+0,-anything) = pi */
				case 3: return -pi; /* atan(-0,-anything) =-pi */
			}
		}
		/* when x = 0 */
		if (ix == 0)
			return m & 1 ? -pi / 2 : pi / 2;
		/* when x is INF */
		if (ix == 0x7f800000) {
			if (iy == 0x7f800000) {
				switch (m) {
					case 0: return  pi / 4; /* atan(+INF,+INF) */
					case 1: return -pi / 4; /* atan(-INF,+INF) */
					case 2: return 3 * pi / 4;  /*atan(+INF,-INF)*/
					case 3: return -3 * pi / 4; /*atan(-INF,-INF)*/
				}
			}
			else {
				switch (m) {
					case 0: return  0.0f;    /* atan(+...,+INF) */
					case 1: return -0.0f;    /* atan(-...,+INF) */
					case 2: return  pi; /* atan(+...,-INF) */
					case 3: return -pi; /* atan(-...,-INF) */
				}
			}
		}
		/* |y/x| > 0x1p26 */
		if (ix + (26 << 23) < iy || iy == 0x7f800000)
			return m & 1 ? -pi / 2 : pi / 2;

		/* z = atan(|y/x|) with correct underflow */
		if ((m & 2) && iy + (26 << 23) < ix)  /*|y/x| < 0x1p-26, x < 0 */
			z = 0.0;
		else
			z = atanf(Math::abs(y / x));
		switch (m) {
			case 0: return z;              /* atan(+,+) */
			case 1: return -z;             /* atan(-,+) */
			case 2: return pi - (z - pi_lo); /* atan(+,-) */
			default: /* case 3 */
				return (z - pi_lo) - pi; /* atan(-,-) */
		}
	}

	namespace trig_details
	{
		NO_SIDE_EFFECTS constexpr f64 scalbn(f64 x, i32 n)noexcept
		{
			union { f64 f; u64 i; } u;
			f64 y = x;

			if (n > 1023) {
				y *= 0x1p1023;
				n -= 1023;
				if (n > 1023) {
					y *= 0x1p1023;
					n -= 1023;
					if (n > 1023)
						n = 1023;
				}
			}
			else if (n < -1022) {
				/* make sure final n < -53 to avoid f64
				   rounding in the subnormal range */
				y *= 0x1p-1022 * 0x1p53;
				n += 1022 - 53;
				if (n < -1022) {
					y *= 0x1p-1022 * 0x1p53;
					n += 1022 - 53;
					if (n < -1022)
						n = -1022;
				}
			}
			u.i = (u64)(0x3ff + n) << 52;
			x = y * u.f;
			return x;
		}

		constexpr i32 __rem_pio2_large(f64* x, f64* y, i32 e0, i32 nx, i32 prec)noexcept
		{
			static constexpr i32 init_jk[]{ 3,4,4,6 };

			static constexpr i32 ipio2[]{
				0xA2F983, 0x6E4E44, 0x1529FC, 0x2757D1, 0xF534DD, 0xC0DB62,
				0x95993C, 0x439041, 0xFE5163, 0xABDEBB, 0xC561B7, 0x246E3A,
				0x424DD2, 0xE00649, 0x2EEA09, 0xD1921C, 0xFE1DEB, 0x1CB129,
				0xA73EE8, 0x8235F5, 0x2EBB44, 0x84E99C, 0x7026B4, 0x5F7E41,
				0x3991D6, 0x398353, 0x39F49C, 0x845F8B, 0xBDF928, 0x3B1FF8,
				0x97FFDE, 0x05980F, 0xEF2F11, 0x8B5A0A, 0x6D1F6D, 0x367ECF,
				0x27CB09, 0xB74F46, 0x3F669E, 0x5FEA2D, 0x7527BA, 0xC7EBE5,
				0xF17B3D, 0x0739F7, 0x8A5292, 0xEA6BFB, 0x5FB11F, 0x8D5D08,
				0x560330, 0x46FC7B, 0x6BABF0, 0xCFBC20, 0x9AF436, 0x1DA9E3,
				0x91615E, 0xE61B08, 0x659985, 0x5F14A0, 0x68408D, 0xFFD880,
				0x4D7327, 0x310606, 0x1556CA, 0x73A8C9, 0x60E27B, 0xC08C6B,

			#if LDBL_MAX_EXP > 1024
				0x47C419, 0xC367CD, 0xDCE809, 0x2A8359, 0xC4768B, 0x961CA6,
				0xDDAF44, 0xD15719, 0x053EA5, 0xFF0705, 0x3F7E33, 0xE832C2,
				0xDE4F98, 0x327DBB, 0xC33D26, 0xEF6B1E, 0x5EF89F, 0x3A1F35,
				0xCAF27F, 0x1D87F1, 0x21907C, 0x7C246A, 0xFA6ED5, 0x772D30,
				0x433B15, 0xC614B5, 0x9D19C3, 0xC2C4AD, 0x414D2C, 0x5D000C,
				0x467D86, 0x2D71E3, 0x9AC69B, 0x006233, 0x7CD2B4, 0x97A7B4,
				0xD55537, 0xF63ED7, 0x1810A3, 0xFC764D, 0x2A9D64, 0xABD770,
				0xF87C63, 0x57B07A, 0xE71517, 0x5649C0, 0xD9D63B, 0x3884A7,
				0xCB2324, 0x778AD6, 0x23545A, 0xB91F00, 0x1B0AF1, 0xDFCE19,
				0xFF319F, 0x6A1E66, 0x615799, 0x47FBAC, 0xD87F7E, 0xB76522,
				0x89E832, 0x60BFE6, 0xCDC4EF, 0x09366C, 0xD43F5D, 0xD7DE16,
				0xDE3B58, 0x929BDE, 0x2822D2, 0xE88628, 0x4D58E2, 0x32CAC6,
				0x16E308, 0xCB7DE0, 0x50C017, 0xA71DF3, 0x5BE018, 0x34132E,
				0x621283, 0x014883, 0x5B8EF5, 0x7FB0AD, 0xF2E91E, 0x434A48,
				0xD36710, 0xD8DDAA, 0x425FAE, 0xCE616A, 0xA4280A, 0xB499D3,
				0xF2A606, 0x7F775C, 0x83C2A3, 0x883C61, 0x78738A, 0x5A8CAF,
				0xBDD76F, 0x63A62D, 0xCBBFF4, 0xEF818D, 0x67C126, 0x45CA55,
				0x36D9CA, 0xD2A828, 0x8D61C2, 0x77C912, 0x142604, 0x9B4612,
				0xC459C4, 0x44C5C8, 0x91B24D, 0xF31700, 0xAD43D4, 0xE54929,
				0x10D5FD, 0xFCBE00, 0xCC941E, 0xEECE70, 0xF53E13, 0x80F1EC,
				0xC3E7B3, 0x28F8C7, 0x940593, 0x3E71C1, 0xB3092E, 0xF3450B,
				0x9C1288, 0x7B20AB, 0x9FB52E, 0xC29247, 0x2F327B, 0x6D550C,
				0x90A772, 0x1FE76B, 0x96CB31, 0x4A1679, 0xE27941, 0x89DFF4,
				0x9794E8, 0x84E6E2, 0x973199, 0x6BED88, 0x365F5F, 0x0EFDBB,
				0xB49A48, 0x6CA467, 0x427271, 0x325D8D, 0xB8159F, 0x09E5BC,
				0x25318D, 0x3974F7, 0x1C0530, 0x010C0D, 0x68084B, 0x58EE2C,
				0x90AA47, 0x02E774, 0x24D6BD, 0xA67DF7, 0x72486E, 0xEF169F,
				0xA6948E, 0xF691B4, 0x5153D1, 0xF20ACF, 0x339820, 0x7E4BF5,
				0x6863B2, 0x5F3EDD, 0x035D40, 0x7F8985, 0x295255, 0xC06437,
				0x10D86D, 0x324832, 0x754C5B, 0xD4714E, 0x6E5445, 0xC1090B,
				0x69F52A, 0xD56614, 0x9D0727, 0x50045D, 0xDB3BB4, 0xC576EA,
				0x17F987, 0x7D6B49, 0xBA271D, 0x296996, 0xACCCC6, 0x5414AD,
				0x6AE290, 0x89D988, 0x50722C, 0xBEA404, 0x940777, 0x7030F3,
				0x27FC00, 0xA871EA, 0x49C266, 0x3DE064, 0x83DD97, 0x973FA3,
				0xFD9443, 0x8C860D, 0xDE4131, 0x9D3992, 0x8C70DD, 0xE7B717,
				0x3BDF08, 0x2B3715, 0xA0805C, 0x93805A, 0x921110, 0xD8E80F,
				0xAF806C, 0x4BFFDB, 0x0F9038, 0x761859, 0x15A562, 0xBBCB61,
				0xB989C7, 0xBD4010, 0x04F2D2, 0x277549, 0xF6B6EB, 0xBB22DB,
				0xAA140A, 0x2F2689, 0x768364, 0x333B09, 0x1A940E, 0xAA3A51,
				0xC2A31D, 0xAEEDAF, 0x12265C, 0x4DC26D, 0x9C7A2D, 0x9756C0,
				0x833F03, 0xF6F009, 0x8C402B, 0x99316D, 0x07B439, 0x15200C,
				0x5BC3D8, 0xC492F5, 0x4BADC6, 0xA5CA4E, 0xCD37A7, 0x36A9E6,
				0x9492AB, 0x6842DD, 0xDE6319, 0xEF8C76, 0x528B68, 0x37DBFC,
				0xABA1AE, 0x3115DF, 0xA1AE00, 0xDAFB0C, 0x664D64, 0xB705ED,
				0x306529, 0xBF5657, 0x3AFF47, 0xB9F96A, 0xF3BE75, 0xDF9328,
				0x3080AB, 0xF68C66, 0x15CB04, 0x0622FA, 0x1DE4D9, 0xA4B33D,
				0x8F1B57, 0x09CD36, 0xE9424E, 0xA4BE13, 0xB52333, 0x1AAAF0,
				0xA8654F, 0xA5C1D2, 0x0F3F0B, 0xCD785B, 0x76F923, 0x048B7B,
				0x721789, 0x53A6C6, 0xE26E6F, 0x00EBEF, 0x584A9B, 0xB7DAC4,
				0xBA66AA, 0xCFCF76, 0x1D02D1, 0x2DF1B1, 0xC1998C, 0x77ADC3,
				0xDA4886, 0xA05DF7, 0xF480C6, 0x2FF0AC, 0x9AECDD, 0xBC5C3F,
				0x6DDED0, 0x1FC790, 0xB6DB2A, 0x3A25A3, 0x9AAF00, 0x9353AD,
				0x0457B6, 0xB42D29, 0x7E804B, 0xA707DA, 0x0EAA76, 0xA1597B,
				0x2A1216, 0x2DB7DC, 0xFDE5FA, 0xFEDB89, 0xFDBE89, 0x6C76E4,
				0xFCA906, 0x70803E, 0x156E85, 0xFF87FD, 0x073E28, 0x336761,
				0x86182A, 0xEABD4D, 0xAFE7B3, 0x6E6D8F, 0x396795, 0x5BBF31,
				0x48D784, 0x16DF30, 0x432DC7, 0x356125, 0xCE70C9, 0xB8CB30,
				0xFD6CBF, 0xA200A4, 0xE46C05, 0xA0DD5A, 0x476F21, 0xD21262,
				0x845CB9, 0x496170, 0xE0566B, 0x015299, 0x375550, 0xB7D51E,
				0xC4F133, 0x5F6E13, 0xE4305D, 0xA92E85, 0xC3B21D, 0x3632A1,
				0xA4B708, 0xD4B1EA, 0x21F716, 0xE4698F, 0x77FF27, 0x80030C,
				0x2D408D, 0xA0CD4F, 0x99A520, 0xD3A2B3, 0x0A5D2F, 0x42F9B4,
				0xCBDA11, 0xD0BE7D, 0xC1DB9B, 0xBD17AB, 0x81A2CA, 0x5C6A08,
				0x17552E, 0x550027, 0xF0147F, 0x8607E1, 0x640B14, 0x8D4196,
				0xDEBE87, 0x2AFDDA, 0xB6256B, 0x34897B, 0xFEF305, 0x9EBFB9,
				0x4F6A68, 0xA82A4A, 0x5AC44F, 0xBCF82D, 0x985AD7, 0x95C7F4,
				0x8D4D0D, 0xA63A20, 0x5F57A4, 0xB13F14, 0x953880, 0x0120CC,
				0x86DD71, 0xB6DEC9, 0xF560BF, 0x11654D, 0x6B0701, 0xACB08C,
				0xD0C0B2, 0x485551, 0x0EFB1E, 0xC37295, 0x3B06A3, 0x3540C0,
				0x7BDC06, 0xCC45E0, 0xFA294E, 0xC8CAD6, 0x41F3E8, 0xDE647C,
				0xD8649B, 0x31BED9, 0xC397A4, 0xD45877, 0xC5E369, 0x13DAF0,
				0x3C3ABA, 0x461846, 0x5F7555, 0xF5BDD2, 0xC6926E, 0x5D2EAC,
				0xED440E, 0x423E1C, 0x87C461, 0xE9FD29, 0xF3D6E7, 0xCA7C22,
				0x35916F, 0xC5E008, 0x8DD7FF, 0xE26A6E, 0xC6FDB0, 0xC10893,
				0x745D7C, 0xB2AD6B, 0x9D6ECD, 0x7B723E, 0x6A11C6, 0xA9CFF7,
				0xDF7329, 0xBAC9B5, 0x5100B7, 0x0DB2E2, 0x24BA74, 0x607DE5,
				0x8AD874, 0x2C150D, 0x0C1881, 0x94667E, 0x162901, 0x767A9F,
				0xBEFDFD, 0xEF4556, 0x367ED9, 0x13D9EC, 0xB9BA8B, 0xFC97C4,
				0x27A831, 0xC36EF1, 0x36C594, 0x56A8D8, 0xB5A8B4, 0x0ECCCF,
				0x2D8912, 0x34576F, 0x89562C, 0xE3CE99, 0xB920D6, 0xAA5E6B,
				0x9C2A3E, 0xCC5F11, 0x4A0BFD, 0xFBF4E1, 0x6D3B8E, 0x2C86E2,
				0x84D4E9, 0xA9B4FC, 0xD1EEEF, 0xC9352E, 0x61392F, 0x442138,
				0xC8D91B, 0x0AFC81, 0x6A4AFB, 0xD81C2F, 0x84B453, 0x8C994E,
				0xCC2254, 0xDC552A, 0xD6C6C0, 0x96190B, 0xB8701A, 0x649569,
				0x605A26, 0xEE523F, 0x0F117F, 0x11B5F4, 0xF5CBFC, 0x2DBC34,
				0xEEBC34, 0xCC5DE8, 0x605EDD, 0x9B8E67, 0xEF3392, 0xB817C9,
				0x9B5861, 0xBC57E1, 0xC68351, 0x103ED8, 0x4871DD, 0xDD1C2D,
				0xA118AF, 0x462C21, 0xD7F359, 0x987AD9, 0xC0549E, 0xFA864F,
				0xFC0656, 0xAE79E5, 0x362289, 0x22AD38, 0xDC9367, 0xAAE855,
				0x382682, 0x9BE7CA, 0xA40D51, 0xB13399, 0x0ED7A9, 0x480569,
				0xF0B265, 0xA7887F, 0x974C88, 0x36D1F9, 0xB39221, 0x4A827B,
				0x21CF98, 0xDC9F40, 0x5547DC, 0x3A74E1, 0x42EB67, 0xDF9DFE,
				0x5FD45E, 0xA4677B, 0x7AACBA, 0xA2F655, 0x23882B, 0x55BA41,
				0x086E59, 0x862A21, 0x834739, 0xE6E389, 0xD49EE5, 0x40FB49,
				0xE956FF, 0xCA0F1C, 0x8A59C5, 0x2BFA94, 0xC5C1D3, 0xCFC50F,
				0xAE5ADB, 0x86C547, 0x624385, 0x3B8621, 0x94792C, 0x876110,
				0x7B4C2A, 0x1A2C80, 0x12BF43, 0x902688, 0x893C78, 0xE4C4A8,
				0x7BDBE5, 0xC23AC4, 0xEAF426, 0x8A67F7, 0xBF920D, 0x2BA365,
				0xB1933D, 0x0B7CBD, 0xDC51A4, 0x63DD27, 0xDDE169, 0x19949A,
				0x9529A8, 0x28CE68, 0xB4ED09, 0x209F44, 0xCA984E, 0x638270,
				0x237C7E, 0x32B90F, 0x8EF5A7, 0xE75614, 0x08F121, 0x2A9DB5,
				0x4D7E6F, 0x5119A5, 0xABF9B5, 0xD6DF82, 0x61DD96, 0x023616,
				0x9F3AC4, 0xA1A283, 0x6DED72, 0x7A8D39, 0xA9B882, 0x5C326B,
				0x5B2746, 0xED3400, 0x7700D2, 0x55F4FC, 0x4D5901, 0x8071E0,
			#endif
			};

			static constexpr f64 PIo2[]{
				1.57079625129699707031e+00, /* 0x3FF921FB, 0x40000000 */
				7.54978941586159635335e-08, /* 0x3E74442D, 0x00000000 */
				5.39030252995776476554e-15, /* 0x3CF84698, 0x80000000 */
				3.28200341580791294123e-22, /* 0x3B78CC51, 0x60000000 */
				1.27065575308067607349e-29, /* 0x39F01B83, 0x80000000 */
				1.22933308981111328932e-36, /* 0x387A2520, 0x40000000 */
				2.73370053816464559624e-44, /* 0x36E38222, 0x80000000 */
				2.16741683877804819444e-51, /* 0x3569F31D, 0x00000000 */
			};
			i32 jz, jx, jv, jp, jk, carry, n, iq[20], i, j, k, m, q0, ih;
			f64 z, fw, f[20], fq[20], q[20];

			/* initialize jk*/
			jk = init_jk[prec];
			jp = jk;

			/* determine jx,jv,q0, note that 3>q0 */
			jx = nx - 1;
			jv = (e0 - 3) / 24;  if (jv < 0) jv = 0;
			q0 = e0 - 24 * (jv + 1);

			/* set up f[0] to f[jx+jk] where f[jx+jk] = ipio2[jv+jk] */
			j = jv - jx; m = jx + jk;
			for (i = 0; i <= m; i++, j++)
				f[i] = j < 0 ? 0.0 : (f64)ipio2[j];

			/* compute q[0],q[1],...q[jk] */
			for (i = 0; i <= jk; i++) {
				for (j = 0, fw = 0.0; j <= jx; j++)
					fw += x[j] * f[jx + i - j];
				q[i] = fw;
			}

			jz = jk;
		recompute:
			/* distill q[] into iq[] reversingly */
			for (i = 0, j = jz, z = q[jz]; j > 0; i++, j--) {
				fw = (f64)(i32)(0x1p-24 * z);
				iq[i] = (i32)(z - 0x1p24 * fw);
				z = q[j - 1] + fw;
			}

			/* compute n */
			z = scalbn(z, q0);       /* actual value of z */
			z -= 8.0 * floor(z * 0.125); /* trim off integer >= 8 */
			n = (i32)z;
			z -= (f64)n;
			ih = 0;
			if (q0 > 0) {  /* need iq[jz-1] to determine n */
				i = iq[jz - 1] >> (24 - q0); n += i;
				iq[jz - 1] -= i << (24 - q0);
				ih = iq[jz - 1] >> (23 - q0);
			}
			else if (q0 == 0) ih = iq[jz - 1] >> 23;
			else if (z >= 0.5) ih = 2;

			if (ih > 0) {  /* q > 0.5 */
				n += 1; carry = 0;
				for (i = 0; i < jz; i++) {  /* compute 1-q */
					j = iq[i];
					if (carry == 0) {
						if (j != 0) {
							carry = 1;
							iq[i] = 0x1000000 - j;
						}
					}
					else
						iq[i] = 0xffffff - j;
				}
				if (q0 > 0) {  /* rare case: chance is 1 in 12 */
					switch (q0) {
						case 1:
							iq[jz - 1] &= 0x7fffff; break;
						case 2:
							iq[jz - 1] &= 0x3fffff; break;
					}
				}
				if (ih == 2) {
					z = 1.0 - z;
					if (carry != 0)
						z -= scalbn(1.0, q0);
				}
			}

			/* check if recomputation is needed */
			if (z == 0.0) {
				j = 0;
				for (i = jz - 1; i >= jk; i--) j |= iq[i];
				if (j == 0) {  /* need recomputation */
					for (k = 1; iq[jk - k] == 0; k++);  /* k = no. of terms needed */

					for (i = jz + 1; i <= jz + k; i++) {  /* add q[jz+1] to q[jz+k] */
						f[jx + i] = (f64)ipio2[jv + i];
						for (j = 0, fw = 0.0; j <= jx; j++)
							fw += x[j] * f[jx + i - j];
						q[i] = fw;
					}
					jz += k;
					goto recompute;
				}
			}

			/* chop off zero terms */
			if (z == 0.0) {
				jz -= 1;
				q0 -= 24;
				while (iq[jz] == 0) {
					jz--;
					q0 -= 24;
				}
			}
			else { /* break z into 24-bit if necessary */
				z = scalbn(z, -q0);
				if (z >= 0x1p24) {
					fw = (f64)(i32)(0x1p-24 * z);
					iq[jz] = (i32)(z - 0x1p24 * fw);
					jz += 1;
					q0 += 24;
					iq[jz] = (i32)fw;
				}
				else
					iq[jz] = (i32)z;
			}

			/* convert integer "bit" chunk to floating-point value */
			fw = scalbn(1.0, q0);
			for (i = jz; i >= 0; i--) {
				q[i] = fw * (f64)iq[i];
				fw *= 0x1p-24;
			}

			/* compute PIo2[0,...,jp]*q[jz,...,0] */
			for (i = jz; i >= 0; i--) {
				for (fw = 0.0, k = 0; k <= jp && k <= jz - i; k++)
					fw += PIo2[k] * q[i + k];
				fq[jz - i] = fw;
			}

			/* compress fq[] into y[] */
			switch (prec) {
				case 0:
					fw = 0.0;
					for (i = jz; i >= 0; i--)
						fw += fq[i];
					y[0] = ih == 0 ? fw : -fw;
					break;
				case 1:
				case 2:
					fw = 0.0;
					for (i = jz; i >= 0; i--)
						fw += fq[i];
					// TODO: drop excess precision here once f64 is used
					fw = (f64)fw;
					y[0] = ih == 0 ? fw : -fw;
					fw = fq[0] - fw;
					for (i = 1; i <= jz; i++)
						fw += fq[i];
					y[1] = ih == 0 ? fw : -fw;
					break;
				case 3:  /* painful */
					for (i = jz; i > 0; i--) {
						fw = fq[i - 1] + fq[i];
						fq[i] += fq[i - 1] - fw;
						fq[i - 1] = fw;
					}
					for (i = jz; i > 1; i--) {
						fw = fq[i - 1] + fq[i];
						fq[i] += fq[i - 1] - fw;
						fq[i - 1] = fw;
					}
					for (fw = 0.0, i = jz; i >= 2; i--)
						fw += fq[i];
					if (ih == 0) {
						y[0] = fq[0]; y[1] = fq[1]; y[2] = fw;
					}
					else {
						y[0] = -fq[0]; y[1] = -fq[1]; y[2] = -fw;
					}
			}
			return n & 7;
		}

		NO_SIDE_EFFECTS constexpr f64 __cos(f64 x, f64 y)noexcept
		{
			static constexpr f64
				C1 = 4.16666666666666019037e-02, /* 0x3FA55555, 0x5555554C */
				C2 = -1.38888888888741095749e-03, /* 0xBF56C16C, 0x16C15177 */
				C3 = 2.48015872894767294178e-05, /* 0x3EFA01A0, 0x19CB1590 */
				C4 = -2.75573143513906633035e-07, /* 0xBE927E4F, 0x809C52AD */
				C5 = 2.08757232129817482790e-09, /* 0x3E21EE9E, 0xBDB4B1C4 */
				C6 = -1.13596475577881948265e-11; /* 0xBDA8FAE9, 0xBE8838D4 */

			f64 hz, z, r, w;

			z = x * x;
			w = z * z;
			r = z * (C1 + z * (C2 + z * C3)) + w * w * (C4 + z * (C5 + z * C6));
			hz = 0.5 * z;
			w = 1.0 - hz;
			return w + (((1.0 - w) - hz) + (z * r - x * y));
		}

		NO_SIDE_EFFECTS constexpr f64 __sin(f64 x, f64 y, i32 iy)noexcept
		{
			static constexpr f64
				S1 = -1.66666666666666324348e-01, /* 0xBFC55555, 0x55555549 */
				S2 = 8.33333333332248946124e-03, /* 0x3F811111, 0x1110F8A6 */
				S3 = -1.98412698298579493134e-04, /* 0xBF2A01A0, 0x19C161D5 */
				S4 = 2.75573137070700676789e-06, /* 0x3EC71DE3, 0x57B1FE7D */
				S5 = -2.50507602534068634195e-08, /* 0xBE5AE5E6, 0x8A2B9CEB */
				S6 = 1.58969099521155010221e-10; /* 0x3DE5D93A, 0x5ACFD57C */

			f64 z, r, v, w;

			z = x * x;
			w = z * z;
			r = S2 + z * (S3 + z * S4) + z * w * (S5 + z * S6);
			v = z * x;
			if (iy == 0)
				return x + v * (S1 + z * r);
			else
				return x - ((z * (0.5 * y - v * r) - y) - v * S1);
		}

		NO_SIDE_EFFECTS constexpr f64 __tan(f64 x, f64 y, i32 odd)noexcept
		{
			static constexpr f64 T[]{
				 3.33333333333334091986e-01, /* 3FD55555, 55555563 */
				 1.33333333333201242699e-01, /* 3FC11111, 1110FE7A */
				 5.39682539762260521377e-02, /* 3FABA1BA, 1BB341FE */
				 2.18694882948595424599e-02, /* 3F9664F4, 8406D637 */
				 8.86323982359930005737e-03, /* 3F8226E3, E96E8493 */
				 3.59207910759131235356e-03, /* 3F6D6D22, C9560328 */
				 1.45620945432529025516e-03, /* 3F57DBC8, FEE08315 */
				 5.88041240820264096874e-04, /* 3F4344D8, F2F26501 */
				 2.46463134818469906812e-04, /* 3F3026F7, 1A8D1068 */
				 7.81794442939557092300e-05, /* 3F147E88, A03792A6 */
				 7.14072491382608190305e-05, /* 3F12B80F, 32F0A7E9 */
				-1.85586374855275456654e-05, /* BEF375CB, DB605373 */
				 2.59073051863633712884e-05, /* 3EFB2A70, 74BF7AD4 */
			},
			pio4 = 7.85398163397448278999e-01, /* 3FE921FB, 54442D18 */
			pio4lo = 3.06161699786838301793e-17; /* 3C81A626, 33145C07 */

			f64 z, r, v, w, s, a;
			f64 w0, a0;
			u32 hx;
			i32 big, sign;

			GET_HIGH_WORD(hx, x);
			big = (hx & 0x7fffffff) >= 0x3FE59428; /* |x| >= 0.6744 */
			if (big) {
				sign = hx >> 31;
				if (sign) {
					x = -x;
					y = -y;
				}
				x = (pio4 - x) + (pio4lo - y);
				y = 0.0;
			}
			z = x * x;
			w = z * z;
			/*
			 * Break x^5*(T[1]+x^2*T[2]+...) into
			 * x^5(T[1]+x^4*T[3]+...+x^20*T[11]) +
			 * x^5(x^2*(T[2]+x^4*T[4]+...+x^22*[T12]))
			 */
			r = T[1] + w * (T[3] + w * (T[5] + w * (T[7] + w * (T[9] + w * T[11]))));
			v = z * (T[2] + w * (T[4] + w * (T[6] + w * (T[8] + w * (T[10] + w * T[12])))));
			s = z * x;
			r = y + z * (s * (r + v) + y) + s * T[0];
			w = x + r;
			if (big) {
				s = 1 - 2 * odd;
				v = s - 2.0 * (x + (r - w * w / (w + s)));
				return sign ? -v : v;
			}
			if (!odd)
				return w;
			/* -1.0/(x+r) has up to 2ulp error, so compute it accurately */
			w0 = w;
			SET_LOW_WORD(w0, 0);
			v = r - (w0 - x);       /* w0+v = r+x */
			a0 = a = -1.0 / w;
			SET_LOW_WORD(a0, 0);
			return a0 + a * (1.0 + a0 * w0 + a0 * v);
		}

		constexpr i32 __rem_pio2(f64 x, f64* y)noexcept
		{
			static constexpr f64 EPS =
#if FLT_EVAL_METHOD==0 || FLT_EVAL_METHOD==1
				DBL_EPSILON;
#elif FLT_EVAL_METHOD==2
				LDBL_EPSILON;
#endif

			static constexpr f64
				toint = 1.5 / EPS,
				pio4 = 0x1.921fb54442d18p-1,
				invpio2 = 6.36619772367581382433e-01, /* 0x3FE45F30, 0x6DC9C883 */
				pio2_1 = 1.57079632673412561417e+00, /* 0x3FF921FB, 0x54400000 */
				pio2_1t = 6.07710050650619224932e-11, /* 0x3DD0B461, 0x1A626331 */
				pio2_2 = 6.07710050630396597660e-11, /* 0x3DD0B461, 0x1A600000 */
				pio2_2t = 2.02226624879595063154e-21, /* 0x3BA3198A, 0x2E037073 */
				pio2_3 = 2.02226624871116645580e-21, /* 0x3BA3198A, 0x2E000000 */
				pio2_3t = 8.47842766036889956997e-32; /* 0x397B839A, 0x252049C1 */

			union { f64 f; u64 i; } u = { x };
			f64 z, w, t, r, fn;
			f64 tx[3], ty[2];
			u32 ix;
			i32 sign, n, ex, ey, i;

			sign = u.i >> 63;
			ix = u.i >> 32 & 0x7fffffff;
			if (ix <= 0x400f6a7a) {  /* |x| ~<= 5pi/4 */
				if ((ix & 0xfffff) == 0x921fb)  /* |x| ~= pi/2 or 2pi/2 */
					goto medium;  /* cancellation -- use medium case */
				if (ix <= 0x4002d97c) {  /* |x| ~<= 3pi/4 */
					if (!sign) {
						z = x - pio2_1;  /* one round good to 85 bits */
						y[0] = z - pio2_1t;
						y[1] = (z - y[0]) - pio2_1t;
						return 1;
					}
					else {
						z = x + pio2_1;
						y[0] = z + pio2_1t;
						y[1] = (z - y[0]) + pio2_1t;
						return -1;
					}
				}
				else {
					if (!sign) {
						z = x - 2 * pio2_1;
						y[0] = z - 2 * pio2_1t;
						y[1] = (z - y[0]) - 2 * pio2_1t;
						return 2;
					}
					else {
						z = x + 2 * pio2_1;
						y[0] = z + 2 * pio2_1t;
						y[1] = (z - y[0]) + 2 * pio2_1t;
						return -2;
					}
				}
			}
			if (ix <= 0x401c463b) {  /* |x| ~<= 9pi/4 */
				if (ix <= 0x4015fdbc) {  /* |x| ~<= 7pi/4 */
					if (ix == 0x4012d97c)  /* |x| ~= 3pi/2 */
						goto medium;
					if (!sign) {
						z = x - 3 * pio2_1;
						y[0] = z - 3 * pio2_1t;
						y[1] = (z - y[0]) - 3 * pio2_1t;
						return 3;
					}
					else {
						z = x + 3 * pio2_1;
						y[0] = z + 3 * pio2_1t;
						y[1] = (z - y[0]) + 3 * pio2_1t;
						return -3;
					}
				}
				else {
					if (ix == 0x401921fb)  /* |x| ~= 4pi/2 */
						goto medium;
					if (!sign) {
						z = x - 4 * pio2_1;
						y[0] = z - 4 * pio2_1t;
						y[1] = (z - y[0]) - 4 * pio2_1t;
						return 4;
					}
					else {
						z = x + 4 * pio2_1;
						y[0] = z + 4 * pio2_1t;
						y[1] = (z - y[0]) + 4 * pio2_1t;
						return -4;
					}
				}
			}
			if (ix < 0x413921fb) {  /* |x| ~< 2^20*(pi/2), medium size */
			medium:
				/* rint(x/(pi/2)) */
				fn = (f64)x * invpio2 + toint - toint;
				n = (i32)fn;
				r = x - fn * pio2_1;
				w = fn * pio2_1t;  /* 1st round, good to 85 bits */
				/* Matters with directed rounding. */
				if (predict_false(r - w < -pio4)) {
					n--;
					fn--;
					r = x - fn * pio2_1;
					w = fn * pio2_1t;
				}
				else if (predict_false(r - w > pio4)) {
					n++;
					fn++;
					r = x - fn * pio2_1;
					w = fn * pio2_1t;
				}
				y[0] = r - w;
				u.f = y[0];
				ey = u.i >> 52 & 0x7ff;
				ex = ix >> 20;
				if (ex - ey > 16) { /* 2nd round, good to 118 bits */
					t = r;
					w = fn * pio2_2;
					r = t - w;
					w = fn * pio2_2t - ((t - r) - w);
					y[0] = r - w;
					u.f = y[0];
					ey = u.i >> 52 & 0x7ff;
					if (ex - ey > 49) {  /* 3rd round, good to 151 bits, covers all cases */
						t = r;
						w = fn * pio2_3;
						r = t - w;
						w = fn * pio2_3t - ((t - r) - w);
						y[0] = r - w;
					}
				}
				y[1] = (r - y[0]) - w;
				return n;
			}
			/*
				* all other (large) arguments
				*/
			if (ix >= 0x7ff00000) {  /* x is inf or NaN */
				y[0] = y[1] = x - x;
				return 0;
			}
			/* set z = scalbn(|x|,-ilogb(x)+23) */
			u.f = x;
			u.i &= (u64)-1 >> 12;
			u.i |= (u64)(0x3ff + 23) << 52;
			z = u.f;
			for (i = 0; i < 2; i++) {
				tx[i] = (f64)(i32)z;
				z = (z - tx[i]) * 0x1p24;
			}
			tx[i] = z;
			/* skip zero terms, first term is non-zero */
			while (tx[i] == 0.0)
				i--;
			n = __rem_pio2_large(tx, ty, (i32)(ix >> 20) - (0x3ff + 23), i + 1, 1);
			if (sign) {
				y[0] = -ty[0];
				y[1] = -ty[1];
				return -n;
			}
			y[0] = ty[0];
			y[1] = ty[1];
			return n;
		}

		NO_SIDE_EFFECTS constexpr f32 __cosdf(f64 x)noexcept
		{
			static constexpr f64
				C0 = -0x1ffffffd0c5e81.0p-54, /* -0.499999997251031003120 */
				C1 = 0x155553e1053a42.0p-57, /*  0.0416666233237390631894 */
				C2 = -0x16c087e80f1e27.0p-62, /* -0.00138867637746099294692 */
				C3 = 0x199342e0ee5069.0p-68; /*  0.0000243904487962774090654 */
			f64 r, w, z;

			/* Try to optimize for parallel evaluation as in __tandf.c. */
			z = x * x;
			w = z * z;
			r = C2 + z * C3;
			return static_cast<f32>(((1.0 + z * C0) + w * C1) + (w * z) * r);
		}

		NO_SIDE_EFFECTS constexpr f32 __sindf(f64 x)noexcept
		{
			static constexpr f64
				S1 = -0x15555554cbac77.0p-55, /* -0.166666666416265235595 */
				S2 = 0x111110896efbb2.0p-59, /*  0.0083333293858894631756 */
				S3 = -0x1a00f9e2cae774.0p-65, /* -0.000198393348360966317347 */
				S4 = 0x16cd878c3b46a7.0p-71; /*  0.0000027183114939898219064 */

			f64 r, s, w, z;

			/* Try to optimize for parallel evaluation as in __tandf.c. */
			z = x * x;
			w = z * z;
			r = S3 + z * S4;
			s = z * x;
			return static_cast<f32>((x + s * (S1 + z * S2)) + s * w * r);
		}

		NO_SIDE_EFFECTS constexpr f32 __tandf(f64 x, i32 odd)noexcept
		{
			static constexpr f64 T[]{
				0x15554d3418c99f.0p-54, /* 0.333331395030791399758 */
				0x1112fd38999f72.0p-55, /* 0.133392002712976742718 */
				0x1b54c91d865afe.0p-57, /* 0.0533812378445670393523 */
				0x191df3908c33ce.0p-58, /* 0.0245283181166547278873 */
				0x185dadfcecf44e.0p-61, /* 0.00297435743359967304927 */
				0x1362b9bf971bcd.0p-59, /* 0.00946564784943673166728 */
			};

			f64 z, r, w, s, t, u;

			z = x * x;
			/*
			 * Split up the polynomial into small independent terms to give
			 * opportunities for parallel evaluation.  The chosen splitting is
			 * micro-optimized for Athlons (XP, X64).  It costs 2 multiplications
			 * relative to Horner's method on sequential machines.
			 *
			 * We add the small terms from lowest degree up for efficiency on
			 * non-sequential machines (the lowest degree terms tend to be ready
			 * earlier).  Apart from this, we don't care about order of
			 * operations, and don't need to to care since we have precision to
			 * spare.  However, the chosen splitting is good for accuracy too,
			 * and would give results as accurate as Horner's method if the
			 * small terms were added from highest degree down.
			 */
			r = T[4] + z * T[5];
			t = T[2] + z * T[3];
			w = z * z;
			s = z * x;
			u = T[0] + z * T[1];
			r = (x + s * u) + (s * w) * (t + w * r);
			return static_cast<f32>(odd ? -1.0 / r : r);
		}

		constexpr i32 __rem_pio2f(f32 x, f64* y)noexcept
		{
			static constexpr f64 EPS =
#if FLT_EVAL_METHOD==0 || FLT_EVAL_METHOD==1
				DBL_EPSILON;
#elif FLT_EVAL_METHOD==2
				LDBL_EPSILON;
#endif

			static constexpr f64
				toint = 1.5 / EPS,
				pio4 = 0x1.921fb6p-1,
				invpio2 = 6.36619772367581382433e-01, /* 0x3FE45F30, 0x6DC9C883 */
				pio2_1 = 1.57079631090164184570e+00, /* 0x3FF921FB, 0x50000000 */
				pio2_1t = 1.58932547735281966916e-08; /* 0x3E5110b4, 0x611A6263 */

			union { f32 f; u32 i; } u = { x };
			f64 tx[1], ty[1];
			f64 fn;
			u32 ix;
			i32 n, sign, e0;

			ix = u.i & 0x7fffffff;
			/* 25+53 bit pi is good enough for medium size */
			if (ix < 0x4dc90fdb) {  /* |x| ~< 2^28*(pi/2), medium size */
				/* Use a specialized rint() to get fn. */
				fn = (f64)x * invpio2 + toint - toint;
				n = (i32)fn;
				*y = x - fn * pio2_1 - fn * pio2_1t;
				/* Matters with directed rounding. */
				if (predict_false(*y < -pio4)) {
					n--;
					fn--;
					*y = x - fn * pio2_1 - fn * pio2_1t;
				}
				else if (predict_false(*y > pio4)) {
					n++;
					fn++;
					*y = x - fn * pio2_1 - fn * pio2_1t;
				}
				return n;
			}
			if (ix >= 0x7f800000) {  /* x is inf or NaN */
				*y = x - x;
				return 0;
			}
			/* scale x into [2^23, 2^24-1] */
			sign = u.i >> 31;
			e0 = (ix >> 23) - (0x7f + 23);  /* e0 = ilogb(|x|)-23, positive */
			u.i = ix - (e0 << 23);
			tx[0] = u.f;
			n = __rem_pio2_large(tx, ty, e0, 1, 0);
			if (sign) {
				*y = -ty[0];
				return -n;
			}
			*y = ty[0];
			return n;
		}
	}

	NO_SIDE_EFFECTS constexpr f64 cos(f64 x)noexcept
	{
		using namespace trig_details;
		f64 y[2];
		u32 ix;
		unsigned n;

		GET_HIGH_WORD(ix, x);
		ix &= 0x7fffffff;

		/* |x| ~< pi/4 */
		if (ix <= 0x3fe921fb) {
			if (ix < 0x3e46a09e) {  /* |x| < 2**-27 * sqrt(2) */
				/* raise inexact if x!=0 */
				FORCE_EVAL(x + 0x1p120f);
				return 1.0;
			}
			return __cos(x, 0);
		}

		/* cos(Inf or NaN) is NaN */
		if (ix >= 0x7ff00000)
			return x - x;

		/* argument reduction */
		n = __rem_pio2(x, y);
		switch (n & 3) {
			case 0: return  __cos(y[0], y[1]);
			case 1: return -__sin(y[0], y[1], 1);
			case 2: return -__cos(y[0], y[1]);
			default:
				return  __sin(y[0], y[1], 1);
		}
	}

	NO_SIDE_EFFECTS constexpr f64 sin(f64 x)noexcept
	{
		using namespace trig_details;
		f64 y[2];
		u32 ix;
		unsigned n;

		/* High word of x. */
		GET_HIGH_WORD(ix, x);
		ix &= 0x7fffffff;

		/* |x| ~< pi/4 */
		if (ix <= 0x3fe921fb) {
			if (ix < 0x3e500000) {  /* |x| < 2**-26 */
				/* raise inexact if x != 0 and underflow if subnormal*/
				FORCE_EVAL(ix < 0x00100000 ? x / 0x1p120f : x + 0x1p120f);
				return x;
			}
			return __sin(x, 0.0, 0);
		}

		/* sin(Inf or NaN) is NaN */
		if (ix >= 0x7ff00000)
			return x - x;

		/* argument reduction needed */
		n = __rem_pio2(x, y);
		switch (n & 3) {
			case 0: return  __sin(y[0], y[1], 1);
			case 1: return  __cos(y[0], y[1]);
			case 2: return -__sin(y[0], y[1], 1);
			default:
				return -__cos(y[0], y[1]);
		}
	}

	NO_SIDE_EFFECTS constexpr f64 tan(f64 x)noexcept
	{
		using namespace trig_details;
		f64 y[2];
		u32 ix;
		unsigned n;

		GET_HIGH_WORD(ix, x);
		ix &= 0x7fffffff;

		/* |x| ~< pi/4 */
		if (ix <= 0x3fe921fb) {
			if (ix < 0x3e400000) { /* |x| < 2**-27 */
				/* raise inexact if x!=0 and underflow if subnormal */
				FORCE_EVAL(ix < 0x00100000 ? x / 0x1p120f : x + 0x1p120f);
				return x;
			}
			return __tan(x, 0.0, 0);
		}

		/* tan(Inf or NaN) is NaN */
		if (ix >= 0x7ff00000)
			return x - x;

		/* argument reduction */
		n = __rem_pio2(x, y);
		return __tan(y[0], y[1], n & 1);
	}

	constexpr void sincos(f64 x, f64* sin, f64* cos)noexcept
	{
		using namespace trig_details;
		f64 y[2], s, c;
		u32 ix;
		unsigned n;

		GET_HIGH_WORD(ix, x);
		ix &= 0x7fffffff;

		/* |x| ~< pi/4 */
		if (ix <= 0x3fe921fb) {
			/* if |x| < 2**-27 * sqrt(2) */
			if (ix < 0x3e46a09e) {
				/* raise inexact if x!=0 and underflow if subnormal */
				FORCE_EVAL(ix < 0x00100000 ? x / 0x1p120f : x + 0x1p120f);
				*sin = x;
				*cos = 1.0;
				return;
			}
			*sin = __sin(x, 0.0, 0);
			*cos = __cos(x, 0.0);
			return;
		}

		/* sincos(Inf or NaN) is NaN */
		if (ix >= 0x7ff00000) {
			*sin = *cos = x - x;
			return;
		}

		/* argument reduction needed */
		n = __rem_pio2(x, y);
		s = __sin(y[0], y[1], 1);
		c = __cos(y[0], y[1]);
		switch (n & 3) {
			case 0:
				*sin = s;
				*cos = c;
				break;
			case 1:
				*sin = c;
				*cos = -s;
				break;
			case 2:
				*sin = -s;
				*cos = -c;
				break;
			case 3:
			default:
				*sin = -c;
				*cos = s;
				break;
		}
	}

	NO_SIDE_EFFECTS constexpr f32 cosf(f32 x)noexcept
	{
		using namespace trig_details;
		static constexpr f64
			c1pio2 = 1 * Math::pi<f64> / 2, /* 0x3FF921FB, 0x54442D18 */
			c2pio2 = 2 * Math::pi<f64> / 2, /* 0x400921FB, 0x54442D18 */
			c3pio2 = 3 * Math::pi<f64> / 2, /* 0x4012D97C, 0x7F3321D2 */
			c4pio2 = 4 * Math::pi<f64> / 2; /* 0x401921FB, 0x54442D18 */

		f64 y;
		u32 ix;
		unsigned n, sign;

		GET_FLOAT_WORD(ix, x);
		sign = ix >> 31;
		ix &= 0x7fffffff;

		if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
			if (ix < 0x39800000) {  /* |x| < 2**-12 */
				/* raise inexact if x != 0 */
				FORCE_EVAL(x + 0x1p120f);
				return 1.0f;
			}
			return __cosdf(x);
		}
		if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
			if (ix > 0x4016cbe3)  /* |x|  ~> 3*pi/4 */
				return -__cosdf(sign ? x + c2pio2 : x - c2pio2);
			else {
				if (sign)
					return __sindf(x + c1pio2);
				else
					return __sindf(c1pio2 - x);
			}
		}
		if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
			if (ix > 0x40afeddf)  /* |x| ~> 7*pi/4 */
				return __cosdf(sign ? x + c4pio2 : x - c4pio2);
			else {
				if (sign)
					return __sindf(-x - c3pio2);
				else
					return __sindf(x - c3pio2);
			}
		}

		/* cos(Inf or NaN) is NaN */
		if (ix >= 0x7f800000)
			return x - x;

		/* general argument reduction needed */
		n = __rem_pio2f(x, &y);
		switch (n & 3) {
			case 0: return  __cosdf(y);
			case 1: return  __sindf(-y);
			case 2: return -__cosdf(y);
			default:
				return  __sindf(y);
		}
	}

	NO_SIDE_EFFECTS constexpr f32 sinf(f32 x)noexcept
	{
		using namespace trig_details;
		static constexpr f64
			s1pio2 = 1 * Math::pi<f64> / 2, /* 0x3FF921FB, 0x54442D18 */
			s2pio2 = 2 * Math::pi<f64> / 2, /* 0x400921FB, 0x54442D18 */
			s3pio2 = 3 * Math::pi<f64> / 2, /* 0x4012D97C, 0x7F3321D2 */
			s4pio2 = 4 * Math::pi<f64> / 2; /* 0x401921FB, 0x54442D18 */

		f64 y;
		u32 ix;
		i32 n, sign;

		GET_FLOAT_WORD(ix, x);
		sign = ix >> 31;
		ix &= 0x7fffffff;

		if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
			if (ix < 0x39800000) {  /* |x| < 2**-12 */
				/* raise inexact if x!=0 and underflow if subnormal */
				FORCE_EVAL(ix < 0x00800000 ? x / 0x1p120f : x + 0x1p120f);
				return x;
			}
			return __sindf(x);
		}
		if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
			if (ix <= 0x4016cbe3) {  /* |x| ~<= 3pi/4 */
				if (sign)
					return -__cosdf(x + s1pio2);
				else
					return __cosdf(x - s1pio2);
			}
			return __sindf(sign ? -(x + s2pio2) : -(x - s2pio2));
		}
		if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
			if (ix <= 0x40afeddf) {  /* |x| ~<= 7*pi/4 */
				if (sign)
					return __cosdf(x + s3pio2);
				else
					return -__cosdf(x - s3pio2);
			}
			return __sindf(sign ? x + s4pio2 : x - s4pio2);
		}

		/* sin(Inf or NaN) is NaN */
		if (ix >= 0x7f800000)
			return x - x;

		/* general argument reduction needed */
		n = __rem_pio2f(x, &y);
		switch (n & 3) {
			case 0: return  __sindf(y);
			case 1: return  __cosdf(y);
			case 2: return  __sindf(-y);
			default:
				return -__cosdf(y);
		}
	}

	constexpr void sincosf(f32 x, f32* sin, f32* cos)noexcept
	{
		using namespace trig_details;
		static constexpr f64 s1pio2 = 1 * Math::pi<f64> / 2; /* 0x3FF921FB, 0x54442D18 */
		static constexpr f64 s2pio2 = 2 * Math::pi<f64> / 2; /* 0x400921FB, 0x54442D18 */
		static constexpr f64 s3pio2 = 3 * Math::pi<f64> / 2; /* 0x4012D97C, 0x7F3321D2 */
		static constexpr f64 s4pio2 = 4 * Math::pi<f64> / 2; /* 0x401921FB, 0x54442D18 */
		f64 y;
		float_t s, c;
		u32 ix;
		unsigned n, sign;

		GET_FLOAT_WORD(ix, x);
		sign = ix >> 31;
		ix &= 0x7fffffff;

		/* |x| ~<= pi/4 */
		if (ix <= 0x3f490fda) {
			/* |x| < 2**-12 */
			if (ix < 0x39800000) {
				/* raise inexact if x!=0 and underflow if subnormal */
				FORCE_EVAL(ix < 0x00100000 ? x / 0x1p120f : x + 0x1p120f);
				*sin = x;
				*cos = 1.0f;
				return;
			}
			*sin = __sindf(x);
			*cos = __cosdf(x);
			return;
		}

		/* |x| ~<= 5*pi/4 */
		if (ix <= 0x407b53d1) {
			if (ix <= 0x4016cbe3) {  /* |x| ~<= 3pi/4 */
				if (sign) {
					*sin = -__cosdf(x + s1pio2);
					*cos = __sindf(x + s1pio2);
				}
				else {
					*sin = __cosdf(s1pio2 - x);
					*cos = __sindf(s1pio2 - x);
				}
				return;
			}
			/* -sin(x+c) is not correct if x+c could be 0: -0 vs +0 */
			*sin = -__sindf(sign ? x + s2pio2 : x - s2pio2);
			*cos = -__cosdf(sign ? x + s2pio2 : x - s2pio2);
			return;
		}

		/* |x| ~<= 9*pi/4 */
		if (ix <= 0x40e231d5) {
			if (ix <= 0x40afeddf) {  /* |x| ~<= 7*pi/4 */
				if (sign) {
					*sin = __cosdf(x + s3pio2);
					*cos = -__sindf(x + s3pio2);
				}
				else {
					*sin = -__cosdf(x - s3pio2);
					*cos = __sindf(x - s3pio2);
				}
				return;
			}
			*sin = __sindf(sign ? x + s4pio2 : x - s4pio2);
			*cos = __cosdf(sign ? x + s4pio2 : x - s4pio2);
			return;
		}

		/* sin(Inf or NaN) is NaN */
		if (ix >= 0x7f800000) {
			*sin = *cos = x - x;
			return;
		}

		/* general argument reduction needed */
		n = __rem_pio2f(x, &y);
		s = __sindf(y);
		c = __cosdf(y);
		switch (n & 3) {
			case 0:
				*sin = s;
				*cos = c;
				break;
			case 1:
				*sin = c;
				*cos = -s;
				break;
			case 2:
				*sin = -s;
				*cos = -c;
				break;
			case 3:
			default:
				*sin = -c;
				*cos = s;
				break;
		}
	}

	NO_SIDE_EFFECTS constexpr f32 tanf(f32 x)noexcept
	{
		using namespace trig_details;
		static constexpr f64
			t1pio2 = 1 * Math::pi<f64> / 2, /* 0x3FF921FB, 0x54442D18 */
			t2pio2 = 2 * Math::pi<f64> / 2, /* 0x400921FB, 0x54442D18 */
			t3pio2 = 3 * Math::pi<f64> / 2, /* 0x4012D97C, 0x7F3321D2 */
			t4pio2 = 4 * Math::pi<f64> / 2; /* 0x401921FB, 0x54442D18 */

		f64 y;
		u32 ix;
		unsigned n, sign;

		GET_FLOAT_WORD(ix, x);
		sign = ix >> 31;
		ix &= 0x7fffffff;

		if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
			if (ix < 0x39800000) {  /* |x| < 2**-12 */
				/* raise inexact if x!=0 and underflow if subnormal */
				FORCE_EVAL(ix < 0x00800000 ? x / 0x1p120f : x + 0x1p120f);
				return x;
			}
			return __tandf(x, 0);
		}
		if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
			if (ix <= 0x4016cbe3)  /* |x| ~<= 3pi/4 */
				return __tandf((sign ? x + t1pio2 : x - t1pio2), 1);
			else
				return __tandf((sign ? x + t2pio2 : x - t2pio2), 0);
		}
		if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
			if (ix <= 0x40afeddf)  /* |x| ~<= 7*pi/4 */
				return __tandf((sign ? x + t3pio2 : x - t3pio2), 1);
			else
				return __tandf((sign ? x + t4pio2 : x - t4pio2), 0);
		}

		/* tan(Inf or NaN) is NaN */
		if (ix >= 0x7f800000)
			return x - x;

		/* argument reduction */
		n = __rem_pio2f(x, &y);
		return __tandf(y, n & 1);
	}
	namespace expf_details
	{
		constexpr i32 EXP2F_TABLE_BITS = 5;
		constexpr i32 EXP2F_POLY_ORDER = 3;
		struct exp2f_data
		{
			u64 tab[1 << EXP2F_TABLE_BITS];
			f64 shift_scaled;
			f64 poly[EXP2F_POLY_ORDER];
			f64 shift;
			f64 invln2_scaled;
			f64 poly_scaled[EXP2F_POLY_ORDER];
		};

		constexpr exp2f_data __exp2f_data
		{
			.tab
			{
				0x3ff0000000000000, 0x3fefd9b0d3158574, 0x3fefb5586cf9890f, 0x3fef9301d0125b51,
				0x3fef72b83c7d517b, 0x3fef54873168b9aa, 0x3fef387a6e756238, 0x3fef1e9df51fdee1,
				0x3fef06fe0a31b715, 0x3feef1a7373aa9cb, 0x3feedea64c123422, 0x3feece086061892d,
				0x3feebfdad5362a27, 0x3feeb42b569d4f82, 0x3feeab07dd485429, 0x3feea47eb03a5585,
				0x3feea09e667f3bcd, 0x3fee9f75e8ec5f74, 0x3feea11473eb0187, 0x3feea589994cce13,
				0x3feeace5422aa0db, 0x3feeb737b0cdc5e5, 0x3feec49182a3f090, 0x3feed503b23e255d,
				0x3feee89f995ad3ad, 0x3feeff76f2fb5e47, 0x3fef199bdd85529c, 0x3fef3720dcef9069,
				0x3fef5818dcfba487, 0x3fef7c97337b9b5f, 0x3fefa4afa2a490da, 0x3fefd0765b6e4540,
			},
			.shift_scaled{ 0x1.8p+52 / (1 << EXP2F_TABLE_BITS) },
			.poly{ 0x1.c6af84b912394p-5, 0x1.ebfce50fac4f3p-3, 0x1.62e42ff0c52d6p-1 },
			.shift{ 0x1.8p+52 },
			.invln2_scaled{ 0x1.71547652b82fep+0 * (1 << EXP2F_TABLE_BITS) },
			.poly_scaled{ 0x1.c6af84b912394p-5 / (1 << EXP2F_TABLE_BITS) / (1 << EXP2F_TABLE_BITS) / (1 << EXP2F_TABLE_BITS), 0x1.ebfce50fac4f3p-3 / (1 << EXP2F_TABLE_BITS) / (1 << EXP2F_TABLE_BITS), 0x1.62e42ff0c52d6p-1 / (1 << EXP2F_TABLE_BITS) },
		};
	}
	namespace powf_details
	{
		constexpr i32 POWF_LOG2_TABLE_BITS = 4;
		constexpr i32 POWF_LOG2_POLY_ORDER = 5;
		constexpr i32 POWF_SCALE_BITS = 0;
		constexpr f64 POWF_SCALE = ((f64)(1 << POWF_SCALE_BITS));

		struct powf_log2_data 
		{
			struct 
			{
				f64 invc, logc;
			} tab[1 << POWF_LOG2_TABLE_BITS];
			f64 poly[POWF_LOG2_POLY_ORDER];
		};

		constexpr powf_log2_data __powf_log2_data
		{
			.tab 
			{
				{ 0x1.661ec79f8f3bep+0, -0x1.efec65b963019p-2 * POWF_SCALE },
				{ 0x1.571ed4aaf883dp+0, -0x1.b0b6832d4fca4p-2 * POWF_SCALE },
				{ 0x1.49539f0f010bp+0, -0x1.7418b0a1fb77bp-2 * POWF_SCALE },
				{ 0x1.3c995b0b80385p+0, -0x1.39de91a6dcf7bp-2 * POWF_SCALE },
				{ 0x1.30d190c8864a5p+0, -0x1.01d9bf3f2b631p-2 * POWF_SCALE },
				{ 0x1.25e227b0b8eap+0, -0x1.97c1d1b3b7afp-3 * POWF_SCALE },
				{ 0x1.1bb4a4a1a343fp+0, -0x1.2f9e393af3c9fp-3 * POWF_SCALE },
				{ 0x1.12358f08ae5bap+0, -0x1.960cbbf788d5cp-4 * POWF_SCALE },
				{ 0x1.0953f419900a7p+0, -0x1.a6f9db6475fcep-5 * POWF_SCALE },
				{ 0x1p+0, 0x0p+0 * POWF_SCALE },
				{ 0x1.e608cfd9a47acp-1, 0x1.338ca9f24f53dp-4 * POWF_SCALE },
				{ 0x1.ca4b31f026aap-1, 0x1.476a9543891bap-3 * POWF_SCALE },
				{ 0x1.b2036576afce6p-1, 0x1.e840b4ac4e4d2p-3 * POWF_SCALE },
				{ 0x1.9c2d163a1aa2dp-1, 0x1.40645f0c6651cp-2 * POWF_SCALE },
				{ 0x1.886e6037841edp-1, 0x1.88e9c2c1b9ff8p-2 * POWF_SCALE },
				{ 0x1.767dcf5534862p-1, 0x1.ce0a44eb17bccp-2 * POWF_SCALE },
			},
			.poly 
			{
				0x1.27616c9496e0bp-2 * POWF_SCALE, -0x1.71969a075c67ap-2 * POWF_SCALE,
				0x1.ec70a6ca7baddp-2 * POWF_SCALE, -0x1.7154748bef6c8p-1 * POWF_SCALE,
				0x1.71547652ab82bp0 * POWF_SCALE,
			}
		};

		NO_SIDE_EFFECTS INLINE constexpr f64 log2_inline(u32 ix)noexcept
		{
			static constexpr i32 N{ 1 << POWF_LOG2_TABLE_BITS };
			static constexpr i32 OFF{ 0x3f330000 };

			f64 z, r, r2, r4, p, q, y, y0, invc, logc;
			u32 iz, top, tmp;
			i32 k, i;

			/* x = 2^k z; where z is in range [OFF,2*OFF] and exact.
			   The range is split into N subintervals.
			   The ith subinterval contains z and c is near its center.  */
			tmp = ix - OFF;
			i = (tmp >> (23 - POWF_LOG2_TABLE_BITS)) % N;
			top = tmp & 0xff800000;
			iz = ix - top;
			k = (i32)top >> (23 - POWF_SCALE_BITS); /* arithmetic shift */
			invc = __powf_log2_data.tab[i].invc;
			logc = __powf_log2_data.tab[i].logc;
			z = (f64)__builtin_bit_cast(f32,iz);

			/* log2(x) = log1p(z/c-1)/ln2 + log2(c) + k */
			r = z * invc - 1;
			y0 = logc + (f64)k;

			/* Pipelined polynomial evaluation to approximate log1p(r)/ln2.  */
			r2 = r * r;
			y = __powf_log2_data.poly[0] * r + __powf_log2_data.poly[1];
			p = __powf_log2_data.poly[2] * r + __powf_log2_data.poly[3];
			r4 = r2 * r2;
			q = __powf_log2_data.poly[4] * r + y0;
			q = p * r2 + q;
			y = y * r4 + q;
			return y;
		}

		NO_SIDE_EFFECTS INLINE constexpr f32 exp2_inline(f64 xd, u32 sign_bias)noexcept
		{
			using namespace expf_details;
			static constexpr i32 N{ 1 << EXP2F_TABLE_BITS };

			u64 ki, ski, t;
			f64 kd, z, r, r2, y, s;

			/* x = k/N + r with r in [-1/(2N), 1/(2N)] */
			kd = xd + __exp2f_data.shift_scaled;
			ki = BITCAST(u64,kd);
			kd -= __exp2f_data.shift_scaled; /* k/N */
			r = xd - kd;

			/* exp2(x) = 2^(k/N) * 2^r ~= s * (C0*r^3 + C1*r^2 + C2*r + 1) */
			t = __exp2f_data.tab[ki % N];
			ski = ki + sign_bias;
			t += ski << (52 - EXP2F_TABLE_BITS);
			s = __builtin_bit_cast(f64,t);
			z = __exp2f_data.poly[0] * r + __exp2f_data.poly[1];
			r2 = r * r;
			y = __exp2f_data.poly[2] * r + 1;
			y = z * r2 + y;
			y = y * s;
			return static_cast<f32>(y);
		}

		//Returns 0 if not i32, 1 if odd i32, 2 if even i32.  The argument is the bit representation of a non-zero finite floating-point value
		NO_SIDE_EFFECTS INLINE constexpr i32 checkint(u32 iy)noexcept
		{
			i32 e = iy >> 23 & 0xff;
			if (e < 0x7f)
				return 0;
			if (e > 0x7f + 23)
				return 2;
			if (iy & ((1 << (0x7f + 23 - e)) - 1))
				return 0;
			if (iy & (1 << (0x7f + 23 - e)))
				return 1;
			return 2;
		}

		NO_SIDE_EFFECTS INLINE constexpr i32 zeroinfnan(u32 ix)noexcept { return 2 * ix - 1 >= 2u * 0x7f800000 - 1; }
	}

	NO_SIDE_EFFECTS constexpr f32 powf(f32 x, f32 y)noexcept
	{
		using namespace powf_details;
		using namespace expf_details;
		u32 sign_bias = 0;
		u32 ix, iy;

		ix = BITCAST(u32,x);
		iy = BITCAST(u32,y);
		if (predict_false(ix - 0x00800000 >= 0x7f800000 - 0x00800000 || zeroinfnan(iy))) {
			/* Either (x < 0x1p-126 or inf or nan) or (y is 0 or inf or nan).  */
			if (predict_false(zeroinfnan(iy))) {
				if (2 * iy == 0)
					return 1.0f;
				if (ix == 0x3f800000)
					return 1.0f;
				if (2 * ix > 2u * 0x7f800000 ||
					2 * iy > 2u * 0x7f800000)
					return x + y;
				if (2 * ix == 2 * 0x3f800000)
					return 1.0f;
				if ((2 * ix < 2 * 0x3f800000) == !(iy & 0x80000000))
					return 0.0f; /* |x|<1 && y==inf or |x|>1 && y==-inf.  */
				return y * y;
			}
			if (predict_false(zeroinfnan(ix))) {
				float_t x2 = x * x;
				if (ix & 0x80000000 && checkint(iy) == 1)
					x2 = -x2;
				/* Without the barrier some versions of clang hoist the 1/x2 and
				   thus division by zero exception can be signaled spuriously.  */
				return iy & 0x80000000 ? (1 / x2) : x2;
			}
			/* x and y are non-zero finite.  */
			if (ix & 0x80000000) {
				/* Finite x < 0.  */
				i32 yint = checkint(iy);
				if (yint == 0)
					return std::numeric_limits<f32>::quiet_NaN();
				if (yint == 1)
					sign_bias = (1 << (EXP2F_TABLE_BITS + 11));
				ix &= 0x7fffffff;
			}
			if (ix < 0x00800000) {
				/* Normalize subnormal x so exponent becomes negative.  */
				ix = BITCAST(u32,x * 0x1p23f);
				ix &= 0x7fffffff;
				ix -= 23 << 23;
			}
		}
		f64 logx = log2_inline(ix);
		f64 ylogx = y * logx; /* cannot overflow, y is single prec.  */
		if (predict_false((BITCAST(u64,ylogx) >> 47 & 0xffff) >= BITCAST(u64,126.0 * POWF_SCALE) >> 47)) {
			/* |y*log(x)| >= 126.  */
			if (ylogx > 0x1.fffffffd1d571p+6 * POWF_SCALE)
				return __math_oflowf(sign_bias);
			if (ylogx <= -150.0 * POWF_SCALE)
				return __math_uflowf(sign_bias);
		}
		return exp2_inline(ylogx, sign_bias);
	}

	namespace exp_details
	{
		constexpr i32 EXP_TABLE_BITS = 7;
		constexpr i32 EXP_POLY_ORDER = 5;
		constexpr i32 EXP2_POLY_ORDER = 5;
		struct exp_data
		{
			f64 invln2N;
			f64 shift;
			f64 negln2hiN;
			f64 negln2loN;
			f64 poly[4]; /* Last four coefficients.  */
			f64 exp2_shift;
			f64 exp2_poly[EXP2_POLY_ORDER];
			u64 tab[2 * (1 << EXP_TABLE_BITS)];
		};

		constexpr exp_data __exp_data
		{
			// N/ln2
			.invln2N{ 0x1.71547652b82fep0 * (1 << EXP_TABLE_BITS) },
			.shift{ 0x1.8p52 },
			// -ln2/N
			.negln2hiN{ -0x1.62e42fefa0000p-8 },
			.negln2loN{ -0x1.cf79abc9e3b3ap-47 },
			// exp polynomial coefficients.
			.poly
			{
				// abs error: 1.555*2^-66
				// ulp error: 0.509 (0.511 without fma)
				// if |x| < ln2/256+eps
				// abs error if |x| < ln2/256+0x1p-15: 1.09*2^-65
				// abs error if |x| < ln2/128: 1.7145*2^-56
				0x1.ffffffffffdbdp-2,
				0x1.555555555543cp-3,
				0x1.55555cf172b91p-5,
				0x1.1111167a4d017p-7,
			},
			.exp2_shift{ 0x1.8p52 / (1 << EXP_TABLE_BITS) },
			// exp2 polynomial coefficients.
			.exp2_poly
			{
				// abs error: 1.2195*2^-65
				// ulp error: 0.507 (0.511 without fma)
				// if |x| < 1/256
				// abs error if |x| < 1/128: 1.9941*2^-56
				0x1.62e42fefa39efp-1,
				0x1.ebfbdff82c424p-3,
				0x1.c6b08d70cf4b5p-5,
				0x1.3b2abd24650ccp-7,
				0x1.5d7e09b4e3a84p-10,
			},
			// 2^(k/N) ~= H[k]*(1 + T[k]) for i32 k in [0,N)
			// tab[2*k] = __builtin_bit_cast(u64, T[k])
			// tab[2*k+1] = __builtin_bit_cast(u64, H[k]) - (k << 52)/N
			.tab
			{
				0x0, 0x3ff0000000000000,
				0x3c9b3b4f1a88bf6e, 0x3feff63da9fb3335,
				0xbc7160139cd8dc5d, 0x3fefec9a3e778061,
				0xbc905e7a108766d1, 0x3fefe315e86e7f85,
				0x3c8cd2523567f613, 0x3fefd9b0d3158574,
				0xbc8bce8023f98efa, 0x3fefd06b29ddf6de,
				0x3c60f74e61e6c861, 0x3fefc74518759bc8,
				0x3c90a3e45b33d399, 0x3fefbe3ecac6f383,
				0x3c979aa65d837b6d, 0x3fefb5586cf9890f,
				0x3c8eb51a92fdeffc, 0x3fefac922b7247f7,
				0x3c3ebe3d702f9cd1, 0x3fefa3ec32d3d1a2,
				0xbc6a033489906e0b, 0x3fef9b66affed31b,
				0xbc9556522a2fbd0e, 0x3fef9301d0125b51,
				0xbc5080ef8c4eea55, 0x3fef8abdc06c31cc,
				0xbc91c923b9d5f416, 0x3fef829aaea92de0,
				0x3c80d3e3e95c55af, 0x3fef7a98c8a58e51,
				0xbc801b15eaa59348, 0x3fef72b83c7d517b,
				0xbc8f1ff055de323d, 0x3fef6af9388c8dea,
				0x3c8b898c3f1353bf, 0x3fef635beb6fcb75,
				0xbc96d99c7611eb26, 0x3fef5be084045cd4,
				0x3c9aecf73e3a2f60, 0x3fef54873168b9aa,
				0xbc8fe782cb86389d, 0x3fef4d5022fcd91d,
				0x3c8a6f4144a6c38d, 0x3fef463b88628cd6,
				0x3c807a05b0e4047d, 0x3fef3f49917ddc96,
				0x3c968efde3a8a894, 0x3fef387a6e756238,
				0x3c875e18f274487d, 0x3fef31ce4fb2a63f,
				0x3c80472b981fe7f2, 0x3fef2b4565e27cdd,
				0xbc96b87b3f71085e, 0x3fef24dfe1f56381,
				0x3c82f7e16d09ab31, 0x3fef1e9df51fdee1,
				0xbc3d219b1a6fbffa, 0x3fef187fd0dad990,
				0x3c8b3782720c0ab4, 0x3fef1285a6e4030b,
				0x3c6e149289cecb8f, 0x3fef0cafa93e2f56,
				0x3c834d754db0abb6, 0x3fef06fe0a31b715,
				0x3c864201e2ac744c, 0x3fef0170fc4cd831,
				0x3c8fdd395dd3f84a, 0x3feefc08b26416ff,
				0xbc86a3803b8e5b04, 0x3feef6c55f929ff1,
				0xbc924aedcc4b5068, 0x3feef1a7373aa9cb,
				0xbc9907f81b512d8e, 0x3feeecae6d05d866,
				0xbc71d1e83e9436d2, 0x3feee7db34e59ff7,
				0xbc991919b3ce1b15, 0x3feee32dc313a8e5,
				0x3c859f48a72a4c6d, 0x3feedea64c123422,
				0xbc9312607a28698a, 0x3feeda4504ac801c,
				0xbc58a78f4817895b, 0x3feed60a21f72e2a,
				0xbc7c2c9b67499a1b, 0x3feed1f5d950a897,
				0x3c4363ed60c2ac11, 0x3feece086061892d,
				0x3c9666093b0664ef, 0x3feeca41ed1d0057,
				0x3c6ecce1daa10379, 0x3feec6a2b5c13cd0,
				0x3c93ff8e3f0f1230, 0x3feec32af0d7d3de,
				0x3c7690cebb7aafb0, 0x3feebfdad5362a27,
				0x3c931dbdeb54e077, 0x3feebcb299fddd0d,
				0xbc8f94340071a38e, 0x3feeb9b2769d2ca7,
				0xbc87deccdc93a349, 0x3feeb6daa2cf6642,
				0xbc78dec6bd0f385f, 0x3feeb42b569d4f82,
				0xbc861246ec7b5cf6, 0x3feeb1a4ca5d920f,
				0x3c93350518fdd78e, 0x3feeaf4736b527da,
				0x3c7b98b72f8a9b05, 0x3feead12d497c7fd,
				0x3c9063e1e21c5409, 0x3feeab07dd485429,
				0x3c34c7855019c6ea, 0x3feea9268a5946b7,
				0x3c9432e62b64c035, 0x3feea76f15ad2148,
				0xbc8ce44a6199769f, 0x3feea5e1b976dc09,
				0xbc8c33c53bef4da8, 0x3feea47eb03a5585,
				0xbc845378892be9ae, 0x3feea34634ccc320,
				0xbc93cedd78565858, 0x3feea23882552225,
				0x3c5710aa807e1964, 0x3feea155d44ca973,
				0xbc93b3efbf5e2228, 0x3feea09e667f3bcd,
				0xbc6a12ad8734b982, 0x3feea012750bdabf,
				0xbc6367efb86da9ee, 0x3fee9fb23c651a2f,
				0xbc80dc3d54e08851, 0x3fee9f7df9519484,
				0xbc781f647e5a3ecf, 0x3fee9f75e8ec5f74,
				0xbc86ee4ac08b7db0, 0x3fee9f9a48a58174,
				0xbc8619321e55e68a, 0x3fee9feb564267c9,
				0x3c909ccb5e09d4d3, 0x3feea0694fde5d3f,
				0xbc7b32dcb94da51d, 0x3feea11473eb0187,
				0x3c94ecfd5467c06b, 0x3feea1ed0130c132,
				0x3c65ebe1abd66c55, 0x3feea2f336cf4e62,
				0xbc88a1c52fb3cf42, 0x3feea427543e1a12,
				0xbc9369b6f13b3734, 0x3feea589994cce13,
				0xbc805e843a19ff1e, 0x3feea71a4623c7ad,
				0xbc94d450d872576e, 0x3feea8d99b4492ed,
				0x3c90ad675b0e8a00, 0x3feeaac7d98a6699,
				0x3c8db72fc1f0eab4, 0x3feeace5422aa0db,
				0xbc65b6609cc5e7ff, 0x3feeaf3216b5448c,
				0x3c7bf68359f35f44, 0x3feeb1ae99157736,
				0xbc93091fa71e3d83, 0x3feeb45b0b91ffc6,
				0xbc5da9b88b6c1e29, 0x3feeb737b0cdc5e5,
				0xbc6c23f97c90b959, 0x3feeba44cbc8520f,
				0xbc92434322f4f9aa, 0x3feebd829fde4e50,
				0xbc85ca6cd7668e4b, 0x3feec0f170ca07ba,
				0x3c71affc2b91ce27, 0x3feec49182a3f090,
				0x3c6dd235e10a73bb, 0x3feec86319e32323,
				0xbc87c50422622263, 0x3feecc667b5de565,
				0x3c8b1c86e3e231d5, 0x3feed09bec4a2d33,
				0xbc91bbd1d3bcbb15, 0x3feed503b23e255d,
				0x3c90cc319cee31d2, 0x3feed99e1330b358,
				0x3c8469846e735ab3, 0x3feede6b5579fdbf,
				0xbc82dfcd978e9db4, 0x3feee36bbfd3f37a,
				0x3c8c1a7792cb3387, 0x3feee89f995ad3ad,
				0xbc907b8f4ad1d9fa, 0x3feeee07298db666,
				0xbc55c3d956dcaeba, 0x3feef3a2b84f15fb,
				0xbc90a40e3da6f640, 0x3feef9728de5593a,
				0xbc68d6f438ad9334, 0x3feeff76f2fb5e47,
				0xbc91eee26b588a35, 0x3fef05b030a1064a,
				0x3c74ffd70a5fddcd, 0x3fef0c1e904bc1d2,
				0xbc91bdfbfa9298ac, 0x3fef12c25bd71e09,
				0x3c736eae30af0cb3, 0x3fef199bdd85529c,
				0x3c8ee3325c9ffd94, 0x3fef20ab5fffd07a,
				0x3c84e08fd10959ac, 0x3fef27f12e57d14b,
				0x3c63cdaf384e1a67, 0x3fef2f6d9406e7b5,
				0x3c676b2c6c921968, 0x3fef3720dcef9069,
				0xbc808a1883ccb5d2, 0x3fef3f0b555dc3fa,
				0xbc8fad5d3ffffa6f, 0x3fef472d4a07897c,
				0xbc900dae3875a949, 0x3fef4f87080d89f2,
				0x3c74a385a63d07a7, 0x3fef5818dcfba487,
				0xbc82919e2040220f, 0x3fef60e316c98398,
				0x3c8e5a50d5c192ac, 0x3fef69e603db3285,
				0x3c843a59ac016b4b, 0x3fef7321f301b460,
				0xbc82d52107b43e1f, 0x3fef7c97337b9b5f,
				0xbc892ab93b470dc9, 0x3fef864614f5a129,
				0x3c74b604603a88d3, 0x3fef902ee78b3ff6,
				0x3c83c5ec519d7271, 0x3fef9a51fbc74c83,
				0xbc8ff7128fd391f0, 0x3fefa4afa2a490da,
				0xbc8dae98e223747d, 0x3fefaf482d8e67f1,
				0x3c8ec3bc41aa2008, 0x3fefba1bee615a27,
				0x3c842b94c3a9eb32, 0x3fefc52b376bba97,
				0x3c8a64a931d185ee, 0x3fefd0765b6e4540,
				0xbc8e37bae43be3ed, 0x3fefdbfdad9cbe14,
				0x3c77893b4d91cd9d, 0x3fefe7c1819e90d8,
				0x3c5305c14160cc89, 0x3feff3c22b8f71f1,
			},
		};
	}

	namespace pow_details
	{
		constexpr i32 POW_LOG_TABLE_BITS = 7;
		constexpr i32 POW_LOG_POLY_ORDER = 8;
		struct pow_log_data 
		{
			f64 ln2hi;
			f64 ln2lo;
			f64 poly[POW_LOG_POLY_ORDER - 1]; /* First coefficient is 1.  */
			/* Note: the pad field is unused, but allows slightly faster indexing.  */
			struct 
			{
				f64 invc, pad, logc, logctail;
			} tab[1 << POW_LOG_TABLE_BITS];
		};
		//#define N (1 << POW_LOG_TABLE_BITS)

		constexpr pow_log_data __pow_log_data
		{
			.ln2hi{ 0x1.62e42fefa3800p-1 },
			.ln2lo{ 0x1.ef35793c76730p-45 },
			.poly
			{
				-0x1p-1,
				0x1.555555555556p-2 * -2,
				-0x1.0000000000006p-2 * -2,
				0x1.999999959554ep-3 * 4,
				-0x1.555555529a47ap-3 * 4,
				0x1.2495b9b4845e9p-3 * -8,
				-0x1.0002b8b263fc3p-3 * -8,
			},
			.tab 
			{
				{0x1.6a00000000000p+0,0, -0x1.62c82f2b9c800p-2, 0x1.ab42428375680p-48},
				{0x1.6800000000000p+0,0, -0x1.5d1bdbf580800p-2, -0x1.ca508d8e0f720p-46},
				{0x1.6600000000000p+0,0, -0x1.5767717455800p-2, -0x1.362a4d5b6506dp-45},
				{0x1.6400000000000p+0,0, -0x1.51aad872df800p-2, -0x1.684e49eb067d5p-49},
				{0x1.6200000000000p+0,0, -0x1.4be5f95777800p-2, -0x1.41b6993293ee0p-47},
				{0x1.6000000000000p+0,0, -0x1.4618bc21c6000p-2, 0x1.3d82f484c84ccp-46},
				{0x1.5e00000000000p+0,0, -0x1.404308686a800p-2, 0x1.c42f3ed820b3ap-50},
				{0x1.5c00000000000p+0,0, -0x1.3a64c55694800p-2, 0x1.0b1c686519460p-45},
				{0x1.5a00000000000p+0,0, -0x1.347dd9a988000p-2, 0x1.5594dd4c58092p-45},
				{0x1.5800000000000p+0,0, -0x1.2e8e2bae12000p-2, 0x1.67b1e99b72bd8p-45},
				{0x1.5600000000000p+0,0, -0x1.2895a13de8800p-2, 0x1.5ca14b6cfb03fp-46},
				{0x1.5600000000000p+0,0, -0x1.2895a13de8800p-2, 0x1.5ca14b6cfb03fp-46},
				{0x1.5400000000000p+0,0, -0x1.22941fbcf7800p-2, -0x1.65a242853da76p-46},
				{0x1.5200000000000p+0,0, -0x1.1c898c1699800p-2, -0x1.fafbc68e75404p-46},
				{0x1.5000000000000p+0,0, -0x1.1675cababa800p-2, 0x1.f1fc63382a8f0p-46},
				{0x1.4e00000000000p+0,0, -0x1.1058bf9ae4800p-2, -0x1.6a8c4fd055a66p-45},
				{0x1.4c00000000000p+0,0, -0x1.0a324e2739000p-2, -0x1.c6bee7ef4030ep-47},
				{0x1.4a00000000000p+0,0, -0x1.0402594b4d000p-2, -0x1.036b89ef42d7fp-48},
				{0x1.4a00000000000p+0,0, -0x1.0402594b4d000p-2, -0x1.036b89ef42d7fp-48},
				{0x1.4800000000000p+0,0, -0x1.fb9186d5e4000p-3, 0x1.d572aab993c87p-47},
				{0x1.4600000000000p+0,0, -0x1.ef0adcbdc6000p-3, 0x1.b26b79c86af24p-45},
				{0x1.4400000000000p+0,0, -0x1.e27076e2af000p-3, -0x1.72f4f543fff10p-46},
				{0x1.4200000000000p+0,0, -0x1.d5c216b4fc000p-3, 0x1.1ba91bbca681bp-45},
				{0x1.4000000000000p+0,0, -0x1.c8ff7c79aa000p-3, 0x1.7794f689f8434p-45},
				{0x1.4000000000000p+0,0, -0x1.c8ff7c79aa000p-3, 0x1.7794f689f8434p-45},
				{0x1.3e00000000000p+0,0, -0x1.bc286742d9000p-3, 0x1.94eb0318bb78fp-46},
				{0x1.3c00000000000p+0,0, -0x1.af3c94e80c000p-3, 0x1.a4e633fcd9066p-52},
				{0x1.3a00000000000p+0,0, -0x1.a23bc1fe2b000p-3, -0x1.58c64dc46c1eap-45},
				{0x1.3a00000000000p+0,0, -0x1.a23bc1fe2b000p-3, -0x1.58c64dc46c1eap-45},
				{0x1.3800000000000p+0,0, -0x1.9525a9cf45000p-3, -0x1.ad1d904c1d4e3p-45},
				{0x1.3600000000000p+0,0, -0x1.87fa06520d000p-3, 0x1.bbdbf7fdbfa09p-45},
				{0x1.3400000000000p+0,0, -0x1.7ab890210e000p-3, 0x1.bdb9072534a58p-45},
				{0x1.3400000000000p+0,0, -0x1.7ab890210e000p-3, 0x1.bdb9072534a58p-45},
				{0x1.3200000000000p+0,0, -0x1.6d60fe719d000p-3, -0x1.0e46aa3b2e266p-46},
				{0x1.3000000000000p+0,0, -0x1.5ff3070a79000p-3, -0x1.e9e439f105039p-46},
				{0x1.3000000000000p+0,0, -0x1.5ff3070a79000p-3, -0x1.e9e439f105039p-46},
				{0x1.2e00000000000p+0,0, -0x1.526e5e3a1b000p-3, -0x1.0de8b90075b8fp-45},
				{0x1.2c00000000000p+0,0, -0x1.44d2b6ccb8000p-3, 0x1.70cc16135783cp-46},
				{0x1.2c00000000000p+0,0, -0x1.44d2b6ccb8000p-3, 0x1.70cc16135783cp-46},
				{0x1.2a00000000000p+0,0, -0x1.371fc201e9000p-3, 0x1.178864d27543ap-48},
				{0x1.2800000000000p+0,0, -0x1.29552f81ff000p-3, -0x1.48d301771c408p-45},
				{0x1.2600000000000p+0,0, -0x1.1b72ad52f6000p-3, -0x1.e80a41811a396p-45},
				{0x1.2600000000000p+0,0, -0x1.1b72ad52f6000p-3, -0x1.e80a41811a396p-45},
				{0x1.2400000000000p+0,0, -0x1.0d77e7cd09000p-3, 0x1.a699688e85bf4p-47},
				{0x1.2400000000000p+0,0, -0x1.0d77e7cd09000p-3, 0x1.a699688e85bf4p-47},
				{0x1.2200000000000p+0,0, -0x1.fec9131dbe000p-4, -0x1.575545ca333f2p-45},
				{0x1.2000000000000p+0,0, -0x1.e27076e2b0000p-4, 0x1.a342c2af0003cp-45},
				{0x1.2000000000000p+0,0, -0x1.e27076e2b0000p-4, 0x1.a342c2af0003cp-45},
				{0x1.1e00000000000p+0,0, -0x1.c5e548f5bc000p-4, -0x1.d0c57585fbe06p-46},
				{0x1.1c00000000000p+0,0, -0x1.a926d3a4ae000p-4, 0x1.53935e85baac8p-45},
				{0x1.1c00000000000p+0,0, -0x1.a926d3a4ae000p-4, 0x1.53935e85baac8p-45},
				{0x1.1a00000000000p+0,0, -0x1.8c345d631a000p-4, 0x1.37c294d2f5668p-46},
				{0x1.1a00000000000p+0,0, -0x1.8c345d631a000p-4, 0x1.37c294d2f5668p-46},
				{0x1.1800000000000p+0,0, -0x1.6f0d28ae56000p-4, -0x1.69737c93373dap-45},
				{0x1.1600000000000p+0,0, -0x1.51b073f062000p-4, 0x1.f025b61c65e57p-46},
				{0x1.1600000000000p+0,0, -0x1.51b073f062000p-4, 0x1.f025b61c65e57p-46},
				{0x1.1400000000000p+0,0, -0x1.341d7961be000p-4, 0x1.c5edaccf913dfp-45},
				{0x1.1400000000000p+0,0, -0x1.341d7961be000p-4, 0x1.c5edaccf913dfp-45},
				{0x1.1200000000000p+0,0, -0x1.16536eea38000p-4, 0x1.47c5e768fa309p-46},
				{0x1.1000000000000p+0,0, -0x1.f0a30c0118000p-5, 0x1.d599e83368e91p-45},
				{0x1.1000000000000p+0,0, -0x1.f0a30c0118000p-5, 0x1.d599e83368e91p-45},
				{0x1.0e00000000000p+0,0, -0x1.b42dd71198000p-5, 0x1.c827ae5d6704cp-46},
				{0x1.0e00000000000p+0,0, -0x1.b42dd71198000p-5, 0x1.c827ae5d6704cp-46},
				{0x1.0c00000000000p+0,0, -0x1.77458f632c000p-5, -0x1.cfc4634f2a1eep-45},
				{0x1.0c00000000000p+0,0, -0x1.77458f632c000p-5, -0x1.cfc4634f2a1eep-45},
				{0x1.0a00000000000p+0,0, -0x1.39e87b9fec000p-5, 0x1.502b7f526feaap-48},
				{0x1.0a00000000000p+0,0, -0x1.39e87b9fec000p-5, 0x1.502b7f526feaap-48},
				{0x1.0800000000000p+0,0, -0x1.f829b0e780000p-6, -0x1.980267c7e09e4p-45},
				{0x1.0800000000000p+0,0, -0x1.f829b0e780000p-6, -0x1.980267c7e09e4p-45},
				{0x1.0600000000000p+0,0, -0x1.7b91b07d58000p-6, -0x1.88d5493faa639p-45},
				{0x1.0400000000000p+0,0, -0x1.fc0a8b0fc0000p-7, -0x1.f1e7cf6d3a69cp-50},
				{0x1.0400000000000p+0,0, -0x1.fc0a8b0fc0000p-7, -0x1.f1e7cf6d3a69cp-50},
				{0x1.0200000000000p+0,0, -0x1.fe02a6b100000p-8, -0x1.9e23f0dda40e4p-46},
				{0x1.0200000000000p+0,0, -0x1.fe02a6b100000p-8, -0x1.9e23f0dda40e4p-46},
				{0x1.0000000000000p+0,0, 0x0.0000000000000p+0, 0x0.0000000000000p+0},
				{0x1.0000000000000p+0,0, 0x0.0000000000000p+0, 0x0.0000000000000p+0},
				{0x1.fc00000000000p-1,0, 0x1.0101575890000p-7, -0x1.0c76b999d2be8p-46},
				{0x1.f800000000000p-1,0, 0x1.0205658938000p-6, -0x1.3dc5b06e2f7d2p-45},
				{0x1.f400000000000p-1,0, 0x1.8492528c90000p-6, -0x1.aa0ba325a0c34p-45},
				{0x1.f000000000000p-1,0, 0x1.0415d89e74000p-5, 0x1.111c05cf1d753p-47},
				{0x1.ec00000000000p-1,0, 0x1.466aed42e0000p-5, -0x1.c167375bdfd28p-45},
				{0x1.e800000000000p-1,0, 0x1.894aa149fc000p-5, -0x1.97995d05a267dp-46},
				{0x1.e400000000000p-1,0, 0x1.ccb73cdddc000p-5, -0x1.a68f247d82807p-46},
				{0x1.e200000000000p-1,0, 0x1.eea31c006c000p-5, -0x1.e113e4fc93b7bp-47},
				{0x1.de00000000000p-1,0, 0x1.1973bd1466000p-4, -0x1.5325d560d9e9bp-45},
				{0x1.da00000000000p-1,0, 0x1.3bdf5a7d1e000p-4, 0x1.cc85ea5db4ed7p-45},
				{0x1.d600000000000p-1,0, 0x1.5e95a4d97a000p-4, -0x1.c69063c5d1d1ep-45},
				{0x1.d400000000000p-1,0, 0x1.700d30aeac000p-4, 0x1.c1e8da99ded32p-49},
				{0x1.d000000000000p-1,0, 0x1.9335e5d594000p-4, 0x1.3115c3abd47dap-45},
				{0x1.cc00000000000p-1,0, 0x1.b6ac88dad6000p-4, -0x1.390802bf768e5p-46},
				{0x1.ca00000000000p-1,0, 0x1.c885801bc4000p-4, 0x1.646d1c65aacd3p-45},
				{0x1.c600000000000p-1,0, 0x1.ec739830a2000p-4, -0x1.dc068afe645e0p-45},
				{0x1.c400000000000p-1,0, 0x1.fe89139dbe000p-4, -0x1.534d64fa10afdp-45},
				{0x1.c000000000000p-1,0, 0x1.1178e8227e000p-3, 0x1.1ef78ce2d07f2p-45},
				{0x1.be00000000000p-1,0, 0x1.1aa2b7e23f000p-3, 0x1.ca78e44389934p-45},
				{0x1.ba00000000000p-1,0, 0x1.2d1610c868000p-3, 0x1.39d6ccb81b4a1p-47},
				{0x1.b800000000000p-1,0, 0x1.365fcb0159000p-3, 0x1.62fa8234b7289p-51},
				{0x1.b400000000000p-1,0, 0x1.4913d8333b000p-3, 0x1.5837954fdb678p-45},
				{0x1.b200000000000p-1,0, 0x1.527e5e4a1b000p-3, 0x1.633e8e5697dc7p-45},
				{0x1.ae00000000000p-1,0, 0x1.6574ebe8c1000p-3, 0x1.9cf8b2c3c2e78p-46},
				{0x1.ac00000000000p-1,0, 0x1.6f0128b757000p-3, -0x1.5118de59c21e1p-45},
				{0x1.aa00000000000p-1,0, 0x1.7898d85445000p-3, -0x1.c661070914305p-46},
				{0x1.a600000000000p-1,0, 0x1.8beafeb390000p-3, -0x1.73d54aae92cd1p-47},
				{0x1.a400000000000p-1,0, 0x1.95a5adcf70000p-3, 0x1.7f22858a0ff6fp-47},
				{0x1.a000000000000p-1,0, 0x1.a93ed3c8ae000p-3, -0x1.8724350562169p-45},
				{0x1.9e00000000000p-1,0, 0x1.b31d8575bd000p-3, -0x1.c358d4eace1aap-47},
				{0x1.9c00000000000p-1,0, 0x1.bd087383be000p-3, -0x1.d4bc4595412b6p-45},
				{0x1.9a00000000000p-1,0, 0x1.c6ffbc6f01000p-3, -0x1.1ec72c5962bd2p-48},
				{0x1.9600000000000p-1,0, 0x1.db13db0d49000p-3, -0x1.aff2af715b035p-45},
				{0x1.9400000000000p-1,0, 0x1.e530effe71000p-3, 0x1.212276041f430p-51},
				{0x1.9200000000000p-1,0, 0x1.ef5ade4dd0000p-3, -0x1.a211565bb8e11p-51},
				{0x1.9000000000000p-1,0, 0x1.f991c6cb3b000p-3, 0x1.bcbecca0cdf30p-46},
				{0x1.8c00000000000p-1,0, 0x1.07138604d5800p-2, 0x1.89cdb16ed4e91p-48},
				{0x1.8a00000000000p-1,0, 0x1.0c42d67616000p-2, 0x1.7188b163ceae9p-45},
				{0x1.8800000000000p-1,0, 0x1.1178e8227e800p-2, -0x1.c210e63a5f01cp-45},
				{0x1.8600000000000p-1,0, 0x1.16b5ccbacf800p-2, 0x1.b9acdf7a51681p-45},
				{0x1.8400000000000p-1,0, 0x1.1bf99635a6800p-2, 0x1.ca6ed5147bdb7p-45},
				{0x1.8200000000000p-1,0, 0x1.214456d0eb800p-2, 0x1.a87deba46baeap-47},
				{0x1.7e00000000000p-1,0, 0x1.2bef07cdc9000p-2, 0x1.a9cfa4a5004f4p-45},
				{0x1.7c00000000000p-1,0, 0x1.314f1e1d36000p-2, -0x1.8e27ad3213cb8p-45},
				{0x1.7a00000000000p-1,0, 0x1.36b6776be1000p-2, 0x1.16ecdb0f177c8p-46},
				{0x1.7800000000000p-1,0, 0x1.3c25277333000p-2, 0x1.83b54b606bd5cp-46},
				{0x1.7600000000000p-1,0, 0x1.419b423d5e800p-2, 0x1.8e436ec90e09dp-47},
				{0x1.7400000000000p-1,0, 0x1.4718dc271c800p-2, -0x1.f27ce0967d675p-45},
				{0x1.7200000000000p-1,0, 0x1.4c9e09e173000p-2, -0x1.e20891b0ad8a4p-45},
				{0x1.7000000000000p-1,0, 0x1.522ae0738a000p-2, 0x1.ebe708164c759p-45},
				{0x1.6e00000000000p-1,0, 0x1.57bf753c8d000p-2, 0x1.fadedee5d40efp-46},
				{0x1.6c00000000000p-1,0, 0x1.5d5bddf596000p-2, -0x1.a0b2a08a465dcp-47},
			},
		};

		// Compute y+TAIL = log(x) where the rounded result is y and TAIL has about additional 15 bits precision.  IX is the bit representation of x, but normalized in the subnormal range using the sign bit for the exponent.
		INLINE constexpr f64 log_inline(u64 ix, f64* tail)noexcept
		{
			static constexpr i32 N = 1 << POW_LOG_TABLE_BITS;
			static constexpr i64 OFF = 0x3fe6955500000000;

			/* f64 for better performance on targets with FLT_EVAL_METHOD==2.  */
			f64 z, r, y, invc, logc, logctail, kd, hi, t1, t2, lo, lo1, lo2, p;
			u64 iz, tmp;
			i32 k, i;

			/* x = 2^k z; where z is in range [OFF,2*OFF) and exact.
			   The range is split into N subintervals.
			   The ith subinterval contains z and c is near its center.  */
			tmp = ix - OFF;
			i = (tmp >> (52 - POW_LOG_TABLE_BITS)) % N;
			k = (int64_t)tmp >> 52; /* arithmetic shift */
			iz = ix - (tmp & 0xfffULL << 52);
			z = __builtin_bit_cast(f64,iz);
			kd = (f64)k;

			/* log(x) = k*Ln2 + log(c) + log1p(z/c-1).  */
			invc = __pow_log_data.tab[i].invc;
			logc = __pow_log_data.tab[i].logc;
			logctail = __pow_log_data.tab[i].logctail;

			/* Note: 1/c is j/N or j/N/2 where j is an integer in [N,2N) and
			 |z/c - 1| < 1/N, so r = z/c - 1 is exactly representible.  */
#if __FP_FAST_FMA
			r = __builtin_fma(z, invc, -1.0);
#else
			 /* Split z such that rhi, rlo and rhi*rhi are exact and |rlo| <= |r|.  */
			f64 zhi = __builtin_bit_cast(f64,(iz + (1ULL << 31)) & (-1ULL << 32));
			f64 zlo = z - zhi;
			f64 rhi = zhi * invc - 1.0;
			f64 rlo = zlo * invc;
			r = rhi + rlo;
#endif

			/* k*Ln2 + log(c) + r.  */
			t1 = kd * __pow_log_data.ln2hi + logc;
			t2 = t1 + r;
			lo1 = kd * __pow_log_data.ln2lo + logctail;
			lo2 = t1 - t2 + r;

			/* Evaluation is optimized assuming superscalar pipelined execution.  */
			f64 ar, ar2, ar3, lo3, lo4;
			ar = __pow_log_data.poly[0] * r; /* __pow_log_data.poly[0] = -0.5.  */
			ar2 = r * ar;
			ar3 = r * ar2;
			/* k*Ln2 + log(c) + r + __pow_log_data.poly[0]*r*r.  */
#if __FP_FAST_FMA
			hi = t2 + ar2;
			lo3 = __builtin_fma(ar, r, -ar2);
			lo4 = t2 - hi + ar2;
#else
			f64 arhi = __pow_log_data.poly[0] * rhi;
			f64 arhi2 = rhi * arhi;
			hi = t2 + arhi2;
			lo3 = rlo * (ar + arhi);
			lo4 = t2 - hi + arhi2;
#endif
			/* p = log1p(r) - r - __pow_log_data.poly[0]*r*r.  */
			p = (ar3 * (__pow_log_data.poly[1] + r * __pow_log_data.poly[2] +
						ar2 * (__pow_log_data.poly[3] + r * __pow_log_data.poly[4] + ar2 * (__pow_log_data.poly[5] + r * __pow_log_data.poly[6]))));
			lo = lo1 + lo2 + lo3 + lo4 + p;
			y = hi + lo;
			*tail = hi - y + lo;
			return y;
		}

		/* Handle cases that may overflow or underflow when computing the result that
		   is scale*(1+TMP) without intermediate rounding.  The bit representation of
		   scale is in SBITS, however it has a computed exponent that may have
		   overflown into the sign bit so that needs to be adjusted before using it as
		   a f64.  (i32)KI is the k used in the argument reduction and exponent
		   adjustment of scale, positive k here means the result may overflow and
		   negative k means the result may underflow.  */
		NO_SIDE_EFFECTS INLINE constexpr f64 specialcase_exp_inline(f64 tmp, u64 sbits, u64 ki)noexcept
		{
			f64 scale, y;

			if ((ki & 0x80000000) == 0) 
			{
				/* k > 0, the exponent of scale might have overflowed by <= 460.  */
				sbits -= 1009ull << 52;
				scale = __builtin_bit_cast(f64,sbits);
				y = 0x1p1009 * (scale + scale * tmp);
				return static_cast<f64>(y);
			}
			/* k < 0, need special care in the subnormal range.  */
			sbits += 1022ull << 52;
			/* Note: sbits is signed scale.  */
			scale = __builtin_bit_cast(f64,sbits);
			y = scale + scale * tmp;
			if (fabs(y) < 1.0) 
			{
				/* Round y to the right precision before scaling it into the subnormal
				   range to avoid f64 rounding that can cause 0.5+E/2 ulp error where
				   E is the worst-case ulp error outside the subnormal range.  So this
				   is only useful if the goal is better than 1 ulp worst-case error.  */
				f64 hi, lo, one = 1.0;
				if (y < 0.0)
					one = -1.0;
				lo = scale - y + scale * tmp;
				hi = one + y;
				lo = one - hi + y + lo;
				y = static_cast<f64>(hi + lo) - one;
				/* Fix the sign of 0.  */
				if (y == 0.0)
					y = __builtin_bit_cast(f64,sbits & 0x8000000000000000);
			}
			y = 0x1p-1022 * y;
			return static_cast<f64>(y);
		}

		// Computes sign*exp(x+xtail) where |xtail| < 2^-8/N and |xtail| <= |x|. The sign_bias argument is SIGN_BIAS or 0 and sets the sign to -1 or 1.
		NO_SIDE_EFFECTS INLINE constexpr f64 exp_inline(f64 x, f64 xtail, u32 sign_bias)noexcept
		{
			using namespace exp_details;
			static constexpr i32 N{ 1 << EXP_TABLE_BITS };
			static constexpr f64 Shift{ __exp_data.shift };

			static constexpr f64 C2 { __exp_data.poly[5 - EXP_POLY_ORDER] };
			static constexpr f64 C3 { __exp_data.poly[6 - EXP_POLY_ORDER] };
			static constexpr f64 C4 { __exp_data.poly[7 - EXP_POLY_ORDER] };
			static constexpr f64 C5 { __exp_data.poly[8 - EXP_POLY_ORDER] };

			u32 abstop;
			u64 ki, idx, top, sbits;
			/* f64 for better performance on targets with FLT_EVAL_METHOD==2.  */
			f64 kd, z, r, r2, scale, tail, tmp;

			abstop = top12(x) & 0x7ff;
			if (predict_false(abstop - top12(0x1p-54) >=
							  top12(512.0) - top12(0x1p-54))) {
				if (abstop - top12(0x1p-54) >= 0x80000000) {
					/* Avoid spurious underflow for tiny x.  */
					/* Note: 0 is common input.  */
					f64 one = WANT_ROUNDING ? 1.0 + x : 1.0;
					return sign_bias ? -one : one;
				}
				if (abstop >= top12(1024.0)) {
					/* Note: inf and nan are already handled.  */
					if (BITCAST(u64,x) >> 63)
						return __math_uflow(sign_bias);
					else
						return __math_oflow(sign_bias);
				}
				/* Large x is special cased below.  */
				abstop = 0;
			}

			/* exp(x) = 2^(k/N) * exp(r), with exp(r) in [2^(-1/2N),2^(1/2N)].  */
			/* x = ln2/N*k + r, with i32 k and r in [-ln2/2N, ln2/2N].  */
			z = __exp_data.invln2N * x;
#if TOINT_INTRINSICS
			kd = roundtoint(z);
			ki = converttoint(z);
#elif EXP_USE_TOINT_NARROW
			/* z - kd is in [-0.5-2^-16, 0.5] in all rounding modes.  */
			kd = static_cast<f64>(z + Shift);
			ki = BITCAST(u64,kd) >> 16;
			kd = (f64)(i32)ki;
#else
			/* z - kd is in [-1, 1] in non-nearest rounding modes.  */
			kd = static_cast<f64>(z + Shift);
			ki = BITCAST(u64,kd);
			kd -= Shift;
#endif
			r = x + kd * __exp_data.negln2hiN + kd * __exp_data.negln2loN;
			/* The code assumes 2^-200 < |xtail| < 2^-8/N.  */
			r += xtail;
			/* 2^(k/N) ~= scale * (1 + tail).  */
			idx = 2 * (ki % N);
			top = (ki + sign_bias) << (52 - EXP_TABLE_BITS);
			tail = __builtin_bit_cast(f64,__exp_data.tab[idx]);
			/* This is only a valid scale when -1023*N < k < 1024*N.  */
			sbits = __exp_data.tab[idx + 1] + top;
			/* exp(x) = 2^(k/N) * exp(r) ~= scale + scale * (tail + exp(r) - 1).  */
			/* Evaluation is optimized assuming superscalar pipelined execution.  */
			r2 = r * r;
			/* Without fma the worst case error is 0.25/N ulp larger.  */
			/* Worst case error is less than 0.5+1.11/N+(abs poly error * 2^53) ulp.  */
			tmp = tail + r + r2 * (C2 + r * C3) + r2 * r2 * (C4 + r * C5);
			if (predict_false(abstop == 0))
				return specialcase_exp_inline(tmp, sbits, ki);
			scale = __builtin_bit_cast(f64,sbits);
			/* Note: tmp == 0 or |tmp| > 2^-200 and scale > 2^-739, so there
			   is no spurious underflow here even without fma.  */
			return static_cast<f64>(scale + scale * tmp);
		}

		// Returns 0 if not i32, 1 if odd i32, 2 if even i32.  The argument is the bit representation of a non-zero finite floating-point value
		NO_SIDE_EFFECTS INLINE constexpr i32 checkint(u64 iy)noexcept
		{
			i32 e = iy >> 52 & 0x7ff;
			if (e < 0x3ff)
				return 0;
			if (e > 0x3ff + 52)
				return 2;
			if (iy & ((1ULL << (0x3ff + 52 - e)) - 1))
				return 0;
			if (iy & (1ULL << (0x3ff + 52 - e)))
				return 1;
			return 2;
		}

		// Returns 1 if input is the bit representation of 0, infinity or nan
		NO_SIDE_EFFECTS INLINE constexpr i32 zeroinfnan(u64 i)noexcept { return 2 * i - 1 >= 2 * BITCAST(u64,(f64)(INFINITY)) - 1; }
	}

	NO_SIDE_EFFECTS constexpr f64 pow(f64 x, f64 y)noexcept
	{
		using namespace pow_details;
		using namespace exp_details;
		u32 sign_bias = 0;
		u64 ix, iy;
		u32 topx, topy;

		ix = BITCAST(u64,x);
		iy = BITCAST(u64,y);
		topx = top12(x);
		topy = top12(y);
		if (predict_false(topx - 0x001 >= 0x7ff - 0x001 || (topy & 0x7ff) - 0x3be >= 0x43e - 0x3be)) 
		{
			/* Note: if |y| > 1075 * ln2 * 2^53 ~= 0x1.749p62 then pow(x,y) = inf/0
			   and if |y| < 2^-54 / 1075 ~= 0x1.e7b6p-65 then pow(x,y) = +-1.  */
			   /* Special cases: (x < 0x1p-126 or inf or nan) or
				  (|y| < 0x1p-65 or |y| >= 0x1p63 or nan).  */
			if (predict_false(zeroinfnan(iy))) 
			{
				if (2 * iy == 0)
					return 1.0;
				if (ix == BITCAST(u64,1.0))
					return 1.0;
				if (2 * ix > 2 * BITCAST(u64,(f64)(INFINITY)) ||
					2 * iy > 2 * BITCAST(u64,(f64)(INFINITY)))
					return x + y;
				if (2 * ix == 2 * BITCAST(u64,1.0))
					return 1.0;
				if ((2 * ix < 2 * BITCAST(u64,1.0)) == !(iy >> 63))
					return 0.0; /* |x|<1 && y==inf or |x|>1 && y==-inf.  */
				return y * y;
			}
			if (predict_false(zeroinfnan(ix))) 
			{
				f64 x2 = x * x;
				if (ix >> 63 && checkint(iy) == 1)
					x2 = -x2;
				/* Without the barrier some versions of clang hoist the 1/x2 and
				   thus division by zero exception can be signaled spuriously.  */
				return iy >> 63 ? (1 / x2) : x2;
			}
			/* Here x and y are non-zero finite.  */
			if (ix >> 63) 
			{
				/* Finite x < 0.  */
				i32 yint = checkint(iy);
				if (yint == 0)
					return std::numeric_limits<f64>::quiet_NaN();
				if (yint == 1)
					sign_bias = (0x800 << EXP_TABLE_BITS);
				ix &= 0x7fffffffffffffff;
				topx &= 0x7ff;
			}
			if ((topy & 0x7ff) - 0x3be >= 0x43e - 0x3be) 
			{
				/* Note: sign_bias == 0 here because y is not odd.  */
				if (ix == BITCAST(u64,1.0))
					return 1.0;
				if ((topy & 0x7ff) < 0x3be) {
					/* |y| < 2^-65, x^y ~= 1 + y*log(x).  */
					if (WANT_ROUNDING)
						return ix > BITCAST(u64,1.0) ? 1.0 + y :
						1.0 - y;
					else
						return 1.0;
				}
				return (ix > BITCAST(u64,1.0)) == (topy < 0x800) ?
					__math_oflow(0) :
					__math_uflow(0);
			}
			if (topx == 0) 
			{
				/* Normalize subnormal x so exponent becomes negative.  */
				ix = BITCAST(u64,x * 0x1p52);
				ix &= 0x7fffffffffffffff;
				ix -= 52ULL << 52;
			}
		}

		f64 lo;
		f64 hi = log_inline(ix, &lo);
		f64 ehi, elo;
#if __FP_FAST_FMA
		ehi = y * hi;
		elo = y * lo + __builtin_fma(y, hi, -ehi);
#else
		f64 yhi = __builtin_bit_cast(f64,iy & -1ULL << 27);
		f64 ylo = y - yhi;
		f64 lhi = __builtin_bit_cast(f64,BITCAST(u64,hi) & -1ULL << 27);
		f64 llo = hi - lhi + lo;
		ehi = yhi * lhi;
		elo = ylo * lhi + y * llo; /* |elo| < |ehi| * 2^-25.  */
#endif
		return exp_inline(ehi, elo, sign_bias);
	}

	NO_SIDE_EFFECTS INLINE constexpr f32 fmodf(f32 x, f32 y)noexcept
	{
		u32 uxi = BITCAST(u32,x);
		u32 uyi = BITCAST(u32,y);
		i32 ex = uxi >> 23 & 0xff;
		i32 ey = uyi >> 23 & 0xff;
		u32 sx = uxi & 0x80000000;
		u32 i;

		if (uyi << 1 == 0 || Math::isNaN(y) || ex == 0xff)
			return (x * y) / (x * y);
		if (uxi << 1 <= uyi << 1) 
		{
			if (uxi << 1 == uyi << 1)
				return 0 * x;
			return x;
		}

		/* normalize x and y */
		if (!ex) 
		{
			for (i = uxi << 9; i >> 31 == 0; ex--, i <<= 1);
			uxi <<= -ex + 1;
		}
		else 
		{
			uxi &= -1U >> 9;
			uxi |= 1U << 23;
		}
		if (!ey) 
		{
			for (i = uyi << 9; i >> 31 == 0; ey--, i <<= 1);
			uyi <<= -ey + 1;
		}
		else 
		{
			uyi &= -1U >> 9;
			uyi |= 1U << 23;
		}

		/* x mod y */
		for (; ex > ey; ex--) 
		{
			i = uxi - uyi;
			if (i >> 31 == 0) 
			{
				if (i == 0)
					return 0 * x;
				uxi = i;
			}
			uxi <<= 1;
		}
		i = uxi - uyi;
		if (i >> 31 == 0) 
		{
			if (i == 0)
				return 0 * x;
			uxi = i;
		}
		for (; uxi >> 23 == 0; uxi <<= 1, ex--);

		/* scale result up */
		if (ex > 0) 
		{
			uxi -= 1U << 23;
			uxi |= (u32)ex << 23;
		}
		else 
			uxi >>= -ex + 1;
		uxi |= sx;
		return __builtin_bit_cast(f32,uxi);
	}

	NO_SIDE_EFFECTS INLINE constexpr f64 fmod(f64 x, f64 y)noexcept
	{
		u64 uxi = BITCAST(u64,x);
		u64 uyi = BITCAST(u64,y);
		i32 ex = uxi >> 52 & 0x7ff;
		i32 ey = uyi >> 52 & 0x7ff;
		i32 sx = uxi >> 63;
		u64 i;

		if (uyi << 1 == 0 || Math::isNaN(y) || ex == 0x7ff)
			return (x * y) / (x * y);
		if (uxi << 1 <= uyi << 1) 
		{
			if (uxi << 1 == uyi << 1)
				return 0 * x;
			return x;
		}

		/* normalize x and y */
		if (!ex) 
		{
			for (i = uxi << 12; i >> 63 == 0; ex--, i <<= 1);
			uxi <<= -ex + 1;
		}
		else 
		{
			uxi &= -1ULL >> 12;
			uxi |= 1ULL << 52;
		}
		if (!ey) 
		{
			for (i = uyi << 12; i >> 63 == 0; ey--, i <<= 1);
			uyi <<= -ey + 1;
		}
		else 
		{
			uyi &= -1ULL >> 12;
			uyi |= 1ULL << 52;
		}

		/* x mod y */
		for (; ex > ey; ex--) 
		{
			i = uxi - uyi;
			if (i >> 63 == 0) 
			{
				if (i == 0)
					return 0 * x;
				uxi = i;
			}
			uxi <<= 1;
		}
		i = uxi - uyi;
		if (i >> 63 == 0) 
		{
			if (i == 0)
				return 0 * x;
			uxi = i;
		}
		for (; uxi >> 52 == 0; uxi <<= 1, ex--);

		/* scale result */
		if (ex > 0) 
		{
			uxi -= 1ULL << 52;
			uxi |= (u64)ex << 52;
		}
		else 
			uxi >>= -ex + 1;
		uxi |= (u64)sx << 63;
		return __builtin_bit_cast(f64,uxi);
	}

	constexpr u8 LOG2F_TABLE_BITS{4};
	constexpr u8 LOG2F_POLY_ORDER{4};
	struct log2f_data 
	{
		struct 
		{
			f64 invc, logc;
		} tab[1 << LOG2F_TABLE_BITS];
		f64 poly[LOG2F_POLY_ORDER];
	};

	constexpr log2f_data __log2f_data
	{
		.tab
		{
			{ 0x1.661ec79f8f3bep+0, -0x1.efec65b963019p-2 },
			{ 0x1.571ed4aaf883dp+0, -0x1.b0b6832d4fca4p-2 },
			{ 0x1.49539f0f010bp+0, -0x1.7418b0a1fb77bp-2 },
			{ 0x1.3c995b0b80385p+0, -0x1.39de91a6dcf7bp-2 },
			{ 0x1.30d190c8864a5p+0, -0x1.01d9bf3f2b631p-2 },
			{ 0x1.25e227b0b8eap+0, -0x1.97c1d1b3b7afp-3 },
			{ 0x1.1bb4a4a1a343fp+0, -0x1.2f9e393af3c9fp-3 },
			{ 0x1.12358f08ae5bap+0, -0x1.960cbbf788d5cp-4 },
			{ 0x1.0953f419900a7p+0, -0x1.a6f9db6475fcep-5 },
			{ 0x1p+0, 0x0p+0 },
			{ 0x1.e608cfd9a47acp-1, 0x1.338ca9f24f53dp-4 },
			{ 0x1.ca4b31f026aap-1, 0x1.476a9543891bap-3 },
			{ 0x1.b2036576afce6p-1, 0x1.e840b4ac4e4d2p-3 },
			{ 0x1.9c2d163a1aa2dp-1, 0x1.40645f0c6651cp-2 },
			{ 0x1.886e6037841edp-1, 0x1.88e9c2c1b9ff8p-2 },
			{ 0x1.767dcf5534862p-1, 0x1.ce0a44eb17bccp-2 },
		},
		.poly
		{
			-0x1.712b6f70a7e4dp-2, 0x1.ecabf496832ep-2, -0x1.715479ffae3dep-1,
			0x1.715475f35c8b8p0,
		}
	};

	NO_SIDE_EFFECTS INLINE constexpr f32 log2f(f32 x)noexcept
	{
		static constexpr auto N{ (1 << LOG2F_TABLE_BITS) };
		static constexpr auto OFF{ 0x3f330000 };

		f64 z, r, r2, p, y, y0, invc, logc;
		u32 ix, iz, top, tmp;
		i32 k, i;

		ix = __builtin_bit_cast(u32, x);
		/* Fix sign of zero with downward rounding when x==1.  */
		if (WANT_ROUNDING && predict_false(ix == 0x3f800000))
			return 0;
		if (predict_false(ix - 0x00800000 >= 0x7f800000 - 0x00800000)) {
			/* x < 0x1p-126 or inf or nan.  */
			if (ix * 2 == 0)
				return __math_divzerof(1);
			if (ix == 0x7f800000) /* log2(inf) == inf.  */
				return x;
			if ((ix & 0x80000000) || ix * 2 >= 0xff000000)
				return __math_invalidf(x);
			/* x is subnormal, normalize it.  */
			ix = __builtin_bit_cast(u32, x * 0x1p23f);
			ix -= 23 << 23;
		}

		/* x = 2^k z; where z is in range [OFF,2*OFF] and exact.
		   The range is split into N subintervals.
		   The ith subinterval contains z and c is near its center.  */
		tmp = ix - OFF;
		i = (tmp >> (23 - LOG2F_TABLE_BITS)) % N;
		top = tmp & 0xff800000;
		iz = ix - top;
		k = (i32)tmp >> 23; /* arithmetic shift */
		invc = __log2f_data.tab[i].invc;
		logc = __log2f_data.tab[i].logc;
		z = (f64)__builtin_bit_cast(f32, iz);

		/* log2(x) = log1p(z/c-1)/ln2 + log2(c) + k */
		r = z * invc - 1;
		y0 = logc + (f64)k;

		/* Pipelined polynomial evaluation to approximate log1p(r)/ln2.  */
		r2 = r * r;
		y = __log2f_data.poly[1] * r + __log2f_data.poly[2];
		y = __log2f_data.poly[0] * r2 + y;
		p = __log2f_data.poly[3] * r + y0;
		y = y * r2 + p;
		return static_cast<f32>(y);
	}

	constexpr i32 LOG2_TABLE_BITS{ 6 };
	constexpr i32 LOG2_POLY_ORDER{ 7 };
	constexpr i32 LOG2_POLY1_ORDER{ 11 };
	struct log2_data 
	{
		f64 invln2hi;
		f64 invln2lo;
		f64 poly[LOG2_POLY_ORDER - 1];
		f64 poly1[LOG2_POLY1_ORDER - 1];
		struct {
			f64 invc, logc;
		} tab[1 << LOG2_TABLE_BITS];
#if !__FP_FAST_FMA
		struct 
		{
			f64 chi, clo;
		} tab2[1 << LOG2_TABLE_BITS];
#endif
	};

	//#define N (1 << LOG2_TABLE_BITS)

	constexpr log2_data __log2_data
	{
		// First coefficient: 0x1.71547652b82fe1777d0ffda0d24p0
		.invln2hi{0x1.7154765200000p+0},
		.invln2lo{0x1.705fc2eefa200p-33},
		.poly
		{
			-0x1.71547652b8339p-1,
			0x1.ec709dc3a04bep-2,
			-0x1.7154764702ffbp-2,
			0x1.2776c50034c48p-2,
			-0x1.ec7b328ea92bcp-3,
			0x1.a6225e117f92ep-3,
		},
		.poly1
		{
			-0x1.71547652b82fep-1,
			0x1.ec709dc3a03f7p-2,
			-0x1.71547652b7c3fp-2,
			0x1.2776c50f05be4p-2,
			-0x1.ec709dd768fe5p-3,
			0x1.a61761ec4e736p-3,
			-0x1.7153fbc64a79bp-3,
			0x1.484d154f01b4ap-3,
			-0x1.289e4a72c383cp-3,
			0x1.0b32f285aee66p-3,
		},
		.tab
		{
			{0x1.724286bb1acf8p+0, -0x1.1095feecdb000p-1},
			{0x1.6e1f766d2cca1p+0, -0x1.08494bd76d000p-1},
			{0x1.6a13d0e30d48ap+0, -0x1.00143aee8f800p-1},
			{0x1.661ec32d06c85p+0, -0x1.efec5360b4000p-2},
			{0x1.623fa951198f8p+0, -0x1.dfdd91ab7e000p-2},
			{0x1.5e75ba4cf026cp+0, -0x1.cffae0cc79000p-2},
			{0x1.5ac055a214fb8p+0, -0x1.c043811fda000p-2},
			{0x1.571ed0f166e1ep+0, -0x1.b0b67323ae000p-2},
			{0x1.53909590bf835p+0, -0x1.a152f5a2db000p-2},
			{0x1.5014fed61adddp+0, -0x1.9217f5af86000p-2},
			{0x1.4cab88e487bd0p+0, -0x1.8304db0719000p-2},
			{0x1.49539b4334feep+0, -0x1.74189f9a9e000p-2},
			{0x1.460cbdfafd569p+0, -0x1.6552bb5199000p-2},
			{0x1.42d664ee4b953p+0, -0x1.56b23a29b1000p-2},
			{0x1.3fb01111dd8a6p+0, -0x1.483650f5fa000p-2},
			{0x1.3c995b70c5836p+0, -0x1.39de937f6a000p-2},
			{0x1.3991c4ab6fd4ap+0, -0x1.2baa1538d6000p-2},
			{0x1.3698e0ce099b5p+0, -0x1.1d98340ca4000p-2},
			{0x1.33ae48213e7b2p+0, -0x1.0fa853a40e000p-2},
			{0x1.30d191985bdb1p+0, -0x1.01d9c32e73000p-2},
			{0x1.2e025cab271d7p+0, -0x1.e857da2fa6000p-3},
			{0x1.2b404cf13cd82p+0, -0x1.cd3c8633d8000p-3},
			{0x1.288b02c7ccb50p+0, -0x1.b26034c14a000p-3},
			{0x1.25e2263944de5p+0, -0x1.97c1c2f4fe000p-3},
			{0x1.234563d8615b1p+0, -0x1.7d6023f800000p-3},
			{0x1.20b46e33eaf38p+0, -0x1.633a71a05e000p-3},
			{0x1.1e2eefdcda3ddp+0, -0x1.494f5e9570000p-3},
			{0x1.1bb4a580b3930p+0, -0x1.2f9e424e0a000p-3},
			{0x1.19453847f2200p+0, -0x1.162595afdc000p-3},
			{0x1.16e06c0d5d73cp+0, -0x1.f9c9a75bd8000p-4},
			{0x1.1485f47b7e4c2p+0, -0x1.c7b575bf9c000p-4},
			{0x1.12358ad0085d1p+0, -0x1.960c60ff48000p-4},
			{0x1.0fef00f532227p+0, -0x1.64ce247b60000p-4},
			{0x1.0db2077d03a8fp+0, -0x1.33f78b2014000p-4},
			{0x1.0b7e6d65980d9p+0, -0x1.0387d1a42c000p-4},
			{0x1.0953efe7b408dp+0, -0x1.a6f9208b50000p-5},
			{0x1.07325cac53b83p+0, -0x1.47a954f770000p-5},
			{0x1.05197e40d1b5cp+0, -0x1.d23a8c50c0000p-6},
			{0x1.03091c1208ea2p+0, -0x1.16a2629780000p-6},
			{0x1.0101025b37e21p+0, -0x1.720f8d8e80000p-8},
			{0x1.fc07ef9caa76bp-1, 0x1.6fe53b1500000p-7},
			{0x1.f4465d3f6f184p-1, 0x1.11ccce10f8000p-5},
			{0x1.ecc079f84107fp-1, 0x1.c4dfc8c8b8000p-5},
			{0x1.e573a99975ae8p-1, 0x1.3aa321e574000p-4},
			{0x1.de5d6f0bd3de6p-1, 0x1.918a0d08b8000p-4},
			{0x1.d77b681ff38b3p-1, 0x1.e72e9da044000p-4},
			{0x1.d0cb5724de943p-1, 0x1.1dcd2507f6000p-3},
			{0x1.ca4b2dc0e7563p-1, 0x1.476ab03dea000p-3},
			{0x1.c3f8ee8d6cb51p-1, 0x1.7074377e22000p-3},
			{0x1.bdd2b4f020c4cp-1, 0x1.98ede8ba94000p-3},
			{0x1.b7d6c006015cap-1, 0x1.c0db86ad2e000p-3},
			{0x1.b20366e2e338fp-1, 0x1.e840aafcee000p-3},
			{0x1.ac57026295039p-1, 0x1.0790ab4678000p-2},
			{0x1.a6d01bc2731ddp-1, 0x1.1ac056801c000p-2},
			{0x1.a16d3bc3ff18bp-1, 0x1.2db11d4fee000p-2},
			{0x1.9c2d14967feadp-1, 0x1.406464ec58000p-2},
			{0x1.970e4f47c9902p-1, 0x1.52dbe093af000p-2},
			{0x1.920fb3982bcf2p-1, 0x1.651902050d000p-2},
			{0x1.8d30187f759f1p-1, 0x1.771d2cdeaf000p-2},
			{0x1.886e5ebb9f66dp-1, 0x1.88e9c857d9000p-2},
			{0x1.83c97b658b994p-1, 0x1.9a80155e16000p-2},
			{0x1.7f405ffc61022p-1, 0x1.abe186ed3d000p-2},
			{0x1.7ad22181415cap-1, 0x1.bd0f2aea0e000p-2},
			{0x1.767dcf99eff8cp-1, 0x1.ce0a43dbf4000p-2},
		},
	#if !__FP_FAST_FMA
		.tab2
		{
			{0x1.6200012b90a8ep-1, 0x1.904ab0644b605p-55},
			{0x1.66000045734a6p-1, 0x1.1ff9bea62f7a9p-57},
			{0x1.69fffc325f2c5p-1, 0x1.27ecfcb3c90bap-55},
			{0x1.6e00038b95a04p-1, 0x1.8ff8856739326p-55},
			{0x1.71fffe09994e3p-1, 0x1.afd40275f82b1p-55},
			{0x1.7600015590e1p-1, -0x1.2fd75b4238341p-56},
			{0x1.7a00012655bd5p-1, 0x1.808e67c242b76p-56},
			{0x1.7e0003259e9a6p-1, -0x1.208e426f622b7p-57},
			{0x1.81fffedb4b2d2p-1, -0x1.402461ea5c92fp-55},
			{0x1.860002dfafcc3p-1, 0x1.df7f4a2f29a1fp-57},
			{0x1.89ffff78c6b5p-1, -0x1.e0453094995fdp-55},
			{0x1.8e00039671566p-1, -0x1.a04f3bec77b45p-55},
			{0x1.91fffe2bf1745p-1, -0x1.7fa34400e203cp-56},
			{0x1.95fffcc5c9fd1p-1, -0x1.6ff8005a0695dp-56},
			{0x1.9a0003bba4767p-1, 0x1.0f8c4c4ec7e03p-56},
			{0x1.9dfffe7b92da5p-1, 0x1.e7fd9478c4602p-55},
			{0x1.a1fffd72efdafp-1, -0x1.a0c554dcdae7ep-57},
			{0x1.a5fffde04ff95p-1, 0x1.67da98ce9b26bp-55},
			{0x1.a9fffca5e8d2bp-1, -0x1.284c9b54c13dep-55},
			{0x1.adfffddad03eap-1, 0x1.812c8ea602e3cp-58},
			{0x1.b1ffff10d3d4dp-1, -0x1.efaddad27789cp-55},
			{0x1.b5fffce21165ap-1, 0x1.3cb1719c61237p-58},
			{0x1.b9fffd950e674p-1, 0x1.3f7d94194cep-56},
			{0x1.be000139ca8afp-1, 0x1.50ac4215d9bcp-56},
			{0x1.c20005b46df99p-1, 0x1.beea653e9c1c9p-57},
			{0x1.c600040b9f7aep-1, -0x1.c079f274a70d6p-56},
			{0x1.ca0006255fd8ap-1, -0x1.a0b4076e84c1fp-56},
			{0x1.cdfffd94c095dp-1, 0x1.8f933f99ab5d7p-55},
			{0x1.d1ffff975d6cfp-1, -0x1.82c08665fe1bep-58},
			{0x1.d5fffa2561c93p-1, -0x1.b04289bd295f3p-56},
			{0x1.d9fff9d228b0cp-1, 0x1.70251340fa236p-55},
			{0x1.de00065bc7e16p-1, -0x1.5011e16a4d80cp-56},
			{0x1.e200002f64791p-1, 0x1.9802f09ef62ep-55},
			{0x1.e600057d7a6d8p-1, -0x1.e0b75580cf7fap-56},
			{0x1.ea00027edc00cp-1, -0x1.c848309459811p-55},
			{0x1.ee0006cf5cb7cp-1, -0x1.f8027951576f4p-55},
			{0x1.f2000782b7dccp-1, -0x1.f81d97274538fp-55},
			{0x1.f6000260c450ap-1, -0x1.071002727ffdcp-59},
			{0x1.f9fffe88cd533p-1, -0x1.81bdce1fda8bp-58},
			{0x1.fdfffd50f8689p-1, 0x1.7f91acb918e6ep-55},
			{0x1.0200004292367p+0, 0x1.b7ff365324681p-54},
			{0x1.05fffe3e3d668p+0, 0x1.6fa08ddae957bp-55},
			{0x1.0a0000a85a757p+0, -0x1.7e2de80d3fb91p-58},
			{0x1.0e0001a5f3fccp+0, -0x1.1823305c5f014p-54},
			{0x1.11ffff8afbaf5p+0, -0x1.bfabb6680bac2p-55},
			{0x1.15fffe54d91adp+0, -0x1.d7f121737e7efp-54},
			{0x1.1a00011ac36e1p+0, 0x1.c000a0516f5ffp-54},
			{0x1.1e00019c84248p+0, -0x1.082fbe4da5dap-54},
			{0x1.220000ffe5e6ep+0, -0x1.8fdd04c9cfb43p-55},
			{0x1.26000269fd891p+0, 0x1.cfe2a7994d182p-55},
			{0x1.2a00029a6e6dap+0, -0x1.00273715e8bc5p-56},
			{0x1.2dfffe0293e39p+0, 0x1.b7c39dab2a6f9p-54},
			{0x1.31ffff7dcf082p+0, 0x1.df1336edc5254p-56},
			{0x1.35ffff05a8b6p+0, -0x1.e03564ccd31ebp-54},
			{0x1.3a0002e0eaeccp+0, 0x1.5f0e74bd3a477p-56},
			{0x1.3e000043bb236p+0, 0x1.c7dcb149d8833p-54},
			{0x1.4200002d187ffp+0, 0x1.e08afcf2d3d28p-56},
			{0x1.460000d387cb1p+0, 0x1.20837856599a6p-55},
			{0x1.4a00004569f89p+0, -0x1.9fa5c904fbcd2p-55},
			{0x1.4e000043543f3p+0, -0x1.81125ed175329p-56},
			{0x1.51fffcc027f0fp+0, 0x1.883d8847754dcp-54},
			{0x1.55ffffd87b36fp+0, -0x1.709e731d02807p-55},
			{0x1.59ffff21df7bap+0, 0x1.7f79f68727b02p-55},
			{0x1.5dfffebfc3481p+0, -0x1.180902e30e93ep-54},
		},
	#endif
	};

	/* Top 16 bits of a f64.  */
	NO_SIDE_EFFECTS INLINE constexpr u32 top16(f64 x)noexcept
	{
		return __builtin_bit_cast(u64, x) >> 48;
	}

	NO_SIDE_EFFECTS INLINE constexpr f64 log2(f64 x)noexcept
	{
		static constexpr auto N {(1 << LOG2_TABLE_BITS)};
		static constexpr auto OFF {0x3fe6000000000000};
		f64 z, r, r2, r4, y, invc, logc, kd, hi, lo, t1, t2, t3, p;
		u64 ix, iz, tmp;
		u32 top;
		i32 k, i;
		ix = __builtin_bit_cast(u64, x);
		top = top16(x);
		if (predict_false(ix - __builtin_bit_cast(u64, 1.0 - 0x1.5b51p-5) < __builtin_bit_cast(u64, 1.0 + 0x1.6ab2p-5) - __builtin_bit_cast(u64, 1.0 - 0x1.5b51p-5))) {
			/* Handle close to 1.0 inputs separately.  */
			/* Fix sign of zero with downward rounding when x==1.  */
			if (WANT_ROUNDING && predict_false(ix == __builtin_bit_cast(u64, 1.0)))
				return 0;
			r = x - 1.0;
#if __FP_FAST_FMA
			hi = r * __log2_data.invln2hi;
			lo = r * __log2_data.invln2lo + __builtin_fma(r, __log2_data.invln2hi, -hi);
#else
			f64 rhi, rlo;
			rhi = __builtin_bit_cast(f64, __builtin_bit_cast(u64, r) & -1ULL << 32);
			rlo = r - rhi;
			hi = rhi * __log2_data.invln2hi;
			lo = rlo * __log2_data.invln2hi + r * __log2_data.invln2lo;
#endif
			r2 = r * r; /* rounding error: 0x1p-62.  */
			r4 = r2 * r2;
			p = r2 * (__log2_data.poly1[0] + r * __log2_data.poly1[1]);
			y = hi + p;
			lo += hi - y + p;
			lo += r4 * (__log2_data.poly1[2] + r * __log2_data.poly1[3] + r2 * (__log2_data.poly1[4] + r * __log2_data.poly1[5]) +
				r4 * (__log2_data.poly1[6] + r * __log2_data.poly1[7] + r2 * (__log2_data.poly1[8] + r * __log2_data.poly1[9])));
			y += lo;
			return (y);
		}
		if (predict_false(top - 0x0010 >= 0x7ff0 - 0x0010)) {
			/* x < 0x1p-1022 or inf or nan.  */
			if (ix * 2 == 0)
				return __math_divzero(1);
			if (ix == __builtin_bit_cast(u64, (f64)(INFINITY))) /* log(inf) == inf.  */
				return x;
			if ((top & 0x8000) || (top & 0x7ff0) == 0x7ff0)
				return __math_invalid(x);
			ix = __builtin_bit_cast(u64, x * 0x1p52);
			ix -= 52ULL << 52;
		}
		tmp = ix - OFF;
		i = (tmp >> (52 - LOG2_TABLE_BITS)) % N;
		k = (int64_t)tmp >> 52; /* arithmetic shift */
		iz = ix - (tmp & 0xfffULL << 52);
		invc = __log2_data.tab[i].invc;
		logc = __log2_data.tab[i].logc;
		z = __builtin_bit_cast(f64, iz);
		kd = (f64)k;
#if __FP_FAST_FMA
		r = __builtin_fma(z, invc, -1.0);
		t1 = r * __log2_data.invln2hi;
		t2 = r * __log2_data.invln2lo + __builtin_fma(r, __log2_data.invln2hi, -t1);
#else
		f64 rhi, rlo;
		r = (z - __log2_data.tab2[i].chi - __log2_data.tab2[i].clo) * invc;
		rhi = __builtin_bit_cast(f64, __builtin_bit_cast(u64, r) & -1ULL << 32);
		rlo = r - rhi;
		t1 = rhi * __log2_data.invln2hi;
		t2 = rlo * __log2_data.invln2hi + r * __log2_data.invln2lo;
#endif
		t3 = kd + logc;
		hi = t3 + t1;
		lo = t3 - hi + t1 + t2;
		r2 = r * r; /* rounding error: 0x1p-54/N^2.  */
		r4 = r2 * r2;
		p = __log2_data.poly[0] + r * __log2_data.poly[1] + r2 * (__log2_data.poly[2] + r * __log2_data.poly[3]) + r4 * (__log2_data.poly[4] + r * __log2_data.poly[5]);
		y = lo + r2 * p + hi;
		return (y);
	}





	constexpr u8 LOGF_TABLE_BITS{4};
	constexpr u8 LOGF_POLY_ORDER{4};
	struct logf_data 
	{
		struct {
			f64 invc, logc;
		} tab[1 << LOGF_TABLE_BITS];
		f64 ln2;
		f64 poly[LOGF_POLY_ORDER - 1]; /* First order coefficient is 1.  */
	};

	constexpr logf_data __logf_data
	{
		.tab
		{
			{ 0x1.1661ec79f8f3bep+0, -0x1.57bf7808caadep-2 },
			{ 0x1.571ed4aaf883dp+0, -0x1.2bef0a7c06ddbp-2 },
			{ 0x1.49539f0f010bp+0, -0x1.01eae7f513a67p-2 },
			{ 0x1.3c995b0b80385p+0, -0x1.b31d8a68224e9p-3 },
			{ 0x1.30d190c8864a5p+0, -0x1.6574f0ac07758p-3 },
			{ 0x1.25e227b0b8eap+0, -0x1.1aa2bc79c81p-3 },
			{ 0x1.1bb4a4a1a343fp+0, -0x1.a4e76ce8c0e5ep-4 },
			{ 0x1.12358f08ae5bap+0, -0x1.1973c5a611cccp-4 },
			{ 0x1.0953f419900a7p+0, -0x1.252f438e10c1ep-5 },
			{ 0x1p+0, 0x0p+0 },
			{ 0x1.e608cfd9a47acp-1, 0x1.aa5aa5df25984p-5 },
			{ 0x1.ca4b31f026aap-1, 0x1.c5e53aa362eb4p-4 },
			{ 0x1.b2036576afce6p-1, 0x1.526e57720db08p-3 },
			{ 0x1.9c2d163a1aa2dp-1, 0x1.bc2860d22477p-3 },
			{ 0x1.886e6037841edp-1, 0x1.1058bc8a07ee1p-2 },
			{ 0x1.767dcf5534862p-1, 0x1.4043057b6ee09p-2 },
		},
		.ln2{ 0x1.62e42fefa39efp-1},
		.poly{-0x1.00ea348b88334p-2, 0x1.5575b0be00b6ap-2, -0x1.ffffef20a4123p-2}
	};

	NO_SIDE_EFFECTS INLINE constexpr f32 logf(f32 x)noexcept
	{
		f64 z, r, r2, y, y0, invc, logc;
		u32 ix, iz, tmp;
		i32 k, i;

		ix = __builtin_bit_cast(i32, x);
		/* Fix sign of zero with downward rounding when x==1.  */
		if (WANT_ROUNDING && predict_false(ix == 0x3f800000))
			return 0;
		if (predict_false(ix - 0x00800000 >= 0x7f800000 - 0x00800000)) {
			/* x < 0x1p-126 or inf or nan.  */
			if (ix * 2 == 0)
				return __math_divzerof(1);
			if (ix == 0x7f800000) /* log(inf) == inf.  */
				return x;
			if ((ix & 0x80000000) || ix * 2 >= 0xff000000)
				return __math_invalidf(x);
			/* x is subnormal, normalize it.  */
			ix = __builtin_bit_cast(i32, x * 0x1p23f);
			ix -= 23 << 23;
		}

		/* x = 2^k z; where z is in range [0x3f330000,2*0x3f330000] and exact.
		   The range is split into N subintervals.
		   The ith subinterval contains z and c is near its center.  */
		tmp = ix - 0x3f330000;
		i = (tmp >> (23 - LOGF_TABLE_BITS)) % (1 << LOGF_TABLE_BITS);
		k = (int32_t)tmp >> 23; /* arithmetic shift */
		iz = ix - (tmp & 0xff800000);
		invc = __logf_data.tab[i].invc;
		logc = __logf_data.tab[i].logc;
		z = (f64)__builtin_bit_cast(f32, iz);

		/* log(x) = log1p(z/c-1) + log(c) + k*__logf_data.ln2 */
		r = z * invc - 1;
		y0 = logc + (f64)k * __logf_data.ln2;

		/* Pipelined polynomial evaluation to approximate log1p(r).  */
		r2 = r * r;
		y = __logf_data.poly[1] * r + __logf_data.poly[2];
		y = __logf_data.poly[0] * r2 + y;
		y = y * r2 + (y0 + r);
		return static_cast<f32>(y);
	}


	constexpr u8 LOG_TABLE_BITS{ 7 };
	constexpr u8 LOG_POLY_ORDER{ 6 };
	constexpr u8 LOG_POLY1_ORDER{ 12 };
	struct log_data 
	{
		f64 ln2hi;
		f64 ln2lo;
		f64 poly[LOG_POLY_ORDER - 1]; /* First coefficient is 1.  */
		f64 poly1[LOG_POLY1_ORDER - 1];
		struct {
			f64 invc, logc;
		} tab[1 << LOG_TABLE_BITS];
#if !__FP_FAST_FMA
		struct {
			f64 chi, clo;
		} tab2[1 << LOG_TABLE_BITS];
#endif
	};

//#define N (1 << LOG_TABLE_BITS)

	constexpr log_data __log_data
	{
		.ln2hi = 0x1.62e42fefa3800p-1,
		.ln2lo = 0x1.ef35793c76730p-45,
		.poly
		{
			-0x1.0000000000001p-1,
			0x1.555555551305bp-2,
			-0x1.fffffffeb459p-3,
			0x1.999b324f10111p-3,
			-0x1.55575e506c89fp-3,
		},
		.poly1 = 
		{
			-0x1p-1,
			0x1.5555555555577p-2,
			-0x1.ffffffffffdcbp-3,
			0x1.999999995dd0cp-3,
			-0x1.55555556745a7p-3,
			0x1.24924a344de3p-3,
			-0x1.fffffa4423d65p-4,
			0x1.c7184282ad6cap-4,
			-0x1.999eb43b068ffp-4,
			0x1.78182f7afd085p-4,
			-0x1.5521375d145cdp-4,
		},
		.tab
		{
			{0x1.734f0c3e0de9fp+0, -0x1.7cc7f79e69000p-2},
			{0x1.713786a2ce91fp+0, -0x1.76feec20d0000p-2},
			{0x1.6f26008fab5a0p+0, -0x1.713e31351e000p-2},
			{0x1.6d1a61f138c7dp+0, -0x1.6b85b38287800p-2},
			{0x1.6b1490bc5b4d1p+0, -0x1.65d5590807800p-2},
			{0x1.69147332f0cbap+0, -0x1.602d076180000p-2},
			{0x1.6719f18224223p+0, -0x1.5a8ca86909000p-2},
			{0x1.6524f99a51ed9p+0, -0x1.54f4356035000p-2},
			{0x1.63356aa8f24c4p+0, -0x1.4f637c36b4000p-2},
			{0x1.614b36b9ddc14p+0, -0x1.49da7fda85000p-2},
			{0x1.5f66452c65c4cp+0, -0x1.445923989a800p-2},
			{0x1.5d867b5912c4fp+0, -0x1.3edf439b0b800p-2},
			{0x1.5babccb5b90dep+0, -0x1.396ce448f7000p-2},
			{0x1.59d61f2d91a78p+0, -0x1.3401e17bda000p-2},
			{0x1.5805612465687p+0, -0x1.2e9e2ef468000p-2},
			{0x1.56397cee76bd3p+0, -0x1.2941b3830e000p-2},
			{0x1.54725e2a77f93p+0, -0x1.23ec58cda8800p-2},
			{0x1.52aff42064583p+0, -0x1.1e9e129279000p-2},
			{0x1.50f22dbb2bddfp+0, -0x1.1956d2b48f800p-2},
			{0x1.4f38f4734ded7p+0, -0x1.141679ab9f800p-2},
			{0x1.4d843cfde2840p+0, -0x1.0edd094ef9800p-2},
			{0x1.4bd3ec078a3c8p+0, -0x1.09aa518db1000p-2},
			{0x1.4a27fc3e0258ap+0, -0x1.047e65263b800p-2},
			{0x1.4880524d48434p+0, -0x1.feb224586f000p-3},
			{0x1.46dce1b192d0bp+0, -0x1.f474a7517b000p-3},
			{0x1.453d9d3391854p+0, -0x1.ea4443d103000p-3},
			{0x1.43a2744b4845ap+0, -0x1.e020d44e9b000p-3},
			{0x1.420b54115f8fbp+0, -0x1.d60a22977f000p-3},
			{0x1.40782da3ef4b1p+0, -0x1.cc00104959000p-3},
			{0x1.3ee8f5d57fe8fp+0, -0x1.c202956891000p-3},
			{0x1.3d5d9a00b4ce9p+0, -0x1.b81178d811000p-3},
			{0x1.3bd60c010c12bp+0, -0x1.ae2c9ccd3d000p-3},
			{0x1.3a5242b75dab8p+0, -0x1.a45402e129000p-3},
			{0x1.38d22cd9fd002p+0, -0x1.9a877681df000p-3},
			{0x1.3755bc5847a1cp+0, -0x1.90c6d69483000p-3},
			{0x1.35dce49ad36e2p+0, -0x1.87120a645c000p-3},
			{0x1.34679984dd440p+0, -0x1.7d68fb4143000p-3},
			{0x1.32f5cceffcb24p+0, -0x1.73cb83c627000p-3},
			{0x1.3187775a10d49p+0, -0x1.6a39a9b376000p-3},
			{0x1.301c8373e3990p+0, -0x1.60b3154b7a000p-3},
			{0x1.2eb4ebb95f841p+0, -0x1.5737d76243000p-3},
			{0x1.2d50a0219a9d1p+0, -0x1.4dc7b8fc23000p-3},
			{0x1.2bef9a8b7fd2ap+0, -0x1.4462c51d20000p-3},
			{0x1.2a91c7a0c1babp+0, -0x1.3b08abc830000p-3},
			{0x1.293726014b530p+0, -0x1.31b996b490000p-3},
			{0x1.27dfa5757a1f5p+0, -0x1.2875490a44000p-3},
			{0x1.268b39b1d3bbfp+0, -0x1.1f3b9f879a000p-3},
			{0x1.2539d838ff5bdp+0, -0x1.160c8252ca000p-3},
			{0x1.23eb7aac9083bp+0, -0x1.0ce7f57f72000p-3},
			{0x1.22a012ba940b6p+0, -0x1.03cdc49fea000p-3},
			{0x1.2157996cc4132p+0, -0x1.f57bdbc4b8000p-4},
			{0x1.201201dd2fc9bp+0, -0x1.e370896404000p-4},
			{0x1.1ecf4494d480bp+0, -0x1.d17983ef94000p-4},
			{0x1.1d8f5528f6569p+0, -0x1.bf9674ed8a000p-4},
			{0x1.1c52311577e7cp+0, -0x1.adc79202f6000p-4},
			{0x1.1b17c74cb26e9p+0, -0x1.9c0c3e7288000p-4},
			{0x1.19e010c2c1ab6p+0, -0x1.8a646b372c000p-4},
			{0x1.18ab07bb670bdp+0, -0x1.78d01b3ac0000p-4},
			{0x1.1778a25efbcb6p+0, -0x1.674f145380000p-4},
			{0x1.1648d354c31dap+0, -0x1.55e0e6d878000p-4},
			{0x1.151b990275fddp+0, -0x1.4485cdea1e000p-4},
			{0x1.13f0ea432d24cp+0, -0x1.333d94d6aa000p-4},
			{0x1.12c8b7210f9dap+0, -0x1.22079f8c56000p-4},
			{0x1.11a3028ecb531p+0, -0x1.10e4698622000p-4},
			{0x1.107fbda8434afp+0, -0x1.ffa6c6ad20000p-5},
			{0x1.0f5ee0f4e6bb3p+0, -0x1.dda8d4a774000p-5},
			{0x1.0e4065d2a9fcep+0, -0x1.bbcece4850000p-5},
			{0x1.0d244632ca521p+0, -0x1.9a1894012c000p-5},
			{0x1.0c0a77ce2981ap+0, -0x1.788583302c000p-5},
			{0x1.0af2f83c636d1p+0, -0x1.5715e67d68000p-5},
			{0x1.09ddb98a01339p+0, -0x1.35c8a49658000p-5},
			{0x1.08cabaf52e7dfp+0, -0x1.149e364154000p-5},
			{0x1.07b9f2f4e28fbp+0, -0x1.e72c082eb8000p-6},
			{0x1.06ab58c358f19p+0, -0x1.a55f152528000p-6},
			{0x1.059eea5ecf92cp+0, -0x1.63d62cf818000p-6},
			{0x1.04949cdd12c90p+0, -0x1.228fb8caa0000p-6},
			{0x1.038c6c6f0ada9p+0, -0x1.c317b20f90000p-7},
			{0x1.02865137932a9p+0, -0x1.419355daa0000p-7},
			{0x1.0182427ea7348p+0, -0x1.81203c2ec0000p-8},
			{0x1.008040614b195p+0, -0x1.0040979240000p-9},
			{0x1.fe01ff726fa1ap-1, 0x1.feff384900000p-9},
			{0x1.fa11cc261ea74p-1, 0x1.7dc41353d0000p-7},
			{0x1.f6310b081992ep-1, 0x1.3cea3c4c28000p-6},
			{0x1.f25f63ceeadcdp-1, 0x1.b9fc114890000p-6},
			{0x1.ee9c8039113e7p-1, 0x1.1b0d8ce110000p-5},
			{0x1.eae8078cbb1abp-1, 0x1.58a5bd001c000p-5},
			{0x1.e741aa29d0c9bp-1, 0x1.95c8340d88000p-5},
			{0x1.e3a91830a99b5p-1, 0x1.d276aef578000p-5},
			{0x1.e01e009609a56p-1, 0x1.07598e598c000p-4},
			{0x1.dca01e577bb98p-1, 0x1.253f5e30d2000p-4},
			{0x1.d92f20b7c9103p-1, 0x1.42edd8b380000p-4},
			{0x1.d5cac66fb5ccep-1, 0x1.606598757c000p-4},
			{0x1.d272caa5ede9dp-1, 0x1.7da76356a0000p-4},
			{0x1.cf26e3e6b2ccdp-1, 0x1.9ab434e1c6000p-4},
			{0x1.cbe6da2a77902p-1, 0x1.b78c7bb0d6000p-4},
			{0x1.c8b266d37086dp-1, 0x1.d431332e72000p-4},
			{0x1.c5894bd5d5804p-1, 0x1.f0a3171de6000p-4},
			{0x1.c26b533bb9f8cp-1, 0x1.067152b914000p-3},
			{0x1.bf583eeece73fp-1, 0x1.147858292b000p-3},
			{0x1.bc4fd75db96c1p-1, 0x1.2266ecdca3000p-3},
			{0x1.b951e0c864a28p-1, 0x1.303d7a6c55000p-3},
			{0x1.b65e2c5ef3e2cp-1, 0x1.3dfc33c331000p-3},
			{0x1.b374867c9888bp-1, 0x1.4ba366b7a8000p-3},
			{0x1.b094b211d304ap-1, 0x1.5933928d1f000p-3},
			{0x1.adbe885f2ef7ep-1, 0x1.66acd2418f000p-3},
			{0x1.aaf1d31603da2p-1, 0x1.740f8ec669000p-3},
			{0x1.a82e63fd358a7p-1, 0x1.815c0f51af000p-3},
			{0x1.a5740ef09738bp-1, 0x1.8e92954f68000p-3},
			{0x1.a2c2a90ab4b27p-1, 0x1.9bb3602f84000p-3},
			{0x1.a01a01393f2d1p-1, 0x1.a8bed1c2c0000p-3},
			{0x1.9d79f24db3c1bp-1, 0x1.b5b515c01d000p-3},
			{0x1.9ae2505c7b190p-1, 0x1.c2967ccbcc000p-3},
			{0x1.9852ef297ce2fp-1, 0x1.cf635d5486000p-3},
			{0x1.95cbaeea44b75p-1, 0x1.dc1bd3446c000p-3},
			{0x1.934c69de74838p-1, 0x1.e8c01b8cfe000p-3},
			{0x1.90d4f2f6752e6p-1, 0x1.f5509c0179000p-3},
			{0x1.8e6528effd79dp-1, 0x1.00e6c121fb800p-2},
			{0x1.8bfce9fcc007cp-1, 0x1.071b80e93d000p-2},
			{0x1.899c0dabec30ep-1, 0x1.0d46b9e867000p-2},
			{0x1.87427aa2317fbp-1, 0x1.13687334bd000p-2},
			{0x1.84f00acb39a08p-1, 0x1.1980d67234800p-2},
			{0x1.82a49e8653e55p-1, 0x1.1f8ffe0cc8000p-2},
			{0x1.8060195f40260p-1, 0x1.2595fd7636800p-2},
			{0x1.7e22563e0a329p-1, 0x1.2b9300914a800p-2},
			{0x1.7beb377dcb5adp-1, 0x1.3187210436000p-2},
			{0x1.79baa679725c2p-1, 0x1.377266dec1800p-2},
			{0x1.77907f2170657p-1, 0x1.3d54ffbaf3000p-2},
			{0x1.756cadbd6130cp-1, 0x1.432eee32fe000p-2},
			},
	#if !__FP_FAST_FMA
		.tab2
		{
			{0x1.61000014fb66bp-1, 0x1.e026c91425b3cp-56},
			{0x1.63000034db495p-1, 0x1.dbfea48005d41p-55},
			{0x1.650000d94d478p-1, 0x1.e7fa786d6a5b7p-55},
			{0x1.67000074e6fadp-1, 0x1.1fcea6b54254cp-57},
			{0x1.68ffffedf0faep-1, -0x1.c7e274c590efdp-56},
			{0x1.6b0000763c5bcp-1, -0x1.ac16848dcda01p-55},
			{0x1.6d0001e5cc1f6p-1, 0x1.33f1c9d499311p-55},
			{0x1.6efffeb05f63ep-1, -0x1.e80041ae22d53p-56},
			{0x1.710000e86978p-1, 0x1.bff6671097952p-56},
			{0x1.72ffffc67e912p-1, 0x1.c00e226bd8724p-55},
			{0x1.74fffdf81116ap-1, -0x1.e02916ef101d2p-57},
			{0x1.770000f679c9p-1, -0x1.7fc71cd549c74p-57},
			{0x1.78ffffa7ec835p-1, 0x1.1bec19ef50483p-55},
			{0x1.7affffe20c2e6p-1, -0x1.07e1729cc6465p-56},
			{0x1.7cfffed3fc9p-1, -0x1.08072087b8b1cp-55},
			{0x1.7efffe9261a76p-1, 0x1.dc0286d9df9aep-55},
			{0x1.81000049ca3e8p-1, 0x1.97fd251e54c33p-55},
			{0x1.8300017932c8fp-1, -0x1.afee9b630f381p-55},
			{0x1.850000633739cp-1, 0x1.9bfbf6b6535bcp-55},
			{0x1.87000204289c6p-1, -0x1.bbf65f3117b75p-55},
			{0x1.88fffebf57904p-1, -0x1.9006ea23dcb57p-55},
			{0x1.8b00022bc04dfp-1, -0x1.d00df38e04b0ap-56},
			{0x1.8cfffe50c1b8ap-1, -0x1.8007146ff9f05p-55},
			{0x1.8effffc918e43p-1, 0x1.3817bd07a7038p-55},
			{0x1.910001efa5fc7p-1, 0x1.93e9176dfb403p-55},
			{0x1.9300013467bb9p-1, 0x1.f804e4b980276p-56},
			{0x1.94fffe6ee076fp-1, -0x1.f7ef0d9ff622ep-55},
			{0x1.96fffde3c12d1p-1, -0x1.082aa962638bap-56},
			{0x1.98ffff4458a0dp-1, -0x1.7801b9164a8efp-55},
			{0x1.9afffdd982e3ep-1, -0x1.740e08a5a9337p-55},
			{0x1.9cfffed49fb66p-1, 0x1.fce08c19bep-60},
			{0x1.9f00020f19c51p-1, -0x1.a3faa27885b0ap-55},
			{0x1.a10001145b006p-1, 0x1.4ff489958da56p-56},
			{0x1.a300007bbf6fap-1, 0x1.cbeab8a2b6d18p-55},
			{0x1.a500010971d79p-1, 0x1.8fecadd78793p-55},
			{0x1.a70001df52e48p-1, -0x1.f41763dd8abdbp-55},
			{0x1.a90001c593352p-1, -0x1.ebf0284c27612p-55},
			{0x1.ab0002a4f3e4bp-1, -0x1.9fd043cff3f5fp-57},
			{0x1.acfffd7ae1ed1p-1, -0x1.23ee7129070b4p-55},
			{0x1.aefffee510478p-1, 0x1.a063ee00edea3p-57},
			{0x1.b0fffdb650d5bp-1, 0x1.a06c8381f0ab9p-58},
			{0x1.b2ffffeaaca57p-1, -0x1.9011e74233c1dp-56},
			{0x1.b4fffd995badcp-1, -0x1.9ff1068862a9fp-56},
			{0x1.b7000249e659cp-1, 0x1.aff45d0864f3ep-55},
			{0x1.b8ffff987164p-1, 0x1.cfe7796c2c3f9p-56},
			{0x1.bafffd204cb4fp-1, -0x1.3ff27eef22bc4p-57},
			{0x1.bcfffd2415c45p-1, -0x1.cffb7ee3bea21p-57},
			{0x1.beffff86309dfp-1, -0x1.14103972e0b5cp-55},
			{0x1.c0fffe1b57653p-1, 0x1.bc16494b76a19p-55},
			{0x1.c2ffff1fa57e3p-1, -0x1.4feef8d30c6edp-57},
			{0x1.c4fffdcbfe424p-1, -0x1.43f68bcec4775p-55},
			{0x1.c6fffed54b9f7p-1, 0x1.47ea3f053e0ecp-55},
			{0x1.c8fffeb998fd5p-1, 0x1.383068df992f1p-56},
			{0x1.cb0002125219ap-1, -0x1.8fd8e64180e04p-57},
			{0x1.ccfffdd94469cp-1, 0x1.e7ebe1cc7ea72p-55},
			{0x1.cefffeafdc476p-1, 0x1.ebe39ad9f88fep-55},
			{0x1.d1000169af82bp-1, 0x1.57d91a8b95a71p-56},
			{0x1.d30000d0ff71dp-1, 0x1.9c1906970c7dap-55},
			{0x1.d4fffea790fc4p-1, -0x1.80e37c558fe0cp-58},
			{0x1.d70002edc87e5p-1, -0x1.f80d64dc10f44p-56},
			{0x1.d900021dc82aap-1, -0x1.47c8f94fd5c5cp-56},
			{0x1.dafffd86b0283p-1, 0x1.c7f1dc521617ep-55},
			{0x1.dd000296c4739p-1, 0x1.8019eb2ffb153p-55},
			{0x1.defffe54490f5p-1, 0x1.e00d2c652cc89p-57},
			{0x1.e0fffcdabf694p-1, -0x1.f8340202d69d2p-56},
			{0x1.e2fffdb52c8ddp-1, 0x1.b00c1ca1b0864p-56},
			{0x1.e4ffff24216efp-1, 0x1.2ffa8b094ab51p-56},
			{0x1.e6fffe88a5e11p-1, -0x1.7f673b1efbe59p-58},
			{0x1.e9000119eff0dp-1, -0x1.4808d5e0bc801p-55},
			{0x1.eafffdfa51744p-1, 0x1.80006d54320b5p-56},
			{0x1.ed0001a127fa1p-1, -0x1.002f860565c92p-58},
			{0x1.ef00007babcc4p-1, -0x1.540445d35e611p-55},
			{0x1.f0ffff57a8d02p-1, -0x1.ffb3139ef9105p-59},
			{0x1.f30001ee58ac7p-1, 0x1.a81acf2731155p-55},
			{0x1.f4ffff5823494p-1, 0x1.a3f41d4d7c743p-55},
			{0x1.f6ffffca94c6bp-1, -0x1.202f41c987875p-57},
			{0x1.f8fffe1f9c441p-1, 0x1.77dd1f477e74bp-56},
			{0x1.fafffd2e0e37ep-1, -0x1.f01199a7ca331p-57},
			{0x1.fd0001c77e49ep-1, 0x1.181ee4bceacb1p-56},
			{0x1.feffff7e0c331p-1, -0x1.e05370170875ap-57},
			{0x1.00ffff465606ep+0, -0x1.a7ead491c0adap-55},
			{0x1.02ffff3867a58p+0, -0x1.77f69c3fcb2ep-54},
			{0x1.04ffffdfc0d17p+0, 0x1.7bffe34cb945bp-54},
			{0x1.0700003cd4d82p+0, 0x1.20083c0e456cbp-55},
			{0x1.08ffff9f2cbe8p+0, -0x1.dffdfbe37751ap-57},
			{0x1.0b000010cda65p+0, -0x1.13f7faee626ebp-54},
			{0x1.0d00001a4d338p+0, 0x1.07dfa79489ff7p-55},
			{0x1.0effffadafdfdp+0, -0x1.7040570d66bcp-56},
			{0x1.110000bbafd96p+0, 0x1.e80d4846d0b62p-55},
			{0x1.12ffffae5f45dp+0, 0x1.dbffa64fd36efp-54},
			{0x1.150000dd59ad9p+0, 0x1.a0077701250aep-54},
			{0x1.170000f21559ap+0, 0x1.dfdf9e2e3deeep-55},
			{0x1.18ffffc275426p+0, 0x1.10030dc3b7273p-54},
			{0x1.1b000123d3c59p+0, 0x1.97f7980030188p-54},
			{0x1.1cffff8299eb7p+0, -0x1.5f932ab9f8c67p-57},
			{0x1.1effff48ad4p+0, 0x1.37fbf9da75bebp-54},
			{0x1.210000c8b86a4p+0, 0x1.f806b91fd5b22p-54},
			{0x1.2300003854303p+0, 0x1.3ffc2eb9fbf33p-54},
			{0x1.24fffffbcf684p+0, 0x1.601e77e2e2e72p-56},
			{0x1.26ffff52921d9p+0, 0x1.ffcbb767f0c61p-56},
			{0x1.2900014933a3cp+0, -0x1.202ca3c02412bp-56},
			{0x1.2b00014556313p+0, -0x1.2808233f21f02p-54},
			{0x1.2cfffebfe523bp+0, -0x1.8ff7e384fdcf2p-55},
			{0x1.2f0000bb8ad96p+0, -0x1.5ff51503041c5p-55},
			{0x1.30ffffb7ae2afp+0, -0x1.10071885e289dp-55},
			{0x1.32ffffeac5f7fp+0, -0x1.1ff5d3fb7b715p-54},
			{0x1.350000ca66756p+0, 0x1.57f82228b82bdp-54},
			{0x1.3700011fbf721p+0, 0x1.000bac40dd5ccp-55},
			{0x1.38ffff9592fb9p+0, -0x1.43f9d2db2a751p-54},
			{0x1.3b00004ddd242p+0, 0x1.57f6b707638e1p-55},
			{0x1.3cffff5b2c957p+0, 0x1.a023a10bf1231p-56},
			{0x1.3efffeab0b418p+0, 0x1.87f6d66b152bp-54},
			{0x1.410001532aff4p+0, 0x1.7f8375f198524p-57},
			{0x1.4300017478b29p+0, 0x1.301e672dc5143p-55},
			{0x1.44fffe795b463p+0, 0x1.9ff69b8b2895ap-55},
			{0x1.46fffe80475ep+0, -0x1.5c0b19bc2f254p-54},
			{0x1.48fffef6fc1e7p+0, 0x1.b4009f23a2a72p-54},
			{0x1.4afffe5bea704p+0, -0x1.4ffb7bf0d7d45p-54},
			{0x1.4d000171027dep+0, -0x1.9c06471dc6a3dp-54},
			{0x1.4f0000ff03ee2p+0, 0x1.77f890b85531cp-54},
			{0x1.5100012dc4bd1p+0, 0x1.004657166a436p-57},
			{0x1.530001605277ap+0, -0x1.6bfcece233209p-54},
			{0x1.54fffecdb704cp+0, -0x1.902720505a1d7p-55},
			{0x1.56fffef5f54a9p+0, 0x1.bbfe60ec96412p-54},
			{0x1.5900017e61012p+0, 0x1.87ec581afef9p-55},
			{0x1.5b00003c93e92p+0, -0x1.f41080abf0ccp-54},
			{0x1.5d0001d4919bcp+0, -0x1.8812afb254729p-54},
			{0x1.5efffe7b87a89p+0, -0x1.47eb780ed6904p-54},
		},
	#endif
	};

	NO_SIDE_EFFECTS INLINE constexpr f64 log(f64 x)noexcept
	{
		f64 w, z, r, r2, r3, y, invc, logc, kd, hi, lo;
		u64 ix, iz, tmp;
		u32 top;
		i32 k, i;

		ix = __builtin_bit_cast(u64, x);
		top = top16(x);
		if (predict_false(ix - __builtin_bit_cast(u64, 1.0 - 0x1p-4) < __builtin_bit_cast(u64, 1.0 + 0x1.09p-4) - __builtin_bit_cast(u64, 1.0 - 0x1p-4))) {
			/* Handle close to 1.0 inputs separately.  */
			/* Fix sign of zero with downward rounding when x==1.  */
			if (WANT_ROUNDING && predict_false(ix == __builtin_bit_cast(u64, 1.0)))
				return 0;
			r = x - 1.0;
			r2 = r * r;
			r3 = r * r2;
			y = r3 *
				(__log_data.poly1[1] + r * __log_data.poly1[2] + r2 * __log_data.poly1[3] +
					r3 * (__log_data.poly1[4] + r * __log_data.poly1[5] + r2 * __log_data.poly1[6] +
						r3 * (__log_data.poly1[7] + r * __log_data.poly1[8] + r2 * __log_data.poly1[9] + r3 * __log_data.poly1[10])));
			/* Worst-case error is around 0.507 ULP.  */
			w = r * 0x1p27;
			f64 rhi = r + w - w;
			f64 rlo = r - rhi;
			w = rhi * rhi * __log_data.poly1[0]; /* __log_data.poly1[0] == -0.5.  */
			hi = r + w;
			lo = r - hi + w;
			lo += __log_data.poly1[0] * rlo * (rhi + r);
			y += lo;
			y += hi;
			return y;
		}
		if (predict_false(top - 0x0010 >= 0x7ff0 - 0x0010)) {
			/* x < 0x1p-1022 or inf or nan.  */
			if (ix * 2 == 0)
				return __math_divzero(1);
			if (ix == __builtin_bit_cast(u64, (f64)INFINITY)) /* log(inf) == inf.  */
				return x;
			if ((top & 0x8000) || (top & 0x7ff0) == 0x7ff0)
				return __math_invalid(x);
			/* x is subnormal, normalize it.  */
			ix = __builtin_bit_cast(u64, x * 0x1p52);
			ix -= 52ULL << 52;
		}

		/* x = 2^k z; where z is in range [0x3fe6000000000000,2*0x3fe6000000000000) and exact.
		   The range is split into (1 << LOG_TABLE_BITS) subintervals.
		   The ith subinterval contains z and c is near its center.  */
		tmp = ix - 0x3fe6000000000000;
		i = (tmp >> (52 - LOG_TABLE_BITS)) % (1 << LOG_TABLE_BITS);
		k = (int64_t)tmp >> 52; /* arithmetic shift */
		iz = ix - (tmp & 0xfffULL << 52);
		invc = __log_data.tab[i].invc;
		logc = __log_data.tab[i].logc;
		z = __builtin_bit_cast(f64, iz);

		/* log(x) = log1p(z/c-1) + log(c) + k*Ln2.  */
		/* r ~= z/c - 1, |r| < 1/(2*(1 << LOG_TABLE_BITS)).  */
#if __FP_FAST_FMA
	/* rounding error: 0x1p-55/(1 << LOG_TABLE_BITS).  */
		r = __builtin_fma(z, invc, -1.0);
#else
	/* rounding error: 0x1p-55/(1 << LOG_TABLE_BITS) + 0x1p-66.  */
		r = (z - __log_data.tab2[i].chi - __log_data.tab2[i].clo) * invc;
#endif
		kd = (f64)k;

		/* hi + lo = r + log(c) + k*Ln2.  */
		w = kd * __log_data.ln2hi + logc;
		hi = w + r;
		lo = w - hi + r + kd * __log_data.ln2lo;

		/* log(x) = lo + (log1p(r) - r) + hi.  */
		r2 = r * r; /* rounding error: 0x1p-54/(1 << LOG_TABLE_BITS)^2.  */
		/* Worst case error if |y| > 0x1p-5:
		   0.5 + 4.13/(1 << LOG_TABLE_BITS) + abs-poly-error*2^57 ULP (+ 0.002 ULP without fma)
		   Worst case error if |y| > 0x1p-4:
		   0.5 + 2.06/(1 << LOG_TABLE_BITS) + abs-poly-error*2^56 ULP (+ 0.001 ULP without fma).  */
		y = lo + r2 * __log_data.poly[0] +
			r * r2 * (__log_data.poly[1] + r * __log_data.poly[2] + r2 * (__log_data.poly[3] + r * __log_data.poly[4])) + hi;
		return y;
	}
	namespace exp2_details
	{
		NO_SIDE_EFFECTS INLINE constexpr f64 specialcase_exp2(f64 tmp, u64 sbits, u64 ki)noexcept
		{
			f64 scale, y;

			if ((ki & 0x80000000) == 0) {
				/* k > 0, the exponent of scale might have overflowed by 1.  */
				sbits -= 1ull << 52;
				scale = __builtin_bit_cast(f64, sbits);
				y = 2 * (scale + scale * tmp);
				return (y);
			}
			/* k < 0, need special care in the subnormal range.  */
			sbits += 1022ull << 52;
			scale = __builtin_bit_cast(f64, sbits);
			y = scale + scale * tmp;
			if (y < 1.0) {
				/* Round y to the right precision before scaling it into the subnormal
				   range to avoid f64 rounding that can cause 0.5+E/2 ulp error where
				   E is the worst-case ulp error outside the subnormal range.  So this
				   is only useful if the goal is better than 1 ulp worst-case error.  */
				f64 hi, lo;
				lo = scale - y + scale * tmp;
				hi = 1.0 + y;
				lo = 1.0 - hi + y + lo;
				y = (hi + lo) - 1.0;
				/* Avoid -0.0 with downward rounding.  */
				if (WANT_ROUNDING && y == 0.0)
					y = 0.0;
				/* The underflow exception needs to be signaled explicitly.  */
				//fp_force_eval(fp_barrier(0x1p-1022) * 0x1p-1022);
			}
			y = 0x1p-1022 * y;
			return (y);
		}
	}

	NO_SIDE_EFFECTS INLINE constexpr f64 exp2(f64 x)noexcept
	{
		using namespace exp2_details;
		using namespace exp_details;
		u32 abstop;
		u64 ki, idx, top, sbits;
		f64 kd, r, r2, scale, tail, tmp;

		abstop = top12(x) & 0x7ff;
		if (predict_false(abstop - top12(0x1p-54) >= top12(512.0) - top12(0x1p-54))) {
			if (abstop - top12(0x1p-54) >= 0x80000000)
				/* Avoid spurious underflow for tiny x.  */
				/* Note: 0 is common input.  */
				return WANT_ROUNDING ? 1.0 + x : 1.0;
			if (abstop >= top12(1024.0)) {
				if (__builtin_bit_cast(u64, x) == __builtin_bit_cast(u64, (f64)-INFINITY))
					return 0.0;
				if (abstop >= top12(INFINITY))
					return 1.0 + x;
				if (!(__builtin_bit_cast(u64, x) >> 63))
					return __math_oflow(0);
				else if (__builtin_bit_cast(u64, x) >= __builtin_bit_cast(u64, -1075.0))
					return __math_uflow(0);
			}
			if (2 * __builtin_bit_cast(u64, x) > 2 * __builtin_bit_cast(u64, 928.0))
				/* Large x is special cased below.  */
				abstop = 0;
		}

		/* exp2(x) = 2^(k/(1 << EXP_TABLE_BITS)) * 2^r, with 2^r in [2^(-1/2N),2^(1/2N)].  */
		/* x = k/(1 << EXP_TABLE_BITS) + r, with int k and r in [-1/2N, 1/2N].  */
		kd = (x + __exp_data.exp2_shift);
		ki = __builtin_bit_cast(u64, kd); /* k.  */
		kd -= __exp_data.exp2_shift; /* k/(1 << EXP_TABLE_BITS) for int k.  */
		r = x - kd;
		/* 2^(k/(1 << EXP_TABLE_BITS)) ~= scale * (1 + tail).  */
		idx = 2 * (ki % (1 << EXP_TABLE_BITS));
		top = ki << (52 - EXP_TABLE_BITS);
		tail = __builtin_bit_cast(f64, __exp_data.tab[idx]);
		/* This is only a valid scale when -1023*(1 << EXP_TABLE_BITS) < k < 1024*(1 << EXP_TABLE_BITS).  */
		sbits = __exp_data.tab[idx + 1] + top;
		/* exp2(x) = 2^(k/(1 << EXP_TABLE_BITS)) * 2^r ~= scale + scale * (tail + 2^r - 1).  */
		/* Evaluation is optimized assuming superscalar pipelined execution.  */
		r2 = r * r;
		/* Without fma the worst case error is 0.5/(1 << EXP_TABLE_BITS) ulp larger.  */
		/* Worst case error is less than 0.5+0.86/(1 << EXP_TABLE_BITS)+(abs poly error * 2^53) ulp.  */
		tmp = tail + r * __exp_data.exp2_poly[0] + r2 * (__exp_data.exp2_poly[1] + r * __exp_data.exp2_poly[2]) + r2 * r2 * (__exp_data.exp2_poly[3] + r * __exp_data.exp2_poly[4]);
		if (predict_false(abstop == 0))
			return specialcase_exp2(tmp, sbits, ki);
		scale = __builtin_bit_cast(f64, sbits);
		/* Note: tmp == 0 or |tmp| > 2^-65 and scale > 2^-928, so there
		   is no spurious underflow here even without fma.  */
		return (scale + scale * tmp);
	}

	NO_SIDE_EFFECTS INLINE constexpr f32 exp2f(f32 x)noexcept
	{
		using namespace expf_details;
		u32 abstop;
		u64 ki, t;
		f64 kd, xd, z, r, r2, y, s;

		xd = (f64)x;
		abstop = top12(x) & 0x7ff;
		if (predict_false(abstop >= top12(128.0f))) {
			/* |x| >= 128 or x is nan.  */
			if (__builtin_bit_cast(u32, x) == __builtin_bit_cast(u32, -INFINITY))
				return 0.0f;
			if (abstop >= top12(INFINITY))
				return x + x;
			if (x > 0.0f)
				return __math_oflowf(0);
			if (x <= -150.0f)
				return __math_uflowf(0);
		}

		/* x = k/N + r with r in [-1/(2N), 1/(2N)] and int k.  */
		kd = xd + __exp2f_data.shift_scaled;
		ki = __builtin_bit_cast(u64, kd);
		kd -= __exp2f_data.shift_scaled; /* k/N for int k.  */
		r = xd - kd;

		/* exp2(x) = 2^(k/N) * 2^r ~= s * (C0*r^3 + C1*r^2 + C2*r + 1) */
		t = __exp2f_data.tab[ki % (1 << EXP2F_TABLE_BITS)];
		t += ki << (52 - EXP2F_TABLE_BITS);
		s = __builtin_bit_cast(f64, t);
		z = __exp2f_data.poly[0] * r + __exp2f_data.poly[1];
		r2 = r * r;
		y = __exp2f_data.poly[2] * r + 1;
		y = z * r2 + y;
		y = y * s;
		return static_cast<f32>(y);
	}

	NO_SIDE_EFFECTS INLINE constexpr f64 specialcase_exp(f64 tmp, u64 sbits, u64 ki)noexcept
	{
		f64 scale, y;

		if ((ki & 0x80000000) == 0) {
			/* k > 0, the exponent of scale might have overflowed by <= 460.  */
			sbits -= 1009ull << 52;
			scale = __builtin_bit_cast(f64, sbits);
			y = 0x1p1009 * (scale + scale * tmp);
			return static_cast<f64>(y);
		}
		/* k < 0, need special care in the subnormal range.  */
		sbits += 1022ull << 52;
		scale = __builtin_bit_cast(f64, sbits);
		y = scale + scale * tmp;
		if (y < 1.0) {
			/* Round y to the right precision before scaling it into the subnormal
			 range to avoid f64 rounding that can cause 0.5+E/2 ulp error where
			 E is the worst-case ulp error outside the subnormal range.  So this
			 is only useful if the goal is better than 1 ulp worst-case error.  */
			f64 hi, lo;
			lo = scale - y + scale * tmp;
			hi = 1.0 + y;
			lo = 1.0 - hi + y + lo;
			y = static_cast<f64>(hi + lo) - 1.0;
			/* Avoid -0.0 with downward rounding.  */
			if (WANT_ROUNDING && y == 0.0)
				y = 0.0;
			/* The underflow exception needs to be signaled explicitly.  */
			//fp_force_eval(fp_barrier(0x1p-1022) * 0x1p-1022);
		}
		y = 0x1p-1022 * y;
		return static_cast<f64>(y);
	}

	NO_SIDE_EFFECTS INLINE constexpr f64 exp(f64 x)noexcept
	{
		using namespace exp_details;
		u32 abstop;
		u64 ki, idx, top, sbits;
		f64 kd, z, r, r2, scale, tail, tmp;

		abstop = top12(x) & 0x7ff;
		if (predict_false(abstop - top12(0x1p-54) >= top12(512.0) - top12(0x1p-54))) {
			if (abstop - top12(0x1p-54) >= 0x80000000)
				/* Avoid spurious underflow for tiny x.  */
				/* Note: 0 is common input.  */
				return WANT_ROUNDING ? 1.0 + x : 1.0;
			if (abstop >= top12(1024.0)) {
				if (__builtin_bit_cast(u64, x) == __builtin_bit_cast(u64, (f64)-INFINITY))
					return 0.0;
				if (abstop >= top12(INFINITY))
					return 1.0 + x;
				if (__builtin_bit_cast(u64, x) >> 63)
					return __math_uflow(0);
				else
					return __math_oflow(0);
			}
			/* Large x is special cased below.  */
			abstop = 0;
		}

		/* exp(x) = 2^(k/(1 << EXP_TABLE_BITS)) * exp(r), with exp(r) in [2^(-1/2N),2^(1/2N)].  */
		/* x = ln2/(1 << EXP_TABLE_BITS)*k + r, with int k and r in [-ln2/2N, ln2/2N].  */
		z = __exp_data.invln2N * x;
#if TOINT_INTRINSICS
		kd = roundtoint(z);
		ki = converttoint(z);
#elif EXP_USE_TOINT_NARROW
		/* z - kd is in [-0.5-2^-16, 0.5] in all rounding modes.  */
		kd = static_cast<f64>(z + __exp_data.shift);
		ki = __builtin_bit_cast(u64, kd) >> 16;
		kd = (f64)(int32_t)ki;
#else
		/* z - kd is in [-1, 1] in non-nearest rounding modes.  */
		kd = static_cast<f64>(z + __exp_data.shift);
		ki = __builtin_bit_cast(u64, kd);
		kd -= __exp_data.shift;
#endif
		r = x + kd * __exp_data.negln2hiN + kd * __exp_data.negln2loN;
		/* 2^(k/(1 << EXP_TABLE_BITS)) ~= scale * (1 + tail).  */
		idx = 2 * (ki % (1 << EXP_TABLE_BITS));
		top = ki << (52 - EXP_TABLE_BITS);
		tail = __builtin_bit_cast(f64, __exp_data.tab[idx]);
		/* This is only a valid scale when -1023*(1 << EXP_TABLE_BITS) < k < 1024*(1 << EXP_TABLE_BITS).  */
		sbits = __exp_data.tab[idx + 1] + top;
		/* exp(x) = 2^(k/(1 << EXP_TABLE_BITS)) * exp(r) ~= scale + scale * (tail + exp(r) - 1).  */
		/* Evaluation is optimized assuming superscalar pipelined execution.  */
		r2 = r * r;
		/* Without fma the worst case error is 0.25/(1 << EXP_TABLE_BITS) ulp larger.  */
		/* Worst case error is less than 0.5+1.11/(1 << EXP_TABLE_BITS)+(abs poly error * 2^53) ulp.  */
		tmp = tail + r + r2 * (__exp_data.poly[5 - EXP_POLY_ORDER] + r * __exp_data.poly[6 - EXP_POLY_ORDER]) + r2 * r2 * (__exp_data.poly[7 - EXP_POLY_ORDER] + r * __exp_data.poly[8 - EXP_POLY_ORDER]);
		if (predict_false(abstop == 0))
			return specialcase_exp(tmp, sbits, ki);
		scale = __builtin_bit_cast(f64, sbits);
		/* Note: tmp == 0 or |tmp| > 2^-200 and scale > 2^-739, so there
		   is no spurious underflow here even without fma.  */
		return static_cast<f64>(scale + scale * tmp);
	}

	NO_SIDE_EFFECTS INLINE constexpr f32 expf(f32 x)noexcept
	{
		using namespace expf_details;
		u32 abstop;
		u64 ki, t;
		f64 kd, xd, z, r, r2, y, s;

		xd = (f64)x;
		abstop = top12(x) & 0x7ff;
		if (predict_false(abstop >= top12(88.0f))) {
			/* |x| >= 88 or x is nan.  */
			if (__builtin_bit_cast(u32, x) == __builtin_bit_cast(u32, -INFINITY))
				return 0.0f;
			if (abstop >= top12(INFINITY))
				return x + x;
			if (x > 0x1.62e42ep6f) /* x > log(0x1p128) ~= 88.72 */
				return __math_oflowf(0);
			if (x < -0x1.9fe368p6f) /* x < log(0x1p-150) ~= -103.97 */
				return __math_uflowf(0);
		}

		/* x*(1 << EXP2F_TABLE_BITS)/Ln2 = k + r with r in [-1/2, 1/2] and int k.  */
		z = __exp2f_data.invln2_scaled * xd;

		/* Round and convert z to int, the result is in [-150*(1 << EXP2F_TABLE_BITS), 128*(1 << EXP2F_TABLE_BITS)] and
		   ideally ties-to-even rule is used, otherwise the magnitude of r
		   can be bigger which gives larger approximation error.  */
#if TOINT_INTRINSICS
		kd = roundtoint(z);
		ki = converttoint(z);
#else
		kd = z + __exp2f_data.shift;
		ki = __builtin_bit_cast(u64, kd);
		kd -= __exp2f_data.shift;
#endif
		r = z - kd;

		/* exp(x) = 2^(k/(1 << EXP2F_TABLE_BITS)) * 2^(r/(1 << EXP2F_TABLE_BITS)) ~= s * (C0*r^3 + C1*r^2 + C2*r + 1) */
		t = __exp2f_data.tab[ki % (1 << EXP2F_TABLE_BITS)];
		t += ki << (52 - EXP2F_TABLE_BITS);
		s = __builtin_bit_cast(f64, t);
		z = __exp2f_data.poly_scaled[0] * r + __exp2f_data.poly_scaled[1];
		r2 = r * r;
		y = __exp2f_data.poly_scaled[2] * r + 1;
		y = z * r2 + y;
		y = y * s;
		return static_cast<f32>(y);
	}
}

#undef FORCE_EVAL
#undef EXTRACT_WORDS
#undef GET_HIGH_WORD
#undef GET_LOW_WORD
#undef INSERT_WORDS
#undef SET_HIGH_WORD
#undef SET_LOW_WORD
#undef GET_FLOAT_WORD
#undef SET_FLOAT_WORD
#undef TOINT_INTRINSICS 
#undef EXP_USE_TOINT_NARROW 
#undef WANT_ROUNDING

PRAGMA_MSVC(warning(pop));