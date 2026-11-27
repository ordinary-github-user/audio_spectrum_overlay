#pragma once
#include "../Constant.h"
#include "../TextEncoding/TextEncodingStringConvertStatic.h"
#include "OString.h"
#include "TStringImpl.h"
template<class Stream, class T, T...Cs>
requires(sizeof(typename Stream::char_type) == sizeof(T))
INLINE constexpr Stream& operator<<(Stream& os, TString<T, Cs...>)noexcept
{
	static constexpr auto length{ compact<sizeof...(Cs)> };
	/* */if constexpr (length == 1)os.put(constant<TString<T, Cs...>::chars[0]>);
	else if constexpr (length >= 2)
	{
		if consteval
		{
			static constexpr typename Stream::char_type str[sizeof...(Cs)]{ Cs... };
			os.write(str, length);
		}
		else
		{
			const OString s{ TString<typename Stream::char_type, Cs...>{} };
			os.write(s.data(), length);
		}
	}
	return os;
}

template<class Stream, class T, T...Cs>
requires(sizeof(typename Stream::char_type) == 2 && sizeof(T) == 1)
INLINE constexpr Stream& operator<<(Stream& os, TString<T, Cs...>)noexcept
{
	if constexpr (sizeof...(Cs))
		os << tstring<encode<FixedString{Cs...}, c16>>;
	return os;
}

template<class Stream, class T, T...Cs>
requires(sizeof(typename Stream::char_type) == 1 && sizeof(T) == 2)
INLINE constexpr Stream& operator<<(Stream& os, TString<T, Cs...>)noexcept
{
	if constexpr (sizeof...(Cs))
		os << tstring<encode<FixedString{Cs...}, c8>>;
	return os;
}