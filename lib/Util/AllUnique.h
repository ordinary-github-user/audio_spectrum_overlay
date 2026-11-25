#pragma once
#include <utility>
#include "BasicTypes.h"
#include "Concepts/Same.h"
#include "ElementAt.h"
template<auto...X>
constexpr bool all_unique{ [] <usize...I>(std::index_sequence<I...>)static
{
	return(...&&[]static -> bool
	{
		static constexpr auto i{I / sizeof...(X)};
		static constexpr auto j{I % sizeof...(X)};
		if constexpr (i == j)return true;
		else
		{
			static constexpr bool e { requires{element_at<i>(X...) == element_at<j>(X...); } };
			static constexpr bool ne{ requires{element_at<i>(X...) != element_at<j>(X...); } };
			if constexpr (e && ne)
			{
				static constexpr bool result0{ element_at<i>(X...) != element_at<j>(X...) };
				static constexpr bool result1{ !(element_at<i>(X...) == element_at<j>(X...)) };
				static_assert(result0 == result1, "bad comparison");
				return result0;
			}
			else if constexpr (ne)return element_at<i>(X...) != element_at<j>(X...);
			else if constexpr (e)return !(element_at<i>(X...) == element_at<j>(X...));
			else
			{
				static_assert(!$Same<std::remove_cvref_t<decltype(element_at<i>(X...))>, std::remove_cvref_t<decltype(element_at<j>(X...))>>, "same type but no operator==/!=");
				return true;
			}
		}
	}());
}(std::make_index_sequence<sizeof...(X) * sizeof...(X)>{})};