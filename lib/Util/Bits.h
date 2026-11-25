#pragma once
#include <bit>
#include <immintrin.h>
#include <limits>
#include "Assert.h"
#include "BasicTypes.h"
#include "BitCast.h"
#include "Compact.h"
#include "Compiler.h"
#include "Concepts/Integral.h"
#include "Constant.h"
#include "Inline.h"
#include "Pure.h"
#include "Rot.h"
#include "SizedInt.h"
#include "StaticAssert.h"
#if MSVC || CLANG_CL
	#include <intrin.h>
#endif

namespace Bits
{
	template<usize Index>
	NO_SIDE_EFFECTS INLINE constexpr bool at($Int auto x)noexcept 
	requires (Index < sizeof(x) * 8)
	{
		using T = std::remove_cvref_t<decltype(x)>;
		return static_cast<bool>(x & constant<T{1} << Index>); 
	}

	NO_SIDE_EFFECTS INLINE constexpr u8 count($Int auto x)noexcept
	{
#if MSVC
		if consteval
		{
			return static_cast<u8>(std::popcount(BITCAST(UnsignedBits<sizeof(x) * 8>, x)));//todo
		}
		else
		{
			if constexpr (sizeof(x) == 1)return static_cast<u8>(__popcnt16(BITCAST(UnsignedBits<sizeof(x) * 8>, x)));
			if constexpr (sizeof(x) == 2)return static_cast<u8>(__popcnt16(BITCAST(UnsignedBits<sizeof(x) * 8>, x)));
			if constexpr (sizeof(x) == 4)return static_cast<u8>(__popcnt  (BITCAST(UnsignedBits<sizeof(x) * 8>, x)));
			if constexpr (sizeof(x) == 8)return static_cast<u8>(__popcnt64(BITCAST(UnsignedBits<sizeof(x) * 8>, x)));
		}
#else
		return __builtin_popcountg(BITCAST(UnsignedBits<sizeof(x) * 8>, x));
#endif
	}

	template<$UInt T>
	NO_SIDE_EFFECTS INLINE constexpr T reverse(T x)noexcept
	{
		if constexpr (sizeof(T) == 1)
		{
#if CLANG
			return __builtin_bitreverse8(x);
#else
			x = ((x & 0x55) << 1) | ((x & 0xAA) >> 1);
			x = ((x & 0x33) << 2) | ((x & 0xCC) >> 2);
			x = ((x & 0x0F) << 4) | ((x & 0xF0) >> 4);
			return x;
#endif
		}
		else if constexpr (sizeof(T) == 2)
		{
#if CLANG
			return __builtin_bitreverse16(x);
#else
			x = ((x & 0x5555) << 1) | ((x & 0xAAAA) >> 1);
			x = ((x & 0x3333) << 2) | ((x & 0xCCCC) >> 2);
			x = ((x & 0x0F0F) << 4) | ((x & 0xF0F0) >> 4);
			x = ((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8);
			return x;
#endif
		}
		else if constexpr (sizeof(T) == 4)
		{
#if CLANG
			return __builtin_bitreverse32(x);
#else
			x = ((x & 0x55555555) << 1)  | ((x & 0xAAAAAAAA) >> 1);
			x = ((x & 0x33333333) << 2)  | ((x & 0xCCCCCCCC) >> 2);
			x = ((x & 0x0F0F0F0F) << 4)  | ((x & 0xF0F0F0F0) >> 4);
			x = ((x & 0x00FF00FF) << 8)  | ((x & 0xFF00FF00) >> 8);
			x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16);
			return x;
#endif
		}
		else if constexpr (sizeof(T) == 8)
		{
#if CLANG
			return __builtin_bitreverse64(x);
#else
			x = ((x & 0x5555555555555555) << 1) | ((x & 0xAAAAAAAAAAAAAAAA) >> 1);
			x = ((x & 0x3333333333333333) << 2) | ((x & 0xCCCCCCCCCCCCCCCC) >> 2);
			x = ((x & 0x0F0F0F0F0F0F0F0F) << 4) | ((x & 0xF0F0F0F0F0F0F0F0) >> 4);
			x = ((x & 0x00FF00FF00FF00FF) << 8) | ((x & 0xFF00FF00FF00FF00) >> 8);
			x = ((x & 0x0000FFFF0000FFFF) << 16) | ((x & 0xFFFF0000FFFF0000) >> 16);
			x = ((x & 0x00000000FFFFFFFF) << 32) | ((x & 0xFFFFFFFF00000000) >> 32);
			return x;
#endif
		}
	}

	NO_SIDE_EFFECTS INLINE constexpr u8 leastSignificantBitPosPlus1($UInt auto x)noexcept
	{
		using T = std::remove_cvref_t<decltype(x)>;
		if consteval
		{
			for (u8 i{}; i != sizeof(T) * 8; ++i)
				if (const T mask{ static_cast<T>(T{ 1 } << i) }; mask & x)
					return i + 1;
			return 0;
		}
		else
		{
#if CLANG || GCC
			if constexpr (sizeof(T) <= 4)
				return __builtin_ffs(x);
			else
				return __builtin_ffsll(x);
#else
			unsigned long result;
			if constexpr (sizeof(T) <= 4)
			{
				if (!_BitScanForward(&result, x))
					return 0;
			}
			else
			{
				if (!_BitScanForward64(&result, x))
					return 0;
			}
			return static_cast<u8>(result + 1);
#endif
		}
	}

