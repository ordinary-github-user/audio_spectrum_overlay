#pragma once
#include "D2DAlignment.h"
#include "D2DBrush.h"
#include "D2DFactory.h"
#include "D2DGeometry.h"
#include "D2DHwndRenderTarget.h"
#include "D2DLayer.h"
#include "D2DTextFormat.h"
#include "D2DTextLayout.h"
#include "NoCopyNoMove.h"
#include "Tensor.h"
#include "Win32u.h"
#include <d2d1_1.h>
#include <wrl/client.h>
namespace Direct2D
{
	struct Graphics :NoCopyMove
	{
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//private:
		HwndRenderTarget m_hwndRenderTarget;
		Brush::Solid m_solidBrush;
		Rgba<f32> m_backGroundColor{ 0,0,0,0 };
		//Layer m_layer{m_hwndRenderTarget};
	public:
		Graphics(HWND hwnd, const Math::Vector2<u32>& size) noexcept:
			m_hwndRenderTarget{ hwnd,size },
			m_solidBrush{ m_hwndRenderTarget }
		{}
		const HwndRenderTarget& renderTarget()const noexcept { return m_hwndRenderTarget; }
		void drawEllipse(const Math::float2& position, const Math::float2& radiuses, f32 strokeWidth, $Brush auto const& brush)const { m_hwndRenderTarget->DrawEllipse(D2D1_ELLIPSE{ reinterpret_cast<const D2D_POINT_2F&>(position), radiuses[0], radiuses[1] }, brush.ptr(), strokeWidth); }
		void drawEllipse(const Math::float2& position, const Math::float2& radiuses, f32 strokeWidth = 1, const Rgba<f32>& color = { 1,1,1,1 }) const { m_solidBrush.setColor(color); drawEllipse(position, radiuses, strokeWidth, m_solidBrush); }
		
		void drawCircle(const Math::float2& position, f32 radius, f32 strokeWidth, $Brush auto const& brush) const { drawEllipse(position, { radius, radius }, strokeWidth, brush); }
		void drawCircle(const Math::float2& position, f32 radius, f32 strokeWidth = 1, const Rgba<f32>& color = { 1,1,1,1 }) const { drawEllipse(position, { radius, radius }, strokeWidth, color); }
		
		void fillEllipse(const Math::float2& position, const Math::float2& radiuses, $Brush auto const& brush)const { m_hwndRenderTarget->FillEllipse(D2D1_ELLIPSE{ reinterpret_cast<const D2D_POINT_2F&>(position), radiuses[0], radiuses[1] }, brush.ptr()); }
		void fillEllipse(const Math::float2& position, const Math::float2& radiuses, const Rgba<f32>& color = { 1,1,1,1 }) const { m_solidBrush.setColor(color); fillEllipse(position, radiuses, m_solidBrush); }
		
		void fillCircle(const Math::float2& position, f32 radius, $Brush auto const& brush) const { fillEllipse(position, { radius, radius }, brush); }
		void fillCircle(const Math::float2& position, f32 radius, const Rgba<f32>& color = { 1,1,1,1 }) const { fillEllipse(position, { radius, radius }, color); }
		
