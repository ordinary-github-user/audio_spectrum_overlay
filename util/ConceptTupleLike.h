#pragma once
#include "BasicTypes.h"
#include "ConceptIntegral.h"
#include "ConceptSame.h"
#include <tuple>
#if MSVC//msvc bug
	template<class, class>
	constexpr bool tuple_like_impl{};
	template<class T, usize... I>
	constexpr bool tuple_like_impl<T, std::index_sequence<I...>>{($Same<std::remove_cvref_t<decltype(get<I>(std::declval<const std::remove_cvref_t<T>>()))>, std::remove_cvref_t<typename std::tuple_element<I, std::remove_cvref_t<T>>::type>> && ...)};
	template<class T>
	concept $TupleLike = requires { { std::tuple_size<std::remove_cvref_t<T>>::value } -> $UInt; } && tuple_like_impl<T, std::make_index_sequence<std::tuple_size<std::remove_cvref_t<T>>::value>>;
#else
	template<class T>
	concept $TupleLike = requires{{ std::tuple_size<std::remove_cvref_t<T>>::value }->$UInt;}
	&& []<usize...I>(std::index_sequence<I...>)static
	{
		return ($Same<std::remove_cvref_t<decltype(get<I>(std::declval<const std::remove_cvref_t<T>>()))>, std::remove_cvref_t<typename std::tuple_element<I, std::remove_cvref_t<T>>::type>>&&...);
	}(std::make_index_sequence<std::tuple_size<std::remove_cvref_t<T>>::value>{});
#endif
template<class T, usize N>
concept $TupleOfSize = $TupleLike<T> && (std::tuple_size<std::remove_cvref_t<T>>::value == N);