#pragma once
#include "AlgorithmCopy.h"
#include "AlgorithmMinMax.h"
#include "Allocator.h"
#include "BasicTypes.h"
#include "Compiler.h"
#include "ConceptConstructible.h"
#include "ConceptCvrefOf.h"
#include "ConceptLinearContainer.h"
#include "ConceptNotCvref.h"
#include "ConceptPassByValue.h"
#include "ConceptPrintable.h"
#include "Like.h"
#include "MacroIf.h"
#include "MacroMovForward.h"
#include "MacroPlacementNew.h"
#include "TagUninitialized.h"
struct FromElements {};
constexpr FromElements from_elements[[maybe_unused]];
template<$NotCvref T, $UInt S = usize>
struct GrowableArray
{
private:
	using Index = S;
	T* m_ptr;
	Index m_size;
	Index m_cap;

	constexpr void destruct()noexcept
	{
		if (m_ptr)
		{
			if constexpr (!__is_trivially_destructible(T))
				for (T& e : *this)
					e.~T();
			deallocate(m_ptr, m_cap);
		}
	}

	constexpr Index growCap(Index newSize)noexcept
	{
		return Algorithm::max(m_cap * 2, newSize);
	}
	template<usize...I>
	constexpr GrowableArray(std::index_sequence<I...>, $CvrefOf<T> auto&&...args)noexcept : GrowableArray{ uninitialized, sizeof...(args) }
	{
		(..., CTOR_CONST_AT(m_ptr + I, FWD(args)));
	}
public:
	constexpr GrowableArray(Uninitialized, Index s)noexcept :m_ptr{ ::allocate<T>(s) }, m_size{ s }, m_cap{ s } {}

	constexpr GrowableArray(FromElements, $CvrefOf<T> auto&&...args)noexcept requires(0 != sizeof...(args)) : GrowableArray{ std::make_index_sequence<sizeof...(args)>{}, FWD(args)... } {}

	constexpr GrowableArray(Index s, auto&&...args)noexcept : GrowableArray{ uninitialized, s }
	{
		for (T& e : *this)
			CTOR_CONST_AT(__builtin_addressof(e), FWD(args)...);
	}

	IF(CLANG, CONSTEVAL, constexpr) GrowableArray()noexcept :m_ptr{}, m_size{}, m_cap{} {}//msvc bug, gcc bug

	constexpr GrowableArray(GrowableArray&& r)noexcept :m_ptr{ r.m_ptr }, m_size{ r.m_size }, m_cap{ r.m_cap } { r.m_ptr = {}; }

	constexpr GrowableArray(const GrowableArray& r)noexcept :GrowableArray{ r.data(), r.size() } {}

	//clang bug: if T is recursive, something very wrong happens here
	//constexpr GrowableArray($LinearContainer auto&& container)noexcept : GrowableArray{ container.data(), container.size() } {}

	constexpr GrowableArray($Constructible<T> auto const* pData, Index s)noexcept :GrowableArray{ uninitialized, s }
	{
		for (T& e : *this)
		{
			CTOR_CONST_AT(__builtin_addressof(e), *pData);
			++pData;
		}
	}

	template<$Constructible<T> U>
	constexpr GrowableArray(const U* pBegin, const U* pEnd)noexcept :GrowableArray{ pBegin, static_cast<Index>(pEnd - pBegin) } {}

	constexpr GrowableArray& operator=(GrowableArray&& r)noexcept
	{
		destruct();
		CTOR_CONST_AT(this, MOV(r));
		return *this;
	}

	constexpr GrowableArray& operator=(const GrowableArray& r)noexcept
	{
		destruct();
		CTOR_CONST_AT(this, r);
		return *this;
	}

	INLINE constexpr ~GrowableArray()noexcept { destruct(); }

	INLINE constexpr Index size()const noexcept { return m_size; }
	INLINE constexpr Index capacity()const noexcept { return m_cap; }
	INLINE constexpr bool empty()const noexcept { return !m_size; }

