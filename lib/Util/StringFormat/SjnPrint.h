#pragma once
#include "../Algorithm/Find.h"
#include "../Concepts/Floating.h"
#include "../Concepts/ForEach.h"
#include "../Concepts/Integral.h"
#include "../Concepts/LinearContainer.h"
#include "../Concepts/Optional.h"
#include "../Concepts/TupleLike.h"
#include "../Concepts/WrapperOf.h"
#include "../DataStructures/TString.h"
#include "../ElementOf.h"
#include "../Enum.h"
#include "../Macro/Concat.h"
#include "../Macro/UnsignedCast.h"
#include "../NameOf.h"
#include "../Reflection.h"
#include "../SizedInt.h"
#include "../TextEncoding/Wtf.h"
#include "../ValueList.h"
#include "BinToStream.h"
#include "DecToStream.h"
#include "FloatToStream.h"
//do not replace std::conditional_t with tstring_if, gcc will 4x compile time
namespace Sjn//(S)uperior (J)son (N)otation
{
	struct PrintingOptions
	{
		bool Pack;
		bool EnumFlagName;
		bool EnumFlagFields;
		c8 Indent{ '\t' };
		u8 ErrorCodeUnit8{};
		u16 ErrorCodeUnit16{};
	};
	DEFINE_ENUM(PrintMode, u8, start, array_element, object_value);

	template<class>
	constexpr bool must_pack{ false };

	template<class T>//use sjn_print_pack to declare custom struct should be packed
	requires requires{sjn_print_pack(std::declval<T>()); }
	constexpr bool must_pack<T> { true };

	template<$Reflection T>//empty struct should be packed
	requires(!$ForEach<T> && !$String<T>&& Reflection::size<T> == 0)
	constexpr bool must_pack<T> { true };

	template<class T>
	requires($Floating<T> || $Int<T> || $Bool<T> || __is_enum(std::remove_cvref_t<T>) || $String<T> || $WrapperAnyOf<T, bool, u8, u16, u32, u64, i8, i16, i32, i64, f32, f64>)
	constexpr bool must_pack<T> { true };

	template<$Optional T>
	constexpr bool must_pack<T> {must_pack<std::remove_cvref_t<decltype(*std::declval<T>())>> };

	template<$ForEach T>//array should be packed if their elements should be packed
	requires(!$String<T>)
	constexpr bool must_pack<T> {must_pack<ElementOf<T>> };

	template<$TupleLike T>//tuple should be packed if their elements should be packed
	requires(!$LinearContainer<T> && !$Reflection<T>)
	constexpr bool must_pack<T> 
	{
		[] <usize...I>(std::index_sequence<I...>)static
		{
			return (...&&must_pack<std::tuple_element_t<I, std::remove_cvref_t<T>>>); 
		}(std::make_index_sequence<std::tuple_size<std::remove_cvref_t<T>>::value>{})
	};

	template<$Reflection T>//struct with only 1 member should be packed if that member should be packed
	requires(!$ForEach<T> && !$String<T>&& Reflection::size<T> == 1)
	constexpr bool must_pack<T> { must_pack<Reflection::type_at<T, 0>> };

	template<class>
	constexpr bool need_colon{ true };

	template<class T>//use sjn_print_colon to declare custom struct requires colon before it
	requires requires{sjn_print_colon(std::declval<T>()); }
	constexpr bool need_colon<T> { true };

	template<$Reflection T>//struct need no colon before it
	requires(!$ForEach<T> && !$String<T> && !requires{sjn_print_colon(std::declval<T>()); })
	constexpr bool need_colon<T> { false };

	template<$String T>//string need no colon before it
	constexpr bool need_colon<T> { false };

	template<$Char T>
	constexpr bool need_colon<T> { false };

	template<class T>
	requires($Floating<T> || ($Int<T> && !$Char<T>) || __is_enum(std::remove_cvref_t<T>) || $WrapperAnyOf<T, bool, u8, u16, u32, u64, i8, i16, i32, i64, f32, f64>)
	constexpr bool need_colon<T> { true };

	template<$ForEach T>//array need no colon before it
	requires(!$String<T>)
	constexpr bool need_colon<T> { false };

	template<$TupleLike T>//tuple need no colon before it
	requires(!$LinearContainer<T> && !$Reflection<T>)
	constexpr bool need_colon<T> { false };

