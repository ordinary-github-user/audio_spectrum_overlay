#pragma once
#include <type_traits>
template<class T>concept $Aggregate = __is_aggregate(std::remove_cvref_t<T>);