#pragma once
#include "StaticStringImpl.h"
#include "ConceptFixSized.h"
#include "ConceptSize.h"
#include "ConceptString.h"
#include "ElementOf.h"
#include "Inline.h"
#include "TextEncoding.h"
template<HaltAtNull haltAtNull = HaltAtNull::no, $StringOfCharSize<1> Container>
constexpr StaticString<c16, static_size<Container>> utf8ToUtf16(Container&& str)noexcept
requires($FixSized<Container>)
{
	static constexpr auto N{ static_size<Container> };
	StaticString<c16, N> result{};
	Utf8::decode<haltAtNull>(str.data(), str.data() + N, [&](u32 codePoint)LAMBDA_INLINE
	{
		Utf16::encode(codePoint, [&](c16 codeUnit)LAMBDA_INLINE
		{
			result.push_back(codeUnit);
		});
	});
	return result;
}
template<HaltAtNull haltAtNull = HaltAtNull::no, $StringOfCharSize<2> Container>
constexpr StaticString<c8, 3 * static_size<Container>> utf16ToUtf8(Container&& str)noexcept
requires($FixSized<Container>)
{
	static constexpr auto N{ static_size<Container> };
	StaticString<c8, 3 * N> result{};
	Utf16::decode<haltAtNull>(str.data(), str.data() + N, [&](u32 codePoint)LAMBDA_INLINE
	{
		Utf8::encode(codePoint, [&](c8 codeUnit)LAMBDA_INLINE
		{
			result.push_back(codeUnit);
		});
	});
	return result;
}
template<HaltAtNull haltAtNull = HaltAtNull::no, $Size<1> Char, usize N>
constexpr StaticString<c16, N> utf8ToUtf16(const StaticString<Char, N>& str)noexcept
{
	StaticString<c16, N> result{};
	Utf8::decode<haltAtNull>(str.data(), str.data() + str.size(), [&](u32 codePoint)LAMBDA_INLINE
	{
		Utf16::encode(codePoint, [&](c16 codeUnit)LAMBDA_INLINE
		{
			result.push_back(codeUnit);
		});
	});
	return result;
}
template<HaltAtNull haltAtNull = HaltAtNull::no, $Size<2> Char, usize N>
constexpr StaticString<c8, 3 * N> utf16ToUtf8(const StaticString<Char, N>& str)noexcept
{
	StaticString<c8, 3 * N> result{};
	Utf16::decode<haltAtNull>(str.data(), str.data() + str.size(), [&](u32 codePoint)LAMBDA_INLINE
	{
		Utf8::encode(codePoint, [&](c8 codeUnit)LAMBDA_INLINE
		{
			result.push_back(codeUnit);
		});
	});
	return result;
}

template<FixedString from, $CharOfSizeAny<1, 2> T, HaltAtNull haltAtNull = HaltAtNull::no>
requires($StringOfCharSizeAny<decltype(from), 1, 2>)
constexpr auto encode{ [] static
{
	using C = ElementOf<decltype(from)>;
	if constexpr (sizeof(C) == sizeof(T))return from;
	else if constexpr (sizeof(C) == 1 && sizeof(T) == 2)
	{
		static constexpr auto s{ utf8ToUtf16<haltAtNull>(from) };
		return FixedString<T, s.size()>{ s };
	}
	else if constexpr (sizeof(C) == 2 && sizeof(T) == 1)
	{
		static constexpr auto s{ utf16ToUtf8<haltAtNull>(from) };
		return FixedString<T, s.size()>{ s };
	}
}()};