	template<class Self>INLINE constexpr Like<Self, T> front(this Self&& self) noexcept { return FWD(self).m_ptr[0]; }
	template<class Self>INLINE constexpr Like<Self, T> back(this Self&& self) noexcept { return FWD(self).m_ptr[self.m_size - 1]; }
	template<class Self>INLINE constexpr Like<Self, T> operator[](this Self&& self, Index index) noexcept { return FWD(self).m_ptr[index]; }
	template<class Self>INLINE constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return self.m_ptr; }
	template<class Self>INLINE constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return self.m_ptr; }
	template<class Self>INLINE constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + self.m_size; }

	INLINE constexpr Index index_of(const T& e)const noexcept { return static_cast<Index>(__builtin_addressof(e) - data()); }
	INLINE constexpr Index index_of(const T* p)const noexcept { return static_cast<Index>(p - data()); }

	INLINE constexpr void reserve(Index s)noexcept
	{
		if (s > m_cap)
			reserve_impl(s);
	}

	constexpr void resize(Index s, auto&&...args)noexcept
	{
		if (s <= m_size)
		{
			if constexpr (!__is_trivially_destructible(T))
				for (T* p{ m_ptr + s }, * pEnd{ m_ptr + m_size }; p != pEnd; ++p)
					p->~T();
		}
		else
		{
			const Index oldSize{ m_size };
			reserve_impl(growCap(s));
			for (T* p{ m_ptr + oldSize }, * pEnd{ m_ptr + s }; p != pEnd; ++p)
				CTOR_CONST_AT(p, FWD(args)...);
		}
		m_size = s;
	}

	//s < m_size
	constexpr void truncate(Index s)noexcept
	{
		if constexpr (!__is_trivially_destructible(T))
			for (T* p{ m_ptr + s }, * pEnd{ m_ptr + m_size }; p != pEnd; ++p)
				p->~T();
		m_size = s;
	}

	constexpr void clear()noexcept 
	{
		if constexpr (!__is_trivially_destructible(T))
			for (T& e : *this)
				e.~T(); 
		m_size = {}; 
	}

	constexpr void reserve_impl(Index x)noexcept
	{
		T* newPtr{ allocate<T>(x) };
		for (T* p{ newPtr }, * pEnd{ newPtr + m_size }, * pSrc{ m_ptr }; p != pEnd; ++p, ++pSrc)
			CTOR_CONST_AT(p, MOV(*pSrc));
		if (m_ptr)
			deallocate(m_ptr, m_cap);
		m_ptr = newPtr;
		m_cap = x;
	}
	//std::conditional_t<$PassByValue<T>, const T, const T&> result in immediate instantiation of T, if T is incomplete due to recursion (json), code wont compile
	constexpr void unchecked_push_back(const T c)noexcept
	requires($PassByValue<T>)
	{
		CTOR_CONST_AT(m_ptr + m_size, c);
		++m_size;
	}
	constexpr void unchecked_push_back(const T& c)noexcept
	requires($PassByRef<T>)
	{
		CTOR_CONST_AT(m_ptr + m_size, c);
		++m_size;
	}
	constexpr void unchecked_push_back(T&& c)noexcept
	requires($PassByRef<T>)
	{
		CTOR_CONST_AT(m_ptr + m_size, MOV(c));
		++m_size;
	}
	constexpr void push_back(const T c)noexcept
	requires($PassByValue<T>)
	{
		const Index newSize{ m_size + 1 };
		if (newSize > m_cap)
			reserve_impl(growCap(newSize));
		CTOR_CONST_AT(m_ptr + m_size, c);
		m_size = newSize;
	}
	constexpr void push_back(const T& c)noexcept
	requires($PassByRef<T>)
	{
		const Index newSize{ m_size + 1 };
		if (newSize > m_cap)
			reserve_impl(growCap(newSize));
		CTOR_CONST_AT(m_ptr + m_size, c);
		m_size = newSize;
	}
	constexpr void push_back(T&& c)noexcept
	requires($PassByRef<T>)
	{
		const Index newSize{ m_size + 1 };
		if (newSize > m_cap)
			reserve_impl(growCap(newSize));
		CTOR_CONST_AT(m_ptr + m_size, MOV(c));
		m_size = newSize;
	}
	constexpr void unchecked_emplace_back($PassByValue auto...args)noexcept
	{
		if constexpr (requires{CTOR_CONST_AT(m_ptr + m_size, args...); })//todo: bug
			CTOR_CONST_AT(m_ptr + m_size, args...);
		else
			CTOR_AT(m_ptr + m_size){ args... };
		++m_size;
	}
	constexpr void unchecked_emplace_back(auto&&...args)noexcept
	requires(!($PassByValue<decltype(args)>&&...))
	{
		if constexpr (requires{CTOR_CONST_AT(m_ptr + m_size, FWD(args)...); })
			CTOR_CONST_AT(m_ptr + m_size, FWD(args)...);
		else
			CTOR_AT(m_ptr + m_size){ FWD(args)... };
		++m_size;
	}
	constexpr void emplace_back($PassByValue auto...args)noexcept
	{
		const Index newSize{ m_size + 1 };
		if (newSize > m_cap)
			reserve_impl(growCap(newSize));
		if constexpr (requires{CTOR_CONST_AT(m_ptr + m_size, args...); })//todo: bug
			CTOR_CONST_AT(m_ptr + m_size, args...);
		else
			CTOR_AT(m_ptr + m_size){ args... };
		m_size = newSize;
	}
	constexpr void emplace_back(auto&&...args)noexcept
	requires(!($PassByValue<decltype(args)>&&...))
	{
		const Index newSize{ m_size + 1 };
		if (newSize > m_cap)
			reserve_impl(growCap(newSize));
		if constexpr (requires{CTOR_CONST_AT(m_ptr + m_size, FWD(args)...); })
			CTOR_CONST_AT(m_ptr + m_size, FWD(args)...);
		else
			CTOR_AT(m_ptr + m_size){ FWD(args)... };
		m_size = newSize;
	}

	constexpr void unchecked_insert(Index index, auto&&...args)noexcept
	{
		T* ptr{ data() + index };
		for (T* pEntry{ end() }; pEntry != ptr; --pEntry)
			CTOR_CONST_AT(pEntry, MOV(*(pEntry - 1)));
		CTOR_CONST_AT(ptr, FWD(args)...);
		++m_size;
	}

	constexpr void unchecked_insert(const T* ptr, auto&&...args)noexcept
	{
		return unchecked_insert(index_of(ptr), FWD(args)...);
	}

	constexpr void insert(Index index, auto&&...args)noexcept
	{
		const Index newSize{ m_size + 1 };
		if (newSize > m_cap)
		{
			const Index x{ growCap(newSize) };
			T* newPtr{ allocate<T>(x) };
			for (Index i{}; i != index; ++i)
				CTOR_CONST_AT(newPtr + i, MOV(m_ptr[i]));
			for (Index i{ index }; i != m_size; ++i)
				CTOR_CONST_AT(newPtr + i + 1, MOV(m_ptr[i]));
			CTOR_CONST_AT(newPtr + index, FWD(args)...);
			if (m_ptr)
				deallocate(m_ptr, m_cap);
			m_ptr = newPtr;
			m_cap = x;
		}
		else
		{
			T* ptr{ data() + index };
			for (T* pEntry{ end() }; pEntry != ptr; --pEntry)
				CTOR_CONST_AT(pEntry, MOV(*(pEntry - 1)));
			CTOR_CONST_AT(ptr, FWD(args)...);
		}
		m_size = newSize;
	}

	constexpr void insert(const T* ptr, auto&&...args)noexcept
	{
		return insert(index_of(ptr), FWD(args)...);
	}

	constexpr void erase(T* p)noexcept
	{
		if constexpr (!__is_trivially_destructible(T))
			p->~T();
		--m_size;
		const T* pLast{ end() };
		while (p != pLast)
		{
			CTOR_CONST_AT(p, MOV(p[1]));
			++p;
		}
	}

	constexpr void erase(Index index)noexcept { erase(__builtin_addressof(data()[index])); }

	constexpr void append(const T* pData, Index s)noexcept
	{
		const Index newSize{ m_size + s };
		if (newSize > m_cap)
			reserve_impl(Algorithm::max(m_cap * 2, newSize));

		const T* pEnd{ m_ptr + newSize };
		for (T* p{ m_ptr + m_size }; p != pEnd; ++p)
		{
			CTOR_CONST_AT(p, *pData);
			++pData;
		}
		m_size = newSize;
	}

	constexpr void pop_front()noexcept
	{
		if constexpr (!__is_trivially_destructible(T))
			front().~T();
		--m_size;
		for (T* p{ m_ptr }, *pEnd{ end() }; p != pEnd; ++p)
			CTOR_CONST_AT(p, MOV(p[1]));
	}

	constexpr void pop_back()noexcept
	{
		if constexpr (!__is_trivially_destructible(T))
			back().~T();
		--m_size;
	}
};