	template<PrintingOptions, class>
	constexpr auto comment_of_impl{ TString<c8>{} };

	template<PrintingOptions opts, $Enum T>
	requires((opts.EnumFlagName || opts.EnumFlagFields) && Enum<T>::count != 0)
	constexpr auto comment_of_impl<opts, T>
	{
		[] <usize...I>(std::index_sequence<I...>)static CONSTEVAL
		{
			return std::conditional_t<opts.EnumFlagName, decltype(tstring<nameof<T>>), TString<c8>>{} 
				 + std::conditional_t<opts.EnumFlagFields && opts.EnumFlagName, TString<c8, ':'>, TString<c8>>{}
				 + std::conditional_t<opts.EnumFlagFields, decltype((... + (std::conditional_t<I == 0, TString<c8>, TString<c8, ','>>{} + enum_name<Enum<T>::entries[I]>))), TString<c8>>{};
		}(std::make_index_sequence<Enum<T>::count>{})
	};

	template<PrintingOptions opts, $Flag T>
	requires((opts.EnumFlagName || opts.EnumFlagFields) && Flag<T>::named_entries.size() != 0)
	constexpr auto comment_of_impl<opts, T>
	{
		[] <usize...I, T...flag>(std::index_sequence<I...>, ValueList<flag...>)static CONSTEVAL
		{
			return std::conditional_t<opts.EnumFlagName, decltype(tstring<nameof<T>>), TString<c8>>{}
				 + std::conditional_t<opts.EnumFlagFields && opts.EnumFlagName, TString<c8, ':'>, TString<c8>>{}
				 + std::conditional_t<opts.EnumFlagFields, decltype((... + (std::conditional_t<I == 0, TString<c8>, TString<c8, '|'>>{} + enum_name<flag>))), TString<c8>>{};
		}(std::make_index_sequence<Flag<T>::named_entries.size()>{}, MakeValueList<Flag<T>::named_entries>{})
	};

	template<PrintingOptions opts, $Optional T>
	constexpr auto comment_of_impl<opts, T>{ comment_of_impl<opts, std::remove_cvref_t<decltype(*std::declval<T>())>> };

	template<PrintingOptions opts, $ForEach T>
	requires(!$String<T>)
	constexpr auto comment_of_impl<opts, T>{ comment_of_impl<opts, ElementOf<T>> };

	template<PrintingOptions opts, class T>
	constexpr bool has_trailing_comment{ !$Same<std::remove_cvref_t<decltype(comment_of_impl<opts, T>)>, TString<c8>> };

	template<PrintingOptions opts, bool pack, class T>
	constexpr auto comment_of
	{
		std::conditional_t
		<
			!has_trailing_comment<opts, std::remove_cvref_t<T>>, 
			TString<c8>,
			std::conditional_t
			<
				pack,
				decltype(TString<c8, '/', '*'>{} + comment_of_impl<opts, std::remove_cvref_t<T>> + TString<c8, '*', '/'>{}),
				decltype(TString<c8, '/', '/'>{} + comment_of_impl<opts, std::remove_cvref_t<T>>)
			>
		>{}
	};

	template<PrintingOptions opts, bool pack, FixedString totalIndent, PrintMode mode>
	constexpr void print_impl(auto& os, $Same<bool> auto x)noexcept
	{
		using Prefix = std::conditional_t<!pack && mode == PrintMode::array_element, decltype(tstring<'\n' + totalIndent>), TString<c8>>;
		if (x)os << (Prefix{} + TString<c8, 't', 'r', 'u', 'e'>{});
		else os << (Prefix{} + TString<c8, 'f', 'a', 'l', 's', 'e'>{});
	}

	template<PrintingOptions opts, bool pack, FixedString totalIndent, PrintMode mode>
	INLINE constexpr void print_impl(auto& os, $Floating auto x)noexcept
	{
		if constexpr (!pack && mode == PrintMode::array_element)
			os << tstring<'\n' + totalIndent>;
		os << Float{ x };
	}

