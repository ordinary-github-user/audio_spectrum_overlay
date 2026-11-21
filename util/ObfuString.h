#pragma once
#include "Compiler.h"
#if defined(__INTELLISENSE__) || MSVC
	#define STRING_OBFUSCATION 0
#elif !defined(STRING_OBFUSCATION)
	#define STRING_OBFUSCATION 0
#endif

#if STRING_OBFUSCATION == 0
#include "RdataString.h"
#define ObfuString RdataString
#define S8 RS8
#define S16 RS16
#elif STRING_OBFUSCATION == 1
	#include "XorString.h"
	#define ObfuString XorString
	#define S8 XS8
	#define S16 XS16
#elif STRING_OBFUSCATION == 2
	#include "AString.h"
	#define ObfuString AString
	#define S8 AS8
	#define S16 AS16
#elif STRING_OBFUSCATION == 3
	#include "MString.h"
	#define ObfuString MString
	#define S8 MS8
	#define S16 MS16
#endif