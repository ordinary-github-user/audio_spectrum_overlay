#pragma once
#include "BasicTypes.h"
#include "ConceptLinearContainer.h"
#include "ConceptSig.h"
#include "ConceptSize.h"
#include "ElementOf.h"
#include "IndexTypeOf.h"
#include "Inline.h"
#include "UnsignedCast.h"
#define WTF_IN_RANGE(x,l,h) (UNSIGNED(x)>=UnsignedBytes<sizeof(std::remove_cvref_t<decltype(x)>)>{l}&&UNSIGNED(x)<=UnsignedBytes<sizeof(std::remove_cvref_t<decltype(x)>)>{h})
namespace Wtf8
{
	//1. 00–7F
	//2. C2–DF 80–BF
	//3. E0    A0–BF 80–BF
	//3. E1–EF 80–BF 80–BF
	//4. F0    90–BF 80–BF 80–BF
	//4. F1–F3 80–BF 80–BF 80–BF
	//4. F4    80–8F 80–BF 80–BF
	template<$Size<1> CodeUnit>
	constexpr bool valid(const CodeUnit* pBegin, const CodeUnit* pEnd)noexcept
	{
		bool prevHSurrogate{};
		while (pBegin < pEnd)
		{
			const u8 unit{ UNSIGNED(pBegin[0]) };
			/* */if (unit <= 0x7F) 
			{
				prevHSurrogate = false;
				pBegin += 1;
			}
			else if (unit <= 0xC1) [[unlikely]] return false;
			else if (unit <= 0xDF)
			{
				if (pEnd - pBegin < 2 || 
					!WTF_IN_RANGE(pBegin[1], 0x80, 0xbf)) [[unlikely]] 
					return false;
				prevHSurrogate = false;
				pBegin += 2;
			}
			else if (unit == 0xE0)
			{
				if (pEnd - pBegin < 3 || 
					!WTF_IN_RANGE(pBegin[1], 0xa0, 0xbf) || 
					!WTF_IN_RANGE(pBegin[2], 0x80, 0xbf)) [[unlikely]] 
					return false;
				prevHSurrogate = false;
				pBegin += 3;
			}
			else if (unit <= 0xEF)
			{
				if (pEnd - pBegin < 3 ||
					!WTF_IN_RANGE(pBegin[1], 0x80, 0xbf) ||
					!WTF_IN_RANGE(pBegin[2], 0x80, 0xbf)) [[unlikely]] 
					return false;
				if (prevHSurrogate && unit == 0xED && UNSIGNED(pBegin[1]) >= 0xB0)[[unlikely]]
					return false;
				prevHSurrogate = (unit == 0xED && UNSIGNED(pBegin[1]) >= 0xA0 && UNSIGNED(pBegin[1]) <= 0xAF);
				pBegin += 3;
			}
			else if (unit == 0xF0)
			{
				if (pEnd - pBegin < 4 || 
					!WTF_IN_RANGE(pBegin[1], 0x90, 0xbf) || 
					!WTF_IN_RANGE(pBegin[2], 0x80, 0xbf) || 
					!WTF_IN_RANGE(pBegin[3], 0x80, 0xbf)) [[unlikely]] 
					return false;
				prevHSurrogate = false;
				pBegin += 4;
			}
			else if (unit <= 0xF3)
			{
				if (pEnd - pBegin < 4 || 
					!WTF_IN_RANGE(pBegin[1], 0x80, 0xbf) || 
					!WTF_IN_RANGE(pBegin[2], 0x80, 0xbf) || 
					!WTF_IN_RANGE(pBegin[3], 0x80, 0xbf)) [[unlikely]] 
					return false;
				prevHSurrogate = false;
				pBegin += 4;
			}
			else if (unit == 0xF4)
			{
				if (pEnd - pBegin < 4 || 
					!WTF_IN_RANGE(pBegin[1], 0x80, 0x8f) || 
					!WTF_IN_RANGE(pBegin[2], 0x80, 0xbf) || 
					!WTF_IN_RANGE(pBegin[3], 0x80, 0xbf)) [[unlikely]] 
					return false;
				prevHSurrogate = false;
				pBegin += 4;
			}
			else [[unlikely]] return false;
		}
		return pBegin == pEnd;
	}

	constexpr bool valid($LinearContainer auto&& string)noexcept { return valid(string.data(), string.data() + string.size()); }

	//assume valid
	constexpr u8 code_point_size($Size<1> auto unit) noexcept
	{
		/* */if (UNSIGNED(unit) <= 0x7F) return 1;
		else if (UNSIGNED(unit) <= 0xDF) return 2;
		else if (UNSIGNED(unit) <= 0xEF) return 3;
		else /*                       */ return 4;
	}

