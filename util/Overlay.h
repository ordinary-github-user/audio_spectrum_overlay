#pragma once
#include "D2DGraphics.h"
#include "Dwmapi.h"
#include "ExeBaseAddress.h"
#include "IterationControl.h"
#include "Like.h"
#include "MacroMovForward.h"
#include "MacroNoCopyMove.h"
#include "Message.h"
#include "NoUniqueAddress.h"
#include "RawInput.h"
#include "RNG.h"
#include "ScreenDim.h"
#include "StringView.h"
#include "TString.h"
#include "UpdateRate.h"
#include "Window.h"
#include "WindowClass.h"
#include "WinSock2_NoMinMax.h"

template<class Input = RawInput<>, class Graphics = Direct2D::Graphics>
struct Overlay
{
	NO_COPY_MOVE(Overlay)
private:
	MSG m_msg{};
	Math::Vector2<i32> m_pos;
	Math::Vector2<u32> m_size;
	Windows::Window m_window;
	Windows::WindowStyle m_windowStyle;
	Windows::WindowStyleEx m_windowStyleEx;
	String16 m_windowTitle;
	Windows::WindowClassEx m_class;
	NO_UNIQUE_ADDRESS Input m_input;
	NO_UNIQUE_ADDRESS Graphics m_graphics;

	//Windows::Window m_targetWindow{};

	bool m_resize{};
	bool m_show{ true };

	INLINE bool initWindow()noexcept
	{
		if (!m_class.reg())
			return false;
#if 1
		m_window = Windows::Window::create(m_windowStyleEx, m_windowStyle, m_class.name(), m_windowTitle, m_pos, m_size);
#else
		RECT rect{ 0,0, static_cast<long>(m_size[0]),static_cast<long>(m_size[1]) };
		User32::AdjustWindowRectEx(&rect, UNDERLYING(m_windowStyle), false, UNDERLYING(m_windowStyleEx));
		m_window = Windows::Window::create(m_windowStyleEx, m_windowStyle, m_class.name(), m_windowTitle, m_pos, { rect.right - rect.left,rect.bottom - rect.top });
#endif
		if (!m_window)
			return false;
		Win32u::NtUserSetLayeredWindowAttributes(m_window.handle(), RGB(0, 0, 0), 255, LWA_ALPHA);
		static constexpr MARGINS margin{ -1,0,0,0 };
		Dwmapi::DwmExtendFrameIntoClientArea(m_window.handle(), &margin);
		//show();
		return true;
	}
public:

	INLINE Overlay(
		const Math::Vector2<i32>& size,
		$StringOf<c16> auto&& windowTitle,
		Windows::WindowStyle windowStyle,
		Windows::WindowStyleEx windowStyleEx,
		$StringOf<c16> auto&& className)noexcept :
		m_pos{ (Windows::getFirstScreenSize() - size) / 2 },
		m_size{ cast<u32>(size) },
		m_window{},
		m_windowStyle{ windowStyle },
		m_windowStyleEx{ windowStyleEx },
		m_windowTitle{ MOV(windowTitle) },
		m_class{ MOV(className), NTDLL::NtdllDefWindowProc_W.address() },//[](HWND, u32, WPARAM, LPARAM)->LRESULT {return 1; }
		m_input{ [this] LAMBDA_INLINE {if (!initWindow()) CRASH(); return m_window.handle(); }() },//TODO
		m_graphics{ m_window.handle(),m_size }
	{}

	INLINE Overlay(
		const Math::Vector2<i32>& size = Windows::getFirstScreenSize(),
		Windows::WindowStyleEx initStyleEx = Windows::WindowStyleEx::layered | Windows::WindowStyleEx::transparent | Windows::WindowStyleEx::noactivate | Windows::WindowStyleEx::topmost)noexcept :
		Overlay{ size, FixedString{L""},Windows::WindowStyle::popup, initStyleEx, randString16<8, 16>() }
	{}

	INLINE ~Overlay()noexcept { Input::detach(); m_window.destroy(); m_class.unreg(); }

	INLINE void peek()noexcept 
	{
		while (Windows::peekMessage(m_msg, m_window.handle()))
			switch (m_msg.message)
			{
				case WM_INPUT: Input   ::template processMsg<WM_INPUT>(m_msg.lParam); NTDLL::NtdllDefWindowProc_W(m_msg.hwnd, WM_INPUT, m_msg.wParam, m_msg.lParam); break;
				case WM_PAINT: Graphics::template processMsg<WM_PAINT>(m_msg.hwnd); break;
				default:break;
			}
	}

