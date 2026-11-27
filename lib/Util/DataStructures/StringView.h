#pragma once
#include "../StringViewImpl.h"
#include "../TextEncoding/TextEncoding.h"
template<class Stream, class T>
INLINE constexpr Stream& operator<<(Stream& os, StringView<T> s)noexcept
requires($Same<typename Stream::char_type, std::remove_cv_t<T>>)
{
	os.write(s.data(), s.size());
	return os;
}
template<class Stream, class T>
INLINE constexpr Stream& operator<<(Stream& os, StringView<T> s)noexcept
requires(sizeof(T) == 1 && sizeof(typename Stream::char_type) == 2)
{
	Utf8::decode(s, [&](const u32 codePoint)LAMBDA_INLINE{ Utf16::encode(codePoint,[&](const c16 unit)LAMBDA_INLINE{os.put(unit); }); });
	return os;
}