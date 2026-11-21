#pragma once
#include "Allocator.h"
#include "Assert.h"
#include "BasicTypes.h"
#include "Compiler.h"
#include "ConceptCvrefOf.h"
#include "ConceptIntegral.h"
#include "ConceptLinearContainer.h"
#include "ConceptNotCvref.h"
#include "ConceptSig.h"
#include "ElementAt.h"
#include "ElementOf.h"
#include "IndexTypeOf.h"
#include "Inline.h"
#include "IntegerSequence.h"
#include "Like.h"
#include "MacroIf.h"
#include "MacroMovForward.h"
#include "ReversePointer.h"
#include "TagUninitialized.h"
#include "Traits.h"
#include <algorithm>
template<$NotCvref T, $UInt S = usize>
struct DynamicArray
{
//private:
	T* m_ptr;
	S m_size;
	INLINE constexpr void destruct()noexcept
	{
		if (m_ptr)
		{
			if constexpr (!__is_trivially_destructible(T))
				for (T& x : *this)
					x.~T();
			::deallocate(m_ptr, m_size);
		}
	}
//public:
	INLINE IF(CLANG, CONSTEVAL, constexpr) DynamicArray()noexcept :m_ptr{}, m_size{} {}
	INLINE constexpr DynamicArray(Uninitialized, S s)noexcept : m_ptr{ ::allocate<T>(s) }, m_size{ s } {}
	INLINE constexpr DynamicArray($CvrefOf<T> auto&&...args)noexcept : DynamicArray{ uninitialized, S{sizeof...(args)} } 
	{
		[&] <usize...I>(std::index_sequence<I...>) 
		{
			(..., CTOR_CONST_AT(m_ptr + I, FWD(args)));
		}(std::make_index_sequence<sizeof...(args)>{});
	}
	INLINE constexpr DynamicArray(S s, auto&&...args)noexcept requires requires{T{ FWD(args)... }; } :DynamicArray{ uninitialized, s }
	{
		for (T& x : *this)
			CTOR_CONST_AT(__builtin_addressof(x), FWD(args)...);
	}
	INLINE constexpr DynamicArray(const T* pBegin, const T* pEnd)noexcept : DynamicArray{ uninitialized, static_cast<S>(pEnd - pBegin) }
	{
		for(T* p{ m_ptr }; pBegin != pEnd; ++p, ++pBegin)
			CTOR_CONST_AT(p, *pBegin);
	}

	INLINE constexpr explicit DynamicArray($LinearContainerOf<T> auto&& container)noexcept : DynamicArray{ container.begin(), container.end() }{}
	INLINE constexpr DynamicArray(DynamicArray&& rhs)noexcept :m_ptr{ rhs.m_ptr }, m_size{ rhs.m_size } { rhs.m_ptr = {}; }
	INLINE constexpr DynamicArray(const DynamicArray& rhs)noexcept : DynamicArray{ rhs.begin(), rhs.end() } {}
	INLINE constexpr DynamicArray& operator=(DynamicArray&& rhs)noexcept { this->~DynamicArray(); CTOR_CONST_AT(this, MOV(rhs)); return *this; }
	INLINE constexpr DynamicArray& operator=(const DynamicArray& rhs)noexcept = delete;
	INLINE constexpr ~DynamicArray()noexcept { destruct(); }
	INLINE constexpr S size()const noexcept { return m_size; }

	INLINE constexpr bool empty()const noexcept { return !m_size; }

	//there must be exactly s elements in m_ptr constructed already
	//no way to test this with assert tho
	INLINE constexpr void modify_size(S s)noexcept
	requires(TRIVIALLY_DESTRUCTIBLE(T))
	{
		ASSERT(s <= m_size);
		if consteval
		{
			T* newPtr{ ::allocate<T>(s) };
			T* pDest{ newPtr };
			for (T* pSrc{ m_ptr }, * pEnd{ m_ptr + s }; pSrc != pEnd; ++pSrc, ++pDest)
				CTOR_CONST_AT(pDest, MOV(*pSrc));
			::deallocate(m_ptr, m_size);
			m_ptr = newPtr;
			m_size = s;
		}
		else
		{
			m_size = s;
		}
	}

