#pragma once
#include "BasicTypes.h"
#include "ConceptIntegral.h"
#include "Consteval.h"
#include "Inline.h"
template<auto V, $UInt Index>
struct RepeatingImpl
{
	Index m_count;
	struct EndIterator;
	struct Iterator
	{
		Index i;
		INLINE constexpr bool operator!=(EndIterator) noexcept { return i; }
		INLINE constexpr Iterator& operator+=(auto r) noexcept requires requires{i -= r; } { i -= r; return *this; }
		INLINE constexpr Iterator& operator++() noexcept { --i; return *this; }
		INLINE constexpr auto operator*() const noexcept { return V; }
	};
	struct EndIterator
	{
		INLINE friend constexpr Index operator-(EndIterator, Iterator r) noexcept { return r.i; }
	};
	INLINE constexpr Index size() const noexcept { return m_count; }
	INLINE constexpr Iterator begin() const noexcept { return Iterator{ m_count }; }
	INLINE constexpr Iterator rbegin() const noexcept { return Iterator{ m_count }; }
	INLINE static CONSTEVAL EndIterator end() noexcept { return {}; }
	INLINE static CONSTEVAL EndIterator rend() noexcept { return {}; }
};
template<auto V, $UInt Index>
INLINE constexpr RepeatingImpl<V, Index> repeating(Index count)noexcept { return { count }; }
STATIC_ASSERT([]static
{
	static constexpr auto test{ [] <auto V>($UInt auto count)static
	{
		const auto r{repeating<V>(count)};
		using Count = decltype(count);
		if (r.end() - r.begin() != count)return false;
		if (r.rend() - r.rbegin() != count)return false;
		Count c0{};
		for (auto i{r.begin()}; i != r.end(); ++i)
		{
			if (*i != V)return false;
			++c0;
		}
		if (c0 != count)return false;
		Count c1{};
		for (auto i{r.rbegin()}; i != r.rend(); ++i)
		{
			if (*i != V)return false;
			++c1;
		}
		if (c1 != count)return false;
		return true;
	} };
	if (!test.template operator()<123.456>(u8{ 10 }))return false;
	if (!test.template operator()<123.456>(u8{ 0 }))return false;
	if (!test.template operator()<123.456>(u16{ 0x100 }))return false;
	return true;
}());