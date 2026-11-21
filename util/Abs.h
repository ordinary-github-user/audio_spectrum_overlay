#pragma once
#include "ConceptFloating.h"
#include "ConceptIntegral.h"
#include "MathBuiltin.h"
#include "Signbit.h"
#include "StaticAssert.h"
#include <limits>
namespace Math
{
	template<class T>
	NO_SIDE_EFFECTS INLINE constexpr T abs(const T x) noexcept
	{
		STATIC_ASSERT(!$Int<T>, "call uabs instead");
		if constexpr ($Floating<T>)
		{
			if consteval
			{
				return signbit(x) ? -x : x;
			}
			else
			{
#if CLANG || GCC
				MATH_BUILTIN(fabs, T, x);
#else
				/* */if constexpr ($Same<f32, T>)return fabsf(x);
				else if constexpr ($Same<f64, T>)return fabs(x);
#endif
			}
		}
		else
			return x < T{} ? -x : x;
	}

	template<$SInt T>
	NO_SIDE_EFFECTS INLINE constexpr std::make_unsigned_t<T> uabs(const T x) noexcept
	{
		using U = std::make_unsigned_t<T>;
		return x < T{} ? static_cast<U>(U{} - static_cast<U>(x)) : static_cast<U>(x);
	}
}
STATIC_ASSERT(Math::uabs(i32{ 0 }) == u32{ 0 });
STATIC_ASSERT(Math::uabs(i32{ 1 }) == u32{ 1 });
STATIC_ASSERT(Math::uabs(i32{ -1 }) == u32{ 1 });
STATIC_ASSERT(Math::uabs(i32{ 42 }) == u32{ 42 });
STATIC_ASSERT(Math::uabs(i32{ -42 }) == u32{ 42 });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i8>::max()) == u8{ 127 });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i16>::max()) == u16{ 32767 });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i32>::max()) == u32{ 2147483647 });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i64>::max()) == u64{ 9223372036854775807 });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i8>::min()) == u8{ 128 });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i16>::min()) == u16{ 32768 });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i32>::min()) == u32{ 2147483648 });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i64>::min()) == u64{ 9223372036854775808ull });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i32>::min() + 1) == u32{ 2147483647 });
STATIC_ASSERT(Math::uabs(std::numeric_limits<i64>::min() + 1) == u64{ 9223372036854775807 });