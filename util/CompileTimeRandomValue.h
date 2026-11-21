#pragma once
#include "FNV.h"
#include "FunctionName.h"
#include "SourceLocation.h"
#if defined(CONSTANT_BUILD_SEED)
	#define BUILDTIME_HASH_VALUE32 u32{0x87654321}
	#define BUILDTIME_HASH_VALUE64 u64{0x8765432187654321}
#else
	#define BUILDTIME_HASH_VALUE32 Fnv32::hash(__DATE__ __TIME__)
	#define BUILDTIME_HASH_VALUE64 Fnv64::hash(__DATE__ __TIME__)
#endif

#define FUNCTION_NAME_HASH_VALUE32 Fnv32::hash(FUNCTION_NAME)
#define FUNCTION_NAME_HASH_VALUE64 Fnv64::hash(FUNCTION_NAME)

#define LINE_NUMBER_HASH_VALUE32 Fnv32::hash(LINE_STRING)
#define LINE_NUMBER_HASH_VALUE64 Fnv64::hash(LINE_STRING)