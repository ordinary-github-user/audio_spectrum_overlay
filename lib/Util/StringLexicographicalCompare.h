#pragma once
#include <algorithm>
#include "Concepts/Same.h"	
#include "Concepts/String.h"	
#include "ElementOf.h"	
#include "Inline.h"
INLINE constexpr bool operator<($String auto&& l, $String auto&& r)noexcept
requires($Same<ElementOf<decltype(l)>, ElementOf<decltype(r)>>)
{
	//return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end(), [](ElementOf<decltype(l)> l, ElementOf<decltype(r)> r)STATIC_LAMBDA_INLINE{ return l < r; });
	auto lIt{ l.begin() };
	auto rIt{ r.begin() };
	const auto lEnd{ l.end() };
	const auto rEnd{ r.end() };
	for (;; ++lIt, ++rIt)
	{
		if (lIt == lEnd)return rIt != rEnd;
		if (rIt == rEnd)return false;
		if (*lIt < *rIt)return true;
		if (*lIt > *rIt)return false;
	}
}

INLINE constexpr bool operator>($String auto&& l, $String auto&& r)noexcept
requires($Same<ElementOf<decltype(l)>, ElementOf<decltype(r)>>)
{
	//return std::lexicographical_compare(r.begin(), r.end(), l.begin(), l.end(), [](ElementOf<decltype(l)> l, ElementOf<decltype(r)> r)STATIC_LAMBDA_INLINE{ return l < r; });
	auto lIt{ l.begin() };
	auto rIt{ r.begin() };
	const auto lEnd{ l.end() };
	const auto rEnd{ r.end() };
	for (;; ++lIt, ++rIt)
	{
		if (rIt == rEnd)return lIt != lEnd;
		if (lIt == lEnd)return false;
		if (*rIt < *lIt)return true;
		if (*rIt > *lIt)return false;
	}
}