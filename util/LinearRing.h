#pragma once
#include "Assert.h"
#include "ConceptNotCvref.h"
#include "ConceptPrintable.h"
#include "Consteval.h"
#include "Crash.h"
#include "Iota.h"
#include "Like.h"
#include "MacroNoCopyMove.h"
#include "MacroPlacementNew.h"
#include "NextAligned.h"
#include "ReversePointer.h"
#include "Section.h"
template<$NotCvref T, $UInt Index>
struct LinearRingImpl
{
	bool allocate(this auto&& self)noexcept
	{
		u64 singleBufferSize64{ self.singleBufferSize() };
		if constexpr (requires{self.m_capacity; })
			self.m_capacity = singleBufferSize64 / sizeof(T);
		usize bufferSize{ static_cast<usize>(singleBufferSize64) * 2 };
		if (!self.m_section.create({}, &singleBufferSize64, SECTION_ALL_ACCESS, Windows::PageProtectFlag::readwrite, SEC_COMMIT))
			return false;
		if (!NT_SUCCESS(NTDLL::NtAllocateVirtualMemoryEx(NTDLL::NtCurrentProcess(), reinterpret_cast<usize*>(&self.m_pBuffer), &bufferSize, Windows::MemAllocationFlag::reserve | Windows::MemAllocationFlag::reserve_placeholder, Windows::PageProtectFlag::noaccess, {}, {})))
			return false;
		if (!NT_SUCCESS(NTDLL::NtFreeVirtualMemory(NTDLL::NtCurrentProcess(), reinterpret_cast<void**>(&self.m_pBuffer), reinterpret_cast<usize*>(&singleBufferSize64), Windows::MemFreeFlag::release | Windows::MemFreeFlag::preserve_placeholder)))
			return false;
		for (const u8 i : iota<2>)
		{
			u8* pBuffers{ reinterpret_cast<u8*>(self.m_pBuffer) + i * static_cast<usize>(singleBufferSize64) };
			if (!self.m_section.map(NTDLL::NtCurrentProcess(), pBuffers, reinterpret_cast<usize&>(singleBufferSize64), Windows::PageProtectFlag::readwrite, Windows::MemAllocationFlag::replace_placeholder))
				return false;
		}
		return true;
	}
	bool deallocate(this auto&& self)noexcept
	{
		return self.m_section.unmap(NTDLL::NtCurrentProcess(), reinterpret_cast<usize>(self.m_pBuffer) + self.singleBufferSize())
			&& self.m_section.unmap(NTDLL::NtCurrentProcess(), reinterpret_cast<usize>(self.m_pBuffer));
	}
#if 1
	template<class Self>INLINE LikePtr<Self, T> data(this Self&& self)noexcept { return self.m_pBuffer + self.capacity() + self.m_iBack + 1 - self.size(); }
	template<class Self>INLINE LikePtr<Self, T> begin(this Self&& self)noexcept { return self.m_pBuffer + self.capacity() + self.m_iBack + 1 - self.size(); }
	template<class Self>INLINE LikePtr<Self, T> end(this Self&& self)noexcept { return self.m_pBuffer + self.capacity() + self.m_iBack + 1; }
	template<class Self>INLINE ReversePointer<LikePtr<Self, T>> rbegin(this Self&& self) noexcept { return ReversePointer<LikePtr<Self, T>>{ self.end() }; }
	template<class Self>INLINE ReversePointer<LikePtr<Self, T>> rend(this Self&& self) noexcept { return ReversePointer<LikePtr<Self, T>>{ self.begin() }; }
#else
	//when empty(), m_pBuffer is invalid. "pointer+int" is ub https://eel.is/c++draft/expr.add. hence the workaround
	template<class Self>INLINE UPointer<LikePtr<Self, T>> data(this Self&& self)noexcept { return UPointer<LikePtr<Self, T>>{ self.m_pBuffer } + self.capacity() + self.m_iBack + 1 - self.size(); }
	template<class Self>INLINE UPointer<LikePtr<Self, T>> begin(this Self&& self)noexcept { return UPointer<LikePtr<Self, T>>{ self.m_pBuffer } + self.capacity() + self.m_iBack + 1 - self.size(); }
	template<class Self>INLINE UPointer<LikePtr<Self, T>> end(this Self&& self)noexcept { return UPointer<LikePtr<Self, T>>{ self.m_pBuffer } + self.capacity() + self.m_iBack + 1; }
	template<class Self>INLINE UReversePointer<LikePtr<Self, T>> rbegin(this Self&& self) noexcept { return UReversePointer<LikePtr<Self, T>>{ self.m_pBuffer } - self.capacity() - self.m_iBack; }
	template<class Self>INLINE UReversePointer<LikePtr<Self, T>> rend(this Self&& self) noexcept { return UReversePointer<LikePtr<Self, T>>{ self.m_pBuffer } - self.capacity() - self.m_iBack + self.size(); }
#endif

