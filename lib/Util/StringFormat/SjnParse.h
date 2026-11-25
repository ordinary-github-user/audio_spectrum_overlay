#pragma once
#include <charconv>
#include "../Algorithm/Iequal.h"
#include "../Concepts/Char.h"
#include "../Concepts/FixSized.h"
#include "../Concepts/ForEach.h"
#include "../Concepts/LinearContainer.h"
#include "../Concepts/Optional.h"
#include "../Concepts/Same.h"
#include "../Concepts/String.h"
#include "../Concepts/TupleLike.h"
#include "../Concepts/WrapperOf.h"
#include "../DataStructures/Bitset.h"
#include "../DataStructures/Span.h"
#include "../DataStructures/TStringImpl.h"
#include "../ElementOf.h"
#include "../EnumImpl.h"
#include "../fast_float.h"
#include "../Hash/MinHash.h"
#include "../Macro/ForEach.h"
#include "../Macro/If.h"
#include "../Macro/MovForward.h"
#include "../MsvcUtf8.h"
#include "../NameOf.h"
#include "../Reflection.h"
#include "../StringViewImpl.h"
#include "../TextEncoding/Wtf.h"
namespace Sjn//(S)uperior (J)son (N)otation
{
	struct ParsingOptions//default value = max speed
	{
		bool RollBackOnFail{};
		bool NumberEnums{};
		bool Comment{};
		bool KeyVerify{};
		bool SlashU{};
		bool AllowUnknowns{};
		bool AllowMissing{true};
		bool WtfVerify{};//might crash if (!WtfVerify && !Wtf8::valid(input))
		CONSTEVAL bool valid()const noexcept { return !AllowUnknowns || KeyVerify; }
	};
	template<FixedString string, bool advance = true>
	INLINE constexpr bool consume_iequal_unsafe(std::conditional_t<advance, c8 const*&, c8 const*> pBegin)noexcept
	{
		if (!iequal_unsafe<string>(pBegin))
			return false;
		if constexpr(advance)
			pBegin += string.size();
		return true;
	}
	template<FixedString string, bool advance = true>
	INLINE constexpr bool consume_equal_unsafe(std::conditional_t<advance, c8 const*&, c8 const*> pBegin)noexcept
	{
		if (!equal_unsafe<string>(pBegin))
			return false;
		if constexpr (advance)
			pBegin += string.size();
		return true;
	}
	constexpr bool parse_slash_x_digits_unsafe(const c8* pBegin, u8& result)noexcept
	{
		const c8 c0{pBegin[0]};
		const c8 c1{pBegin[1]};
		switch (c0)
		{
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':	result = static_cast<u8>(c0 - '0');			break;
			case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':										result = static_cast<u8>(c0 - 'A' + 10);	break;
			case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':										result = static_cast<u8>(c0 - 'a' + 10);	break;
			default: [[unlikely]] return false;
		}
		result *= 16;
		switch (c1)
		{
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':	result += static_cast<u8>(c1 - '0');		break;
			case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':										result += static_cast<u8>(c1 - 'A' + 10);	break;
			case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':										result += static_cast<u8>(c1 - 'a' + 10);	break;
			default: [[unlikely]] return false;
		}
		return true;
	}
	constexpr bool parse_slash_u_digits_unsafe(const c8* pBegin, u16& result)noexcept
	{
		const c8 c0{ pBegin[0] };
		const c8 c1{ pBegin[1] };
		const c8 c2{ pBegin[2] };
		const c8 c3{ pBegin[3] };
		switch (c0)
		{
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':	result = static_cast<u8>(c0 - '0');			break;
			case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':										result = static_cast<u8>(c0 - 'A' + 10);	break;
			case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':										result = static_cast<u8>(c0 - 'a' + 10);	break;
			default: [[unlikely]] return false;
		}
		result *= 16;
		switch (c1)
		{
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':	result += static_cast<u8>(c1 - '0');		break;
			case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':										result += static_cast<u8>(c1 - 'A' + 10);	break;
			case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':										result += static_cast<u8>(c1 - 'a' + 10);	break;
			default: [[unlikely]] return false;
		}
		result *= 16;
		switch (c2)
		{
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':	result += static_cast<u8>(c2 - '0');		break;
			case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':										result += static_cast<u8>(c2 - 'A' + 10);	break;
			case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':										result += static_cast<u8>(c2 - 'a' + 10);	break;
			default: [[unlikely]] return false;
		}
		result *= 16;
		switch (c3)
		{
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':	result += static_cast<u8>(c3 - '0');		break;
			case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':										result += static_cast<u8>(c3 - 'A' + 10);	break;
			case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':										result += static_cast<u8>(c3 - 'a' + 10);	break;
			default: [[unlikely]] return false;
		}
		return true;
	}
	template<ParsingOptions opts>
	INLINE constexpr bool is_end_of_key(const c8 c)noexcept
	{
#if MSVC
		static constexpr std::array<bool, 256> table{ [] static
		{
			std::array<bool, 256> ret{};
			ret['\r'] = true;
			ret['\n'] = true;
			ret['\t'] = true;
			ret[':']  = true;
			ret['{']  = true;
			ret['[']  = true;
			ret[' ']  = true;
			ret['"']  = true;
			ret['\''] = true;
			ret['`']  = true;
			ret['/']  = opts.Comment;
			return ret;
		}() };
		return table[static_cast<u8>(c)];
#elif CLANG
		//clang compile this into jump table, deleting chars would result in shitty branches, do not delete                             || c == '/'
		return c == '\r' || c == '\n' || c == ':' || c == '{' || c == '[' || c == ' ' || c == '\t' || c == '"' || c == '\'' || c == '`' || c == '/';
#else
		if constexpr(opts.Comment)return c == '\r' || c == '\n' || c == ':' || c == '{' || c == '[' || c == ' ' || c == '\t' || c == '"' || c == '\'' || c == '`' || c == '/';
		else/*                  */return c == '\r' || c == '\n' || c == ':' || c == '{' || c == '[' || c == ' ' || c == '\t' || c == '"' || c == '\'' || c == '`';
#endif
	}
	
