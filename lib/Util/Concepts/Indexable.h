#pragma once
#include <utility>
#include "../BasicTypes.h"
template<class T, class U>
concept $IndexableBy = requires(const T container, const U index) { container[index]; };
template<class T>
concept $Indexable = $IndexableBy<T, u8>;