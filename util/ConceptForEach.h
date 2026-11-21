#pragma once
#include "ConceptCArray.h"
template<class T>
concept $ForEach = $CArray<T> || requires(const T x) { x.begin() != x.end(); };