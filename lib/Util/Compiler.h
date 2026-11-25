#pragma once
#if defined(__has_feature)
	#define HAS_FEATURE(...) __has_feature(__VA_ARGS__)
#else
	#define HAS_FEATURE(...) 0
#endif
#if defined(__has_builtin)
	#define HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
	#define HAS_BUILTIN(...) 0
#endif
#if defined(__clang__)
	#define CLANG 1
#else
	#define CLANG 0
#endif
#if defined(__GNUC__)&&!defined(__clang__)
	#define GCC 1
#else
	#define GCC 0
#endif
#if defined(_MSC_VER)&&!defined(__clang__)
	#define MSVC 1
#else
	#define MSVC 0
#endif
#if defined(_MSC_VER)&&defined(__clang__)
	#define CLANG_CL 1
#else
	#define CLANG_CL 0
#endif
#if CLANG && HAS_FEATURE(reflection) && HAS_FEATURE(template_strings)//https://github.com/brevzin/llvm-project/commits/compiler-explorer/barry
	#define BARRY 1
#else
	#define BARRY 0
#endif
static_assert(CLANG + GCC + MSVC == 1);
#if HAS_FEATURE(expansion_statements) || __cpp_expansion_statements >= 202506L
	#define TEMPLATE_FOR 1
#else
	#define TEMPLATE_FOR 0
#endif
#if (HAS_FEATURE(reflection) || __cpp_impl_reflection >= 202506L) && !GCC && !CLANG
	#define META 1
#else
	#define META 0
#endif
#if __cpp_structured_bindings >= 202411L && !GCC
	#define VARIADIC_STRUCTURED_BINDING 1
#else
	#define VARIADIC_STRUCTURED_BINDING 0
#endif
#if __cpp_constexpr >= 202406L
	#define CONSTEXPR_PLACEMENT_NEW 1
#else
	#define CONSTEXPR_PLACEMENT_NEW 0
#endif
#if defined(_WIN64) || (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8)
	#define BITNESS 64
#else
	#define BITNESS 32
#endif
static_assert(sizeof(void*) * 8 == BITNESS);
#if defined(__SSE2__) || defined(__AVX2__)
	#define SSE2 1
#else
	#define SSE2 0
#endif
#if defined(__AVX2__)
	#define AVX2 1
#else
	#define AVX2 0
#endif
#if defined(__AVX512BW__)
	#define AVX512BW 1
#else
	#define AVX512BW 0
#endif
#if defined(__AVX512F__)
	#define AVX512F 1
#else
	#define AVX512F 0
#endif
#if defined(__SHA__) || (MSVC && defined(__AVX512F__))
	#define SHA 1
#else
	#define SHA 0
#endif
#if defined(__RDSEED__) || (MSVC && defined(__AVX512F__))
	#define RDSEED 1
#else
	#define RDSEED 0
#endif
#if defined(__BMI2__) || (MSVC && defined(__AVX2__))
	#define BMI2 1
#else
	#define BMI2 0
#endif