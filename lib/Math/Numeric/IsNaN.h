#pragma once
#if defined(__FAST_MATH__) || defined(_M_FP_FAST)
	#error turn off fast math
#endif
#include "../../Util/Concepts/Floating.h"
#include "../../Util/Inline.h"
#include "../../Util/Pure.h"
namespace Math
{
	NO_SIDE_EFFECTS INLINE constexpr bool isNaN($Floating auto x)noexcept
	{
		return x != x;
	}
}