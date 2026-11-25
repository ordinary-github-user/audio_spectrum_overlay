#pragma once
#include <array>
#include "../Assert.h"
#include "../Compact.h"
#include "../Concepts/Char.h"
#include "../Concepts/Constructible.h"
#include "../Concepts/LinearContainer.h"
#include "../Concepts/String.h"
#include "../Constant.h"
#include "../Macro/Consteval.h"
#include "../Macro/MovForward.h"
#include "../StringImpl.h"
#include "../Tags/Uninitialized.h"
#include "../Traits/Like.h"
template<$Char T, usize N>
struct StaticString: StringImpl
{
	using Index = Compact<N>;
//private:
	std::array<T, N> m_data;
	Index m_size;
//public:

	INLINE constexpr StaticString(Uninitialized, Index size)noexcept : m_size{ size } {}

	INLINE constexpr StaticString($Constructible<T> auto&& ...args)noexcept
	requires(sizeof...(args) <= N)
	:m_size{ sizeof...(args) }
	{
		if consteval { CTOR_CONST_AT(&m_data); }
#if __cpp_expansion_statements >= 202506L
		template for (constexpr auto I : iota<sizeof...(args)>)
			CTOR_CONST_AT(&m_data[I], FWD(args...[I]));
#elif __cpp_structured_bindings >= 202411L
		static constexpr auto [...I] { iota<sizeof...(args)> };
		(..., CTOR_CONST_AT(&m_data[constant<I>], FWD(args)));
#else
		[&]<usize...I>(std::index_sequence<I...>)LAMBDA_INLINE
		{
			(..., CTOR_CONST_AT(&m_data[I], FWD(args)));
		}(std::make_index_sequence<sizeof...(args)>{});
#endif
	}

	INLINE CONSTEVAL StaticString(const T(&s)[N + 1])noexcept : StaticString{ __builtin_addressof(s[0]), __builtin_addressof(s[N]) } {}
	INLINE constexpr StaticString(const StaticString& s) noexcept :m_data{ s.m_data }, m_size{ s.m_size }{}
	INLINE constexpr StaticString(T const* pBegin, T const* pEnd) noexcept :m_size{ static_cast<Index>(pEnd - pBegin) }
	{
		if consteval { CTOR_CONST_AT(&m_data); }
		for (T* p{ m_data.data() }; pBegin != pEnd; ++p, ++pBegin)
			*p = *pBegin;
	}
	INLINE constexpr StaticString($LinearContainerOf<T> auto&& s) noexcept :StaticString{ s.data(), s.data() + s.size() } {}

	INLINE constexpr StaticString& operator=(const StaticString& s)noexcept { m_data = s.m_data; m_size = s.m_size; return *this; }

	INLINE constexpr Index size()const noexcept { return m_size; }

	INLINE constexpr void modify_size(Index size)noexcept { m_size = size; }

	template<class Self>INLINE constexpr Like<Self, T> front(this Self&& self) noexcept { return FWD(self).m_data[0]; }
	template<class Self>INLINE constexpr Like<Self, T> back(this Self&& self) noexcept { return FWD(self).m_data[self.m_size - 1]; }
	template<class Self>INLINE constexpr Like<Self, T> operator[](this Self&& self, Index index) noexcept { return FWD(self).m_data[index]; }
	template<class Self>INLINE constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return self.m_data.data(); }
	template<class Self>INLINE constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return self.m_data.data(); }
	template<class Self>INLINE constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + self.m_size; }

	INLINE static CONSTEVAL Index capacity()noexcept { return N; }

	INLINE constexpr void clear()noexcept { m_size = 0; }

	INLINE constexpr void push_back(T entry)noexcept { m_data[m_size++] = entry; }

	INLINE constexpr void append(const T* pBegin, Index count)noexcept
	{
		ASSERT(N - m_size >= count);
		T* pDst{ __builtin_addressof(m_data[m_size]) };
		const T* pEnd{ pBegin + count };
		for (; pBegin != pEnd; ++pBegin, ++pDst)
			*pDst = *pBegin;
		m_size += count;
	}

	INLINE constexpr void append($StringOf<T> auto&& string)noexcept
	{
		ASSERT(string.size() <= N);
		append(string.data(), static_cast<Index>(string.size()));
	}
};
template<$Char T, usize N>
StaticString(const T(&)[N]) -> StaticString<T, N - 1>;

template<$Char T, usize N>
StaticString(FixedString<T, N>) -> StaticString<T, N>;

STATIC_ASSERT([]static
{
	StaticString s{ "" };
	if (s.find('0') != nullptr)return false;
	if (s.rfind('0') != nullptr)return false;
	return true;
}());

STATIC_ASSERT([]static
{
	StaticString s{ "0" };
	if (s.index_of(s.find('0')) != 0)return false;
	if (s.index_of(s.rfind('0')) != 0)return false;
	return true;
}());

STATIC_ASSERT([]static
{
	StaticString s{ "01" };
	if (s.index_of(s.find('0')) != 0)return false;
	if (s.index_of(s.rfind('0')) != 0)return false;
	if (s.index_of(s.find('1')) != 1)return false;
	if (s.index_of(s.rfind('1')) != 1)return false;
	return true;
}());

STATIC_ASSERT([]static
{
	StaticString s{ "" };
	if (s.find<"0">() != nullptr)return false;
	if (s.rfind<"0">() != nullptr)return false;
	return true;
}());

STATIC_ASSERT([]static
{
	StaticString s{ "0" };
	if (s.index_of(s.find<"0">()) != 0)return false;
	if (s.index_of(s.rfind<'0'>()) != 0)return false;
	return true;
}());

STATIC_ASSERT([]static
{
	StaticString s{ "01" };
	if (s.index_of(s.find<"0">()) != 0)return false;
	if (s.index_of(s.rfind<'0'>()) != 0)return false;
	if (s.index_of(s.find<"1">()) != 1)return false;
	if (s.index_of(s.rfind<'1'>()) != 1)return false;
	return true;
}());