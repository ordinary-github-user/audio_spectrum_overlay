#pragma once
#include <limits>
#include "../Assert.h"
#include "../BasicTypes.h"
#include "../Bits.h"
#include "../Compact.h"
#include "../Concepts/Integral.h"
#include "../Inline.h"
#include "../Pow10Table.h"
#include "../Pure.h"
NO_SIDE_EFFECTS INLINE constexpr u8 count_digits10_no_zero($UInt auto x) noexcept
{
	using T = decltype(x);
	ASSERT(x != 0);
	if constexpr (1 == sizeof(x))
		return  1 + (x >= 10) + (x >= 100);
	else
	{
		const u8 bits{ static_cast<u8>(std::numeric_limits<T>::digits - Bits::leftZeroCount(x)) };
		const u8 guess{ static_cast<u8>(((u32)bits * (u32)1233) >> 12) };
		return x >= pow10_table<T, std::numeric_limits<T>::digits10 + 1>[guess] ? u8(guess + 1) : guess;
	}
}
NO_SIDE_EFFECTS INLINE constexpr u8 count_digits10($UInt auto x) noexcept
{
	if constexpr (1 == sizeof(x))
		return  1 + (x >= 10) + (x >= 100);
	else
	{
		if (!x) return 1;
		return count_digits10_no_zero(x);
	}
}
template<u8 d> CONSTEVAL bool verify_count_digits10($UInt auto u)noexcept { return count_digits10(u) == d; }
STATIC_ASSERT(1 == count_digits10(u8(0)));
STATIC_ASSERT(1 == count_digits10(u8(1)));
STATIC_ASSERT(1 == count_digits10(u8(9)));
STATIC_ASSERT(2 == count_digits10(u8(10)));
STATIC_ASSERT(2 == count_digits10(u8(99)));
STATIC_ASSERT(3 == count_digits10(u8(100)));
STATIC_ASSERT(3 == count_digits10(u8(255)));
STATIC_ASSERT(1 == count_digits10(u16(0)));
STATIC_ASSERT(1 == count_digits10(u16(1)));
STATIC_ASSERT(1 == count_digits10(u16(9)));
STATIC_ASSERT(2 == count_digits10(u16(10)));
STATIC_ASSERT(2 == count_digits10(u16(99)));
STATIC_ASSERT(3 == count_digits10(u16(100)));
STATIC_ASSERT(3 == count_digits10(u16(999)));
STATIC_ASSERT(4 == count_digits10(u16(1000)));
STATIC_ASSERT(5 == count_digits10(u16(10000)));
STATIC_ASSERT(5 == count_digits10(u16(65535)));
STATIC_ASSERT(1 == count_digits10(u32(0)));
STATIC_ASSERT(1 == count_digits10(u32(1)));
STATIC_ASSERT(1 == count_digits10(u32(9)));
STATIC_ASSERT(2 == count_digits10(u32(10)));
STATIC_ASSERT(2 == count_digits10(u32(99)));
STATIC_ASSERT(3 == count_digits10(u32(100)));
STATIC_ASSERT(3 == count_digits10(u32(999)));
STATIC_ASSERT(4 == count_digits10(u32(1000)));
STATIC_ASSERT(4 == count_digits10(u32(9999)));
STATIC_ASSERT(5 == count_digits10(u32(10000)));
STATIC_ASSERT(5 == count_digits10(u32(99999)));
STATIC_ASSERT(6 == count_digits10(u32(100000)));
STATIC_ASSERT(6 == count_digits10(u32(999999)));
STATIC_ASSERT(7 == count_digits10(u32(1000000)));
STATIC_ASSERT(7 == count_digits10(u32(9999999)));
STATIC_ASSERT(8 == count_digits10(u32(10000000)));
STATIC_ASSERT(8 == count_digits10(u32(99999999)));
STATIC_ASSERT(9 == count_digits10(u32(100000000)));
STATIC_ASSERT(9 == count_digits10(u32(999999999)));
STATIC_ASSERT(10 == count_digits10(u32(1000000000)));
STATIC_ASSERT(10 == count_digits10(u32(4294967295)));
STATIC_ASSERT(1 == count_digits10(u64(0ULL)));
STATIC_ASSERT(1 == count_digits10(u64(1ULL)));
STATIC_ASSERT(1 == count_digits10(u64(9ULL)));
STATIC_ASSERT(2 == count_digits10(u64(10ULL)));
STATIC_ASSERT(2 == count_digits10(u64(99ULL)));
STATIC_ASSERT(3 == count_digits10(u64(100ULL)));
STATIC_ASSERT(3 == count_digits10(u64(999ULL)));
STATIC_ASSERT(4 == count_digits10(u64(1000ULL)));
STATIC_ASSERT(4 == count_digits10(u64(9999ULL)));
STATIC_ASSERT(5 == count_digits10(u64(10000ULL)));
STATIC_ASSERT(5 == count_digits10(u64(99999ULL)));
STATIC_ASSERT(6 == count_digits10(u64(100000ULL)));
STATIC_ASSERT(6 == count_digits10(u64(999999ULL)));
STATIC_ASSERT(7 == count_digits10(u64(1000000ULL)));
STATIC_ASSERT(7 == count_digits10(u64(9999999ULL)));
STATIC_ASSERT(8 == count_digits10(u64(10000000ULL)));
STATIC_ASSERT(8 == count_digits10(u64(99999999ULL)));
STATIC_ASSERT(9 == count_digits10(u64(100000000ULL)));
STATIC_ASSERT(9 == count_digits10(u64(999999999ULL)));
STATIC_ASSERT(10 == count_digits10(u64(1000000000ULL)));
STATIC_ASSERT(10 == count_digits10(u64(9999999999ULL)));
STATIC_ASSERT(11 == count_digits10(u64(10000000000ULL)));
STATIC_ASSERT(11 == count_digits10(u64(99999999999ULL)));
STATIC_ASSERT(12 == count_digits10(u64(100000000000ULL)));
STATIC_ASSERT(12 == count_digits10(u64(999999999999ULL)));
STATIC_ASSERT(13 == count_digits10(u64(1000000000000ULL)));
STATIC_ASSERT(13 == count_digits10(u64(9999999999999ULL)));
STATIC_ASSERT(14 == count_digits10(u64(10000000000000ULL)));
STATIC_ASSERT(14 == count_digits10(u64(99999999999999ULL)));
STATIC_ASSERT(15 == count_digits10(u64(100000000000000ULL)));
STATIC_ASSERT(15 == count_digits10(u64(999999999999999ULL)));
STATIC_ASSERT(16 == count_digits10(u64(1000000000000000ULL)));
STATIC_ASSERT(16 == count_digits10(u64(9999999999999999ULL)));
STATIC_ASSERT(17 == count_digits10(u64(10000000000000000ULL)));
STATIC_ASSERT(17 == count_digits10(u64(99999999999999999ULL)));
STATIC_ASSERT(18 == count_digits10(u64(999999999999999999ULL)));
STATIC_ASSERT(19 == count_digits10(u64(1000000000000000000ULL)));
STATIC_ASSERT(19 == count_digits10(u64(9999999999999999999ULL)));
STATIC_ASSERT(20 == count_digits10(u64(10000000000000000000ULL)));
STATIC_ASSERT(20 == count_digits10(u64(18446744073709551615ULL)));