	template<class Self>
	INLINE Like<Self, T> operator[](this Self&& self, Index index)noexcept
	{
		ASSERT(index < self.size());
		ASSERT(self.size() != 0);
		return FWD_LIKE(self, self.m_pBuffer[self.capacity() + self.m_iBack + 1 - self.size() + index]);
	}

	template<class Self>INLINE Like<Self, T> front(this Self&& self)noexcept
	{
		ASSERT(self.size() != 0);
		return FWD_LIKE(self, self.m_pBuffer[self.capacity() + self.m_iBack + 1 - self.size()]);
	}
	template<class Self>INLINE Like<Self, T> back(this Self && self)noexcept
	{
		ASSERT(self.size() != 0);
		return FWD_LIKE(self, self.m_pBuffer[self.capacity() + self.m_iBack]);
	}

	INLINE void reset(this auto&& self, auto&&...args) noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		for (T& x : self)
		{
			DTOR(x);
			CTOR(x) { FWD(args)... };
		}
	}
	INLINE void emplace_front(this auto&& self, auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		ASSERT(self.size() != 0);
		ASSERT(self.m_iBack < self.capacity());
		T* const pDest{ __builtin_addressof(self.m_pBuffer[self.capacity() + self.m_iBack - self.size()]) };
		DTOR_AT(pDest);
		CTOR_AT(pDest) { FWD(args)... };
		self.m_iBack = (self.m_iBack + self.capacity() - 1) % self.capacity();
	}
	INLINE void emplace_back(this auto&& self, auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		ASSERT(self.size() != 0);
		ASSERT(self.m_iBack < self.capacity());
		self.m_iBack = (self.m_iBack + 1) % self.capacity();
		DTOR(self.m_pBuffer[self.m_iBack]);
		CTOR(self.m_pBuffer[self.m_iBack]) { FWD(args)... };
	}
	INLINE constexpr void prepend(this auto&& self, $IteratorOf<T> auto p, $UInt auto s)noexcept
	{
		ASSERT(self.size() != 0);
		if (s > self.size())
			s = static_cast<decltype(s)>(self.size());
		ASSERT(s <= self.size());
		ASSERT(self.m_iBack < self.capacity());
		self.m_iBack = (self.m_iBack + self.capacity() - s) % self.capacity();
		for (T* pDest{ __builtin_addressof(self.front()) }, *const pEnd{ pDest + s }; pDest != pEnd; ++pDest, ++p)
		{
			DTOR_AT(pDest);
			CTOR_AT(pDest) { *p };
		}
	}
	INLINE constexpr void prepend(this auto&& self, $IteratorOf<T> auto pBegin, auto pEnd)noexcept
	requires (!requires{pBegin + pEnd; }&& requires{pEnd - pBegin; })
	{
		self.prepend(pBegin, static_cast<UnsignedBytes<sizeof(decltype(pEnd - pBegin))>>(pEnd - pBegin));
	}
	INLINE constexpr void prepend(this auto&& self, $LinearContainerOf<T> auto&& elements)noexcept
	{
		self.prepend(elements.begin(), elements.size());
	}
	INLINE constexpr void append(this auto&& self, $IteratorOf<T> auto p, $UInt auto s)noexcept
	requires (requires{p += static_cast<decltype(s)>(s - self.size()); })
	{
		ASSERT(self.size() != 0);
		if (s > self.size())
		{
			p += static_cast<decltype(s)>(s - self.size());
			s = static_cast<decltype(s)>(self.size());
		}
		ASSERT(s <= self.size());
		for (T* pDest{ self.m_pBuffer + self.m_iBack + Index{ 1 } }, *const pDestEnd{ pDest + s }; pDest != pDestEnd; ++pDest, ++p)
		{
			DTOR_AT(pDest);
			CTOR_AT(pDest) { *p };
		}
		ASSERT(self.m_iBack < self.capacity());
		self.m_iBack = (self.m_iBack + s) % self.capacity();
	}
	INLINE constexpr void append(this auto&& self, $IteratorOf<T> auto pBegin, auto pEnd)noexcept
	requires (!requires{pBegin + pEnd; }&& requires{pEnd - pBegin; })
	{
		self.append(pBegin, static_cast<UnsignedBytes<sizeof(decltype(pEnd - pBegin))>>(pEnd - pBegin));
	}
	INLINE constexpr void append(this auto&& self, /*$LinearContainerOf<T> */auto&& elements)noexcept
	{
		self.append(elements.begin(), elements.size());
	}
};

template<class, usize...>
struct LinearRing;

template<$NotCvref T, usize N= 0x1000 / sizeof(T)>
using FixedLinearRing = LinearRing<T, N>;

