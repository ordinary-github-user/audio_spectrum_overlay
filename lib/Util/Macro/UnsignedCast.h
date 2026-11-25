#pragma once
#include <bit>
#include "../Compiler.h"
#include "../SizedInt.h"
#if MSVC//msvc bug
	#define UNSIGNED(...) std::bit_cast<UnsignedBytes<sizeof(std::remove_cvref_t<decltype(__VA_ARGS__)>)>>(__VA_ARGS__)
#else
	#define UNSIGNED(...) __builtin_bit_cast(UnsignedBytes<sizeof(std::remove_cvref_t<decltype(__VA_ARGS__)>)>,__VA_ARGS__)
#endif