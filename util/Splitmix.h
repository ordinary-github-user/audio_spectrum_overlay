#pragma once
#include "BasicTypes.h"
#include "ConceptIntegral.h"
#include "Inline.h"
template<$UInt T>
constexpr T splitmix(T& x) noexcept
{
	if constexpr (sizeof(T) == 8)
	{
		u64 z{ x += 0x9E3779B97F4A7C15uLL };
		z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9uLL;
		z = (z ^ (z >> 27)) * 0x94D049BB133111EBuLL;
		return z ^ (z >> 31);
	}
	else if constexpr (sizeof(T) == 4)// mixer [16 21f0aaad 15 735a2d97 15] (hash-prospector); same integers as bryc's JS splitmix32
	{
		u32 z{ x += 0x9E3779B9u };
		z = (z ^ (z >> 16)) * 0x21F0AAADu;
		z = (z ^ (z >> 15)) * 0x735A2D97u;
		return z ^ (z >> 15);
	}
	else if constexpr (sizeof(T) == 2)// mixer [8 a3d3 7 4b2d 9]: exact avalanche bias 0.0073 (hash16_xm2's is 0.0086)
	{
		u16 z{ x = static_cast<u16>(x + 0x9E37u) };
		z = static_cast<u16>((z ^ (z >> 8)) * 0xA3D3u);
		z = static_cast<u16>((z ^ (z >> 7)) * 0x4B2Du);
		return static_cast<u16>(z ^ (z >> 9));
	}
	else if constexpr (sizeof(T) == 1)// mixer [1 0f 3 dd 4]: lowest exact avalanche bias of all 5.6M possible 2-round 8-bit mixers
	{
		u8 z{ x = static_cast<u8>(x + 0x9Fu) };
		z = static_cast<u8>((z ^ (z >> 1)) * 0x0Fu);
		z = static_cast<u8>((z ^ (z >> 3)) * 0xDDu);
		return static_cast<u8>(z ^ (z >> 4));
	}
}
template<$UInt T>
struct Splitmix
{
	T x;
	INLINE constexpr T operator()()noexcept { return splitmix(x); }
};