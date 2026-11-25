#pragma once
#include "Compiler.h"
#if MSVC
	#define ASSUME(...) __assume(__VA_ARGS__)
#else
	#define ASSUME(...) [[assume(__VA_ARGS__)]]
#endif