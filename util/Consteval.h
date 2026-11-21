#pragma once
#include "Compiler.h"
#if defined(__INTELLISENSE__) || GCC//gcc bug
	#define CONSTEVAL constexpr
#else
	#define CONSTEVAL consteval
#endif