#pragma once
#include <dwrite_3.h>
#include <wrl/client.h>
#include "../../../Util/DataStructures/StringSso.h"
#include "../../../Util/DataStructures/StringView.h"
#include "../../../Util/DataStructures/TString.h"
#include "../../../Util/Tags/Uninitialized.h"
#include "D2DGeometry.h"
#include "DWriteFactory.h"

namespace Direct2D
{
	struct TextFormat
	{
	private:
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

		ComPtr<IDWriteTextFormat> m_pTextFormat;
	public:
		TextFormat(Uninitialized)noexcept :m_pTextFormat{} {};
		TextFormat(
			f32 size = 20.f,
			const c16* fontFamily = S16(L"consolas\0").data(),
			DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_EXTRA_LIGHT, 
			DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL, 
			DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_ULTRA_CONDENSED,
			const c16* localName= S16(L"en-us\0").data())noexcept
		{
			pDwriteFactory->CreateTextFormat(fontFamily, nullptr, weight, style, stretch, size, localName, m_pTextFormat.GetAddressOf());
		}

		TextFormat(
			f32 size,
			StringView16 fontFamily,
			DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_EXTRA_LIGHT,
			DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_ULTRA_CONDENSED,
			const c16* localName = S16(L"en-us\0").data())noexcept:TextFormat{size, nullTerminated(fontFamily).data(), weight, style, stretch, localName}
		{}

		explicit operator bool()const noexcept { return ptr(); }
		IDWriteTextFormat* ptr()const noexcept{ return m_pTextFormat.Get(); }
		f32 size()const noexcept { return m_pTextFormat->GetFontSize(); }
		String16 name()const noexcept 
		{
			const u32 len{ m_pTextFormat->GetFontFamilyNameLength() };
			String16 result{ uninitialized, len + 1 };
			m_pTextFormat->GetFontFamilyName(result.data(), static_cast<u32>(len + 1));
			result.truncate(len);
			return result;
		}
	};
}