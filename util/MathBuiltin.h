#pragma once
#include "Compiler.h"
#include "ConceptSame.h"
#if CLANG || GCC
	#define MATH_BUILTIN(callable, T, ...)						\
	do{															\
		if constexpr ($Same<f32, T>)							\
			return __builtin_ ## callable ## f(__VA_ARGS__);	\
		else if constexpr($Same<f64, T>)						\
			return __builtin_ ## callable (__VA_ARGS__);		\
		else if constexpr ($Same<f128, T>)						\
			return __builtin_ ## callable ## l (__VA_ARGS__);	\
	}while(0)
#endif