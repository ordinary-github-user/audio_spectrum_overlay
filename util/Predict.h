#pragma once
/* Helps static branch prediction so hot path can be better optimized.  */
#ifdef __GNUC__
	#define predict_true(...) __builtin_expect(!!(__VA_ARGS__), 1)
	#define predict_false(...) __builtin_expect(__VA_ARGS__, 0)
#else
	#define predict_true(...) (__VA_ARGS__)
	#define predict_false(...) (__VA_ARGS__)
#endif