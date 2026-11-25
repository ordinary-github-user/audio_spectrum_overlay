#pragma once
#include "../Math/Tensor/Tensor.h"
#include "../Util/Concepts/Sig.h"
#include "../Util/DataStructures/DynamicArray.h"
#include "../Util/DataStructures/GrowableArray.h"
#include "../Util/DataStructures/Optional.h"
#include "../Util/DataStructures/StaticString.h"
#include "../Util/DataStructures/StringSso.h"
#include "../Util/DataStructures/StringView.h"
#include "../Util/Enum.h"
#include "../Util/Truncate.h"
#include "ExeBaseAddress.h"
#include "NtStatus.h"
#include "WinAPI/User32.h"
#include "WinAPI/Win32u.h"
#include "WinSock2_NoMinMax.h"

//constexpr bool operator==(const RECT& l, const RECT& r)noexcept
//{
//	return l.left == r.left
//		&& l.top == r.top
//		&& l.right == r.right
//		&& l.bottom == r.bottom;
//}
//constexpr bool operator!=(const RECT& l, const RECT& r)noexcept { return !(l == r); }

template<class Stream>
constexpr Stream& operator<<(Stream& os, const RECT& rect)noexcept
{
	os << '<' << rect.left << ',' << rect.top << ',' << rect.right << ',' << rect.bottom << '>';
	return os;
}

namespace Windows
{
	FLAG(WindowStyle,u32)
	{
		overlapped    =  0x00000000L,
		popup         =  0x80000000L,
		child         =  0x40000000L,
		minimize      =  0x20000000L,
		visible       =  0x10000000L,
		disabled      =  0x08000000L,
		clipsiblings  =  0x04000000L,
		clipchildren  =  0x02000000L,
		maximize      =  0x01000000L,
		caption       =  0x00C00000L,     /* WS_BORDER | WS_DLGFRAME  */
		border        =  0x00800000L,
		dlgframe      =  0x00400000L,
		vscroll       =  0x00200000L,
		hscroll       =  0x00100000L,
		sysmenu       =  0x00080000L,
		thickframe    =  0x00040000L,
		group         =  0x00020000L,
		tabstop       =  0x00010000L
	};
	FLAG(WindowStyleEx,u32)
	{
		dlgmodalframe       = 0x00000001L,
		noparentnotify      = 0x00000004L,
		topmost             = 0x00000008L,
		acceptfiles         = 0x00000010L,
		transparent         = 0x00000020L,
		mdichild            = 0x00000040L,
		toolwindow          = 0x00000080L,
		windowedge          = 0x00000100L,
		clientedge          = 0x00000200L,
		contexthelp         = 0x00000400L,
		right               = 0x00001000L,
		left                = 0x00000000L,
		rtlreading          = 0x00002000L,
		ltrreading          = 0x00000000L,
		leftscrollbar       = 0x00004000L,
		rightscrollbar      = 0x00000000L,
		controlparent       = 0x00010000L,
		staticedge          = 0x00020000L,
		appwindow           = 0x00040000L,
		layered             = 0x00080000 ,
		noinheritlayout     = 0x00100000L, // Disable inheritance of mirroring by children
		noredirectionbitmap = 0x00200000L,
		layoutrtl           = 0x00400000L, // Right to left mirroring
		composited          = 0x02000000L,
		noactivate          = 0x08000000L
	};

	struct Window
	{
	private:
		HWND m_handle;
	public:
		constexpr Window(HWND hwnd = nullptr) noexcept :m_handle{ hwnd } {}

		bool isValid()const noexcept { return User32::IsWindow(m_handle); }

		template<u16 MaxCount = 256>
		requires(MaxCount <= u16(256))//Max value of lpszClassName is 256 
		StaticString<c16, MaxCount> className()const noexcept
		{
			StaticString<c16, MaxCount> result{};
			//result.size() = static_cast<typename StaticString<c16, MaxCount>::Index>(User32::GetClassNameW(m_handle, result.data(), MaxCount));
			UnicodeString unicodeStr{
				0,									//USHORT Length;
				MaxCount * sizeof(c16),				//USHORT MaximumLength;
				result.data()						//PWSTR  Buffer;
			};
			result.modify_size(static_cast<typename StaticString<c16, MaxCount>::Index>(Win32u::NtUserGetClassName(m_handle, 0, &unicodeStr)));
			return result;
		}

