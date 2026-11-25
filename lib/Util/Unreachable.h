#pragma once
#include "Compiler.h"
#if MSVC
	#define UNREACHABLE __assume(false)
#else
	#define UNREACHABLE __builtin_unreachable()
#endif