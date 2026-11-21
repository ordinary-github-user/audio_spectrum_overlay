#pragma once
#include "ConceptSame.h"
template<class T, class U>
concept $WrapperOf = !$Same<std::remove_cvref_t<T>, U> && sizeof(std::remove_cvref_t<T>) == sizeof(U) && requires(std::remove_cvref_t<T>& x) { static_cast<U&>(x); };

template<class T, class...Ts>
concept $WrapperAnyOf = ($WrapperOf<T, Ts> || ...);