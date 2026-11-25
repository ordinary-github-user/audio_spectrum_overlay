#pragma once
#include <utility>
#include "../BasicTypes.h"
#include "../Compact.h"
#include "../Compiler.h"
#include "../Concepts/Char.h"
#include "../Concepts/FixSized.h"
#include "../Concepts/LinearContainer.h"
#include "../Concepts/Same.h"
#include "../Concepts/String.h"
#include "../ElementOf.h"
#include "../IndexTypeOf.h"
#include "../Inline.h"
#include "../Iota.h"
#include "../Macro/Consteval.h"
#include "../Macro/If.h"
#include "../Macro/MovForward.h"
#include "../StaticAssert.h"
#include "../StrcmpImpl.h"
#include "../StringLexicographicalCompare.h"
#include "../Traits/Like.h"
#define FIXED_STRING_IMPL																																									\
template<class Self>INLINE constexpr IndexOf<Self> index_of(this Self&& self, const ElementOf<Self>& e)noexcept { return static_cast<IndexOf<Self>>(__builtin_addressof(e)-self.data()); }	\
template<class Self>INLINE constexpr IndexOf<Self> index_of(this Self&& self, const ElementOf<Self>* p)noexcept { return static_cast<IndexOf<Self>>(p - self.data()); }						\
template<class Self>																																										\
constexpr auto find(this Self&& self, ElementOf<Self> c)noexcept																															\
{																																															\
	using Iter = std::remove_cvref_t<decltype(self.begin())>;																																\
	for (Iter pChar{ self.begin() }, pEnd{ self.end() }; pChar != pEnd; ++pChar)																											\
		if (*pChar == c)																																									\
			return pChar;																																									\
	return Iter{};																																											\
}																																															\
template<class Self>																																										\
constexpr auto rfind(this Self&& self, ElementOf<Self> c)noexcept																															\
{																																															\
	using Iter = std::remove_cvref_t<decltype(self.begin())>;																																\
	for (Iter pBegin{ self.begin() }, pChar{ self.end() };;)																																\
	{																																														\
		if (pChar == pBegin)return Iter{};																																					\
		--pChar;																																											\
		if (*pChar == c)return pChar;																																						\
	}																																														\
}																																															\
template<::FixedString target, Strcmp::Encoding encoding = Strcmp::Encoding::ascii, bool case_sensitive = true>																				\
constexpr auto starts_with(this auto&& self)noexcept																																		\
{																																															\
	if constexpr (target.empty())return true;																																				\
	else if constexpr (self.size() < target.size())return false;																															\
	else return std::remove_cvref_t<decltype([]<usize...I>(std::index_sequence<I...>)static																									\
	{																																														\
		return Strcmp::EqualImpl<T, encoding, case_sensitive, target[I]...>{};																												\
	}(std::make_index_sequence<target.size()>{}))>::match(self.data());																														\
}																																															\
template<::FixedString target, Strcmp::Encoding encoding = Strcmp::Encoding::ascii, bool case_sensitive = true>																				\
constexpr auto ends_with(this auto&& self)noexcept																																			\
{																																															\
	if constexpr (target.empty())return true;																																				\
	else if constexpr (self.size() < target.size())return false;																															\
	else return std::remove_cvref_t<decltype([]<usize...I>(std::index_sequence<I...>)static																									\
	{																																														\
		return Strcmp::EqualImpl<T, encoding, case_sensitive, target[I]...>{};																												\
	}(std::make_index_sequence<target.size()>{}))>::match(self.data() + constant<self.size() - target.size()>);																				\
}																																															\
template<::FixedString target, Strcmp::Encoding encoding = Strcmp::Encoding::ascii, bool case_sensitive = true>																				\
requires(!target.empty())																																									\
constexpr auto find(this auto&& self)noexcept																																				\
{																																															\
	using Iter = std::remove_cvref_t<decltype(self.begin())>;																																\
	if constexpr (self.size() < target.size())return Iter{};																																\
	else																																													\
	{																																														\
		for (Iter pChar{ self.begin() }, pEnd{ self.end() - compact<target.size() - 1> }; pChar != pEnd; ++pChar)																			\
			if (std::remove_cvref_t<decltype([]<usize...I>(std::index_sequence<I...>)static																									\
			{																																												\
				return Strcmp::EqualImpl<T, encoding, case_sensitive, target[I]...>{};																										\
			}(std::make_index_sequence<target.size()>{}))>::match(pChar))																													\
				return pChar;																																								\
		return Iter{};																																										\
	}																																														\
}																																															\
template<::FixedString target, Strcmp::Encoding encoding = Strcmp::Encoding::ascii, bool case_sensitive = true>																				\
requires(!target.empty())																																									\
constexpr auto rfind(this auto&& self)noexcept																																				\
{																																															\
	using Iter = std::remove_cvref_t<decltype(self.begin())>;																																\
	if constexpr (self.size() < target.size())return Iter{};																																\
	else																																													\
	{																																														\
		for (Iter pChar{ self.end() - target.size() };;--pChar)																																\
		{																																													\
			if (std::remove_cvref_t<decltype([]<usize...I>(std::index_sequence<I...>)static																									\
			{																																												\
				return Strcmp::EqualImpl<T, encoding, case_sensitive, target[I]...>{};																										\
			}(std::make_index_sequence<target.size()>{}))>::match(pChar))																													\
				return pChar;																																								\
			if (pChar == self.begin())																																						\
				return Iter{};																																								\
		}																																													\
	}																																														\
}
template<$Char T, usize N>
struct FixedString
{
	using char_type = T;
	using Index = Compact<N>;
	T m_data[N];
private:
	template<usize...I>
	INLINE CONSTEVAL FixedString(auto&& str, std::index_sequence<I...>)noexcept :m_data{ str[I]... } {}
public:
	INLINE constexpr FixedString()noexcept {}//uninitialized
	INLINE CONSTEVAL FixedString(const T(&str)[N + 1])noexcept : FixedString{ str, std::make_index_sequence<N>{} } {}
	INLINE constexpr FixedString($Same<T> auto...c)noexcept requires(sizeof...(c) == N) : m_data{ c... } {}
	INLINE CONSTEVAL explicit FixedString($LinearContainerOf<T> auto&& str)noexcept : FixedString{ str, std::make_index_sequence<N>{} } {}
	static IF(MSVC, constexpr, CONSTEVAL) Index size()noexcept { return N; }
	static CONSTEVAL bool empty()noexcept { return false; }
	template<class Self>INLINE constexpr Like<Self, T> front(this Self&& self) noexcept { return FWD(self).m_data[0]; }
	template<class Self>INLINE constexpr Like<Self, T> back(this Self&& self) noexcept { return FWD(self).m_data[N - 1]; }
	template<class Self>INLINE constexpr Like<Self, T> operator[](this Self&& self, Index index) noexcept { return FWD(self).m_data[index]; }
	template<class Self>INLINE constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return self.m_data; }
	template<class Self>INLINE constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return self.m_data; }
	template<class Self>INLINE constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.m_data + N; }
	template<Index start, Index end = N>
	requires(start <= end && end <= N)
	INLINE constexpr FixedString<T, end - start> substr()const noexcept
	{
		return[] <usize...I>(std::index_sequence<I...>)static consteval
		{
			return[](T const* p)STATIC_LAMBDA_INLINE{ return FixedString<T, end - start>{ p[compact<I + start>]... }; };
		}(std::make_index_sequence<end - start>{})(m_data);
	}
	template<usize width>
	requires(width >= N)
	INLINE constexpr FixedString<T, width> pad_left()const noexcept
	{
		return[] <auto...L, auto...R>(std::index_sequence<L...>, std::index_sequence<R...>)static
		{
			return[](T const* p)STATIC_LAMBDA_INLINE{ return FixedString<T, width>{ ((void)L, T{ ' ' })..., p[R]... }; };
		}(std::make_index_sequence<width - N>{}, std::make_index_sequence<N>{})(m_data);
	}
	template<usize width>
	requires(width >= N)
	INLINE constexpr FixedString<T, width> pad_right()const noexcept
	{
		return[]<auto...L, auto...R>(std::index_sequence<L...>, std::index_sequence<R...>)static consteval
		{
			return[](T const* p)STATIC_LAMBDA_INLINE{ return FixedString<T, width>{ p[L]..., ((void)R, T{ ' ' })... }; };
		}(std::make_index_sequence<N>{}, std::make_index_sequence<width - N>{})(m_data);
	}
	template<usize times>
	INLINE constexpr FixedString<T, N * times> repeat()const noexcept
	{
		return[] <usize...I>(std::index_sequence<I...>)static consteval
		{
			return[](T const* p)STATIC_LAMBDA_INLINE{ return FixedString<T, N* times>{ p[I % N]... }; };
		}(std::make_index_sequence<N* times>{})(m_data);
	}
	template<usize count>
	requires(count <= N)
	INLINE constexpr FixedString<T, N - count> erase(const T* p)const noexcept
	{
		if constexpr (!count)return *this;
		else
		{
			FixedString<T, N - count> result;
			T* pWrite{ result.data() };
			T* pWriteEnd{ result.data() + (N - count) };
			for (const T* pBegin{ data() }; pBegin != p; ++pBegin, ++pWrite)
				*pWrite = *pBegin;
			for (const T* pBegin{ p + count }; pWrite != pWriteEnd; ++pBegin, ++pWrite)
				*pWrite = *pBegin;
			return result;
		}
	}
	template<usize count>
	requires(count <= N)
	INLINE constexpr FixedString<T, N - count> erase(Index index)const noexcept
	{
		if constexpr (!count)return *this;
		else return erase<count>(data() + index);
	}
	FIXED_STRING_IMPL
};
template<$Char T>
struct FixedString<T, 0>
{
	using char_type = T;
	using Index = u8;
	INLINE CONSTEVAL FixedString(const T(&)[1])noexcept {}
	INLINE CONSTEVAL FixedString()noexcept {}
	INLINE constexpr explicit FixedString($LinearContainerOf<T> auto&&)noexcept {}
	INLINE static constexpr Index size()noexcept { return 0; }
	static CONSTEVAL bool empty()noexcept { return true; }
	template<class Self>INLINE constexpr Like<Self, T> front(this Self&&) noexcept;
	template<class Self>INLINE constexpr Like<Self, T> back(this Self&&) noexcept;
	template<class Self>INLINE constexpr Like<Self, T> operator[](this Self&&, Index) noexcept;
	template<class Self>INLINE constexpr LikePtr<Self, T> data(this Self&&) noexcept { return nullptr; }
	template<class Self>INLINE constexpr LikePtr<Self, T> begin(this Self&&) noexcept { return nullptr; }
	template<class Self>INLINE constexpr LikePtr<Self, T> end(this Self&&) noexcept { return nullptr; }
	template<Index start, Index end = 0>
	requires(start == 0 && end == 0)
	INLINE constexpr FixedString<T, 0> substr()const noexcept { return {}; }
	template<usize width>
	INLINE constexpr FixedString<T, width> pad_left()const noexcept
	{
		return[]<usize...I>(std::index_sequence<I...>)static consteval
		{
			return FixedString<T, width>{ ((void)I, T{' '})... };
		}(std::make_index_sequence<width>{});
	}
	template<usize width>
	INLINE constexpr FixedString<T, width> pad_right()const noexcept
	{
		return[]<usize...I>(std::index_sequence<I...>)static consteval
		{
			return FixedString<T, width>{ ((void)I,T{' '})... };
		}(std::make_index_sequence<width>{});
	}
	template<usize>
	INLINE constexpr FixedString<T, 0> repeat()const noexcept { return {}; }
	template<usize count>
	requires(count == 0)
	INLINE static constexpr FixedString<T, 0> erase(const T*) noexcept { return {}; }
	template<usize count>
	requires(count == 0)
	INLINE static constexpr FixedString<T, 0> erase(Index) noexcept { return {}; }
	FIXED_STRING_IMPL
};
#undef FIXED_STRING_IMPL
#if GCC//gcc bug
	_Pragma("GCC diagnostic push")_Pragma("GCC diagnostic ignored \"-Wunused-but-set-variable\"")
