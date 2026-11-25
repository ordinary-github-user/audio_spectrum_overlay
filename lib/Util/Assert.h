#pragma once
#include "Assume.h"
#include "Compiler.h"
#include "Crash.h"
#include "Macro/Pragma.h"
#include "Unreachable.h"
#if 1
	#if defined(NDEBUG)
		#define ASSERT(...) do{if consteval{if(!(__VA_ARGS__))UNREACHABLE;}else{PRAGMA_CLANG(clang diagnostic push);PRAGMA_CLANG(clang diagnostic ignored "-Wassume");ASSUME(__VA_ARGS__);PRAGMA_CLANG(clang diagnostic pop);}}while(0)
	#else
		#define ASSERT(...) do{if consteval{if(!(__VA_ARGS__))UNREACHABLE;}else{if(!(__VA_ARGS__))CRASH();}}while(0)
	#endif
#else
	#define ASSERT(...) ASSUME(__VA_ARGS__)
#endif