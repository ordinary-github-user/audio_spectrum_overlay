#pragma once
#include "WinSock2_NoMinMax.h"
#include <dwmapi.h>
#include "LoadLibrary.h"
#include "SetDefaultDllSearchPath.h"
#include "TString.h"
#include "Run.h"

#pragma comment(lib, "Dwmapi.lib")

namespace Dwmapi
{
	const Run pe{ []STATIC_LAMBDA_INLINE{Windows::loadLibrary<L"dwmapi.dll">(); } };

	DLLCALL_CALLER(DwmExtendFrameIntoClientArea, __stdcall, HRESULT, (HWND, hWnd), (const MARGINS*, pMarInset));
	//constexpr DllCaller DwmExtendFrameIntoClientArea{ dllCaller<Stateless, HRESULT(__stdcall*)(HWND,const MARGINS *)>(TS("DwmExtendFrameIntoClientArea")) };
};