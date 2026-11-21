#pragma once
#include "Compiler.h"
#if MSVC
	#define LIKELY(...) (!!(__VA_ARGS__))
	#define UNLIKELY(...) (!!(__VA_ARGS__))
#else
	#define LIKELY(...) __builtin_expect(!!(__VA_ARGS__), 1)
	#define UNLIKELY(...) __builtin_expect(!!(__VA_ARGS__), 0)
#endif