#endif
template<class T>concept $FixedString = $FixSized<T> && $String<T>;

template<$String auto string>
constexpr FixedString to_fixed_string{ [] <usize...I>(std::index_sequence<I...>)static
{
	return FixedString<ElementOf<decltype(string)>, string.size()>{string[I]...};
}(std::make_index_sequence<string.size()>{}) };

template<$FixedString L, $CArray R>
requires($Same<ElementOf<L>, ElementOf<R>>)
INLINE constexpr bool operator==(const L& l, const R& r)noexcept
{
	if constexpr (static_size<L> != static_size<R> - 1)return false;
	else
	{
	#if __cpp_structured_bindings >= 202411L
		static constexpr auto [...I] {iota<static_size<L>>};
		return (...&&(l[compact<I>] == r[compact<I>]));
	#else
		return[] <usize...I>(std::index_sequence<I...>)static consteval
		{
			return[](const L& l, const R& r)STATIC_LAMBDA_INLINE
			{
				return (...&&(l[I] == r[I]));
			};
		}(std::make_index_sequence<static_size<L>>{})(l, r);
	#endif
	}
}

template<$CArray L, $FixedString R>
requires($Same<ElementOf<L>, ElementOf<R>>)
INLINE constexpr bool operator==(const L& l, const R& r)noexcept
{
	if constexpr (static_size<L> - 1 != static_size<R>)return false;
	else
	{
	#if __cpp_structured_bindings >= 202411L
		static constexpr auto [...I] {iota<static_size<R>>};
		return (...&&(l[compact<I>] == r[compact<I>]));
	#else
		return[] <usize...I>(std::index_sequence<I...>)static consteval
		{
			return[](const L& l, const R& r)STATIC_LAMBDA_INLINE
			{
				return (...&&(l[I] == r[I]));
			};
		}(std::make_index_sequence<static_size<R>>{})(l, r);
	#endif
	}
}

