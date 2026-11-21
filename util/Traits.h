#pragma once
#include "Compiler.h"
#include <utility>
#if GCC//gcc bug: use of built-in trait '__is_trivially_destructible(T)' in function signature; use library traits instead
	#define TRIVIALLY_DESTRUCTIBLE(...) std::is_trivially_destructible_v<__VA_ARGS__>
#else
	#define TRIVIALLY_DESTRUCTIBLE(...) __is_trivially_destructible(__VA_ARGS__)
#endif