	constexpr void truncate(S s)noexcept
	{
		ASSERT(s <= m_size);
		ASSERT(m_ptr);
		if consteval
		{
			T* newPtr{ ::allocate<T>(s) };
			T* pDest{ newPtr };
			for (T* pSrc{ m_ptr }, *pEnd{ m_ptr + s}; pSrc != pEnd; ++pSrc, ++pDest)
				CTOR_CONST_AT(pDest, MOV(*pSrc));
			::deallocate(m_ptr, m_size);
			m_ptr = newPtr;
			m_size = s;
		}
		else
		{
			if constexpr (!__is_trivially_destructible(T))
				for (T* p{ m_ptr + s }, * pEnd{ m_ptr + m_size }; p != pEnd; ++p)
					p->~T();
			m_size = s;
		}
	}

	//destroy old elements, new elements not initialized
	INLINE constexpr void resize_uninitialized(S s)noexcept
	{
		destruct();
		CTOR_CONST_AT(this, uninitialized, s);
	}

	INLINE constexpr void resize(S s, auto&&...args)noexcept requires requires{T{ FWD(args)... }; }
	{
		destruct();
		CTOR_CONST_AT(this, s, FWD(args)...);
	}

	INLINE constexpr void clear()noexcept
	{
		destruct();
		CTOR_CONST_AT(this);
	}

	constexpr void erase(T* p)noexcept
	{
		ASSERT(m_ptr);
		if constexpr (!__is_trivially_destructible(T))
			p->~T();
		if consteval
		{
			const S newSize{ m_size - 1 };
			T* newPtr{ ::allocate<T>(newSize) };
			T* pDest{ newPtr };
			for (T* pSrc{ m_ptr }; pSrc != p; ++pSrc, ++pDest)
				CTOR_CONST_AT(pDest, MOV(*pSrc));
			for (T* pSrc{ p + 1 }, *pEnd{ m_ptr + m_size }; pSrc != pEnd; ++pSrc, ++pDest)
				CTOR_CONST_AT(pDest, MOV(*pSrc));
			::deallocate(m_ptr, m_size);
			m_ptr = newPtr;
			m_size = newSize;
		}
		else
		{
			--m_size;
			const T* pLast{ end() };
			while (p != pLast)
			{
				CTOR_CONST_AT(p, MOV(p[1]));
				++p;
			}
		}
	}

	INLINE constexpr void erase(S index)noexcept { erase(m_ptr + index); }

	INLINE constexpr S index_of(const T& e)const noexcept { return static_cast<S>(__builtin_addressof(e)-data()); }
	INLINE constexpr S index_of(const T* p)const noexcept { return static_cast<S>(p - data()); }

	template<class Self>INLINE constexpr Like<Self, T> front(this Self&& self) noexcept { return FWD(self).m_ptr[0]; }
	template<class Self>INLINE constexpr Like<Self, T> back(this Self&& self) noexcept { return FWD(self).m_ptr[self.m_size - 1]; }
	template<class Self>INLINE constexpr Like<Self, T> operator[](this Self&& self, S index) noexcept { return FWD(self).m_ptr[index]; }
	template<class Self>INLINE constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return self.m_ptr; }
	template<class Self>INLINE constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return self.m_ptr; }
	template<class Self>INLINE constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + self.m_size; }
	template<class Self>INLINE constexpr ReversePointer<LikePtr<Self, T>> rbegin(this Self&& self) noexcept { return ReversePointer<LikePtr<Self, T>>{ self.end() }; }
	template<class Self>INLINE constexpr ReversePointer<LikePtr<Self, T>> rend(this Self&& self) noexcept { return ReversePointer<LikePtr<Self, T>>{ self.begin() }; }
};

template<$NotCvref T, $UInt S = usize>
INLINE constexpr bool operator==(const DynamicArray<T, S>& lhs, const DynamicArray<T, S>& rhs)noexcept
{
	if (lhs.size() != rhs.size())return false;
	const T* pL{ lhs.data() };
	const T* pR{ rhs.data() };
	const T* pEndL{ lhs.end() };
	for (; pL != pEndL; ++pL, ++pR)
		if (*pL != *pR)
			return false;
	return true;
}

template<class Stream, $NotCvref T, $UInt S>
constexpr Stream& operator<<(Stream& os, const DynamicArray<T, S>& arr)noexcept
{
	os << '[';
	for (const T& v : arr)
	{
		if (&v != &arr.front())
			os << ',';
		os << v;
	}
	os << ']';
	return os;
}

