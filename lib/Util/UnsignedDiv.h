#pragma once
#include "Concepts/Integral.h"
#include "Inline.h"
#include "Pure.h"
template<$UInt T>NO_SIDE_EFFECTS INLINE constexpr T unsigned_div_floor(T a, T b)noexcept{return a / b;}
template<$UInt T>NO_SIDE_EFFECTS INLINE constexpr T unsigned_div_ceil (T a, T b)noexcept{return a / b + (a % b != 0);}