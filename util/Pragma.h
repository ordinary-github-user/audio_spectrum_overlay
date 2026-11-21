#pragma once
#include "Compiler.h"
#if GCC
	#define PRAGMA_GCC(x) _Pragma(#x)
#else
	#define PRAGMA_GCC(...)
#endif
#if CLANG
	#define PRAGMA_CLANG(x) _Pragma(#x)
#else
	#define PRAGMA_CLANG(...)
#endif
#if MSVC
	#define PRAGMA_MSVC(x) _Pragma(#x)
#else
	#define PRAGMA_MSVC(...)
#endif