template<$FixedString L, $FixedString R>
requires($Same<ElementOf<L>, ElementOf<R>>)
INLINE constexpr bool operator==(const L& l, const R& r)noexcept
{
	if constexpr (static_size<L> != static_size<R>)return false;
	else
	{
	#if __cpp_structured_bindings >= 202411L
		static constexpr auto [...I] {iota<static_size<L>>};
		return (...&&(l[compact<I>] == r[compact<I>]));
	#else
		return[] <usize...I>(std::index_sequence<I...>)static consteval
		{
			return[](const L& l, const R& r)STATIC_LAMBDA_INLINE
			{
				return (...&&(l[I] == r[I]));
			};
		}(std::make_index_sequence<static_size<L>>{})(l, r);
	#endif
	}
}

template<$FixedString L, $CArray R>
requires($Same<ElementOf<L>, ElementOf<R>>)
INLINE constexpr FixedString<ElementOf<L>, static_size<L> + static_size<R> - 1> operator+(const L& l, const R& r)noexcept
{
#if __cpp_structured_bindings >= 202411L
	static constexpr auto [...I] {iota<static_size<L>>};
	static constexpr auto [...J] {iota<static_size<R>-1>};
	return { l[compact<I>]..., r[compact<J>]... };
#else
	return[] <usize...I, usize...J>(std::index_sequence<I...>, std::index_sequence<J...>)static consteval
	{
		return[](const L& l, const R& r)STATIC_LAMBDA_INLINE->FixedString<ElementOf<L>, static_size<L> +static_size<R>-1>
		{
			return {l[I]..., r[J]...};
		};
	}(std::make_index_sequence<static_size<L>>{}, std::make_index_sequence<static_size<R>-1>{})(l, r);
#endif
}

