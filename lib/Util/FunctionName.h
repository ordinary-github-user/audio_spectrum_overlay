#pragma once
#include "Compiler.h"
#if CLANG || GCC
	#define FUNCTION_NAME __PRETTY_FUNCTION__
#else
	#define FUNCTION_NAME __FUNCSIG__
#endif