#pragma once
#include "../../Compiler.h"
#include "../../Macro/Consteval.h"
#include "../../Macro/If.h"
#include "../TStringImpl.h"
template<$SizeAnyOf<1, 2, 4> Char, Char...Cs>
struct RString
{
	static constexpr Char m_data[]{ Cs... };
	INLINE IF(MSVC, constexpr, CONSTEVAL) RString()noexcept {}
	INLINE IF(MSVC, constexpr, CONSTEVAL) RString(TString<Char, Cs...>)noexcept{}
	INLINE static IF(MSVC, constexpr, CONSTEVAL) Compact<sizeof...(Cs)> size() noexcept { return sizeof...(Cs); }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* data()noexcept { return m_data; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* begin()noexcept { return m_data; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* end()noexcept { return data() + size(); }
	INLINE static constexpr Char operator[](Compact<sizeof...(Cs)> index) noexcept { return data()[index]; }
};

template<$SizeAnyOf<1, 2, 4> Char>
struct RString<Char>
{
	using char_type = Char;
	INLINE IF(MSVC, constexpr, CONSTEVAL) RString()noexcept {}
	INLINE IF(MSVC, constexpr, CONSTEVAL) RString(TString<Char>)noexcept {}
	INLINE static IF(MSVC, constexpr, CONSTEVAL) u8 size() noexcept { return {}; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* data() noexcept { return nullptr; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* begin() noexcept { return nullptr; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* end() noexcept { return nullptr; }
	INLINE static constexpr Char operator[](u8) noexcept;
};

template<class Char, Char...Cs>
RString(TString<Char, Cs...>) -> RString<Char, Cs...>;

template<class Stream, class Char, Char...Cs>
requires(requires(Stream& os, const Char* p, usize s) { os.write(p, s); })
INLINE Stream& operator<<(Stream& os, const RString<Char, Cs...>& str)noexcept
{
	if constexpr (sizeof...(Cs))
		os.write(str.data(), sizeof...(Cs));
	return os;
}

#if CLANG || GCC
	template<class T, T...Cs>consteval RString<T, Cs...> operator""_rstring()noexcept { return {}; }
	#define RS(s) s ## _rstring
#else
	template<FixedString s, usize...I>
	consteval RString<typename decltype(s)::char_type, s[I]...> rstring_impl(std::index_sequence<I...>)noexcept { return {}; }
	#define RS(s) rstring_impl<s>(std::make_index_sequence<sizeof(s)/sizeof(s[0]) - 1>{})
#endif