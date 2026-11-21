#pragma once
#include "Compiler.h"
#if CLANG
	#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
	#pragma clang diagnostic ignored "-Wbraced-scalar-init"//https://github.com/llvm/llvm-project/issues/57286
	#pragma clang diagnostic ignored "-Wundefined-var-template"
	#pragma clang diagnostic ignored "-Winvalid-offsetof"//offset of on non-standard-layout type '???' [-Winvalid-offsetof]
#elif GCC
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif