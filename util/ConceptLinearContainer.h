#pragma once
#include "ConceptCvrefOf.h"
#include "ConceptIntegral.h"
#include "ConceptPointer.h"
#include "ConceptSame.h"
#include <utility>
template<class T>
concept $LinearContainer = requires(const std::remove_cvref_t<T> container)
{
	{ *container.begin() };
	{ *container.end() };
	{ *container.data() };
	{ container.data() } -> $Pointer;
	{ container.size() } -> $UInt;
	{ container[container.size()] };
};

template<class T, class E>
concept $LinearContainerOf = requires(const std::remove_cvref_t<T> container)
{
	{ *container.begin() };
	{ *container.end() };
	{ *container.data() } -> $CvrefOf<std::remove_cvref_t<E>>;
	{ container.data() } -> $Pointer;
	{ container.size() } -> $UInt;
	{ container[container.size()] }-> $CvrefOf<std::remove_cvref_t<E>>;
};

template<class T, class...Es>
concept $LinearContainerAnyOf = ($LinearContainerOf<T, Es> || ...);