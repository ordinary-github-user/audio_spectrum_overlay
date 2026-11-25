#pragma once
#include "../Algorithm/Copy.h"
#include "../Algorithm/MinMax.h"
#include "../BasicTypes.h"
#include "../Compiler.h"
#include "../Concepts/Assignable.h"
#include "../Concepts/Char.h"
#include "../Concepts/Constructible.h"
#include "../Concepts/FixSized.h"
#include "../Concepts/LinearContainer.h"
#include "../Concepts/Printable.h"
#include "../Concepts/Same.h"
#include "../Concepts/String.h"
#include "../Constant.h"
#include "../ElementOf.h"
#include "../Inline.h"
#include "../Macro/MovForward.h"
#include "../StaticAssert.h"
#include "../StringImpl.h"
#include "../Tags/Uninitialized.h"
#include "../TextEncoding/TextEncoding.h"
#include "Allocator.h"
#include "FixedString.h"
#include "StringView.h"
template<$Char T, usize Size = 16>
requires(Size >= sizeof(void*) && (Size % sizeof(void*) == 0))
struct Sso: StringImpl
{
private:
	friend StringImpl;
	static constexpr usize sso_cap{ Size / sizeof(T) };
	T* m_ptr;
	usize m_size;
	union
	{
		std::array<T, sso_cap> m_array;//todo: https://wg21.link/P3074
		usize m_cap;
	};
	INLINE constexpr bool sso()const noexcept 
	{
#if MSVC//MSVC bug: msvc think (const void*)__builtin_addressof(m_array[0]) != (const void*)&m_array in constexpr context, but is fine with operator[] on non active union member
		return m_ptr == static_cast<const void*>(__builtin_addressof(m_array[0]));
#else
		return m_ptr == static_cast<const void*>(&m_array);//cant call operator[] on m_array unless it is active union member
#endif
	}
	INLINE constexpr void destruct()noexcept
	{
		if (!sso())
			::deallocate(m_ptr, m_cap);
	}
public:
	constexpr Sso(Uninitialized, usize s)noexcept :m_size{s}
	{
		if (s <= sso_cap)
		{
			if consteval { CTOR_CONST_AT(&m_array); }
			m_ptr = __builtin_addressof(m_array[0]);
		}
		else
		{
			m_ptr = ::allocate<T>(s);
			m_cap = s;
		}
	}
	template<usize N>
	constexpr Sso(const T(&str)[N])noexcept requires(N - 1 <= sso_cap)
	{
		if consteval { CTOR_CONST_AT(&m_array); }
		m_ptr = __builtin_addressof(m_array[0]);
		m_size = N - 1;
#if VARIADIC_STRUCTURED_BINDING
		static constexpr auto [...I] { iota<N - 1> };
		(..., CTOR_CONST_AT(__builtin_addressof(m_array[constant<I>]), str[constant<I>]));
#else
		[&]<usize...I>(std::index_sequence<I...>)LAMBDA_INLINE
		{
			(..., CTOR_CONST_AT(__builtin_addressof(m_array[I]), str[I]));
		}(std::make_index_sequence<N - 1>{});
#endif
	}
	template<usize N>
	constexpr Sso(const T(&str)[N])noexcept requires(N - 1 > sso_cap) : m_ptr{ ::allocate<T>(N - 1) }, m_size{ N - 1 }, m_cap{ N - 1 }
	{
#if VARIADIC_STRUCTURED_BINDING
		static constexpr auto [...I] { iota<N - 1> };
		(..., CTOR_CONST_AT(m_ptr + constant<I>, str[constant<I>]));
#else
		[&]<usize...I>(std::index_sequence<I...>)LAMBDA_INLINE
		{
			(..., CTOR_CONST_AT(m_ptr + I, str[I]));
		}(std::make_index_sequence<N - 1>{});
#endif
	}

