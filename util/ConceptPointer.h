#pragma once
#include <type_traits>
template<class T>concept $Pointer = std::is_pointer_v<T>;