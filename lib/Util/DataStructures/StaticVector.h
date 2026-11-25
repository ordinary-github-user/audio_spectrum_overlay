#pragma once
#include <array>
#include <memory>
#include <type_traits>
#include "../BasicTypes.h"
#include "../Compact.h"
#include "../Compiler.h"
#include "../Concepts/Constructible.h"
#include "../Concepts/NotCvref.h"
#include "../Concepts/PassByValue.h"
#include "../Concepts/Printable.h"
#include "../Constant.h"
#include "../ElementAt.h"
#include "../ElementOf.h"
#include "../Inline.h"
#include "../Iota.h"
#include "../Macro/Consteval.h"
#include "../Macro/If.h"
#include "../Macro/MovForward.h"
#include "../Macro/PlacementNew.h"
#include "../ReversePointer.h"
#include "../Tags/Uninitialized.h"
#include "../Traits/Like.h"
#include "../Unreachable.h"
#include "IntegerSequence.h"
template<$NotCvref, usize>struct StaticVector;
template<$NotCvref T, usize N>
requires(N > 0)
struct StaticVector<T, N>
{
	using Index = Compact<N>;
	union { T m_data[N]; };
	Index m_size;
	INLINE constexpr StaticVector($Constructible<T> auto&&...args)noexcept requires(sizeof...(args) <= N) : m_size{ sizeof...(args) }
	{
#if !defined(__cpp_trivial_union)
		if consteval
		{
			if constexpr (!__is_trivially_copyable(T))UNREACHABLE;//constexpr StaticVector<NonTrivial, N> cant be implemented without __cpp_trivial_union
			CTOR_CONST(m_data);
		}
#endif
#if __cpp_expansion_statements >= 202506L
		template for (constexpr auto I : iota<sizeof...(args)>)
			CTOR_CONST(m_data[I], FWD(args...[I]));
#elif __cpp_structured_bindings >= 202411L
		static constexpr auto [...I] { iota<sizeof...(args)> };
		(..., CTOR_CONST(m_data[constant<I>], FWD(args)));
#else
		[&]<usize...I>(std::index_sequence<I...>)LAMBDA_INLINE
		{
			(..., CTOR_CONST(m_data[I], FWD(args)));
		}(std::make_index_sequence<sizeof...(args)>{});
#endif
	}
	INLINE constexpr StaticVector(Uninitialized, Index size)noexcept requires(!requires{T{ uninitialized }; } || !requires{T{ size }; }) : m_size{ size } {}
	INLINE constexpr StaticVector(const StaticVector& rhs)noexcept : m_size{ rhs.size() }
	{
#if !defined(__cpp_trivial_union)
		if consteval
		{
			if constexpr (!__is_trivially_copyable(T))UNREACHABLE;//constexpr StaticVector<NonTrivial, N> cant be implemented without __cpp_trivial_union
			CTOR_CONST(m_data);
		}
#endif
		T* pDst{ __builtin_addressof(m_data[0]) };
		for (const T& e : rhs)
			CTOR_CONST_AT(pDst++, e);
	}
	INLINE constexpr StaticVector(StaticVector&& rhs)noexcept : m_size{ rhs.size() }
	{
#if !defined(__cpp_trivial_union)
		if consteval
		{
			if constexpr (!__is_trivially_copyable(T))UNREACHABLE;//constexpr StaticVector<NonTrivial, N> cant be implemented without __cpp_trivial_union
			CTOR_CONST(m_data);
		}
#endif
		T* pDst{ __builtin_addressof(m_data[0]) };
		for (T& e : rhs)
			CTOR_CONST_AT(pDst++, MOV(e));
		rhs.m_size = {};
	}
	INLINE constexpr StaticVector& operator=(const StaticVector& rhs)noexcept
	{
		if constexpr (!__is_trivially_destructible(T))for (T& entry : *this)entry.~T();
		CTOR_CONST_AT(this, rhs);
		return *this;
	}
	INLINE constexpr StaticVector& operator=(StaticVector&& rhs)noexcept
	{
		if constexpr (!__is_trivially_destructible(T))for (T& entry : *this)entry.~T();
		CTOR_CONST_AT(this, MOV(rhs));
		return *this;
	}
	INLINE constexpr ~StaticVector()noexcept { if constexpr (!__is_trivially_destructible(T))for (T& entry : *this)entry.~T(); }
	INLINE constexpr Index size()const noexcept { return m_size; }
	INLINE constexpr bool empty()const noexcept { return !m_size; }
	static CONSTEVAL Index capacity() noexcept { return N; }
	template<class Self>constexpr Like<Self, T> front(this Self&& self) noexcept { return FWD(self)[{}]; }
	template<class Self>constexpr Like<Self, T> back(this Self&& self) noexcept { return FWD(self)[self.m_size - 1]; }
	template<class Self>constexpr Like<Self, T> operator[](this Self&& self, Index index) noexcept { return FWD(self).m_data[index]; }
	template<class Self>constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return __builtin_addressof(self.m_data[0]); }
	template<class Self>constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return self.data(); }
	template<class Self>constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + self.m_size; }
	template<class Self>constexpr ReversePointer<LikePtr<Self, T>> rbegin(this Self&& self) noexcept { return ReversePointer<LikePtr<Self, T>>{__builtin_addressof(self.m_data[self.m_size])}; }
	template<class Self>constexpr ReversePointer<LikePtr<Self, T>> rend(this Self&& self) noexcept { return ReversePointer<LikePtr<Self, T>>{__builtin_addressof(self.m_data[0])}; }
	INLINE constexpr void clear()noexcept
	{
		if constexpr (!__is_trivially_destructible(T))for (T& entry : *this)entry.~T();
		m_size = {};
	}
	INLINE constexpr void pop_back()noexcept
	{
		--m_size;
		if constexpr (!__is_trivially_destructible(T))
			m_data[m_size].~T();
	}
	INLINE constexpr void push_back(T&& x)noexcept
	requires(!$PassByValue<T>)
	{
		CTOR_CONST(m_data[m_size], MOV(x));
		++m_size;
	}
	INLINE constexpr void push_back(std::conditional_t<$PassByValue<T>, const T, const T&> x)noexcept
	{
		CTOR_CONST(m_data[m_size], x);
		++m_size;
	}
	INLINE constexpr void emplace_back(auto&&...args)noexcept
	{
		CTOR_CONST(m_data[m_size], FWD(args)...);
		++m_size;
	}
	INLINE constexpr void modify_size(Index s)noexcept
	requires(IF(GCC, std::is_trivially_destructible_v<T>, __is_trivially_destructible(T)))
	{
		m_size = s;
	}
	INLINE constexpr void erase(T* p)noexcept
	{
		p->~T();
		--m_size;
		const T* pLast{ end() };
		while (p != pLast)
		{
			CTOR_CONST_AT(p, MOV(p[1]));
			++p;
		}
	}
	INLINE constexpr void erase(Index index)noexcept { erase(__builtin_addressof(data()[index])); }
};
template<$NotCvref T>
struct StaticVector<T, 0>
{
	static CONSTEVAL u8 size() noexcept { return {}; }
	static CONSTEVAL u8 capacity() noexcept { return {}; }
};
template<$NotCvref T, usize N>
constexpr bool operator==(const StaticVector<T, N>& l, const StaticVector<T, N>& r)noexcept
{
	if (l.size() != r.size())return false;
	const T* p{ l.data() };
	for (const T& e : r)
		if (*(p++) != e)
			return false;
	return true;
}
template<$NotCvref T, usize N>
constexpr bool operator!=(const StaticVector<T, N>& l, const StaticVector<T, N>& r)noexcept
{
	return !(l == r);
}
template<$NotCvref Stream, $Printable<Stream> T, usize N>
constexpr Stream& operator<<(Stream& os, const StaticVector<T, N>& array)noexcept
{
	os << '[';
	bool isFirst{ true };
	for (const T& e : array)
	{
		if (isFirst)
			isFirst = false;
		else
			os << ',';
		os << e;
	}
	os << ']';
	return os;
}
namespace details::StaticVectorTests
{
	struct TestStruct
	{
		std::unique_ptr<i32> i;
		constexpr TestStruct(i32 i)noexcept :i{ new i32{i} } {}
		constexpr TestStruct(const TestStruct& x)noexcept : i{ new i32{*x.i} } {}
		constexpr TestStruct(TestStruct&& x)noexcept : i{ MOV(x.i) } {}
		friend constexpr bool operator==(const TestStruct& l, i32 r) noexcept { return *l.i == r; }
		friend constexpr bool operator==(i32 l, const TestStruct& r) noexcept { return *r.i == l; }
		friend constexpr bool operator==(const TestStruct& l, const TestStruct& r) noexcept { return *l.i == *r.i; }
	};
	template<class Container>
	CONSTEVAL bool test()noexcept
	{
		using Index = Container::Index;
		using Element = ElementOf<Container>;
		static constexpr auto verify{ [](const Container& vector, auto...e)static
		{
			return[&]<usize...I, usize...J>(std::index_sequence<I...>, std::index_sequence<J...>)
			{
				if (vector.size() != sizeof...(e))return false;
				if constexpr (sizeof...(e))
				{
					if (element_at<0>(e...) != vector.front())return false;
					if (element_at<sizeof...(e) - 1>(e...) != vector.back())return false;
					if (!((vector[I] == e) && ...))return false;
				}
				{
					auto iter{ vector.begin() };
					const auto iterEnd{ vector.end() };
					if (!(((iter != iterEnd && *iter == e) && (++iter, true)) && ...))return false;
					if (iter != iterEnd)return false;
				}
				{
					auto iter{ vector.rbegin() };
					const auto iterEnd{ vector.rend() };
					std::array<Element, sizeof...(e)> expected{e...};
					auto iterExpected{ expected.rbegin() };
					for (; iter != iterEnd; ++iter, ++iterExpected)
						if (*iter != *iterExpected)
							return false;
				}
				if (Container{ e... } != vector)return false;
				return true;
			}(std::make_index_sequence<sizeof...(e)>{}, inverse(std::make_index_sequence<sizeof...(e)>{}));
		} };
		Container vec0;					if (!verify(vec0))return false;
		vec0.emplace_back(1);			if (!verify(vec0, 1))return false;
		vec0.emplace_back(2);			if (!verify(vec0, 1, 2))return false;
		vec0.emplace_back(3);			if (!verify(vec0, 1, 2, 3))return false;
		vec0.emplace_back(4);			if (!verify(vec0, 1, 2, 3, 4))return false;
		vec0.emplace_back(5);			if (!verify(vec0, 1, 2, 3, 4, 5))return false;
		vec0.emplace_back(6);			if (!verify(vec0, 1, 2, 3, 4, 5, 6))return false;
		vec0.emplace_back(7);			if (!verify(vec0, 1, 2, 3, 4, 5, 6, 7))return false;
		vec0.erase(Index{0});			if (!verify(vec0, 2, 3, 4, 5, 6, 7))return false;
		vec0.erase(Index{2});			if (!verify(vec0, 2, 3, 5, 6, 7))return false;
		vec0.clear();					if (!verify(vec0))return false;
		vec0.emplace_back(8);			if (!verify(vec0, 8))return false;
		Container vec1{ MOV(vec0) };	if (!verify(vec1, 8))return false;
		Container vec2{ vec1 };			if (!verify(vec1, 8) || !verify(vec2, 8))return false;
		return true;
	};
	STATIC_ASSERT(test<StaticVector<i32, 7>>());
#if defined(__cpp_trivial_union)
	STATIC_ASSERT(test<StaticVector<TestStruct, 7>>());
#endif
}