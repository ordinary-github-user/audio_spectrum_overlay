#pragma once
#include "Compiler.h"
#include "BasicTypes.h"
#include "Inline.h"
#include "KuserSharedData.h"
#include "ProcessCookie.h"
#include <intrin.h>
//https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=rdseed&ig_expand=5270
template<class>struct RandomDevice;
template<>struct RandomDevice<u64>
{
	INLINE static u64 operator()() noexcept
	{
#if RDSEED
		u64 x;
	#if BITNESS == 64
		while (!_rdseed64_step(&x));
	#else
		while (!_rdseed32_step(reinterpret_cast<u32*>(&x) + 0));
		while (!_rdseed32_step(reinterpret_cast<u32*>(&x) + 1));
	#endif
		return x;
#else
		return __rdtsc();
		//constexpr auto mix{ [] (u64 x)static LAMBDA_INLINE { return (x << 1) | 1; } };
		//return __rdtsc() ^ (mix(Windows::kuserSharedData().InterruptTime.value)* mix(Windows::kuserSharedData().SystemTime.value)* mix(Windows::kuserSharedData().TimeUpdateLock)* mix(Windows::kuserSharedData().BaselineSystemTimeQpc)* mix(Windows::cookie()));
#endif
	}
};
template<>struct RandomDevice<u32> 
{
	INLINE static u32 operator()() noexcept
	{
#if RDSEED
		u32 x;
		while (!_rdseed32_step(&x));
		return x;
#else
		const u64 v{ RandomDevice<u64>::operator()() }; 
		return __builtin_bit_cast(std::array<u32, 2>,v)[0] ^ __builtin_bit_cast(std::array<u32, 2>,v)[1];
#endif
	}
};
template<>struct RandomDevice<u16> 
{
	INLINE static u16 operator()() noexcept
	{
#if RDSEED
		u16 x;
		while (!_rdseed16_step(&x));
		return x;
#else
		const u64 v{ RandomDevice<u64>::operator()() }; 
		return __builtin_bit_cast(std::array<u16, 4>,v)[0] ^ __builtin_bit_cast(std::array<u16, 4>,v)[1] ^ __builtin_bit_cast(std::array<u16, 4>,v)[2] ^ __builtin_bit_cast(std::array<u16, 4>,v)[3];
#endif
	}
};
template<>struct RandomDevice<u8> 
{
	INLINE static u8 operator()() noexcept
	{
#if RDSEED
		u16 x;
		while (!_rdseed16_step(&x));
		return __builtin_bit_cast(std::array<u8, 2>,x)[0] ^ __builtin_bit_cast(std::array<u8, 2>,x)[1];
#else
		const u64 v{ RandomDevice<u64>::operator()() }; 
		return __builtin_bit_cast(std::array<u8, 8>,v)[0] ^ __builtin_bit_cast(std::array<u8, 8>,v)[1] ^ __builtin_bit_cast(std::array<u8, 8>,v)[2] ^ __builtin_bit_cast(std::array<u8, 8>,v)[3] ^ __builtin_bit_cast(std::array<u8, 8>,v)[4] ^ __builtin_bit_cast(std::array<u8, 8>,v)[5] ^ __builtin_bit_cast(std::array<u8, 8>,v)[6] ^ __builtin_bit_cast(std::array<u8, 8>,v)[7];
#endif
	}
};