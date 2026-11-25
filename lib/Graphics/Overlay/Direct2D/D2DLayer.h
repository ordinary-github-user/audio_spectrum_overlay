#pragma once

#include "D2DHwndRenderTarget.h"
namespace Direct2D
{
	struct Layer
	{
	private:
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
		using color_t = Rgba<f32>;
		ComPtr<ID2D1Layer> m_layer;

		Layer(const HwndRenderTarget& renderTarget, const D2D1_SIZE_F* pSize)
		{
			if (FAILED(renderTarget.ptr()->CreateLayer(pSize, m_layer.GetAddressOf())))
				CRASH();
		}
	public:

		Layer(const HwndRenderTarget& renderTarget, const D2D1_SIZE_F& size) :Layer{ renderTarget, &size } {}
		Layer(const HwndRenderTarget& renderTarget) :Layer{ renderTarget, nullptr } {}

		ID2D1Layer* ptr()const noexcept { return m_layer.Get(); }
	};
}