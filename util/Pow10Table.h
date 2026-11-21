#pragma once
#include "Assert.h"
#include "BasicTypes.h"
#include "Compact.h"
#include "ConceptIntegral.h"
#include <array>
template<$UInt T, usize N>
constexpr std::array<T, N> pow10_table{ [] static
{
	std::array<T, N> result;
	result[0] = 1;
	for (Compact<N> i{ 1 }; i != N; ++i)
	{
		result[i] = result[i - 1] * 10;
		ASSERT(result[i]);
	}
	return result;
}() };