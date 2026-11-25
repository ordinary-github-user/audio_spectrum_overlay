#pragma once
#include <cstring>//std::memcpy
#include "../Assert.h"
#include "../Bits.h"
#include "../Compiler.h"
#include "../Concepts/Char.h"
#include "../DataStructures/FixedStringImpl.h"
#include "../DataStructures/StaticStringImpl.h"
#include "../IndexTypeOf.h"
#include "../Iota.h"
#include "../Pow10Table.h"
#include "../SizedInt.h"
#include "CountDigit10.h"
//todo: every algo that has input of size 1 can just make a table of size 0x100, and look up at run time
template<$UInt T>
constexpr auto udec2chars_required_buffer_size{compact<std::numeric_limits<T>::digits10 + 1>};
//constexpr std::array<std::array<c8, 2>, 100> udec2chars_digit_pairs{ [] static
//{
//	std::array<std::array<c8, 2>, 100> result;
//	for (const auto i: iota<100>)
//		result[i] = { static_cast<c8>('0' + i / 10), static_cast<c8>('0' + i % 10) };
//	return result;
//}() };
constexpr std::array<std::array<c8, 4>, 10000> udec2chars_digit_quads{ [] static
{
	std::array<std::array<c8, 4>, 10000> result;
	for (const auto i : iota<10000>)
		result[i] =
		{
			static_cast<c8>('0' + i / 1000),
			static_cast<c8>('0' + i / 100 % 10),
			static_cast<c8>('0' + i / 10 % 10),
			static_cast<c8>('0' + i % 10)
		};
	return result;
}() };
template<c8 pad>
requires(pad == '0' || pad == ' ')
INLINE constexpr auto* udec2chars_fixed($UInt auto u, $Char auto* p)noexcept
{
	using U = decltype(u);
	static constexpr auto buffer_size{ udec2chars_required_buffer_size<U> };
	if consteval { p[buffer_size - 1] = {}; }//test if buffer is big enough
	if constexpr (sizeof(u) == 1)
	{
		static constexpr auto table{ [] static
		{
			std::array<std::array<c8, 3>, 0x100> result;
			for (const auto i : iota<0x100>)
			{
				const std::array<c8, 4>& quad{ udec2chars_digit_quads[i] };
				auto* p{ result[i].data() };
				if (i >= 100)
				{
					p[0] = quad[1];
					p[1] = quad[2];
					p[2] = quad[3];
				}
				else if (i >= 10)
				{
					p[0] = pad;
					p[1] = quad[2];
					p[2] = quad[3];
				}
				else
				{
					p[0] = pad;
					p[1] = pad;
					p[2] = quad[3];
				}
			}
			return result;
		}() };
		const std::array<c8, 3>& entry{ table[u] };
		p[0] = entry[0];
		p[1] = entry[1];
		p[2] = entry[2];
	}
	else if constexpr (pad == '0')
	{
		if constexpr (sizeof(u) == 8)
		{
			const auto& quad16{ udec2chars_digit_quads[u / 10000000000000000ULL] };
			const auto& quad12{ udec2chars_digit_quads[(u / 1000000000000ULL) % 10000] };
			const auto& quad8{ udec2chars_digit_quads[(u / 100000000ULL) % 10000] };
			const auto& quad4{ udec2chars_digit_quads[(u / 10000ULL) % 10000] };
			const auto& quad0{ udec2chars_digit_quads[u % 10000] };
			p[0] = quad16[0]; p[1] = quad16[1]; p[2] = quad16[2]; p[3] = quad16[3];
			p[4] = quad12[0]; p[5] = quad12[1]; p[6] = quad12[2]; p[7] = quad12[3];
			p[8] = quad8[0]; p[9] = quad8[1]; p[10] = quad8[2]; p[11] = quad8[3];
			p[12] = quad4[0]; p[13] = quad4[1]; p[14] = quad4[2]; p[15] = quad4[3];
			p[16] = quad0[0]; p[17] = quad0[1]; p[18] = quad0[2]; p[19] = quad0[3];
		}
		if constexpr (sizeof(u) == 4)
		{
			const auto& quad8{ udec2chars_digit_quads[u / 100000000] };
			const auto& quad4{ udec2chars_digit_quads[u / 10000 % 10000] };
			const auto& quad0{ udec2chars_digit_quads[u % 10000] };
			p[0] = quad8[2]; p[1] = quad8[3];
			p[2] = quad4[0]; p[3] = quad4[1]; p[4] = quad4[2]; p[5] = quad4[3];
			p[6] = quad0[0]; p[7] = quad0[1]; p[8] = quad0[2]; p[9] = quad0[3];
		}
		if constexpr (sizeof(u) == 2)
		{
			const auto& quad0{ udec2chars_digit_quads[u % 10000] };
			p[0] = static_cast<c8>(u / 10000 + '0');
			p[1] = quad0[0]; p[2] = quad0[1]; p[3] = quad0[2]; p[4] = quad0[3];
		}
	}
	else
	{
		udec2chars_fixed<'0'>(u, p);
		if constexpr (sizeof(u) < 8)
			for (auto* pPad{ p }, * pEnd1{ p + buffer_size - 1 }; pPad != pEnd1 && *pPad == '0'; ++pPad)
				*pPad = pad;
		else
		{
			const u8 digits{ count_digits10(u) };
			for (auto* pPad{ p }, * pEndPad{ p + buffer_size - digits }; pPad != pEndPad; ++pPad)
				*pPad = pad;
		}
	}
	return p + buffer_size;
}
template<c8 pad = ' ', $Char Char = c8>
requires(pad == '0' || pad == ' ')
constexpr auto udec2chars_fixed($UInt auto u)noexcept
{
	FixedString<Char, udec2chars_required_buffer_size<decltype(u)>> result;
	udec2chars_fixed<pad>(u, result.data());
	return result;
}