	constexpr Sso($Constructible<T> auto&&...args)noexcept requires(sizeof...(args) <= sso_cap)
	{
		if consteval { CTOR_CONST_AT(&m_array); }
		m_ptr = __builtin_addressof(m_array[0]);
		m_size = sizeof...(args);
#if VARIADIC_STRUCTURED_BINDING
		static constexpr auto [...I] { iota<sizeof...(args)> };
		(..., CTOR_CONST_AT(__builtin_addressof(m_array[constant<I>]), FWD(args)));
#else
		[&]<usize...I>(std::index_sequence<I...>)LAMBDA_INLINE
		{
			(..., CTOR_CONST_AT(__builtin_addressof(m_array[I]), FWD(args)));
		}(std::make_index_sequence<sizeof...(args)>{});
#endif
	}

	constexpr Sso($Constructible<T> auto&&...args)noexcept requires(sizeof...(args) > sso_cap) : m_ptr{ ::allocate<T>(sizeof...(args)) }, m_size{ sizeof...(args) }, m_cap{ sizeof...(args) }
	{
#if VARIADIC_STRUCTURED_BINDING
		static constexpr auto [...I] { iota<sizeof...(args)> };
		(..., CTOR_CONST_AT(m_ptr + constant<I>, FWD(args)));
#else
		[&]<usize...I>(std::index_sequence<I...>)LAMBDA_INLINE
		{
			(..., CTOR_CONST_AT(m_ptr + I, FWD(args)));
		}(std::make_index_sequence<sizeof...(args)>{});
#endif
	}

	constexpr Sso()noexcept
	{
		if consteval { CTOR_CONST_AT(&m_array); }
		m_ptr = __builtin_addressof(m_array[0]);
		m_size = {};
	}

	constexpr Sso(Sso&& rhs)noexcept
	{
		m_size = rhs.m_size;
		if (rhs.sso())
		{
			if consteval { CTOR_CONST_AT(&m_array); }
			m_ptr = __builtin_addressof(m_array[0]);
			Algorithm::copy_ctor(rhs, __builtin_addressof(m_array[0]));
		}
		else
		{
			m_ptr = rhs.m_ptr;
			m_cap = rhs.m_cap;
			if consteval { CTOR_CONST_AT(&rhs.m_array); }
			rhs.m_ptr = __builtin_addressof(rhs.m_array[0]);
		}
	}

	constexpr Sso(const Sso& rhs)noexcept :Sso{ rhs.data(), rhs.size() } {}

	constexpr Sso($LinearContainerOf<T> auto&& container)noexcept requires(!$FixSized<decltype(container)>) : Sso{container.data(), container.size()} {}

	constexpr Sso($LinearContainerOf<T> auto&& container)noexcept requires($FixSized<decltype(container)>&& static_size<decltype(container)> <= sso_cap)
	: m_size{ static_size<decltype(container)> }
	{
		if consteval { CTOR_CONST_AT(&m_array); }
		m_ptr = __builtin_addressof(m_array[0]);
#if VARIADIC_STRUCTURED_BINDING
		static constexpr auto [...I] { iota<static_size<decltype(container)>> };
		(..., CTOR_CONST_AT(__builtin_addressof(m_array[constant<I>]), FWD(container)[constant<I>]));
#else
		[&]<usize...I>(std::index_sequence<I...>)LAMBDA_INLINE
		{
			(..., CTOR_CONST_AT(__builtin_addressof(m_array[I]), FWD(container)[I]));
		}(std::make_index_sequence<static_size<decltype(container)>>{});
#endif
	}

	constexpr Sso($LinearContainerOf<T> auto&& container)noexcept requires($FixSized<decltype(container)>&& static_size<decltype(container)> > sso_cap)
	: m_ptr{ ::allocate<T>(static_size<decltype(container)>) }, m_size{ static_size<decltype(container)> }, m_cap{ static_size<decltype(container)> }
	{
#if VARIADIC_STRUCTURED_BINDING
		static constexpr auto [...I] { iota<static_size<decltype(container)>> };
		(..., CTOR_CONST_AT(m_ptr + constant<I>, FWD(container)[constant<I>]));
#else
		[&]<usize...I>(std::index_sequence<I...>)LAMBDA_INLINE
		{
			(..., CTOR_CONST_AT(m_ptr + I, FWD(container)[I]));
		}(std::make_index_sequence<static_size<decltype(container)>>{});
#endif
	}

