#pragma once
#include "../../Util/Compiler.h"
#include "../../Util/Concepts/Floating.h"
#include "../../Util/Pure.h"
#include "../Musl.h"
#include "MathBuiltin.h"
namespace Math
{
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T fmod(T x, T y)noexcept
	{
	//#if CLANG || GCC
	//	if !consteval
	//		MATH_BUILTIN(fmod, T, x, y);
	//	else
	//#endif
		{
			if constexpr ($Same<T, f32>)
				return musl::fmodf(x, y);
			else if constexpr ($Same<T, f64>)
				return musl::fmod(x, y);
			//y = Math::abs(y);
			//if (x >= 0)
			//	return x - Math::floor(x / y) * y;
			//else
			//	return x + Math::floor(-x / y) * y;
		}
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T fmod_positive(T x, T y)noexcept
	{
		const T result{ fmod(x,y) };
		return result < T{} ? result + y : result;
	}
}
