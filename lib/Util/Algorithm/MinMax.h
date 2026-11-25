#pragma once
#include "../BasicTypes.h"
#include "../Concepts/Same.h"
#include "../Inline.h"
namespace Algorithm
{
	template<class T>
	requires(__is_trivially_copyable(T) && sizeof(T) <= sizeof(u64))
	INLINE constexpr T min(T arg0, $Same<T> auto const...args)noexcept
	{
		(..., (arg0 = (args < arg0) ? args : arg0));
		return arg0;
	}

	template<class T>
	requires(__is_trivially_copyable(T) && sizeof(T) <= sizeof(u64))
	INLINE constexpr T max(T arg0, $Same<T> auto const...args)noexcept
	{
		(..., (arg0 = (args > arg0) ? args : arg0));
		return arg0;
	}
}