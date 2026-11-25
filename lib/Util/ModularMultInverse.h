#pragma once
#include "BasicTypes.h"
#include "Concepts/Integral.h"
#include "IntegerOverflow.h"
namespace Math
{
	template<$UInt T>
	constexpr T modular_inverse(T x)noexcept
	{
		T y{ x };
		for (;;)
		{
			const T z{ Overflow::mullo(y, static_cast<T>(T{ 2 } - Overflow::mullo(x, y))) };
			if (z == y)return z;
			y = z;
		}
	}
}