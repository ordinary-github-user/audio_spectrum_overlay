#pragma once
#include "Compact.h"
#include "ConceptSame.h"
#include "Consteval.h"
template<class T, class...Ts>
requires(($Same<T, Ts>+...) == 1)
constexpr Compact<sizeof...(Ts)> index_of_type{ [] static
{
	Compact<sizeof...(Ts)> result{};
	(void)((!$Same<T, Ts> ? (++result, true) : false)&&...);
	return result;
}() };

CONSTEVAL bool relaxed_equal(auto l, auto r)noexcept
{
	if constexpr (requires{l == r; })return l == r;
	else return false;
}

template<auto X, auto...Xs>
requires((relaxed_equal(X, Xs) + ...) == 1)
constexpr Compact<sizeof...(Xs)> index_of_value{ [] static
{
	Compact<sizeof...(Xs)> result{};
	(void)((!relaxed_equal(X, Xs) ? (++result, true) : false)&&...);//in case != is faulty
	return result;
}() };