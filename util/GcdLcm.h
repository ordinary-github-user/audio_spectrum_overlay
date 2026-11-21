#pragma once
#include "ConceptIntegral.h"
#include <numeric>
#include "Pure.h"
#include "Inline.h"
namespace Math
{
	//TODO:variadic
	template<$UInt T>
	NO_SIDE_EFFECTS INLINE constexpr T gcd(T a, T b) noexcept
	{
		return std::gcd(a, b);
	}

	template<$UInt T>
	NO_SIDE_EFFECTS INLINE constexpr T lcm(T a, T b) noexcept
	{
		return std::lcm(a,b);
	}

	template<$UInt First, $UInt...Args>
	NO_SIDE_EFFECTS INLINE constexpr std::common_type_t<First, Args...> lcm(First first, Args...args)noexcept
	{
		std::common_type_t<First, Args...> result{first};
		((result = std::lcm(result, args)), ...);
		return result;
	}
}