	constexpr Sso(auto const* pData, usize s)noexcept requires(requires{T{ *pData }; }) : m_size{ s }
	{
		if (s <= sso_cap)
		{
			if consteval { CTOR_CONST_AT(&m_array); }
			m_ptr = __builtin_addressof(m_array[0]);
			Algorithm::copy_ctor(pData, s, __builtin_addressof(m_array[0]));
		}
		else
		{
			m_ptr = ::allocate<T>(s);
			m_cap = s;
			Algorithm::copy_ctor(pData, s, m_ptr);
		}
	}

	template<class U>
	constexpr Sso(const U* pBegin, const U* pEnd)noexcept requires(requires{T{ *pBegin }; }) :Sso{ pBegin, static_cast<usize>(pEnd - pBegin) } {}

	constexpr Sso& operator=(Sso&& rhs)noexcept
	{
		destruct();
		CTOR_CONST_AT(this, MOV(rhs));
		return *this;
	}

	constexpr Sso& operator=(const Sso& rhs)noexcept
	{
		destruct();
		CTOR_CONST_AT(this, rhs);
		return *this;
	}

	constexpr Sso& operator=($LinearContainerOf<T> auto&& container)noexcept
	{
		destruct();
		CTOR_CONST_AT(this, FWD(container));
		return *this;
	}

	constexpr ~Sso()noexcept{destruct();}

	constexpr usize size()const noexcept { return m_size; }
	constexpr bool empty()const noexcept { return m_size == 0; }
	constexpr usize capacity()const noexcept { return sso() ? sso_cap : m_cap; }

