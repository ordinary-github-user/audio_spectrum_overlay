#pragma once
#include "BasicTypes.h"
#include "ConceptSig.h"
#include "ConceptSize.h"
#include "ConceptString.h"
#include "IndexTypeOf.h"
#include "StaticError.h"
#include "Unreachable.h"
//https://github.com/ww898/utf-cpp
enum class HaltAtNull : u8 { no, yes };
namespace Utf8
{
	constexpr u8 codePointSize($Size<1> auto firstCodeUnit) noexcept
	{
		const u8 unit{ static_cast<u8>(firstCodeUnit) };
		/* */if (unit <= 0x7F) return 1;
		else if (unit <= 0xBF) [[unlikely]] return 0;//invalid
		else if (unit <= 0xDF) return 2;
		else if (unit <= 0xEF) return 3;
		else if (unit <= 0xF7) return 4;
		else[[unlikely]] return 0;//invalid
	}

	//size = 1/2/3/4, otherwise ub
	constexpr u32 decode($Size<1> auto const* pBegin, u8 size)noexcept
	{
		switch (size)
		{
			case 1:return pBegin[0];
			case 2:return ((pBegin[0] & u8(0b11111)) << 6) | (pBegin[1] & u8(0b111111));
			case 3:return ((pBegin[0] & u8(0b1111)) << 12) | ((pBegin[1] & u8(0b111111)) << 6) | ((pBegin[2] & u8(0b111111)));
			case 4:return ((pBegin[0] & u8(0b111)) << 18) | ((pBegin[1] & u8(0b111111)) << 12) | ((pBegin[2] & u8(0b111111)) << 6) | ((pBegin[3] & u8(0b111111)));
			default:UNREACHABLE;
		}
	}

	template<HaltAtNull haltAtNull = HaltAtNull::no, $Size<1> CodeUnit>
	constexpr const CodeUnit* decode(const CodeUnit* pBegin, const CodeUnit* pEnd, $Sig<void(const u32)> auto&& action)noexcept
	{
		const CodeUnit* pUnit{ pBegin };
		while (pUnit != pEnd)
		{
			const u8 unit{ u8(*pUnit) };
			if constexpr (haltAtNull == HaltAtNull::yes)if (unit == 0)return pUnit;
			u32 codePoint;
			if (unit <= 0x7F)					//1 byte
			{
				codePoint = unit;
				pUnit += 1;
			}
			else if (unit <= 0xBF) [[unlikely]]	//Invalid
				return pUnit;
			else if (unit <= 0xDF)				//2 byte
			{
				if (pUnit + 1 >= pEnd)return pUnit;
				codePoint =
					((unit & u8(0b11111)) << 6) |
					(pUnit[1] & u8(0b111111));
				pUnit += 2;
			}
			else if (unit <= 0xEF)				//3 byte
			{
				if (pUnit + 2 >= pEnd)return pUnit;
				codePoint =
					((unit & u8(0b1111)) << 12) |
					((pUnit[1] & u8(0b111111)) << 6) |
					((pUnit[2] & u8(0b111111)));
				pUnit += 3;
			}
			else if (unit <= 0xF7)				//4 byte
			{
				if (pUnit + 3 >= pEnd)return pUnit;
				codePoint =
					((unit & u8(0b111)) << 18) |
					((pUnit[1] & u8(0b111111)) << 12) |
					((pUnit[2] & u8(0b111111)) << 6) |
					((pUnit[3] & u8(0b111111)));
				pUnit += 4;
			}
			else[[unlikely]]
				return pUnit;
			action(codePoint);
		}
		return pUnit;
	}

	template<HaltAtNull haltAtNull = HaltAtNull::no, class Container>
	constexpr auto decode(Container&& container, $Sig<void(const u32)> auto&& action)noexcept
	{
		return decode<haltAtNull>(container.data(), container.data() + container.size(), action);
	}

	constexpr bool encode(u32 codePoint, $Sig<void(const c8)> auto&& action)noexcept//TODO
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

	template<HaltAtNull haltAtNull = HaltAtNull::no>
	constexpr auto size($StringOfCharSize<1> auto&& string)noexcept
	{
		IndexOf<decltype(string)> result{};
		decode<haltAtNull>(string.data(), string.data() + string.size(), [&](auto)LAMBDA_INLINE{ ++result; });
		return result;
	}
};

namespace Utf16
{
	constexpr u8 codePointSize($Size<2> auto firstCodeUnit) noexcept
	{
		const u16 unit{ static_cast<u16>(firstCodeUnit) };
		return (unit >= u16(0xD800) && unit < u16(0xE000)) ? 2 : 1;
	}

	//size = 1/2, otherwise ub
	constexpr u32 decode($Size<2> auto const* pBegin, u8 size)noexcept
	{
		//https://github.com/ww898/utf-cpp
		switch (size)
		{
			case 1:return pBegin[0];
			case 2:
			{
				const u32 highSurrogate{ (u32{pBegin[0]} - u32(0xD800)) * u32(0x400) };
				const u32 lowSurrogate{ u32{pBegin[1]} - u32(0xDC00) };
				return highSurrogate + lowSurrogate + u32(0x10000);
			}
			default:UNREACHABLE;
		}
	}

	template<HaltAtNull haltAtNull = HaltAtNull::no, $Size<2> CodeUnit>
	constexpr const CodeUnit* decode(const CodeUnit* pBegin, const CodeUnit* pEnd, $SigAnyOf<void(const u32), bool(const u32)> auto&& action)noexcept
	{
		const CodeUnit* pUnit{ pBegin };
		while (pUnit != pEnd)
		{
			const u16 unit{ static_cast<u16>(*pUnit) };
			if constexpr (haltAtNull == HaltAtNull::yes)
				if (unit == 0)
					return pUnit;
			u32 codePoint;
			if (unit >= u16(0xD800) && unit < u16(0xE000))
			{
				if (pUnit + 1 == pEnd)
					return pUnit;
				const u32 highSurrogate{ (u32{unit} - u32(0xD800)) * u32(0x400) };
				const u32 lowSurrogate{ u32{pUnit[1]} - u32(0xDC00) };
				codePoint = highSurrogate + lowSurrogate + u32(0x10000);
				pUnit += 2;
			}
			else
			{
				pUnit += 1;
				codePoint = unit;
			}
			/* */if constexpr ($Sig<decltype(action), void(const u32)>) action(codePoint);
			else if constexpr ($Sig<decltype(action), bool(const u32)>) { if (!action(codePoint)) return pUnit; }
			else STATIC_ERROR();
		}
		return pEnd;
	}

	template<HaltAtNull haltAtNull = HaltAtNull::no, class Container>
	constexpr auto decode(Container&& container, $SigAnyOf<void(const u32), bool(const u32)> auto&& action)noexcept
	{
		return decode<haltAtNull>(container.data(), container.data() + container.size(), action);
	}

	constexpr bool encode(u32 codePoint, $Sig<void(const c16)> auto&& action)noexcept
	{
		if ((codePoint >= 0xD800 && codePoint < 0xE000) || codePoint >= 0x110000)
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

	template<HaltAtNull haltAtNull = HaltAtNull::no>
	constexpr auto size($StringOfCharSize<2> auto&& string)noexcept
	{
		IndexOf<decltype(string)> result{};
		decode<haltAtNull>(string.data(), string.data() + string.size(), [&](auto)LAMBDA_INLINE{ ++result; });
		return result;
	}
}