	#define SJN_PARSE_GARBAGE_CASE(c) case c:
	#define SJN_PARSE_SKIP_GARBAGE_IMPL(Comment,...)							\
		do																		\
		{																		\
			if (input.empty()) [[unlikely]] return false;						\
			switch (input[0])													\
			{																	\
				FOR_EACH(SJN_PARSE_GARBAGE_CASE, __VA_ARGS__)					\
					input += 1;													\
					break;														\
				IF(Comment, case '/': [[unlikely]]								\
				{																\
					input += 1;													\
					if (input.empty()) [[unlikely]] return false;				\
					const c8 c{ input[0] };										\
					input += 1;													\
					if (c == '/')												\
					{															\
						for (;;)												\
						{														\
							if (input.empty()) [[unlikely]] return false;		\
							if (input[0] == '\n') [[unlikely]]					\
							{													\
								input += 1;										\
								break;											\
							}													\
							input += 1;											\
						}														\
						break;													\
					}															\
					else if (c == '*')											\
					{															\
						for (;;)												\
						{														\
							if (input.size() <= 2) [[unlikely]] return false;	\
							if (consume_equal_unsafe<"*/">(input.m_pBegin))		\
								break;											\
							input += 1;											\
						}														\
						break;													\
					}															\
					else [[unlikely]] return false;								\
				},)																\
				default:return true;											\
			}																	\
		}while(1)
	template<ParsingOptions opts, auto...LogError>
	INLINE constexpr bool skip_garbage(Span<const c8>& input)noexcept
	{
		if constexpr (opts.Comment)SJN_PARSE_SKIP_GARBAGE_IMPL(1, ' ', '\t', '\r', '\n');
		else                       SJN_PARSE_SKIP_GARBAGE_IMPL(0, ' ', '\t', '\r', '\n');
	}
	template<ParsingOptions opts, auto...LogError>
	INLINE constexpr bool skip_garbage_comma(Span<const c8>& input)noexcept
	{
		if constexpr (opts.Comment)SJN_PARSE_SKIP_GARBAGE_IMPL(1, ' ', '\t', '\r', '\n', ',');
		else                       SJN_PARSE_SKIP_GARBAGE_IMPL(0, ' ', '\t', '\r', '\n', ',');
	}
	#define SJN_PARSE_RETURN_FAIL_IMPL(...) (LogError(tstring<"Invalid " + nameof<std::remove_reference_t<decltype(result)>>> __VA_ARGS__), ...); return false;
	#define SJN_PARSE_RETURN_FAIL(...) do{ SJN_PARSE_RETURN_FAIL_IMPL(__VA_ARGS__); }while(0)
	#define SJN_PARSE_RETURN(...) do{ if(__VA_ARGS__)[[likely]]return true;else[[unlikely]]{SJN_PARSE_RETURN_FAIL_IMPL();} }while(0)
	template<ParsingOptions opts, auto...LogError>
	constexpr bool skip_unknowns(Span<const c8>& input)noexcept
	{
		const c8 c0{ input[0] };
		input += 1;
		switch (c0)
		{
			case '"':case '`':case '\'':
				for (;;)
				{
					if (input.empty()) [[unlikely]] return false;
					const c8 c{ input[0] };
					if (c == c0) { input += 1; return true; }
					else if (c == '\\') [[unlikely]]
					{
						input += 1;
						if (input.empty()) [[unlikely]] return false;
						const c8 c1{ input[0] };
						if (c1 == 'x')//\x##
						{
							if (input.size() <= 3) [[unlikely]] return false;
							input += 3;//dont care even if they aren't digits
						}
						else//\u#### or \t or \r ...
						{
							if constexpr (opts.SlashU)
							{
								if (c1 == 'u') [[unlikely]]
								{
									if (input.size() <= 5) [[unlikely]] return false;
									input += 5;//dont care even if they aren't digits
								}
								else [[likely]] input += 1;
							}
							else input += 1;
						}
					}
					else input += 1;
				}
			case '{':
				for (;;)
				{
					if (!skip_garbage_comma<opts, LogError...>(input)) [[unlikely]] return false;
					if (input[0] == '}') { input += 1; return true; }
					for (;;)//skip key
					{
						if (input.empty()) [[unlikely]] return false;
						if (is_end_of_key<opts>(input[0]))break;
						input += 1;
					}
					if (!skip_garbage<opts, LogError...>(input)) [[unlikely]] return false;
					if (input[0] == ':')//':' is sometimes optional
					{
						input += 1;
						if (!skip_garbage<opts, LogError...>(input)) [[unlikely]] return false;
					}
					if (!skip_unknowns<opts, LogError...>(input)) [[unlikely]] return false;
				}
			case '[':
				for (;;)
				{
					if (!skip_garbage_comma<opts, LogError...>(input)) [[unlikely]] return false;
					if(input[0] == ']') { input += 1; return true; }
					if (!skip_unknowns<opts, LogError...>(input)) [[unlikely]] return false;
				}
			default://skip until garbage
			{
				for (;;)
				{
					if (input.empty()) [[unlikely]] return false;
					const c8 c{ input[0] };
					if constexpr (opts.Comment) { if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == ',' || c == '\'' || c == '"' || c == '`' || c == '{' || c == '}' || c == '[' || c == ']' || c == '/')return true; }
					else/*                   */ { if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == ',' || c == '\'' || c == '"' || c == '`' || c == '{' || c == '}' || c == '[' || c == ']')return true; }
					input += 1;
				}
			}
		}
	}
	template<ParsingOptions opts, auto...LogError>
	constexpr bool count_array_unknowns(Span<const c8> input, $UInt auto& result)noexcept
	{
		ASSERT(result == 0);
		for (;;)
		{
			if (!skip_garbage_comma<opts, LogError...>(input)) [[unlikely]]
				return false;
			if (input[0] == ']')
				return true;
			if constexpr (sizeof(result) < sizeof(void*))
				if (result == std::remove_cvref_t<decltype(result)>(-1)) [[unlikely]]
					return false;
			if (!skip_unknowns<opts, LogError...>(input)) [[unlikely]]
				return false;
			++result;
		}
	}
	template<ParsingOptions, auto...LogError>
	constexpr bool parse_impl(bool& result, Span<const c8>& input)noexcept
	{
		if (input.size() < 5) [[unlikely]] SJN_PARSE_RETURN_FAIL();//because `true}`
		if (consume_iequal_unsafe<"true">(input.m_pBegin)) { result = true; return true; }
		if (consume_iequal_unsafe<"false">(input.m_pBegin))[[likely]] { result = false; return true; }
		[[unlikely]];
		SJN_PARSE_RETURN_FAIL();
	}
	template<ParsingOptions, auto...LogError>
	constexpr bool parse_impl($AnyOf<f32,f64> auto& result, Span<const c8>& input)noexcept
	{
		const fast_float::from_chars_result status{ fast_float::from_chars(input.begin(), input.end(), result) };//std::from_chars(f32/f64) is slow af, also not constexpr
		if (!status) [[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		input.m_pBegin = status.ptr;
		return true;
	}
	template<ParsingOptions, auto...LogError>
	constexpr bool parse_impl($AnyOf<i8,i16,i32,i64> auto& result, Span<const c8>& input)noexcept
	{
		const std::from_chars_result status{ std::from_chars(input.begin(), input.end(), result) };
		if (status.ec != std::errc{}) [[unlikely]]//todo: #if __cpp_lib_to_chars >= 202306L
			SJN_PARSE_RETURN_FAIL();
		input.m_pBegin = status.ptr;
		return true;
	}
	template<ParsingOptions, auto...LogError>
	constexpr bool parse_impl($AnyOf<u8,u16,u32,u64> auto& result, Span<const c8>& input)noexcept
	{
		int base{10};
		if (input.size() > 2 && input[0] == '0')[[unlikely]]
		{
			/* */if (input[1] == 'x') { input += 2; base = 16; }
			else if (input[1] == 'b') { input += 2; base = 2; }
		}
		const std::from_chars_result status{ std::from_chars(input.begin(), input.end(), result, base) };
		if (status.ec != std::errc{}) [[unlikely]]//todo: #if __cpp_lib_to_chars >= 202306L
			SJN_PARSE_RETURN_FAIL();
		input.m_pBegin = status.ptr;
		return true;
	}
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<bool> auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<bool&>(result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<i8>   auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<i8&>  (result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<i16>  auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<i16&> (result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<i32>  auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<i32&> (result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<i64>  auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<i64&> (result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<u8>   auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<u8&>  (result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<u16>  auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<u16&> (result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<u32>  auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<u32&> (result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<u64>  auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<u64&> (result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<f32>  auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<f32&> (result), input)); }
	template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($WrapperOf<f64>  auto& result, Span<const c8>& input)noexcept { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(static_cast<f64&> (result), input)); }
	template<ParsingOptions opts, auto...LogError>
	constexpr bool parse_impl($Enum auto& result, Span<const c8>& input)noexcept
	{
		using T = std::remove_cvref_t<decltype(result)>;
		using E = Enum<T>;
		if (input[0] == '"') [[likely]]
		{
			static constexpr auto minEnumLen{ [] <usize...I>(std::index_sequence<I...>)static{ return compact<Algorithm::min(enum_name<E::entries[I]>.size...)>; }(std::make_index_sequence<E::count>{}) };
			if (input.size() < compact<minEnumLen + 3>) [[unlikely]] SJN_PARSE_RETURN_FAIL();
			const c8* const pBegin{ input.begin() + 1 };
			input += compact<minEnumLen + 1>;
			for (;;)
			{
				if (input.empty()) [[unlikely]] SJN_PARSE_RETURN_FAIL();
				if (input[0] == '"')
				{
					const StringView8 name{ pBegin, input.begin() };
					input += 1;
					SJN_PARSE_RETURN(parse_enum<true>(name, result));
				}
				input += 1;
			}
		}
		else [[unlikely]] if constexpr (opts.NumberEnums)
		{
			using Underlying = std::underlying_type_t<T>;
			if consteval
			{
				Underlying underlying;
				if (!parse_impl<opts, LogError...>(underlying, input)) [[unlikely]] SJN_PARSE_RETURN_FAIL();
				result = static_cast<T>(underlying);
				return true;
			}
			else { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(reinterpret_cast<Underlying&>(result), input)); }
		}
		else SJN_PARSE_RETURN_FAIL();
	}
	template<ParsingOptions opts, auto...LogError>
	constexpr bool parse_impl($Flag auto& result, Span<const c8>& input)noexcept
	{
		using T = std::remove_cvref_t<decltype(result)>;
		if (input[0] == '"') [[likely]]
		{
			input += 1;
			T temp{};
			const c8* pBegin{ input.begin() };
			for (;;)
			{
				if (input.empty()) [[unlikely]] SJN_PARSE_RETURN_FAIL();
				if (input[0] == '"' || input[0] == '|')
				{
					const StringView8 name{ pBegin, input.begin() };
					pBegin = input.begin() + 1;
					if (!parse_flag_step(name, temp)) [[unlikely]] SJN_PARSE_RETURN_FAIL(+TS(", Invalid flag entry: "), name);
					if (input[0] == '"')
					{
						result = temp;
						input += 1;
						return true;
					}
				}
				input += 1;
			}
		}
		else
		{
			using Underlying = std::underlying_type_t<T>;
			if consteval
			{
				Underlying underlying;
				if (!parse_impl<opts, LogError...>(underlying, input)) [[unlikely]]
					SJN_PARSE_RETURN_FAIL();
				result = static_cast<T>(underlying);
				return true;
			}
			else { SJN_PARSE_RETURN(parse_impl<opts, LogError...>(reinterpret_cast<Underlying&>(result), input)); }
		}
	}
	
	template<ParsingOptions opts, auto...LogError>
	constexpr bool parse_impl($Char auto& result, Span<const c8>& input)noexcept
	{
		using T = std::remove_cvref_t<decltype(result)>;
		const c8 quote{ input[0] };
		if (quote != '"' && quote != '\'' && quote != '`') [[unlikely]] SJN_PARSE_RETURN_FAIL();
		input += 1;
		if (input.empty()) [[unlikely]] SJN_PARSE_RETURN_FAIL();
		if (input[0] == '\\') [[unlikely]]
		{
			input += 1;
			if (input.empty()) [[unlikely]]
				SJN_PARSE_RETURN_FAIL();
			switch (input[0])
			{
				case '"':case '\'':case '`':case '\\':	result = input[0];	break;
				case 'b':								result = '\b';		break;
				case 'f':								result = '\f';		break;
				case 'n':								result = '\n';		break;
				case 'r':								result = '\r';		break;
				case 't':								result = '\t';		break;
				case 'x'://assume \x## is never part of a 2/3/4 byte utf8 code point
				{
					if (input.size() <= 3) [[unlikely]] SJN_PARSE_RETURN_FAIL();
					u8 c;
					input += 1;
					if(!parse_slash_x_digits_unsafe(input.m_pBegin, c)) [[unlikely]] SJN_PARSE_RETURN_FAIL();
					input += 1;
					result = static_cast<T>(c);
					break;
				}
				case 'u': [[unlikely]]// \u#### can be part of a 4 byte utf16 code point, if yes, must be followed by another \u####
					if constexpr (opts.SlashU)
					{
						if (input.size() <= 5) [[unlikely]] SJN_PARSE_RETURN_FAIL();
						u16 c0;
						input += 1;
						if (!parse_slash_u_digits_unsafe(input.m_pBegin, c0)) [[unlikely]] SJN_PARSE_RETURN_FAIL();
						input += 3;
						if constexpr (sizeof(T) == 1)
						{
							if (c0 >= 0x100) [[unlikely]] SJN_PARSE_RETURN_FAIL();
							result = static_cast<T>(c0);
						}
						else if constexpr (sizeof(T) == 2)
							result = static_cast<T>(c0);
						else if constexpr (sizeof(T) == 4)
						{
							u32 codePoint;
							if (u16 c1;
								c0 >= 0xD800 && c0 <= 0xDBFF &&
								input.size() >= 7 &&
								consume_equal_unsafe<"\\u", false>(input.m_pBegin + 1) &&
								parse_slash_u_digits_unsafe(input.m_pBegin + 3, c1) &&
								c1 >= u16{ 0xDC00 } && c1 <= u16{ 0xDFFF })//paired high surrogate
							{
								input += 6;
								const u32 highSurrogate{ (u32{c0} - u32{0xD800}) * u32 { 0x400 } };
								const u32 lowSurrogate{ u32{c1} - u32{0xDC00} };
								codePoint = highSurrogate + lowSurrogate + u32{ 0x10000 };
							}
							else
								codePoint = c0;
							result = static_cast<T>(codePoint);
						}
						break;
					}
					else [[fallthrough]];
				default: [[unlikely]] SJN_PARSE_RETURN_FAIL();
			}
			input += 1;
		}
		else
		{
			if (input[0] == quote) [[unlikely]] SJN_PARSE_RETURN_FAIL();//use '"' or "\"", not """,''',```
			if constexpr (sizeof(T) == 1)
			{
				result = input[0];
				input += 1;
			}
			else if constexpr (sizeof(T) == 2 || sizeof(T) == 4)
			{
				const u8 codePointSize{ Wtf8::code_point_size(input[0]) };
				u32 codePoint;
				switch (codePointSize)
				{
					case 1:codePoint = Wtf8::decode<1>(input.data()); break;
					case 2:codePoint = Wtf8::decode<2>(input.data()); break;
					case 3:codePoint = Wtf8::decode<3>(input.data()); break;
					case 4:codePoint = Wtf8::decode<4>(input.data()); break;
					default:UNREACHABLE;
				}
				if constexpr (sizeof(T) == 2) { if (codePoint >= 0x10000) [[unlikely]] SJN_PARSE_RETURN_FAIL();  }
				result = static_cast<T>(codePoint);
				input += codePointSize;
			}
		}
		if (!input.empty() && input[0] == quote)
		{
			input += 1;
			return true;
		}
		else [[unlikely]] SJN_PARSE_RETURN_FAIL();
	}
#define SJN_PARSE_STRING_PROCESS_CODEPOINT(codepoint) do{\
	     if constexpr (sizeof(C) == 1) { if (!Wtf8 ::encode(codepoint, [&](c8  x)LAMBDA_INLINE{ result.push_back(x); })) [[unlikely]] SJN_PARSE_RETURN_FAIL(); }\
	else if constexpr (sizeof(C) == 2) { if (!Wtf16::encode(codepoint, [&](c16 x)LAMBDA_INLINE{ result.push_back(x); })) [[unlikely]] SJN_PARSE_RETURN_FAIL(); }\
	else if constexpr (sizeof(C) == 4)result.push_back(codepoint);\
}while(0)
	//todo: no reallocation
	template<ParsingOptions opts, auto...LogError>
	constexpr bool parse_impl($StringOfCharSizeAny<1, 2, 4> auto& result, Span<const c8>& input)noexcept
	requires(requires{result.clear(); result.push_back(result[0]); })
	{
		using C = ElementOf<decltype(result)>;
		result.clear();
		const c8 quote{ input[0] };
		if (quote != '"' && quote != '\'' && quote != '`') [[unlikely]] SJN_PARSE_RETURN_FAIL();
		input += 1;
		for (;;)
		{
			if (input.empty()) [[unlikely]] SJN_PARSE_RETURN_FAIL();
			if (input[0] == quote)
			{
				input += 1;
				return true;
			}
			else if (input[0] == '\\') [[unlikely]]
			{
				input += 1;
				if (input.empty()) [[unlikely]]
					SJN_PARSE_RETURN_FAIL();
				switch (input[0])
				{
					case '"':case '\'':case '`':case '\\':	result.push_back(input[0]);	break;
					case 'b':								result.push_back('\b');		break;
					case 'f':								result.push_back('\f');		break;
					case 'n':								result.push_back('\n');		break;
					case 'r':								result.push_back('\r');		break;
					case 't':								result.push_back('\t');		break;
					case 'x'://assume \x## is never part of a 2/3/4 byte utf8 code point
					{
						if (input.size() <= 3) [[unlikely]] SJN_PARSE_RETURN_FAIL();
						u8 c;
						input += 1;
						if (!parse_slash_x_digits_unsafe(input.m_pBegin, c)) [[unlikely]] SJN_PARSE_RETURN_FAIL();
						input += 1;
						if (c > 0x7F) [[unlikely]]SJN_PARSE_RETURN_FAIL();
						result.push_back(static_cast<C>(c));
						break;
					}
					case 'u': [[unlikely]]// \u#### can be part of a 4 byte wtf16 code point, if yes, must be followed by another \u####
						if constexpr (opts.SlashU)
						{
							if (input.size() <= 5) [[unlikely]] SJN_PARSE_RETURN_FAIL();
							u16 c0;
							input += 1;
							if (!parse_slash_u_digits_unsafe(input.m_pBegin, c0)) [[unlikely]] SJN_PARSE_RETURN_FAIL();
							input += 3;
							if constexpr (sizeof(C) == 2)
								result.push_back(static_cast<C>(c0));
							else if constexpr (sizeof(C) == 1 || sizeof(C) == 4)
							{
								if (u16 c1;
									c0 >= 0xD800 && c0 <= 0xDBFF &&
									input.size() >= 7 &&
									consume_equal_unsafe<"\\u", false>(input.m_pBegin + 1) &&
									parse_slash_u_digits_unsafe(input.m_pBegin + 3, c1) &&
									c1 >= u16{ 0xDC00 } && c1 <= u16{ 0xDFFF })//paired high surrogate
								{
									input += 6;
									const u32 highSurrogate{ (u32{c0} - u32{0xD800}) * u32 { 0x400 } };
									const u32 lowSurrogate{ u32{c1} - u32{0xDC00} };
									const u32 codePoint{ highSurrogate + lowSurrogate + u32{ 0x10000 } };
									SJN_PARSE_STRING_PROCESS_CODEPOINT(codePoint);
								}
								else SJN_PARSE_STRING_PROCESS_CODEPOINT(c0);
							}
							break;
						}
						else [[fallthrough]];
					default: [[unlikely]] SJN_PARSE_RETURN_FAIL();
				}
				input += 1;
			}
			else
			{
				if constexpr (sizeof(C) == 1)
				{
					result.push_back(input[0]);
					input += 1;
				}
				else if constexpr (sizeof(C) == 2 || sizeof(C) == 4)
				{
					const u8 codePointSize{ Wtf8::code_point_size(input[0]) };
					u32 codePoint;
					switch (codePointSize)
					{
						case 1:codePoint = Wtf8::decode<1>(input.data()); break;
						case 2:codePoint = Wtf8::decode<2>(input.data()); break;
						case 3:codePoint = Wtf8::decode<3>(input.data()); break;
						case 4:codePoint = Wtf8::decode<4>(input.data()); break;
						default:UNREACHABLE;
					}
					SJN_PARSE_STRING_PROCESS_CODEPOINT(codePoint);
					input += codePointSize;
				}
			}
		}
	}
#undef SJN_PARSE_STRING_PROCESS_CODEPOINT
#define SJN_PARSE_OPTIONAL template<ParsingOptions opts, auto...LogError>constexpr bool parse_impl($Optional auto& result, Span<const c8>& input)noexcept requires(requires{result.emplace();} || requires{result.unchecked_emplace();})
#define SJN_PARSE_FIXSIZED_ARRAY template<ParsingOptions opts, auto...LogError>constexpr bool parse_impl($FixSized auto& result, Span<const c8>& input)noexcept requires(!$String<decltype(result)>)
#define SJN_PARSE_DYNAMIC_ARRAY template<ParsingOptions opts, auto...LogError>constexpr bool parse_impl($LinearContainer auto& result, Span<const c8>& input)noexcept requires(!$FixSized<decltype(result)> &&!$String<decltype(result)> &&requires{result.resize(result.size()); } && requires{ElementOf<decltype(result)>{}; })
#define SJN_PARSE_REFLECTION template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($Reflection auto& result, Span<const c8>& input)noexcept requires(!$ForEach<decltype(result)> && !$String<decltype(result)>)
#define SJN_PARSE_TUPLE template<ParsingOptions opts, auto...LogError>INLINE constexpr bool parse_impl($TupleLike auto& result, Span<const c8>& input)noexcept requires(!$LinearContainer<decltype(result)> && !$Reflection<decltype(result)>)
	SJN_PARSE_OPTIONAL;			//forward decls
	SJN_PARSE_FIXSIZED_ARRAY;	//forward decls
	SJN_PARSE_DYNAMIC_ARRAY;	//forward decls
	SJN_PARSE_REFLECTION;		//forward decls
	SJN_PARSE_TUPLE;			//forward decls

	template<ParsingOptions opts, auto...LogError, auto...I>
	constexpr bool parse_tuple($TupleLike auto& result, Span<const c8>& input, std::index_sequence<I...>)noexcept
	{
		if (input[0] != '[') [[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		input += 1;
		if (!((skip_garbage_comma<opts, LogError...>(input) && parse_impl<opts, LogError...>(get<I>(result), input))&& ...))[[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		if (!skip_garbage_comma<opts, LogError...>(input)) [[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		if (input[0] != ']')[[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		input += 1;
		return true;
	}
	SJN_PARSE_TUPLE
	{
		return parse_tuple<opts, LogError...>(result, input, std::make_index_sequence<std::tuple_size<std::remove_cvref_t<decltype(result)>>::value>{});
	}

	SJN_PARSE_OPTIONAL
	{
		if (input.size() >= 4 && consume_iequal_unsafe<"null">(input.m_pBegin))
		{
			result.reset();
			return true;
		}
		if (!result)
		{
			if constexpr (requires{result.unchecked_emplace(); })
				result.unchecked_emplace();
			else
				result.emplace();
		}
		SJN_PARSE_RETURN(parse_impl<opts, LogError...>(*result, input));
	}
	SJN_PARSE_FIXSIZED_ARRAY
	{
		if (input[0] != '[') [[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		input += 1;
		Compact<static_size<decltype(result)>> index{};
		for (;;)
		{
			if (!skip_garbage_comma<opts, LogError...>(input)) [[unlikely]]
				SJN_PARSE_RETURN_FAIL();
			if (input[0] == ']')
			{
				input += 1;
				SJN_PARSE_RETURN(index == static_size<decltype(result)>);
			}
			if (index == static_size<decltype(result)>) [[unlikely]]
				SJN_PARSE_RETURN_FAIL();
			if (!parse_impl<opts, LogError...>(result[index], input)) [[unlikely]]
				SJN_PARSE_RETURN_FAIL();
			++index;
		}
	}
	
	SJN_PARSE_DYNAMIC_ARRAY
	{
		if (input[0] != '[') [[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		input += 1;
		using Index = IndexOf<decltype(result)>;
		Index n{};
		if (!count_array_unknowns<opts, LogError...>(input, n)) [[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		if (result.size() != n)
		{
			using T = ElementOf<decltype(result)>;
			if constexpr (__is_trivially_copyable(T) && requires{result.resize_uninitialized(n); })result.resize_uninitialized(n);
			else/*                                                                               */result.resize(n);
		}
		for (auto& x: result)
		{
			if (!skip_garbage_comma<opts, LogError...>(input) || input[0] == ']') [[unlikely]]
				SJN_PARSE_RETURN_FAIL();
			if (!parse_impl<opts, LogError...>(x, input)) [[unlikely]]
				SJN_PARSE_RETURN_FAIL();
		}
		if (!skip_garbage_comma<opts, LogError...>(input) || input[0] != ']') [[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		input += 1;
		return true;
	}
	
	template<ParsingOptions opts, auto...LogError, auto...I>
	constexpr bool parse_reflection($Reflection auto& result, Span<const c8>& input, std::index_sequence<I...>)noexcept
	requires(!$ForEach<decltype(result)> && !$String<decltype(result)>)
	{
		using T = std::remove_cvref_t<decltype(result)>;
		static constexpr auto N{ compact<sizeof...(I)> };
		if (input[0] != '{') [[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		input += 1;
		if constexpr (N == 0 && !opts.AllowUnknowns)
		{
			if (!skip_garbage_comma<opts, LogError...>(input) || input[0] != '}') [[unlikely]]
				SJN_PARSE_RETURN_FAIL();
			else [[likely]]
			{
				input += 1;
				return true;
			}
		}
		else 
		{
			Bitset<N> not_seen{ [] static consteval
			{
				Bitset<N> result{};
				(..., result.template set<I, true>());
				return result;
			}() };
			for (;;)
			{
				if (!skip_garbage_comma<opts, LogError...>(input)) [[unlikely]]
					SJN_PARSE_RETURN_FAIL();
				if (input[0] == '}')
				{
					if constexpr (!opts.AllowMissing)
						if (not_seen.any()) [[unlikely]]
							SJN_PARSE_RETURN_FAIL(+TS(", Some keys missing"));
					input += 1;
					return true;
				}
				if constexpr (N == 1 && !opts.AllowUnknowns)
				{
					if (input.size() < Reflection::name<T, 0>.size) [[unlikely]]
						SJN_PARSE_RETURN_FAIL();
					if constexpr (opts.KeyVerify)
						if (Reflection::name<T, 0> != input.subSpanFront(Reflection::name<T, 0>.size)) [[unlikely]]
							SJN_PARSE_RETURN_FAIL();
					if (!not_seen.template at<0>()) [[unlikely]]
						SJN_PARSE_RETURN_FAIL(+(TS(", Duplicate key: ") + Reflection::name<T, 0>));
					input += Reflection::name<T, 0>.size;
					if (!skip_garbage<opts, LogError...>(input)) [[unlikely]]
						SJN_PARSE_RETURN_FAIL();
					if (input[0] == ':')//':' is sometimes optional
					{
						input += 1;
						if (!skip_garbage<opts, LogError...>(input)) [[unlikely]]
							SJN_PARSE_RETURN_FAIL();
					}
					if(!parse_impl<opts, LogError...>(Reflection::at<0>(result), input)) [[unlikely]]
						SJN_PARSE_RETURN_FAIL(+TS(", Failed to parse ") + Reflection::name<T, 0>);
					not_seen.template set<0, false>();
				}
				else
				{
					static constexpr auto minKeyLen{ compact<Algorithm::min(usize{Reflection::name<T, I>.size}..., usize(-1))> };
					static constexpr auto maxKeyLen{ compact<Algorithm::max(usize{Reflection::name<T, I>.size}..., usize{})> };
					const c8* const pKeyBegin{ input.begin() };
					if constexpr (!opts.AllowUnknowns)//skip minKeyLen of known keys
					{
						if (input.size() < compact<minKeyLen + 2>) [[unlikely]]//key takes minKeyLen, value takes at least 1 char, last '}' takes 1 char
							SJN_PARSE_RETURN_FAIL();
						input += minKeyLen;
					}
					if constexpr (opts.AllowUnknowns || minKeyLen != maxKeyLen)
						for (;;)
						{
							if (input.empty()) [[unlikely]] SJN_PARSE_RETURN_FAIL();
							const c8 c{ input[0] };
							if (is_end_of_key<opts>(c))break;
							input += 1;
						}
					const Span<const c8> key{ pKeyBegin, input.begin() };
					if (!skip_garbage<opts, LogError...>(input)) [[unlikely]]SJN_PARSE_RETURN_FAIL();
					if (input[0] == ':')
					{
						input += 1;
						if (!skip_garbage<opts, LogError...>(input)) [[unlikely]]SJN_PARSE_RETURN_FAIL();
					}
		#if !MSVC
					if constexpr (N >= 2 && !opts.AllowUnknowns)//todo
					{
						using Hasher = MinHash<true, Reflection::name<T, I>.chars...>;//assume key.size() >= minKeyLen
						using Hash = Hasher::Result;
						static constexpr std::array<Hash, N> hashes{ Hasher::operator()(Reflection::name<T, I>.chars)... };
						if (!Switch<hashes>(
							Hasher::operator()(key),
							[&]<Hash H>(ImmediateValue<H>)LAMBDA_INLINE
							{
								static constexpr auto index{ Algorithm::find_index_unsafe(hashes, H) };//for MinHash, index == H
								if constexpr (opts.KeyVerify)
									if (Reflection::name<T, index> != key) [[unlikely]]
										SJN_PARSE_RETURN_FAIL(+TS(", Failed to parse unknown key "), key);
								if (!not_seen.template at<index>()) [[unlikely]]
									SJN_PARSE_RETURN_FAIL(+(TS(", Duplicate key: ") + Reflection::name<T, index>));
								if (parse_impl<opts, LogError...>(Reflection::at<index>(result), input)) [[likely]]
								{
									not_seen.template set<index, false>();
									return true;
								}
								else [[unlikely]]
									SJN_PARSE_RETURN_FAIL(+TS(", Failed to parse "), key);
							},
							[&]LAMBDA_INLINE //name not found, not even hash match
							{
								[[unlikely]];
								SJN_PARSE_RETURN_FAIL(+TS(", Unknown key "), key);
							}
							)) [[unlikely]]
								return false;
					}
					else
		#endif
					{
						bool error{};
						const bool key_not_found
						{ ((
							(Reflection::name<T, I> == key) ?
							(
								(!not_seen.template at<I>() || !parse_impl<opts, LogError...>(Reflection::at<I>(result), input)) ?
								((error = true),false) :
								(not_seen.template set<I, false>(),false)
							) :
							true
						)&&...) };
						if (key_not_found) [[unlikely]]
						{
							if constexpr (opts.AllowUnknowns) { if (!skip_unknowns<opts, LogError...>(input))SJN_PARSE_RETURN_FAIL(+TS(", Failed to parse unknown key "), key); }
							else SJN_PARSE_RETURN_FAIL(+TS(", Unknown key "), key);
						}
						if (error) [[unlikely]]
							SJN_PARSE_RETURN_FAIL(+TS(", Failed to parse "), key);
					}
				}
			}
		}
	}
	SJN_PARSE_REFLECTION
	{
		return parse_reflection<opts, LogError...>(result, input, std::make_index_sequence<Reflection::size<std::remove_cvref_t<decltype(result)>>>{});
	}
	template<ParsingOptions opts, auto...LogError>
	requires(opts.valid())
	INLINE constexpr bool parse($Reflection auto& result, Span<const c8> input)noexcept
	{
		if constexpr (opts.WtfVerify)
			if(!Wtf8::valid(input))[[unlikely]]
				SJN_PARSE_RETURN_FAIL();
		if (!skip_garbage<opts, LogError...>(input)) [[unlikely]]
			SJN_PARSE_RETURN_FAIL();
		if constexpr (opts.RollBackOnFail)
		{
			std::remove_cvref_t<decltype(result)> old{ result };
			if (!parse_impl<opts, LogError...>(result, input)) [[unlikely]]
			{
				result = MOV(old);
				SJN_PARSE_RETURN_FAIL();
			}
		}
		else if(!parse_impl<opts, LogError...>(result, input))//if return false, result must not be use at all, other than dtor
			SJN_PARSE_RETURN_FAIL();
		return true;
	}
	template<ParsingOptions opts, auto...LogError>
	INLINE bool parse(auto& result, Span<const u8> input)noexcept
	requires(opts.valid())
	{
		return parse<opts, LogError...>(result, { reinterpret_cast<const c8*>(input.begin()), reinterpret_cast<const c8*>(input.end()) });
	}
	#undef SJN_PARSE_SKIP_GARBAGE_IMPL
	#undef SJN_PARSE_GARBAGE_CASE
	#undef SJN_PARSE_RETURN_FAIL_IMPL
	#undef SJN_PARSE_RETURN_FAIL
	#undef SJN_PARSE_RETURN
	#undef SJN_PARSE_OPTIONAL
	#undef SJN_PARSE_FIXSIZED_ARRAY
	#undef SJN_PARSE_DYNAMIC_ARRAY
	#undef SJN_PARSE_REFLECTION
	#undef SJN_PARSE_TUPLE
}
#include <optional>
#include <string>
#include <tuple>
#include <vector>
#include "../../Math/Numeric/IsNaN.h"
namespace details
{
	DEFINE_ENUM(SjnTestEnum, u16, (あ, 123), (bb, 444));
	FLAG(SjnTestFlag, u8)
	{
		あ = 1,
		啊 = 2,
		ㅏ = 8
	};
	struct Time
	{
	private:
		u16 m_totalMinutes;//minutes since 00:00
	public:
		constexpr u16 minutes()const noexcept { return m_totalMinutes; }
		constexpr operator u16& ()noexcept { return m_totalMinutes; }	 //parse_impl(Time& result, Span<const c8>& input) is better fit than
		constexpr operator u16()const noexcept { return m_totalMinutes; }//parse_impl($WrapperOf<u16>  auto& result, Span<const c8>& input)
		template<auto...args>
		friend INLINE constexpr bool parse_impl(Time& result, Span<const c8>& input)noexcept
		{
			u16 value;
			if (!Sjn::parse_impl<args...>(value, input)) [[unlikely]]
				return false;
			const u16 h{ static_cast<u16>(value / 100) };
			const u16 m{ static_cast<u16>(value % 100) };
			if (h >= 24) [[unlikely]] return false;
			if (m >= 60) [[unlikely]] return false;
			result.m_totalMinutes = h * 60 + m;
			return true;
		}
	};
	struct SjnTestStruct
	{
		struct A
		{
			SjnTestEnum e;
			SjnTestFlag f;
			std::array<f64, 3> def;
		};
		struct B {};
		struct C { bool x; };
		B empty;
		C single_member;
		bool a0123;
		bool b456;
		i8  _i8;
		i16 _i16;
		i32 _i32;
		i64 _i64;
		u8  _u8;
		u16 _u16;
		u32 _u32;
		u64 _u64;
		f32 _f32;
		f64 _f64;
		Time time;
		std::tuple<std::tuple<>, std::pair<bool, i32>> _t;	//works with any type that impl std::tuple_size<T>, std::tuple_element<I, T>, get<I, T>, see $TupleLike for detail
		std::basic_string<char8_t> $;						//works with custom string type as well, see $String for detail
		std::basic_string<char16_t> あ;						//cjks are valid c++ identifiers
		std::basic_string<char32_t> 啊;						//😀 is not
		std::basic_string<wchar_t> ㅏ;						//its not parser limitation, blame P1949R7
		std::optional<A> vec_fix[4];
		std::vector<A> vec;									//works with custom types as well, see $LinearContainer for detail
		char8_t c8;
		char16_t c16;
		char32_t c32;
		wchar_t wc;
	};

	STATIC_ASSERT([] static
	{
		const FixedString inputComment{ R"(
		{
			empty:{/**/}
			single_member{/*/*/x:tRuE/*/*/}
			b456 : trUe,	//keys are c++ identifiers
			a0123 : FaLsE	//c++ identifiers contain no special chars
			_i8: -127		//c++ identifiers need no quotes
			_i16: 12324		//neither does key
			_i32: 2147483647//in fact quoted keys are not allowed
			_i64: 9223372036854775807
			_u8: 255
			_u16: 00123//
			_u32: 0b1111111111111111
				/*/*////
			_u64: 0xffffffffffffffff
			_f32: -inFiNiTy
				/***/
			_f64:/*asdf546*/-nAn///
			time: 1234//"12:34"
			_t[/**/[/**/]/**/,/**/[/**/true/**/,/**/123/**/]/**/]//
			$"\u3042\n\t\x00啊\uD83D\uDE00😀\"'`"
			あ/**/'\u3042啊\n\t\x00\uD83D\uDE00😀"\'`'
			啊`\u3042あ\uD83D\uDE00😀"'\``
			ㅏ:/*""*/"\u3042啊aあ\uD83D\uDE00😀"
			vec_fix/*':' is not needed here*/[
				{e"あ", f"あ|啊|ㅏ" def[1,2,3]}//':' is optional here
				{e"bb", f"啊"def: [-1,-2,-3]}{e: 123, f: 5 def: [-.1,-.2,-.3]}
				nUlL//null is case insensitive
			]
			vec//':' is not needed here
			[
				{e"あ", f"あ|啊|ㅏ" def[1,2,3]}//':' is optional here
				{e"bb", f"啊" def: [-1,-2,-3]}
				{e: 123, f:5 def: [-.1,-.2,-.3]}
			]
			c8"\""
			c16"\uffff"//garbage in, garbage out
			c32"😀"
			wc"あ"
		})" };
		const FixedString inputNoComment{ R"(
		{
			empty{}
			single_member:{x:TrUe}
			b456 : tRue,
			a0123 : fAlSe
			_i8: -127
			_i16: 12324
			_i32: 2147483647
			_i64: 9223372036854775807
			_u8: 255
			_u16: 00123
			_u32: 0b1111111111111111
			_u64: 0xffffffffffffffff
			_f32: -inFinity
			_f64:-nAn
			time: 1234
			_t[[],[true,123]]
			$"\u3042\n\t\x00啊\uD83D\uDE00😀\"'`"
			あ'\u3042啊\n\t\x00\uD83D\uDE00😀"\'`'
			啊`\u3042あ\uD83D\uDE00😀"'\``
			ㅏ:"\u3042啊aあ\uD83D\uDE00😀"
			vec_fix
			[
				{e"あ", f"あ|啊|ㅏ" def[1,2,3]}
				{e"bb", f"啊" def: [-1,-2,-3]}
				{e: 123, f: 5 def: [-.1,-.2,-.3]}
				nUlL
			]
			vec
			[
				{e"あ", f"あ|啊|ㅏ" def[1,2,3]}
				{e"bb", f"啊" def: [-1,-2,-3]}
				{e: 123, f: 5 def: [-.1,-.2,-.3]}
			]
			c8"\""
			c16"\uffff"
			c32"😀"
			wc"あ"
		})" };
		const FixedString inputUnknownKeys{ R"(
		{
			empty{empty:1}
			single_member:{x:TrUe y 2}
			b456 : tRue,_:""
			a0123 : fAlSe,_""
			_i8: -127, _"123"
			_i16: 12324, __[]
			_i32: 2147483647,__[[99]{}]
			_i64: 9223372036854775807,_:{a[]b{}c[[[{}]]]}
			_u8: 255, _ : { a : [ ] b :{ } c: [ [ [ { } ] ] ] }
			_u16: 00123,_:{a[""]b{inf:-1.1}c[[[{}inf Infinity]]]}
			_u32: 0b1111111111111111, _:"\\"
			_u64: 0xffffffffffffffff, _:"\""
			_f32: -inFinity, _:"\xab"
			_f64:-nAn, _:"\ucdef"
			time: 1234
			_t[[],[true,123]]
			$"\u3042\n\t\x00啊\uD83D\uDE00😀\"'`"_:"\xxx\uuuuu😀"
			あ'\u3042啊\n\t\x00\uD83D\uDE00😀"\'`'_ S()D*Fmn98SNDM(*Fnm980SNMDF9umN)(*SDNMFoinmopiklsJNDJKFnhioumnio~~~~
			啊`\u3042あ\uD83D\uDE00😀"'\``_:[[][][][[]]]
			ㅏ:"\u3042啊aあ\uD83D\uDE00😀"_ :{}
			vec_fix
			[
				{e"あ", f"あ|啊|ㅏ" def[1,2,3]}
				{e"bb", f"啊" def: [-1,-2,-3]}
				{e: 123, f: 5 def: [-.1,-.2,-.3]}
				nUlL
			]
			vec
			[
				{e"あ", f"あ|啊|ㅏ" def[1,2,3]}
				{e"bb", f"啊" def: [-1,-2,-3]}
				{e: 123, f: 5 def: [-.1,-.2,-.3]}
			]
			c8"\""
			c16"\uffff"
			c32"😀"
			wc"あ"
		})" };
		SjnTestStruct s0{};
		SjnTestStruct s1{};
		SjnTestStruct s2{};
		if(!Sjn::parse<{.RollBackOnFail{0},.NumberEnums{1},.Comment{0},.KeyVerify{0},.SlashU{1},.AllowUnknowns{0},.AllowMissing{0}}>(s0,inputNoComment))return false;
		if(!Sjn::parse<{.RollBackOnFail{0},.NumberEnums{1},.Comment{1},.KeyVerify{0},.SlashU{1},.AllowUnknowns{0},.AllowMissing{0}}>(s1,inputComment))return false;
		if(!Sjn::parse<{.RollBackOnFail{0},.NumberEnums{1},.Comment{1},.KeyVerify{1},.SlashU{1},.AllowUnknowns{1},.AllowMissing{0}}>(s2,inputUnknownKeys))return false;
		const auto verify{ [] (const SjnTestStruct& s)static
		{
			if (s.single_member.x != true)return false;
			if (s.a0123 != false)return false;
			if (s.b456 != true)return false;
			if (s._i8 != -127)return false;
			if (s._i16 != 12324)return false;
			if (s._i32 != 2147483647)return false;
			if (s._i64 != 9223372036854775807)return false;
			if (s._u8 != 255)return false;
			if (s._u16 != 123)return false;
			if (s._u32 != 65535)return false;
			if (s._u64 != 18446744073709551615ULL)return false;
			if (s._f32 != -std::numeric_limits<f32>::infinity())return false;
			if (!Math::isNaN(s._f64))return false;
			if (s.time.minutes() != 754)return false;
			if (get<0>(get<1>(s._t)) != true)return false;
			if (get<1>(get<1>(s._t)) != 123)return false;
			if (s.vec_fix[0]->e != SjnTestEnum::あ)return false;
			if (s.vec_fix[0]->f != (SjnTestFlag::あ | SjnTestFlag::啊 | SjnTestFlag::ㅏ))return false;
			if (s.vec_fix[0]->def != std::array<f64, 3>{1, 2, 3})return false;
			if (s.vec_fix[1]->e != SjnTestEnum::bb)return false;
			if (s.vec_fix[1]->f != SjnTestFlag::啊)return false;
			if (s.vec_fix[1]->def != std::array<f64, 3>{-1, -2, -3})return false;
			if (s.vec_fix[2]->e != SjnTestEnum::あ)return false;
			if (s.vec_fix[2]->f != SjnTestFlag(5))return false;
			if (s.vec_fix[2]->def != std::array<f64, 3>{-.1, -.2, -.3})return false;
			if (s.vec_fix[3].has_value())return false;
			if (s.vec.size() != 3)return false;
			if (s.vec[0].e != SjnTestEnum::あ)return false;
			if (s.vec[0].f != (SjnTestFlag::あ | SjnTestFlag::啊 | SjnTestFlag::ㅏ))return false;
			if (s.vec[0].def != std::array<f64, 3>{1, 2, 3})return false;
			if (s.vec[1].e != SjnTestEnum::bb)return false;
			if (s.vec[1].f != SjnTestFlag::啊)return false;
			if (s.vec[1].def != std::array<f64, 3>{-1, -2, -3})return false;
			if (s.vec[2].e != SjnTestEnum::あ)return false;
			if (s.vec[2].f != SjnTestFlag(5))return false;
			if (s.vec[2].def != std::array<f64, 3>{-.1, -.2, -.3})return false;
			if (s.$ != StringView{ u8"あ\n\t\x00啊😀😀\"'`" })return false;
			if (s.あ != StringView{ u"あ啊\n\t\x00😀😀\"'`" })return false;
			if (s.啊 != StringView{ U"ああ😀😀\"'`" })return false;
			if (s.ㅏ != StringView{ L"あ啊aあ😀😀" })return false;
			if (s.c8 != '"')return false;
			if (s.c16 != u'\uffff')return false;
			if (s.c32 != U'😀')return false;
			if (s.wc != L'あ')return false;
			return true;
		} };
		if (!verify(s0))return false;
		if (!verify(s1))return false;
		if (!verify(s2))return false;
		return true;
	}());
}