#pragma once
#include <bit>
#include <limits>
#include "BasicTypes.h"
#include "Compiler.h"
#include "Concepts/Floating.h"
#include "Inline.h"
#include "SizedInt.h"
#include "StaticAssert.h"
template<$Floating T>
struct IEEE754
{
	using U = UnsignedBytes<sizeof(T)>;
	static constexpr u8 mantissa_bits{ std::numeric_limits<T>::digits - 1 };
	static constexpr u8 exp_bits{ sizeof(T) * 8 - mantissa_bits - 1 };
	static constexpr U mantissa_mask{ (U{ 1 } << mantissa_bits) - 1 };
	static constexpr U exp_mask{ (U{ 1 } << exp_bits) - 1 };
	U mantissa : mantissa_bits;
	U exp : exp_bits;
	U sign : 1;
#if CLANG//clang bug: constexpr bit_cast involving bit-field is not yet supported
	INLINE constexpr IEEE754(U u) noexcept
	{
		if consteval
		{
			mantissa = u & mantissa_mask;
			exp = (u >> mantissa_bits) & exp_mask;
			sign = (u >> (sizeof(T) * 8 - 1)) & 1;
		}
		else { reinterpret_cast<U&>(*this) = u; }
	}
	INLINE constexpr IEEE754(T f) noexcept : IEEE754{ __builtin_bit_cast(U, f) } {}
	INLINE constexpr explicit operator T() const noexcept
	{
		if consteval { return __builtin_bit_cast(T, U{ mantissa } | (U{ exp } << mantissa_bits) | (U{ sign } << (mantissa_bits + exp_bits))); }
		else { return reinterpret_cast<const T&>(*this); }
	}
	INLINE friend constexpr bool operator==(IEEE754, IEEE754) noexcept = default;
	INLINE friend constexpr bool operator!=(IEEE754, IEEE754) noexcept = default;
#else
	INLINE constexpr IEEE754(U u) noexcept : IEEE754{ std::bit_cast<IEEE754>(u) } {}//msvc bug: error C2760: syntax error: '__builtin_bit_cast' was unexpected here; expected '}'
	INLINE constexpr IEEE754(T f) noexcept : IEEE754{ std::bit_cast<IEEE754>(f) } {}//msvc bug: error C2760: syntax error: '__builtin_bit_cast' was unexpected here; expected '}'
	INLINE constexpr explicit operator T() const noexcept { return __builtin_bit_cast(T, *this); }
	INLINE friend constexpr bool operator==(IEEE754 l, IEEE754 r) noexcept { return __builtin_bit_cast(U, l) == __builtin_bit_cast(U, r); }
	INLINE friend constexpr bool operator!=(IEEE754 l, IEEE754 r) noexcept { return __builtin_bit_cast(U, l) != __builtin_bit_cast(U, r); }
#endif

	INLINE constexpr bool is_negative() const noexcept { return sign == 1; }
	INLINE constexpr bool is_finite() const noexcept { return exp != exp_mask; }
	INLINE constexpr bool is_nan() const noexcept { return (exp == exp_mask) && (mantissa != 0); }
	INLINE constexpr bool is_inf() const noexcept { return (exp == exp_mask) && (mantissa == 0); }
	INLINE constexpr bool is_zero() const noexcept { return (exp == 0) && (mantissa == 0); }

	static constexpr T infinity{ std::numeric_limits<T>::infinity() };
	static constexpr T nan{ std::numeric_limits<T>::quiet_NaN() };
	static constexpr T most_positive{ std::numeric_limits<T>::max() };
	static constexpr T most_negative{ std::numeric_limits<T>::lowest() };
};
template<$Floating T>
IEEE754(T) -> IEEE754<T>;

STATIC_ASSERT(sizeof(IEEE754<f32>) == sizeof(f32));
STATIC_ASSERT(sizeof(IEEE754<f64>) == sizeof(f64));
STATIC_ASSERT(IEEE754{ IEEE754<f32>::infinity } != IEEE754{ -IEEE754<f32>::infinity });
STATIC_ASSERT(IEEE754{ IEEE754<f32>::nan } == IEEE754{ IEEE754<f32>::nan });
STATIC_ASSERT(f64{ IEEE754{ 123.456 } } == 123.456);
STATIC_ASSERT(f64{ IEEE754{ -123.456 } } == -123.456);
STATIC_ASSERT(f32{ IEEE754{ 123.456f } } == 123.456f);
STATIC_ASSERT(f32{ IEEE754{ -123.456f } } == -123.456f);