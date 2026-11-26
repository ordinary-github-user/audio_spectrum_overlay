#pragma once
#include <array>
#include "Compact.h"
#include "Compiler.h"
#include "Concepts/Same.h"
#include "IndexOf.h"
#include "Iota.h"
#include "Macro/Consteval.h"
#include "Macro/If.h"
#include "Macro/Pragma.h"
#include "TypeAt.h"
template<class...Ts>
struct TypeList
{
	using Index = Compact<sizeof...(Ts) ? sizeof...(Ts) - 1 : 0>;
	static constexpr auto size{ compact<sizeof...(Ts)> };
	template<class T>static constexpr auto count{ compact<(usize{} + ... + $Same<Ts,T>)>};
	template<class T>static constexpr bool contains{ (... || $Same<Ts,T>) };
	template<class T>requires(count<T> == 1)static constexpr Index index_of{ index_of_type<T, Ts...> };
	template<template<class...>class Container> using make = Container<Ts...>;
	template<template<class...>class Mutator> using map = TypeList<typename Mutator<Ts>::type...>;
private:
#if __cpp_expansion_statements >= 202506L
	static constexpr std::array<bool, sizeof...(Ts)> is_duplicate{ [] static
	{
		std::array<bool, sizeof...(Ts)> result{};
		template for (constexpr auto I : iota<sizeof...(Ts)>)
			template for (constexpr auto J [[maybe_unused]] : iota<I>)
				if constexpr ($Same<Ts...[I], Ts...[J]>)
				{
					result[I] = true;
					IF(GCC, , break);//gcc bug: 'constexpr' call flows off the end of the function
				}
		return result;
	}() };
#else
	template<usize I>
	static constexpr bool is_duplicate_at{ [] <usize...J>(std::index_sequence<J...>) static
	{
		return (... || $Same<TYPE_AT<I, Ts...>, TYPE_AT<J, Ts...>>);
	}(std::make_index_sequence<I>{}) };

	static constexpr std::array<bool, sizeof...(Ts)> is_duplicate{ []<usize...I>(std::index_sequence<I...>) static
	{
		return std::array<bool, sizeof...(Ts)>{is_duplicate_at<I>...};
	}(std::make_index_sequence<sizeof...(Ts)>{}) };
#endif
	static constexpr auto unique_size{ compact<[]static
	{
		usize result{};
		for (const bool x : is_duplicate)
			result += !x;
		return result;
	}()>};

	static constexpr std::array<Compact<sizeof...(Ts)>, unique_size> indexes_of_uniques{ [] static
	{
		std::array<Compact<sizeof...(Ts)>, unique_size> result;
		auto* p{ result.data() };
		for (const auto i : iota<sizeof...(Ts)>)
			if (!is_duplicate[i])
				*p++ = i;
		return result;
	}() };
public:
	static constexpr bool contains_duplicates{ unique_size != size };
	using unique = decltype([]<usize...I>(std::index_sequence<I...>)static ->TypeList<TYPE_AT<indexes_of_uniques[I], Ts...>...>
	{
		return {};
	}(std::make_index_sequence<unique_size>{}));
	template<Index I>requires(I<sizeof...(Ts))using type_at = TYPE_AT<I, Ts...>;
private:
#if __cpp_structured_bindings >= 202411L && !GCC//gcc bug: internal compiler error: tree check: expected tree_vec, have array_ref in tsubst_pack_expansion, at cp/pt.cc:13999
	template<class condition, bool accept>
	static constexpr std::array<bool, sizeof...(Ts)> keep{ [] static->std::array<bool, sizeof...(Ts)>
	{
		static constexpr auto [...I] {iota<sizeof...(Ts)>};
		return { (accept == requires{ condition{}.template operator()<Ts...[I]>(); })... };
	}() };
#else//clang bug: error: constexpr variable 'keep' must be initialized by a constant expression
	PRAGMA_MSVC(warning(push)); PRAGMA_MSVC(warning(disable : 4268));//msvc bug: warning C4268: 'private: static std::array<bool,6> const TypeList<unsigned char,unsigned short,float,__int64,unsigned char,unsigned __int64>::keep<<lambda_100>,1>': 'const' static/global data initialized with compiler generated default constructor fills the object with zeros
	template<class condition, bool accept>
	static constexpr std::array<bool, sizeof...(Ts)> keep{ [] <usize...I>(std::index_sequence<I...>) static->std::array<bool, sizeof...(Ts)>
	{
		return { (accept == requires{ condition{}.template operator()<TYPE_AT<I, Ts...>>(); })...};
	}(std::make_index_sequence<sizeof...(Ts)>{}) };
	PRAGMA_MSVC(warning(pop));
#endif

	template<class condition, bool accept>
	static constexpr auto filtered_size{ compact<[]static
	{
		usize result{};
		for (const bool x : keep<condition, accept>)
			result += x;
		return result;
	}()> };

