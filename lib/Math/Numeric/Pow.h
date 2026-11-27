#pragma once
#include "../../Util/Concepts/Floating.h"
#include "../../Util/Pure.h"
#include "../MathConstant.h"
#include "../Musl.h"
namespace Math
{
	//base^exp
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T pow(T base, T exp)noexcept
	{
		/* */if constexpr($Same<f32, T>)return musl::powf(base, exp);
		else if constexpr($Same<f64, T>)return musl::pow(base, exp);
		else static_assert(0);
	}

	//e^exp
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T exp(T exp)noexcept
	{
		/* */if constexpr($Same<f32, T>)return musl::expf(exp);
		else if constexpr($Same<f64, T>)return musl::exp(exp);
		else static_assert(0);
	}

	//2^exp
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T exp2(T exp)noexcept
	{
		/* */if constexpr($Same<f32, T>)return musl::exp2f(exp);
		else if constexpr($Same<f64, T>)return musl::exp2(exp);
		else static_assert(0);
	}
}