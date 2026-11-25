#pragma once
#include "Concepts/Char.h"
#include "Inline.h"
INLINE constexpr bool isDigit($Char auto x)noexcept { return x >= '0' && x <= '9'; }