	INLINE void loop($SigAnyOf<void(), IterationControl()> auto&& step)noexcept
	{
		for(;;) [[likely]]
		{
			peek();
			beginFrame();
			if constexpr ($Sig<decltype(step), void()>)
				step();
			else
			{
				if (step() == IterationControl::stop)
					break;
			}
			endFrame();
		}
	}

	INLINE void resize(const Math::Vector2<u32>& size, const Math::Vector2<i32>& position)noexcept
	{
		m_resize = true;
		m_size = size;
		m_pos = position;
	}
	INLINE const Math::Vector2<u32>& size()const noexcept { return m_size; }
	INLINE void prepareFrame()noexcept 
	{
		//maintainStyle();
		maintainShow();
		resizeImpl();
		//placeAtTop();
	}
	INLINE void beginFrame()noexcept
	{
		prepareFrame();
		m_graphics.beginFrame();
		m_input.beginFrame();
	}
	INLINE void endFrame() noexcept
	{
		m_graphics.endFrame();
		m_input.endFrame();
	}
	INLINE const Math::Vector2<i32>& cursorPosition()const noexcept { return reinterpret_cast<const Math::Vector2<i32>&>(m_msg.pt); }
	
	INLINE void setVisible(bool visible)noexcept { m_show = visible; }

	//void setTargetWindow(const Windows::Window& targetWindow) noexcept { m_targetWindow = targetWindow; }
	//void removeTargetWindow() noexcept { setTargetWindow({}); }
	//const Windows::Window& targetWindow()const noexcept { return m_targetWindow; }
	//bool placeAtTop()const noexcept
	//{
	//	if constexpr (TrueTopMost)
	//		return true;
	//	else
	//	{
	//		if (m_targetWindow)
	//			return m_window.insertAbove(m_targetWindow);
	//		else
	//			//return m_window.insertAbove(Windows::Window::foregroundWindow());
	//			return forcePlaceAtTop();
	//	}
	//}
	//bool forcePlaceAtTop()const noexcept
	//{
	//	return 0!=Win32u::NtUserSetWindowPos(m_window.handle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//}
	INLINE bool setStyle(Windows::WindowStyle style) noexcept { return style == m_windowStyle || ((m_windowStyle = style), resetStyle()); }
	INLINE bool setStyleEx(Windows::WindowStyleEx styleEx) noexcept { return styleEx == m_windowStyleEx || ((m_windowStyleEx = styleEx), resetStyleEx()); }
	INLINE bool resetStyle()const noexcept { return m_window.setStyle(m_windowStyle); }
	INLINE bool resetStyleEx()const noexcept { return m_window.setStyleEx(m_windowStyleEx); }
	INLINE const Math::Vector2<i32>& position()const noexcept { return m_pos; }

	INLINE Windows::Window window()const noexcept { return m_window; }

	template<class Self>INLINE Like<Self, MSG> msg(this Self&& self)noexcept { return self.m_msg; }
	template<class Self>INLINE Like<Self, Input> input(this Self&& self)noexcept { return self.m_input; }
	template<class Self>INLINE Like<Self, Graphics> graphics(this Self&& self)noexcept { return self.m_graphics; }
private:

	INLINE void maintainStyle()const noexcept
	{
		const Windows::WindowStyle prevStyle{ m_window.style() };
		const Windows::WindowStyleEx prevStyleEx{ m_window.styleEx() };
		const bool styleMatch{ (prevStyle & ~(Windows::WindowStyle::clipsiblings | Windows::WindowStyle::visible)) == ((m_windowStyle & ~(Windows::WindowStyle::clipsiblings | Windows::WindowStyle::visible))) };
		const bool styleExMatch{ prevStyleEx == m_windowStyleEx };
		if (!styleExMatch || !styleMatch)
		{
			m_window.show();
			if (!styleExMatch)
				resetStyleEx();
			if (!styleMatch)
				resetStyle();
		}
	}

	INLINE void maintainShow()const noexcept
	{
		if (m_show)[[likely]]
		{
			if (!m_window.isVisible())[[unlikely]]
				m_window.show();
		}
		else [[unlikely]]
			if (m_window.isVisible())
				m_window.hide();
	}

	INLINE void resizeImpl()noexcept
	{
		if (m_resize)
		{
			m_graphics.resize(m_size);
			m_window.move(m_pos, m_size);
			m_resize = false;
		}
	}
};