template<class T>
constexpr bool operator==(const GrowableArray<T>& l, const GrowableArray<T>& r)noexcept
{
	if (l.size() != r.size())return false;
	const T* pL{ l.data() };
	const T* pR{ r.data() };
	const T* pEndL{ l.end() };
	for (; pL != pEndL; ++pL, ++pR)
		if (*pL != *pR)
			return false;
	return true;
}

template<class T>
constexpr bool operator!=(const GrowableArray<T>& l, const GrowableArray<T>& r)noexcept
{
	return !operator==(l, r);
}

template<class Stream, $Printable<Stream> T>
constexpr Stream& operator<<(Stream& os, const GrowableArray<T>& array)noexcept
{
	os << '[';
	for (const T& v : array)
	{
		if (__builtin_addressof(v) != __builtin_addressof(array.front()))
			os << ',';
		os << v;
	}
	os << ']';
	return os;
}

//tests-----------------------------------------------------------------------------------------------------------------------------------------
//ctor
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	GrowableArray<Element> a{ 3, uninitialized, usize{10} };
	if (a.size() != 3)return false;
	for (Element& e : a)
		if (e.size() != 10)
			return false;
	return true;
}());

//pop_back
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	GrowableArray<Element> a{ uninitialized, 3 };
	if (a.size() != 3)return false;
	CTOR_CONST_AT(&a[0], uninitialized, 0);
	CTOR_CONST_AT(&a[1], uninitialized, 1);
	CTOR_CONST_AT(&a[2], uninitialized, 2);
	a.pop_back();
	if (a.size() != 2)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	a.pop_back();
	if (a.size() != 1)return false;
	if (a[0].size() != 0)return false;
	a.pop_back();
	if (a.size() != 0)return false;
	return true;
}());
//erase back
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	GrowableArray<Element> a{ uninitialized, 3 };
	if (a.size() != 3)return false;
	CTOR_CONST_AT(&a[0], uninitialized, 0);
	CTOR_CONST_AT(&a[1], uninitialized, 1);
	CTOR_CONST_AT(&a[2], uninitialized, 2);
	a.erase(a.size() - 1);
	if (a.size() != 2)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	a.erase(a.size() - 1);
	if (a.size() != 1)return false;
	if (a[0].size() != 0)return false;
	a.erase(a.size() - 1);
	if (a.size() != 0)return false;
	return true;
}());
//pop_front
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	GrowableArray<Element> a{ uninitialized, 3 };
	if (a.size() != 3)return false;
	CTOR_CONST_AT(&a[0], uninitialized, 0);
	CTOR_CONST_AT(&a[1], uninitialized, 1);
	CTOR_CONST_AT(&a[2], uninitialized, 2);
	a.pop_front();
	if (a.size() != 2)return false;
	if (a[0].size() != 1)return false;
	if (a[1].size() != 2)return false;
	a.pop_front();
	if (a.size() != 1)return false;
	if (a[0].size() != 2)return false;
	a.pop_front();
	if (a.size() != 0)return false;
	return true;
}());
//erase front
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	GrowableArray<Element> a{ uninitialized, 3 };
	if (a.size() != 3)return false;
	CTOR_CONST_AT(&a[0], uninitialized, 0);
	CTOR_CONST_AT(&a[1], uninitialized, 1);
	CTOR_CONST_AT(&a[2], uninitialized, 2);
	a.erase(usize{ 0 });
	if (a.size() != 2)return false;
	if (a[0].size() != 1)return false;
	if (a[1].size() != 2)return false;
	a.erase(usize{ 0 });
	if (a.size() != 1)return false;
	if (a[0].size() != 2)return false;
	a.erase(usize{ 0 });
	if (a.size() != 0)return false;
	return true;
}());
//push_back(T&&) && push_back(const T&)
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	GrowableArray<Element> a;
	if (a.size() != 0)return false;
	a.push_back({ 0 });
	if (a.size() != 1)return false;
	if (a[0].size() != 0)return false;
	a.push_back({ 1 });
	if (a.size() != 2)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	a.push_back({ 2 });
	if (a.size() != 3)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	if (a[2].size() != 2)return false;
	GrowableArray<Element> b;
	b.push_back(a[0]);
	if (b.size() != 1)return false;
	if (b[0].size() != 0)return false;
	b.push_back(a[1]);
	if (b.size() != 2)return false;
	if (b[0].size() != 0)return false;
	if (b[1].size() != 1)return false;
	b.push_back(a[2]);
	if (b.size() != 3)return false;
	if (b[0].size() != 0)return false;
	if (b[1].size() != 1)return false;
	if (b[2].size() != 2)return false;
	return true;
}());
//emplace_back(T&&) emplace_back(const T&)
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	GrowableArray<Element> a;
	if (a.size() != 0)return false;
	a.emplace_back(usize{ 0 });
	if (a.size() != 1)return false;
	if (a[0].size() != 0)return false;
	a.emplace_back(usize{ 1 });
	if (a.size() != 2)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	a.emplace_back(usize{ 2 });
	if (a.size() != 3)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	if (a[2].size() != 2)return false;
	GrowableArray<Element> b;
	b.emplace_back(a[0]);
	if (b.size() != 1)return false;
	if (b[0].size() != 0)return false;
	b.emplace_back(a[1]);
	if (b.size() != 2)return false;
	if (b[0].size() != 0)return false;
	if (b[1].size() != 1)return false;
	b.emplace_back(a[2]);
	if (b.size() != 3)return false;
	if (b[0].size() != 0)return false;
	if (b[1].size() != 1)return false;
	if (b[2].size() != 2)return false;
	return true;
}());

