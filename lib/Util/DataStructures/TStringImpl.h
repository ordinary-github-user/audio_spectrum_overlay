#pragma once
#include "../Compiler.h"
#include "../Concepts/Same.h"
#include "../Strcmp.h"
#include "../TextEncoding/TextEncodingStringConvertStatic.h"
#include "FixedStringImpl.h"
template<class T, T...Cs>
struct TString
{
	using char_type = T;
	static constexpr auto size{ compact<sizeof...(Cs)> };
	static constexpr FixedString<T, sizeof...(Cs)> chars{ Cs... };
};
template<class T, T...l, T...r> consteval TString<T, l..., r...> operator+(TString<T, l...>, TString<T, r...>)noexcept { return{}; }
template<class T, T...l, T...r> consteval bool operator==(TString<T, l...>, TString<T, r...>)noexcept { return  $Same<TString<T, l...>, TString<T, r...>>; }
template<class T, T...l, T...r> consteval bool operator!=(TString<T, l...>, TString<T, r...>)noexcept { return !$Same<TString<T, l...>, TString<T, r...>>; }
template<class T, T...cs>FLATTEN INLINE constexpr bool operator==(TString<T, cs...> l, $String auto&& r)noexcept{ return  equal  <encode<l.chars, ElementOf<decltype(r)>>>(r);}
template<class T, T...cs>FLATTEN INLINE constexpr bool operator==($String auto&& l, TString<T, cs...> r)noexcept{ return  equal  <encode<r.chars, ElementOf<decltype(l)>>>(l);}
template<class T, T...cs>FLATTEN INLINE constexpr bool operator!=(TString<T, cs...> l, $String auto&& r)noexcept{ return !equal  <encode<l.chars, ElementOf<decltype(r)>>>(r);}
template<class T, T...cs>FLATTEN INLINE constexpr bool operator!=($String auto&& l, TString<T, cs...> r)noexcept{ return !equal  <encode<r.chars, ElementOf<decltype(l)>>>(l);}
template<class T, T...cs>FLATTEN INLINE constexpr bool operator< (TString<T, cs...> l, $String auto&& r)noexcept{ return  less   <encode<l.chars, ElementOf<decltype(r)>>>(r);}
template<class T, T...cs>FLATTEN INLINE constexpr bool operator< ($String auto&& l, TString<T, cs...> r)noexcept{ return  greater<encode<r.chars, ElementOf<decltype(l)>>>(l);}
template<class T, T...cs>FLATTEN INLINE constexpr bool operator> (TString<T, cs...> l, $String auto&& r)noexcept{ return  greater<encode<l.chars, ElementOf<decltype(r)>>>(r);}
template<class T, T...cs>FLATTEN INLINE constexpr bool operator> ($String auto&& l, TString<T, cs...> r)noexcept{ return  less   <encode<r.chars, ElementOf<decltype(l)>>>(l);}

namespace details
{
	template<class T>constexpr bool is_tstring{};
	template<class T, T ... c>constexpr bool is_tstring<TString<T, c...>>{true};
}

template<class T>concept $TString = details::is_tstring<std::remove_cvref_t<T>>;
template<class T, class Char>concept $TStringOf = details::is_tstring<std::remove_cvref_t<T>> && $Same<typename std::remove_cvref_t<T>::char_type, Char>;

template<FixedString s>
constexpr auto tstring{ [] <class T, T...I>(std::integer_sequence<T, I...>)static->TString<typename decltype(s)::char_type, s[I]...>
{
	return {};
}(std::make_integer_sequence<Compact<s.size()>, s.size()>{})};

template<bool b, FixedString t, FixedString f>constexpr auto tstring_if/*         */{ tstring<t> };
template</*    */FixedString t, FixedString f>constexpr auto tstring_if<false, t, f>{ tstring<f> };

#if 0
	#define TS(s) tstring<s>
#else
	#if CLANG || GCC
		template<class T, T...Cs>consteval TString<T, Cs...> operator""_tstring()noexcept { return {}; }
		#define TS(s) s ## _tstring
	#else
		template<FixedString s, usize...I>
		consteval TString<typename decltype(s)::char_type, s[I]...> tstring_impl(std::index_sequence<I...>)noexcept { return {}; }
		#define TS(s) tstring_impl<s>(std::make_index_sequence<sizeof(s)/sizeof(s[0]) - 1>{})
	#endif
#endif