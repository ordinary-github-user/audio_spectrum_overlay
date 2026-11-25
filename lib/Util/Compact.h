#pragma once
#include <type_traits>
#include "Algorithm/MinMax.h"
#include "BasicTypes.h"
template<u64 x>
constexpr auto compact{ [] static
{
	/* */if constexpr (x <= u8 (-1))return u8 (x);
	else if constexpr (x <= u16(-1))return u16(x);
	else if constexpr (x <= u32(-1))return u32(x);
	else/*                        */return u64(x);
}()};
template<u64...x>requires(sizeof...(x) != 0)using Compact = std::remove_const_t<decltype(compact<Algorithm::max(x...)>)>;