#pragma once
#include <utility>
#include "Concepts/CArray.h"
namespace details
{
	template<class>
	struct ElementOfImpl;
	template<class T>
	requires(requires{*std::declval<const std::remove_cvref_t<T>>().begin(); })
	struct ElementOfImpl<T>
	{
		using type = std::remove_cvref_t<decltype(*std::declval<const std::remove_cvref_t<T>>().begin())>;
	};
	template<$CArray T>
	struct ElementOfImpl<T> 
	{
		using type = std::remove_cvref_t<decltype(*std::declval<T>())>; 
	};
}
template<class T>
using ElementOf = typename details::ElementOfImpl<T>::type;