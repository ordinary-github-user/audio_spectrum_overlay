#pragma once
#include "../../Util/Inline.h"
#include "../../Util/Pure.h"
namespace Math
{
	template<class T>
	[[nodiscard]] NO_SIDE_EFFECTS INLINE constexpr T clamp(T x, T min, T max)noexcept
	{
		if (x < min)return min;
		if (x > max)return max;
		return x;
	}
}