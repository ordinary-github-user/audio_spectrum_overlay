#pragma once
#include "Compiler.h"
//function return value depends only on parameter, can't access global variable
#if defined(NDEBUG) && (CLANG || GCC)
	#define NO_SIDE_EFFECTS [[gnu::const]]
#else
	#define NO_SIDE_EFFECTS
#endif