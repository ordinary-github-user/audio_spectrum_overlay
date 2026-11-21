#pragma once
#include <utility>
template<class T>
concept $PassByValue = __is_trivially_copyable(std::remove_cvref_t<T>) && sizeof(void*) >= sizeof(std::remove_cvref_t<T>);

template<class T>
concept $PassByRef = !$PassByValue<T>;