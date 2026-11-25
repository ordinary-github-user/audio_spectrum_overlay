#pragma once
#include <utility>
template<class T>concept $Floating = std::is_floating_point_v<std::remove_cvref_t<T>>;