		//template<int MaxCount = 0x100>
		//StaticString<c16, MaxCount> title()const noexcept
		//{
		//	StaticString<c16, MaxCount> result{};
		//	result.size() = static_cast<typename StaticString<c16, MaxCount>::Index>(User32::GetWindowTextW(m_handle, result.data(), MaxCount));
		//	return result;
		//}

		String16 title()const noexcept 
		{
			const u32 maxLength{ static_cast<u32>(User32::GetWindowTextLengthW(m_handle)) };
			String16 result(uninitialized, maxLength + 1);
			const u32 length{ static_cast<u32>(User32::GetWindowTextW(m_handle, result.data(), maxLength + 1))};
			result.truncate(Algorithm::min(length, maxLength));
			return result; 
		}

		WindowStyle style()const noexcept { return static_cast<WindowStyle>(static_cast<u32>(User32::GetWindowLongPtrW(m_handle, GWL_STYLE))); }
		WindowStyleEx styleEx()const noexcept { return static_cast<WindowStyleEx>(static_cast<u32>(User32::GetWindowLongPtrW(m_handle, GWL_EXSTYLE))); }
		Opt<Band> band()const noexcept
		{
			Band result;
			if (1 != Win32u::NtUserGetWindowBand(m_handle, &result))
				return {};
			return result;
		}		
		
		NtStatus setBand(Band band)const noexcept { return Win32u::NtUserSetWindowBand(m_handle, HWND_TOPMOST, band); }

		WNDPROC pMsgProc()const noexcept { return reinterpret_cast<WNDPROC>(User32::GetWindowLongPtrW(m_handle, /*GWL_WNDPROC*/-4)); }

		bool setStyle(WindowStyle v) const noexcept { return User32::SetWindowLongPtrW(m_handle, GWL_STYLE, UNDERLYING(v)) != 0; }
		bool setStyleEx(WindowStyleEx v) const noexcept { return User32::SetWindowLongPtrW(m_handle, GWL_EXSTYLE, UNDERLYING(v)) != 0; }
		isize setWndProc(isize wndproc)const noexcept {return User32::SetWindowLongPtrW(m_handle, /*GWL_WNDPROC*/(-4), wndproc) != 0;}
		isize setWndProc(WNDPROC wndproc)const noexcept {return setWndProc(reinterpret_cast<isize>(wndproc)); }

		HWND parent()const noexcept { return truncate<HWND>(User32::GetWindowLongPtrW(m_handle, GWLP_HWNDPARENT)); }

		constexpr HWND handle()const noexcept { return m_handle; }
		bool destroy()const noexcept { return Win32u::NtUserDestroyWindow(m_handle); }

		u64 processID()const noexcept { return Win32u::NtUserQueryWindow(m_handle, WindowQueryID::UNIQUE_PROCESS_ID); }//DWORD result; User32::GetWindowThreadProcessId(m_handle, &result); return result; 
		u64 threadID()const noexcept {return Win32u::NtUserQueryWindow(m_handle, WindowQueryID::UNIQUE_THREAD_ID);}//return User32::GetWindowThreadProcessId(m_handle, nullptr); 

		// screen coordinates of the upper-left and lower-right corners of the window.
		// the bottom-right coordinates of the returned rectangle are exclusive. In other words, the pixel at (right, bottom) lies immediately outside the rectangle.
		//Opt<RECT> windowRect()const noexcept { RECT result; return User32::GetWindowRect(m_handle, &result) ? result : Opt<RECT>{}; }
		
