#pragma once
#include "ConceptSame.h"
#include <utility>
template<class T>concept $Bool = $Same<std::remove_cvref_t<T>, bool>;
template<class T>concept $Int = std::is_integral_v<std::remove_cvref_t<T>> && !$Same<std::remove_cvref_t<T>, bool>;
template<class T>concept $UInt = std::is_integral_v<std::remove_cvref_t<T>> && std::is_unsigned_v<std::remove_cvref_t<T>> && !$Same<std::remove_cvref_t<T>, bool>;
template<class T>concept $SInt = std::is_integral_v<std::remove_cvref_t<T>> && std::is_signed_v<std::remove_cvref_t<T>>;