	template<PrintingOptions opts, bool pack, FixedString totalIndent, PrintMode mode>
	INLINE constexpr void print_impl(auto& os, $Int auto x)noexcept
	requires(!$Same<decltype(x), bool> && !$Char<decltype(x)>)
	{
		if constexpr (!pack && mode == PrintMode::array_element)
			os << tstring<'\n' + totalIndent>;
		os << Dec{ x };
	}

	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<bool> auto&& x)noexcept { print_impl<Args...>(os, static_cast<bool>(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<i8  > auto&& x)noexcept { print_impl<Args...>(os, static_cast<i8  >(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<i16 > auto&& x)noexcept { print_impl<Args...>(os, static_cast<i16 >(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<i32 > auto&& x)noexcept { print_impl<Args...>(os, static_cast<i32 >(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<i64 > auto&& x)noexcept { print_impl<Args...>(os, static_cast<i64 >(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<u8  > auto&& x)noexcept { print_impl<Args...>(os, static_cast<u8  >(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<u16 > auto&& x)noexcept { print_impl<Args...>(os, static_cast<u16 >(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<u32 > auto&& x)noexcept { print_impl<Args...>(os, static_cast<u32 >(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<u64 > auto&& x)noexcept { print_impl<Args...>(os, static_cast<u64 >(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<f32 > auto&& x)noexcept { print_impl<Args...>(os, static_cast<f32 >(x)); }
	template<auto...Args>INLINE constexpr void print_impl(auto& os, $WrapperOf<f64 > auto&& x)noexcept { print_impl<Args...>(os, static_cast<f64 >(x)); }

	template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode>
	constexpr void print_impl(auto& os, $Char auto x)noexcept
	{
		using T = std::remove_cvref_t<decltype(x)>;
		using StreamChar = typename std::remove_cvref_t<decltype(os)>::char_type;
		using Prefix = std::conditional_t<!packParent && mode == PrintMode::array_element, decltype(tstring<'\n' + totalIndent>), TString<c8>>;
		c8 quote;
		if (x == '"') [[unlikely]] { quote = '\''; os << (Prefix{} + TString<c8, '\''>{}); }
		else/*       */ [[likely]] { quote = '"';  os << (Prefix{} + TString<c8, '"' >{}); }
		switch (x)
		{
			case '\\': os << TString<c8, '\\', '\\'>{}; break;
			case '\b': os << TString<c8, '\\', 'b' >{}; break;
			case '\f': os << TString<c8, '\\', 'f' >{}; break;
			case '\n': os << TString<c8, '\\', 'n' >{}; break;
			case '\r': os << TString<c8, '\\', 'r' >{}; break;
			case '\t': os << TString<c8, '\\', 't' >{}; break;
			default:
				if (UNSIGNED(x) < 0x20) [[unlikely]]
					os << TString<c8, '\\', 'x'>{} << hex<1, '0'>(static_cast<u8>(UNSIGNED(x)));
				else [[likely]]
				{
					/* */if constexpr (sizeof(StreamChar) >= sizeof(T))os.put(static_cast<StreamChar>(UNSIGNED(x)));
					else if constexpr (sizeof(StreamChar) == 1){if (!Wtf8 ::encode(x, [&](c8  unit) { os.put(std::bit_cast<StreamChar>(unit)); }))os.put(std::bit_cast<StreamChar>(opts.ErrorCodeUnit8 ));}
					else if constexpr (sizeof(StreamChar) == 2){if (!Wtf16::encode(x, [&](c16 unit) { os.put(std::bit_cast<StreamChar>(unit)); }))os.put(std::bit_cast<StreamChar>(opts.ErrorCodeUnit16));}
					else STATIC_ERROR(unsupported stream type);
				}
		}
		os.put(quote);
	}
	template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode>
	constexpr void print_impl(auto& os, $StringOfCharSizeAny<1, 2, 4> auto&& x)noexcept
	{
		using StreamChar = typename std::remove_cvref_t<decltype(os)>::char_type;
		using Prefix = std::conditional_t<!packParent && mode == PrintMode::array_element, decltype(tstring<'\n' + totalIndent>), TString<c8>>;
		c8 quote;
		/* */if (!Algorithm::find(x, '"'))  [[likely]] { quote = '"';  os << (Prefix{} + TString<c8, '"'>{}); }
		else if (!Algorithm::find(x, '\'')) [[likely]] { quote = '\''; os << (Prefix{} + TString<c8, '\''>{}); }
		else if (!Algorithm::find(x, '`'))  [[likely]] { quote = '`';  os << (Prefix{} + TString<c8, '`'>{}); }
		else/*                         */ [[unlikely]] { quote = '"';  os << (Prefix{} + TString<c8, '"'>{}); }
#define SJN_PRINT_ENCODE(bits, c) CAT(SJN_PRINT_ENCODE,bits)(c)
#define SJN_PRINT_ENCODE8(c) if (!Wtf8::encode(c, [&](auto unit) { os << unit; })) [[unlikely]] os.put(std::bit_cast<StreamChar>(opts.ErrorCodeUnit8))
#define SJN_PRINT_ENCODE16(c) if (!Wtf16::encode(c, [&](auto unit) { os << unit; })) [[unlikely]] os.put(std::bit_cast<StreamChar>(opts.ErrorCodeUnit16))
#define SJN_PRINT_ENCODE32(c) os.put(static_cast<StreamChar>(c))
#define SJN_PRINT_ENCODESAME(c) os.put(static_cast<StreamChar>(c))
#define SJN_PRINT_PUSH_CODEPOINT(bits, c)																													\
		switch (c)																																			\
		{																																					\
			case '"': { if (quote == '"') [[unlikely]] os << TString<c8, '\\', '"'>{}; else [[likely]] os << '"'; } break;									\
			case '\\': os << TString<c8, '\\', '\\'>{}; break;																								\
			case '\b': os << TString<c8, '\\', 'b' >{}; break;																								\
			case '\f': os << TString<c8, '\\', 'f' >{}; break;																								\
			case '\n': os << TString<c8, '\\', 'n' >{}; break;																								\
			case '\r': os << TString<c8, '\\', 'r' >{}; break;																								\
			case '\t': os << TString<c8, '\\', 't' >{}; break;																								\
			default:																																		\
				if (UNSIGNED(c) < 0x20) [[unlikely]] os << TString<c8, '\\', 'x'>{} << hex<1, '0'>(static_cast<u8>(c));										\
				else [[likely]] SJN_PRINT_ENCODE(bits, c);																									\
		}
		if constexpr (sizeof(StreamChar) == sizeof(ElementOf<decltype(x)>))
			for (const auto c : x)
				SJN_PRINT_PUSH_CODEPOINT(SAME, c)
		else if constexpr (sizeof(StreamChar) == 1)
		{
			if constexpr ($StringOfCharSize<decltype(x), 2>)
				Wtf16::decode(x, [&](const u32 codePoint)LAMBDA_INLINE{SJN_PRINT_PUSH_CODEPOINT(8, codePoint)});
			else if constexpr ($StringOfCharSize<decltype(x), 4>)
				for (const c32 codePoint : x)
					SJN_PRINT_PUSH_CODEPOINT(8, codePoint)
		}
		else if constexpr (sizeof(StreamChar) == 2)
		{
			if constexpr ($StringOfCharSize<decltype(x), 1>)
				Wtf8::decode(x, [&](const u32 codePoint)LAMBDA_INLINE{ SJN_PRINT_PUSH_CODEPOINT(16, codePoint) });
			else if constexpr ($StringOfCharSize<decltype(x), 4>)
				for (const c32 codePoint : x)
					SJN_PRINT_PUSH_CODEPOINT(16, codePoint)
		}
		else if constexpr (sizeof(StreamChar) == 4)
		{
			if constexpr ($StringOfCharSize<decltype(x), 1>)
				Wtf8::decode(x, [&](const u32 codePoint)LAMBDA_INLINE{ SJN_PRINT_PUSH_CODEPOINT(32, codePoint) });
			else if constexpr ($StringOfCharSize<decltype(x), 2>)
				Wtf16::decode(x, [&](const u32 codePoint)LAMBDA_INLINE{ SJN_PRINT_PUSH_CODEPOINT(32, codePoint) });
		}
		else STATIC_ERROR(unsupported stream type);
		os << quote;
#undef SJN_PRINT_ENCODE
#undef SJN_PRINT_ENCODE8
#undef SJN_PRINT_ENCODE16
#undef SJN_PRINT_ENCODE32
#undef SJN_PRINT_ENCODESAME
#undef SJN_PRINT_PUSH_CODEPOINT
	}

	template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode>
	constexpr void print_impl(auto& os, $Enum auto x)noexcept
	{
		using T = std::remove_cvref_t<decltype(x)>;
		using Prefix = std::conditional_t<!packParent && mode == PrintMode::array_element, decltype(tstring<'\n' + totalIndent>), TString<c8>>;
		Switch<Enum<T>::entries>
		(
			x,
			[&](auto e)LAMBDA_INLINE{ os << (Prefix{} + TString<c8, '"'>{} + enum_name<e()> + TString<c8, '"'>{}); },
			[&]LAMBDA_INLINE{ os << Prefix{} << Dec{UNDERLYING(x)}; }
		);
	}

	template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode, $Flag T, usize...I, T...flag>
	INLINE constexpr void print_flag(auto& os, T x, std::index_sequence<I...>, ValueList<flag...>)noexcept
	{
		using Prefix = std::conditional_t<!packParent && mode == PrintMode::array_element, decltype(tstring<'\n' + totalIndent>), TString<c8>>;
		if (has_unnamed_fields(x))[[unlikely]]
			os << (Prefix{} + TString<c8, '0', 'b'>{}) << bin<0, '0'>(UNDERLYING(x));
		else
		{
			os << (Prefix{} + TString<c8, '"'>{});
			bool isFirst{ true };
			(..., [&]LAMBDA_INLINE
			{
				if (x & flag)
				{
					if (isFirst)
					{
						isFirst = false;
						os << enum_name<flag>;
					}
					else
						os << (TString<c8, '|'>{} + enum_name<flag>);
				}
			}());
			if (isFirst)
				os << enum_name<T{}>;
			os << '"';
		}
	}

	template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode>
	constexpr void print_impl(auto& os, $Flag auto x)noexcept
	{
		print_flag<opts, packParent, totalIndent, mode>(os, x, std::make_index_sequence<Flag<decltype(x)>::named_entries.size()>{}, MakeValueList<Flag<decltype(x)>::named_entries>{});
	}

#define SJN_PRINT_OPTIONAL template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode>constexpr void print_impl(auto& os, $Optional auto&& x)noexcept
#define SJN_PRINT_TUPLE template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode>constexpr void print_impl(auto& os, $TupleLike auto&& tuple)noexcept requires(!$LinearContainer<decltype(tuple)> && !$Reflection<decltype(tuple)>)
#define SJN_PRINT_ARRAY template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode>constexpr void print_impl(auto& os, $ForEach auto&& x)noexcept requires(!$String<decltype(x)>)
#define SJN_PRINT_REFLECTION template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode>INLINE constexpr void print_impl(auto& os, $Reflection auto&& reflect)noexcept requires(!$ForEach<decltype(reflect)> && !$String<decltype(reflect)>)

	SJN_PRINT_OPTIONAL;		//forward decl
	SJN_PRINT_TUPLE;		//forward decl
	SJN_PRINT_ARRAY;		//forward decl
	SJN_PRINT_REFLECTION;	//forward decl
	
	template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode, usize...I>
	constexpr void print_reflection(auto& os, $Reflection auto&& reflect, std::index_sequence<I...>)noexcept
	{
		static constexpr bool pack{ packParent || must_pack<decltype(reflect)> };
		using T = std::remove_cvref_t<decltype(reflect)>;
		using Prefix = std::conditional_t<mode != PrintMode::start && !packParent && (!pack || mode == PrintMode::array_element), decltype(tstring<'\n' + totalIndent + '{'>), TString<c8, '{'>>;
		if constexpr (!sizeof...(I))
			os << Prefix{};
		(...,(
			(
				os <<
				(
					std::conditional_t<I == 0, Prefix, decltype(comment_of<opts, pack, typename Reflection::type_at<T, I ? I - 1 : 0>>)>{} +
					std::conditional_t<pack,std::conditional_t<I != 0, TString<c8, ','>, TString<c8>>,decltype(tstring<'\n' + totalIndent + opts.Indent>)>{} +
					Reflection::name<T, I> +
					std::conditional_t<need_colon<typename Reflection::type_at<T, I>>, TString<c8, ':'>, TString<c8>>{}
				)
			),
			print_impl<opts, pack, totalIndent + opts.Indent, PrintMode::object_value>(os, Reflection::at<I>(reflect))
		));
		if constexpr (sizeof...(I))
			os << (
				comment_of<opts, pack, typename Reflection::type_at<T, sizeof...(I) - 1>> + 
				std::conditional_t<pack, TString<c8, '}'>, decltype(tstring<'\n' + totalIndent + '}'>)>{}
			);
		else
			os << std::conditional_t<pack, TString<c8, '}'>, decltype(tstring<'\n' + totalIndent + '}'>)>{};
	}

	template<PrintingOptions opts, bool packParent, FixedString totalIndent, PrintMode mode, usize...I>
	constexpr void print_tuple(auto& os, $TupleLike auto&& tuple, std::index_sequence<I...>)noexcept
	{
		static constexpr bool pack{ packParent || must_pack<decltype(tuple)> };
		using Prefix = std::conditional_t<mode != PrintMode::start && !packParent && (!pack || mode == PrintMode::array_element), decltype(tstring<'\n' + totalIndent + '['>), TString<c8, '['>>;
		if constexpr (!sizeof...(I))
			os << Prefix{};
		(...,(
			(
				os << 
				(
					std::conditional_t<I == 0, Prefix, decltype(comment_of<opts, pack, std::tuple_element_t<I ? I - 1 : 0, std::remove_cvref_t<decltype(tuple)>>>)>{} +
					std::conditional_t<pack && I, TString<c8, ','>, TString<c8>>{}
				)
			),
			print_impl<opts, pack, totalIndent + opts.Indent, PrintMode::array_element>(os, get<I>(tuple))
		));
		if constexpr (sizeof...(I))
			os << (
				comment_of<opts, pack, std::tuple_element_t<sizeof...(I) - 1, std::remove_cvref_t<decltype(tuple)>>> +
				std::conditional_t<pack, TString<c8, ']'>, decltype(tstring<'\n' + totalIndent + ']'>)>{}
			);
		else
			os << std::conditional_t<pack, TString<c8, ']'>, decltype(tstring<'\n' + totalIndent + ']'>)>{};
	}

	SJN_PRINT_ARRAY
	{
		static constexpr bool pack{ packParent || must_pack<decltype(x)> };
		using Prefix = std::conditional_t<mode != PrintMode::start && !packParent && (!pack || mode == PrintMode::array_element), decltype(tstring<'\n' + totalIndent + '['>), TString<c8, '['>>;
		os << Prefix{};
		if constexpr (pack)
		{
			bool first{ true };
			for (auto&& e : FWD(x))
			{
				if (first)
					first = false;
				else
					os << ',';
				print_impl<opts, 1, totalIndent, PrintMode::array_element>(os, FWD(e));
			}
			os << ']';
		}
		else
		{
			for (auto&& e : FWD(x))
				print_impl<opts, 0, totalIndent + opts.Indent, PrintMode::array_element>(os, FWD(e));
			os << tstring<'\n' + totalIndent + ']'>;
		}
	}

	SJN_PRINT_REFLECTION
	{
		print_reflection<opts, packParent, totalIndent, mode>(os, reflect, std::make_index_sequence<Reflection::size<std::remove_cvref_t<decltype(reflect)>>>{});
	}

	SJN_PRINT_TUPLE
	{
		print_tuple<opts, packParent, totalIndent, mode>(os, tuple, std::make_index_sequence<std::tuple_size<std::remove_cvref_t<decltype(tuple)>>::value>{});
	}

	SJN_PRINT_OPTIONAL
	{
		if (x)
			print_impl<opts,packParent,totalIndent,mode>(os, *x);
		else
			os << std::conditional_t<!packParent && mode == PrintMode::array_element, decltype(tstring<'\n' + totalIndent + "null">), TString<c8, 'n', 'u', 'l', 'l'>>{};
	}
#undef SJN_PRINT_OPTIONAL
#undef SJN_PRINT_TUPLE
#undef SJN_PRINT_ARRAY
#undef SJN_PRINT_REFLECTION
	template<PrintingOptions opts>
	INLINE constexpr void print(auto& os, $Reflection auto&& reflect)noexcept
	requires(!$ForEach<decltype(reflect)> && !$String<decltype(reflect)>)
	{
		print_reflection<opts, opts.Pack, "", PrintMode::start>(os, reflect, std::make_index_sequence<Reflection::size<std::remove_cvref_t<decltype(reflect)>>>{});
	}
}