#pragma once
#include "Compiler.h"
#include <cstdint>
#include <utility>
#if defined(_LIBCPP_VERSION) && BITNESS == 64
	using i128 = __int128;
	using u128 = unsigned __int128;
#elif MSVC
	#include <__msvc_int128.hpp>
	using i128 = std::_Signed128;
	using u128 = std::_Unsigned128;
#endif
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using usize = std::size_t;
using isize = std::intptr_t;

using f32 = float;
using f64 = double;
using f128 = long double;

using c8 = char;
#if MSVC || CLANG_CL
	using c16 = wchar_t;
#elif GCC || CLANG
	using c16 = std::conditional_t<sizeof(wchar_t) == 2, wchar_t, char16_t>;
#endif
using c32 = char32_t;

consteval i8  operator""_i8 (unsigned long long v)noexcept { return static_cast<i8 >(v); }
consteval i16 operator""_i16(unsigned long long v)noexcept { return static_cast<i16>(v); }
consteval i32 operator""_i32(unsigned long long v)noexcept { return static_cast<i32>(v); }
consteval i64 operator""_i64(unsigned long long v)noexcept { return static_cast<i64>(v); }
consteval u8  operator""_u8 (unsigned long long v)noexcept { return static_cast<u8 >(v); }
consteval u16 operator""_u16(unsigned long long v)noexcept { return static_cast<u16>(v); }
consteval u32 operator""_u32(unsigned long long v)noexcept { return static_cast<u32>(v); }
consteval u64 operator""_u64(unsigned long long v)noexcept { return static_cast<u64>(v); }