template<bool skip_buffer_check = false>
INLINE constexpr auto* udec2chars($UInt auto u, $Char auto* p)noexcept
{
	if constexpr (sizeof(u) == 8)
	{
		const auto& quad16{ udec2chars_digit_quads[u / 10000000000000000ULL] };
		const auto& quad12{ udec2chars_digit_quads[(u / 1000000000000ULL) % 10000] };
		const auto& quad8{ udec2chars_digit_quads[(u / 100000000ULL) % 10000] };
		const auto& quad4{ udec2chars_digit_quads[(u / 10000ULL) % 10000] };
		const auto& quad0{ udec2chars_digit_quads[u % 10000] };
		if (u >= 10000000000000000000ULL)
		{
			p[0] = quad16[0]; p[1] = quad16[1]; p[2] = quad16[2]; p[3] = quad16[3];
			p[4] = quad12[0]; p[5] = quad12[1]; p[6] = quad12[2]; p[7] = quad12[3];
			p[8] = quad8[0]; p[9] = quad8[1]; p[10] = quad8[2]; p[11] = quad8[3];
			p[12] = quad4[0]; p[13] = quad4[1]; p[14] = quad4[2]; p[15] = quad4[3];
			p[16] = quad0[0]; p[17] = quad0[1]; p[18] = quad0[2]; p[19] = quad0[3];
			return p + 20;
		}
		if (u >= 1000000000000000000ULL)
		{
			p[0] = quad16[1]; p[1] = quad16[2]; p[2] = quad16[3];
			p[3] = quad12[0]; p[4] = quad12[1]; p[5] = quad12[2]; p[6] = quad12[3];
			p[7] = quad8[0]; p[8] = quad8[1]; p[9] = quad8[2]; p[10] = quad8[3];
			p[11] = quad4[0]; p[12] = quad4[1]; p[13] = quad4[2]; p[14] = quad4[3];
			p[15] = quad0[0]; p[16] = quad0[1]; p[17] = quad0[2]; p[18] = quad0[3];
			return p + 19;
		}
		if (u >= 100000000000000000ULL)
		{
			p[0] = quad16[2]; p[1] = quad16[3];
			p[2] = quad12[0]; p[3] = quad12[1]; p[4] = quad12[2]; p[5] = quad12[3];
			p[6] = quad8[0]; p[7] = quad8[1]; p[8] = quad8[2]; p[9] = quad8[3];
			p[10] = quad4[0]; p[11] = quad4[1]; p[12] = quad4[2]; p[13] = quad4[3];
			p[14] = quad0[0]; p[15] = quad0[1]; p[16] = quad0[2]; p[17] = quad0[3];
			return p + 18;
		}
		if (u >= 10000000000000000ULL)
		{
			p[0] = quad16[3];
			p[1] = quad12[0]; p[2] = quad12[1]; p[3] = quad12[2]; p[4] = quad12[3];
			p[5] = quad8[0]; p[6] = quad8[1]; p[7] = quad8[2]; p[8] = quad8[3];
			p[9] = quad4[0]; p[10] = quad4[1]; p[11] = quad4[2]; p[12] = quad4[3];
			p[13] = quad0[0]; p[14] = quad0[1]; p[15] = quad0[2]; p[16] = quad0[3];
			return p + 17;
		}
		if (u >= 1000000000000000ULL)
		{
			p[0] = quad12[0]; p[1] = quad12[1]; p[2] = quad12[2]; p[3] = quad12[3];
			p[4] = quad8[0]; p[5] = quad8[1]; p[6] = quad8[2]; p[7] = quad8[3];
			p[8] = quad4[0]; p[9] = quad4[1]; p[10] = quad4[2]; p[11] = quad4[3];
			p[12] = quad0[0]; p[13] = quad0[1]; p[14] = quad0[2]; p[15] = quad0[3];
			return p + 16;
		}
		if (u >= 100000000000000ULL)
		{
			p[0] = quad12[1]; p[1] = quad12[2]; p[2] = quad12[3];
			p[3] = quad8[0]; p[4] = quad8[1]; p[5] = quad8[2]; p[6] = quad8[3];
			p[7] = quad4[0]; p[8] = quad4[1]; p[9] = quad4[2]; p[10] = quad4[3];
			p[11] = quad0[0]; p[12] = quad0[1]; p[13] = quad0[2]; p[14] = quad0[3];
			return p + 15;
		}
		if (u >= 10000000000000ULL)
		{
			p[0] = quad12[2]; p[1] = quad12[3];
			p[2] = quad8[0]; p[3] = quad8[1]; p[4] = quad8[2]; p[5] = quad8[3];
			p[6] = quad4[0]; p[7] = quad4[1]; p[8] = quad4[2]; p[9] = quad4[3];
			p[10] = quad0[0]; p[11] = quad0[1]; p[12] = quad0[2]; p[13] = quad0[3];
			return p + 14;
		}
		if (u >= 1000000000000ULL)
		{
			p[0] = quad12[3];
			p[1] = quad8[0]; p[2] = quad8[1]; p[3] = quad8[2]; p[4] = quad8[3];
			p[5] = quad4[0]; p[6] = quad4[1]; p[7] = quad4[2]; p[8] = quad4[3];
			p[9] = quad0[0]; p[10] = quad0[1]; p[11] = quad0[2]; p[12] = quad0[3];
			return p + 13;
		}
		if (u >= 100000000000ULL)
		{
			p[0] = quad8[0]; p[1] = quad8[1]; p[2] = quad8[2]; p[3] = quad8[3];
			p[4] = quad4[0]; p[5] = quad4[1]; p[6] = quad4[2]; p[7] = quad4[3];
			p[8] = quad0[0]; p[9] = quad0[1]; p[10] = quad0[2]; p[11] = quad0[3];
			return p + 12;
		}
		if (u >= 10000000000ULL)
		{
			p[0] = quad8[1]; p[1] = quad8[2]; p[2] = quad8[3];
			p[3] = quad4[0]; p[4] = quad4[1]; p[5] = quad4[2]; p[6] = quad4[3];
			p[7] = quad0[0]; p[8] = quad0[1]; p[9] = quad0[2]; p[10] = quad0[3];
			return p + 11;
		}
		if (u >= 1000000000ULL)
		{
			p[0] = quad8[2]; p[1] = quad8[3];
			p[2] = quad4[0]; p[3] = quad4[1]; p[4] = quad4[2]; p[5] = quad4[3];
			p[6] = quad0[0]; p[7] = quad0[1]; p[8] = quad0[2]; p[9] = quad0[3];
			return p + 10;
		}
		if (u >= 100000000ULL)
		{
			p[0] = quad8[3];
			p[1] = quad4[0]; p[2] = quad4[1]; p[3] = quad4[2]; p[4] = quad4[3];
			p[5] = quad0[0]; p[6] = quad0[1]; p[7] = quad0[2]; p[8] = quad0[3];
			return p + 9;
		}
		if (u >= 10000000ULL)
		{
			p[0] = quad4[0]; p[1] = quad4[1]; p[2] = quad4[2]; p[3] = quad4[3];
			p[4] = quad0[0]; p[5] = quad0[1]; p[6] = quad0[2]; p[7] = quad0[3];
			return p + 8;
		}
		if (u >= 1000000ULL)
		{
			p[0] = quad4[1]; p[1] = quad4[2]; p[2] = quad4[3];
			p[3] = quad0[0]; p[4] = quad0[1]; p[5] = quad0[2]; p[6] = quad0[3];
			return p + 7;
		}
		if (u >= 100000ULL)
		{
			p[0] = quad4[2]; p[1] = quad4[3];
			p[2] = quad0[0]; p[3] = quad0[1]; p[4] = quad0[2]; p[5] = quad0[3];
			return p + 6;
		}
		if (u >= 10000ULL)
		{
			p[0] = quad4[3];
			p[1] = quad0[0]; p[2] = quad0[1]; p[3] = quad0[2]; p[4] = quad0[3];
			return p + 5;
		}
		if (u >= 1000ULL)
		{
			p[0] = quad0[0]; p[1] = quad0[1]; p[2] = quad0[2]; p[3] = quad0[3];
			return p + 4;
		}
		if (u >= 100ULL)
		{
			p[0] = quad0[1]; p[1] = quad0[2]; p[2] = quad0[3];
			return p + 3;
		}
		if (u >= 10ULL)
		{
			p[0] = quad0[2]; p[1] = quad0[3];
			return p + 2;
		}
		p[0] = static_cast<c8>(u + '0');
		return p + 1;
	}
	if constexpr (sizeof(u) == 4)
	{
		const auto& quad8{ udec2chars_digit_quads[u / 100000000] };
		const auto& quad4{ udec2chars_digit_quads[u / 10000 % 10000] };
		const auto& quad0{ udec2chars_digit_quads[u % 10000] };
		if (u >= 1000000000)
		{
			p[0] = quad8[2]; p[1] = quad8[3];
			p[2] = quad4[0]; p[3] = quad4[1]; p[4] = quad4[2]; p[5] = quad4[3];
			p[6] = quad0[0]; p[7] = quad0[1]; p[8] = quad0[2]; p[9] = quad0[3];
			return p + 10;
		}
		if (u >= 100000000)
		{
			p[0] = quad8[3];
			p[1] = quad4[0]; p[2] = quad4[1]; p[3] = quad4[2]; p[4] = quad4[3];
			p[5] = quad0[0]; p[6] = quad0[1]; p[7] = quad0[2]; p[8] = quad0[3];
			return p + 9;
		}
		if (u >= 10000000)
		{
			p[0] = quad4[0]; p[1] = quad4[1]; p[2] = quad4[2]; p[3] = quad4[3];
			p[4] = quad0[0]; p[5] = quad0[1]; p[6] = quad0[2]; p[7] = quad0[3];
			return p + 8;
		}
		if (u >= 1000000)
		{
			p[0] = quad4[1]; p[1] = quad4[2]; p[2] = quad4[3];
			p[3] = quad0[0]; p[4] = quad0[1]; p[5] = quad0[2]; p[6] = quad0[3];
			return p + 7;
		}
		if (u >= 100000)
		{
			p[0] = quad4[2]; p[1] = quad4[3];
			p[2] = quad0[0]; p[3] = quad0[1]; p[4] = quad0[2]; p[5] = quad0[3];
			return p + 6;
		}
		if (u >= 10000)
		{
			p[0] = quad4[3];
			p[1] = quad0[0]; p[2] = quad0[1]; p[3] = quad0[2]; p[4] = quad0[3];
			return p + 5;
		}
		if (u >= 1000)
		{
			p[0] = quad0[0]; p[1] = quad0[1]; p[2] = quad0[2]; p[3] = quad0[3];
			return p + 4;
		}
		if (u >= 100)
		{
			p[0] = quad0[1]; p[1] = quad0[2]; p[2] = quad0[3];
			return p + 3;
		}
		if (u >= 10)
		{
			p[0] = quad0[2]; p[1] = quad0[3];
			return p + 2;
		}
		p[0] = static_cast<c8>(u + '0');
		return p + 1;
	}
	if constexpr (sizeof(u) == 2)
	{
		const auto& quad0{ udec2chars_digit_quads[u % 10000] };
		if (u >= 10000)
		{
			p[0] = static_cast<c8>(u / 10000 + '0');
			p[1] = quad0[0]; p[2] = quad0[1]; p[3] = quad0[2]; p[4] = quad0[3];
			return p + 5;
		}
		if (u >= 1000)
		{
			p[0] = quad0[0]; p[1] = quad0[1]; p[2] = quad0[2]; p[3] = quad0[3];
			return p + 4;
		}
		if (u >= 100)
		{
			p[0] = quad0[1]; p[1] = quad0[2]; p[2] = quad0[3];
			return p + 3;
		}
		if (u >= 10)
		{
			p[0] = quad0[2]; p[1] = quad0[3];
			return p + 2;
		}
		p[0] = static_cast<c8>(u + '0');
		return p + 1;
	}
	if constexpr (sizeof(u) == 1)
	{
		static constexpr auto table{ [] static
		{
			std::array<std::pair<std::array<c8, 3>, u8>, 0x100> result;
			for (const auto i : iota<0x100>)
			{
				const std::array<c8, 4>& quad{ udec2chars_digit_quads[i] };
				auto* p{ result[i].first.data() };
				if (i >= 100)
				{
					p[0] = quad[1];
					p[1] = quad[2];
					p[2] = quad[3];
					result[i].second = 3;
				}
				else if (i >= 10)
				{
					p[0] = quad[2];
					p[1] = quad[3];
					p[2] = '?';
					result[i].second = 2;
				}
				else
				{
					p[0] = quad[3];
					p[1] = '?';
					p[2] = '?';
					result[i].second = 1;
				}
			}
			return result;
		}()};
		const std::pair<std::array<c8, 3>, u8>& entry{ table[u] };
		p[0] = entry.first[0];
		p[1] = entry.first[1];
		p[2] = entry.first[2];
		return p + entry.second;
	}
}
template<$Char Char = c8>
constexpr auto udec2chars($UInt auto u)noexcept
{
	StaticString<Char, udec2chars_required_buffer_size<decltype(u)>> result;
	result.modify_size(static_cast<IndexOf<decltype(result)>>(udec2chars(u, result.data()) - result.data()));
	return result;
}
template<$UInt T>
constexpr auto ubin2chars_required_buffer_size{ compact<std::numeric_limits<T>::digits> };
//constexpr std::array<std::array<c8, 8>, 0x100> ubin2chars_digit_octos{ [] static
//{
//	std::array<std::array<c8, 8>, 0x100> result;
//	for (const auto i : iota<0x100>)
//		result[i] = 
//		{
//			static_cast<c8>(Bits::at<7>(i) + '0'),
//			static_cast<c8>(Bits::at<6>(i) + '0'),
//			static_cast<c8>(Bits::at<5>(i) + '0'),
//			static_cast<c8>(Bits::at<4>(i) + '0'),
//			static_cast<c8>(Bits::at<3>(i) + '0'),
//			static_cast<c8>(Bits::at<2>(i) + '0'),
//			static_cast<c8>(Bits::at<1>(i) + '0'),
//			static_cast<c8>(Bits::at<0>(i) + '0')
//		};
//	return result;
//}() };
template<c8 pad>
requires(pad == '0' || pad == ' ')
INLINE constexpr auto* ubin2chars_fixed($UInt auto u, $Char auto* p) noexcept
{
	static constexpr auto buffer_size{ ubin2chars_required_buffer_size<decltype(u)> };
	if consteval { p[buffer_size - 1] = {}; }//test if buffer is big enough
	if constexpr (pad == '0')
	{
	#if __cpp_expansion_statements >= 202506L
		template for (constexpr auto i : iota<buffer_size>)
			p[i] = static_cast<c8>('0' + ((u >> compact<buffer_size - 1 - i>) & 1));
	#else
		for (auto* pDst{ p + buffer_size }; pDst != p; u >>= 1)
			*--pDst = static_cast<c8>('0' + (u & 1));
	#endif
	}
	else
	{
		ubin2chars_fixed<'0'>(u, p);
		u |= 1;
		const u8 lz{ Bits::leftZeroCount(u) };
		for (auto* pPad{ p }, * pEndPad{ p + lz }; pPad != pEndPad; ++pPad)
			*pPad = pad;
	}
	return p + buffer_size;
}
template<c8 pad = ' ', $Char Char = c8>
requires(pad == '0' || pad == ' ')
constexpr auto ubin2chars_fixed($UInt auto u)noexcept
{
	FixedString<Char, ubin2chars_required_buffer_size<decltype(u)>> result;
	ubin2chars_fixed<pad>(u, result.data());
	return result;
}

