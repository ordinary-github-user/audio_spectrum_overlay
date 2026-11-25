#pragma once
#include <utility>
template<class T>concept $Arithmetic = std::is_arithmetic_v<std::remove_cvref_t<T>>;