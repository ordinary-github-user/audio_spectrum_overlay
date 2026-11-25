#pragma once
#include <array>
#include <memory>
#include "../../Math/Numeric/Pow2.h"
#include "../Assert.h"
#include "../BasicTypes.h"
#include "../Compact.h"
#include "../Compiler.h"
#include "../Concepts/Integral.h"
#include "../Concepts/Iterator.h"
#include "../Concepts/LinearContainer.h"
#include "../Concepts/NotCvref.h"
#include "../Concepts/Printable.h"
#include "../Identity.h"
#include "../Inline.h"
#include "../Iota.h"
#include "../Macro/Consteval.h"
#include "../Macro/If.h"
#include "../Macro/MovForward.h"
#include "../Macro/NoCopyMove.h"
#include "../Macro/PlacementNew.h"
#include "../SizedInt.h"
#include "../StaticAssert.h"
#include "../Traits/Like.h"
template<$NotCvref T, $UInt Index, $UInt Size>
struct RingBufferImpl
{
	template<class Self>INLINE constexpr Like<Self, T> front(this Self&& self)noexcept { ASSERT(self.size() != 0); return self.m_data[(self.m_iBack + Index{ 1 }) % self.size()]; }
	template<class Self>INLINE constexpr Like<Self, T> back (this Self&& self)noexcept { ASSERT(self.size() != 0); return self.m_data[self.m_iBack]; }
	INLINE constexpr void emplace_front(this auto&& self, auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		ASSERT(self.m_iBack < self.size());
		T* const p{ __builtin_addressof(self.m_data[self.m_iBack]) };
		if constexpr (!__is_trivially_destructible(T))
			p->~T();
		CTOR_CONST_AT(p, FWD(args)...);
#if 1
		if (self.m_iBack)
			--self.m_iBack;
		else
			self.m_iBack = self.size() - 1;
#else
		self.m_iBack = (self.m_iBack + self.size() - 1) % self.size();
#endif
	}
	INLINE constexpr void emplace_back(this auto&& self, auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		ASSERT(self.m_iBack < self.size());
#if 1
		++self.m_iBack;
		if (self.m_iBack == self.size())
			self.m_iBack = {};
#else
		self.m_iBack = (self.m_iBack + 1) % self.size();
#endif
		T* const p{ __builtin_addressof(self.m_data[self.m_iBack]) };
		if constexpr (!__is_trivially_destructible(T))
			p->~T();
		CTOR_CONST_AT(p, FWD(args)...);
	}
	INLINE constexpr void append(this auto&& self, $IteratorOf<T> auto p, $UInt auto s)noexcept
	requires (requires{p != p + s; })
	{
		if (s > self.size())
		{
			p += (s - self.size());
			s = static_cast<decltype(s)>(self.size());
		}
		ASSERT(s <= self.size());
		for (const auto pEnd{ p + s }; p != pEnd; ++p)
			self.emplace_back(*p);
	}
	INLINE constexpr void append(this auto&& self, $IteratorOf<T> auto pBegin, auto pEnd)noexcept
	requires (!requires{pBegin + pEnd; }&& requires{pBegin != pEnd; }&& requires{pEnd - pBegin; })
	{
		self.append(pBegin, static_cast<UnsignedBytes<sizeof(decltype(pEnd - pBegin))>>(pEnd - pBegin));
	}
	INLINE constexpr void append(this auto&& self, $LinearContainerOf<T> auto&& elements)noexcept
	{
		return self.append(elements.begin(), elements.size());
	}
	INLINE constexpr void prepend(this auto&& self, $IteratorOf<T> auto p, $UInt auto s)noexcept
	requires (requires{p != p + s; })
	{
		if (s > self.size())
			s = static_cast<decltype(s)>(self.size());
		ASSERT(s <= self.size());
		ASSERT(self.m_iBack < self.size());
		if constexpr (requires{--p; })
			for (auto pCurr{ p + s }; pCurr != p;)
			{
				--pCurr;
				self.emplace_front(*pCurr);
			}
		else
			while (s--)
				self.emplace_front(*(p + s));
	}
	INLINE constexpr void prepend(this auto&& self, $IteratorOf<T> auto pBegin, auto pEnd)noexcept
	requires (!requires{pBegin + pEnd; } && requires{pBegin != pEnd; }&& requires{pEnd - pBegin; })
	{
		self.prepend(pBegin, static_cast<UnsignedBytes<sizeof(decltype(pEnd - pBegin))>>(pEnd - pBegin));
	}
	INLINE constexpr void prepend(this auto&& self, $LinearContainerOf<T> auto&& elements)noexcept
	{
		return self.prepend(elements.begin(), elements.size());
	}
	INLINE constexpr void clear(this auto&& self, auto&&...args) noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		T* const pEnd{ __builtin_addressof(self.m_data[self.size()]) };
		for (T* p{ __builtin_addressof(self.m_data[0]) }; p != pEnd; ++p)
		{
			if constexpr (!__is_trivially_destructible(T))
				p->~T();
			CTOR_CONST_AT(p, FWD(args)...);
		}
	}
};
template<class, usize, class>struct FixedRingImpl;
template<$NotCvref T, usize N>using FixedRing = FixedRingImpl<T, N, std::make_index_sequence<N>>;
template<$NotCvref T, usize N, usize...I>
requires(N != 0)
struct FixedRingImpl<T, N, std::index_sequence<I...>> :RingBufferImpl<T, Compact<N - 1>, Compact<N>>
{
	NO_COPY_MOVE(FixedRingImpl)
private:
	using IterIndex = Compact<N * 2 - 1>;
public:
	using Index = Compact<N - 1>;
	using Size = Compact<N>;
	T m_data[N];
	Index m_iBack{};
	static IF(MSVC, constexpr, CONSTEVAL) Size size()noexcept { return N; }
	INLINE constexpr FixedRingImpl(auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	: m_data{ Identity<T, I>{ FWD(args)... }...}
	{}
	template<bool forward, class U>
	struct Iterator
	{
		U* pBuffer;
		IterIndex index;
		INLINE constexpr Iterator& operator++() noexcept { if constexpr (forward) ++index; else --index; return *this; }
		INLINE constexpr bool operator!=($UInt auto iEnd)const noexcept { return index != iEnd; }
		INLINE constexpr U& operator*() const noexcept
		{
			if constexpr(Math::isPowerOf2(N))return pBuffer[index % compact<N>];
			else return pBuffer[index >= N ? index - N : index];
		}
	};
	template<class Self>
	INLINE constexpr Iterator<1, std::remove_reference_t<Like<Self, T>>> begin(this Self&& self, Index offset = {})noexcept
	{
		return
		{
			.pBuffer{__builtin_addressof(self.m_data[0])},
			.index{ IterIndex(offset + (IterIndex{self.m_iBack} + IterIndex{1}) % compact<N>) }
		};
	}
	INLINE constexpr IterIndex end()const noexcept { return IterIndex(compact<N>+Index(Size(m_iBack + Size{ 1 }) % compact<N>)); }
	template<class Self>
	INLINE constexpr Iterator<0, std::remove_reference_t<Like<Self, T>>> rbegin(this Self&& self, Index offset = {})noexcept
	{
		return
		{
			.pBuffer{__builtin_addressof(self.m_data[0])},
			.index{ IterIndex(IterIndex{self.m_iBack} + compact<N> - offset) },
		};
	}
	INLINE constexpr Index rend()const noexcept { return m_iBack; }
	template<class Self>
	INLINE constexpr Like<Self, T> operator[](this Self&& self, Index index)noexcept
	{
		ASSERT(index < N);
		if constexpr (Math::isPowerOf2(N))return self.m_data[(index + self.m_iBack + 1) % N];
		else
		{
			const IterIndex i{ IterIndex(IterIndex{index} + IterIndex{self.m_iBack} + Index{1}) };
			return self.m_data[i >= N ? Index(i - N) : Index(i)];
		}
		
	}
};
template<class Stream, $Printable<Stream> T, usize N>
INLINE constexpr Stream& operator<<(Stream& os, const FixedRing<T, N>& ring)noexcept
{
	bool isFirst{ true };
	os << '[';
	for(const T& entry: ring)
	{
		if (!isFirst)
			os << ',';
		os << entry;
		isFirst = false;
	};
	os << ']';
	return os;
}
STATIC_ASSERT([]static
{
	using T = i32;
	static constexpr u8 N{ 3 };
	FixedRing<T, N> ring{ 999 };
	const auto verifyElements{ [&ring](std::array<T, N> xs)
	{
		{
			usize i{N - 1};
			for (auto it{ring.rbegin()}; it != ring.rend(); ++it, --i)
				if (xs[i] != *it)
					return false;
		}
		for (const auto i : iota<N>)
			if (ring[i] != xs[i])
				return false;
		return xs.front() == ring.front() && ring.back() == xs.back();
	} };
	if (!verifyElements({ 999,999,999 }))return false;
	ring.~FixedRing<T, N>();
	CTOR_CONST_AT(&ring);
	if (!verifyElements({ 0,0,0 }))return false;
	ring.emplace_back(1); if (!verifyElements({ 0,0,1 }))return false;
	ring.emplace_back(2); if (!verifyElements({ 0,1,2 }))return false;
	ring.emplace_back(3); if (!verifyElements({ 1,2,3 }))return false;
	ring.emplace_back(4); if (!verifyElements({ 2,3,4 }))return false;
	ring.emplace_back(5); if (!verifyElements({ 3,4,5 }))return false;
	ring.emplace_back(6); if (!verifyElements({ 4,5,6 }))return false;
	ring.clear(); if (!verifyElements({ 0,0,0 }))return false;
	ring.emplace_front(1); if (!verifyElements({ 1,0,0 }))return false;
	ring.emplace_front(2); if (!verifyElements({ 2,1,0 }))return false;
	ring.emplace_front(3); if (!verifyElements({ 3,2,1 }))return false;
	ring.emplace_front(4); if (!verifyElements({ 4,3,2 }))return false;
	ring.emplace_front(5); if (!verifyElements({ 5,4,3 }))return false;
	ring.emplace_front(6); if (!verifyElements({ 6,5,4 }))return false;
	ring.append(std::array{ 1 }); if (!verifyElements(std::array{ 5,4,1 }))return false;
	ring.append(std::array{ 2,3 }); if (!verifyElements(std::array{ 1,2,3 }))return false;
	ring.append(std::array{ 4,5,6 }); if (!verifyElements(std::array{ 4,5,6 }))return false;
	ring.append(std::array{ 1,2,3,4 }); if (!verifyElements(std::array{ 2,3,4 }))return false;
	ring.append(std::array{ 5,6,7,8,9 }); if (!verifyElements(std::array{ 7,8,9 }))return false;
	ring.prepend(std::array{ 1 }); if (!verifyElements(std::array{ 1,7,8 }))return false;
	ring.prepend(std::array{ 2,3 }); if (!verifyElements(std::array{ 2,3,1 }))return false;
	ring.prepend(std::array{ 4,5,6 }); if (!verifyElements(std::array{ 4,5,6 }))return false;
	ring.prepend(std::array{ 1,2,3,4 }); if (!verifyElements(std::array{ 1,2,3 }))return false;
	ring.prepend(std::array{ 5,6,7,8,9 }); if (!verifyElements(std::array{ 5,6,7 }))return false;
	return true;
}());
STATIC_ASSERT([]static
{
	using T = std::unique_ptr<i32>;
	static constexpr u8 N{ 3 };
	FixedRing<T, N> ring{};
	for (const auto _ [[maybe_unused]] : iota<N * 2>)
		ring.emplace_back(new i32);
	return true;
}());
STATIC_ASSERT([]static
{
	struct T
	{
		std::unique_ptr<i32> a;
		constexpr T(i32 x)noexcept :a{ new i32{x} } {}
	};
	static constexpr u8 N{ 3 };
	FixedRing<T, N> ring{ 123 };
	for (const auto i : iota<N * 2>)
		ring.emplace_back(i);
	return true;
}());
STATIC_ASSERT([]static
{
	using T = i32;
	static constexpr u8 N{ 129 };
	FixedRing<T, N> ring{ -1 };
	for (const auto i : iota<1, N>)
		ring.emplace_back(static_cast<T>(i));
	if (ring[0] != -1)
		return false;
	return ring[compact<N - 1>] == compact<N - 1>;
}());
#include "Allocator.h"
template<$NotCvref T, $UInt Index = usize>
struct DynamicRing :RingBufferImpl<T, Index, Index>
{
	NO_COPY_MOVE(DynamicRing)
	using IterIndex = UnsignedBytes<sizeof(Index) == sizeof(usize) ? sizeof(usize) : sizeof(Index) * 2>;
	T* m_data;
	Index m_size;
	Index m_iBack{ 0 };
	INLINE constexpr Index size()const noexcept { return m_size; }
	INLINE constexpr bool empty()const noexcept { return !m_size; }
	INLINE constexpr DynamicRing()noexcept :m_data{}, m_size{} {}
	INLINE constexpr ~DynamicRing()noexcept
	{
		if (m_data)
		{
			if constexpr (!__is_trivially_destructible(T))
				for (T* p{ m_data }, *const pEnd{ m_data + m_size }; p != pEnd; ++p)
					p->~T();
			deallocate(m_data, m_size);
		}
	}
	INLINE constexpr DynamicRing(Index size, auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	: m_data{ allocate<T>(size) }, m_size{ size }
	{
		for (T* p{ m_data }, *const pEnd{ m_data + m_size }; p != pEnd; ++p)
			CTOR_CONST_AT(p, FWD(args)...);
	}
	INLINE constexpr void resize(Index size, auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		this->~DynamicRing();
		CTOR_CONST_AT(this, size, FWD(args)...);
	}
	template<bool forward, class U>
	struct Iterator
	{
		IterIndex index;
		Index size;
		U* pBuffer;
		INLINE constexpr Iterator& operator++() noexcept { if constexpr (forward) ++index; else --index; return *this; }
		INLINE constexpr bool operator!=($UInt auto iEnd)const noexcept { return index != iEnd; }
		INLINE constexpr U& operator*() const noexcept { return pBuffer[index >= size ? index - size : index]; }
	};
	template<class Self>
	INLINE constexpr Iterator<1, std::remove_reference_t<Like<Self, T>>> begin(this Self&& self, Index offset = {})noexcept
	{
		return
		{
			.index{ IterIndex(offset + (self.m_size ? IterIndex((self.m_iBack + IterIndex{1}) % self.m_size) : IterIndex{})) },
			.size{ self.m_size },
			.pBuffer{ __builtin_addressof(self.m_data[0]) }
		};
	}
	INLINE constexpr IterIndex end()const noexcept { return m_size ? IterIndex(m_size + static_cast<IterIndex>(m_iBack + IterIndex{ 1 }) % m_size) : IterIndex{}; }
	template<class Self>
	INLINE constexpr Iterator<0, std::remove_reference_t<Like<Self, T>>> rbegin(this Self&& self, Index offset = {})noexcept
	{
		return
		{
			.index{ IterIndex{self.m_iBack} + IterIndex{self.m_size} - offset },
			.size{ self.m_size },
			.pBuffer{ __builtin_addressof(self.m_data[0]) }
		};
	}
	INLINE constexpr Index rend()const noexcept { return m_iBack; }
	template<class Self>
	INLINE constexpr Like<Self, T> operator[](this Self&& self, Index index)noexcept
	{
		ASSERT(index < self.m_size);
		ASSERT(self.m_size != 0);
		const IterIndex i{ IterIndex(IterIndex{index} + IterIndex{self.m_iBack} + Index{1}) };
		return self.m_data[i >= self.m_size ? Index(i - self.m_size): Index(i)];
	}
};
template<class Stream, $Printable<Stream> T, class Index>
INLINE constexpr Stream& operator<<(Stream& os, const DynamicRing<T, Index>& ring)noexcept
{
	bool isFirst{ true };
	os << '[';
	for (const T& entry : ring)
	{
		if (!isFirst)
			os << ',';
		os << entry;
		isFirst = false;
	};
	os << ']';
	return os;
}
STATIC_ASSERT([]static
{
	using T = i32;
	DynamicRing<T> ring{ 3, 999 };
	const auto verifyElements{ [&ring](auto const& xs)
	{
		if (ring.size() != xs.size())return false;
		{
			usize i{};
			for (auto&& e : ring)
				if (xs[i++] != e)
					return false;
		}
		{
			usize i{ xs.size() - 1};
			for (auto it{ ring.rbegin() }; it != ring.rend(); ++it, --i)
				if (xs[i] != *it)
					return false;
		}
		for (const auto i : iota_up(ring.size()))
			if (ring[i] != xs[i])
				return false;
		return xs.front() == ring.front() && ring.back() == xs.back();
	} };
	if (!verifyElements(std::array{ 999,999,999 }))return false;
	ring.~DynamicRing<T>();
	CTOR_CONST_AT(&ring, 3);
	if (!verifyElements(std::array{ 0,0,0 }))return false;
	ring.emplace_back(1); if (!verifyElements(std::array{ 0,0,1 }))return false;
	ring.emplace_back(2); if (!verifyElements(std::array{ 0,1,2 }))return false;
	ring.emplace_back(3); if (!verifyElements(std::array{ 1,2,3 }))return false;
	ring.emplace_back(4); if (!verifyElements(std::array{ 2,3,4 }))return false;
	ring.emplace_back(5); if (!verifyElements(std::array{ 3,4,5 }))return false;
	ring.emplace_back(6); if (!verifyElements(std::array{ 4,5,6 }))return false;
	ring.clear(); if (!verifyElements(std::array{ 0,0,0 }))return false;
	ring.emplace_front(1); if (!verifyElements(std::array{ 1,0,0 }))return false;
	ring.emplace_front(2); if (!verifyElements(std::array{ 2,1,0 }))return false;
	ring.emplace_front(3); if (!verifyElements(std::array{ 3,2,1 }))return false;
	ring.emplace_front(4); if (!verifyElements(std::array{ 4,3,2 }))return false;
	ring.emplace_front(5); if (!verifyElements(std::array{ 5,4,3 }))return false;
	ring.emplace_front(6); if (!verifyElements(std::array{ 6,5,4 }))return false;
	ring.resize(4); if (!verifyElements(std::array{ 0,0,0,0 }))return false;
	ring.emplace_back(1); if (!verifyElements(std::array{ 0,0,0,1 }))return false;
	ring.emplace_back(2); if (!verifyElements(std::array{ 0,0,1,2 }))return false;
	ring.emplace_back(3); if (!verifyElements(std::array{ 0,1,2,3 }))return false;
	ring.emplace_back(4); if (!verifyElements(std::array{ 1,2,3,4 }))return false;
	ring.emplace_back(5); if (!verifyElements(std::array{ 2,3,4,5 }))return false;
	ring.emplace_back(6); if (!verifyElements(std::array{ 3,4,5,6 }))return false;
	ring.append(std::array{ 1 }); if (!verifyElements(std::array{ 4,5,6,1 }))return false;
	ring.append(std::array{ 2,3 }); if (!verifyElements(std::array{ 6,1,2,3 }))return false;
	ring.append(std::array{ 4,5,6 }); if (!verifyElements(std::array{ 3,4,5,6 }))return false;
	ring.append(std::array{ 1,2,3,4 }); if (!verifyElements(std::array{ 1,2,3,4 }))return false;
	ring.append(std::array{ 5,6,7,8,9 }); if (!verifyElements(std::array{ 6,7,8,9 }))return false;
	ring.prepend(std::array{ 1 }); if (!verifyElements(std::array{ 1,6,7,8 }))return false;
	ring.prepend(std::array{ 2,3 }); if (!verifyElements(std::array{ 2,3,1,6 }))return false;
	ring.prepend(std::array{ 4,5,6 }); if (!verifyElements(std::array{ 4,5,6,2 }))return false;
	ring.prepend(std::array{ 1,2,3,4 }); if (!verifyElements(std::array{ 1,2,3,4 }))return false;
	ring.prepend(std::array{ 5,6,7,8,9 }); if (!verifyElements(std::array{ 5,6,7,8 }))return false;
	return true;
}());
STATIC_ASSERT([]static
{
	using T = std::unique_ptr<i32>;
	static constexpr u8 N{ 3 };
	DynamicRing<T> ring{ N };
	for (const auto _ [[maybe_unused]] : iota<N * 2>)
		ring.emplace_back(new i32);
	return true;
}());
STATIC_ASSERT([]static
{
	struct T
	{
		std::unique_ptr<i32> a;
		constexpr T(i32 x)noexcept :a{ new i32{x} } {}
	};
	static constexpr u8 N{ 3 };
	DynamicRing<T> ring{ N, 123 };
	for (const auto i : iota<N * 2>)
		ring.emplace_back(i);
	return true;
}());
STATIC_ASSERT([]static
{
	using T = std::unique_ptr<i32>;
	static constexpr u8 N{ 0 };
	DynamicRing<T> ring{ N };
	if (ring.begin() != ring.end())return false;
	if (ring.rbegin() != ring.rend())return false;
	return true;
}());
STATIC_ASSERT([]static
{
	using Ring = DynamicRing<i32, u8>;
	Ring ring{ u8{200}, -1 };
	for (const auto value : iota<1, 301>)
		ring.emplace_back(value);
	i32 expected{ 101 };
	for (const i32 value : ring)
	{
		if (value != expected)
			return false;
		++expected;
	}
	return expected == 301;
}());