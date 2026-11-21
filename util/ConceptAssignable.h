#pragma once
#include <utility>
template<class T, class U>
concept $Assignable = requires(U& u, const T x){u = x; };