	template<class Self>constexpr Like<Self, T> front(this Self&& self) noexcept { return *FWD(self).data(); }
	template<class Self>constexpr Like<Self, T> back(this Self&& self) noexcept { return *(FWD(self).end() - 1); }
	template<class Self>constexpr Like<Self, T> operator[](this Self&& self, usize index) noexcept { return FWD(self).data()[index]; }
	template<class Self>constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return self.m_ptr; }
	template<class Self>constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return self.data(); }
	template<class Self>constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + self.size(); }

	constexpr void reserve(usize s)noexcept
	{
		if (s > capacity())
			reserve_impl(s);
	}

	//s <= m_size
	constexpr void truncate(usize s)noexcept { m_size = s; }
	constexpr void clear()noexcept { truncate({}); }

	constexpr void pop_back()noexcept { --m_size; }

	constexpr void reserve_impl(usize x)noexcept
	{
		T* newPtr{ ::allocate<T>(x) };
		Algorithm::copy_ctor(m_ptr, m_size, newPtr);
		destruct();
		m_ptr = newPtr;
		m_cap = x;
	}

	constexpr void push_back(T c)noexcept
	{
		const usize newSize{ size() + 1 };
		const usize oldCap{ capacity() };
		if (newSize > oldCap)
			reserve_impl(oldCap * 2);
		CTOR_CONST_AT(data() + m_size, c);
		m_size = newSize;
	}

	constexpr void unchecked_push_back(T c)noexcept
	{
		CTOR_CONST_AT(data() + m_size, c);
		++m_size;
	}
	constexpr void append(T c)noexcept { push_back(c); }

	constexpr void append(const T* pData, usize s)noexcept
	{
		const usize newSize{ size() + s };
		const usize oldCap{ capacity() };
		if (newSize > oldCap)
			reserve_impl(Algorithm::max(oldCap * 2, newSize));
		Algorithm::copy_ctor(pData, s, data() + m_size);
		m_size = newSize;
	}

	constexpr void append(const T* pBegin, const T* pEnd)noexcept { append(pBegin, static_cast<usize>(pEnd - pBegin)); }
	
	constexpr void append($LinearContainer auto&& view)noexcept { append(view.data(), view.size()); }

	constexpr void insert(usize index, const T* pBegin, usize s)noexcept
	{
		const usize newSize{ m_size + s };
		const usize oldCap{ capacity() };
		if (newSize > oldCap)
		{
			const usize newCap{ Algorithm::max(oldCap * 2, newSize) };
			T* newPtr{ ::allocate<T>(newCap) };
			for (usize i{}; i != index; ++i)
				CTOR_CONST_AT(newPtr + i, MOV(m_ptr[i]));
			for (usize i{ index }; i != m_size; ++i)
				CTOR_CONST_AT(newPtr + i + s, MOV(m_ptr[i]));
			for (usize i{}; i!= s;++i)
				CTOR_CONST_AT(newPtr + index + i, pBegin[i]);
			destruct();
			m_ptr = newPtr;
			m_cap = newCap;
		}
		else if(s != 0)
		{
			T* pInsertLast{ m_ptr + index + s - 1 };
			for (T* p{ end() + s - 1 }; p != pInsertLast; --p)
				CTOR_CONST_AT(p, MOV(*(p - s)));
			for (T* p{ m_ptr + index }, * pInsertEnd{ m_ptr + index + s }; p != pInsertEnd; ++p, ++pBegin)
				CTOR_CONST_AT(p, MOV(*pBegin));
		}
		m_size = newSize;
	}
	constexpr void insert(usize index, const T* pBegin, const T* pEnd)noexcept { insert(index, pBegin, pEnd - pBegin); }

	constexpr void insert(usize index, T x)noexcept
	{
		const usize newSize{ m_size + 1 };
		const usize oldCap{ capacity() };
		if (newSize > oldCap)
		{
			const usize newCap{ oldCap * 2 };
			T* newPtr{ ::allocate<T>(newCap) };
			for (usize i{}; i != index; ++i)
				CTOR_CONST_AT(newPtr + i, MOV(m_ptr[i]));
			for (usize i{ index }; i != m_size; ++i)
				CTOR_CONST_AT(newPtr + i + 1, MOV(m_ptr[i]));
			CTOR_CONST_AT(newPtr + index, x);
			destruct();
			m_ptr = newPtr;
			m_cap = newCap;
		}
		else
		{
			T* pInsert{ m_ptr + index };
			for (T* p{ end() }; p != pInsert; --p)
				CTOR_CONST_AT(p, MOV(*(p - 1)));
			CTOR_CONST_AT(pInsert, x);
		}
		m_size = newSize;
	}

	constexpr Sso& operator+=(T c)noexcept { append(c); return *this; }

	constexpr Sso& operator+=($LinearContainer auto&& rhs)noexcept { append(rhs.data(), rhs.size()); return *this; }

	constexpr StringView<const T> substr(usize pos, usize count)const noexcept { return { begin() + pos, begin() + pos + count }; }
	constexpr StringView<const T> substr(usize pos)const noexcept { return { begin() + pos, end() }; }
};
template<class Stream, $Printable<Stream> T, usize S>
constexpr Stream& operator<<(Stream& os, const Sso<T, S>& s)noexcept
{
	if constexpr ($Same<typename Stream::char_type, T>)
		os.write(s.data(), s.size());
	else if constexpr (sizeof(T) == 1 && sizeof(typename Stream::char_type) == 2)
		Utf8::decode(s, [&](const u32 codePoint)LAMBDA_INLINE{ Utf16::encode(codePoint,[&](const c16 unit)LAMBDA_INLINE{os.put(unit); }); });
	else if constexpr (sizeof(T) == 2 && sizeof(typename Stream::char_type) == 1)
		Utf16::decode(s, [&](const u32 codePoint)LAMBDA_INLINE{ Utf8::encode(codePoint,[&](const c8 unit)LAMBDA_INLINE{os.put(unit); }); });
	return os;
}
template<class T>using BasicString = Sso<T, 16>;
using String8 = BasicString<c8>;
using String16 = BasicString<c16>;
using String32 = BasicString<c32>;

template<class T0, usize S0, class T1, usize S1>
requires($Same<T0,T1>&&S0==S1)
constexpr bool operator==(const Sso<T0,S0>& lhs, const Sso<T1,S1>& rhs)noexcept
{
	if (lhs.size() != rhs.size())return false;
	const T0* pL{ lhs.data() };
	const T0* pLEnd{ pL+lhs.size() };
	const T0* pR{ rhs.data() };
	for (; pL != pLEnd; ++pL, ++pR)
		if (*pL != *pR)
			return false;
	return true;
}