//insert front
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	using Index = usize;
	GrowableArray<Element> a;
	if (a.size() != 0)return false;
	a.insert(Index{ 0 }, uninitialized, Index{ 0 });
	if (a.size() != 1)return false;
	if (a[0].size() != 0)return false;
	a.insert(Index{ 0 }, uninitialized, Index{ 1 });
	if (a.size() != 2)return false;
	if (a[0].size() != 1)return false;
	if (a[1].size() != 0)return false;
	a.insert(Index{ 0 }, uninitialized, Index{ 2 });
	if (a.size() != 3)return false;
	if (a[0].size() != 2)return false;
	if (a[1].size() != 1)return false;
	if (a[2].size() != 0)return false;
	return true;
}());

//insert back middle
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	using Index = usize;
	GrowableArray<Element> a;
	if (a.size() != 0)return false;
	a.insert(Index{ 0 }, uninitialized, Index{ 0 });
	if (a.size() != 1)return false;
	if (a[0].size() != 0)return false;
	a.insert(Index{ 1 }, uninitialized, Index{ 1 });
	if (a.size() != 2)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	a.insert(Index{ 2 }, uninitialized, Index{ 2 });
	if (a.size() != 3)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	if (a[2].size() != 2)return false;
	a.insert(Index{ 2 }, uninitialized, Index{ 3 });
	if (a.size() != 4)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	if (a[2].size() != 3)return false;
	if (a[3].size() != 2)return false;
	a.insert(Index{ 2 }, uninitialized, Index{ 4 });
	if (a.size() != 5)return false;
	if (a[0].size() != 0)return false;
	if (a[1].size() != 1)return false;
	if (a[2].size() != 4)return false;
	if (a[3].size() != 3)return false;
	if (a[4].size() != 2)return false;
	return true;
}());

