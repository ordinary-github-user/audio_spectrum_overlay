#pragma once
#include "BasicTypes.h"
#include "Compiler.h"
#include "Macro/Global.h"
#include "Macro/If.h"
#if MSVC
	#pragma section(".syscall",execute,read)
	#pragma comment(linker, "/merge:.syscall=.text")
#endif
template<u8...bytes>
IF(MSVC, __declspec(allocate(".syscall")), __attribute__((section(".text"))))
GLOBAL IF(CLANG_CL,static,) constexpr u8 callable_bytes[]{ bytes... };