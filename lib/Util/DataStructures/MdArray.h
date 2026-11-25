#pragma once
#include "../Assert.h"
#include "../BasicTypes.h"
#include "../Concepts/Integral.h"
#include "../Concepts/NotCvref.h"
#include "../ElementAt.h"
#include "../Inline.h"
#include "../StaticAssert.h"
#include "../Tags/Uninitialized.h"
#include "../Traits/Like.h"
#include "Allocator.h"
#include "Tuple.h"
template<class...>
struct MdArrayImpl;

template<$NotCvref T, $UInt...Ns>
requires((... && $NotCvref<Ns>))
using MdArray = MdArrayImpl<std::make_index_sequence<sizeof...(Ns)>, T, Ns...>;

template<usize...I, $NotCvref T, $UInt...Ns>
requires(sizeof...(Ns) != 0 && (... && $NotCvref<Ns>))
struct MdArrayImpl<std::index_sequence<I...>, T, Ns...>
{
private:
	using RealIndex = std::remove_cvref_t<decltype((...*Ns{})) > ;
	T* m_ptr;
	Tuple<Ns...> m_extends;
	INLINE constexpr void destruct()noexcept
	{
		if (m_ptr)
		{
			if constexpr (!__is_trivially_destructible(T))
				for (T* p{ m_ptr }, * const pEnd{ m_ptr + size() }; p != pEnd; ++p)
					p->~T();
			::deallocate(m_ptr, size());
		}
	}
public:
	template<Compact<sizeof...(Ns) - 1> index>
	requires(index < sizeof...(Ns))
	INLINE constexpr TYPE_AT<index, Ns...> extend()const noexcept { return get<index>(m_extends); }
	INLINE constexpr RealIndex size()const noexcept { return (... * get<I>(m_extends)); }
	template<class Self>INLINE constexpr Like<Self, T> front(this Self&& self) noexcept { ASSERT(!self.empty()); return FWD(self).m_ptr[0]; }
	template<class Self>INLINE constexpr Like<Self, T> back(this Self&& self) noexcept { ASSERT(!self.empty()); return FWD(self).m_ptr[self.size() - 1]; }
	template<class Self>INLINE constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return self.m_ptr; }
	template<class Self>INLINE constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return self.m_ptr; }
	template<class Self>INLINE constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + self.size(); }
	template<class Self>INLINE constexpr Like<Self, T> operator[](this Self&& self, Ns...indexes) noexcept
	{
		RealIndex realIndex{};
#if __cpp_pack_indexing >= 202311L
		(..., (realIndex = realIndex * get<I>(self.m_extends) + indexes...[I]));
#else
		(..., (realIndex = realIndex * get<I>(self.m_extends) + element_at<I>(indexes...)));
#endif
		ASSERT(self.size() > realIndex);
		return FWD(self).m_ptr[realIndex];
	}
	template<class Self>INLINE constexpr LikePtr<Self, T> ptr(this Self&& self, Ns...indexes) noexcept
	{
		RealIndex realIndex{};
#if __cpp_pack_indexing >= 202311L
		(..., (realIndex = realIndex * get<I>(self.m_extends) + indexes...[I]));
#else
		(..., (realIndex = realIndex * get<I>(self.m_extends) + element_at<I>(indexes...)));
#endif
		return FWD(self).m_ptr + realIndex;
	}
	INLINE constexpr MdArrayImpl()noexcept :m_ptr{}, m_extends{} {}
	INLINE constexpr MdArrayImpl(Uninitialized, Ns...ns)noexcept : m_ptr{ ::allocate<T>((... * ns)) }, m_extends{ ns... } {}
	INLINE constexpr MdArrayImpl(Ns...ns, auto&&...args)noexcept requires requires{T{ FWD(args)... }; } : MdArrayImpl{ uninitialized, ns... }
	{
		for (T* p{ m_ptr }, * const pEnd{ m_ptr + (... * ns) }; p != pEnd; ++p)
			CTOR_CONST_AT(p, FWD(args)...);
	}
	INLINE constexpr MdArrayImpl(MdArrayImpl&& rhs)noexcept :m_ptr{ rhs.m_ptr }, m_extends{ rhs.m_extends } { rhs.m_ptr = {}; }
	INLINE constexpr MdArrayImpl(const MdArrayImpl& rhs)noexcept = delete;
	INLINE constexpr MdArrayImpl& operator=(MdArrayImpl&& rhs)noexcept { this->~MdArrayImpl(); CTOR_CONST_AT(this, MOV(rhs)); return *this; }
	INLINE constexpr MdArrayImpl& operator=(const MdArrayImpl& rhs)noexcept = delete;
	INLINE constexpr ~MdArrayImpl()noexcept { destruct(); }

	INLINE constexpr bool empty()const noexcept { return (... || !get<I>(m_extends)); }

	//destroy old elements, new elements not initialized
	INLINE constexpr void resize_uninitialized(Ns...ns)noexcept
	{
		destruct();
		CTOR_CONST_AT(this, uninitialized, ns...);
	}

	INLINE constexpr void resize(Ns...ns, auto&&...args)noexcept
	requires requires{T{ FWD(args)... }; }
	{
		destruct();
		CTOR_CONST_AT(this, ns..., FWD(args)...);
	}

	INLINE constexpr void clear()noexcept
	{
		destruct();
		CTOR_CONST_AT(this);
	}
};

