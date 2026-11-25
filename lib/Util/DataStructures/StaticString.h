#pragma once
#include "../TextEncoding/TextEncoding.h"
#include "StaticStringImpl.h"
template<class Stream, class Char, usize N>
constexpr Stream& operator<<(Stream& os, const StaticString<Char, N>& str)noexcept
{
	using char_t = typename Stream::char_type;
	if constexpr (sizeof(Char) == sizeof(char_t))
		os.write(str.data(), str.size());
	else if constexpr (sizeof(Char) == 1 && sizeof(typename Stream::char_type) == 2)
		Utf8::decode(str, [&](const u32 codePoint)LAMBDA_INLINE{ Utf16::encode(codePoint,[&](const c16 unit)LAMBDA_INLINE{os.put(unit); }); });
	else
		STATIC_ERROR(todo: encoding);
	return os;
}