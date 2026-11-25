#pragma once
#include "../Util/Concepts/Floating.h"
#include "../Util/Concepts/Same.h"
#include "../Util/Inline.h"
#include "../Util/Pure.h"
#include "Numeric/Sqrt.h"
namespace Math
{
	NO_SIDE_EFFECTS INLINE constexpr auto hypot($Floating auto...args)noexcept
	{
		return sqrt(((args * args) + ...));
	}
}