	//assume valid
	template<$UInt Result = usize, $Size<1> CodeUnit>
	constexpr Result code_point_count(CodeUnit const* pBegin, CodeUnit const* pEnd)noexcept
	{
		Result result{};
		while (pBegin != pEnd)
		{
			pBegin += code_point_size(*pBegin);
			++result;
		}
		return result;
	}

	//assume valid
	constexpr auto code_point_count($LinearContainer auto&& string)noexcept
	{
		return code_point_count<IndexOf<decltype(string)>>(string.data(), string.data() + string.size());
	}

	//assume valid
	template<u8 size>
	requires(size >= 1 && size <= 4)
	constexpr u32 decode($Size<1> auto const* pBegin)noexcept
	{
		//u8 & u8 = i32
		/* */if constexpr (size == 1) return   UNSIGNED(pBegin[0]);
		else if constexpr (size == 2) return ((UNSIGNED(pBegin[0]) & u8(0b11111)) << 6) |  (UNSIGNED(pBegin[1]) & u8(0b111111));
		else if constexpr (size == 3) return ((UNSIGNED(pBegin[0]) & u8(0b1111)) << 12) | ((UNSIGNED(pBegin[1]) & u8(0b111111)) << 6)  | ((UNSIGNED(pBegin[2]) & u8(0b111111)));
		else if constexpr (size == 4) return ((UNSIGNED(pBegin[0]) & u8(0b111)) << 18)  | ((UNSIGNED(pBegin[1]) & u8(0b111111)) << 12) | ((UNSIGNED(pBegin[2]) & u8(0b111111)) << 6) | ((UNSIGNED(pBegin[3]) & u8(0b111111)));
	}

	//assume valid
	template<$Size<1> CodeUnit>
	constexpr void decode(const CodeUnit* pBegin, const CodeUnit* pEnd, $Sig<void(const u32)> auto&& action)noexcept
	{
		const CodeUnit* pUnit{ pBegin };
		while (pUnit != pEnd)
		{
			const u8 unit{ u8(*pUnit) };
			u32 codePoint;
			if (unit <= 0x7F)					//1 byte
			{
				codePoint = unit;
				pUnit += 1;
			}
			else if (unit <= 0xDF)				//2 byte
			{
				codePoint = ((unit & u8(0b11111)) << 6) | (pUnit[1] & u8(0b111111));
				pUnit += 2;
			}
			else if (unit <= 0xEF)				//3 byte
			{
				codePoint = ((unit & u8(0b1111)) << 12) | ((pUnit[1] & u8(0b111111)) << 6) | ((pUnit[2] & u8(0b111111)));
				pUnit += 3;
			}
			else
			{
				codePoint = ((unit & u8(0b111)) << 18) | ((pUnit[1] & u8(0b111111)) << 12) | ((pUnit[2] & u8(0b111111)) << 6) | ((pUnit[3] & u8(0b111111)));
				pUnit += 4;
			}
			action(codePoint);
		}
	}

	//assume valid
	constexpr void decode($LinearContainer auto&& container, $Sig<void(const u32)> auto&& action)noexcept
	{
		decode(container.data(), container.data() + container.size(), FWD(action));
	}

	//assume valid
	constexpr void encode_unsafe(u32 codePoint, $Sig<void(const c8)> auto&& action)noexcept
	{
		if (codePoint < 0x80)			//1 bytes
			action(c8(codePoint));
		else if (codePoint < 0x800)		//2 bytes
		{
			action(c8(u8(codePoint >> 6) | u8(0b11000000)));
			action(c8(u8(u8(codePoint) & u8(0b111111)) | u8(0b10000000)));
		}
		else if (codePoint < 0x10000)	//3 bytes
		{
			action(c8(u8(codePoint >> 12) | u8(0b11100000)));
			action(c8(u8(u8(codePoint >> 6) & u8(0b111111)) | u8(0b10000000)));
			action(c8(u8(u8(codePoint) & u8(0b111111)) | u8(0b10000000)));
		}
		else							//4 bytes
		{
			action(c8(u8(codePoint >> 18) | u8(0b11110000)));
			action(c8(u8(u8(codePoint >> 12) & u8(0b111111)) | u8(0b10000000)));
			action(c8(u8(u8(codePoint >> 6) & u8(0b111111)) | u8(0b10000000)));
			action(c8(u8(u8(codePoint) & u8(0b111111)) | u8(0b10000000)));
		}
	}

