#pragma once
#include "../../Util/Concepts/Floating.h"
#include "../../Util/Pure.h"
#include "../Sleef.h"
#include "MathBuiltin.h"
namespace Math
{
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T round(T x)noexcept
	{
		if !consteval
		{
#if CLANG
			MATH_BUILTIN(round, T, x);
#elif GCC//todo
			/* */if constexpr ($Same<f32, T>){const f32 t{ __builtin_truncf(x) };return __builtin_fabsf(x - t) >= 0.5f ? t + __builtin_copysignf(1.f, x) : t;}
			else if constexpr ($Same<f64, T>){const f64 t{ __builtin_trunc (x) };return __builtin_fabs (x - t) >= 0.5  ? t + __builtin_copysign (1. , x) : t;}
			else static_assert(0);
#else
			/* */if constexpr ($Same<f32, T>) return __roundf(x);
			else if constexpr ($Same<f64, T>) return __round(x);
			else static_assert(0);
#endif
		}
		else
		{
			/* */if constexpr ($Same<f32, T>)return xroundf(x);
			else if constexpr ($Same<f64, T>)return xround(x);
		}
	}
}