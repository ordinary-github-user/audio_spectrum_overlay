#pragma once
#include "BasicTypes.h"
#include "Inline.h"
#include "MacroMovForward.h"
#include "TypeAt.h"

#if defined(__cpp_pack_indexing)
	template<usize N, class...Ts>
	requires(N < sizeof...(Ts))
	[[nodiscard]] INLINE constexpr Ts...[N]&& element_at(Ts&&...args)noexcept
	{
		return FWD(args...[N]);
	}
#elif MSVC//msvc bug
	template<class...>struct ElementAtImpl;

	template<usize...I, class...Ts>
	struct ElementAtImpl<std::index_sequence<I...>, Ts...>
	{
		INLINE static constexpr decltype(auto) impl(TYPE_AT<I, Ts...>&&..., auto&& result, auto&&...)noexcept { return FWD(result); }
	};

	template<usize N, class...Ts>
	requires(N < sizeof...(Ts))
	[[nodiscard]] INLINE constexpr TYPE_AT<N, Ts...>&& element_at(Ts&&...args)noexcept
	{
		return ElementAtImpl<std::make_index_sequence<N>, Ts...>::impl(FWD(args)...);
	}
#else
	template<usize N, class...Ts>
	requires(N < sizeof...(Ts))
	[[nodiscard]] INLINE constexpr TYPE_AT<N, Ts...>&& element_at(Ts&&...args)noexcept
	{
		static constexpr auto impl{ [] <usize...I>(std::index_sequence<I...>)static
		{
			return [](TYPE_AT<I, Ts...>&&..., auto&& result, auto&&...)STATIC_LAMBDA_INLINE->TYPE_AT<N, Ts...>&& {return FWD(result); };
		}(std::make_index_sequence<N>{}) };
		return impl(FWD(args)...);
	}
#endif

STATIC_ASSERT([]
{
	int a [[maybe_unused]] {};
	return std::is_same_v<int&, decltype(element_at<0>(a))>;
}(), "&");

STATIC_ASSERT([]
{
	const int a [[maybe_unused]] {};
	return std::is_same_v<const int&, decltype(element_at<0>(a))>;
}(), "const &");

STATIC_ASSERT([]
{
	volatile int a [[maybe_unused]] {};
	return std::is_same_v<volatile int&, decltype(element_at<0>(a))>;
}(), "volatile &");

STATIC_ASSERT([]
{
	const volatile int a [[maybe_unused]] {};
	return std::is_same_v<const volatile int&, decltype(element_at<0>(a))>;
}(), "const volatile &");

STATIC_ASSERT([]
{
	int a [[maybe_unused]] {};
	return std::is_same_v<int&&, decltype(element_at<0>(MOV(a)))>;
}(), "&&");

STATIC_ASSERT([]
{
	const int a [[maybe_unused]] {};
	return std::is_same_v<const int&&, decltype(element_at<0>(MOV(a)))>;
}(), "const &&");

STATIC_ASSERT([]
{
	volatile int a [[maybe_unused]] {};
	return std::is_same_v<volatile int&&, decltype(element_at<0>(MOV(a)))>;
}(), "volatile &&");

STATIC_ASSERT([]
{
	const volatile int a [[maybe_unused]] {};
	return std::is_same_v<const volatile int&&, decltype(element_at<0>(MOV(a)))>;
}(), "const volatile &&");