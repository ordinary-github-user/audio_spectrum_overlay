#pragma once
#include "ConceptIntegral.h"
#include "Pure.h"
#include "Inline.h"
NO_SIDE_EFFECTS INLINE constexpr bool isOdd($Int auto x)noexcept { return x & 1; }
NO_SIDE_EFFECTS INLINE constexpr bool isEven($Int auto x)noexcept { return !(x & 1); }