STATIC_ASSERT([]static
{
	using T = int;
	static constexpr auto verify{ [] (auto&& arr, std::initializer_list<T> expected)static
	{
		if (arr.size() != expected.size())return false;
		auto it{arr.begin()};
		auto itEnd{arr.end()};
		for (auto p{expected.begin()}; it != itEnd; ++it, ++p)
			if (*p != *it)
				return false;
		return true;
	} };
	{
		DynamicArray<T> arr{ usize{2} }; verify(arr, { 0,0 });
		arr[0] = 0; verify(arr, { 0,0 });
		arr[1] = 1; verify(arr, { 0,1 });
		arr.clear(); verify(arr, {});
		arr.resize(3); verify(arr, { 0,0,0 });
		arr[0] = 0; verify(arr, { 0,0,0 });
		arr[1] = 1; verify(arr, { 0,1,0 });
		arr[2] = 2; verify(arr, { 0,1,2 });
		DynamicArray<T> moved{ MOV(arr) }; verify(moved, { 0,1,2 });
		moved.erase(1); verify(moved, { 0,2 });
	}
	return true;
}());

namespace details::DynamicArrayTests
{
	struct TestStruct
	{
		std::unique_ptr<i32> i;
		IF(MSVC, constexpr, CONSTEVAL) TestStruct(i32 i = 0)noexcept :i{ new i32{i} } {}
		friend IF(MSVC, constexpr, CONSTEVAL) bool operator==(const TestStruct& l, i32 r) noexcept { return *l.i == r; }
		friend IF(MSVC, constexpr, CONSTEVAL) bool operator==(i32 l, const TestStruct& r) noexcept { return *r.i == l; }
		friend IF(MSVC, constexpr, CONSTEVAL) bool operator==(const TestStruct& l, const TestStruct& r) noexcept { return *l.i == *r.i; }
	};
	template<class Container>
	CONSTEVAL bool test()noexcept
	{
		using Index = IndexOf<Container>;
		using Element = ElementOf<Container>;
		static constexpr auto verify{ [] (const Container& vector, auto...e)static
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
				return true;
			}(std::make_index_sequence<sizeof...(e)>{}, inverse(std::make_index_sequence<sizeof...(e)>{}));
		} };
		Container vec0{ Index{7} };		if (!verify(vec0, 0, 0, 0, 0, 0, 0, 0))return false;
		vec0[1] = 1;					if (!verify(vec0, 0, 1, 0, 0, 0, 0, 0))return false;
		vec0[2] = 2;					if (!verify(vec0, 0, 1, 2, 0, 0, 0, 0))return false;
		vec0[3] = 3;					if (!verify(vec0, 0, 1, 2, 3, 0, 0, 0))return false;
		vec0[4] = 4;					if (!verify(vec0, 0, 1, 2, 3, 4, 0, 0))return false;
		vec0[5] = 5;					if (!verify(vec0, 0, 1, 2, 3, 4, 5, 0))return false;
		vec0[6] = 6;					if (!verify(vec0, 0, 1, 2, 3, 4, 5, 6))return false;
		vec0.erase(Index{ 0 });			if (!verify(vec0, 1, 2, 3, 4, 5, 6))return false;
		vec0.erase(Index{ 2 });			if (!verify(vec0, 1, 2, 4, 5, 6))return false;
		vec0.erase(Index{ 4 });			if (!verify(vec0, 1, 2, 4, 5))return false;
		vec0.erase(Index{ 0 });			if (!verify(vec0, 2, 4, 5))return false;
		vec0.erase(Index{ 0 });			if (!verify(vec0, 4, 5))return false;
		vec0.erase(Index{ 0 });			if (!verify(vec0, 5))return false;
		vec0.erase(Index{ 0 });			if (!verify(vec0))return false;
		vec0.resize(3, 3);				if (!verify(vec0, 3, 3, 3))return false;
		Container vec1{ MOV(vec0) };	if (!verify(vec1, 3, 3, 3))return false;
		vec1.clear();					if (!verify(vec1))return false;
		return true;
	};
	STATIC_ASSERT(test<DynamicArray<i32>>());
	STATIC_ASSERT(test<DynamicArray<TestStruct>>());
}
