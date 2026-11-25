#pragma once
#include <bit>
#include "../Math/Numeric/Pow2.h"
#include "Assert.h"
#include "Compact.h"
#include "Compiler.h"
#include "Concepts/Integral.h"
#include "Inline.h"
#include "Pure.h"
template<usize align, $UInt T>
requires(align == T(align) && align != 0)
NO_SIDE_EFFECTS INLINE constexpr T next_aligned_greater_equal(T x)noexcept
{
#if !CLANG//gcc and msvc cant optimize: https://godbolt.org/z/6cPa3jo9M
	if constexpr (Math::isPowerOf2(align))
	{
		static constexpr T mask{ static_cast<T>(align - 1) };
		static constexpr T mask_inv{ static_cast<T>(~mask) };
		return static_cast<T>(static_cast<T>(x + mask) & mask_inv);
	}
	else
#endif
	{
		const T offset{ static_cast<T>(x % compact<align>) };
		return offset ? x - offset + compact<align> : x;
	}
}

template<usize align, $UInt T>
requires(align == T(align) && align != 0)
NO_SIDE_EFFECTS INLINE constexpr T next_aligned_greater(T x)noexcept
{
	return static_cast<T>(static_cast<T>(static_cast<T>(x / compact<align>) + T{ 1 })* compact<align>);
}
template<usize align, $UInt T>
requires(align == T(align) && align != 0)
NO_SIDE_EFFECTS INLINE constexpr T prev_aligned_less_equal(T x) noexcept
{
	const T offset{ static_cast<T>(x % compact<align>) };
	return static_cast<T>(x - offset);
}

template<usize align, $UInt T>
requires(align == T(align) && align != 0)
NO_SIDE_EFFECTS INLINE constexpr T prev_aligned_less(T x) noexcept
{
	ASSERT(x != 0);
	return prev_aligned_less_equal<align>(static_cast<T>(x - T{ 1 }));
}

template<usize align, class T>requires(align != 0)NO_SIDE_EFFECTS INLINE constexpr T* next_aligned_greater_equal(T* x)noexcept { return __builtin_bit_cast(T*, next_aligned_greater_equal<align>(std::bit_cast<usize>(x))); }
template<usize align, class T>requires(align != 0)NO_SIDE_EFFECTS INLINE constexpr T* next_aligned_greater      (T* x)noexcept { return __builtin_bit_cast(T*, next_aligned_greater      <align>(std::bit_cast<usize>(x))); }
template<usize align, class T>requires(align != 0)NO_SIDE_EFFECTS INLINE constexpr T* prev_aligned_less_equal   (T* x)noexcept { return __builtin_bit_cast(T*, prev_aligned_less_equal   <align>(std::bit_cast<usize>(x))); }
template<usize align, class T>requires(align != 0)NO_SIDE_EFFECTS INLINE constexpr T* prev_aligned_less         (T* x)noexcept { return __builtin_bit_cast(T*, prev_aligned_less         <align>(std::bit_cast<usize>(x))); }