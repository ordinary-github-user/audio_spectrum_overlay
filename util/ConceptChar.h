#pragma once
#include "BasicTypes.h"
#include "ConceptSame.h"
#include <utility>
template<class T>concept $Char = $AnyOf<std::remove_cvref_t<T>, char, wchar_t, char8_t, char16_t, char32_t>;
template<class T, usize Size>concept $CharOfSize = $Char<T> && sizeof(T) == Size;
template<class T, usize...Sizes>concept $CharOfSizeAny = $Char<T> && ((sizeof(T) == Sizes) || ...);