//uninitialized ctor + resize + move ctor + move assign + truncate + clear
STATIC_ASSERT([]
{
	using Element = GrowableArray<int>;
	GrowableArray<Element> a{ uninitialized, 10 };
	if (a.size() != 10)return false;
	for (Element& x : a)
		CTOR_CONST_AT(&x, uninitialized, 10);
	a.resize(100);
	if (a.size() != 100)return false;
	a[99].resize(10);
	a.resize(9);
	GrowableArray<Element> b{ MOV(a) };
	if (b.size() != 9)return false;
	GrowableArray<Element> c{ uninitialized, 20 };
	if (c.size() != 20)return false;
	for (Element& x : c)
		CTOR_CONST_AT(&x, uninitialized, 10);
	c = MOV(b);
	if (c.size() != 9)return false;
	c.truncate(1);
	if (c.size() != 1)return false;
	c.clear();
	if (c.size() != 0)return false;
	return true;
}());

//reserve
STATIC_ASSERT([]
{
	using Element = std::unique_ptr<int>;
	GrowableArray<Element> a{ uninitialized, 5 };
	if (a.size() != 5)return false;
	for (Element& x : a)
		CTOR_CONST_AT(&x, new int);
	a.reserve(100);
	if (a.size() != 5)return false;
	if (a.capacity() != 100)return false;
	return true;
}());