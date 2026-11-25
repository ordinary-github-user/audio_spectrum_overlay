#pragma once
#include "../ElementOf.h"
#include "Char.h"
#include "LinearContainer.h"
#include "Same.h"
template<class T>concept $String = $LinearContainer<T> && $Char<ElementOf<T>>;
template<class T, class Char>concept $StringOf = $LinearContainer<T> && $Char<Char> && $Same<ElementOf<T>, Char>;
template<class T, usize Size>concept $StringOfCharSize = $LinearContainer<T> && $Char<ElementOf<T>> && sizeof(ElementOf<T>) == Size;
template<class T, usize...Sizes>concept $StringOfCharSizeAny = $LinearContainer<T> && $Char<ElementOf<T>> && ((sizeof(ElementOf<T>) == Sizes) || ...);