template<$CArray L, $FixedString R>
requires($Same<ElementOf<L>, ElementOf<R>>)
INLINE constexpr FixedString<ElementOf<L>, static_size<L> + static_size<R> - 1> operator+(const L& l, const R& r)noexcept
{
#if __cpp_structured_bindings >= 202411L
	static constexpr auto [...I] {iota<static_size<L>-1>};
	static constexpr auto [...J] {iota<static_size<R>>};
	return { l[compact<I>]..., r[compact<J>]... };
#else
	return[] <usize...I, usize...J>(std::index_sequence<I...>, std::index_sequence<J...>)static consteval
	{
		return[](const L& l, const R& r)STATIC_LAMBDA_INLINE->FixedString<ElementOf<L>, static_size<L> +static_size<R>-1>
		{
			return {l[I]..., r[J]...};
		};
	}(std::make_index_sequence<static_size<L>-1>{}, std::make_index_sequence<static_size<R>>{})(l, r);
#endif
}

template<$FixedString L, $FixedString R>
requires($Same<ElementOf<L>, ElementOf<R>>)
INLINE constexpr FixedString<ElementOf<L>, static_size<L> + static_size<R>> operator+(const L& l, const R& r)noexcept
{
#if __cpp_structured_bindings >= 202411L
	static constexpr auto [...I] {iota<static_size<L>>};
	static constexpr auto [...J] {iota<static_size<R>>};
	return { l[compact<I>]..., r[compact<J>]... };
#else
	return[] <usize...I, usize...J>(std::index_sequence<I...>, std::index_sequence<J...>)static consteval
	{
		return[](const L& l, const R& r)STATIC_LAMBDA_INLINE->FixedString<ElementOf<L>, static_size<L> +static_size<R>>
		{
			return {l[I]..., r[J]...};
		};
	}(std::make_index_sequence<static_size<L>>{}, std::make_index_sequence<static_size<R>>{})(l, r);
#endif
}
template<$FixedString L>
INLINE constexpr FixedString<ElementOf<L>, static_size<L> + 1> operator+(const L& l, ElementOf<L> r)noexcept
{
#if __cpp_structured_bindings >= 202411L
	static constexpr auto [...I] {iota<static_size<L>>};
	return { l[compact<I>]..., r };
#else
	return[] <usize...I>(std::index_sequence<I...>)static consteval
	{
		return[](const L& l, ElementOf<L> r)STATIC_LAMBDA_INLINE->FixedString<ElementOf<L>, static_size<L> +1>
		{
			return { l[I]..., r };
		};
	}(std::make_index_sequence<static_size<L>>{})(l, r);
#endif
}
template<$Char Char, $FixedString R>
requires($Same<Char, ElementOf<R>>)
INLINE constexpr FixedString<Char, static_size<R> + 1> operator+(Char l, const R& r)noexcept
{
#if __cpp_structured_bindings >= 202411L
	static constexpr auto [...I] {iota<static_size<R>>};
	return { l, r[compact<I>]... };
#else
	return[] <usize...I>(std::index_sequence<I...>)static consteval
	{
		return[](Char l, const R& r)STATIC_LAMBDA_INLINE->FixedString<Char, static_size<R> +1>
		{
			return { l, r[I]... };
		};
	}(std::make_index_sequence<static_size<R>>{})(l, r);
#endif
}
#if GCC
	_Pragma("GCC diagnostic pop")
#endif
template<class Char, usize N>
FixedString(const Char(&)[N]) -> FixedString<Char, N - 1>;
template<class Char, $Same<Char>...Chars>
FixedString(Char, Chars...) -> FixedString<Char, sizeof...(Chars) + 1>;
template<bool x, FixedString l, FixedString r>constexpr FixedString fstring_if/*         */{ l };
template</*    */FixedString l, FixedString r>constexpr FixedString fstring_if<false, l, r>{ r };
STATIC_ASSERT(FixedString{ "" }.starts_with<"">());
STATIC_ASSERT(FixedString{ "" }.ends_with<"">());
STATIC_ASSERT(FixedString{ "0" }.starts_with<"">());
STATIC_ASSERT(FixedString{ "0" }.ends_with<"">());
STATIC_ASSERT(FixedString{ "0" }.starts_with<"0">());
STATIC_ASSERT(FixedString{ "0" }.ends_with<"0">());
STATIC_ASSERT(FixedString{ "00" }.starts_with<"0">());
STATIC_ASSERT(FixedString{ "00" }.ends_with<"0">());
STATIC_ASSERT(FixedString{ "01" }.starts_with<"0">());
STATIC_ASSERT(FixedString{ "01" }.ends_with<"1">());
STATIC_ASSERT(FixedString{ "01" }.starts_with<"01">());
STATIC_ASSERT(FixedString{ "01" }.ends_with<"01">());

STATIC_ASSERT(!FixedString{ "" }.starts_with<"0">());
STATIC_ASSERT(!FixedString{ "" }.ends_with<"0">());
STATIC_ASSERT(!FixedString{ "0" }.starts_with<"1">());
STATIC_ASSERT(!FixedString{ "0" }.ends_with<"1">());
STATIC_ASSERT(!FixedString{ "0" }.starts_with<"01">());
STATIC_ASSERT(!FixedString{ "0" }.ends_with<"01">());
STATIC_ASSERT(!FixedString{ "00" }.starts_with<"1">());
STATIC_ASSERT(!FixedString{ "00" }.ends_with<"1">());
STATIC_ASSERT(!FixedString{ "01" }.starts_with<"1">());
STATIC_ASSERT(!FixedString{ "01" }.ends_with<"0">());
STATIC_ASSERT(!FixedString{ "01" }.starts_with<"10">());
STATIC_ASSERT(!FixedString{ "01" }.ends_with<"10">());

