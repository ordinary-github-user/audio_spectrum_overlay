#pragma once
#include "Inline.h"
template<auto Value>
struct ImmediateValue
{
	using type = decltype(Value);
	[[nodiscard]] INLINE constexpr type operator()()const noexcept { return Value; }
	[[nodiscard]] INLINE constexpr operator type()const noexcept { return Value; }
};

template<auto Value>
constexpr ImmediateValue<Value> IV{};