INLINE constexpr auto operator+($String auto&& l, $String auto&& r)noexcept-> Sso<ElementOf<decltype(l)>>
requires
(
	$Same<ElementOf<decltype(l)>, ElementOf<decltype(r)>>&&
	!($FixSized<decltype(l)> && $FixSized<decltype(r)>)
)
{
	Sso<ElementOf<decltype(l)>> result{ uninitialized, l.size() + r.size() };
	Algorithm::copy_ctor(l, result.data());
	Algorithm::copy_ctor(r, result.data() + l.size());
	return result;
}

template<$LinearContainer Container>
constexpr Sso<ElementOf<Container>> nullTerminated(Container&& s)noexcept//TODO:Sso&& optimize
{
	Sso<ElementOf<Container>> result{ uninitialized, s.size() + 1 };
	Algorithm::copy_ctor(s, result.data());
	result.back() = ElementOf<Container>{};
	return result;
}

//tests--------------------------------------------------------------------------------------------------------------------------
//copy ctor empty
STATIC_ASSERT([]static
{
	StringView8 view{ "" };
	if (view.size() != 0)return false;
	String8 str{ view };
	if (str.size() != 0)return false;
	return view == str;
}());

//copy ctor short + move ctor + move assign
STATIC_ASSERT([]static
{
	StringView8 view{ "0123" };
	if (view.size() != 4)return false;
	String8 str{ view };
	if (str.size() != 4)return false;
	if (view != str)return false;
	String8 str2{ MOV(str) };
	if (view != str2)return false;
	str = MOV(str2);
	if (view != str)return false;
	return true;
}());

//copy ctor long + move ctor + move assign
STATIC_ASSERT([]static
{
	StringView8 view{ "012345678901234567890123456789" };
	if (view.size() != 30)return false;
	String8 str{ view };
	if (str.size() != 30)return false;
	if (view != str)return false;
	String8 str2{ MOV(str) };
	if (view != str2)return false;
	str = MOV(str2);
	if (view != str)return false;
	return true;
}());

//ctor fixed short
STATIC_ASSERT([]static
{
	FixedString fs{ "01234" };
	String8 str{ fs };
	if (StringView8{ fs } != StringView8{ str })return false;
	return true;
}());

//ctor fixed long
STATIC_ASSERT([]static
{
	FixedString fs{ "012345678901234567890123456789" };
	String8 str{ fs };
	if (StringView8{ fs } != StringView8{ str })return false;
	return true;
}());

//reserve + unchecked_push_back
STATIC_ASSERT([]static
{
	String8 s;
	s.reserve(100);
	if (s.capacity() != 100)return false;
	for (const auto i : iota<100>)
		s.unchecked_push_back(i);
	if (s.size() != 100)return false;
	if (s.capacity() != 100)return false;
	return true;
}());

//uninitialized ctor short + reserve
STATIC_ASSERT([]static
{
	String8 str2{ uninitialized, 4 };
	if (str2.size() != 4)return false;
	for (auto& x : str2)
		x = '1';
	str2.reserve(100);
	if (str2.size() != 4)return false;
	for (auto& x : str2)
		x = '1';
	return true;
}());

//uninitialized ctor long + push_back
STATIC_ASSERT([]static
{
	String8 str{ uninitialized, 100 };
	if (str.size() != 100)return false;
	for (auto& x : str)
		CTOR_CONST_AT(&x, '1');
	for (usize i{ 100 }; i != 1000; ++i)
	{
		str.push_back('1');
		if (str.size() != i + 1)
			return false;
	}
	for (auto& x : str)
		if (x != '1')
			return false;
	return true;
}());

//push_back
STATIC_ASSERT([]static
{
	String8 str{};
	if (str[2])return false;
	if (str.size() != 0)return false;
	for (usize i{}; i != 1; ++i)
	{
		str.push_back(static_cast<c8>(i));
		if (str.size() != i + 1)
			return false;
	}
	return true;
}());

//copy assign short->short view
STATIC_ASSERT([]static
{
	constexpr StringView8 short_view{ "0123" };
	constexpr StringView8 long_view{ "45678" };
	String8 str{ short_view };
	if (str != short_view)return false;
	str = long_view;
	if (str != long_view)return false;
	return true;
}());

