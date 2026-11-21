#pragma once
//https://github.com/Quuxplusone/Xoshiro256ss
//The "xoshiro256** 1.0" generator.
//C++ port by Arthur O'Dwyer (2021).
//Based on the C version by David Blackman and Sebastiano Vigna (2018),
//https://prng.di.unimi.it/xoshiro256starstar.c
#include "BasicTypes.h"
#include "Consteval.h"
#include "Inline.h"
#include "Rot.h"
#include "Splitmix.h"
#include <limits>
enum class XoshiroType :u8
{
	plus,
	plus_plus,
	star_star
};

template<XoshiroType, u64>struct BasicXoshiro;

template<XoshiroType Type>
struct BasicXoshiro<Type, 256>
{
private:
	u64 s[4];
public:
	using result_type = u64;

	constexpr BasicXoshiro(u64 initialSeed = 0) noexcept { seed(initialSeed); }

	constexpr void seed(u64 v)noexcept
	{
		s[0] = splitmix(v);
		s[1] = splitmix(v);
		s[2] = splitmix(v);
		s[3] = splitmix(v);
	}

	static CONSTEVAL u64 min()noexcept { return 0; }
	static CONSTEVAL u64 max()noexcept { return std::numeric_limits<u64>::max(); }

	constexpr u64 operator()() noexcept
	{
		const u64 result{ [&]LAMBDA_INLINE 
		{
			/* */if constexpr (Type == XoshiroType::plus)return s[0] + s[3];
			else if constexpr (Type == XoshiroType::plus_plus)return Bits::rotL(s[0] + s[3], 23) + s[0];
			else if constexpr (Type == XoshiroType::star_star)return Bits::rotL(s[1] * 5, 7) * 9;
		}()};
		const u64 t{ s[1] << 17 }; 
		s[2] ^= s[0]; 
		s[3] ^= s[1]; 
		s[1] ^= s[2]; 
		s[0] ^= s[3]; 
		s[2] ^= t; 
		s[3] = Bits::rotL(s[3], 45);
		return result;
	}
};

using Xoshiro256p = BasicXoshiro<XoshiroType::plus, 256>;
using Xoshiro256pp = BasicXoshiro<XoshiroType::plus_plus, 256>;
using Xoshiro256ss = BasicXoshiro<XoshiroType::star_star, 256>;