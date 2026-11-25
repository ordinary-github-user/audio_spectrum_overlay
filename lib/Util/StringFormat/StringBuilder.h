#pragma once
#include "../Concepts/CArray.h"
#include "../Concepts/Char.h"
#include "../Concepts/Floating.h"
#include "../Concepts/Pointer.h"
#include "../Concepts/Printable.h"
#include "../DataStructures/StringSso.h"
#include "../Macro/MovForward.h"
#include "Num2Chars.h"
#include "StringBuilderImpl.h"
using StringBuilder8 = StringBuilderImpl<c8, BasicString<c8>>;
using StringBuilder16 = StringBuilderImpl<c16, BasicString<c16>>;
using StringBuilder32 = StringBuilderImpl<c32, BasicString<c32>>;
template<class Char, class Buffer>
constexpr StringBuilderImpl<Char, Buffer>& operator<<(StringBuilderImpl<Char, Buffer>& os, $Same<bool> auto value)noexcept
{
	os.put(value ? '1' : '0');
	return os;
}

template<class Char, class Buffer>
constexpr StringBuilderImpl<Char, Buffer>& operator<<(StringBuilderImpl<Char, Buffer>& os, $Char auto value)noexcept
requires requires{os.put(value); }
{
	os.put(value);
	return os;
}

template<class Char, class Buffer>
constexpr StringBuilderImpl<Char, Buffer>& operator<<(StringBuilderImpl<Char, Buffer>& os, $Floating auto value)noexcept
{
	const StaticString s{ f2chars<Char>(value) };
	os.write(s.data(), s.size());
	return os;
}

template<class Char, class Buffer>
constexpr StringBuilderImpl<Char, Buffer>& operator<<(StringBuilderImpl<Char, Buffer>& os, $Int auto value)noexcept
requires(!$Same<bool, decltype(value)> && !$Char<decltype(value)>)
{
	if constexpr ($SInt<decltype(value)>)
	{
		const StaticString s{ sdec2chars<Char>(value) };
		os.write(s.data(), s.size());
	}
	else
	{
		const StaticString s{ udec2chars<Char>(value) };
		os.write(s.data(), s.size());
	}
	return os;
}

template<class Char, class Buffer>
constexpr StringBuilderImpl<Char, Buffer>& operator<<(StringBuilderImpl<Char, Buffer>& os, $CArrayOf<Char> auto&& value)noexcept
{
	os.write(value, compact<carray_size<decltype(value)> -1>);
	return os;
}

template<class Char, class Buffer>
constexpr StringBuilderImpl<Char, Buffer>& operator<<(StringBuilderImpl<Char, Buffer>& os, $Pointer auto value)noexcept
{
	const StaticString s{ uhex2chars<Capitalized::yes, Char>(std::bit_cast<usize>(value)) };
	os.write(s.data(), s.size());
	return os;
}

[[deprecated]] constexpr String16 toString16($Printable<StringBuilder16> auto&&...args)noexcept
{
	STATIC_ASSERT(sizeof...(args) >= 1);
	STATIC_ASSERT(!(sizeof...(args) == 1 && ($String<decltype(args)>&&...)));
	StringBuilder16 os;
	((os << FWD(args)), ...);
	return MOV(os.str());
}

[[deprecated]] constexpr String8 toString8($Printable<StringBuilder8> auto&&...args)noexcept
{
	STATIC_ASSERT(sizeof...(args) >= 1);
	STATIC_ASSERT(!(sizeof...(args) == 1 && ($String<decltype(args)>&&...)));
	StringBuilder8 os;
	((os << FWD(args)), ...);
	return MOV(os.str());
}

//template<$Printable<StringBuilder8>  T>requires($LinearContainerOf<T, c8 >)constexpr decltype(auto) toString8 (T&& arg)noexcept{STATIC_ERROR();return FWD(arg);}
//template<$Printable<StringBuilder16> T>requires($LinearContainerOf<T, c16>)constexpr decltype(auto) toString16(T&& arg)noexcept{STATIC_ERROR();return FWD(arg);}