#pragma once
#include "Compiler.h"
#include "Consteval.h"
#include "MacroIf.h"
#include "TStringImpl.h"
template<$SizeAnyOf<1, 2, 4> Char, Char...Cs>
struct RdataString
{
	static constexpr Char m_data[]{ Cs... };
	INLINE IF(MSVC, constexpr, CONSTEVAL) RdataString()noexcept {}
	INLINE IF(MSVC, constexpr, CONSTEVAL) RdataString(TString<Char, Cs...>)noexcept{}
	INLINE static IF(MSVC, constexpr, CONSTEVAL) Compact<sizeof...(Cs)> size() noexcept { return sizeof...(Cs); }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* data()noexcept { return m_data; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* begin()noexcept { return m_data; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* end()noexcept { return data() + size(); }
	INLINE static constexpr Char operator[](Compact<sizeof...(Cs)> index) noexcept { return data()[index]; }
};

template<$SizeAnyOf<1, 2, 4> Char>
struct RdataString<Char>
{
	using char_type = Char;
	INLINE IF(MSVC, constexpr, CONSTEVAL) RdataString()noexcept {}
	INLINE IF(MSVC, constexpr, CONSTEVAL) RdataString(TString<Char>)noexcept {}
	INLINE static IF(MSVC, constexpr, CONSTEVAL) u8 size() noexcept { return {}; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* data() noexcept { return nullptr; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* begin() noexcept { return nullptr; }
	INLINE static IF(MSVC, constexpr, CONSTEVAL) const Char* end() noexcept { return nullptr; }
	INLINE static constexpr Char operator[](u8) noexcept;
};

template<class Char, Char...Cs>
RdataString(TString<Char, Cs...>) -> RdataString<Char, Cs...>;

template<class Stream, class Char, Char...Cs>
requires(requires(Stream& os, const Char* p, usize s) { os.write(p, s); })
INLINE Stream& operator<<(Stream& os, const RdataString<Char, Cs...>& str)noexcept
{
	if constexpr (sizeof...(Cs))
		os.write(str.data(), sizeof...(Cs));
	return os;
}

#if CLANG || GCC
	template<class T, T...Cs>IF(MSVC, constexpr, CONSTEVAL) RdataString<c8, Cs...> operator""_rstring8() { return {}; }
	template<class T, T...Cs>IF(MSVC, constexpr, CONSTEVAL) RdataString<c16, Cs...> operator""_rstring16() { return {}; }
	template<class T, T...Cs>IF(MSVC, constexpr, CONSTEVAL) RdataString<c32, Cs...> operator""_rstring32() { return {}; }

	#define RS8(s)  s ## _rstring8
	#define RS16(s) s ## _rstring16
	#define RS32(s) s ## _rstring32
#else
	namespace details::rdatastring
	{
		template<class Char, FixedString s, usize...I>
		requires(sizeof(Char) == sizeof(typename decltype(s)::char_type))
		IF(MSVC, constexpr, CONSTEVAL) RdataString<Char, s[I]...> make_string(std::index_sequence<I...>)noexcept
		{
			return {};
		}
	}
	#define RS8(s) (::details::rdatastring::make_string<c8,s>(std::make_index_sequence<sizeof(s)/1 - 1>{}))
	#define RS16(s) (::details::rdatastring::make_string<c16,s>(std::make_index_sequence<sizeof(s)/2 - 1>{}))
	#define RS32(s) (::details::rdatastring::make_string<c32,s>(std::make_index_sequence<sizeof(s)/4 - 1>{}))
#endif