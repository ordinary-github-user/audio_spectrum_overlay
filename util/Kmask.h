#pragma once
#include "Compiler.h"
#include "BasicTypes.h"
#include "Compiler.h"
#include "Inline.h"
#include <immintrin.h>
#if defined(__AVX512F__)
	#if MSVC && BITNESS == 32//msvc bug
		#define kmask_zero(...) (!(__VA_ARGS__))
	#else
		INLINE constexpr bool kmask_zero(__mmask8  x)noexcept { if consteval { return !x; } else { return  _kortestz_mask8_u8(x, x); } }
		INLINE constexpr bool kmask_zero(__mmask16 x)noexcept { if consteval { return !x; } else { return _kortestz_mask16_u8(x, x); } }
		INLINE constexpr bool kmask_zero(__mmask32 x)noexcept { if consteval { return !x; } else { return _kortestz_mask32_u8(x, x); } }
		INLINE constexpr bool kmask_zero(__mmask64 x)noexcept { if consteval { return !x; } else { return _kortestz_mask64_u8(x, x); } }
	#endif
#endif