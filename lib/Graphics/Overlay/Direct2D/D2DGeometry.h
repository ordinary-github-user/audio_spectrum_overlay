#pragma once
#include <wrl/client.h>
#include "../../../Math/Geometry2d.h"
#include "../../../Math/Tensor/Tensor.h"
#include "../../../Util/Concepts/LinearContainer.h"
#include "../../../Util/DataStructures/Span.h"
#include "../../../Util/Macro/MovForward.h"
#include "../../../Util/ScopeGuard.h"
#include "../../../Windows/WinAPI/DWrite.h"
#include "D2DFactory.h"
#include "D2DHwndRenderTarget.h"

namespace Direct2D
{
	template<bool Solid>
	struct SimplePathGeometry
	{
	//private:
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

		ComPtr<ID2D1PathGeometry> m_pPathGeometry;
		ComPtr<ID2D1SimplifiedGeometrySink> m_pSink;

		bool init()noexcept
		{
			return
				S_OK == pFactory->CreatePathGeometry(m_pPathGeometry.GetAddressOf()) &&
				S_OK == m_pPathGeometry->Open(reinterpret_cast<ID2D1GeometrySink**>(m_pSink.GetAddressOf()));
		}

	public:
		SimplePathGeometry()noexcept : m_pPathGeometry{}, m_pSink{} {}

		SimplePathGeometry(ComPtr<ID2D1PathGeometry>&& pPathGeometry, ComPtr<ID2D1GeometrySink>&& pSink)noexcept :m_pPathGeometry{ MOV(pPathGeometry) }, m_pSink{ MOV(pSink) } {}

		SimplePathGeometry(const DWRITE_GLYPH_RUN* pGlyphRun)noexcept //: m_pPathGeometry{}, m_pSink{}
		{
			STATIC_ASSERT(Solid);
			init();

			pGlyphRun->fontFace->GetGlyphRunOutline(
				pGlyphRun->fontEmSize,
				pGlyphRun->glyphIndices,
				pGlyphRun->glyphAdvances,
				pGlyphRun->glyphOffsets,
				pGlyphRun->glyphCount,
				pGlyphRun->isSideways,
				pGlyphRun->bidiLevel % 2,
				m_pSink.Get());

			m_pSink->Close();
		}

		SimplePathGeometry($LinearContainerOf<Math::float2> auto&& vertices, D2D1_FIGURE_END end = D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED) :m_pPathGeometry{}, m_pSink{}
		{
			if (vertices.size() < 2)return;
			init();
			beginFigure(vertices[0]);
			addLines(&vertices[1], static_cast<UINT32>(vertices.size() - 1));
			endFigure(end);
			m_pSink->Close();
		}

		void unionWith(SimplePathGeometry& prev)
		{
			Microsoft::WRL::ComPtr<ID2D1PathGeometry> pNewGeometry;
			Direct2D::pFactory->CreatePathGeometry(&pNewGeometry);
			Microsoft::WRL::ComPtr<ID2D1SimplifiedGeometrySink> pNewSink;
			pNewGeometry->Open(reinterpret_cast<ID2D1GeometrySink**>(pNewSink.GetAddressOf()));
			pNewSink->SetFillMode(D2D1_FILL_MODE_WINDING);
			prev.pPath()->CombineWithGeometry(
				pPath(),
				D2D1_COMBINE_MODE_UNION,
				nullptr,
				D2D1_DEFAULT_FLATTENING_TOLERANCE,
				pNewSink.Get()
			);
			pNewSink->Close();
			m_pPathGeometry = MOV(pNewGeometry);
			m_pSink = MOV(pNewSink);
		}

		void beginFigure(const Math::float2& point)noexcept { m_pSink->BeginFigure(reinterpret_cast<const D2D1_POINT_2F&>(point), Solid ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW); }
		void addLines(const Math::float2* pPoints, u32 count)noexcept { m_pSink->AddLines(reinterpret_cast<const D2D1_POINT_2F*>(pPoints), count); }
		//void addLine(const Math::float2& point)noexcept{ m_pSink->AddLine(reinterpret_cast<const D2D1_POINT_2F&>(point)); }
		//void addBezier(const Math::float2& p0, const Math::float2& p1, const Math::float2& p2)noexcept{ m_pSink->AddBezier({ reinterpret_cast<const D2D1_POINT_2F&>(p0) ,reinterpret_cast<const D2D1_POINT_2F&>(p1) ,reinterpret_cast<const D2D1_POINT_2F&>(p2) }); }
		void endFigure(D2D1_FIGURE_END end = D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED)noexcept { m_pSink->EndFigure(end); }

		D2D1_RECT_F getBounds(const D2D1::Matrix3x2F* pWorldTransform = nullptr)noexcept
		{
			D2D1_RECT_F result;
			m_pPathGeometry->GetBounds(pWorldTransform, &result);
			return result;
		}

		ID2D1PathGeometry* pPath()const noexcept { return m_pPathGeometry.Get(); }
	};
	using SimplePathGeometrySolid = SimplePathGeometry<1>;
	using SimplePathGeometryHollow = SimplePathGeometry<0>;