		void drawSegment(const Math::Segment2<f32>& line, f32 strokeWidth, $Brush auto const& brush)const { m_hwndRenderTarget->DrawLine(reinterpret_cast<const D2D_POINT_2F&>(line.p0), reinterpret_cast<const D2D_POINT_2F&>(line.p1), brush.ptr(), strokeWidth); }
		void drawSegment(const Math::Segment2<f32>& line, f32 strokeWidth = 1, const Rgba<f32>& color = { 1,1,1,1 })const { m_solidBrush.setColor(color); drawSegment(line, strokeWidth, m_solidBrush); }
		//template<bool Solid>
		//void drawTransformedGeometry(const TransformedGeometry<Solid>& transformedGeometry, f32 strokeWidth, $Brush auto const& brush)const { m_hwndRenderTarget->DrawGeometry(transformedGeometry.ptr(), brush.ptr(), strokeWidth, nullptr); }
		//template<bool Solid>
		//void drawTransformedGeometry(const TransformedGeometry<Solid>& transformedGeometry, f32 strokeWidth = 1, const Rgba<f32>& color = { 1,1,1,1 })const { m_solidBrush.setColor(color); drawTransformedGeometry(transformedGeometry, strokeWidth, m_solidBrush); }
		//
		//void fillTransformedGeometry(const TransformedGeometrySolid& transformedGeometrySolid, $Brush auto const& brush)const { m_hwndRenderTarget->FillGeometry(transformedGeometrySolid.ptr(), brush.ptr()); }
		//void fillTransformedGeometry(const TransformedGeometrySolid& transformedGeometrySolid, const Rgba<f32>& color = { 1,1,1,1 })const { m_solidBrush.setColor(color); fillTransformedGeometry(transformedGeometrySolid, m_solidBrush); }
		template<bool Solid>
		void drawSimplePathGeometry(const SimplePathGeometry<Solid>& pathGeometry, f32 strokeWidth, $Brush auto const& brush)const { m_hwndRenderTarget->DrawGeometry(pathGeometry.pPath(), brush.ptr(), strokeWidth); }
		template<bool Solid>
		void drawSimplePathGeometry(const SimplePathGeometry<Solid>& pathGeometry, f32 strokeWidth = 1, const Rgba<f32>& color = { 1,1,1,1 })const { m_solidBrush.setColor(color); drawSimplePathGeometry(pathGeometry, strokeWidth, m_solidBrush); }
		
		template<bool Solid>
		void drawPathGeometry(const PathGeometry<Solid>& pathGeometry, f32 strokeWidth, $Brush auto const& brush)const { m_hwndRenderTarget->DrawGeometry(pathGeometry.pPath(), brush.ptr(), strokeWidth); }
		template<bool Solid>
		void drawPathGeometry(const PathGeometry<Solid>& pathGeometry, f32 strokeWidth = 1, const Rgba<f32>& color = { 1,1,1,1 })const { m_solidBrush.setColor(color); drawPathGeometry(pathGeometry, strokeWidth, m_solidBrush); }
		//template<bool Solid>
		//void drawPathGeometry(const PathGeometry<Solid>& pathGeometry, const D2D1::Matrix3x2F& transform, f32 strokeWidth, $Brush auto const& brush)const { drawTransformedGeometry<Solid, Brush>({ pathGeometry, transform }, strokeWidth, brush); }
		//template<bool Solid>
		//void drawPathGeometry(const PathGeometry<Solid>& pathGeometry, const D2D1::Matrix3x2F& transform, f32 strokeWidth = 1, const Rgba<f32>& color = { 1,1,1,1 })const { m_solidBrush.setColor(color); drawPathGeometry(pathGeometry, transform, strokeWidth, m_solidBrush); }
		
		void fillSimplePathGeometry(const SimplePathGeometrySolid& pathGeometry, $Brush auto const& brush)const { m_hwndRenderTarget->FillGeometry(pathGeometry.pPath(), brush.ptr()); }
		void fillSimplePathGeometry(const SimplePathGeometrySolid& pathGeometry, const Rgba<f32>& color = { 1,1,1,1 })const { m_solidBrush.setColor(color); fillSimplePathGeometry(pathGeometry, m_solidBrush); }
		
		void fillPathGeometry(const PathGeometrySolid& pathGeometry, $Brush auto const& brush)const { m_hwndRenderTarget->FillGeometry(pathGeometry.pPath(), brush.ptr()); }
		void fillPathGeometry(const PathGeometrySolid& pathGeometry, const Rgba<f32>& color = { 1,1,1,1 })const { m_solidBrush.setColor(color); fillPathGeometry(pathGeometry, m_solidBrush); }