template<$NotCvref T>
using DynamicLinearRing = LinearRing<T>;

template<$NotCvref T, usize N>
requires(0x1000 % sizeof(T) == 0 && 0x1000 % alignof(T) == 0 && N != 0)
struct LinearRing<T, N>: LinearRingImpl<T, Compact<next_aligned_greater_equal<0x1000>(N * sizeof(T))>>
{
private:
	NO_COPY_MOVE(LinearRing)
	using Base = LinearRingImpl<T, Compact<next_aligned_greater_equal<0x1000>(N * sizeof(T))>>;
	friend Base;
	using Index = Compact<next_aligned_greater_equal<0x1000>(N * sizeof(T))>;
	Windows::Section m_section;
	T* m_pBuffer;
	Index m_iBack;//in number of T

	static consteval auto singleBufferSize()noexcept { return compact<next_aligned_greater_equal<0x1000>(N * sizeof(T))>; }
	static consteval auto capacity()noexcept { return compact<singleBufferSize() / sizeof(T)>; }
public:
	INLINE LinearRing(auto&&...args)noexcept requires(requires{T{ FWD(args)... }; }) : m_section{}, m_pBuffer{}, m_iBack{}
	{
		if (!Base::allocate()) [[unlikely]]
			CRASH();
		for (T* p{ m_pBuffer }, *const pEnd{ m_pBuffer + capacity() }; p != pEnd; ++p)
			CTOR_AT(p) { FWD(args)... };
	}
	INLINE ~LinearRing()noexcept
	{
		if constexpr (!__is_trivially_destructible(T))
			for (T* p{ m_pBuffer }, *const pEnd{ m_pBuffer + capacity() }; p != pEnd; ++p)
				p->~T();
		if (!Base::deallocate()) [[unlikely]]
			CRASH();
	}
	static CONSTEVAL Index size()noexcept { return N; }
	static CONSTEVAL bool empty()noexcept { return !size(); }
};

template<$NotCvref T>
requires(0x1000 % sizeof(T) == 0 && 0x1000 % alignof(T) == 0)
struct LinearRing<T> : LinearRingImpl<T, usize>
{
private:
	NO_COPY_MOVE(LinearRing)
	using Base = LinearRingImpl<T, usize>;
	friend Base;
	using Index = usize;
	Windows::Section m_section;
	T* m_pBuffer;
	Index m_iBack;//in number of T
	Index m_size;//in number of T
	Index m_capacity;//in number of T

	INLINE Index singleBufferSize()const noexcept { return next_aligned_greater_equal<0x1000>(m_size * sizeof(T)); }
	INLINE Index capacity()const noexcept { return m_capacity; }

public:
	INLINE LinearRing()noexcept: m_section{}, m_pBuffer{}, m_iBack{}, m_size{}, m_capacity{} {}
	INLINE LinearRing(Index size, auto&&...args)noexcept requires(requires{T{ FWD(args)... }; }) : m_section{}, m_pBuffer{}, m_iBack{}, m_size { size }
	{
		if (!m_size) [[unlikely]] { m_capacity = {}; return; }
		if (!Base::allocate()) [[unlikely]]
			CRASH();
		for (T* p{ m_pBuffer }, *const pEnd{ m_pBuffer + m_capacity }; p != pEnd; ++p)
			CTOR_AT(p) { FWD(args)... };
	}
	INLINE ~LinearRing()noexcept
	{
		if (!m_size) [[unlikely]] return;
		if constexpr (!__is_trivially_destructible(T))
			for (T* p{ m_pBuffer }, *const pEnd{ m_pBuffer + m_capacity }; p != pEnd; ++p)
				p->~T();
		if (!Base::deallocate()) [[unlikely]]
			CRASH();
	}

	INLINE Index size()const noexcept { return m_size; }
	INLINE bool empty()const noexcept { return !size(); }

	INLINE void resize(Index size, auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		if (next_aligned_greater_equal<0x1000>(size * sizeof(T)) == singleBufferSize())
		{
			for (T* p{ m_pBuffer }, *const pEnd{ m_pBuffer + m_capacity }; p != pEnd; ++p)
			{
				DTOR_AT(p);
				CTOR_AT(p) { FWD(args)... };
			}
			m_size = size;
		}
		else
		{
			DTOR_AT(this);
			CTOR_AT(this) { size, FWD(args)... };
		}
	}
};
template<class Stream, $Printable<Stream> T, usize...N>
Stream& operator<<(Stream& os, const LinearRing<T, N...>& ring)noexcept
{
	bool first{ true };
	os << '[';
	for (const T& x : ring)
	{
		if (!first)
			os << ',';
		first = false;
		os << x;
	}
	os << ']';
	return os;
}