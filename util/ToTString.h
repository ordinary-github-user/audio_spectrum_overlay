#pragma once
#include "StringBuilderImpl.h"
#include "StaticString.h"
#include "GrowableArray.h"
template<auto...args>
constexpr auto toTString8{ [] static
{
	static constexpr auto result{StringBuilderImpl<c8, StaticString<c8, StringBuilderImpl<c8, GrowableArray<c8>>{ args... }.str().size()>>{ args... }.str()};
	return[]<usize...I>(std::index_sequence<I...>)static
	{
		return TString<c8, result[I]...>{};
	}(std::make_index_sequence<result.size()>{});
}() };