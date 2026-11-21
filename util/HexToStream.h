#pragma once
#include "ConceptPointer.h"
#include "Int2Chars.h"
template<$UInt T, bool FullWidth, c8 Padding = ' ', bool Cap = true>
requires(!$Same<bool, T>)
struct Hex
{
	T value;
	INLINE constexpr Hex(T x)noexcept :value{ x } {}
	INLINE Hex(const void* ptr)noexcept requires($Same<usize, T>) : value{ std::bit_cast<usize>(ptr) } {}
};

template<$UInt T>Hex(T) -> Hex<T, false, ' ', true>;
template<class T>Hex(T*) -> Hex<usize, false, ' ', true>;

template<bool FullWidth = 1, c8 Padding = ' ', bool Cap = true, class T>
requires($Pointer<T> || $UInt<T>)
INLINE constexpr Hex<std::conditional_t<std::is_pointer_v<T>, usize, T>, FullWidth, Padding, Cap> hex(T x)noexcept { return { x }; }

template<class Stream, class T, bool FullWidth, c8 Padding, bool Cap>
constexpr Stream& operator<<(Stream& os, Hex<T, FullWidth, Padding, Cap> x)noexcept
{
	using Char = typename std::remove_cvref_t<Stream>::char_type;
	static constexpr Capitalized cap{ Cap ? Capitalized::yes : Capitalized::no };
	if constexpr (FullWidth)
	{
		const auto buf{ uhex2chars_fixed<Padding, cap, Char>(x.value) };
		os.write(buf.data(), buf.size());
	}
	else
	{
		const auto buf{ uhex2chars<cap, Char>(x.value) };
		os.write(buf.data(), buf.size());
	}
	return os;
}