INLINE constexpr auto* ubin2chars($UInt auto u, $Char auto* p) noexcept
{
	static constexpr auto buffer_size{ ubin2chars_required_buffer_size<decltype(u)> };
	if consteval { p[buffer_size - 1] = {}; } // test if buffer is big enough
	if (!u) [[unlikely]]
	{
		*p = '0';
		return p + 1;
	}
	auto* const pEnd{ p + static_cast<u8>(std::numeric_limits<decltype(u)>::digits - Bits::leftZeroCount(u)) };
	for (auto* pDst{ pEnd }; pDst != p; u >>= 1)
		*--pDst = static_cast<c8>('0' + (u & 1));
	return pEnd;
}

template<$Char Char = c8>
constexpr auto ubin2chars($UInt auto u) noexcept
{
	StaticString<Char, ubin2chars_required_buffer_size<decltype(u)>> result;
	result.modify_size(static_cast<IndexOf<decltype(result)>>(ubin2chars(u, result.data()) - result.data()));
	return result;
}
template<$UInt T>
constexpr auto uhex2chars_required_buffer_size{ compact<sizeof(T) * 2>};
enum class Capitalized:u8 {no, yes};
template<Capitalized cap>
constexpr std::array<std::array<c8, 2>, 0x100> uhex2chars_digit_pairs{ [] static
{
	std::array<std::array<c8, 2>, 0x100> result;
	for (const auto i: iota<0x100>)
	{
		const u8 high{ static_cast<u8>((i >> 4) & 0xF) };
		const u8 low{ static_cast<u8>(i & 0xF) };
		const c8 cHigh{ static_cast<c8>(high + (high < 10 ? '0' : (cap == Capitalized::yes ? 'A' - 10 : 'a' - 10))) };
		const c8 cLow{ static_cast<c8>(low + (low < 10 ? '0' : (cap == Capitalized::yes ? 'A' - 10 : 'a' - 10))) };
		result[i] = { cHigh, cLow };
	}
	return result;
}() };
template<c8 pad, Capitalized cap>
requires(pad == '0' || pad == ' ')
INLINE constexpr auto* uhex2chars_fixed($UInt auto u, $Char auto* p) noexcept
{
	static constexpr auto buffer_size{ uhex2chars_required_buffer_size<decltype(u)>};
	if consteval { p[buffer_size - 1] = {}; }//test if buffer is big enough
	if constexpr (sizeof(u) == 1)
	{
		if constexpr (pad == '0')
		{
			const std::array<c8, 2>& pair{ uhex2chars_digit_pairs<cap>[u] };
			p[0] = pair[0];
			p[1] = pair[1];
		}
		else//pad == ' '
		{
			static constexpr std::array<std::array<c8, 2>, 0x100> table{ [] static
			{
				std::array<std::array<c8, 2>, 0x100> result;
				for (const auto i : iota<0x100>)
				{
					const u8 high{ static_cast<u8>((i >> 4) & 0xF) };
					const u8 low{ static_cast<u8>(i & 0xF) };
					const c8 cHigh{ high ? static_cast<c8>(high + (high < 10 ? '0' : (cap == Capitalized::yes ? 'A' - 10 : 'a' - 10))) : pad };
					const c8 cLow{ (high || low) ? static_cast<c8>(low + (low < 10 ? '0' : (cap == Capitalized::yes ? 'A' - 10 : 'a' - 10))) : '0' };
					result[i] = { cHigh, cLow };
				}
				return result;
			}() };
			const std::array<c8, 2>& pair{ table[u] };
			p[0] = pair[0];
			p[1] = pair[1];
		}
	}
	else
	{
		if constexpr (pad == '0')
		{
		#if __cpp_expansion_statements >= 202506L
			template for(constexpr auto i: reversed(iota<sizeof(u)>))
			{
				const std::array<c8, 2>& pair{ uhex2chars_digit_pairs<cap>[static_cast<u8>(u >> compact<(sizeof(u) - 1 - i) * 8>)] };
				p[compact<2 * i + 0>] = pair[0];
				p[compact<2 * i + 1>] = pair[1];
			}
		#else
			for (auto* pDst{ p + buffer_size }; pDst != p; u >>= 8)
			{
				pDst -= 2;
				const std::array<c8, 2>& pair{ uhex2chars_digit_pairs<cap>[static_cast<u8>(u)] };
				pDst[0] = pair[0];
				pDst[1] = pair[1];
			}
		#endif
		}
		else
		{
			uhex2chars_fixed<'0', cap>(u, p);
			u |= 1;
			const u8 lz{ Bits::leftZeroCount(u) };
			const u8 significant_bits{ static_cast<u8>(std::numeric_limits<decltype(u)>::digits - lz) };
			const u8 num_digits{ static_cast<u8>((significant_bits + 3) / 4) };
			const u8 padding_count{ static_cast<u8>(buffer_size - num_digits) };
			for (auto* pPad{p},* pEndPad{ p + padding_count }; pPad != pEndPad; ++pPad)
				*pPad = pad;
		}
	}
	return p + buffer_size;
}
template<c8 pad = ' ', Capitalized cap = Capitalized::yes, $Char Char = c8>
requires(pad == '0' || pad == ' ')
constexpr auto uhex2chars_fixed($UInt auto u)noexcept
{
	FixedString<Char, uhex2chars_required_buffer_size<decltype(u)>> result;
	uhex2chars_fixed<pad, cap>(u, result.data());
	return result;
}

