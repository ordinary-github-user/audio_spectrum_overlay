#pragma once
#include <array>
#include "../../Util/Compiler.h"
#include "../../Util/Concepts/Floating.h"
#include "../../Util/Pure.h"
#include "../Musl.h"
#include "../SinCos.h"
#include "../Sleef.h"
namespace Math::Trig
{
	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T sin(T x)noexcept
	{
		/* */if constexpr ($Same<f32, T>)return xsinf(x);
		else if constexpr ($Same<f64, T>)return xsin(x);
		else static_assert(0);
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T cos(T x)noexcept
	{
		/* */if constexpr ($Same<f32, T>)return xcosf(x);
		else if constexpr ($Same<f64, T>)return xcos(x);
		else static_assert(0);
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T tan(T x)noexcept
	{
		/* */if constexpr ($Same<f32, T>)return xtanf(x);
		else if constexpr ($Same<f64, T>)return xtan(x);
		else static_assert(0);
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T asin(T x)noexcept
	{
		/* */if constexpr ($Same<f32, T>)return xasinf(x);
		else if constexpr ($Same<f64, T>)return xasin(x);
		else static_assert(0);
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T acos(T x)noexcept
	{
		/* */if constexpr ($Same<f32, T>)return xacosf(x);
		else if constexpr ($Same<f64, T>)return xacos(x);
		else static_assert(0);
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T atan(T x)noexcept
	{
		/* */if constexpr ($Same<f32, T>)return xatanf(x);
		else if constexpr ($Same<f64, T>)return xatan(x);
		else static_assert(0);
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr T atan2(T x, T y)noexcept
	{
		/* */if constexpr ($Same<f32, T>)return xatan2f(x, y);
		else if constexpr ($Same<f64, T>)return xatan2(x, y);
		else static_assert(0);
	}

	template<$Floating T>
	NO_SIDE_EFFECTS INLINE constexpr SinCos<T> sincos(T x)noexcept
	{
		/* */if constexpr ($Same<f32, T>)return xsincosf(x);
		else if constexpr ($Same<f64, T>)return xsincos(x);
		else static_assert(0);
	}
}