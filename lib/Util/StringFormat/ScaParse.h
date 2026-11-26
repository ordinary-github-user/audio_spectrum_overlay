#pragma once
#include <charconv>
#include "../../Windows/ExePath.h"
#include "../../Windows/File/Filepath.h"
#include "../Algorithm/Iequal.h"
#include "../Concepts/Char.h"
#include "../Concepts/ForEach.h"
#include "../Concepts/Optional.h"
#include "../Concepts/Same.h"
#include "../Concepts/Sig.h"
#include "../Concepts/String.h"
#include "../Concepts/TupleLike.h"
#include "../Concepts/WrapperOf.h"
#include "../DataStructures/Bitset.h"
#include "../DataStructures/Span.h"
#include "../DataStructures/TStringImpl.h"
#include "../ElementOf.h"
#include "../EnumImpl.h"
#include "../fast_float.h"
#include "../IterationControl.h"
#include "../Macro/ForEach.h"
#include "../Macro/If.h"
#include "../Macro/MovForward.h"
#include "../NameOf.h"
#include "../Reflection.h"
#include "../TextEncoding/TextEncodingStringConvertStatic.h"//encode<"asdf",c16> because c16 is wchar_t on windows, char16_t on linux
namespace Sca//(S)uperior (C)ommandLine (A)rguments
{
	struct ParsingOptions//default value = max speed
	{
		bool RollBackOnFail{};
		bool NumberEnums{};
		bool AllowMissing{true};
	};
	INLINE constexpr bool is_space(c16 c)noexcept { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
	INLINE constexpr bool is_quote(c16 c)noexcept { return c == '\'' || c == '"' || c == '`'; }
	INLINE constexpr bool is_letter(c16 c)noexcept { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
	INLINE constexpr bool at_boundary(Span<const c16> input) noexcept { return input.empty() || is_space(input[0]); }
	INLINE constexpr void skip_garbage(Span<const c16>& input)noexcept { while (!input.empty() && is_space(input[0])) input += 1; }
	INLINE constexpr bool skip_equal(Span<const c16>& input) noexcept
	{
		if (input.empty()) [[unlikely]] return false;
		else if (input[0] == '=') { input += 1; skip_garbage(input); return !input.empty();}
		else return true;
	}
	INLINE constexpr bool read_string(Span<const c16>& input, const c16*& pBegin, const c16*& pEnd)noexcept
	{
		const c16 quote{ input[0] };
		if (is_quote(quote))
		{
			input += 1;
			pBegin = input.begin();
			for(;;)
			{
				/* */if (input.empty()) [[unlikely]]return false;//unclosed quote
				else if (input[0] == quote)break;
				else input += 1;
			}
			pEnd = input.begin();
			input += 1;
			return at_boundary(input);
		}
		else
		{
			pBegin = input.begin();
			while (!input.empty() && !is_space(input[0])) input += 1;
			pEnd = input.begin();
			return true;
		}
	}
	INLINE constexpr bool skip_arg0(Span<const c16>& input)noexcept
	{
		if (input.empty()) [[unlikely]] return true;
		const c16 quote{ input[0] };
		if (is_quote(quote))
		{
			input += 1;
			for(;;)
			{
				/* */if (input.empty()) [[unlikely]]return false;//unclosed quote
				else if (input[0] == quote)break;
				else input += 1;
			}
			input += 1;
			return true;
		}
		else
		{
			while (!input.empty() && !is_space(input[0])) input += 1;
			return true;
		}
	}
	template<bool is_field = false, ParsingOptions = {}>
	constexpr bool parse_impl(bool& result, Span<const c16>& input)noexcept
	{
		if constexpr (is_field)
		{
			if (!input.empty() && input[0] == '=')
			{
				input += 1;
				skip_garbage(input);
				if (input.empty() || (input[0] != '0' && input[0] != '1'))[[unlikely]]return false;
				result = (input[0] == '1');
				input += 1;
				return at_boundary(input);
			}
			else
			{
				result = true;
				return true;
			}
		}
		else if (input.empty() || (input[0] != '0' && input[0] != '1')) [[unlikely]] return false;
		else
		{
			result = (input[0] == '1');
			input += 1;
			return at_boundary(input);
		}
	}
	template<bool is_field = false, ParsingOptions = {}>
	constexpr bool parse_impl($AnyOf<f32,f64> auto& result, Span<const c16>& input)noexcept
	{
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		const fast_float::from_chars_result_t<c16> status{ fast_float::from_chars(input.begin(), input.end(), result) };
		if (!status) [[unlikely]]
			return false;
		input.m_pBegin = status.ptr;
		return at_boundary(input);
	}
	template<bool is_field = false, ParsingOptions = {}>
	constexpr bool parse_impl($AnyOf<i8,i16,i32,i64> auto& result, Span<const c16>& input)noexcept
	{
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		const fast_float::from_chars_result_t<c16> status{ fast_float::from_chars(input.begin(), input.end(), result) };
		if (!status) [[unlikely]]
			return false;
		input.m_pBegin = status.ptr;
		return at_boundary(input);
	}
	template<bool is_field = false, ParsingOptions = {}>
	constexpr bool parse_impl($AnyOf<u8,u16,u32,u64> auto& result, Span<const c16>& input)noexcept
	{
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		int base{10};
		if (input.size() > 2 && input[0] == '0')[[unlikely]]
		{
			/* */if (input[1] == 'x') { input += 2; base = 16; }
			else if (input[1] == 'b') { input += 2; base = 2; }
		}
		const fast_float::from_chars_result_t<c16> status{ fast_float::from_chars(input.begin(), input.end(), result, base) };
		if (!status) [[unlikely]]
			return false;
		input.m_pBegin = status.ptr;
		return at_boundary(input);
	}
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<bool> auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<bool&>(result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<i8>   auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<i8&>  (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<i16>  auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<i16&> (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<i32>  auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<i32&> (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<i64>  auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<i64&> (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<u8>   auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<u8&>  (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<u16>  auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<u16&> (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<u32>  auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<u32&> (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<u64>  auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<u64&> (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<f32>  auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<f32&> (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>INLINE constexpr bool parse_impl($WrapperOf<f64>  auto& result, Span<const c16>& input)noexcept { return(parse_impl<is_field, opts>(static_cast<f64&> (result), input)); }
	template<bool is_field = false, ParsingOptions opts = {}>
	constexpr bool parse_impl($Enum auto& result, Span<const c16>& input)noexcept
	{
		using T = std::remove_cvref_t<decltype(result)>;
		using E = Enum<T>;
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		else if(input.empty())[[unlikely]]return false;
		if (input[0] < '0' || input[0] > '9') [[likely]]
		{
			static constexpr auto minEnumLen{ [] <usize...I>(std::index_sequence<I...>)static{ return compact<Algorithm::min(enum_name<E::entries[I], c16>.size...)>; }(std::make_index_sequence<E::count>{}) };
			if (input.size() < compact<minEnumLen>) [[unlikely]] return false;
			const c16* const pBegin{ input.begin() };
			input += compact<minEnumLen>;
			for (;;)
			{
				if (input.empty() || is_space(input[0]))
				{
					const StringView16 name{ pBegin, input.begin() };
					return parse_enum<true>(name, result);
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
				if (!parse_impl<is_field, opts>(underlying, input)) [[unlikely]] return false;
				result = static_cast<T>(underlying);
				return true;
			}
			else { return(parse_impl<is_field, opts>(reinterpret_cast<Underlying&>(result), input)); }
		}
		else return false;
	}
	template<bool is_field = false, ParsingOptions opts = {}>
	constexpr bool parse_impl($Flag auto& result, Span<const c16>& input)noexcept
	{
		using T = std::remove_cvref_t<decltype(result)>;
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		else if(input.empty())[[unlikely]]return false;
		if (input[0] < '0' || input[0] > '9') [[likely]]
		{
			T temp{};
			const c16* pBegin{ input.begin() };
			for (;;)
			{
				if (input.empty() || input[0] == '|' || is_space(input[0]))
				{
					const StringView16 name{ pBegin, input.begin() };
					if (!parse_flag_step(name, temp)) [[unlikely]] return false;
					if (input.empty() || input[0] != '|')
					{
						result = temp;
						return at_boundary(input);
					}
					pBegin = input.begin() + 1;
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
				if (!parse_impl<is_field, opts>(underlying, input)) [[unlikely]]
					return false;
				result = static_cast<T>(underlying);
				return true;
			}
			else { return(parse_impl<is_field, opts>(reinterpret_cast<Underlying&>(result), input)); }
		}
		else return false;
	}
	
	template<bool is_field = false, ParsingOptions = {}>
	constexpr bool parse_impl($Char auto& result, Span<const c16>& input)noexcept
	{
		using T = std::remove_cvref_t<decltype(result)>;
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		else if(input.empty())[[unlikely]]return false;
		const c16* pBegin;
		const c16* pEnd;
		if (!read_string(input, pBegin, pEnd))[[unlikely]]return false;
		const usize len{ static_cast<usize>(pEnd - pBegin) };
		if constexpr (sizeof(T) == 1)
		{
			if (len != 1 || pBegin[0] >= 0x100)return false;
			result = static_cast<T>(pBegin[0]);
			return true;
		}
		else if constexpr(sizeof(T) == 2)
		{
			if (len != 1)return false;
			result = static_cast<T>(pBegin[0]);
			return true;
		}
		else if constexpr(sizeof(T) == 4)
		{
			/* */if (len != 1 && len != 2)[[unlikely]]return false;
			else if (len == 1) result = u32{pBegin[0]};
			else
			{
				const u32 h{ pBegin[0] }, l{ pBegin[1] };
				if (h - 0xD800u >= 0x400u || l - 0xDC00u >= 0x400u) return false;
				result = 0x10000u + ((h - 0xD800u) << 10) + (l - 0xDC00u);
			}
			return true;
		}
		else static_assert(0);
	}
	template<bool is_field = false, ParsingOptions opts = {}>
	constexpr bool parse_impl($StringOfCharSize<2> auto& result, Span<const c16>& input)noexcept
	requires(!requires{result.push_back(result[0]); })//must be view/span, not owning
	{
		using T = std::remove_cvref_t<decltype(result)>;
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		else if(input.empty())[[unlikely]]return false;
		const c16* pBegin;
		const c16* pEnd;
		if (!read_string(input, pBegin, pEnd))[[unlikely]]return false;
		result = T{ pBegin, pEnd };
		return true;
	}
	template<bool is_field = false, ParsingOptions opts = {}>
	constexpr bool parse_impl(Windows::DosPath& result, Span<const c16>& input)noexcept
	{
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		else if(input.empty())[[unlikely]]return false;
		const c16* pBegin;
		const c16* pEnd;
		if (!read_string(input, pBegin, pEnd) || pBegin == pEnd)[[unlikely]]return false;
		const Span<const c16> path{ pBegin, pEnd };
		if (path.size() >= 3 && is_letter(path[0]) && path[1] == ':' && path[2] == '\\')
			result = Windows::DosPathView{ path };
		else
			result = Windows::exeDir() / path;
		return true;
	}
	template<bool is_field = false, ParsingOptions opts = {}>
	constexpr bool parse_impl(Windows::NtPath& result, Span<const c16>& input)noexcept
	{
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		else if(input.empty())[[unlikely]]return false;
		const c16* pBegin;
		const c16* pEnd;
		if (!read_string(input, pBegin, pEnd) || pBegin == pEnd)[[unlikely]]return false;
		const Span<const c16> path{ pBegin, pEnd };
		if (path.size() >= 7 && path[0] == '\\' && path[1] == '?' && path[2] == '?' && path[3] == '\\' && is_letter(path[4]) && path[5] == ':' && path[6] == '\\')
			result = Windows::NtPathView{ path };
		else
			result = Windows::exeDir() / path;
		return true;
	}
	template<bool is_field = false, ParsingOptions opts = {}>
	constexpr bool parse_impl($Optional auto& result, Span<const c16>& input)noexcept
	requires(!$Same<bool, std::remove_cvref_t<decltype(*result)>> && (requires{result.emplace();} || requires{result.unchecked_emplace();}))
	{
		if constexpr (is_field){if (!skip_equal(input))[[unlikely]]return false;}
		if (input.size() >= 4 && iequal_unsafe<FixedString{c16{'n'},c16{'u'},c16{'l'},c16{'l'}}>(input.m_pBegin) && (input.size() == 4 || is_space(input[4])))
		{
			input += 4;
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
		return(parse_impl<is_field, opts>(*result, input));
	}
	template<ParsingOptions opts, auto...I>
	constexpr bool parse_reflection($Reflection auto& result, Span<const c16>& input, $SigAnyOf<IterationControl(const Span<const c16>), void(const Span<const c16>)> auto&&...action_nameless)noexcept
	requires(Reflection::size<std::remove_cvref_t<decltype(result)>> != 0 && !$ForEach<decltype(result)> && !$String<decltype(result)> && sizeof...(action_nameless) <= 1)
	{
		using T = std::remove_cvref_t<decltype(result)>;
		static constexpr auto N{ compact<sizeof...(I)> };
		static constexpr bool allow_unknowns{ sizeof...(action_nameless) != 0 };
		static_assert(N != 0);
		Bitset<N> not_seen{ [] static consteval
		{
			Bitset<N> result{};
			(..., result.template set<I, true>());
			return result;
		}() };
		for (;;)
		{
			skip_garbage(input);
			if (input.empty())
			{
				if constexpr (!opts.AllowMissing)
					if (not_seen.any()) [[unlikely]]
						return false;
				return true;
			}
			else
			{
				static constexpr auto minKeyLen{ compact<usize{1} + Algorithm::min(usize{encode<Reflection::name<T, I>.chars, c16>.size()}..., usize(-1))> };
				static constexpr auto maxKeyLen{ compact<usize{1} + Algorithm::max(usize{encode<Reflection::name<T, I>.chars, c16>.size()}..., usize{})> };
				const c16* const pKeyBegin{ input.begin() };
				if constexpr (!allow_unknowns)//skip minKeyLen of known keys
				{
					if (input.size() < minKeyLen) [[unlikely]]
						return false;
					input += minKeyLen;
				}
				if constexpr (allow_unknowns || minKeyLen != maxKeyLen)
					while(!input.empty() && !is_space(input[0]) && input[0] != '=')
						input += 1;
				else
					if (!input.empty() && !is_space(input[0]) && input[0] != '=') [[unlikely]]
						return false;
				const Span<const c16> key{ pKeyBegin, input.begin() };
				skip_garbage(input);
				bool error{};
				const bool key_not_found
				{ ((
					(TString<c8, '-'>{} + Reflection::name<T, I> == key) ?
					(
						(!not_seen.template at<I>() || !parse_impl<true, opts>(Reflection::at<I>(result), input)) ?
						((error = true),false) :
						(not_seen.template set<I, false>(),false)
					) :
					true
				)&&...) };
				if (key_not_found) [[unlikely]]
				{
					if constexpr (allow_unknowns)
					{
						input.m_pBegin = key.begin();
						const c16* pBegin;
						const c16* pEnd;
						if (!read_string(input, pBegin, pEnd))[[unlikely]]return false;
						if constexpr ($Sig<decltype((..., action_nameless)), IterationControl(const Span<const c16>)>)
						{
							if (IterationControl::stop == (..., action_nameless(Span<const c16>{ pBegin, pEnd })))
								return true;
						}
						else
							(..., action_nameless(Span<const c16>{ pBegin, pEnd }));
					}
					else return false;
				}
				if (error) [[unlikely]]return false;
			}
		}
	}
	template<bool is_field = false, ParsingOptions opts = {}>
	INLINE constexpr bool parse_impl($Reflection auto& result, Span<const c16>& input, $SigAnyOf<IterationControl(const Span<const c16>), void(const Span<const c16>)> auto&&...action_nameless)noexcept
	requires(Reflection::size<std::remove_cvref_t<decltype(result)>> != 0 && !$ForEach<decltype(result)> && !$String<decltype(result)> && sizeof...(action_nameless) <= 1)
	{
		return []<usize...I>(std::index_sequence<I...>)static
		{
			return[](auto&&...args)STATIC_LAMBDA_INLINE
			{
				return parse_reflection<opts, I...>(FWD(args)...);
			};
		}(std::make_index_sequence<Reflection::size<std::remove_cvref_t<decltype(result)>>>{})(result, input, FWD(action_nameless)...);
	}
	template<ParsingOptions opts = {}>
	INLINE constexpr bool parse($Reflection auto& result, Span<const c16> input, $SigAnyOf<IterationControl(const Span<const c16>), void(const Span<const c16>)> auto&&...action_nameless)noexcept
	requires(Reflection::size<std::remove_cvref_t<decltype(result)>> != 0 && sizeof...(action_nameless) <= 1)
	{
		skip_garbage(input);
		if (!skip_arg0(input))[[unlikely]]return false;
		skip_garbage(input);
		if constexpr (opts.RollBackOnFail)
		{
			std::remove_cvref_t<decltype(result)> old{ result };
			if (!parse_impl<false, opts>(result, input, FWD(action_nameless)...)) [[unlikely]]
			{
				result = MOV(old);
				return false;
			}
		}
		else if(!parse_impl<false, opts>(result, input, FWD(action_nameless)...))//if return false, result must not be use at all, other than dtor
			return false;
		return true;
	}
	template<ParsingOptions opts = {}>
	INLINE constexpr bool parse(auto& result, Span<const c16> input)noexcept
	requires(!$Reflection<decltype(result)>)
	{
		skip_garbage(input);
		if (!skip_arg0(input))[[unlikely]]return false;
		skip_garbage(input);
		if constexpr (opts.RollBackOnFail)
		{
			std::remove_cvref_t<decltype(result)> old{ result };
			if (!parse_impl<false, opts>(result, input)) [[unlikely]]
			{
				result = MOV(old);
				return false;
			}
		}
		else if(!parse_impl<false, opts>(result, input))//if return false, result must not be use at all, other than dtor
			return false;
		return (skip_garbage(input), input.empty());
	}
	template<ParsingOptions opts = {}>
	constexpr bool parse(Span<const c16> input, $SigAnyOf<IterationControl(const Span<const c16>), void(const Span<const c16>)> auto&& action) noexcept
	{
		skip_garbage(input);
		if (!skip_arg0(input)) [[unlikely]] return false;
		for (;;)
		{
			skip_garbage(input);
			if (input.empty()) return true;
			const c16* pBegin;
			const c16* pEnd;
			if (!read_string(input, pBegin, pEnd)) [[unlikely]] return false;
			if constexpr ($Sig<decltype(action), IterationControl(const Span<const c16>)>)
			{
				if (action(Span<const c16>{ pBegin, pEnd }) == IterationControl::stop)
					return true;
			}
			else action(Span<const c16>{ pBegin, pEnd });
		}
	}
	namespace details::tests
	{
		DEFINE_ENUM(LifeState, u8, alive, dying, dead, respawnable, respawning);
		DEFINE_ENUM(Mood, u8, 喜, 怒, 哀, 楽);//utf8_units=[3,3,3,3]; utf16_units=[1,1,1,1]
		FLAG(TestFlag, u8){あ = 1,啊 = 2,ㅏ = 8};
		struct U8Field{u8 asdf;};
		struct SignedFloatFields{i16 i; f32 f;};
		struct BoolFields{bool v; bool w;};
		struct StringField{StringView16 s;};
		struct C16Field{c16 c;};
		struct C32Field{c32 c;};
		struct EnumField{LifeState a;};
		struct FlagField{TestFlag 旗;};
		struct OptionalField{std::optional<u8> o;};
		struct MixedFields{u8 x; u8 longer; f64 d;};
		struct TwoU8Fields{u8 a; u8 b;};
		struct UnicodeNames{u8 ü; u8 年; u8 𠀀;};//utf8_units=[2,3,4]; utf16_units=[1,1,2]
		struct AstralName{u8 𠀀;};

		//skip_arg0
		STATIC_ASSERT([]static{U8Field s; return  parse(s, encode<R"(.exe -asdf 123)", c16>) && s.asdf == 123;}());
		STATIC_ASSERT([]static{U8Field s; return  parse(s, encode<R"(".exe" -asdf 0)", c16>) && s.asdf == 0;}());
		STATIC_ASSERT([]static{U8Field s; return  parse(s, encode<R"(" .exe"-asdf=255)", c16>) && s.asdf == 255;}());
		STATIC_ASSERT([]static{U8Field s{}; return  parse(s, encode<R"('c:\a b\x.exe' -asdf 5)", c16>) && s.asdf == 5;}());
		STATIC_ASSERT([]static{U8Field s{}; return  parse(s, encode<R"(`c:\a b\x.exe` -asdf 5)", c16>) && s.asdf == 5;}());
		STATIC_ASSERT([]static{U8Field s{}; return !parse(s, encode<R"(".exe -asdf 5)", c16>);}());
		STATIC_ASSERT([]static{U8Field s{}; return  parse(s, encode<R"(  .exe   -asdf   9   )", c16>) && s.asdf == 9;}());
		STATIC_ASSERT([]static{U8Field s{}; return  parse(s, encode<".exe\t-asdf\t7", c16>) && s.asdf == 7;}());

		//u8 fields
		STATIC_ASSERT([]static{U8Field s{}; return  parse(s, encode<R"(.exe -asdf = 7)", c16>) && s.asdf == 7;}());
		STATIC_ASSERT([]static{U8Field s{}; return  parse(s, encode<R"(.exe -asdf 0xff)", c16>) && s.asdf == 255;}());
		STATIC_ASSERT([]static{U8Field s{}; return  parse(s, encode<R"(.exe -asdf 0b101)", c16>) && s.asdf == 5;}());
		STATIC_ASSERT([]static{U8Field s{}; return !parse(s, encode<R"(.exe -asdf 256)", c16>);}());
		STATIC_ASSERT([]static{U8Field s{}; return !parse(s, encode<R"(.exe -asdf 12abc)", c16>);}());
		STATIC_ASSERT([]static{U8Field s; return !parse(s, encode<R"("c:\.exe" -asdf -0)", c16>);}());
		STATIC_ASSERT([]static{U8Field s{}; return !parse(s, encode<R"(.exe -asdf)", c16>);}());
		STATIC_ASSERT([]static{U8Field s{}; return !parse(s, encode<R"(.exe -asdf=)", c16>);}());
		//u8
		STATIC_ASSERT([]static{u8 v{}; return  parse(v, encode<R"(.exe 42)", c16>) && v == 42;}());
		STATIC_ASSERT([]static{u8 v{}; return  parse(v, encode<R"(.exe 42   )", c16>) && v == 42;}());
		STATIC_ASSERT([]static{u8 v{}; return !parse(v, encode<R"(.exe 42 43)", c16>);}());
		STATIC_ASSERT([]static{u8 v{}; return !parse(v, encode<R"(.exe)", c16>);}());

		//i16 f32 fields
		STATIC_ASSERT([]static{MixedFields s{}; return  parse(s, encode<R"(.exe -d -2.25)", c16>) && s.d == -2.25;}());
		STATIC_ASSERT([]static{MixedFields s{}; return  parse(s, encode<R"(.exe -d=2.5e-1)", c16>) && s.d == 0.25;}());
		STATIC_ASSERT([]static{MixedFields s{}; return !parse(s, encode<R"(.exe -d 1,5)", c16>);}());
		STATIC_ASSERT([]static{MixedFields s{}; return !parse(s, encode<R"(.exe -d abc)", c16>);}());
		STATIC_ASSERT([]static{SignedFloatFields s{}; return  parse(s, encode<R"(.exe -i=-123 -f -0.5)", c16>) && s.i == -123 && s.f == -0.5f;}());
		STATIC_ASSERT([]static{SignedFloatFields s{}; return  parse(s, encode<R"(.exe -i -32768)", c16>) && s.i == -32768;}());
		STATIC_ASSERT([]static{SignedFloatFields s{}; return !parse(s, encode<R"(.exe -i 32768)", c16>);}());

		//bool field
		STATIC_ASSERT([]static{BoolFields s{}; return  parse(s, encode<R"(.exe -v)", c16>) && s.v && !s.w;}());
		STATIC_ASSERT([]static{BoolFields s{}; return  parse(s, encode<R"(.exe -w -v)", c16>) && s.v && s.w;}());
		STATIC_ASSERT([]static{BoolFields s{}; s.v = true; return parse(s, encode<R"(.exe -v=0 -w=1)", c16>) && !s.v && s.w;}());
		STATIC_ASSERT([]static{BoolFields s{}; return !parse(s, encode<R"(.exe -v=2)", c16>);}());
		STATIC_ASSERT([]static{BoolFields s{}; return !parse(s, encode<R"(.exe -v 1)", c16>);}());
		//bool
		STATIC_ASSERT([]static{bool b{}; return  parse(b, encode<R"(.exe 1)", c16>) && b;}());
		STATIC_ASSERT([]static{bool b{true}; return parse(b, encode<R"(.exe 0)", c16>) && !b;}());
		STATIC_ASSERT([]static{bool b{}; return !parse(b, encode<R"(.exe 2)", c16>);}());
		STATIC_ASSERT([]static{bool b{}; return !parse(b, encode<R"(.exe 10)", c16>);}());

		//StringView16 field
		STATIC_ASSERT([]static{StringField s; return  parse(s, encode<R"(.exe -s あ)", c16>) && s.s == L"あ";}());
		STATIC_ASSERT([]static{StringField s; return  parse(s, encode<R"(.exe -s a=b)", c16>) && s.s == L"a=b";}());
		STATIC_ASSERT([]static{StringField s; return  parse(s, encode<R"(" .exe"-s=ㅏ)", c16>) && s.s == L"ㅏ";}());
		STATIC_ASSERT([]static{StringField s; return  parse(s, encode<R"(" .exe"-s= -9 )", c16>) && s.s == L"-9";}());
		STATIC_ASSERT([]static{StringField s; return  parse(s, encode<R"(.exe -s='a b')", c16>) && s.s == L"a b";}());
		STATIC_ASSERT([]static{StringField s; return  parse(s, encode<R"("c:\.exe" -s "' '")", c16>) && s.s == L"' '";}());
		STATIC_ASSERT([]static{StringField s; return  parse(s, encode<R"("c:\.exe" -s `"😀'`)", c16>) && s.s == L"\"😀'";}());
		STATIC_ASSERT([]static{StringField s; return  parse(s, encode<R"(.exe -s "")", c16>) && s.s.empty();}());
		STATIC_ASSERT([]static{StringField s; return !parse(s, encode<R"(.exe -s "unclosed)", c16>);}());
		STATIC_ASSERT([]static{StringField s; return !parse(s, encode<R"(.exe -s "a"b)", c16>);}());
		//StringView16
		STATIC_ASSERT([]static{StringView16 s; return  parse(s, encode<R"(.exe あ)", c16>) && s == L"あ";}());
		STATIC_ASSERT([]static{StringView16 s; return  parse(s, encode<R"(" .exe"-s=ㅏ)", c16>) && s == L"-s=ㅏ";}());
		STATIC_ASSERT([]static{StringView16 s; return  parse(s, encode<R"("c:\.exe" "' '")", c16>) && s == L"' '";}());

		//c16 field
		STATIC_ASSERT([]static{C16Field s{}; return  parse(s, encode<R"(.exe -c x)", c16>) && s.c == L'x';}());
		STATIC_ASSERT([]static{C16Field s{}; return  parse(s, encode<R"(.exe -c ' ')", c16>) && s.c == L' ';}());
		STATIC_ASSERT([]static{C16Field s{}; return !parse(s, encode<R"(.exe -c xy)", c16>);}());
		STATIC_ASSERT([]static{C16Field s{}; return !parse(s, encode<R"(.exe -c 😀)", c16>);}());
		STATIC_ASSERT([]static{C32Field s{}; return  parse(s, encode<R"(.exe -c 😀)", c16>) && s.c == U'😀';}());
		STATIC_ASSERT([]static{C32Field s{}; return !parse(s, encode<R"(.exe -c 😀😀)", c16>);}());

		//enum field
		STATIC_ASSERT([]static{EnumField s; return  parse(s, encode<R"(.exe -a alive)", c16>) && s.a == LifeState::alive;}());
		STATIC_ASSERT([]static{EnumField s; return  parse(s, encode<R"(" .exe"-a=respawning)", c16>) && s.a == LifeState::respawning;}());
		STATIC_ASSERT([]static{EnumField s{}; return  parse(s, encode<R"(.exe -a dead)", c16>) && s.a == LifeState::dead;}());
		STATIC_ASSERT([]static{EnumField s{}; return !parse(s, encode<R"(.exe -a zombie)", c16>);}());
		STATIC_ASSERT([]static{EnumField s; return  parse<{.NumberEnums=1}>(s, encode<R"(" .exe"-a= 3 )", c16>) && s.a == LifeState::respawnable;}());
		STATIC_ASSERT([]static{EnumField s; return !parse(s, encode<R"("c:\.exe" -a 0)", c16>);}());
		//enum
		STATIC_ASSERT([]static{LifeState e{}; return  parse(e, encode<R"(.exe respawnable)", c16>) && e == LifeState::respawnable;}());
		STATIC_ASSERT([]static{LifeState e{}; return !parse(e, encode<R"(.exe dead dead)", c16>);}());
		STATIC_ASSERT([]static{LifeState e{}; return !parse(e, encode<R"(.exe dea)", c16>);}());
		STATIC_ASSERT([]static{LifeState e{}; return  parse<{.NumberEnums=1}>(e, encode<R"(.exe 4)", c16>) && e == LifeState::respawning;}());

		//flag
		STATIC_ASSERT([]static{FlagField s; return  parse(s, encode<R"(.exe -旗 あ)", c16>) && s.旗 == TestFlag::あ;}());
		STATIC_ASSERT([]static{FlagField s{}; return  parse(s, encode<R"(.exe -旗 あ|啊|ㅏ)", c16>) && s.旗 == (TestFlag::あ|TestFlag::啊|TestFlag::ㅏ);}());
		STATIC_ASSERT([]static{FlagField s; return  parse(s, encode<R"(" .exe"-旗=ㅏ)", c16>) && s.旗 == TestFlag::ㅏ;}());
		STATIC_ASSERT([]static{FlagField s{}; return !parse(s, encode<R"(.exe -旗 あ|zzz)", c16>);}());
		STATIC_ASSERT([]static{FlagField s{}; return !parse(s, encode<R"(.exe -旗 あ|)", c16>);}());
		STATIC_ASSERT([]static{FlagField s; return  parse<{.NumberEnums=1}>(s, encode<R"(" .exe"-旗= 9 )", c16>) && s.旗 == (TestFlag::あ|TestFlag::ㅏ);}());
		STATIC_ASSERT([]static{FlagField s; return !parse(s, encode<R"("c:\.exe" -旗 0)", c16>);}());

		//optional field
		STATIC_ASSERT([]static{OptionalField s{}; return  parse(s, encode<R"(.exe -o 5)", c16>) && s.o && *s.o == 5;}());
		STATIC_ASSERT([]static{OptionalField s{}; s.o = u8{9}; return parse(s, encode<R"(.exe -o NuLl)", c16>) && !s.o;}());
		STATIC_ASSERT([]static{OptionalField s{}; return !parse(s, encode<R"(.exe -o nullx)", c16>);}());

		//u8 u8 f64 fields
		STATIC_ASSERT([]static{MixedFields s{}; return  parse(s, encode<R"(.exe -x 3 -longer 5)", c16>) && s.x == 3 && s.longer == 5;}());
		STATIC_ASSERT([]static{MixedFields s{}; return  parse(s, encode<R"(.exe -d 2.5 -x 255 -longer 0x10)", c16>) && s.d == 2.5 && s.x == 255 && s.longer == 16;}());
		STATIC_ASSERT([]static{MixedFields s{}; return !parse(s, encode<R"(.exe -long 5)", c16>);}());
		STATIC_ASSERT([]static{MixedFields s{}; return !parse(s, encode<R"(.exe -longest 5)", c16>);}());
		STATIC_ASSERT([]static{MixedFields s{}; return !parse(s, encode<R"(.exe -X 3)", c16>);}());
		STATIC_ASSERT([]static{MixedFields s{}; return !parse(s, encode<R"(.exe -x 1 -x 2)", c16>);}());
		//bool bool fields
		STATIC_ASSERT([]static{BoolFields s{}; return !parse(s, encode<R"(.exe -x)", c16>);}());

		//non-ascii names
		STATIC_ASSERT([]static{UnicodeNames s{}; return  parse(s, encode<R"(.exe -ü 1 -年 2 -𠀀 3)", c16>) && s.ü == 1 && s.年 == 2 && s.𠀀 == 3;}());
		STATIC_ASSERT([]static{AstralName s{}; return  parse(s, encode<R"(.exe -𠀀 9)", c16>) && s.𠀀 == 9;}());
		STATIC_ASSERT([]static{Mood m{}; return  parse(m, encode<R"(.exe 哀)", c16>) && m == Mood::哀;}());
		STATIC_ASSERT([]static{Mood m{}; return !parse(m, encode<R"(.exe 嬉)", c16>);}());

		//ParsingOptions
		STATIC_ASSERT([]static{U8Field s{}; return  parse(s, encode<R"(.exe)", c16>) && s.asdf == 0;}());
		STATIC_ASSERT([]static{U8Field s{}; return !parse<{.AllowMissing=0}>(s, encode<R"(.exe)", c16>);}());
		STATIC_ASSERT([]static{U8Field s{}; return  parse<{.AllowMissing=0}>(s, encode<R"(.exe -asdf 1)", c16>) && s.asdf == 1;}());
		STATIC_ASSERT([]static{TwoU8Fields s{9, 9}; return !parse<{.RollBackOnFail=1}>(s, encode<R"(.exe -a 1 -b x)", c16>) && s.a == 9 && s.b == 9;}());
		STATIC_ASSERT([]static{TwoU8Fields s{9, 9}; return  parse<{.RollBackOnFail=1}>(s, encode<R"(.exe -a 1 -b 2)", c16>) && s.a == 1 && s.b == 2;}());

		//parse($Reflection, input, action)
		STATIC_ASSERT([]static{BoolFields s{}; usize n{}; return  parse(s, encode<R"(.exe a -v "b c" 'd e')", c16>, [&](const Span<const c16>){++n;}) && s.v && n == 3;}());
		STATIC_ASSERT([]static{BoolFields s{}; bool ok{}; return  parse(s, encode<R"(.exe -v 123.456)", c16>, [&](const Span<const c16> t){ok = StringView16{t.begin(), t.end()} == L"123.456";}) && s.v && ok;}());
		STATIC_ASSERT([]static{BoolFields s{}; bool ok{}; return  parse(s, encode<R"(.exe a=b)", c16>, [&](const Span<const c16> t){ok = StringView16{t.begin(), t.end()} == L"a=b";}) && ok;}());
		STATIC_ASSERT([]static{U8Field s{}; usize n{}; return  parse(s, encode<R"(.exe -asdf 7 8)", c16>, [&](const Span<const c16>){++n;}) && s.asdf == 7 && n == 1;}());
		STATIC_ASSERT([]static{BoolFields s{}; usize n{}; return  parse(s, encode<R"(.exe a b -v)", c16>, [&](const Span<const c16>){++n; return IterationControl::stop;}) && n == 1 && !s.v;}());
		STATIC_ASSERT([]static{BoolFields s{}; usize n{}; return !parse(s, encode<R"(.exe a "b)", c16>, [&](const Span<const c16>){++n;});}());
		STATIC_ASSERT([]static{StringField s; return !parse(s, encode<R"(.exe -s a b)", c16>);}());

		//parse(input, action)
		STATIC_ASSERT([]static{usize n{}; bool ok{true}; return parse(encode<R"(.exe a "b c" 'd e' `f g`)", c16>, [&](const Span<const c16> t){const StringView16 v{t.begin(), t.end()}; ok = ok && (n == 0 ? v == L"a" : n == 1 ? v == L"b c" : n == 2 ? v == L"d e" : v == L"f g"); ++n;}) && ok && n == 4;}());
		STATIC_ASSERT([]static{usize n{}; return  parse(encode<R"(.exe a b c)", c16>, [&](const Span<const c16>){return ++n == 2 ? IterationControl::stop : IterationControl::resume;}) && n == 2;}());
		STATIC_ASSERT([]static{usize n{}; return  parse(encode<R"(.exe)", c16>, [&](const Span<const c16>){++n;}) && n == 0;}());
		STATIC_ASSERT([]static{usize n{}; return !parse(encode<R"(.exe a "b c)", c16>, [&](const Span<const c16>){++n;});}());
		STATIC_ASSERT([]static{usize n{}; return !parse(encode<R"(.exe "a"b)", c16>, [&](const Span<const c16>){++n;});}());
	}
}