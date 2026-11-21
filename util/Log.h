#pragma once
#include "Inline.h"
#include "Musl.h"
#include "Pure.h"
namespace Math
{
	NO_SIDE_EFFECTS INLINE constexpr f32 log2(f32 x)noexcept { return musl::log2f(x); }
	NO_SIDE_EFFECTS INLINE constexpr f64 log2(f64 x)noexcept { return musl::log2(x); }
	NO_SIDE_EFFECTS INLINE constexpr f32 ln(f32 x)noexcept { return musl::logf(x); }
	NO_SIDE_EFFECTS INLINE constexpr f64 ln(f64 x)noexcept { return musl::log(x); }
}