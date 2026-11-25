#pragma once
#include "../Util/DataStructures/StringSso.h"
#include "../Util/Macro/NoCopyMove.h"
#include "ExeBaseAddress.h"
#include "WinAPI/User32.h"
#include "WinSock2_NoMinMax.h"
namespace Windows
{
	struct WindowClassEx
	{
		NO_COPY_MOVE(WindowClassEx)
	private:
		String16 m_name;
		WNDCLASSEXW m_data;
	public:
		WindowClassEx(
			StringView16 className,
			WNDPROC msgProc,
			HINSTANCE hInstance = exeBase(),
			u32 style = CS_OWNDC /*| CS_HREDRAW | CS_VREDRAW*/,
			HICON hIcon = nullptr,// User32::LoadIconW(NULL, IDI_APPLICATION),
			HCURSOR hCursor = nullptr,// User32::LoadCursorW(NULL, IDC_ARROW),
			HBRUSH hBrush = nullptr,// GDI32::CreateSolidBrush(RGB(0, 0, 0)),
			HICON hIconSm = nullptr//User32::LoadIconW(NULL, IDI_WINLOGO)
		)noexcept :
			m_name{ nullTerminated(className) },
			m_data{
				.cbSize{sizeof(WNDCLASSEXW)},
				.style{style},
				.lpfnWndProc{msgProc},
				.cbClsExtra{0},
				.cbWndExtra{0},
				.hInstance{hInstance},
				.hIcon{hIcon},
				.hCursor{hCursor},
				.hbrBackground{hBrush},
				.lpszMenuName{nullptr},
				.lpszClassName{m_name.data()},
				.hIconSm{hIconSm}
			} 
		{}

		//template<class Self>INLINE Like<Self, WNDCLASSEXW> native(this Self&& self)noexcept { return self.m_data; }
		const String16& name()const noexcept { return m_name; }

		bool reg()const noexcept { return User32::RegisterClassExW(&m_data); }
		bool unreg()const noexcept { return User32::UnregisterClassW(m_name.data(), exeBase()); }
	};
}