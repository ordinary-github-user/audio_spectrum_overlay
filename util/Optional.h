#pragma once
#include "ConceptPrintable.h"
#include "OptionalImpl.h"
#include "TString.h"
template<class Stream, $Printable<Stream> T, T...default_value>
constexpr Stream& operator<<(Stream& os, const Opt<T, default_value...>& v)noexcept
{
	if (v.has_value())
		os << v.value();
	else
		os << TS("null");
	return os;
}