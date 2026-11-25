#pragma once
#include "Compiler.h"
#if MSVC
	#define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
	#if __has_cpp_attribute(no_unique_address)
		#define NO_UNIQUE_ADDRESS [[no_unique_address]]
	#else
		#define NO_UNIQUE_ADDRESS
	#endif
#endif