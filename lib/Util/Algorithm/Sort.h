#pragma once
#include <algorithm>
#include "../Assert.h"
#include "../Concepts/FixSized.h"
#include "../Concepts/HasBeginEnd.h"
#include "../Concepts/Iterator.h"
#include "../Concepts/LinearContainer.h"
#include "../Concepts/Sig.h"
#include "../DataStructures/StaticVector.h"
#include "../ElementOf.h"
#include "../IndexTypeOf.h"
#include "../Inline.h"
#include "../Iota.h"
#include "../Macro/MovForward.h"
#include "../StaticAssert.h"
#include "../StaticError.h"
namespace Algorithm
{
	template<$HasBeginEnd Container>
	INLINE constexpr bool is_sorted(Container&& container, $Sig<bool(const ElementOf<Container>, const ElementOf<Container>)> auto&& compare)noexcept
	{
		return std::is_sorted(container.begin(), container.end(), FWD(compare));
	}

	template<$HasBeginEnd Container>
	INLINE constexpr void sort(Container&& container, $Sig<bool(const ElementOf<Container>, const ElementOf<Container>)> auto&& compare)noexcept
	{
		std::sort(container.begin(), container.end(), FWD(compare));
	}

	template<$LinearContainer Container>
	INLINE constexpr void bubble_sort(Container&& container, $Sig<bool(const ElementOf<Container>, const ElementOf<Container>)> auto&& compare)noexcept
	{
		using Index = IndexOf<Container>;
		for (Index iEnd{ container.size() }; iEnd > 1;)
		{
			bool swapped{};
			Index newIEnd;											// furthest index where a swap occurred this pass
			for (Index i{ 1 }; i != iEnd; ++i) 
				if (FWD(compare)(container[i], container[i - 1]))	// right < left?
				{
					std::swap(container[i - 1], container[i]);
					swapped = true;
					newIEnd = i;									// everything after i is already in place
				}
			if (!swapped) break;									// already sorted
			iEnd = newIEnd;											// shrink the unsorted tail
		}
	}

	template<$LinearContainer Container>
	INLINE constexpr void quicksort(Container&& container, $Sig<bool(const ElementOf<Container>, const ElementOf<Container>)> auto&& compare)noexcept
	{
		using Element = ElementOf<Container>;
		using Iter = std::remove_cvref_t<decltype(container.begin())>;
		struct Entry { Iter begin, end; };
		if constexpr ($FixSized<Container>)
		{
			if constexpr (static_size<Container> > 1)
			{
				StaticVector<Entry, static_size<Container>> stack;
				stack.emplace_back(container.begin(), container.end());
				while (!stack.empty())
				{
					const auto [begin, end] {stack.back()};
					stack.pop_back();
					if (end - begin > 1)
					{
						const Iter iPivot{ end - 1 };
						Iter i{ begin };
						if constexpr (__is_trivially_copyable(Element) && sizeof(Element) <= sizeof(void*))
						{
							const Element pivot{ *iPivot };
							for (Iter j{ begin }; j != iPivot; ++j)
								if (compare(*j, pivot))
								{
									std::swap(*i, *j);
									++i;
								}
						}
						else
						{
							for (Iter j{ begin }; j != iPivot; ++j)
								if (compare(*j, *iPivot))
								{
									std::swap(*i, *j);
									++i;
								}
						}
						std::swap(*i, *iPivot);
						stack.emplace_back(begin, i);
						stack.emplace_back(i + 1, end);
					}
				}
			}
		}
		else STATIC_ERROR(not implemented);
	}

	template<class Iter>
	constexpr void quicksort_recursive(Iter begin, Iter end, $Sig<bool(const std::remove_cvref_t<decltype(*std::declval<Iter>())>, const std::remove_cvref_t<decltype(*std::declval<Iter>())>)> auto&& compare)noexcept
	requires(requires{++begin < end;})
	{
		using Element = std::remove_cvref_t<decltype(*std::declval<Iter>())>;
		if (end - begin > 1)
		{
			const Iter iPivot{ end - 1 };
			Iter i{ begin };
			if constexpr (__is_trivially_copyable(Element) && sizeof(Element) <= sizeof(void*))
			{
				const Element pivot{ *iPivot };
				for (Iter j{ begin }; j != iPivot; ++j)
					if (compare(*j, pivot))
					{
						std::swap(*i, *j);
						++i;
					}
			}
			else
			{
				for (Iter j{ begin }; j != iPivot; ++j)
					if (compare(*j, *iPivot))
					{
						std::swap(*i, *j);
						++i;
					}
			}
			std::swap(*i, *iPivot);
			quicksort_recursive(begin, i, FWD(compare));
			quicksort_recursive(i + 1, end, FWD(compare));
		}
	}

