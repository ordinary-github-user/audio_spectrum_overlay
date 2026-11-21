#pragma once
#include "ConceptSame.h"
template<class T>
concept $Optional = requires(T x)
{
	static_cast<bool>(x);
	x.reset();
	*x;
	{ x.has_value() } -> $Same<bool>;
};