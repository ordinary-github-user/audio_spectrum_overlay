#pragma once
#include <utility>
#include "../Concepts/ForEach.h"
#include "../Inline.h"
namespace Algorithm
{
	template<class Ret>
	INLINE constexpr Ret sum($ForEach auto&& container)noexcept
	requires(requires(Ret result) { result += *container.begin(); })
	{
		Ret result{};
		for (const auto& entry : container)
			result += entry;
		return result;
	}

	template<class Ret>
	INLINE constexpr Ret sum($ForEach auto&& container, auto&& action)noexcept
	requires(requires(Ret result) { result += action(*container.begin()); })
	{
		Ret result{};
		for (const auto& entry : container)
			result += action(entry);
		return result;
	}
}