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
			if constexpr ($Same<f32, T>)
				return __ceilf(x);
			else
				return __ceil(x);
#endif
		}
		else
		{
			if constexpr ($Same<f32, T>)
				return xceilf(x);
			else
				return xceil(x);
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
			if constexpr ($Same<f32, T>)
				return __floorf(x);
			else
				return __floor(x);
#endif
		}
		else
		{
			if constexpr ($Same<f32, T>)
				return xfloorf(x);
			else
				return xfloor(x);
		}
	}
}