#pragma once
#include "../Concepts/LinearContainer.h"
#include "../Concepts/NotCvref.h"
#include "../Concepts/Same.h"
#include "../Concepts/Sig.h"
#include "../ElementOf.h"
#include "../IndexTypeOf.h"
#include "../Inline.h"
#include "../Macro/MovForward.h"
#include "../Macro/PlacementNew.h"
#include "../StaticAssert.h"
#include "../Traits/Like.h"
template<class Compare>
constexpr bool isHeap($LinearContainer auto&& heap)noexcept
requires requires{{Compare::operator()(heap[heap.size()], heap[heap.size()])}->$Same<bool>; }
{
	using Index = IndexOf<decltype(heap)>;
	for (Index i{ heap.size() }; i-->1;)
		if (const Index iParent{ Index(Index(i - 1) / 2) }; Compare::operator()(heap[i], heap[iParent]))
			return false;
	return true;
}

template<$LinearContainer Container, $Sig<bool(const ElementOf<Container>, const ElementOf<Container>)> Compare>
struct Heap
{
	using T = ElementOf<Container>;
	using Index = IndexOf<Container>;
	Container m_data;
private:
	INLINE static constexpr Index indexParent(Index index)noexcept { return Index(Index(index - 1) / 2); }
	INLINE static constexpr Index indexChildL(Index index)noexcept { return Index(Index(index * 2) + 1); }
	INLINE static constexpr Index indexChildR(Index index)noexcept { return Index(Index(index * 2) + 2); }
	INLINE constexpr void bubbleDown(Index index)noexcept
	{
		for (;;)
		{
			const Index iChildL{ indexChildL(index) };
			if (iChildL >= m_data.size()) break;
			Index indexSwap{ index };
			if (Compare::operator()(m_data[iChildL], m_data[indexSwap])) indexSwap = iChildL;
			const Index iChildR{ indexChildR(index) };
			if (iChildR < m_data.size() && Compare::operator()(m_data[iChildR], m_data[indexSwap])) indexSwap = iChildR;
			if (indexSwap == index) break;
			std::swap(m_data[index], m_data[indexSwap]);
			index = indexSwap;
		}
	}
public:
	INLINE constexpr void clear()noexcept requires requires{m_data.clear(); } { m_data.clear(); }
	INLINE constexpr auto capacity()const noexcept requires requires{m_data.capacity(); } { return m_data.capacity(); }
	INLINE constexpr Index size()const noexcept { return m_data.size(); }
	INLINE constexpr bool empty()const noexcept { return m_data.empty(); }
	template<class Self>INLINE constexpr Like<Self, T> root(this Self&& self)noexcept { return *self.m_data.data(); }
	template<class Self>INLINE constexpr Like<Self, T> operator[](this Self&& self, Index index) noexcept { return FWD(self).m_data[index]; }
	template<class Self>INLINE constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return self.m_data.data(); }
	template<class Self>INLINE constexpr auto begin(this Self&& self) noexcept { return self.m_data.begin(); }
	template<class Self>INLINE constexpr auto end(this Self&& self) noexcept { return self.m_data.end(); }

	constexpr Heap()noexcept {}
	constexpr Heap($LinearContainerOf<T> auto&& container)noexcept : m_data{ FWD(container) }
	{
		for (Index i{ m_data.size() / 2 }; i--;)
			bubbleDown(i);
	}

	constexpr void emplace(auto&&...args)noexcept
	requires requires{ m_data.emplace_back(FWD(args)...); }
	{
		m_data.emplace_back(FWD(args)...);
		for (Index i{ Index(m_data.size() - 1) }; i;)
			if (const Index iParent{ indexParent(i) }; Compare::operator()(m_data[i], m_data[iParent]))
			{
				std::swap(m_data[i], m_data[iParent]);
				i = iParent;
			}
			else break;
	}
	constexpr void pop()noexcept
	requires requires{ m_data.clear(); m_data.pop_back(); }
	{
		if (size() == 1) [[unlikely]]
			m_data.clear();
		else
		{
			if constexpr (requires{m_data.front() = MOV(m_data.back()); })
				m_data.front() = MOV(m_data.back());
			else
			{
				DTOR(m_data.front());
				CTOR(m_data.front()) { MOV(m_data.back()) };
			}
			m_data.pop_back();
			bubbleDown({});
		}
	}
	constexpr void replaceRoot(auto&&...args)noexcept requires requires{T{FWD(args)...}; }
	{
		DTOR(m_data.front());
		CTOR(m_data.front()) { FWD(args)... };
		bubbleDown({});
	}
	constexpr void remove(T* p)noexcept { return remove(Index(p - m_data.data())); }
	constexpr void remove(Index index)noexcept
	requires requires{ m_data.pop_back(); }
	{
		if (Index(index + 1) == size()) [[unlikely]]
			m_data.pop_back();
		else
		{
			m_data[index] = MOV(m_data.back());
			m_data.pop_back();
			if (Index iParent; index && Compare::operator()(m_data[index], m_data[iParent = indexParent(index)]))//bubble up
				for (;;)
				{
					std::swap(m_data[index], m_data[iParent]);
					if (!iParent)return;
					index = iParent;
					iParent = indexParent(index);
					if (!Compare::operator()(m_data[index], m_data[iParent]))return;
				}
			else bubbleDown(index);
		}
	}
	template<class Stream>
	friend constexpr Stream& operator<<(Stream& os, const Heap& heap)noexcept
	{
		os << heap.m_data;
		return os;
	}
};

