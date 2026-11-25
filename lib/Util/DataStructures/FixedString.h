#pragma once
#include "../TextEncoding/TextEncoding.h"
#include "FixedStringImpl.h"
template<class Stream, class T, usize N>
requires(sizeof(typename Stream::char_type) >= sizeof(T))
INLINE constexpr Stream& operator<<(Stream& os, const FixedString<T, N>& s)noexcept
{
	if constexpr (N > 0)
	{
		using type = std::remove_cv_t<T>;
		if constexpr ($Same<typename Stream::char_type, type>)
			os.write(s.data(), N);
		else if constexpr (sizeof(T) == 1 && sizeof(typename Stream::char_type) == 2)
			Utf8::decode(s, [&](const u32 codePoint)LAMBDA_INLINE{ Utf16::encode(codePoint,[&](const c16 unit)LAMBDA_INLINE{os.put(unit); }); });
		else
			STATIC_ERROR();
	}
	return os;
}