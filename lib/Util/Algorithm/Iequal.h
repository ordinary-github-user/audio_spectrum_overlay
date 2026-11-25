#pragma once
#include "../BasicTypes.h"
#include "../Concepts/LinearContainer.h"
#include "../Concepts/Same.h"
#include "../ElementOf.h"
#include "../Inline.h"
#include "../TextEncoding/TextEncoding.h"
namespace Algorithm
{
	template<$AnyOf<c8,c16,u32> Char>INLINE constexpr Char tolower(Char c)noexcept{return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;}
	template<$AnyOf<c8,c16,u32> Char>INLINE constexpr Char toupper(Char c)noexcept{return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;}

	constexpr bool iequal($LinearContainerAnyOf<c8, c16> auto&& l, $LinearContainerAnyOf<c8, c16> auto&& r)noexcept
	requires(sizeof(ElementOf<decltype(l)>) == sizeof(ElementOf<decltype(r)>))
	{
		const auto size{ l.size() };
		if (size != r.size())return false;
		for (std::remove_cvref_t<decltype(size)> i{}; i != size; ++i)
			if (toupper(l[i]) != toupper(r[i]))
				return false;
		return true;
	}

	constexpr bool iequal($LinearContainerAnyOf<c8> auto&& l, $LinearContainerAnyOf<c16> auto&& r)noexcept
	{
		const c8* pL{ l.data() };
		const c8* pEndL{ l.data() + l.size() };
		const c16* pR{ r.data() };
		const c16* pEndR{ r.data() + r.size() };
		for (;;)
		{
			if (pL == pEndL)return pR == pEndR;
			if (pR == pEndR)return pL == pEndL;
			const u8 s8 { Utf8 ::codePointSize(*pL) }; if (!s8)  [[unlikely]] return false;
			const u8 s16{ Utf16::codePointSize(*pR) }; if (!s16) [[unlikely]] return false;
			if (pEndL - pL < s8)  [[unlikely]] return false;
			if (pEndR - pR < s16) [[unlikely]] return false;
			if (toupper(Utf8::decode(pL, s8)) != toupper(Utf16::decode(pR, s16)))return false;
			pL += s8;
			pR += s16;
		}
	}

	INLINE constexpr bool iequal($LinearContainerAnyOf<c16> auto&& l, $LinearContainerAnyOf<c8> auto&& r)noexcept
	{
		return iequal(FWD(r), FWD(l));
	}
}