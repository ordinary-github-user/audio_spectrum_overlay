#pragma once
#include <utility>
#include <array>
#include "BasicTypes.h"
template<class T, T...I>constexpr auto inverse(std::integer_sequence<T, I...>)noexcept 
{
	return[]<usize...J>(std::index_sequence<J...>)static
	{
		return std::integer_sequence<T, std::array<T, sizeof...(I)>{ I... }[sizeof...(J) - J - 1]...>{};
	}(std::make_index_sequence<sizeof...(I)>{});
}
template<class Seq>using Inverse = std::remove_const_t<decltype(inverse(Seq{}))>;
template<class T, T...L, T...R>constexpr std::integer_sequence<T, L..., R...>operator+(std::integer_sequence<T, L...>, std::integer_sequence<T, R...>)noexcept { return {}; }

template<class T, T Min, T Max>requires(Min <= Max)
using make_integer_range = std::remove_const_t<decltype([]<auto...I>(std::integer_sequence<T, I...>)static->std::integer_sequence<T, (Min + I)...>
{
	return {};
}(std::make_integer_sequence<T, Max - Min>{}))>;
template<usize Min, usize Max>requires(Min <= Max) using make_index_range = make_integer_range<usize, Min, Max>;

template<class T, T Min, T Max>requires(Min <= Max) using make_integer_range_inverse = decltype(inverse(make_integer_range<T, Min, Max>{}));
template<usize Min, usize Max>requires(Min <= Max) using make_index_range_inverse = make_integer_range_inverse<usize, Min, Max>;

template<class Stream, class T, T I0, T...I>
constexpr Stream& operator<<(Stream& os, std::integer_sequence<T, I0, I...>)noexcept
{
	os << '[' << I0;
	((os << ',' << I), ...);
	os << ']';
	return os;
}

template<class Stream, class T>
constexpr Stream& operator<<(Stream& os, std::integer_sequence<T>)noexcept
{
	os << '[' << ']'; return os;
}