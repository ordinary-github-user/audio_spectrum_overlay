#pragma once
#include "ConceptSig.h"
#include "Inline.h"
struct Run
{
	FLATTEN INLINE constexpr Run($Sig<void()> auto&&...action)noexcept
	{
		(action(), ...);
	}
};