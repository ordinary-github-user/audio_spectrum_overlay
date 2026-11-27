#pragma once
#include "Compiler.h"
#if !defined(DISABLE_INLINE)
	#define DISABLE_INLINE 0
#endif
#if DISABLE_INLINE
	#if CLANG || GCC
		#define NO_INLINE __attribute__ ((noinline))
		#define INLINE inline
		#define LAMBDA_INLINE __attribute__((always_inline))
		#define FLATTEN
		#define STATIC_LAMBDA_INLINE static
	#else
		#define NO_INLINE __declspec(noinline)
		#define INLINE
		#define LAMBDA_INLINE
		#define FLATTEN
		#define STATIC_LAMBDA_INLINE static
	#endif
#else
	#if CLANG || GCC
		#define NO_INLINE __attribute__ ((noinline))
		#define INLINE __attribute__((always_inline)) inline
		#define LAMBDA_INLINE __attribute__((always_inline))
		#define FLATTEN __attribute__((flatten))
		#if CLANG
			#define STATIC_LAMBDA_INLINE __attribute__((always_inline)) static
		#else
			#define STATIC_LAMBDA_INLINE static __attribute__((always_inline))
		#endif
	#else
		#define NO_INLINE __declspec(noinline)
		#define INLINE inline __forceinline
		#define LAMBDA_INLINE [[msvc::forceinline]]
		#define FLATTEN
		#define STATIC_LAMBDA_INLINE static
	#endif
#endif