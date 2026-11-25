#pragma once
#include "Same.h"
template<class T>
concept $NotCvref = $Same<T, std::remove_cvref_t<T>>;