#include <algorithm>
#include <array>
#include <vector>
#include "../BasicTypes.h"
#include "../Compare.h"
STATIC_ASSERT([]static
{
	using Compare = Less;
	using T = i32;
	static constexpr auto test{ [] <T...x> ()static
	{
		using Container = std::array<T, sizeof...(x)>;
		static constexpr Container original{x...};
		static constexpr Container sorted{ [] static
		{
			Container result{original};
			std::sort(result.begin(), result.end(), Compare{});
			return result;
		}()};
		{
			Heap<Container, Compare> heap{ original };
			if(!isHeap<Compare>(heap))return false;
			std::sort(heap.begin(), heap.end(), Compare{});
			if(sorted != heap.m_data)return false;
		}
		{
			Heap<std::vector<T>, Compare> heap;//todo: std::inplace_vector
			heap.m_data.reserve(sizeof...(x));
			(..., heap.emplace(x));
			if (!isHeap<Compare>(heap))return false;
			Container output;
			while(!heap.empty())
			{
				output[sizeof...(x) - heap.size()] = MOV(heap.root());
				heap.pop();
			}
			if (!std::equal(sorted.begin(), sorted.end(), output.begin()))return false;
		}
		return true;
	} };
	if (!test.template operator()<>())return false;
	if (!test.template operator()<0>())return false;
	if (!test.template operator()<0,1>())return false;
	if (!test.template operator()<1,0>())return false;
	if (!test.template operator()<1,2,0>())return false;
	if (!test.template operator()<0,3,1>())return false;
	if (!test.template operator()<2,0,1>())return false;
	if (!test.template operator()<1,4,4,0>())return false;
	if (!test.template operator()<4,1,4,3>())return false;
	if (!test.template operator()<1,3,4,2>())return false;
	if (!test.template operator()<0,1,3,2>())return false;
	if (!test.template operator()<2,1,1,2,0>())return false;
	if (!test.template operator()<0,3,0,2,2>())return false;
	if (!test.template operator()<4,2,0,5,3>())return false;
	if (!test.template operator()<4,0,3,0,4>())return false;
	if (!test.template operator()<2,5,4,2,4>())return false;
	if (!test.template operator()<1,5,0,0,5,1>())return false;
	if (!test.template operator()<6,2,0,6,1,6>())return false;
	if (!test.template operator()<0,3,2,3,5,6>())return false;
	if (!test.template operator()<2,1,2,2,1,5>())return false;
	if (!test.template operator()<2,5,5,5,0,4>())return false;
	if (!test.template operator()<5,1,4,5,1,1>())return false;
	if (!test.template operator()<7,6,4,3,5,0,3>())return false;
	if (!test.template operator()<0,5,6,4,1,3,5>())return false;
	if (!test.template operator()<3,7,6,7,2,4,2>())return false;
	if (!test.template operator()<3,4,6,6,5,3,2>())return false;
	if (!test.template operator()<7,1,0,1,2,2,6>())return false;
	if (!test.template operator()<1,6,6,7,4,0,1>())return false;
	if (!test.template operator()<4,5,1,4,6,2,7>())return false;
	if (!test.template operator()<0,4,8,2,8,1,4,8>())return false;
	if (!test.template operator()<3,2,5,2,8,8,0,5>())return false;
	if (!test.template operator()<7,0,1,5,4,3,0,3>())return false;
	if (!test.template operator()<1,1,7,1,8,2,2,7>())return false;
	if (!test.template operator()<8,2,4,8,6,3,8,3>())return false;
	if (!test.template operator()<4,6,5,7,8,7,1,3>())return false;
	if (!test.template operator()<3,1,5,0,8,3,3,0>())return false;
	if (!test.template operator()<1,0,3,1,0,5,1,8>())return false;
	if (!test.template operator()<3,4,7,3,8,2,9,9,7>())return false;
	if (!test.template operator()<3,7,6,3,1,1,6,5,6>())return false;
	if (!test.template operator()<6,7,0,1,0,6,5,1,3>())return false;
	if (!test.template operator()<3,3,8,7,2,6,2,4,7>())return false;
	if (!test.template operator()<3,1,7,8,1,0,8,0,1>())return false;
	if (!test.template operator()<3,2,6,7,7,3,6,0,2>())return false;
	if (!test.template operator()<6,0,6,4,7,4,6,8,7>())return false;
	if (!test.template operator()<2,3,4,3,0,9,8,0,5>())return false;
	if (!test.template operator()<0,0,9,7,8,8,2,0,8>())return false;
	return true;
}());