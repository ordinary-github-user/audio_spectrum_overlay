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
		if constexpr($Same<f32, T>)
			return musl::powf(base, exp);
		else
			return musl::pow(base, exp);
	}

	//e^exp
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T exp(T exp)noexcept
	{
#if 1
		if constexpr ($Same<f32, T>)
			return musl::expf(exp);
		else
			return musl::exp(exp);
#else
		if constexpr($Same<f32, T>)
			return musl::powf(euler_number<T>, exp);
		else
			return musl::pow(euler_number<T>, exp);
#endif
	}

	//2^exp
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T exp2(T exp)noexcept
	{
		if constexpr ($Same<f32, T>)
			return musl::exp2f(exp);
		else
			return musl::exp2(exp);
	}
}