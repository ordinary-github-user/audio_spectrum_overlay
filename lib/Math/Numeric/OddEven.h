#pragma once
#include "../../Util/Concepts/Integral.h"
#include "../../Util/Inline.h"
#include "../../Util/Pure.h"
NO_SIDE_EFFECTS INLINE constexpr bool isOdd($Int auto x)noexcept { return x & 1; }
NO_SIDE_EFFECTS INLINE constexpr bool isEven($Int auto x)noexcept { return !(x & 1); }