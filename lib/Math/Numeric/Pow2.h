#pragma once
#include <utility>
#include "../../Util/Bits.h"
#include "../../Util/Compiler.h"
#include "../../Util/Concepts/Integral.h"
#include "../../Util/Macro/If.h"
#include "../../Util/Pure.h"
namespace Math
{
	NO_SIDE_EFFECTS INLINE constexpr bool isPowerOf2($UInt auto x)noexcept
	{
		return Bits::count(x) == u8{ 1 };
	}

	NO_SIDE_EFFECTS INLINE constexpr auto sqr(auto x)noexcept
	requires(IF(GCC, std::is_trivially_copyable_v<decltype(x)>, __is_trivially_copyable(decltype(x))) && sizeof(x) <= sizeof(u64))
	{
		return x * x;
	}
}