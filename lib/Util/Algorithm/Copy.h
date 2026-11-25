#pragma once
#include <memory>
#include <utility>
#include "../Concepts/ForEach.h"
#include "../Concepts/Integral.h"
#include "../Inline.h"
#include "../Macro/PlacementNew.h"
namespace Algorithm
{
	INLINE constexpr void copy_assign($ForEach auto const& container, auto pDest)noexcept
	{
		for (const auto& entry : container)
		{
			*pDest = entry;
			++pDest;
		}
	}

	INLINE constexpr void copy_assign(auto pBegin, auto pEnd, auto pDest)noexcept
	requires(requires{pBegin != pEnd; *pDest = *pBegin; })
	{
		while(pBegin!=pEnd)
		{
			*pDest = *pBegin;
			++pDest;
			++pBegin;
		}
	}

	INLINE constexpr void copy_assign(auto pBegin, $UInt auto size, auto pDest)noexcept
	requires(requires{*pDest = *pBegin; })
	{
		copy_assign(pBegin, pBegin + size, pDest);
	}

	INLINE constexpr void copy_ctor($ForEach auto const& container, auto pDest)noexcept
	{
		for (const auto& entry : container)
		{
			CTOR_CONST_AT(pDest, entry);
			++pDest;
		}
	}

	INLINE constexpr void copy_ctor(auto pBegin, auto pEnd, auto pDest)noexcept
	requires(requires{pBegin != pEnd; CTOR_CONST_AT(pDest, *pBegin); })
	{
		while (pBegin != pEnd)
		{
			CTOR_CONST_AT(pDest, *pBegin);
			++pDest;
			++pBegin;
		}
	}

	INLINE constexpr void copy_ctor(auto pBegin, $UInt auto size, auto pDest)noexcept
	requires(requires{CTOR_CONST_AT(pDest, *pBegin); })
	{
		copy_ctor(pBegin, pBegin + size, pDest);
	}
}