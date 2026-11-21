#pragma once
#include "BasicTypes.h"
#include "Compiler.h"
#include "ConceptIntegral.h"
#include "Inline.h"
#include "Pure.h"
#include <bit>
#if MSVC || CLANG_CL
	#include <intrin.h>
#endif

namespace Bits
{
	template<$UInt T>
	NO_SIDE_EFFECTS INLINE constexpr T rotL(T l, u8 r)noexcept
	{
#if GCC
		return std::rotl(l, r);
#elif CLANG
		/* */if constexpr (sizeof(T) == 1)return __builtin_rotateleft8(l, r);
		else if constexpr (sizeof(T) == 2)return __builtin_rotateleft16(l, r);
		else if constexpr (sizeof(T) == 4)return __builtin_rotateleft32(l, r);
		else if constexpr (sizeof(T) == 8)return __builtin_rotateleft64(l, r);
#elif MSVC
		if consteval
		{
			return std::rotl(l, r);
		}
		else
		{
			/* */if constexpr (sizeof(T) == 1)return _rotl8(l, r);
			else if constexpr (sizeof(T) == 2)return _rotl16(l, r);
			else if constexpr (sizeof(T) == 4)return _rotl(l, r);
			else if constexpr (sizeof(T) == 8)return _rotl64(l, r);
		}
#endif
	}

	template<$UInt T>
	NO_SIDE_EFFECTS INLINE constexpr T rotR(T l, u8 r)noexcept
	{
#if GCC
		return std::rotr(l, r);
#elif CLANG
		/* */if constexpr (sizeof(T) == 1)return __builtin_rotateright8(l, r);
		else if constexpr (sizeof(T) == 2)return __builtin_rotateright16(l, r);
		else if constexpr (sizeof(T) == 4)return __builtin_rotateright32(l, r);
		else if constexpr (sizeof(T) == 8)return __builtin_rotateright64(l, r);
#elif MSVC
		if consteval
		{
			return std::rotr(l, r);
		}
		else
		{
			/* */if constexpr (sizeof(T) == 1)return _rotr8(l, r);
			else if constexpr (sizeof(T) == 2)return _rotr16(l, r);
			else if constexpr (sizeof(T) == 4)return _rotr(l, r);
			else if constexpr (sizeof(T) == 8)return _rotr64(l, r);
		}
#endif
	}
}