#pragma once
#include "TString.h"
#include "D2DGeometry.h"
#include "DWriteFactory.h"
#include "D2DTextFormat.h"
#include <wrl/client.h>
#include "StringView.h"
#include <dwrite_3.h>
#include "NoCopyNoMove.h"
namespace Direct2D
{
	struct TextLayout:NoCopy
	{
	private:
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

		ComPtr<IDWriteTextLayout> m_pTextLayout;
	public:
		TextLayout(StringView16 str, const TextFormat& textFormat, f32 maxWidth = FLT_MAX, f32 maxHeight = FLT_MAX)noexcept
		{
			pDwriteFactory->CreateTextLayout(str.data(), static_cast<u32>(str.size()), textFormat.ptr(), maxWidth, maxHeight, m_pTextLayout.GetAddressOf());
		}

		IDWriteTextLayout* ptr()const noexcept{ return m_pTextLayout.Get(); }

		Math::float2 metrics()const noexcept
		{
			const DWRITE_TEXT_METRICS raw{ rawMetrics() };
			return { raw.width, raw.height };
		}

		DWRITE_TEXT_METRICS rawMetrics()const noexcept
		{
			DWRITE_TEXT_METRICS result;
			m_pTextLayout->GetMetrics(&result);
			return result;
		}

		TextLayout(TextLayout&& )noexcept = default;
		TextLayout& operator=(TextLayout&&)noexcept = default;
	};
}