//copy assign short->short
STATIC_ASSERT([]static
{
	constexpr StringView8 short_view{ "0123" };
	constexpr StringView8 long_view{ "45678" };
	String8 str{ short_view };
	if (str != short_view)return false;
	const String8 str2{ long_view };
	str = str2;
	if (str != long_view)return false;
	return true;
}());

//copy assign long->long view
STATIC_ASSERT([]static
{
	constexpr StringView8 short_view{ "012345678901234567890123456789" };
	constexpr StringView8 long_view{ "qwertyuiopasdfghjklzxcvbnmm" };
	String8 str{ short_view };
	if (str != short_view)return false;
	str = long_view;
	if (str != long_view)return false;
	return true;
}());

//copy assign long->long
STATIC_ASSERT([]static
{
	constexpr StringView8 short_view{ "qwertyuiopasdfghjklzxcvbnmm" };
	constexpr StringView8 long_view{ "012345678901234567890123456789" };
	String8 str{ short_view };
	if (str != short_view)return false;
	const String8 str2{ long_view };
	str = str2;
	if (str != long_view)return false;
	return true;
}());

//copy assign short->long view
STATIC_ASSERT([]static
{
	constexpr StringView8 short_view{ "0123" };
	constexpr StringView8 long_view{ "012345678901234567890123456789" };
	String8 str{ short_view };
	if (str != short_view)return false;
	str = long_view;
	if (str != long_view)return false;
	return true;
}());

//copy assign short->long
STATIC_ASSERT([]static
{
	constexpr StringView8 short_view{ "0123" };
	constexpr StringView8 long_view{ "012345678901234567890123456789" };
	String8 str{ short_view };
	if (str.size() != 4)return false;
	const String8 str2{ long_view };
	str = str2;
	if (str != long_view)return false;
	return true;
}());

//copy assign long->short view
STATIC_ASSERT([]static
{
	constexpr StringView8 short_view{ "0123" };
	constexpr StringView8 long_view{ "012345678901234567890123456789" };
	String8 str{ long_view };
	if (str != long_view)return false;
	str = short_view;
	if (str != short_view)return false;
	return true;
}());

//copy assign long->short
STATIC_ASSERT([]static
{
	constexpr StringView8 short_view{ "0123" };
	constexpr StringView8 long_view{ "012345678901234567890123456789" };
	String8 str{ long_view };
	if (str != long_view)return false;
	const String8 str2{ short_view };
	str = str2;
	if (str != short_view)return false;
	return true;
}());

STATIC_ASSERT([]static
{
	String8 str{ '0','1','2','3' };
	if (str != StringView8{ "0123" })return false;
	return true;
}());

STATIC_ASSERT([]static
{
	String8 str{ '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9' };
	if (str != StringView8{ "012345678901234567890123456789" })return false;
	return true;
}());

STATIC_ASSERT([]static
{
	String8 str{ '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9' };
	str.insert(0, "1", usize{1});
	if (str != StringView8{ "1012345678901234567890123456789" })return false;
	return true;
}());
STATIC_ASSERT([]static
{
	String8 str{ '0','1','2','3' };
	str.insert(0, "", usize{});
	if (str != StringView8{ "0123" })return false;
	return true;
}());
STATIC_ASSERT([]static
{
	String8 str{ '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9' };
	str.insert(0, "", usize{});
	if (str != StringView8{ "012345678901234567890123456789" })return false;
	return true;
}());
//replace
STATIC_ASSERT([]static
{
	String8 str{ '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9' };
	str.replace<"0123">(StringView8{"0000"});
	if (str != StringView8{ "000045678900004567890000456789" })return false;
	return true;
}());

STATIC_ASSERT([]static
{
	String8 str{ '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9' };
	str.replace<"0123">(StringView8{""});
	if (str != StringView8{ "456789456789456789" })return false;
	return true;
}());

STATIC_ASSERT([]static
{
	String8 str{ '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9' };
	str.replace<"0123">(StringView8{"9877654"});
	if (str != StringView8{ "987765445678998776544567899877654456789" })return false;
	return true;
}());

STATIC_ASSERT([]static
{
	String8 str{ '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9' };
	str.replace<"9">(StringView8{"9877654"});
	if (str != StringView8{ "012345678987765401234567898776540123456789877654" })return false;
	return true;
}());