	PRAGMA_MSVC(warning(push)); PRAGMA_MSVC(warning(disable : 4268));//msvc bug: warning C4268: 'private: static std::array<unsigned char,4> const TypeList<unsigned char,unsigned short,float,__int64,unsigned char,unsigned __int64>::indexes_of_valid_ts<<lambda_113>,1>': 'const' static/global data initialized with compiler generated default constructor fills the object with zeros
	template<class condition, bool accept>
	static constexpr std::array<Compact<sizeof...(Ts)>, filtered_size<condition, accept>> indexes_of_valid_ts [[maybe_unused]] { [] static
	{
		std::array<Compact<sizeof...(Ts)>, filtered_size<condition, accept>> result;
		auto* p{ result.data() };
#if MSVC//msvc bug: error C3312: no callable 'begin' function found for type 'const auto'
		for (Compact<sizeof...(Ts)> i{}; i != sizeof...(Ts); ++i)
#else
		for (const auto i : iota<sizeof...(Ts)>)
#endif
			if (keep<condition, accept>[i])
				*p++ = i;
		return result;
	}() };
	PRAGMA_MSVC(warning(pop));
	template<class condition, bool accept>
	using filter_impl = decltype([]<usize...I>(std::index_sequence<I...>)static
	{
		return TypeList<TYPE_AT<indexes_of_valid_ts<condition, accept>[I], Ts...>...>{};
	}(std::make_index_sequence<filtered_size<condition, accept>>{}));
public:
	template<auto condition>using filter = filter_impl<decltype(condition), 1>;
	template<auto condition>using filter_out = filter_impl<decltype(condition), 0>;
};
template<class...L, class...R>CONSTEVAL bool operator==(TypeList<L...>, TypeList<R...>)noexcept { return $Same<TypeList<L...>, TypeList<R...>>; }
template<class...L, class...R>CONSTEVAL bool operator!=(TypeList<L...>, TypeList<R...>)noexcept { return !$Same<TypeList<L...>, TypeList<R...>>; }
template<class...L, class...R>CONSTEVAL TypeList<L..., R...>operator+(TypeList<L...>, TypeList<R...>)noexcept { return {}; }
template<class...Ts, class condition>CONSTEVAL auto operator&(TypeList<Ts...>, condition)noexcept { return typename TypeList<Ts...>::template filter<condition{}>{}; }
template<class condition, class...Ts>CONSTEVAL auto operator&(condition, TypeList<Ts...>)noexcept { return typename TypeList<Ts...>::template filter<condition{}>{}; }
template<class...Ts, class condition>CONSTEVAL auto operator-(TypeList<Ts...>, condition)noexcept { return typename TypeList<Ts...>::template filter_out<condition{}>{}; }
template<class...L, class...R>CONSTEVAL auto operator-(TypeList<L...>, TypeList<R...>)noexcept { return typename TypeList<L...>::template filter_out<[]<$AnyOf<R...>>()static{}>{}; }
namespace details
{
	template<class>constexpr bool is_type_list{};
	template<class...Ts>constexpr bool is_type_list<TypeList<Ts...>> {true};
}
template<class T>
concept $TypeList = details::is_type_list<std::remove_cvref_t<T>>;

template<class...Ts>
using UniqueTypes = TypeList<Ts...>::unique;

STATIC_ASSERT($Same<TypeList<>::map<std::remove_cvref>, TypeList<>>);
STATIC_ASSERT($Same<TypeList<u8, const u16, volatile u32, const volatile u64&&>::map<std::remove_cvref>, TypeList<u8, u16, u32, u64>>);

STATIC_ASSERT($Same<UniqueTypes<>, TypeList<>>);
STATIC_ASSERT($Same<UniqueTypes<i8>, TypeList<i8>>);
STATIC_ASSERT($Same<UniqueTypes<i8, i8>, TypeList<i8>>);
STATIC_ASSERT($Same<UniqueTypes<i8, const i8>, TypeList<i8, const i8>>);
STATIC_ASSERT($Same<UniqueTypes<i8, i8, i16>, TypeList<i8, i16>>);
STATIC_ASSERT($Same<UniqueTypes<i8, i16, i8>, TypeList<i8, i16>>);
STATIC_ASSERT($Same<UniqueTypes<i16, i8, i8>, TypeList<i16, i8>>);

#include "Concepts/Integral.h"
STATIC_ASSERT((TypeList<u8, u16, f32, i64, u8, u64>{} & []<$UInt>()static{}) == TypeList<u8, u16, u8, u64>{});
STATIC_ASSERT(([]<$UInt>()static{} & TypeList<u8, u16, f32, i64, u8, u64>{}) == TypeList<u8, u16, u8, u64>{});
STATIC_ASSERT((TypeList<>{} & []<$UInt>()static{}) == TypeList<>{});
STATIC_ASSERT(([]<$UInt>()static{} & TypeList<>{}) == TypeList<>{});
STATIC_ASSERT((TypeList<u8, u16, f32, i64, u8, u64>{} - []<$UInt>()static{}) == TypeList<f32, i64>{});
STATIC_ASSERT((TypeList<>{} - []<$UInt>()static{}) == TypeList<>{});
STATIC_ASSERT(TypeList<u8, u16, f32, i64, u8, u64>{} - TypeList<u8, u16, f32, i64, u8, u64>{} == TypeList<>{});
STATIC_ASSERT(TypeList<u8, u16, f32, i64, u8, u64>{} - TypeList<u8, u16, f32, i64, u8>{} == TypeList<u64>{});
STATIC_ASSERT(TypeList<u8, u16, f32, i64, u8, u64>{} - TypeList<u8, u16, f32, i64>{} == TypeList<u64>{});
STATIC_ASSERT(TypeList<u8, u16, f32, i64, u8, u64>{} - TypeList<u8, u16, f32>{} == TypeList<i64, u64>{});
STATIC_ASSERT(TypeList<u8, u16, f32, i64, u8, u64>{} - TypeList<u8, u16>{} == TypeList<f32, i64, u64>{});
STATIC_ASSERT(TypeList<u8, u16, f32, i64, u8, u64>{} - TypeList<u8>{} == TypeList<u16, f32, i64, u64>{});
STATIC_ASSERT(TypeList<u8, u16, f32, i64, u8, u64>{} - TypeList<>{} == TypeList<u8, u16, f32, i64, u8, u64>{});
STATIC_ASSERT(TypeList<>{} - TypeList<>{} == TypeList<>{});