	NO_SIDE_EFFECTS INLINE constexpr u8 leftZeroCount($UInt auto x)noexcept
	{
		if consteval
		{
			u8 depth{ u8(-1) };
			while (x)
			{
				x >>= 1;
				++depth;
			}
			return sizeof(x) * 8 - 1 - depth;
		}
		else
		{
			if constexpr (sizeof(x) <= 2)
				return static_cast<u8>(__lzcnt16(x)) - compact<8 * (2 - sizeof(x))>;
			else if constexpr (sizeof(x) == 4)
				return static_cast<u8>(_lzcnt_u32(x));
			else if constexpr (sizeof(x) == 8)
			{
			#if BITNESS == 64
				return static_cast<u8>(_lzcnt_u64(x));
			#else
				const u32 high{ static_cast<u32>(x >> 32) };
				return high ? (u8)_lzcnt_u32(high) : u8(32 + _lzcnt_u32(static_cast<u32>(x)));
			#endif
			}
		}
	}
	STATIC_ASSERT(leftZeroCount( u8{}) == 8 );
	STATIC_ASSERT(leftZeroCount(u16{}) == 16);
	STATIC_ASSERT(leftZeroCount(u32{}) == 32);
	STATIC_ASSERT(leftZeroCount(u64{}) == 64);

	NO_SIDE_EFFECTS INLINE constexpr u8 bitLength($UInt auto x)noexcept
	{
		return static_cast<u8>(sizeof(x) * 8 - leftZeroCount(x));
	}
	NO_SIDE_EFFECTS INLINE constexpr u8 rightZeroCount($UInt auto x)noexcept
	{
		using T = std::remove_cvref_t<decltype(x)>;
		if consteval
		{
			for (u8 i{}; i != sizeof(T) * 8; ++i)
				if (x & T(T{ 1 } << i))
					return i;
			return sizeof(T) * 8;
		}
		else
		{
			/* */if constexpr (sizeof(T) == 1)return static_cast<u8>(_tzcnt_u16(static_cast<u16>(x | u16{ 0xFF00 })));
			else if constexpr (sizeof(T) == 2)return static_cast<u8>(_tzcnt_u16(x));
			else if constexpr (sizeof(T) == 4)return static_cast<u8>(_tzcnt_u32(x));
			else if constexpr (sizeof(T) == 8)
#if BITNESS == 64
				return static_cast<u8>(_tzcnt_u64(x));
#else
				return static_cast<u32>(x) ? static_cast<u8>(_tzcnt_u32(static_cast<u32>(x))) : static_cast<u8>(32 + _tzcnt_u32(static_cast<u32>(x >> 32)));
#endif
		}
	}
#if BMI2
	template<$UInt auto mask>
	requires(sizeof(mask) == 4 || sizeof(mask) == 8)
	NO_SIDE_EFFECTS INLINE constexpr UnsignedBits<count(mask)> pext(std::remove_cvref_t<decltype(mask)> x) noexcept
	{
		using T = std::remove_cvref_t<decltype(mask)>;
		using Ret = UnsignedBits<count(mask)>;
		static constexpr T lowBit{ mask & (T{} - mask) };
		if constexpr (!mask) return {};
		else if constexpr ((mask & (mask + lowBit)) == 0)
			return static_cast<Ret>((x & mask) >> constant<rightZeroCount(mask)>);
		else
		{
			if consteval
			{
				Ret result{};
				Ret bit{ 1 };
				for (T m{ mask }; m; m &= m - 1, bit <<= 1)
					if (x & m & (T{} - m))
						result |= bit;
				return result;
			}
			else
			{
				if constexpr (mask <= 0xFFFFFFFFu) return static_cast<Ret>(_pext_u32(static_cast<u32>(x), static_cast<u32>(mask)));
			#if BITNESS == 64
				else return static_cast<Ret>(_pext_u64(x, mask));
			#else
				else
				{
					static constexpr u32 maskLo{ static_cast<u32>(mask) };
					static constexpr u32 maskHi{ static_cast<u32>(mask >> 32) };
					static constexpr u8  rZeros{ rightZeroCount(mask) };
					if constexpr (!maskLo)
						return pext<maskHi>(static_cast<u32>(x >> 32));
					else if constexpr ((mask >> rZeros) <= 0xFFFFFFFFu)
						return pext<static_cast<u32>(mask >> rZeros)>(static_cast<u32>(x >> rZeros));
					else
						return static_cast<Ret>(Ret{ pext<maskLo>(static_cast<u32>(x)) } | (Ret{ pext<maskHi>(static_cast<u32>(x >> 32)) } << constant<count(maskLo)>));
				}
			#endif
			}
		}
	}
#endif
}