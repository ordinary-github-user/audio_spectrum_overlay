#pragma once
#include "Wtf16CaseConversion.h"
#include "ConceptLinearContainer.h"
namespace Wtf16
{
	constexpr bool iequal($LinearContainerOf<c16> auto&& l, $LinearContainerOf<c16> auto&& r)noexcept
	{
		const auto size{ l.size() };
		if (size != r.size())
			return false;
		const c16* pL{ l.data() };
		const c16* pR{ r.data() };
		const c16* pLEnd{ pL + size };
		for (;;)
		{
			if (pL == pLEnd)return true;
			if (Wtf16::toupper(*pL) != Wtf16::toupper(*pR))
				return false;
			++pL;
			++pR;
		}
	}
}