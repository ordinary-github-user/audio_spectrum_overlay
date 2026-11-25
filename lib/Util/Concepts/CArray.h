#pragma once
#include <utility>
#include "../BasicTypes.h"
#include "../Compact.h"
#include "../Compiler.h"
#include "Same.h"
namespace details
{
#if CLANG
	template<class T>
	constexpr bool IsCArray{ __array_rank(T) };

	template<class T>
	requires(IsCArray<T>)
	struct CArraySize
	{
		static constexpr auto value{ compact<__array_extent(T, 0)> };
	};
#else
	template<class>struct CArraySize;
	template<class T>struct CArraySize<T[0]> { static constexpr u8 value{}; };
	template<class T, usize N>struct CArraySize<T[N]> { static constexpr auto value{ compact<N> }; };

	template<class>constexpr bool IsCArray{};
	template<class T, usize N>constexpr bool IsCArray<T[N]>{true};
	template<class T>constexpr bool IsCArray<T[0]>{true};
#endif
}
template<class T>concept $CArray = details::IsCArray<std::remove_cvref_t<T>>;
template<class T, class E>concept $CArrayOf = $CArray<T> && $Same<E, std::remove_cvref_t<decltype(*std::declval<T>())>>;
template<class T, class...E>concept $CArrayAnyOf = $CArray<T> && ($Same<E, std::remove_cvref_t<decltype(*std::declval<T>())>> || ...);

template<$CArray T>
constexpr auto carray_size{ details::CArraySize<std::remove_cvref_t<T>>::value };

template<class Stream, class T>concept $PrintableCArray = std::is_array_v<std::remove_cvref_t<T>> && requires(std::remove_cvref_t<Stream>& os, const T& array) { os << array[0]; };