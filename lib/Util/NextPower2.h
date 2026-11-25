#pragma once
#include "Assert.h"
#include "Bits.h"
#include "Concepts/Integral.h"
#include "Inline.h"
#include "Pure.h"
template<$UInt T>
constexpr T max_power2{ static_cast<T>(~static_cast<T>(static_cast<T>(~T{ 0 }) >> 1)) };
template<$UInt T>
NO_SIDE_EFFECTS INLINE constexpr T next_power2_greater(T x)noexcept
{
	ASSERT(x < max_power2<T>);
	return static_cast<T>(T{ 1 } << Bits::bitLength(x));
}
template<$UInt T>
NO_SIDE_EFFECTS INLINE constexpr T next_power2_greater_equal(T x)noexcept//std::bit_ceil
{
	ASSERT(x <= max_power2<T>);
	return next_power2_greater<T>(static_cast<T>(x - (x != T{ 0 })));
}
template<$UInt T>
NO_SIDE_EFFECTS INLINE constexpr T prev_power2_less_equal(T x)noexcept//std::bit_floor except std::bit_floor(0)==0
{
	ASSERT(x != T{ 0 });
	return static_cast<T>(T{ 1 } << u8(Bits::bitLength(x) - 1));
}
template<$UInt T>
NO_SIDE_EFFECTS INLINE constexpr T prev_power2_less(T x)noexcept
{
	ASSERT(x > T{ 1 });
	return prev_power2_less_equal<T>(static_cast<T>(x - T{ 1 }));
}