		void drawPathGeometryUnionCpu($LinearContainer auto&& polygons, f32 strokeWidth, $Brush auto const& brush) const noexcept
		requires($LinearContainerOf<ElementOf<decltype(polygons)>, Math::float2>)
		{
			ASSERT(polygons.size() < u32(-1));
			const u32 size{ (u32)polygons.size() };
			DynamicArray<Direct2D::SimplePathGeometrySolid, u32> pathGeometries{ uninitialized, size };
			DynamicArray<ID2D1Geometry*, u32> geometryPointers{ uninitialized, size };
			for(const auto i: iota_up(size))
			{
				CTOR(pathGeometries[i]) { polygons[i], D2D1_FIGURE_END_CLOSED };
				CTOR(geometryPointers[i]) { pathGeometries[i].pPath() };
			}
			Microsoft::WRL::ComPtr<ID2D1GeometryGroup> pGroup;
			Direct2D::pFactory->CreateGeometryGroup(D2D1_FILL_MODE_WINDING, geometryPointers.data(), geometryPointers.size(), &pGroup);
			Microsoft::WRL::ComPtr<ID2D1PathGeometry> pUnionGeometry;
			Direct2D::pFactory->CreatePathGeometry(&pUnionGeometry);
			Microsoft::WRL::ComPtr<ID2D1GeometrySink> pSink;
			pUnionGeometry->Open(&pSink);
			pGroup->Outline(nullptr, pSink.Get());
			pSink->Close();
			m_hwndRenderTarget->DrawGeometry(pUnionGeometry.Get(), brush.ptr(), strokeWidth);
		}
		void drawPathGeometryUnionCpu($LinearContainer auto&& polygons, f32 strokeWidth, const Rgba<f32>& color = { 1,1,1,1 }) const noexcept { m_solidBrush.setColor(color); drawPathGeometryUnionCpu(FWD(polygons), strokeWidth, m_solidBrush); }

		//void fillPathGeometry(const PathGeometrySolid& pathGeometry, const D2D1::Matrix3x2F& transform, $Brush auto const& brush)const { fillTransformedGeometry({ pathGeometry,transform }, brush); }
		//void fillPathGeometry(const PathGeometrySolid& pathGeometry, const D2D1::Matrix3x2F& transform, const Rgba<f32>& color = { 1,1,1,1 })const { m_solidBrush.setColor(color); fillPathGeometry(pathGeometry, transform, m_solidBrush); }
		
		//void fillUnionedPolygons(const auto& polys, $Brush auto const& brush) const
		//{
		//	if (polys.empty())[[unlikely]] return;
		//	if (polys.size() == 1)
		//	{
		//		fillPathGeometry(polys[0], brush);
		//		return;
		//	}
		//	// 1) Create the geometry for the first polygon
		//	ComPtr<ID2D1PathGeometry> unionGeom;
		//	D2DFactory::factory()->CreatePathGeometry(&unionGeom);
		//	{
		//		ComPtr<ID2D1GeometrySink> sink;
		//		unionGeom->Open(&sink);
		//		sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		//		const auto& P = polys[0];
		//		sink->BeginFigure(reinterpret_cast<const D2D1_POINT_2F&>(P[0]), D2D1_FIGURE_BEGIN_FILLED);
		//		sink->AddLines(reinterpret_cast<const D2D1_POINT_2F*>(P.data()), static_cast<UINT32>(P.size()));
		//		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		//		sink->Close();
		//	}
		//	// 2) Combine each remaining polygon into unionGeom
		//	for (size_t i = 1; i < polys.size(); ++i)
		//	{
		//		// build a path-geometry for polys[i]
		//		ComPtr<ID2D1PathGeometry> polyGeom;
		//		D2DFactory::factory()->CreatePathGeometry(&polyGeom);
		//		{
		//			ComPtr<ID2D1GeometrySink> sink;
		//			polyGeom->Open(&sink);
		//			sink->SetFillMode(D2D1_FILL_MODE_WINDING);
		//			const auto& P = polys[i];
		//			sink->BeginFigure(
		//				reinterpret_cast<const D2D1_POINT_2F&>(P[0]),
		//				D2D1_FIGURE_BEGIN_FILLED
		//			);
		//			sink->AddLines(reinterpret_cast<const D2D1_POINT_2F*>(P.data()), static_cast<UINT32>(P.size()));
		//			sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		//			sink->Close();
		//		}
		//		// combine unionGeom ∪ polyGeom → newUnion
		//		ComPtr<ID2D1PathGeometry> newUnion;
		//		D2DFactory::factory()->CreatePathGeometry(&newUnion);
		//		{
		//			ComPtr<ID2D1SimplifiedGeometrySink> combineSink;
		//			newUnion->Open(&combineSink);
		//			combineSink->SetFillMode(D2D1_FILL_MODE_WINDING);
		//			unionGeom->CombineWithGeometry(
		//				polyGeom.Get(),
		//				D2D1_GEOMETRY_COMBINE_MODE_UNION,
		//				nullptr,                     // identity transform
		//				kFlatteningTolerance,
		//				combineSink.Get()
		//			);
		//			combineSink->Close();
		//		}
		//		unionGeom = MOV(newUnion);
		//	}
		//	// 3) Fill the final union geometry
		//	m_hwndRenderTarget->FillGeometry(unionGeom.Get(), brush.ptr());
		//}
		//// Overload that takes a simple color
		//void fillUnionedPolygons(const PolygonList& polys, const color_t& c = { 1,1,1,1 }) const
		//{
		//	m_solidBrush.setColor(c);
		//	fillUnionedPolygons(polys, m_solidBrush);
		//}
		//// Stroke (draw the border) of the unioned outline
		//template< class BrushT >
		//void drawUnionedPolygons(const PolygonList& polys, f32 strokeWidth, BrushT const& brush) const
		//{
		//	if (polys.empty()) return;
		//	// Reuse fillUnionedPolygons’s combine step but skip the final Fill:
		//	// (you could factor out the combine‐loop above into a private helper)
		//	ComPtr<ID2D1PathGeometry> unionGeom;
		//	D2DFactory::factory()->CreatePathGeometry(&unionGeom);
		//	// … same combine code as above … (see fillUnionedPolygons)
		//	// Finally, stroke the outline of that union geometry:
		//	m_hwndRenderTarget->DrawGeometry(unionGeom.Get(), brush.ptr(), strokeWidth);
		//}
		//// Overload that takes a simple color
		//void drawUnionedPolygons(const auto& polys, f32 strokeWidth = 1.0f, const color_t& c = { 1,1,1,1 }) const
		//{
		//	m_solidBrush.setColor(c);
		//	drawUnionedPolygons(polys, strokeWidth, m_solidBrush);
		//}

