#pragma once
#include "Sqrt.h"
#include "ConceptFloating.h"
#include "ConceptSame.h"
#include "Inline.h"
#include "Pure.h"
namespace Math
{
	NO_SIDE_EFFECTS INLINE constexpr auto hypot($Floating auto...args)noexcept
	{
		return sqrt(((args * args) + ...));
	}
}