STATIC_ASSERT(FixedString{ "" } == "");
STATIC_ASSERT(FixedString{ "0" } != "");
STATIC_ASSERT(FixedString{ "" } != "0");
STATIC_ASSERT(FixedString{ "0" } == "0");
STATIC_ASSERT(FixedString{ "0" } != "1");

STATIC_ASSERT(FixedString{ "" }.repeat<0>() == "");
STATIC_ASSERT(FixedString{ "" }.repeat<1>() == "");
STATIC_ASSERT(FixedString{ "" }.repeat<2>() == "");
STATIC_ASSERT(FixedString{ "1" }.repeat<0>() == "");
STATIC_ASSERT(FixedString{ "1" }.repeat<1>() == "1");
STATIC_ASSERT(FixedString{ "1" }.repeat<2>() == "11");
STATIC_ASSERT(FixedString{ "12" }.repeat<0>() == "");
STATIC_ASSERT(FixedString{ "12" }.repeat<1>() == "12");
STATIC_ASSERT(FixedString{ "12" }.repeat<2>() == "1212");

STATIC_ASSERT('a' + FixedString{ "" } == "a");
STATIC_ASSERT(FixedString{ "" } + 'b' == "b");
STATIC_ASSERT('h' + FixedString{ "ello" } == "hello");
STATIC_ASSERT(FixedString{ "hell" } + 'o' == "hello");
STATIC_ASSERT(FixedString{ "hello" } + FixedString{ "" } == "hello");
STATIC_ASSERT(FixedString{ "hell" } + FixedString{ "o" } == "hello");
STATIC_ASSERT(FixedString{ "hel" } + FixedString{ "lo" } == "hello");
STATIC_ASSERT(FixedString{ "he" } + FixedString{ "llo" } == "hello");
STATIC_ASSERT(FixedString{ "h" } + FixedString{ "ello" } == "hello");
STATIC_ASSERT(FixedString{ "" } + FixedString{ "hello" } == "hello");

STATIC_ASSERT("hello" + FixedString{ "" } == "hello");
STATIC_ASSERT("hell" + FixedString{ "o" } == "hello");
STATIC_ASSERT("hel" + FixedString{ "lo" } == "hello");
STATIC_ASSERT("he" + FixedString{ "llo" } == "hello");
STATIC_ASSERT("h" + FixedString{ "ello" } == "hello");
STATIC_ASSERT("" + FixedString{ "hello" } == "hello");

STATIC_ASSERT(FixedString{ "hello" } + "" == "hello");
STATIC_ASSERT(FixedString{ "hell" } + "o" == "hello");
STATIC_ASSERT(FixedString{ "hel" } + "lo" == "hello");
STATIC_ASSERT(FixedString{ "he" } + "llo" == "hello");
STATIC_ASSERT(FixedString{ "h" } + "ello" == "hello");
STATIC_ASSERT(FixedString{ "" } + "hello" == "hello");

STATIC_ASSERT(FixedString{ "" }.find('a') == nullptr);
STATIC_ASSERT(FixedString{ "bcd" }.find('a') == nullptr);
STATIC_ASSERT(FixedString{ "" }.rfind('a') == nullptr);
STATIC_ASSERT(FixedString{ "bcd" }.rfind('a') == nullptr);
STATIC_ASSERT(*FixedString{ "a" }.find('a') == 'a');
STATIC_ASSERT(*FixedString{ "abc" }.find('a') == 'a');
STATIC_ASSERT(*FixedString{ "a" }.rfind('a') == 'a');
STATIC_ASSERT(*FixedString{ "abc" }.rfind('a') == 'a');

STATIC_ASSERT(FixedString{ "hello" }.erase<0>(u8{ 2 }) == "hello");
STATIC_ASSERT(FixedString{ "hello" }.erase<1>(u8{ 0 }) == "ello");
STATIC_ASSERT(FixedString{ "hello" }.erase<1>(u8{ 4 }) == "hell");
STATIC_ASSERT(FixedString{ "hello" }.erase<2>(u8{ 1 }) == "hlo");
STATIC_ASSERT(FixedString{ "hello" }.erase<3>(u8{ 1 }) == "ho");
STATIC_ASSERT(FixedString{ "hello" }.erase<4>(u8{ 1 }) == "h");
STATIC_ASSERT(FixedString{ "hello" }.erase<5>(u8{ 0 }) == "");
STATIC_ASSERT(FixedString{ "" }.erase<0>(u8{ 0 }) == "");

