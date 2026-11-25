#pragma once
#include "../Concepts/Pointer.h"
#include "Int2Chars.h"
template<$UInt T, bool FullWidth, c8 Padding = ' '>
requires(!$Same<bool, T>)
struct Bin
{
	T value;
	INLINE constexpr Bin(T x)noexcept :value{ x } {}
	INLINE Bin(const void* ptr)noexcept requires($Same<usize, T>) : value{ std::bit_cast<usize>(ptr) } {}
};

template<$UInt T>Bin(T) -> Bin<T, false>;
template<class T>Bin(T*) -> Bin<usize, false>;

template<bool FullWidth = 1, c8 Padding = ' ', class T>
requires($Pointer<T> || $UInt<T>)
INLINE constexpr Bin<std::conditional_t<std::is_pointer_v<T>, usize, T>, FullWidth, Padding> bin(T x)noexcept { return { x }; }

template<class Stream, class T, bool FullWidth, c8 Padding>
constexpr Stream& operator<<(Stream& os, Bin<T, FullWidth, Padding> x)noexcept
{
	using Char = typename std::remove_cvref_t<Stream>::char_type;

	if constexpr (FullWidth)
	{
		const auto buf{ ubin2chars_fixed<Padding, Char>(x.value) };
		os.write(buf.data(), buf.size());
	}
	else
	{
		const auto buf{ ubin2chars<Char>(x.value) };
		os.write(buf.data(), buf.size());
	}
	return os;
}