#pragma once
#if defined(__INTELLISENSE__) || defined(DISABLE_STATIC_ASSERT)
	#define STATIC_ASSERT(...)
#else
	#define STATIC_ASSERT static_assert
#endif