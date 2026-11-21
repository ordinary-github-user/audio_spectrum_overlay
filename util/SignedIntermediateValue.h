#pragma once
#include "Inline.h"
#include "BasicTypes.h"
template<class T, bool positive>
requires(__is_trivially_copyable(T) && sizeof(T) <= sizeof(u64))
struct SignedIntermediateValue
{
	T value;
};
template<class T>INLINE constexpr T operator+(T l, SignedIntermediateValue<T, 0> r)noexcept { return l - r.value; }
template<class T>INLINE constexpr T operator+(T l, SignedIntermediateValue<T, 1> r)noexcept { return l + r.value; }
template<class T>INLINE constexpr T operator+(SignedIntermediateValue<T, 1> l, T r)noexcept { return l.value + r; }
template<class T>INLINE constexpr T operator+(SignedIntermediateValue<T, 0> l, T r)noexcept { return r - l.value; }
template<class T>INLINE constexpr T operator+(SignedIntermediateValue<T, 1> l, SignedIntermediateValue<T, 0> r)noexcept { return l.value - r.value; }
template<class T>INLINE constexpr T operator+(SignedIntermediateValue<T, 0> l, SignedIntermediateValue<T, 1> r)noexcept { return r.value - l.value; }

template<bool positive, class T>
INLINE constexpr T signed_value(T value)noexcept
{
	if constexpr (positive)return value;
	else return -value;
}