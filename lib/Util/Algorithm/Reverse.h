#pragma once
#include "../Concepts/FixSized.h"
#include "../Concepts/HasBeginEnd.h"
#include "../Concepts/Indexable.h"
#include "../Fold.h"
#include "../Inline.h"
namespace Algorithm
{
	template<class BidirIt>
	INLINE constexpr void reverse(BidirIt first, BidirIt last)noexcept
	{
		while ((first != last) && (first != --last))
		{
			std::swap(*first, *last);
			++first;
		}
	}

	template<class Container>
	requires($Indexable<Container>&& $FixSized<Container>)
	INLINE constexpr void reverse(Container&& container)noexcept
	{
		constexpr auto size{ static_size<Container> };
		Fold::comma<size / 2>([&](auto I)LAMBDA_INLINE
		{
			if constexpr (I != size - 1 - I)
				std::swap(container[I], container[size - 1 - I]);
		});
	}

	template<$HasBeginEnd Container>
	requires(!($Indexable<Container>&& $FixSized<Container>))
	INLINE constexpr void reverse(Container&& container)noexcept
	{
		Algorithm::reverse(container.begin(), container.end());
	}
}