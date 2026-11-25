#pragma once
#include <utility>
template<class T>concept $Reference = std::is_reference_v<T>;
template<class T>concept $NotReference = !std::is_reference_v<T>;