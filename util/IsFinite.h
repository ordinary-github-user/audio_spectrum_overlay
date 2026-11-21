#pragma once
#include <utility>
#include <limits>
#include "Compiler.h"
#include "ConceptFloating.h"
#include "ConceptForEach.h"
#include "Pure.h"
#include "Inline.h"
namespace Math
{
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr bool isFinite(T x)noexcept
	{
	#if CLANG || GCC
		return __builtin_isfinite(x);
	#else
		return x > -std::numeric_limits<T>::infinity() && x < std::numeric_limits<T>::infinity();
	#endif
	}

	INLINE constexpr bool isFinite($ForEach auto const& x)noexcept
	{
		for (const auto& e : x)
			if (!isFinite(e))
				return false;
		return true;
	}
}