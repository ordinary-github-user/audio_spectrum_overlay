#pragma once
#include <array>
#include <span>
#include <utility>
#include "../BasicTypes.h"
#include "../Compact.h"
#include "../Constant.h"
#include "../StaticAssert.h"
#include "CArray.h"
namespace details
{
	template<class>constexpr bool IsStdArrayImpl{ false };
	template<class T, usize N>constexpr bool IsStdArrayImpl<std::array<T, N>>{true};

	template<class>constexpr bool IsStdSpanFixedImpl{ false };
	template<class T, usize N>requires(N != std::dynamic_extent)constexpr bool IsStdSpanFixedImpl<std::span<T, N>>{true};

	template<class>struct SpanSizeImpl;
	template<class T, usize N>requires(N != std::dynamic_extent)struct SpanSizeImpl<std::span<T, N>> { static constexpr auto value{ compact<N> }; };

	template<class>struct StdArraySize;
	template<class T, usize N>struct StdArraySize<std::array<T, N>> { static constexpr auto value{compact<N>}; };
}

template<class T>concept $FixSized = requires
{
	usize{ constant<std::remove_cvref_t<T>::size()> };
} || $CArray<T> || details::IsStdArrayImpl<std::remove_cvref_t<T>> || details::IsStdSpanFixedImpl<std::remove_cvref_t<T>>;

template<$FixSized T>
constexpr auto static_size{ [] static
{
	using U = std::remove_cvref_t<T>;
	if constexpr (details::IsCArray<U>)
		return details::CArraySize<U>::value;
	else if constexpr (details::IsStdArrayImpl<U>)
		return details::StdArraySize<U>::value;
	else if constexpr (details::IsStdSpanFixedImpl<U>)
		return details::SpanSizeImpl<U>::value;
	else
		return compact<U::size()>;
}() };

STATIC_ASSERT(static_size<std::array<bool, 0>> == 0);
STATIC_ASSERT(static_size<std::array<bool, 1>> == 1);
STATIC_ASSERT(static_size<std::array<bool, 2>> == 2);

STATIC_ASSERT(static_size<const std::array<bool, 0>> == 0);
STATIC_ASSERT(static_size<volatile std::array<bool, 1>> == 1);
STATIC_ASSERT(static_size<const volatile std::array<bool, 2>> == 2);

STATIC_ASSERT(static_size<bool[0]> == 0);
STATIC_ASSERT(static_size<bool[1]> == 1);
STATIC_ASSERT(static_size<bool[2]> == 2);

STATIC_ASSERT(static_size<const bool(&)[0]> == 0);
STATIC_ASSERT(static_size<volatile bool(&&)[1]> == 1);
STATIC_ASSERT(static_size<const volatile bool(&&)[2]> == 2);