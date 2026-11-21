#pragma once
#include "Compiler.h"
#include "SizedInt.h"
#include <bit>
#if MSVC//msvc bug
	#define UNSIGNED(...) std::bit_cast<UnsignedBytes<sizeof(std::remove_cvref_t<decltype(__VA_ARGS__)>)>>(__VA_ARGS__)
#else
	#define UNSIGNED(...) __builtin_bit_cast(UnsignedBytes<sizeof(std::remove_cvref_t<decltype(__VA_ARGS__)>)>,__VA_ARGS__)
#endif