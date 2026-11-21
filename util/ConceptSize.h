#pragma once
#include "BasicTypes.h"
template<class T, usize Size>concept $Size = sizeof(T) == Size;
template<class T, usize...Size>concept $SizeAnyOf = ((sizeof(T) == Size) || ...);