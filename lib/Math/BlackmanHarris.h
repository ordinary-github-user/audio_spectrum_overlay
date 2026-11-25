#pragma once
#include "../Util/Concepts/Floating.h"
#include "../Util/Inline.h"
#include "../Util/Pure.h"
#include "DegreeRadian.h"
namespace Math
{
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T blackmanHarris(T fraction) noexcept
	{
		static constexpr T a0{ T{35875} / T{100000} };
		static constexpr T a1{ T{48829} / T{100000} };
		static constexpr T a2{ T{14128} / T{100000} };
		static constexpr T a3{ T{ 1168} / T{100000} };
		return a0
			-  a1 * cos(Rad<T>::template circle<T{1}> * fraction)
			+  a2 * cos(Rad<T>::template circle<T{2}> * fraction)
			-  a3 * cos(Rad<T>::template circle<T{3}> * fraction);
	}
}