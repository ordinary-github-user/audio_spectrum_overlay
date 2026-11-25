#pragma once
#include <bit>
#include "../BasicTypes.h"
#include "../Concepts/ForEach.h"
#include "../Concepts/Integral.h"
#include "../Concepts/Size.h"
#include "../Inline.h"
#include "../SizedInt.h"
template<$UInt T, T FnvPrime, T OffsetBasis, bool A>
struct FNV
{
	T result{ OffsetBasis };

	template<$SizeAnyOf<1, 2, 4, 8> Entry>
	requires(!$ForEach<Entry>)
	INLINE constexpr T operator()(const Entry& x)noexcept
	{
		if constexpr (A)
		{
			result ^= __builtin_bit_cast(UnsignedBits<sizeof(Entry) * 8>,x);
			result *= FnvPrime;
		}
		else
		{
			result *= FnvPrime;
			result ^= __builtin_bit_cast(UnsignedBits<sizeof(Entry) * 8>,x);
		}
		return result;
	}

	INLINE constexpr T operator()($ForEach auto const& input) noexcept
	{
		for (const auto& x : input)
			operator()(x);
		return result;
	}

	INLINE static constexpr T hash($ForEach auto const& input)noexcept
	{
		return FNV{}(input);
	}

	template<$SizeAnyOf<1, 2, 4, 8> Entry>
	requires(!$ForEach<Entry>)
	INLINE static constexpr T hash(const Entry& input)noexcept
	{
		return FNV{}(input);
	}
};

using Fnv32 = FNV<u32, 16777619_u32, 2166136261_u32, false>;
using Fnv32a = FNV<u32, 16777619_u32, 2166136261_u32, true>;

using Fnv64 = FNV<u64, 1099511628211_u64, 14695981039346656037_u64, false>;
using Fnv64a = FNV<u64, 1099511628211_u64, 14695981039346656037_u64, true>;