		void drawTextLayout(const Math::float2& position, const TextLayout& textLayout, $Brush auto const& brush, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE)const
		{
			m_hwndRenderTarget->DrawTextLayout(reinterpret_cast<const D2D1_POINT_2F&>(position), textLayout.ptr(), brush.ptr(), options);
		}
		template<Alignment alignment = {}>
		void drawTextLayout(const Math::float2& position, const TextLayout& textLayout, const Rgba<f32>& color = { 1,1,1,1 }, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE)const
		{
			m_solidBrush.setColor(color); 
			if constexpr (alignment == Alignment{ Alignment::Horizontal::left, Alignment::Vertical::top })
				drawTextLayout(position, textLayout, m_solidBrush, options);
			else
				drawTextLayout(alignment.getTopLeftPosition(position, textLayout.metrics()), textLayout, m_solidBrush, options);
		}
		
		template<Alignment alignment = {}>
		void fillRect(const Math::float2& position, const Math::float2& size, $Brush auto const& brush)const noexcept 
		{
			if constexpr (alignment == Alignment{ Alignment::Horizontal::left, Alignment::Vertical::top })
				m_hwndRenderTarget->FillRectangle(D2D1::RectF(position[0], position[1], position[0] + size[0], position[1] + size[1]), brush.ptr());
			else
			{
				const Math::float2 p{ alignment.getTopLeftPosition(position, size) };
				m_hwndRenderTarget->FillRectangle(D2D1::RectF(p[0], p[1], p[0] + size[0], p[1] + size[1]), brush.ptr()); 
			}
		}
		template<Alignment alignment = {}>void fillRect(const Math::float2& position, const Math::float2& size, const Rgba<f32>& color = { 1,1,1,1 })const noexcept { m_solidBrush.setColor(color); fillRect<alignment>(position, size, m_solidBrush); }
		
