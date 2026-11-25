#pragma once
#include "../Compiler.h"
#if defined(__INTELLISENSE__)
	#define CONSTEVAL constexpr
#else
	#define CONSTEVAL consteval
#endif