template<class Stream, $NotCvref T, $UInt...Ns>
constexpr Stream& operator<<(Stream& os, const MdArray<T, Ns...>& arr)noexcept
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
		MdArray<T, usize> arr{ 2 }; verify(arr, { 0,0 });
		arr[0] = 0; verify(arr, { 0,0 });
		arr[1] = 1; verify(arr, { 0,1 });
		arr.clear(); verify(arr, {});
		arr.resize(3); verify(arr, { 0,0,0 });
		arr[0] = 0; verify(arr, { 0,0,0 });
		arr[1] = 1; verify(arr, { 0,1,0 });
		arr[2] = 2; verify(arr, { 0,1,2 });
		MdArray<T, usize> moved{ MOV(arr) }; verify(moved, { 0,1,2 });
	}
	{
		MdArray<T, usize, usize> arr{ 2 ,3 }; verify(arr, { 0,0,0,0,0,0 });
		arr[0, 0] = 0; verify(arr, { 0,0,0,0,0,0 });
		arr[0, 1] = 1; verify(arr, { 0,1,0,0,0,0 });
		arr[0, 2] = 2; verify(arr, { 0,1,2,0,0,0 });
		arr[1, 0] = 3; verify(arr, { 0,1,2,3,0,0 });
		arr[1, 1] = 4; verify(arr, { 0,1,2,3,4,0 });
		arr[1, 2] = 5; verify(arr, { 0,1,2,3,4,5 });
		arr.clear(); verify(arr, {});
		arr.resize(3, 2); verify(arr, { 0,0,0,0,0,0 });
		arr[0, 0] = 0; verify(arr, { 0,0,0,0,0,0 });
		arr[0, 1] = 1; verify(arr, { 0,1,0,0,0,0 });
		arr[1, 0] = 2; verify(arr, { 0,1,2,0,0,0 });
		arr[1, 1] = 3; verify(arr, { 0,1,2,3,0,0 });
		arr[2, 0] = 4; verify(arr, { 0,1,2,3,4,0 });
		arr[2, 1] = 5; verify(arr, { 0,1,2,3,4,5 });
		MdArray<T, usize, usize> moved{ MOV(arr) }; verify(moved, { 0,1,2,3,4,5 });
	}
	return true;
}());

STATIC_ASSERT([]static
{
	MdArray<int, usize, usize> arr{ 2 ,0 };
	return arr.ptr(1, 0) == arr.ptr(1, 0);
}());
