#pragma once
#include "../Compiler.h"
#include "TStringImpl.h"
template<$SizeAnyOf<1, 2, 4> Char, Char...Cs>
struct OString
{
	static constexpr Char m_data[]{ Cs... };
	INLINE consteval OString()noexcept {}
	INLINE consteval OString(TString<Char, Cs...>)noexcept{}
	INLINE static consteval Compact<sizeof...(Cs)> size() noexcept { return sizeof...(Cs); }
	INLINE static consteval const Char* data()noexcept { return m_data; }
	INLINE static consteval const Char* begin()noexcept { return m_data; }
	INLINE static consteval const Char* end()noexcept { return data() + size(); }
	INLINE static constexpr Char operator[](Compact<sizeof...(Cs)> index) noexcept { return data()[index]; }
};

template<$SizeAnyOf<1, 2, 4> Char>
struct OString<Char>
{
	using char_type = Char;
	INLINE consteval OString()noexcept {}
	INLINE consteval OString(TString<Char>)noexcept {}
	INLINE static consteval u8 size() noexcept { return {}; }
	INLINE static consteval const Char* data() noexcept { return nullptr; }
	INLINE static consteval const Char* begin() noexcept { return nullptr; }
	INLINE static consteval const Char* end() noexcept { return nullptr; }
	INLINE static constexpr Char operator[](u8) noexcept;
};

template<class Char, Char...Cs>
OString(TString<Char, Cs...>) -> OString<Char, Cs...>;

template<class Stream, class Char, Char...Cs>
requires(requires(Stream& os, const Char* p, usize s) { os.write(p, s); })
INLINE constexpr Stream& operator<<(Stream& os, const OString<Char, Cs...>& str)noexcept
{
	if constexpr (sizeof...(Cs))
		os.write(str.data(), sizeof...(Cs));
	return os;
}

#if CLANG || GCC
	template<class T, T...Cs>consteval OString<T, Cs...> operator""_rstring()noexcept { return {}; }
	#define OS(s) s ## _rstring
#else
	template<FixedString s, usize...I>
	consteval OString<typename decltype(s)::char_type, s[I]...> rstring_impl(std::index_sequence<I...>)noexcept { return {}; }
	#define OS(s) rstring_impl<s>(std::make_index_sequence<sizeof(s)/sizeof(s[0]) - 1>{})
#endif