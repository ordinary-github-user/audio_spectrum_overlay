#pragma once
#include "Compiler.h"
#include "ConceptFloating.h"
#include "Musl.h"
#include "Pure.h"
#include "SinCos.h"
#include "Sleef.h"
#include <array>
namespace Math::Trig
{
	template<$AnyOf<f32, f64> T>
	NO_SIDE_EFFECTS INLINE constexpr T sin(T x)noexcept
	{
		if constexpr ($Same<f32, T>)
			return xsinf(x);
		else
			return xsin(x);
	}

	template<$AnyOf<f32, f64> T>
	NO_SIDE_EFFECTS INLINE constexpr T cos(T x)noexcept
	{
		if constexpr ($Same<f32, T>)
			return xcosf(x);
		else
			return xcos(x);
	}

	template<$AnyOf<f32, f64> T>
	NO_SIDE_EFFECTS INLINE constexpr T tan(T x)noexcept
	{
		if constexpr ($Same<f32, T>)
			return xtanf(x);
		else
			return xtan(x);
	}

	template<$AnyOf<f32, f64> T>
	NO_SIDE_EFFECTS INLINE constexpr T asin(T x)noexcept
	{
		if constexpr ($Same<f32, T>)
			return xasinf(x);
		else
			return xasin(x);
	}

	template<$AnyOf<f32, f64> T>
	NO_SIDE_EFFECTS INLINE constexpr T acos(T x)noexcept
	{
		if constexpr ($Same<f32, T>)
			return xacosf(x);
		else
			return xacos(x);
	}

	template<$AnyOf<f32, f64> T>
	NO_SIDE_EFFECTS INLINE constexpr T atan(T x)noexcept
	{
		if constexpr ($Same<f32, T>)
			return xatanf(x);
		else
			return xatan(x);
	}

	template<$AnyOf<f32, f64> T>
	NO_SIDE_EFFECTS INLINE constexpr T atan2(T x, T y)noexcept
	{
		if constexpr ($Same<f32, T>)
			return xatan2f(x, y);
		else
			return xatan2(x, y);
	}

	//template<$AnyOf<f32, f64> T>
	//INLINE constexpr void sincos(T x, T& s, T& c)noexcept
	//{
	//	if constexpr ($Same<f32, T>)
	//		xsincosf(x, &s, &c);
	//	else
	//		xsincos(x, &s, &c);
	//}

	template<$AnyOf<f32, f64> T>
	INLINE constexpr SinCos<T> sincos(T x)noexcept
	{
		if constexpr ($Same<f32, T>)
			return xsincosf(x);
		else
			return xsincos(x);
	}
}