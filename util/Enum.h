#pragma once
#include "EnumImpl.h"
#include "GcdLcm.h"
#include "HexToStream.h"
#include "ObfuString.h"
#include "ToTString.h"
#if STRING_OBFUSCATION
	template<class Stream, $Enum T, usize...I>
	INLINE constexpr void print_enum_impl(Stream& os, T e, std::index_sequence<I...>)noexcept
	{
		using Char = typename Stream::char_type;
		if consteval
		{
			Switch<Enum<T>::entries>
			(
				e,
				[&](auto E)LAMBDA_INLINE{ os << enum_name<E()>; },
				[&]LAMBDA_INLINE{ os << UNDERLYING(e); }
			);
		}
		else
		{
			static constexpr usize max_length{ Algorithm::max(decltype(ObfuString{ enum_name<Enum<T>::entries[I],Char> })::size()...) };
			static constexpr usize buffer_size{ Algorithm::max(sizeof(decltype(ObfuString{ enum_name<Enum<T>::entries[I],Char> }))...) };
			static constexpr usize buffer_align{ Math::lcm(alignof(decltype(ObfuString{ enum_name<Enum<T>::entries[I],Char> }))...) };
			alignas(buffer_align) u8 buffer[buffer_size];
			Compact<max_length> size;
			if (Switch<Enum<T>::entries>(
					e,
					[&](auto E)LAMBDA_INLINE
					{
						size = decltype(ObfuString{ enum_name<E(),Char>})::size();
						new(&buffer[0])ObfuString{ enum_name<E(),Char> };
						return true;
					},
					[&]LAMBDA_INLINE{ os << UNDERLYING(e); return false; }
				))
				os.write(reinterpret_cast<Char const*>(&buffer[0]), size);
		}
	}

	template<class Stream, $Enum T>
	INLINE constexpr Stream& operator<<(Stream& os, T e)noexcept
	{
		print_enum_impl(os, e, std::make_index_sequence<Enum<T>::count>{});
		return os;
	}
#else
	template<class Stream, $Enum T>
	INLINE constexpr Stream& operator<<(Stream& os, T e)noexcept
	{
		Switch<Enum<T>::entries>
		(
			e,
			[&](auto E)LAMBDA_INLINE{ os << enum_name<E()>; },
			[&]LAMBDA_INLINE{ os << UNDERLYING(e); }
		);
		return os;
	}
#endif

template<class Stream, $Flag T>
INLINE constexpr Stream& operator<<(Stream& os, T e)noexcept
{
	bool isFirst{ true };
	Fold::for_each<Flag<T>::entries>([&]<T flag>(ImmediateValue<flag>)LAMBDA_INLINE
	{
		static constexpr std::underlying_type_t<T> flagValue{ UNDERLYING(flag) };
		if (UNDERLYING(e) & flagValue)
		{
			if constexpr (is_named_enum<flag>)
			{
				if (isFirst)
				{
					isFirst = false;
					os << enum_name<flag>;
				}
				else
					os << TS("|") + enum_name<flag>;
			}
			else
			{
				static constexpr auto str{ toTString8<TS("0x"),Hex{ flagValue }> };
				if (isFirst)
				{
					isFirst = false;
					os << str;
				}
				else
					os << TS("|") + str;
			}
		}
	});
	if (isFirst)
	{
		if constexpr(is_named_enum<T{}>)
			os << enum_name<T{}>;
		else
			os << TS("0");
	}
	return os;
}