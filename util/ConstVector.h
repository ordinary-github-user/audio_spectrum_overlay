#pragma once
#include "BasicTypes.h"
#include "Compact.h"
#include "Compiler.h"
#include "ConceptNotCvref.h"
#include "ConceptPrintable.h"
#include "Consteval.h"
#include "Like.h"
#include "MacroIf.h"
#include "MacroMovForward.h"
#include "MacroPlacementNew.h"
#include <array>
#include <utility>
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

	std::array<T, N> m_data{};
	Index m_size{};

	IF(MSVC, constexpr, CONSTEVAL) ConstVector()noexcept = default;
	IF(MSVC, constexpr, CONSTEVAL) ConstVector(const std::array<T, N>& vec)noexcept : m_data{ vec }, m_size{ N } {}
	IF(MSVC, constexpr, CONSTEVAL) Index size()const noexcept { return m_size; }
	IF(MSVC, constexpr, CONSTEVAL) bool empty()const noexcept { return !m_size; }
	static CONSTEVAL Index capacity() noexcept { return N; }

	template<class Self>IF(MSVC, constexpr, CONSTEVAL) Like<Self, T> front(this Self&& self) noexcept { return FWD(self).m_data[0]; }
	template<class Self>IF(MSVC, constexpr, CONSTEVAL) Like<Self, T> back(this Self&& self) noexcept { return FWD(self).m_data[self.m_size - 1]; }
	template<class Self>IF(MSVC, constexpr, CONSTEVAL) Like<Self, T> operator[](this Self&& self, Index index) noexcept { return FWD(self).m_data[index]; }
	template<class Self>IF(MSVC, constexpr, CONSTEVAL) LikePtr<Self, T> data(this Self&& self) noexcept { return __builtin_addressof(self.m_data[0]); }
	template<class Self>IF(MSVC, constexpr, CONSTEVAL) LikePtr<Self, T> begin(this Self&& self) noexcept { return __builtin_addressof(self.m_data[0]); }
	template<class Self>IF(MSVC, constexpr, CONSTEVAL) LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + self.m_size; }

	IF(MSVC, constexpr, CONSTEVAL) void clear()noexcept { m_size = {}; }

	IF(MSVC, constexpr, CONSTEVAL) void pop_back()noexcept { --m_size; }
	IF(MSVC, constexpr, CONSTEVAL) void push_back(const T& entry)noexcept { m_data[m_size++] = entry; }
	IF(MSVC, constexpr, CONSTEVAL) void push_back(T&& entry)noexcept { m_data[m_size++] = MOV(entry); }
	IF(MSVC, constexpr, CONSTEVAL) void emplace_back(auto&&...args)noexcept { m_data[m_size++] = T{FWD(args)...}; }

	IF(MSVC, constexpr, CONSTEVAL) void erase(T* p)noexcept
	{
		if constexpr (!__is_trivially_destructible(T))
			p->~T();
		--m_size;
		for (const T* pLast{ end() }; p != pLast; ++p)
			CTOR_CONST_AT(p, MOV(p[1]));
	}

	IF(MSVC, constexpr, CONSTEVAL) void erase(Index index)noexcept { erase(&data()[index]); }
};

template<$NotCvref T>
struct ConstVector<T, 0>
{
	using Element = T;

	IF(MSVC, constexpr, CONSTEVAL) ConstVector()noexcept {}

	static IF(MSVC, constexpr, CONSTEVAL) u8 size() noexcept { return 0; }
	static IF(MSVC, constexpr, CONSTEVAL) u8 capacity() noexcept { return 0; }

	IF(MSVC, constexpr, CONSTEVAL) const T* data()const noexcept { return nullptr; }
	IF(MSVC, constexpr, CONSTEVAL) const T* begin()const noexcept { return nullptr; }
	IF(MSVC, constexpr, CONSTEVAL) const T* end()const noexcept { return nullptr; }
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