		// The left and top members are zero. The right and bottom members contain the width and height of the window.
		// the bottom-right coordinates of the returned rectangle are exclusive. In other words, the pixel at (right, bottom) lies immediately outside the rectangle
		Opt<RECT> clientRect()const noexcept { RECT result; return User32::GetClientRect(m_handle, &result) ? result : Opt<RECT>{}; }
		Opt<RECT> clientRect2Screen()const noexcept 
		{
			RECT result; 
			if (!User32::GetClientRect(m_handle, &result))return {};
			if (!User32::ClientToScreen(m_handle, reinterpret_cast<POINT*>(&result.left)))return {};
			if (!User32::ClientToScreen(m_handle, reinterpret_cast<POINT*>(&result.right)))return {};
			return result;
		}

		bool move(const Math::Vector2<i32>& pos, const Math::Vector2<u32>& size, bool setPaintMessage = true)const noexcept { return Win32u::NtUserMoveWindow(m_handle, pos[0], pos[1], size[0], size[1], setPaintMessage); }

		bool isVisible()const noexcept { return User32::IsWindowVisible(m_handle); }

		void show()const noexcept { Win32u::NtUserShowWindow(m_handle, SW_SHOW); }
		void hide()const noexcept { Win32u::NtUserShowWindow(m_handle, SW_HIDE); }

