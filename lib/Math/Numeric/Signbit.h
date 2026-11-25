#pragma once
#include <bit>
#include "../../Util/BasicTypes.h"
#include "../../Util/Concepts/Same.h"
#include "../../Util/Inline.h"
#include "../../Util/Pure.h"
namespace Math
{
	NO_SIDE_EFFECTS INLINE constexpr bool signbit($AnyOf<f32, f64> auto x) noexcept
	{
		using T = std::remove_cvref_t<decltype(x)>;
		if !consteval
		{
#if CLANG || GCC
			return __builtin_signbit(x);
#else
			if constexpr ($Same<f32, T>)
				return __signbitvaluef(x);
			else
				return __signbitvalue(x);
#endif
		}
		else
		{
			using uint = std::conditional_t<$Same<T, f32>, u32, u64>;
			return __builtin_bit_cast(uint,x) & (uint{ 1 } << (sizeof(T) * 8 - 1));
		}
	}
}