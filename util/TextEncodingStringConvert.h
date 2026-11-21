#pragma once
#include "TextEncodingStringConvertStatic.h"
#include "StringSso.h"
template<HaltAtNull haltAtNull = HaltAtNull::no, $Size<2> T>
constexpr String8 utf16ToUtf8(const T* pBegin, const T* pEnd) noexcept
{
	String8 result;
	result.reserve((pEnd - pBegin) * 3);
	Utf16::decode<haltAtNull>(pBegin, pEnd, [&](u32 codePoint)LAMBDA_INLINE
	{
		Utf8::encode(codePoint, [&](c8 codeUnit)LAMBDA_INLINE
		{
			result.unchecked_push_back(codeUnit);
		});
	});
	return result;
}

template<HaltAtNull haltAtNull = HaltAtNull::no>
constexpr String8 utf16ToUtf8($Size<2> auto const* pBegin, $UInt auto size)noexcept { return utf16ToUtf8<haltAtNull>(pBegin, pBegin + size); }

template<HaltAtNull haltAtNull = HaltAtNull::no>
constexpr String8 utf16ToUtf8($StringOfCharSize<2> auto&& str) noexcept
{
	return utf16ToUtf8<haltAtNull>(str.data(), str.data() + str.size());
}

template<HaltAtNull haltAtNull = HaltAtNull::no, $Size<1> T>
constexpr String16 utf8ToUtf16(const T* pBegin, const T* pEnd)noexcept
{
	String16 result;
	result.reserve(pEnd - pBegin);
	Utf8::decode<haltAtNull>(pBegin, pEnd, [&](u32 codePoint)LAMBDA_INLINE
	{
		Utf16::encode(codePoint, [&](c16 codeUnit)LAMBDA_INLINE
		{
			result.unchecked_push_back(codeUnit);
		});
	});
	return result;
}

template<HaltAtNull haltAtNull = HaltAtNull::no>
constexpr String16 utf8ToUtf16($Size<1> auto const* pBegin, $UInt auto size)noexcept { return utf8ToUtf16<haltAtNull>(pBegin, pBegin + size); }

template<HaltAtNull haltAtNull = HaltAtNull::no>
constexpr String16 utf8ToUtf16($StringOfCharSize<1> auto&& str)noexcept
requires(!$FixSized<decltype(str)>)
{ 
	return utf8ToUtf16<haltAtNull>(str.data(), str.data() + str.size());
}