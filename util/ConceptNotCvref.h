#pragma once
#include "ConceptSame.h"
template<class T>
concept $NotCvref = $Same<T, std::remove_cvref_t<T>>;