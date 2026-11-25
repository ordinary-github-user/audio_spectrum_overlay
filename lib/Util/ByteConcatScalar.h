#pragma once
#include "BasicTypes.h"
#include "Concepts/Same.h"
#include "Inline.h"
namespace Bytes
{
	template<$Same<u8>  T>INLINE constexpr u8 concat(u8 v)noexcept { return v; }

	template<$Same<u16> T>INLINE constexpr u16 concat(u16 v)noexcept { return v; }
	template<$Same<u16> T>INLINE constexpr u16 concat(u8 v0, u8 v1) noexcept { return u16(v0) | (u16(v1) << 8); }

	template<$Same<u32> T>INLINE constexpr u32 concat(u32 v)noexcept { return v; }
	template<$Same<u32> T>INLINE constexpr u32 concat(u16 v0, u16 v1) noexcept { return u32(v0) | (u32(v1) << 16); }
	template<$Same<u32> T>INLINE constexpr u32 concat(u8 v0, u8 v1, u8 v2, u8 v3) noexcept { return u32(v0) | (u32(v1) << 8) | (u32(v2) << 16) | (u32(v3) << 24); }

	template<$Same<u64> T>INLINE constexpr u64 concat(u64 v)noexcept { return v; }
	template<$Same<u64> T>INLINE constexpr u64 concat(u32 v0, u32 v1) noexcept { return u64(v0) | (u64(v1) << 32); }
	template<$Same<u64> T>INLINE constexpr u64 concat(u16 v0, u16 v1, u16 v2, u16 v3) noexcept { return u64(v0) | (u64(v1) << 16) | (u64(v2) << 32) | (u64(v3) << 48); }
	template<$Same<u64> T>INLINE constexpr u64 concat(u8 v0, u8 v1, u8 v2, u8 v3, u8 v4, u8 v5, u8 v6, u8 v7) noexcept { return u64(v0) | (u64(v1) << 8) | (u64(v2) << 16) | (u64(v3) << 24) | (u64(v4) << 32) | (u64(v5) << 40) | (u64(v6) << 48) | (u64(v7) << 56); }
}