	template<bool Solid>
	struct PathGeometry
	{
	//private:
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

		ComPtr<ID2D1PathGeometry> m_pPathGeometry;
		ComPtr<ID2D1GeometrySink> m_pSink;

		bool init()noexcept { return S_OK == pFactory->CreatePathGeometry(m_pPathGeometry.GetAddressOf()) && S_OK == m_pPathGeometry->Open(m_pSink.GetAddressOf()); }

	public:
		PathGeometry()noexcept : m_pPathGeometry{}, m_pSink{} {}

		PathGeometry(ComPtr<ID2D1PathGeometry>&& pPathGeometry, ComPtr<ID2D1GeometrySink>&& pSink) :m_pPathGeometry{ MOV(pPathGeometry) }, m_pSink{ MOV(pSink) } {}
		PathGeometry& operator=(PathGeometry&& r)noexcept
		{
			m_pPathGeometry = MOV(r.m_pPathGeometry);
			m_pSink = MOV(r.m_pSink);
			return *this;
		}

		PathGeometry(const DWRITE_GLYPH_RUN* pGlyphRun) : m_pPathGeometry{}, m_pSink{}
		{
			STATIC_ASSERT(Solid);
			init();

			pGlyphRun->fontFace->GetGlyphRunOutline(
				pGlyphRun->fontEmSize,
				pGlyphRun->glyphIndices,
				pGlyphRun->glyphAdvances,
				pGlyphRun->glyphOffsets,
				pGlyphRun->glyphCount,
				pGlyphRun->isSideways,
				pGlyphRun->bidiLevel % 2,
				m_pSink.Get());

			m_pSink->Close();
		}

		PathGeometry($LinearContainerOf<Math::float2> auto&& vertices, D2D1_FIGURE_END end = D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED) :m_pPathGeometry{}, m_pSink{}
		{
			if (vertices.size() < 2)return;
			init();
			beginFigure(vertices[0]);
			addLines(&vertices[1], static_cast<UINT32>(vertices.size() - 1));
			endFigure(end);
			m_pSink->Close();
		}

		bool unionWith(auto&& geometry)noexcept
		{
			return S_OK == m_pPathGeometry->CombineWithGeometry(
				geometry.pPath(),
				D2D1_COMBINE_MODE_UNION,
				NULL,
				NULL,
				m_pSink.Get()
			);
		}

		void beginFigure(const Math::float2& point)noexcept { m_pSink->BeginFigure(reinterpret_cast<const D2D1_POINT_2F&>(point), Solid ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW); }
		void addLines(const Math::float2* pPoints, u32 count)noexcept { m_pSink->AddLines(reinterpret_cast<const D2D1_POINT_2F*>(pPoints), count); }
		void addLine(const Math::float2& point)noexcept { m_pSink->AddLine(reinterpret_cast<const D2D1_POINT_2F&>(point)); }
		void addBezier(const Math::float2& p0, const Math::float2& p1, const Math::float2& p2)noexcept { m_pSink->AddBezier({ reinterpret_cast<const D2D1_POINT_2F&>(p0) ,reinterpret_cast<const D2D1_POINT_2F&>(p1) ,reinterpret_cast<const D2D1_POINT_2F&>(p2) }); }
		void endFigure(D2D1_FIGURE_END end = D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED)noexcept { m_pSink->EndFigure(end); }

		D2D1_RECT_F getBounds(const D2D1::Matrix3x2F* pWorldTransform = nullptr)noexcept
		{
			D2D1_RECT_F result;
			m_pPathGeometry->GetBounds(pWorldTransform, &result);
			return result;
		}

		ID2D1PathGeometry* pPath()const noexcept { return m_pPathGeometry.Get(); }
	};
	using PathGeometrySolid = PathGeometry<1>;
	using PathGeometryHollow = PathGeometry<0>;

	template<bool Solid>
	struct TransformedGeometry
	{
	private:
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

		using pathGeometry_t = PathGeometry<Solid>;

		ComPtr<ID2D1TransformedGeometry> m_pTransformedGeometry;

	public:

		TransformedGeometry(const pathGeometry_t& pathGeometry, const D2D1::Matrix3x2F& transform)noexcept
		{
			pFactory->CreateTransformedGeometry(pathGeometry.pPath(), transform, m_pTransformedGeometry.GetAddressOf());
		}

		ID2D1TransformedGeometry* ptr()const noexcept { return m_pTransformedGeometry.Get(); }

		void draw(const HwndRenderTarget& hwndRenderTarget, ID2D1Brush* pBrush, f32 strokeWidth)const noexcept
		{
			hwndRenderTarget->DrawGeometry(m_pTransformedGeometry.Get(), pBrush, strokeWidth);
		}

		void fill(const HwndRenderTarget& hwndRenderTarget, ID2D1Brush* pBrush)const noexcept
		{
			hwndRenderTarget->FillGeometry(m_pTransformedGeometry.Get(), pBrush);
		}
	};

	using TransformedGeometrySolid = TransformedGeometry<1>;
	using TransformedGeometryHollow = TransformedGeometry<0>;
}