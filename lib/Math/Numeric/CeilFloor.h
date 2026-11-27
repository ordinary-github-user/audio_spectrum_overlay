#pragma once
#include "../../Util/Concepts/Floating.h"
#include "../../Util/Pure.h"
#include "../Sleef.h"
#include "MathBuiltin.h"
namespace Math
{
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T ceil(T x)noexcept
	{
		if !consteval
		{
#if CLANG || GCC
			MATH_BUILTIN(ceil, T, x);
#else
			/* */if constexpr ($Same<f32, T>)return __ceilf(x);
			else if constexpr ($Same<f64, T>)return __ceil(x);
			else static_assert(0);
#endif
		}
		else
		{
			/* */if constexpr ($Same<f32, T>)return xceilf(x);
			else if constexpr ($Same<f64, T>)return xceil(x);
			else static_assert(0);
		}
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T floor(T x)noexcept
	{
		if !consteval
		{
#if CLANG || GCC
			MATH_BUILTIN(floor, T, x);
#else
			/* */if constexpr ($Same<f32, T>)return __floorf(x);
			else if constexpr ($Same<f64, T>)return __floor(x);
			else static_assert(0);
#endif
		}
		else
		{
			/* */if constexpr ($Same<f32, T>)return xfloorf(x);
			else if constexpr ($Same<f64, T>)return xfloor(x);
			else static_assert(0);
		}
	}
}