STATIC_ASSERT(FixedString{ "" }.template pad_left<0>() == "");
STATIC_ASSERT(FixedString{ "" }.template pad_left<1>() == " ");
STATIC_ASSERT(FixedString{ "" }.template pad_left<2>() == "  ");
STATIC_ASSERT(FixedString{ "" }.template pad_right<0>() == "");
STATIC_ASSERT(FixedString{ "" }.template pad_right<1>() == " ");
STATIC_ASSERT(FixedString{ "" }.template pad_right<2>() == "  ");
STATIC_ASSERT(FixedString{ "a" }.template pad_left<1>() == "a");
STATIC_ASSERT(FixedString{ "a" }.template pad_left<2>() == " a");
STATIC_ASSERT(FixedString{ "a" }.template pad_right<1>() == "a");
STATIC_ASSERT(FixedString{ "a" }.template pad_right<2>() == "a ");
STATIC_ASSERT(FixedString{ "" }.template repeat<0>() == "");
STATIC_ASSERT(FixedString{ "" }.template repeat<1>() == "");
STATIC_ASSERT(FixedString{ "" }.template repeat<2>() == "");
STATIC_ASSERT(FixedString{ "a" }.template repeat<0>() == "");
STATIC_ASSERT(FixedString{ "a" }.template repeat<1>() == "a");
STATIC_ASSERT(FixedString{ "a" }.template repeat<2>() == "aa");
STATIC_ASSERT(FixedString{ "ab" }.template repeat<0>() == "");
STATIC_ASSERT(FixedString{ "ab" }.template repeat<1>() == "ab");
STATIC_ASSERT(FixedString{ "ab" }.template repeat<2>() == "abab");

