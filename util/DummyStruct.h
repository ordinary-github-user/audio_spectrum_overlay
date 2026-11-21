#pragma once
#include "Inline.h"
struct Dummy
{
	INLINE constexpr Dummy(auto&&...)noexcept{}
};