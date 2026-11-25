#pragma once
#include <utility>
template<class T, bool C>
using ConditionalConst = typename std::conditional<C, typename std::add_const<T>::type, T>::type;