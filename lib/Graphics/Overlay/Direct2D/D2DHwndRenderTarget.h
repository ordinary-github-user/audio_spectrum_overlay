#pragma once
#include <d2d1_1.h>
#include <wrl/client.h>
#include "../../../Math/Tensor/Tensor.h"
#include "D2DFactory.h"
namespace Direct2D
{
	struct HwndRenderTarget
	{
	private:
		Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
	public:
		HwndRenderTarget(HWND hwnd, const Math::Vector2<u32>& size)noexcept
		{
			if (S_OK != pFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(
					D2D1_RENDER_TARGET_TYPE::D2D1_RENDER_TARGET_TYPE_DEFAULT,
					D2D1::PixelFormat(DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED)
				),
				D2D1::HwndRenderTargetProperties(
					hwnd,
					D2D1::SizeU(size[0], size[1]),
					D2D1_PRESENT_OPTIONS_IMMEDIATELY
				),
				m_pRenderTarget.GetAddressOf()))
				CRASH();

			m_pRenderTarget->SetDpi(96, 96);
		}
		ID2D1HwndRenderTarget* operator->() const noexcept { return m_pRenderTarget.Get(); }
		ID2D1HwndRenderTarget* ptr()const noexcept { return m_pRenderTarget.Get(); }

		Math::float2 getDpi()const noexcept
		{
			Math::float2 result;
			m_pRenderTarget->GetDpi(&result[0], &result[1]);
			return result;
		}

		void setDpi(const Math::float2& dpi)const noexcept
		{
			m_pRenderTarget->SetDpi(dpi[0], dpi[1]);
		}

		void getTransform(Math::float3x2& result)const noexcept
		{
			m_pRenderTarget->GetTransform(reinterpret_cast<D2D1::Matrix3x2F*>(&result));
		}

		void setAntialiasMode(D2D1_ANTIALIAS_MODE mode)const noexcept { return m_pRenderTarget->SetAntialiasMode(mode); }
		void setTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE mode)const noexcept { return m_pRenderTarget->SetTextAntialiasMode(mode); }
		D2D1_ANTIALIAS_MODE getAntialiasMode()const noexcept { return m_pRenderTarget->GetAntialiasMode(); }
		D2D1_TEXT_ANTIALIAS_MODE getTextAntialiasMode()const noexcept { return m_pRenderTarget->GetTextAntialiasMode(); }
	};
}