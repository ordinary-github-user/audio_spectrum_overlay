#pragma once
#include <array>
#include <charconv>
#include "Algorithm/Find.h"
#include "Compiler.h"
#include "Concepts/String.h"
#include "DataStructures/ConstVector.h"
#include "DataStructures/TStringImpl.h"
#include "EnumDefinition.h"
#include "EnumName.h"
#include "Fold.h"
#include "Hash/MinHash.h"
#include "Inline.h"
#include "Iota.h"
#include "IsDigit.h"
#include "Macro/Underlying.h"
#include "VariadicSwitch.h"
template<auto value>constexpr bool is_named_enum [[maybe_unused]] { !isDigit(enum_name_impl<value>()[0]) };

template<$Enum T>struct Enum;

#if 0
	template<$Enum T>
	requires(!requires{enum_entries(T{}); })
	struct Enum<T>
	{
	private:
		static constexpr u64 maxEntryCount{ 0xff };
		static constexpr u64 maxSize{ Algorithm::min(u64{UnsignedBits<sizeof(T) * 8>(-1)}, maxEntryCount) };
		static constexpr std::array<bool, maxSize> validities{ Fold::construct<std::array<bool, maxSize>, maxSize>([](auto I)static{return is_named_enum<T(I())>; }) };
	public:
		static constexpr auto count{ Algorithm::sum<usize>(validities) };
		static constexpr std::array<T, count> entries{ [] static
		{
			usize resultIndex{};
			std::array<T, count> result{};
			for (u64 i{}; i != maxSize; ++i)
				if (validities[static_cast<usize>(i)])
					result[resultIndex++] = T(i);
			return result;
		}() };
	};
#endif

template<$Enum T>
requires(requires{enum_entries(T{}); })
struct Enum<T>
{
	static constexpr auto count{ enum_entries(T{}).size() };
	static constexpr std::array<T, count> entries{ enum_entries(T{}) };
};

template<$Enum auto e>
constexpr Compact<Enum<decltype(e)>::count> enum_index{ []static
{
	for (const auto i : iota<Enum<decltype(e)>::count>)
		if (e == Enum<decltype(e)>::entries[i])
			return i;
	UNREACHABLE;
}() };

template<$Enum T>
requires(Enum<T>::count != 0)
constexpr bool advance_enum(T& e)noexcept
{
	return Switch<Enum<T>::entries>
	(
		e,
		[&](auto E)LAMBDA_INLINE{ e = constant<Enum<T>::entries[(enum_index<E()> + 1) % Enum<T>::count]>; return true; },
		[]STATIC_LAMBDA_INLINE{ return false; }
	);
}

template<$Enum T>
requires(Enum<T>::count != 0)
constexpr void advance_enum_unsafe(T& e)noexcept
{
	Switch<Enum<T>::entries>
	(
		e,
		[&](auto E)LAMBDA_INLINE{ e = constant<Enum<T>::entries[(enum_index<E()> + 1) % Enum<T>::count]>; }
	);
}

template<bool AssumeMinLen, $Enum T, usize...I>
constexpr bool parse_enum_impl($String auto&& input, T& result, std::index_sequence<I...>)noexcept
{
#if !MSVC
	if constexpr (Enum<T>::count >= 3)
	{
		using Char = ElementOf<decltype(input)>;
		if constexpr (!AssumeMinLen)
			if (input.size() < compact<Algorithm::min(enum_name_fixed_string<Enum<T>::entries[I], Char>...)>)
				return false;
		using Hasher = MinHash<true, enum_name_fixed_string<Enum<T>::entries[I], Char>...>;
		using Hash = Hasher::Result;
		static constexpr std::array<Hash, Enum<T>::count> hashes{ Hasher::operator()(enum_name_fixed_string<Enum<T>::entries[I], Char>)... };
		return Switch<hashes>(
			Hasher::operator()(input),
			[&]<Hash H>(ImmediateValue<H>)LAMBDA_INLINE
			{
				static constexpr T e{ Enum<T>::entries[Algorithm::find_index_unsafe(hashes, H)] };
				if (enum_name<e> == input)
				{
					result = e;
					return true;
				}
				return false;
			},
			[]STATIC_LAMBDA_INLINE{ return false; }
		);
	}
	else//todo: AssumeMinLen
#endif
	return ((enum_name<Enum<T>::entries[I]> == input ? ((result = constant<Enum<T>::entries[I]>), true) : false) || ...);
}

template<bool AssumeMinLen, $Enum T>
INLINE constexpr bool parse_enum($String auto&& input, T& result)noexcept
{
	return parse_enum_impl<AssumeMinLen>(FWD(input), result, std::make_index_sequence<Enum<T>::count>{});
}