STATIC_ASSERT(1==(FixedString{""}<FixedString{"264"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"264"}));
STATIC_ASSERT(0==(FixedString{"197"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"197"}>FixedString{""}));
STATIC_ASSERT(0==(FixedString{"9"}<FixedString{"43343"}));
STATIC_ASSERT(1==(FixedString{"9"}>FixedString{"43343"}));
STATIC_ASSERT(1==(FixedString{"3"}<FixedString{"75562"}));
STATIC_ASSERT(0==(FixedString{"3"}>FixedString{"75562"}));
STATIC_ASSERT(1==(FixedString{"02"}<FixedString{"44166"}));
STATIC_ASSERT(0==(FixedString{"02"}>FixedString{"44166"}));
STATIC_ASSERT(0==(FixedString{"22"}<FixedString{"038"}));
STATIC_ASSERT(1==(FixedString{"22"}>FixedString{"038"}));
STATIC_ASSERT(1==(FixedString{"0"}<FixedString{"4"}));
STATIC_ASSERT(0==(FixedString{"0"}>FixedString{"4"}));
STATIC_ASSERT(1==(FixedString{"2"}<FixedString{"549"}));
STATIC_ASSERT(0==(FixedString{"2"}>FixedString{"549"}));
STATIC_ASSERT(0==(FixedString{"87"}<FixedString{"4"}));
STATIC_ASSERT(1==(FixedString{"87"}>FixedString{"4"}));
STATIC_ASSERT(0==(FixedString{""}<FixedString{""}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{""}));
STATIC_ASSERT(1==(FixedString{"02"}<FixedString{"52"}));
STATIC_ASSERT(0==(FixedString{"02"}>FixedString{"52"}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"76"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"76"}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"91806"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"91806"}));
STATIC_ASSERT(1==(FixedString{"003"}<FixedString{"413"}));
STATIC_ASSERT(0==(FixedString{"003"}>FixedString{"413"}));
STATIC_ASSERT(1==(FixedString{"0623"}<FixedString{"8299"}));
STATIC_ASSERT(0==(FixedString{"0623"}>FixedString{"8299"}));
STATIC_ASSERT(1==(FixedString{"261"}<FixedString{"67"}));
STATIC_ASSERT(0==(FixedString{"261"}>FixedString{"67"}));
STATIC_ASSERT(0==(FixedString{"12"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"12"}>FixedString{""}));
STATIC_ASSERT(1==(FixedString{"2"}<FixedString{"503"}));
STATIC_ASSERT(0==(FixedString{"2"}>FixedString{"503"}));
STATIC_ASSERT(0==(FixedString{"924"}<FixedString{"5"}));
STATIC_ASSERT(1==(FixedString{"924"}>FixedString{"5"}));
STATIC_ASSERT(0==(FixedString{"118"}<FixedString{"1"}));
STATIC_ASSERT(1==(FixedString{"118"}>FixedString{"1"}));
STATIC_ASSERT(1==(FixedString{"2"}<FixedString{"56120"}));
STATIC_ASSERT(0==(FixedString{"2"}>FixedString{"56120"}));
STATIC_ASSERT(1==(FixedString{"2620"}<FixedString{"6297"}));
STATIC_ASSERT(0==(FixedString{"2620"}>FixedString{"6297"}));
STATIC_ASSERT(0==(FixedString{"74"}<FixedString{"51"}));
STATIC_ASSERT(1==(FixedString{"74"}>FixedString{"51"}));
STATIC_ASSERT(1==(FixedString{"094"}<FixedString{"339"}));
STATIC_ASSERT(0==(FixedString{"094"}>FixedString{"339"}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"073"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"073"}));
STATIC_ASSERT(0==(FixedString{"35"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"35"}>FixedString{""}));
STATIC_ASSERT(1==(FixedString{"0816"}<FixedString{"7"}));
STATIC_ASSERT(0==(FixedString{"0816"}>FixedString{"7"}));
STATIC_ASSERT(1==(FixedString{"1341"}<FixedString{"44"}));
STATIC_ASSERT(0==(FixedString{"1341"}>FixedString{"44"}));
STATIC_ASSERT(1==(FixedString{"5409"}<FixedString{"616"}));
STATIC_ASSERT(0==(FixedString{"5409"}>FixedString{"616"}));
STATIC_ASSERT(0==(FixedString{"75"}<FixedString{"38965"}));
STATIC_ASSERT(1==(FixedString{"75"}>FixedString{"38965"}));
STATIC_ASSERT(1==(FixedString{"0781"}<FixedString{"72176"}));
STATIC_ASSERT(0==(FixedString{"0781"}>FixedString{"72176"}));
STATIC_ASSERT(1==(FixedString{"1533"}<FixedString{"2985"}));
STATIC_ASSERT(0==(FixedString{"1533"}>FixedString{"2985"}));
STATIC_ASSERT(0==(FixedString{"81988"}<FixedString{"35627"}));
STATIC_ASSERT(1==(FixedString{"81988"}>FixedString{"35627"}));
STATIC_ASSERT(1==(FixedString{"3"}<FixedString{"5375"}));
STATIC_ASSERT(0==(FixedString{"3"}>FixedString{"5375"}));
STATIC_ASSERT(1==(FixedString{"6757"}<FixedString{"95981"}));
STATIC_ASSERT(0==(FixedString{"6757"}>FixedString{"95981"}));
STATIC_ASSERT(0==(FixedString{"80706"}<FixedString{"550"}));
STATIC_ASSERT(1==(FixedString{"80706"}>FixedString{"550"}));
STATIC_ASSERT(1==(FixedString{"23"}<FixedString{"8"}));
STATIC_ASSERT(0==(FixedString{"23"}>FixedString{"8"}));
STATIC_ASSERT(0==(FixedString{"404"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"404"}>FixedString{""}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"0053"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"0053"}));
STATIC_ASSERT(0==(FixedString{"6"}<FixedString{"2138"}));
STATIC_ASSERT(1==(FixedString{"6"}>FixedString{"2138"}));
STATIC_ASSERT(1==(FixedString{"68365"}<FixedString{"81"}));
STATIC_ASSERT(0==(FixedString{"68365"}>FixedString{"81"}));
STATIC_ASSERT(0==(FixedString{"92"}<FixedString{"9"}));
STATIC_ASSERT(1==(FixedString{"92"}>FixedString{"9"}));
STATIC_ASSERT(1==(FixedString{"360"}<FixedString{"76830"}));
STATIC_ASSERT(0==(FixedString{"360"}>FixedString{"76830"}));
STATIC_ASSERT(0==(FixedString{"7666"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"7666"}>FixedString{""}));
STATIC_ASSERT(0==(FixedString{"84841"}<FixedString{"2"}));
STATIC_ASSERT(1==(FixedString{"84841"}>FixedString{"2"}));
STATIC_ASSERT(1==(FixedString{"69"}<FixedString{"86"}));
STATIC_ASSERT(0==(FixedString{"69"}>FixedString{"86"}));
STATIC_ASSERT(1==(FixedString{"2"}<FixedString{"398"}));
STATIC_ASSERT(0==(FixedString{"2"}>FixedString{"398"}));
STATIC_ASSERT(1==(FixedString{"210"}<FixedString{"259"}));
STATIC_ASSERT(0==(FixedString{"210"}>FixedString{"259"}));
STATIC_ASSERT(1==(FixedString{"39536"}<FixedString{"57849"}));
STATIC_ASSERT(0==(FixedString{"39536"}>FixedString{"57849"}));
STATIC_ASSERT(1==(FixedString{"52"}<FixedString{"937"}));
STATIC_ASSERT(0==(FixedString{"52"}>FixedString{"937"}));
STATIC_ASSERT(1==(FixedString{"070"}<FixedString{"60932"}));
STATIC_ASSERT(0==(FixedString{"070"}>FixedString{"60932"}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"00398"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"00398"}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"60612"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"60612"}));
STATIC_ASSERT(0==(FixedString{"805"}<FixedString{"0"}));
STATIC_ASSERT(1==(FixedString{"805"}>FixedString{"0"}));
STATIC_ASSERT(0==(FixedString{"74006"}<FixedString{"34"}));
STATIC_ASSERT(1==(FixedString{"74006"}>FixedString{"34"}));
STATIC_ASSERT(0==(FixedString{"9"}<FixedString{"2179"}));
STATIC_ASSERT(1==(FixedString{"9"}>FixedString{"2179"}));
STATIC_ASSERT(1==(FixedString{"7485"}<FixedString{"985"}));
STATIC_ASSERT(0==(FixedString{"7485"}>FixedString{"985"}));
STATIC_ASSERT(0==(FixedString{"5621"}<FixedString{"42179"}));
STATIC_ASSERT(1==(FixedString{"5621"}>FixedString{"42179"}));
STATIC_ASSERT(0==(FixedString{"7"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"7"}>FixedString{""}));
STATIC_ASSERT(1==(FixedString{"19697"}<FixedString{"6"}));
STATIC_ASSERT(0==(FixedString{"19697"}>FixedString{"6"}));
STATIC_ASSERT(1==(FixedString{"2316"}<FixedString{"534"}));
STATIC_ASSERT(0==(FixedString{"2316"}>FixedString{"534"}));
STATIC_ASSERT(0==(FixedString{"91546"}<FixedString{"5"}));
STATIC_ASSERT(1==(FixedString{"91546"}>FixedString{"5"}));
STATIC_ASSERT(0==(FixedString{"5723"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"5723"}>FixedString{""}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"90"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"90"}));
STATIC_ASSERT(1==(FixedString{"17115"}<FixedString{"27312"}));
STATIC_ASSERT(0==(FixedString{"17115"}>FixedString{"27312"}));
STATIC_ASSERT(1==(FixedString{"1"}<FixedString{"35171"}));
STATIC_ASSERT(0==(FixedString{"1"}>FixedString{"35171"}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"19"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"19"}));
STATIC_ASSERT(0==(FixedString{"7"}<FixedString{"27"}));
STATIC_ASSERT(1==(FixedString{"7"}>FixedString{"27"}));
STATIC_ASSERT(0==(FixedString{"5"}<FixedString{"0"}));
STATIC_ASSERT(1==(FixedString{"5"}>FixedString{"0"}));
STATIC_ASSERT(0==(FixedString{"82888"}<FixedString{"1818"}));
STATIC_ASSERT(1==(FixedString{"82888"}>FixedString{"1818"}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"68"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"68"}));
STATIC_ASSERT(0==(FixedString{"676"}<FixedString{"4927"}));
STATIC_ASSERT(1==(FixedString{"676"}>FixedString{"4927"}));
STATIC_ASSERT(0==(FixedString{"3"}<FixedString{"02"}));
STATIC_ASSERT(1==(FixedString{"3"}>FixedString{"02"}));
STATIC_ASSERT(0==(FixedString{"8375"}<FixedString{"60"}));
STATIC_ASSERT(1==(FixedString{"8375"}>FixedString{"60"}));
STATIC_ASSERT(0==(FixedString{"981"}<FixedString{"33702"}));
STATIC_ASSERT(1==(FixedString{"981"}>FixedString{"33702"}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"21121"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"21121"}));
STATIC_ASSERT(0==(FixedString{"89091"}<FixedString{"6"}));
STATIC_ASSERT(1==(FixedString{"89091"}>FixedString{"6"}));
STATIC_ASSERT(1==(FixedString{"17520"}<FixedString{"860"}));
STATIC_ASSERT(0==(FixedString{"17520"}>FixedString{"860"}));
STATIC_ASSERT(1==(FixedString{"4789"}<FixedString{"5645"}));
STATIC_ASSERT(0==(FixedString{"4789"}>FixedString{"5645"}));
STATIC_ASSERT(0==(FixedString{"87098"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"87098"}>FixedString{""}));
STATIC_ASSERT(0==(FixedString{"3"}<FixedString{"1"}));
STATIC_ASSERT(1==(FixedString{"3"}>FixedString{"1"}));
STATIC_ASSERT(0==(FixedString{"32"}<FixedString{"1"}));
STATIC_ASSERT(1==(FixedString{"32"}>FixedString{"1"}));
STATIC_ASSERT(0==(FixedString{"81503"}<FixedString{"728"}));
STATIC_ASSERT(1==(FixedString{"81503"}>FixedString{"728"}));
STATIC_ASSERT(0==(FixedString{"787"}<FixedString{"0514"}));
STATIC_ASSERT(1==(FixedString{"787"}>FixedString{"0514"}));
STATIC_ASSERT(1==(FixedString{"6981"}<FixedString{"95"}));
STATIC_ASSERT(0==(FixedString{"6981"}>FixedString{"95"}));
STATIC_ASSERT(1==(FixedString{"8"}<FixedString{"9"}));
STATIC_ASSERT(0==(FixedString{"8"}>FixedString{"9"}));
STATIC_ASSERT(1==(FixedString{"1"}<FixedString{"38"}));
STATIC_ASSERT(0==(FixedString{"1"}>FixedString{"38"}));
STATIC_ASSERT(1==(FixedString{""}<FixedString{"1"}));
STATIC_ASSERT(0==(FixedString{""}>FixedString{"1"}));
STATIC_ASSERT(0==(FixedString{"360"}<FixedString{"1112"}));
STATIC_ASSERT(1==(FixedString{"360"}>FixedString{"1112"}));
STATIC_ASSERT(0==(FixedString{"70"}<FixedString{"5766"}));
STATIC_ASSERT(1==(FixedString{"70"}>FixedString{"5766"}));
STATIC_ASSERT(0==(FixedString{"9"}<FixedString{"20678"}));
STATIC_ASSERT(1==(FixedString{"9"}>FixedString{"20678"}));
STATIC_ASSERT(1==(FixedString{"95348"}<FixedString{"993"}));
STATIC_ASSERT(0==(FixedString{"95348"}>FixedString{"993"}));
STATIC_ASSERT(1==(FixedString{"5221"}<FixedString{"6703"}));
STATIC_ASSERT(0==(FixedString{"5221"}>FixedString{"6703"}));
STATIC_ASSERT(0==(FixedString{"2061"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"2061"}>FixedString{""}));
STATIC_ASSERT(1==(FixedString{"44"}<FixedString{"5"}));
STATIC_ASSERT(0==(FixedString{"44"}>FixedString{"5"}));
STATIC_ASSERT(0==(FixedString{"3043"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"3043"}>FixedString{""}));
STATIC_ASSERT(0==(FixedString{"2"}<FixedString{"11"}));
STATIC_ASSERT(1==(FixedString{"2"}>FixedString{"11"}));
STATIC_ASSERT(0==(FixedString{"78133"}<FixedString{""}));
STATIC_ASSERT(1==(FixedString{"78133"}>FixedString{""}));
STATIC_ASSERT(0==(FixedString{"8886"}<FixedString{"6"}));
STATIC_ASSERT(1==(FixedString{"8886"}>FixedString{"6"}));
STATIC_ASSERT(1==(FixedString{"2"}<FixedString{"7"}));
STATIC_ASSERT(0==(FixedString{"2"}>FixedString{"7"}));