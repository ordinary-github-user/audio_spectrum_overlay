#pragma once
#include "../Util/Inline.h"
template<class T>
struct SinCos
{
	T s, c;
	INLINE constexpr SinCos operator*(auto r)const noexcept { return { s * r,c * r }; }
	INLINE constexpr SinCos operator/(auto r)const noexcept { return { s / r,c / r }; }
	INLINE constexpr SinCos& operator/=(auto r) noexcept { s /= r; c /= r; return *this; }
	INLINE constexpr SinCos& operator*=(auto r) noexcept { s *= r; c *= r; return *this; }
};