		template<Alignment alignment = {}>
		void drawRect(const Math::float2& position, const Math::float2& size, f32 strokeWidth, $Brush auto const& brush)const
		{
			if constexpr (alignment == Alignment{ Alignment::Horizontal::left, Alignment::Vertical::top })
				m_hwndRenderTarget->DrawRectangle(D2D1::RectF(position[0], position[1], position[0] + size[0], position[1] + size[1]), brush.ptr(), strokeWidth);
			else
			{
				const Math::float2 p{ alignment.getTopLeftPosition(position, size) };
				m_hwndRenderTarget->DrawRectangle(D2D1::RectF(p[0], p[1], p[0] + size[0], p[1] + size[1]), brush.ptr(), strokeWidth); 
			}
		}
		template<Alignment alignment = {}>void drawRect(const Math::float2& position, const Math::float2& size, f32 strokeWidth = 1, const Rgba<f32>& color = { 1,1,1,1 })const noexcept{ m_solidBrush.setColor(color); drawRect<alignment>(position, size, strokeWidth, m_solidBrush); }
		
		//void drawText(const Math::float2& position, const Text& text, Alignment alignment = { Alignment::Horizontal::left, Alignment::Vertical::top })const noexcept
		//{
		//	const Math::float2 pos{ alignment.getTopLeftPosition(position, text.metrics()) };
		//	if (text.shadowOption().enable())
		//		drawTextLayout(pos + text.shadowOption().offset(), text.layout(), text.shadowOption().color());
		//	if (text.fillOption().enable())
		//		drawTextLayout(pos, text.layout(), text.fillOption().color());
		//}
		//template<bool Solid>
		//void pushLayer(const PathGeometry<Solid>& geometry, const D2D1_MATRIX_3X2_F& transform) const noexcept
		//{
		//	m_hwndRenderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geometry.pPath(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, transform), m_layer.ptr());
		//}
		//void popLayer() const noexcept { m_hwndRenderTarget->PopLayer(); }
		void pushAxisAlignedClip(const D2D1_RECT_F& rect, D2D1_ANTIALIAS_MODE antialiasMode = D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE)const noexcept
		{
			m_hwndRenderTarget->PushAxisAlignedClip(rect, antialiasMode);
		}
		void popAxisAlignedClip()const noexcept
		{
			m_hwndRenderTarget->PopAxisAlignedClip();
		}
		//void pushAlphaLayer(f32 alpha)const noexcept
		//{
		//	//ComPtr<ID2D1Layer> layer;
		//	//m_hwndRenderTarget->CreateLayer(layer.GetAddressOf());
		//	D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters(
		//		D2D1::InfiniteRect(),
		//		nullptr,
		//		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		//		D2D1::IdentityMatrix(),
		//		alpha,
		//		nullptr,
		//		D2D1_LAYER_OPTIONS_NONE
		//	);
		//	m_hwndRenderTarget->PushLayer(layerParams, nullptr);
		//}
		//void popLayer()
		//{
		//	m_hwndRenderTarget->PopLayer();
		//}
		//void drawTextRow(const Math::float2& position, const TextRow& textRow)const noexcept
		//{
		//	const Math::float2 topLeftPos{ textRow.getTopLeftPosition(position) };
		//	Math::float2 pos{ topLeftPos };
		//	for (const Text& text : textRow)
		//	{
		//		pos[1] = topLeftPos[1] - (text.metrics()[1] - textRow.metrics()[1]);
		//		drawText(pos, text);
		//		pos[0] += text.metrics()[0];
		//	}
		//}
		void resize(const Math::Vector2<u32>& size)const noexcept { m_hwndRenderTarget->Resize(D2D1::SizeU(size[0], size[1])); }
		void clear(const Rgba<f32>& color)const noexcept { m_hwndRenderTarget->Clear(reinterpret_cast<const D3DCOLORVALUE&>(color)); }
		void beginFrame()const noexcept { m_hwndRenderTarget->BeginDraw(); clear(m_backGroundColor); }
		void endFrame()const noexcept { m_hwndRenderTarget->EndDraw(); }
		template<class Self>constexpr Like<Self, Rgba<f32>> backgroundColor(this Self&& self)noexcept { return FWD_LIKE(self, self.m_backGroundColor); }
		void setTransform(const D2D_MATRIX_3X2_F& matrix)noexcept
		{
			m_hwndRenderTarget->SetTransform(matrix);
		}
		template<u32 msg>
		requires(msg == WM_PAINT)
		static void processMsg(HWND hwnd)noexcept
		{
			PAINTSTRUCT ps;
			Win32u::NtUserBeginPaint(hwnd, &ps);
			Win32u::NtUserEndPaint(hwnd, &ps);
		}
	};
}