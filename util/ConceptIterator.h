#pragma once
#include "ConceptCvrefOf.h"
template<class T, class U>
concept $IteratorOf = requires(std::remove_cvref_t<T> iterator) { { *(++iterator) }->$CvrefOf<U>; };