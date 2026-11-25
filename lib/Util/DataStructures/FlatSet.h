#pragma once
#include "../Algorithm/Sort.h"
#include "../Compare.h"
#include "../Concepts/Sig.h"
#include "GrowableArray.h"
template<class T>
struct UnorderedFlatSet
{
	GrowableArray<T> m_data;

	constexpr void insert_unsafe(auto&&...args)noexcept { m_data.emplace_back(FWD(args)...); }

	constexpr bool insert(auto&& x)noexcept
	{
		for (const T& e : m_data)
			if (e == x)
				return false;
		m_data.emplace_back(FWD(x));
		return true;
	}
	constexpr bool contains(auto&& x)noexcept 
	{
		for (const T& e : m_data)
			if (x == e)
				return true;
		return false;
	}
	constexpr bool erase(auto&& target)noexcept
	{
		for(T& e: m_data)
			if (e == target)
			{
				m_data.erase(&e);
				return true;
			}
		return false;
	}
	constexpr usize size()const noexcept { return m_data.size(); }
	constexpr bool empty()const noexcept { return m_data.empty(); }
	constexpr void reserve(usize s)	noexcept { m_data.reserve(s); }
	constexpr const T& operator[](usize index)const noexcept { return m_data[index]; }
	constexpr const T* data()const noexcept { return m_data.begin(); }
	constexpr const T* begin()const noexcept { return m_data.begin(); }
	constexpr const T* end()const noexcept { return m_data.end(); }
};

template<class Stream, $Printable<Stream> T>
INLINE constexpr Stream& operator<<(Stream& os, const UnorderedFlatSet<T>& set)noexcept
{
	os << set.m_data;
	return os;
}

template<class T, $Sig<bool(const T, const T)> Compare = Less>
struct OrderedFlatSet
{
	GrowableArray<T> m_data;

	constexpr bool is_sorted()noexcept { return Algorithm::is_sorted(m_data, Compare{}); }

	constexpr void sort()noexcept { Algorithm::sort(m_data, Compare{}); }
	constexpr void stable_sort()noexcept { Algorithm::stable_sort(m_data, Compare{}); }

	constexpr void insert_unsorted(auto&&...args)noexcept { m_data.emplace_back(FWD(args)...); }

	constexpr bool insert(auto&& x)noexcept
	{
		const T* first{ m_data.begin() };
		const T* last{ m_data.end() };
		first = std::lower_bound(first, last, FWD(x), Compare{});
		if (first != last && !Compare{}(FWD(x), *first))
			return false;
		m_data.insert(first, FWD(x));
		return true;
	}
	constexpr bool unchecked_insert(auto&& x)noexcept
	{
		const T* first{ m_data.begin() };
		const T* last{ m_data.end() };
		first = std::lower_bound(first, last, FWD(x), Compare{});
		if (first != last && !Compare{}(FWD(x), *first))
			return false;
		m_data.unchecked_insert(first, FWD(x));
		return true;
	}
	constexpr bool contains(auto&& x)noexcept
	{
#if 1
		const T* first{ m_data.begin() };
		const T* last{ m_data.end() };
		first = std::lower_bound(first, last, FWD(x), Compare{});
		return first != last && !Compare{}(FWD(x), *first);
#else
		return std::binary_search(m_data.begin(), m_data.end(), FWD(x), Compare{});
#endif
	}
	constexpr bool erase(auto&& x)noexcept
	{
		T* first{ m_data.begin() };
		T* last{ m_data.end() };
		first = std::lower_bound(first, last, FWD(x), Compare{});
		if (first == last || Compare{}(FWD(x), *first))
			return false;
		m_data.erase(first);
		return true;
	}
	constexpr void reserve(usize s)	noexcept { m_data.reserve(s); }
	constexpr usize size()const noexcept { return m_data.size(); }
	constexpr bool empty()const noexcept { return m_data.empty(); }
	constexpr const T& operator[](usize index)const noexcept { return m_data[index]; }
	constexpr const T* begin()const noexcept { return m_data.begin(); }
	constexpr const T* end()const noexcept { return m_data.end(); }
};

template<class Stream, $Printable<Stream> T, $Sig<bool(const T, const T)> Compare>
INLINE constexpr Stream& operator<<(Stream& os, const OrderedFlatSet<T, Compare>& set)noexcept
{
	os << set.m_data;
	return os;
}

//tests------------------------------------------------------------------------------------------------------------------
STATIC_ASSERT([]
{
	OrderedFlatSet<int> set;
	if (set.size() != 0)return false;
	if (!set.is_sorted())return false;

	if (!set.insert(0))return false;
	if (set.size() != 1)return false;
	if (!set.contains(0))return false;
	if (!set.is_sorted())return false;

	if (set.insert(0))return false;
	if (set.size() != 1)return false;
	if (!set.contains(0))return false;
	if (!set.is_sorted())return false;

	if (!set.insert(1))return false;
	if (set.size() != 2)return false;
	if (!set.contains(0))return false;
	if (!set.contains(1))return false;
	if (!set.is_sorted())return false;

	if (!set.insert(2))return false;
	if (set.size() != 3)return false;
	if (!set.contains(0))return false;
	if (!set.contains(1))return false;
	if (!set.contains(2))return false;
	if (!set.is_sorted())return false;

	if (!set.insert(-10))return false;
	if (set.size() != 4)return false;
	if (!set.contains(-10))return false;
	if (!set.contains(0))return false;
	if (!set.contains(1))return false;
	if (!set.contains(2))return false;
	if (!set.is_sorted())return false;

	if (!set.insert(-5))return false;
	if (set.size() != 5)return false;
	if (!set.contains(-10))return false;
	if (!set.contains(-5))return false;
	if (!set.contains(0))return false;
	if (!set.contains(1))return false;
	if (!set.contains(2))return false;
	if (!set.is_sorted())return false;

	if (set.erase(-999))return false;
	if (!set.is_sorted())return false;

	if (!set.erase(-5))return false;
	if (set.size() != 4)return false;
	if (!set.contains(-10))return false;
	if (!set.contains(0))return false;
	if (!set.contains(1))return false;
	if (!set.contains(2))return false;
	if (!set.is_sorted())return false;

	if (set.erase(-5))return false;
	if (!set.is_sorted())return false;

	if (!set.erase(-10))return false;
	if (set.size() != 3)return false;
	if (!set.contains(0))return false;
	if (!set.contains(1))return false;
	if (!set.contains(2))return false;
	if (!set.is_sorted())return false;

	if (!set.erase(2))return false;
	if (set.size() != 2)return false;
	if (!set.contains(0))return false;
	if (!set.contains(1))return false;
	if (!set.is_sorted())return false;

	if (!set.erase(0))return false;
	if (set.size() != 1)return false;
	if (!set.contains(1))return false;
	if (!set.is_sorted())return false;

	if (!set.erase(1))return false;
	if (set.size() != 0)return false;
	if (!set.is_sorted())return false;

	return true;
}());