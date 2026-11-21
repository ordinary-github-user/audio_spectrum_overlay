#pragma once
#include <utility>
namespace details
{
	template<class>
	struct IndexTypeOfImpl;

	template<class T>
	requires(requires(const T x) { x.size(); }) 
	struct IndexTypeOfImpl<T> 
	{
		using type = std::remove_cvref_t<decltype(std::declval<T>().size())>; 
	};
}
template<class T>
using IndexOf = details::IndexTypeOfImpl<std::remove_cvref_t<T>>::type;