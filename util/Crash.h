#pragma once
#include "Compiler.h"
#include "Unreachable.h"
#if !defined(ENABLE_CRASH)
	#define ENABLE_CRASH 1
#endif

#if ENABLE_CRASH
	#define CRASH() do{INT3();UNREACHABLE;}while(0)
#else
	#define CRASH() UNREACHABLE
#endif

#if MSVC
	#include <intrin.h>
	#define INT3 __debugbreak
#elif HAS_BUILTIN(__builtin_debugtrap)
	#define INT3 __builtin_debugtrap
#elif (GCC || CLANG) && (defined(__i386__) || defined(__x86_64__))
	#define INT3() __asm__ __volatile__("int3")
#else
	#define INT3 __builtin_trap
#endif