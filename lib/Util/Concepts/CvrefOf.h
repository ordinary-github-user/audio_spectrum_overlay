#pragma once
#include <utility>
#include "Same.h"
template<class T, class U>
concept $CvrefOf = $Same<std::remove_cvref_t<T>, U>;
template<class T, class...Ts>
concept $CvrefAnyOf = ($Same<std::remove_cvref_t<T>, Ts> || ...);