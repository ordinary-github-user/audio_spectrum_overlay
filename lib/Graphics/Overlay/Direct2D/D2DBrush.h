#pragma once
#include <d2d1_1.h>
#include <utility>
#include <wrl/client.h>
#include "../../../Util/Macro/MovForward.h"

#include "../../../Math/Color.h"
#include "../../../Math/Tensor/Tensor.h"
#include "../../../Util/Concepts/Same.h"
#include "../../../Util/NoCopyNoMove.h"
#include "D2DHwndRenderTarget.h"
namespace Direct2D
{
	struct GradientStop
	{
		f32 position;
		Rgba<f32> color;
	};

	struct GradientStops
	{
	private:
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
		using color_t = Rgba<f32>;
		ComPtr<ID2D1GradientStopCollection> m_pStops;
	public:

		GradientStops(const HwndRenderTarget& renderTarget, Span<GradientStop> stops)
		{
			if (FAILED(renderTarget.ptr()->CreateGradientStopCollection(reinterpret_cast<const D2D1_GRADIENT_STOP*>(stops.data()), static_cast<u32>(stops.size()), m_pStops.GetAddressOf())))
				CRASH();
		}

		ID2D1GradientStopCollection* ptr()const noexcept{ return m_pStops.Get(); }
	};

	namespace Brush
	{
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
		using color_t = Rgba<f32>;

		struct Solid:NoCopy
		{
		private:
			ComPtr<ID2D1SolidColorBrush> m_pSolidBrush;

			Solid(Solid&& rhs)noexcept :m_pSolidBrush{ MOV(rhs.m_pSolidBrush) } {}
			Solid& operator=(Solid&& rhs)noexcept { m_pSolidBrush = MOV(rhs.m_pSolidBrush); return *this; }

		public:
			Solid(const HwndRenderTarget& renderTarget, const color_t& color = { 0,0,0,0 })
			{
				if (FAILED(renderTarget.ptr()->CreateSolidColorBrush(reinterpret_cast<const D2D1::ColorF&>(color), m_pSolidBrush.GetAddressOf())))
					CRASH();
			}
			void setColor(const color_t& color)const noexcept{ m_pSolidBrush->SetColor(reinterpret_cast<const D3DCOLORVALUE&>(color)); }
			ID2D1SolidColorBrush* ptr()const noexcept{ return m_pSolidBrush.Get(); }
		};

		struct Radial:NoCopy
		{
		private:
			ComPtr<ID2D1RadialGradientBrush> m_pRadialBrush;
			Radial(Radial&& rhs)noexcept :m_pRadialBrush{ MOV(rhs.m_pRadialBrush) } {}
			Radial& operator=(Radial&& rhs)noexcept { m_pRadialBrush = MOV(rhs.m_pRadialBrush); return *this; }
		public:

			Radial(const HwndRenderTarget& renderTarget, const GradientStops& stops, const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& gradientBrushProperties, const D2D1_BRUSH_PROPERTIES& brushProperties)
			{
				if (FAILED(renderTarget.ptr()->CreateRadialGradientBrush(gradientBrushProperties, brushProperties, stops.ptr(), m_pRadialBrush.GetAddressOf())))
					CRASH();
			}

			Radial(const HwndRenderTarget& renderTarget, const GradientStops& stops, const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& gradientBrushProperties)
			{
				if (FAILED(renderTarget.ptr()->CreateRadialGradientBrush(gradientBrushProperties, stops.ptr(), m_pRadialBrush.GetAddressOf())))
					CRASH();
			}

			Radial(const HwndRenderTarget& renderTarget, const GradientStops& stops, const Math::float2& center, const Math::float2& gradientOriginOffset, const Math::float2& radiuses)noexcept :
				Radial{ renderTarget,stops,{ reinterpret_cast<const D2D1_POINT_2F&>(center),reinterpret_cast<const D2D1_POINT_2F&>(gradientOriginOffset),radiuses[0],radiuses[1] } } {}

			Radial(const HwndRenderTarget& renderTarget, const GradientStops& stops, const Math::float2& center, f32 radius, const Math::float2& gradientOriginOffset = { 0,0 })noexcept :
				Radial{ renderTarget,stops,center,gradientOriginOffset, {radius,radius} } {}
		};
	}
	template<class T>concept $Brush = $Same<Brush::Solid, std::remove_cvref_t<T>>;
}