template<Capitalized cap>
INLINE constexpr auto* uhex2chars($UInt auto u, $Char auto* p)noexcept//todo: u8
{
	if consteval { p[uhex2chars_required_buffer_size<decltype(u)> -1] = {}; }//test if buffer is big enough
	if constexpr (sizeof(u) >= 8)
	{
		if (!u) [[unlikely]]
		{
			*p = '0';
			return p + 1;
		}
		const u8 lz{ Bits::leftZeroCount(u) };
		const u8 significant_bits{ static_cast<u8>(std::numeric_limits<decltype(u)>::digits - lz) };
		const u8 num_digits{ static_cast<u8>((significant_bits + 3) / 4) };
		auto* ptr{ p + num_digits };
		while (ptr > p + 1)
		{
			const u8 byte{ static_cast<u8>(u) };
			const auto& pair{ uhex2chars_digit_pairs<cap>[byte] };
			ptr -= 2;
			ptr[0] = pair[0];
			ptr[1] = pair[1];
			u >>= 8;
		}
		if (ptr > p)
			*--ptr = uhex2chars_digit_pairs<cap>[static_cast<u8>(u)][1];
		return p + num_digits;
	}
	else
	{
	#define UHEX2CHARS_IMPL_1\
		if (u >= 0x10)\
		{\
			p[0] = pair0[0];p[1] = pair0[1];\
			return p + 2;\
		}\
		p[0] = pair0[1];\
		return p + 1;
	#define UHEX2CHARS_IMPL_2\
		if (u >= 0x1000)\
		{\
			p[0] = pair1[0];p[1] = pair1[1];\
			p[2] = pair0[0];p[3] = pair0[1];\
			return p + 4;\
		}\
		if (u >= 0x100)\
		{\
			p[0] = pair1[1];\
			p[1] = pair0[0];p[2] = pair0[1];\
			return p + 3;\
		}
	#define UHEX2CHARS_IMPL_4\
		if (u >= 0x10000000)\
		{\
			p[0] = pair3[0];p[1] = pair3[1];\
			p[2] = pair2[0];p[3] = pair2[1];\
			p[4] = pair1[0];p[5] = pair1[1];\
			p[6] = pair0[0];p[7] = pair0[1];\
			return p + 8;\
		}\
		if (u >= 0x1000000)\
		{\
			p[0] = pair3[1];\
			p[1] = pair2[0];p[2] = pair2[1];\
			p[3] = pair1[0];p[4] = pair1[1];\
			p[5] = pair0[0];p[6] = pair0[1];\
			return p + 7;\
		}\
		if (u >= 0x100000)\
		{\
			p[0] = pair2[0];p[1] = pair2[1];\
			p[2] = pair1[0];p[3] = pair1[1];\
			p[4] = pair0[0];p[5] = pair0[1];\
			return p + 6;\
		}\
		if (u >= 0x10000)\
		{\
			p[0] = pair2[1];\
			p[1] = pair1[0];p[2] = pair1[1];\
			p[3] = pair0[0];p[4] = pair0[1];\
			return p + 5;\
		}
		if constexpr (sizeof(u) == 4)
		{
			const auto& pair3{ uhex2chars_digit_pairs<cap>[u8(u >> 24)] };
			const auto& pair2{ uhex2chars_digit_pairs<cap>[u8(u >> 16)] };
			const auto& pair1{ uhex2chars_digit_pairs<cap>[u8(u >> 8)] };
			const auto& pair0{ uhex2chars_digit_pairs<cap>[u8(u >> 0)] };
			UHEX2CHARS_IMPL_4
			UHEX2CHARS_IMPL_2
			UHEX2CHARS_IMPL_1
		}
		if constexpr (sizeof(u) == 2)
		{
			const auto& pair1{ uhex2chars_digit_pairs<cap>[u8(u >> 8)] };
			const auto& pair0{ uhex2chars_digit_pairs<cap>[u8(u >> 0)] };
			UHEX2CHARS_IMPL_2
			UHEX2CHARS_IMPL_1
		}
		if constexpr (sizeof(u) == 1)
		{
			const auto& pair0{ uhex2chars_digit_pairs<cap>[u8(u >> 0)] };
			UHEX2CHARS_IMPL_1
		}
#undef UHEX2CHARS_IMPL_1
#undef UHEX2CHARS_IMPL_2
#undef UHEX2CHARS_IMPL_4
#undef UHEX2CHARS_IMPL_8
	}
}
template<Capitalized cap = Capitalized::yes, $Char Char = c8>
constexpr auto uhex2chars($UInt auto u)noexcept
{
	StaticString<Char, uhex2chars_required_buffer_size<decltype(u)>> result;
	result.modify_size(static_cast<IndexOf<decltype(result)>>(uhex2chars<cap>(u, result.data()) - result.data()));
	return result;
}
template<$SInt T>
constexpr auto sdec2chars_required_buffer_size{ compact<std::numeric_limits<T>::digits10 + 2> };
INLINE constexpr auto* sdec2chars($SInt auto s, $Char auto* p) noexcept
{
	using S = decltype(s);
	using U = std::make_unsigned_t<S>;
	if consteval { p[sdec2chars_required_buffer_size<S> - 1] = {}; }//test if buffer is big enough
	if (!s)
	{
		*p = '0';
		return p + 1;
	}
	U u{ static_cast<U>(s) };
	if (s < 0)
	{
		*p++ = '-';
		u = static_cast<U>(U{} - u);
	}
	return udec2chars<1>(u, p);
}
template<$Char Char = c8>
constexpr auto sdec2chars($SInt auto s)noexcept
{
	StaticString<Char, sdec2chars_required_buffer_size<decltype(s)>> result;
	result.modify_size(static_cast<IndexOf<decltype(result)>>(sdec2chars(s, result.data()) - result.data()));
	return result;
}
#include <string_view>
namespace details::num2charsTests
{
	template<FixedString s>CONSTEVAL bool verify_udec2chars($UInt auto u)noexcept /*                                                    */ { return std::string_view{ s } == std::string_view{ udec2chars(u) }; }
	template<FixedString s, Capitalized cap = Capitalized::yes>CONSTEVAL bool verify_uhex2chars($UInt auto u) noexcept /*               */ { return std::string_view{ s } == std::string_view{ uhex2chars<cap, c8>(u) }; }
	template<FixedString s>CONSTEVAL bool verify_sdec2chars($SInt auto val)noexcept /*                                                  */ { return std::string_view{ s } == std::string_view{ sdec2chars(val) }; }
	template<FixedString s, c8 pad = ' ', Capitalized cap = Capitalized::yes>CONSTEVAL bool verify_uhex2chars_fixed($UInt auto u) noexcept { return std::string_view{ s } == std::string_view{ uhex2chars_fixed<pad, cap, c8>(u) }; }
	template<FixedString s, c8 pad = ' '>CONSTEVAL bool verify_ubin2chars_fixed($UInt auto u) noexcept /*                               */ { return std::string_view{ s } == std::string_view{ ubin2chars_fixed<pad, c8>(u) }; }
	template<FixedString s>CONSTEVAL bool verify_ubin2chars($UInt auto u) noexcept /*                                                   */ { return std::string_view{ s } == std::string_view{ ubin2chars(u) }; }
	template<FixedString s, c8 pad = ' '>CONSTEVAL bool verify_udec2chars_fixed($UInt auto u) noexcept /*                               */ { return std::string_view{s} == std::string_view{udec2chars_fixed<pad, c8>(u)}; }
	STATIC_ASSERT(verify_udec2chars_fixed<"000", '0'>(u8(0)));
	STATIC_ASSERT(verify_udec2chars_fixed<"  0", ' '>(u8(0)));
	STATIC_ASSERT(verify_udec2chars_fixed<"255", '0'>(u8(255)));
	STATIC_ASSERT(verify_udec2chars_fixed<"00000", '0'>(u16(0)));
	STATIC_ASSERT(verify_udec2chars_fixed<"    0", ' '>(u16(0)));
	STATIC_ASSERT(verify_udec2chars_fixed<"65535", ' '>(u16(65535)));
	STATIC_ASSERT(verify_udec2chars_fixed<"0000000000", '0'>(u32(0)));
	STATIC_ASSERT(verify_udec2chars_fixed<"4294967295", '0'>(std::numeric_limits<u32>::max()));
	STATIC_ASSERT(verify_udec2chars_fixed<"00000000000000000000", '0'>(u64(0)));
	STATIC_ASSERT(verify_udec2chars_fixed<"18446744073709551615", '0'>(std::numeric_limits<u64>::max()));
	STATIC_ASSERT(verify_ubin2chars<"0">(u8(0)));
	STATIC_ASSERT(verify_ubin2chars<"1">(u8(1)));
	STATIC_ASSERT(verify_ubin2chars<"10">(u8(2)));
	STATIC_ASSERT(verify_ubin2chars<"111">(u8(7)));
	STATIC_ASSERT(verify_ubin2chars<"1111">(u8(15)));
	STATIC_ASSERT(verify_ubin2chars<"10000">(u8(16)));
	STATIC_ASSERT(verify_ubin2chars<"11111111">(u8(255)));
	STATIC_ASSERT(verify_ubin2chars<"0">(u16(0)));
	STATIC_ASSERT(verify_ubin2chars<"1">(u16(1)));
	STATIC_ASSERT(verify_ubin2chars<"10101010">(u16(0xAA)));
	STATIC_ASSERT(verify_ubin2chars<"100000000">(u16(256)));
	STATIC_ASSERT(verify_ubin2chars<"1111111111111111">(u16(0xFFFF)));
	STATIC_ASSERT(verify_ubin2chars<"0">(u32(0)));
	STATIC_ASSERT(verify_ubin2chars<"10101010101010101010101010101010">(u32(0xAAAAAAAA)));
	STATIC_ASSERT(verify_ubin2chars<"11111111111111111111111111111111">(u32(0xFFFFFFFF)));
	STATIC_ASSERT(verify_ubin2chars<"0">(u64(0)));
	STATIC_ASSERT(verify_ubin2chars<"1">(u64(1)));
	STATIC_ASSERT(verify_ubin2chars<"1111111111111111111111111111111111111111111111111111111111111111">(u64(0xFFFFFFFFFFFFFFFF)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"00000000", '0'>(u8(0)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"00001111", '0'>(u8(15)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"11111111", '0'>(u8(255)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"       0", ' '>(u8(0)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"       1", ' '>(u8(1)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"    1111", ' '>(u8(15)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"11111111", ' '>(u8(255)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"0000000000000000", '0'>(u16(0)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"0000000010101010", '0'>(u16(0xAA)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"               0", ' '>(u16(0)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"               1", ' '>(u16(1)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"        10101010", ' '>(u16(0xAA)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"00000000000000000000000000000000", '0'>(u32(0)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"11111111111111111111111111111111", '0'>(u32(0xFFFFFFFF)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"                               0", ' '>(u32(0)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"                        10101010", ' '>(u32(0xAA)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"0000000000000000000000000000000000000000000000000000000000000000", '0'>(u64(0)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"1111111111111111111111111111111111111111111111111111111111111111", '0'>(u64(0xFFFFFFFFFFFFFFFF)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"                                                               0", ' '>(u64(0)));
	STATIC_ASSERT(verify_ubin2chars_fixed<"1111111111111111111111111111111111111111111111111111111111111111", ' '>(u64(0xFFFFFFFFFFFFFFFF)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"00", '0'>(u8(0)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"0F", '0'>(u8(0xF)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"FF", '0'>(u8(0xFF)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"0000", '0'>(u16(0)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"00AB", '0'>(u16(0xAB)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"0ABC", '0'>(u16(0xABC)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"00000000", '0'>(u32(0)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"000000AB", '0'>(u32(0xAB)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"0000000000000000", '0'>(u64(0)));
	STATIC_ASSERT(verify_uhex2chars_fixed<" 0", ' '>(u8(0)));
	STATIC_ASSERT(verify_uhex2chars_fixed<" F", ' '>(u8(0xF)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"FF", ' '>(u8(0xFF)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"   0", ' '>(u16(0)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"  AB", ' '>(u16(0xAB)));
	STATIC_ASSERT(verify_uhex2chars_fixed<" ABC", ' '>(u16(0xABC)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"       0", ' '>(u32(0)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"      AB", ' '>(u32(0xAB)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"ABCDEF01", ' '>(u32(0xABCDEF01)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"00ab", '0', Capitalized::no>(u16(0xAB)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"  ab", ' ', Capitalized::no>(u16(0xAB)));
	STATIC_ASSERT(verify_uhex2chars_fixed<"abcdef01", '0', Capitalized::no>(u32(0xABCDEF01)));
	STATIC_ASSERT(verify_uhex2chars<"0">(u8{ 0 }));
	STATIC_ASSERT(verify_uhex2chars<"1">(u8{ 1 }));
	STATIC_ASSERT(verify_uhex2chars<"A">(u8{ 10 }));
	STATIC_ASSERT(verify_uhex2chars<"F">(u8{ 15 }));
	STATIC_ASSERT(verify_uhex2chars<"10">(u8{ 16 }));
	STATIC_ASSERT(verify_uhex2chars<"FF">(u8{ 255 }));
	STATIC_ASSERT(verify_uhex2chars<"FFFF">(65535u));
	STATIC_ASSERT(verify_uhex2chars<"FFFFFFFF">(std::numeric_limits<u32>::max()));
	STATIC_ASSERT(verify_uhex2chars<"FFFFFFFFFFFFFFFF">(std::numeric_limits<u64>::max()));
	STATIC_ASSERT(verify_uhex2chars<"ff", Capitalized::no>(255u));
	STATIC_ASSERT(verify_uhex2chars<"ffff", Capitalized::no>(65535u));
	STATIC_ASSERT(verify_uhex2chars<"ffffffff", Capitalized::no>(std::numeric_limits<u32>::max()));
	STATIC_ASSERT(verify_uhex2chars<"ffffffffffffffff", Capitalized::no>(std::numeric_limits<u64>::max()));
	STATIC_ASSERT(verify_uhex2chars<"100">(256u));
	STATIC_ASSERT(verify_uhex2chars<"ABC">(0xABCu));
	STATIC_ASSERT(verify_uhex2chars<"FFFF">(0xFFFFu));
	STATIC_ASSERT(verify_uhex2chars<"10000">(0x10000u));
	STATIC_ASSERT(verify_uhex2chars<"ABCDEF">(0xABCDEFu));
	STATIC_ASSERT(verify_uhex2chars<"FFFFFFFF">(0xFFFFFFFFu));
	STATIC_ASSERT(verify_uhex2chars<"100000000">(0x100000000ull));
	STATIC_ASSERT(verify_uhex2chars<"ABCDEF0123456789">(0xABCDEF0123456789ull));
	STATIC_ASSERT(verify_uhex2chars<"FFFFFFFFFFFFFFFF">(0xFFFFFFFFFFFFFFFFull));
	STATIC_ASSERT(verify_uhex2chars<"100", Capitalized::no>(256u));
	STATIC_ASSERT(verify_uhex2chars<"abc", Capitalized::no>(0xABCu));
	STATIC_ASSERT(verify_uhex2chars<"ffff", Capitalized::no>(0xFFFFu));
	STATIC_ASSERT(verify_uhex2chars<"10000", Capitalized::no>(0x10000u));
	STATIC_ASSERT(verify_uhex2chars<"abcdef", Capitalized::no>(0xABCDEFu));
	STATIC_ASSERT(verify_uhex2chars<"ffffffff", Capitalized::no>(0xFFFFFFFFu));
	STATIC_ASSERT(verify_uhex2chars<"100000000", Capitalized::no>(0x100000000ull));
	STATIC_ASSERT(verify_uhex2chars<"abcdef0123456789", Capitalized::no>(0xABCDEF0123456789ull));
	STATIC_ASSERT(verify_uhex2chars<"ffffffffffffffff", Capitalized::no>(0xFFFFFFFFFFFFFFFFull));
	STATIC_ASSERT(verify_udec2chars<"0">(0u));
	STATIC_ASSERT(verify_udec2chars<"1">(1u));
	STATIC_ASSERT(verify_udec2chars<"9">(9u));
	STATIC_ASSERT(verify_udec2chars<"10">(10u));
	STATIC_ASSERT(verify_udec2chars<"123">(123u));//fail
	STATIC_ASSERT(verify_udec2chars<"99">(99u));
	STATIC_ASSERT(verify_udec2chars<"100">(100u));//fail
	STATIC_ASSERT(verify_udec2chars<"999">(999u));
	STATIC_ASSERT(verify_udec2chars<"1000">(1000u));//fail
	STATIC_ASSERT(verify_udec2chars<"1000">(u16(1000)));//fail
	STATIC_ASSERT(verify_udec2chars<"1234">(u16(1234)));//fail
	STATIC_ASSERT(verify_udec2chars<"9999">(u16(9999)));
	STATIC_ASSERT(verify_udec2chars<"255">(std::numeric_limits<u8>::max()));
	STATIC_ASSERT(verify_udec2chars<"65535">(std::numeric_limits<u16>::max()));
	STATIC_ASSERT(verify_udec2chars<"4294967295">(std::numeric_limits<u32>::max()));
	STATIC_ASSERT(verify_udec2chars<"18446744073709551615">(std::numeric_limits<u64>::max()));
	STATIC_ASSERT(verify_sdec2chars<"0">(0));
	STATIC_ASSERT(verify_sdec2chars<"1">(1));
	STATIC_ASSERT(verify_sdec2chars<"-1">(-1));
	STATIC_ASSERT(verify_sdec2chars<"123">(123));
	STATIC_ASSERT(verify_sdec2chars<"-123">(-123));
	STATIC_ASSERT(verify_sdec2chars<"127">(std::numeric_limits<i8>::max()));
	STATIC_ASSERT(verify_sdec2chars<"-128">(std::numeric_limits<i8>::min()));
	STATIC_ASSERT(verify_sdec2chars<"32767">(std::numeric_limits<i16>::max()));
	STATIC_ASSERT(verify_sdec2chars<"-32768">(std::numeric_limits<i16>::min()));
	STATIC_ASSERT(verify_sdec2chars<"2147483647">(std::numeric_limits<i32>::max()));
	STATIC_ASSERT(verify_sdec2chars<"-2147483648">(std::numeric_limits<i32>::min()));
	STATIC_ASSERT(verify_sdec2chars<"9223372036854775807">(std::numeric_limits<i64>::max()));
	STATIC_ASSERT(verify_sdec2chars<"-9223372036854775808">(std::numeric_limits<i64>::min()));
}