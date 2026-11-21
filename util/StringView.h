#pragma once
#include "StringViewImpl.h"
#include "ConceptPrintable.h"
#include "StaticError.h"
#include "TextEncoding.h"
template<class Stream, $Printable<Stream> T>
INLINE constexpr Stream& operator<<(Stream& os, const StringView<T>& s)noexcept
{
	using type = std::remove_cv_t<T>;
	if constexpr ($Same<typename Stream::char_type, type>)
		os.write(s.data(), s.size());
	else if constexpr (sizeof(T) == 1 && sizeof(typename Stream::char_type) == 2)
		Utf8::decode(s, [&](const u32 codePoint)LAMBDA_INLINE{ Utf16::encode(codePoint,[&](const c16 unit)LAMBDA_INLINE{os.put(unit); }); });
	else
		STATIC_ERROR();
	return os;
}