	constexpr bool encode(u32 codePoint, $Sig<void(const c8)> auto&& action)noexcept
	{
		if (codePoint < 0x80)			//1 bytes
		{
			action(c8(codePoint));
			return true;
		}
		else if (codePoint < 0x800)		//2 bytes
		{
			action(c8(u8(codePoint >> 6) | u8(0b11000000)));
			action(c8(u8(u8(codePoint) & u8(0b111111)) | u8(0b10000000)));
			return true;
		}
		else if (codePoint < 0x10000)	//3 bytes
		{
			action(c8(u8(codePoint >> 12) | u8(0b11100000)));
			action(c8(u8(u8(codePoint >> 6) & u8(0b111111)) | u8(0b10000000)));
			action(c8(u8(u8(codePoint) & u8(0b111111)) | u8(0b10000000)));
			return true;
		}
		else if (codePoint < 0x110000)	//4 bytes
		{
			action(c8(u8(codePoint >> 18) | u8(0b11110000)));
			action(c8(u8(u8(codePoint >> 12) & u8(0b111111)) | u8(0b10000000)));
			action(c8(u8(u8(codePoint >> 6) & u8(0b111111)) | u8(0b10000000)));
			action(c8(u8(u8(codePoint) & u8(0b111111)) | u8(0b10000000)));
			return true;
		}
		else
			return false;
	}
}
namespace Wtf16
{
	template<$Size<2> CodeUnit>
	constexpr bool valid(const CodeUnit*, const CodeUnit*)noexcept { return true; }

	constexpr bool valid($LinearContainer auto&& string)noexcept { return valid(string.data(), string.data() + string.size()); }

	//assume valid
	template<$UInt Result = usize, $Size<2> CodeUnit>
	constexpr Result code_point_count(CodeUnit const* pBegin, CodeUnit const* pEnd)noexcept
	{
		Result result{};
		while (pBegin != pEnd)
		{
			pBegin += (WTF_IN_RANGE(pBegin[0], 0xD800, 0xDBFF) && pEnd - pBegin >= 2 && WTF_IN_RANGE(pBegin[1], 0xDC00, 0xDFFF)) ? 2 : 1;
			++result;
		}
		return result;
	}

	//assume valid
	constexpr auto code_point_count($LinearContainer auto&& string)noexcept
	{
		return code_point_count<IndexOf<decltype(string)>>(string.data(), string.data() + string.size());
	}

	//assume valid
	template<u8 size>
	requires(size >= 1 && size <= 2)
	constexpr u32 decode($Size<2> auto const* pBegin)noexcept
	{
		/* */if constexpr (size == 1) return UNSIGNED(pBegin[0]);
		else if constexpr (size == 2) return (u32{ UNSIGNED(pBegin[0]) } - u32(0xD800)) * u32(0x400) + u32{ UNSIGNED(pBegin[1]) } - u32(0xDC00) + u32(0x10000);
	}

	//assume valid
	template<$Size<2> CodeUnit>
	constexpr void decode(const CodeUnit* pBegin, const CodeUnit* pEnd, $Sig<void(const u32)> auto&& action)noexcept
	{
		while (pBegin != pEnd)
		{
			const u16 unit{ UNSIGNED(pBegin[0])};
			u32 codePoint;
			if (WTF_IN_RANGE(unit, 0xD800, 0xDBFF) && pEnd - pBegin >= 2 && WTF_IN_RANGE(pBegin[1], 0xDC00, 0xDFFF))
			{
				const u32 h{ (u32{unit} - u32(0xD800)) * u32(0x400) };
				const u32 l{ u32{UNSIGNED(pBegin[1])} - u32(0xDC00) };
				codePoint = h + l + u32(0x10000);
				pBegin += 2;
			}
			else
			{
				codePoint = unit;
				pBegin += 1;
			}
			action(codePoint);
		}
	}

	//assume valid
	constexpr void decode($LinearContainer auto&& container, $Sig<void(const u32)> auto&& action)noexcept
	{
		decode(container.data(), container.data() + container.size(), FWD(action));
	}

	//assume valid
	constexpr void encode_unsafe(u32 codePoint, $Sig<void(const c16)> auto&& action)noexcept
	{
		if (codePoint <= 0xFFFF)
			action(c16(codePoint));
		else
		{
			codePoint -= 0x10000;
			action(c16((codePoint >> 10) + 0xD800));
			action(c16((codePoint & 0x3FF) + 0xDC00));
		}
	}

	constexpr bool encode(u32 codePoint, $Sig<void(const c16)> auto&& action)noexcept
	{
		if (codePoint >= 0x110000)[[unlikely]]
			return false;
		if (codePoint <= 0xFFFF)
			action(c16(codePoint));
		else
		{
			codePoint -= 0x10000;
			action(c16((codePoint >> 10) + 0xD800));
			action(c16((codePoint & 0x3FF) + 0xDC00));
		}
		return true;
	}
}
#undef WTF_IN_RANGE