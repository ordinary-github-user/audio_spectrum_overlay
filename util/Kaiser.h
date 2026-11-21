#pragma once
#include "AlgorithmMinMax.h"
#include "ConceptFloating.h"
#include "Inline.h"
#include "Pure.h"
#include "Sqrt.h"
namespace Math
{
	template<$Floating T, T beta = T(8.6), usize maximumBesselTerm = sizeof(T) * 8>
	NO_SIDE_EFFECTS INLINE constexpr T kaiser(T fraction)noexcept
	{
		static constexpr auto modifiedBesselI0{ [] <usize...I>(std::index_sequence<I...>) static
		{
			return[](T x) STATIC_LAMBDA_INLINE
			{
				T sum{ 1 };
				T term{ 1 };
				const T xSquaredQuarter{ x * x * T(0.25) };
				(..., ((term *= xSquaredQuarter / T((I + 1) * (I + 1))), (sum += term)));
				return sum;
			};
		}(std::make_index_sequence<maximumBesselTerm>{}) };
		static constexpr T inverseModifiedBesselI0Beta{ 1 / modifiedBesselI0(beta) };
		const T centeredFraction{ fraction + fraction - 1 };
		const T radius{ Math::sqrt(Algorithm::max(T{}, 1 - centeredFraction * centeredFraction)) };
		return modifiedBesselI0(beta * radius) * inverseModifiedBesselI0Beta;
	}
}