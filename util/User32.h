#pragma once
#include "DllCaller.h"
#include "LoadLibrary.h"
#include "Run.h"
#include "SetDefaultDllSearchPath.h"
#include "TString.h"
#include "WinSock2_NoMinMax.h"
namespace User32
{
	const Run pe{ []STATIC_LAMBDA_INLINE{Windows::loadLibrary<L"User32.dll">(); } };

	//DllCaller SetWindowsHookExW{ dllCaller<Stateful, HHOOK(__stdcall*)(int idHook,HOOKPROC lpfn,HINSTANCE hmod,DWORD dwThreadId)>(TS("SetWindowsHookExW")) };
	//DllCaller UnhookWindowsHookEx{ dllCaller<Stateful, BOOL(__stdcall*)(HHOOK hhk)>(TS("UnhookWindowsHookEx")) };
	//DllCaller CallNextHookEx{ dllCaller<Stateful, LRESULT(__stdcall*)(HHOOK hhk,int nCode,WPARAM wParam,LPARAM lParam)>(TS("CallNextHookEx")) };

	//DllCaller SendMessageW{ dllCaller<Stateful, LRESULT(__stdcall*)(HWND hWnd,u32 Msg,WPARAM wParam,LPARAM lParam)>(TS("SendMessageW")) };
	//DllCaller AdjustWindowRectEx{ dllCaller<Stateful, BOOL(__stdcall*)(RECT* lpRect,u32 dwStyle,BOOL bMenu,u32 dwExStyle)>(TS("AdjustWindowRectEx")) };
	//DllCaller CreateWindowExW{ dllCaller<Stateful, HWND(__stdcall*)(u32 dwExStyle,const c16* lpClassName,const c16* lpWindowName,u32 dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,void* lpParam)>(TS("CreateWindowExW")) };

	DLLCALL_CALLER_CACHED(ClientToScreen		,__stdcall, HWND,(HWND,hWnd),(POINT*,pPoint));
	DLLCALL_CALLER_CACHED(GetWindow				,__stdcall, HWND,(HWND,hWnd),(u32,uCmd));
	DLLCALL_CALLER_CACHED(GetSystemMetrics		,__stdcall, int,(int,nIndex));
	DLLCALL_CALLER_CACHED(GetClientRect			,__stdcall, BOOL,(HWND,hWnd),(RECT*,lpRect));
	//DLLCALL_CALLER_CACHED(GetWindowRect			,__stdcall, BOOL,(HWND,hWnd),(RECT*,lpRect));
	DLLCALL_CALLER_CACHED(GetWindowTextLengthW	,__stdcall, int,(HWND,hWnd));
	DLLCALL_CALLER_CACHED(GetWindowTextW		,__stdcall, int,(HWND,hWnd),(c16*,lpString),(int,nMaxCount));
	DLLCALL_CALLER_CACHED(SetWindowTextW		,__stdcall, BOOL,(HWND,hWnd),(const c16*,lpString));
	DLLCALL_CALLER_CACHED(IsWindowVisible		,__stdcall, BOOL,(HWND,hWnd));
	DLLCALL_CALLER_CACHED(IsWindow				,__stdcall, BOOL,(HWND,hWnd));
	DLLCALL_CALLER_CACHED(UnregisterClassW		,__stdcall, BOOL,(const c16*,lpClassName),(HINSTANCE,hInstance));
	DLLCALL_CALLER_CACHED(RegisterClassExW		,__stdcall, ATOM,(const WNDCLASSEXW*,Arg1));
	DLLCALL_CALLER_CACHED(CreateWindowInBand	,__stdcall, HWND,(u32,dwExStyle),(const c16*,lpClassName),(const c16*,lpWindowName),(u32,dwStyle),(int,X),(int,Y),(int,nWidth),(int,nHeight),(HWND,hWndParent),(HMENU,hMenu),(HINSTANCE,hInstance),(void*,lpParam),(Windows::Band,band));
#if _WIN64												
	DLLCALL_CALLER_CACHED(SetWindowLongPtrW		,__stdcall, isize,(HWND,hWnd),(int,nIndex),(isize,dwNewLong));
	DLLCALL_CALLER_CACHED(GetWindowLongPtrW		,__stdcall, isize,(HWND,hWnd),(int,nIndex));
#else													
	DLLCALL_CALLER_CACHED(SetWindowLongPtrW		,__stdcall, LONG,(HWND,hWnd),(int,nIndex),(LONG,dwNewLong));
	DLLCALL_CALLER_CACHED(GetWindowLongPtrW		,__stdcall, LONG,(HWND,hWnd),(int,nIndex));
#endif
};