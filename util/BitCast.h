#pragma once
#include "Compiler.h"
#if MSVC
	#define BITCAST(...) (__builtin_bit_cast(__VA_ARGS__))
#else
	#define BITCAST(...) __builtin_bit_cast(__VA_ARGS__)
#endif