		Window nextWindowAbove()const noexcept { return User32::GetWindow(m_handle, GW_HWNDPREV); }
		Window nextWindowBelow()const noexcept { return User32::GetWindow(m_handle, GW_HWNDNEXT); }
		//Opt<Math::Vector2<i32>> windowMetrics()const noexcept { Opt<RECT> rect{ windowRect() }; return rect ? Math::Vector2<i32>{ rect->right - rect->left, rect->bottom - rect->top } : Opt<Math::Vector2<i32>>{}; }
		Opt<Math::Vector2<i32>> clientMetrics()const noexcept { Opt<RECT> rect{ clientRect() }; return rect ? Math::Vector2<i32>{ rect->right - rect->left, rect->bottom - rect->top } : Opt<Math::Vector2<i32>>{}; }
		Opt<Math::Vector2<i32>> client2ScreenMetrics()const noexcept { Opt<RECT> rect{ clientRect2Screen() }; return rect ? Math::Vector2<i32>{ rect->right - rect->left, rect->bottom - rect->top } : Opt<Math::Vector2<i32>>{}; }
		bool insertAbove(const Window& target)const noexcept
		{
			const Window nextWnd{ target.nextWindowAbove() };
			if (nextWnd == *this)
				return true;
			else
				return Win32u::NtUserSetWindowPos(m_handle, nextWnd.handle(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		bool becomeChildOf(const Window& target)const noexcept
		{
			return User32::SetWindowLongPtrW(m_handle, -8/*GWL_HWNDPARENT*/, reinterpret_cast<usize>(target.handle()));
		}
		bool placeAtTopMost()const noexcept { return 0 != Win32u::NtUserSetWindowPos(m_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); }
		bool setPosition(const Math::Vector2<i32>& pos)const noexcept { return 0 != Win32u::NtUserSetWindowPos(m_handle, 0, pos[0], pos[1], 0, 0, SWP_NOZORDER | SWP_NOSIZE); }
		static Window foregroundWindow() noexcept { return Win32u::NtUserGetForegroundWindow(); }
		//static Window desktop() noexcept { return User32::GetDesktopWindow(); }
		bool operator==(Window rhs)const noexcept { return m_handle == rhs.m_handle; }
		bool operator!=(Window rhs)const noexcept { return m_handle != rhs.m_handle; }
		explicit operator bool()const noexcept { return m_handle != 0; }

		bool setAlpha(f32 alpha)const noexcept
		{
			return Win32u::NtUserSetLayeredWindowAttributes(m_handle, 0, BYTE(alpha * 255.f), LWA_ALPHA);
		}

		bool setDisplayAffinity(WindowDisplayAffinity flag = WindowDisplayAffinity::excludefromcapture)const noexcept { return Win32u::NtUserSetWindowDisplayAffinity(m_handle, flag); }

		Opt<WindowDisplayAffinity> displayAffinity()const noexcept
		{
			WindowDisplayAffinity result;
			if (!Win32u::NtUserGetWindowDisplayAffinity(m_handle, &result))
				return {};
			else
				return result;
		}

		static Window create(WindowStyleEx styleEx, WindowStyle style, const c16* className, const c16* title, Math::Vector2<i32> pos = { 0,0 }, Math::Vector2<u32> size = { 0,0 }, HWND parent = nullptr, HMENU hMenu = nullptr, HINSTANCE hInstance = Windows::exeBase(), void* lpParam = nullptr)noexcept
		{
		#if 1
			return User32::CreateWindowInBand(
				UNDERLYING(styleEx),		// u32 dwExStyle,
				className,					// const c16* lpClassName,
				title,						// const c16* lpWindowName
				UNDERLYING(style),			// u32 dwStyle,
				pos[0],						// int X,
				pos[1],						// int Y,
				size[0],					// int nWidth,
				size[1],					// int nHeight,
				parent,						// HWND hWndParent,
				hMenu,						// HMENU hMenu,
				hInstance,					// HINSTANCE hInstance,
				lpParam,					// void* lpParam
				Band::desktop
				//Band::uiaccess
			);
		#else
			return User32::CreateWindowExW(
				UNDERLYING(styleEx),		// u32 dwExStyle,
				className,					// const c16* lpClassName,
				title,						// const c16* lpWindowName
				UNDERLYING(style),			// u32 dwStyle,
				pos[0],						// int X,
				pos[1],						// int Y,
				size[0],					// int nWidth,
				size[1],					// int nHeight,
				parent,						// HWND hWndParent,
				hMenu,						// HMENU hMenu,
				hInstance,					// HINSTANCE hInstance,
				lpParam						// void* lpParam
			);
		#endif
		}
		static Window create(WindowStyleEx styleEx, WindowStyle style, StringView16 className, StringView16 title, Math::Vector2<i32> pos = { 0,0 }, Math::Vector2<u32> size = { 0,0 }, HWND parent = nullptr, HMENU hMenu = nullptr, HINSTANCE hInstance = Windows::exeBase(), void* lpParam = nullptr)noexcept
		{
			return create(styleEx, style, nullTerminated(className).data(), nullTerminated(title).data(), pos, size, parent, hMenu, hInstance, lpParam);
		}

		template<u32 InitialBufferSize = 512_u32>
		INLINE static Opt<DynamicArray<Windows::Window, u32>> list(HDESK hDesktop = nullptr, HWND hwndNext = nullptr, BOOL bEnumChildren = 0, BOOL bRemoveImmersive = 0, UINT threadID = 0)noexcept
		{
			DynamicArray<Windows::Window, u32> result(uninitialized, InitialBufferSize);
			for (;;)
			{
				u32 count;
				const NtStatus status{ [&]
				{
					if (Windows::thisPeb().osBuildNumber >= 9200)//Windows 8 or above
						return Win32u::NtUserBuildHwndList(hDesktop, hwndNext, bEnumChildren, bRemoveImmersive, threadID, result.size(), reinterpret_cast<HWND*>(result.data()), &count);
					else
						return Win32u::NtUserBuildHwndList.overload(hDesktop, hwndNext, bEnumChildren, threadID, result.size(), reinterpret_cast<HWND*>(result.data()), &count);
				}() };
				switch (status)
				{
					case NtStatus::success:
						result.modify_size(count);
						return result;
					case NtStatus::buffer_too_small:
						result.resize_uninitialized(result.size() * 2);
						break;
					default:
						return {};
				}
			}
		}

		template<u32 InitialBufferSize = 512_u32>INLINE static Opt<DynamicArray<Windows::Window, u32>> topLevelWindows()noexcept{return list<InitialBufferSize>();}
		template<u32 InitialBufferSize = 512_u32>INLINE Opt<DynamicArray<Windows::Window, u32>> children()noexcept{return list<InitialBufferSize>(nullptr, m_handle, true);}

		//template<class lambda_t>
		//static GrowableArray<Window> getTopLevelWindowsEx(lambda_t&& match) noexcept
		//{
		//	GrowableArray<Window> result;
		//	enumTopLevelWindows([&](const Window& window)->bool
		//	{
		//		if (match(window))
		//			result.push_back(window);
		//		return true;
		//	});
		//	return result;
		//}

		template<class Stream>
		friend Stream& operator<<(Stream& os, const Window& window)noexcept
		{
			os << TS("Handle = ") << hex(reinterpret_cast<usize>(window.handle()));
			os << TS(", PID = ") << hex(window.processID());
			os << TS(", Title = ") << window.title();
			os << TS(", Class = ") << window.className();
			os << TS(", ClientMetrics = ") << window.client2ScreenMetrics();
			//os << window.windowMetrics();
			const auto style{ window.style() };
			const auto styleEx{ window.styleEx() };
			const auto band{ window.band() };
			const auto affinity{ window.displayAffinity() };
			os << TS(", Style = ") << style << TS(" (") << Hex{UNDERLYING(style)};
			os << TS("), StyleEx = ") << styleEx << TS(" (") << Hex{UNDERLYING(styleEx)};
			if(band)
				os << TS("), Band = ")    << *band << TS(" (") << Hex{UNDERLYING(*band)};
			if(affinity)
				os << TS("), Affinity = ")<< *affinity << TS(" (") << Hex{UNDERLYING(*affinity)};
			os << ')';
			return os;
		}
	};

	//Window getWindowAtPos(const POINT& point)
	//{
	//	HWND hpWnd{ WindowFromPoint(point) }, hcWnd{ 0 };
	//	if (User32::IsWindow(hpWnd))
	//		hcWnd = ChildWindowFromPointEx(hpWnd, point, CWP_ALL);
	//	return ((User32::IsWindow(hcWnd) == 0) ? hpWnd : hcWnd);
	//}
	//Window getWindowAtCursorPos() { POINT point; User32::GetCursorPos(&point); return getWindowAtPos(point); }
	//void monitorOff() { PostMessage(FindWindowA("ConsoleWindowClass", NULL), WM_SYSCOMMAND, SC_MONITORPOWER, 2); }
	//void monitorOn() { PostMessage(FindWindowA("ConsoleWindowClass", NULL), WM_SYSCOMMAND, SC_MONITORPOWER, -1); }

	//template<$Sig<bool(Windows::Window)> lambda_t>
	//void enumTopLevelWindows(lambda_t&& func)
	//{
	//	User32::EnumWindows([](HWND hwnd, LPARAM lparam)->BOOL
	//	{
	//		return (*reinterpret_cast<lambda_t*>(lparam))(hwnd);
	//	},
	//	reinterpret_cast<LPARAM>(&func));
	//}
	//template<$Sig<bool(Windows::Window)> IsMatch>
	//GrowableArray<Window> getTopLevelWindows(IsMatch&& match)
	//{
	//	GrowableArray<Window> result;
	//	enumTopLevelWindows([&](const Window& hwnd)->bool
	//	{
	//		if (match(hwnd))
	//			result.emplace_back(hwnd);
	//		return true;
	//	});
	//	return result;
	//}
	//template<class lambda_t>
	//void enumChildWindows(const Window& parentWindow /*GetDesktopWindow()*/, lambda_t&& func)
	//{
	//	User32::EnumChildWindows(parentWindow.handle(), [](HWND hwnd, LPARAM lparam)->BOOL {return (*reinterpret_cast<lambda_t*>(lparam))(hwnd); }, reinterpret_cast<LPARAM>(&func));
	//}
	//template<class lambda_t>
	//GrowableArray<Window> getChildWindows(const Window& parentWindow /*GetDesktopWindow()*/, lambda_t&& match)
	//{
	//	GrowableArray<Window> result;
	//	enumChildWindows(parentWindow, [&](Window hwnd)->bool
	//	{
	//		if (match(hwnd))
	//			result.emplace_back(hwnd);
	//		return true;
	//	});
	//	return result;
	//}
}