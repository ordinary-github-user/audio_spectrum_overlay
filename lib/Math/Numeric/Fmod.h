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
			/* */if constexpr ($Same<f32, T>)return musl::fmodf(x, y);
			else if constexpr ($Same<f64, T>)return musl::fmod(x, y);
			else static_assert(0);
		}
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T fmod_positive(T x, T y)noexcept
	{
		const T result{ fmod(x,y) };
		return result < T{} ? result + y : result;
	}
}