	template<$LinearContainer Container>
	INLINE constexpr void quicksort_recursive(Container&& container, $Sig<bool(const ElementOf<Container>, const ElementOf<Container>)> auto&& compare)noexcept
	{
		quicksort_recursive(container.begin(), container.end(), FWD(compare));
	}

	template<$HasBeginEnd Container>
	INLINE constexpr void stable_sort(Container&& container, $Sig<bool(const ElementOf<Container>, const ElementOf<Container>)> auto&& compare)noexcept
	{
		if consteval
		{
			bubble_sort(FWD(container), FWD(compare));
		}
		else
		{
			std::stable_sort(container.begin(), container.end(), FWD(compare));
		}
	}

	//after sortAByB, both a and b are sorted 
	template<$LinearContainer ContainerA, $LinearContainer ContainerB>
	INLINE constexpr void sortAByB(ContainerA&& a, ContainerB&& b, $Sig<bool(const ElementOf<ContainerB>, const ElementOf<ContainerB>)> auto&& compare) noexcept
	{
		//using A = ElementOf<ContainerA>;
		//using B = ElementOf<ContainerB>;
		using IndexA = IndexOf<ContainerA>;
		using IndexB = IndexOf<ContainerB>;
		STATIC_ASSERT($UInt<IndexA>);
		STATIC_ASSERT($UInt<IndexB>);
		using Index = std::conditional_t<sizeof(IndexA) <= sizeof(IndexB), IndexA, IndexB>;
		ASSERT(a.size() == b.size());
		[pA = a.data(), pB = b.data(), &compare](this auto&& self, Index l, Index r)//quicksort [l,r)
		{
			if (r <= l + Index{ 1 })return;
			//move pivot (middle) to end-1
			const Index mid{ static_cast<Index>(l + (r - l) / 2) };
			std::swap(pB[mid], pB[r - Index{ 1 }]);
			std::swap(pA[mid], pA[r - Index{ 1 }]);
			// partition [l..r-1) around pivot pB[r-1]
			Index store{ l };
			for (Index k{ l }; k < r - Index{ 1 }; ++k)
				if (compare(pB[k], pB[r - Index{ 1 }]))
				{
					std::swap(pB[k], pB[store]);
					std::swap(pA[k], pA[store]);
					++store;
				}
			//put pivot into its final place
			std::swap(pB[store], pB[r - Index{ 1 }]);
			std::swap(pA[store], pA[r - Index{ 1 }]);
			self(l, store);
			self(store + Index{ 1 }, r);
		}({}, static_cast<Index>(a.size()));
	}
}

#include <array>
#include <string_view>
namespace details::SortTests
{
	struct S
	{
		i32 i;
		std::string_view s;
	};

	STATIC_ASSERT([]static
	{
		std::array arr{ S{2,"a"},S{2,"b"},S{1,"c"},S{4,"d"},S{1,"e"},S{4,"f"},S{5,"g"},S{2,"h"},S{3,"j"},S{4,"k"} };
		Algorithm::bubble_sort(arr, [](const S& l, const S& r)static{return l.i < r.i; });
		return arr[0].i == 1 && arr[0].s == "c"
			&& arr[1].i == 1 && arr[1].s == "e"
			&& arr[2].i == 2 && arr[2].s == "a"
			&& arr[3].i == 2 && arr[3].s == "b"
			&& arr[4].i == 2 && arr[4].s == "h"
			&& arr[5].i == 3 && arr[5].s == "j"
			&& arr[6].i == 4 && arr[6].s == "d"
			&& arr[7].i == 4 && arr[7].s == "f"
			&& arr[8].i == 4 && arr[8].s == "k"
			&& arr[9].i == 5 && arr[9].s == "g";
	}());
}