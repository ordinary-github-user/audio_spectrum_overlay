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
#if CLANG || GCC
			MATH_BUILTIN(round, T, x);
#else
			if constexpr ($Same<f32, T>)
				return __roundf(x);
			else
				return __round(x);
#endif
		}
		else
		{
			if constexpr ($Same<f32, T>)
				return xroundf(x);
			else
				return xround(x);
		}
	}
}