template<$Flag T>
struct Flag
{
private:
	static constexpr usize maxBits{ sizeof(T) * 8 };
public:
	//does not include T{}
	static constexpr std::array<T, maxBits> entries{ [] static
	{
		usize resultIndex{};
		std::array<T, maxBits> result{};
		for (usize i{}; i != maxBits; ++i)
			result[resultIndex++] = static_cast<T>(std::underlying_type_t<T>{1} << i);
		return result;
	}() };
	//include T{} if T{} is named
	static constexpr std::array named_entries{ [] static
	{
		static constexpr ConstVector<T, maxBits> arr{ [] <usize...I>(std::index_sequence<I...>)static
		{
			ConstVector<T, maxBits> result;
			if constexpr (is_named_enum<T{}>)
				result.push_back(T{});
			((is_named_enum<entries[I]> ? ((void)result.push_back(entries[I]), 0) : 0), ...);
			return result;
		}(std::make_index_sequence<maxBits>{})};
		std::array<T, arr.size()> result;
		for (const auto i : iota<result.size()>)
			result[i] = arr[i];
		return result;
	}() };
	static constexpr T mask_named{ [] static
	{
		std::underlying_type_t<T> result{};
		for (const T x : named_entries)
			result |= UNDERLYING(x);
		return static_cast<T>(result);
	}()};
	static constexpr T mask_unnamed{ static_cast<T>(~UNDERLYING(mask_named)) };
};

constexpr bool has_named_fields($Flag auto flag)noexcept
{
	using T = std::remove_cvref_t<decltype(flag)>;
	if (flag == T{})
		return is_named_enum<T{}> ;
	return (Flag<T>::mask_named & flag) != T{};
}

constexpr bool has_unnamed_fields($Flag auto flag)noexcept
{
	using T = std::remove_cvref_t<decltype(flag)>;
	if (flag == T{})
		return !is_named_enum<T{}> ;
	return (Flag<T>::mask_unnamed & flag) != T{};
}

template<$Flag T, usize...I>
constexpr bool parse_flag_step_impl($String auto&& input, T& result, std::index_sequence<I...>)noexcept
{
#if MSVC//msvc bug: failure was caused by control reaching the end of a constexpr function
	return (... || [&] {
			if (enum_name<Flag<T>::named_entries[I]> == input)
			{
				result = static_cast<T>(UNDERLYING(result) | constant<UNDERLYING(Flag<T>::named_entries[I])>);
				return true;
			}
			return false;
		}());
#else
	return (... || (
		(enum_name<Flag<T>::named_entries[I]> == input) ?
		((result = static_cast<T>(UNDERLYING(result) | constant<UNDERLYING(Flag<T>::named_entries[I])>)), true) :
		false
	));//todo: perfect hash
#endif
}

template<$Flag T>
INLINE constexpr bool parse_flag_step($String auto&& input, T& result)noexcept
{
	return parse_flag_step_impl(FWD(input), result, std::make_index_sequence<Flag<T>::named_entries.size()>{});
}

template<$Flag T>
struct FlagAndResult
{
private:
	T value;
public:
	INLINE constexpr FlagAndResult(T x)noexcept :value{ x } {}
	INLINE constexpr operator bool()const noexcept { return value != T{}; }
	INLINE constexpr operator T()const noexcept { return value; }
	friend INLINE constexpr bool operator==(FlagAndResult<T> l, FlagAndResult<T> r)noexcept { return T{ l } == T{ r }; }
	friend INLINE constexpr bool operator==(T/*            */l, FlagAndResult<T> r)noexcept { return l == T{ r }; }
	friend INLINE constexpr bool operator==(FlagAndResult<T> l, T/*            */r)noexcept { return T{ l } == r; }
	friend INLINE constexpr bool operator!=(FlagAndResult<T> l, FlagAndResult<T> r)noexcept { return T{ l } != T{ r }; }
	friend INLINE constexpr bool operator!=(T/*            */l, FlagAndResult<T> r)noexcept { return l != T{ r }; }
	friend INLINE constexpr bool operator!=(FlagAndResult<T> l, T/*            */r)noexcept { return T{ l } != r; }
};

template<$Flag E>INLINE constexpr FlagAndResult<E> operator&(E l, E r)noexcept { return static_cast<E>(UNDERLYING(l) & UNDERLYING(r)); }
template<$Flag E>INLINE constexpr E                operator|(E l, E r)noexcept { return static_cast<E>(UNDERLYING(l) | UNDERLYING(r)); }

template<$Flag E>INLINE constexpr E& operator&=(E& l, E r)noexcept { l = static_cast<E>(UNDERLYING(l) & UNDERLYING(r)); return l; }
template<$Flag E>INLINE constexpr E& operator|=(E& l, E r)noexcept { l = l | r; return l; }

template<$Flag E>INLINE constexpr E operator~(E e)noexcept { return static_cast<E>(~UNDERLYING(e)); }