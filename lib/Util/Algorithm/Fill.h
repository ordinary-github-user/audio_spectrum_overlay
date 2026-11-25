#pragma once
#include "../ElementOf.h"
#include "../Inline.h"
namespace Algorithm
{
	template<class Container>
	INLINE constexpr void fill(Container&& container, const ElementOf<Container>& value)noexcept
	{
		for (auto& entry : container)
			entry = value;
	}
}