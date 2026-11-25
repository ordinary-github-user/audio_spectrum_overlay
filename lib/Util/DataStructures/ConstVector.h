#pragma once
#include <array>
#include <utility>
#include "../BasicTypes.h"
#include "../Compact.h"
#include "../Compiler.h"
#include "../Concepts/NotCvref.h"
#include "../Concepts/Printable.h"
#include "../Macro/Consteval.h"
#include "../Macro/If.h"
#include "../Macro/MovForward.h"
#include "../Macro/PlacementNew.h"
#include "../Traits/Like.h"
#if defined(__cpp_trivial_union)
	#warning todo: constexpr StaticVector
#endif
template<class, usize>struct ConstVector;

template<$NotCvref T, usize N>
requires(N > 0)
struct ConstVector<T, N>
{
	using Index = Compact<N>;
	using Element = T;

	T m_data[N]{};
	Index m_size{};

	constexpr ConstVector($Constructible<T> auto&&...args)noexcept requires(sizeof...(args) <= N) : m_data{FWD(args)...},  m_size{ sizeof...(args) }{}
	constexpr ConstVector(const std::array<T, N>& vec)noexcept : m_data{ vec }, m_size{ N } {}
	constexpr Index size()const noexcept { return m_size; }
	constexpr bool empty()const noexcept { return !m_size; }
	static CONSTEVAL Index capacity() noexcept { return N; }

	template<class Self>constexpr Like<Self, T> front(this Self&& self) noexcept { return FWD(self).m_data[0]; }
	template<class Self>constexpr Like<Self, T> back(this Self&& self) noexcept { return FWD(self).m_data[self.m_size - 1]; }
	template<class Self>constexpr Like<Self, T> operator[](this Self&& self, Index index) noexcept { return FWD(self).m_data[index]; }
	template<class Self>constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return __builtin_addressof(self.m_data[0]); }
	template<class Self>constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return __builtin_addressof(self.m_data[0]); }
	template<class Self>constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + self.m_size; }

	constexpr void clear()noexcept { m_size = {}; }

	constexpr void pop_back()noexcept { --m_size; }
	constexpr void push_back(const T& entry)noexcept { m_data[m_size++] = entry; }
	constexpr void push_back(T&& entry)noexcept { m_data[m_size++] = MOV(entry); }
	constexpr void emplace_back(auto&&...args)noexcept { m_data[m_size++] = T{FWD(args)...}; }

	constexpr void erase(T* p)noexcept
	{
		if constexpr (!__is_trivially_destructible(T))
			p->~T();
		--m_size;
		for (const T* pLast{ end() }; p != pLast; ++p)
			CTOR_CONST_AT(p, MOV(p[1]));
	}

	constexpr void erase(Index index)noexcept { erase(&data()[index]); }
};

template<$NotCvref T>
struct ConstVector<T, 0>
{
	using Element = T;

	constexpr ConstVector()noexcept {}

	static constexpr u8 size() noexcept { return 0; }
	static constexpr u8 capacity() noexcept { return 0; }

	constexpr const T* data()const noexcept { return nullptr; }
	constexpr const T* begin()const noexcept { return nullptr; }
	constexpr const T* end()const noexcept { return nullptr; }
};

template<class>struct IsConstVectorImpl { static constexpr bool value{}; };
template<class T, usize N>struct IsConstVectorImpl<ConstVector<T, N>> { static constexpr bool value{ true }; };
template<class T>concept $ConstVector = IsConstVectorImpl<std::remove_cvref_t<T>>::value;

template<$NotCvref Stream, $Printable<Stream> T, usize N>
constexpr Stream& operator<<(Stream& os, const ConstVector<T, N>& array)noexcept
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