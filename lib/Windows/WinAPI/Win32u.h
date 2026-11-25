#pragma once
#include "../../Util/Compiler.h"
#include "../Structs/WinStructs.h"
#include "../Syscall/SyscallCaller.h"
#include "User32.h"//TODO

#if !defined(DISABLE_SYSCALL)
	#define DISABLE_SYSCALL 0
#endif

#if DISABLE_SYSCALL
	#include "../DllCaller.h"
	#define DEF_WIN32U_SYSCALL(name, ret, ...) DLLCALL_CALLER_CACHED(name,__stdcall,ret __VA_OPT__(,__VA_ARGS__))
#else
	#if BITNESS==64
		#define DEF_WIN32U_SYSCALL(name,ret,...) SYSCALL_CALLER_CACHED(name,win32u,ret __VA_OPT__(,__VA_ARGS__))
	#else
		#define DEF_WIN32U_SYSCALL(name,ret,...) SYSCALL_CALLER_CACHED(name,ret __VA_OPT__(,__VA_ARGS__))
	#endif
#endif

namespace Win32u
{
	DEF_WIN32U_SYSCALL(NtUserSetWindowPos,BOOL,(HWND,hWnd),(HWND,hWndInsertAfter),(int,x),(int,y),(int,cx),(int,cy),(u32,uFlags));
	DEF_WIN32U_SYSCALL(NtUserEndPaint,BOOL,(HWND,hWnd),(const PAINTSTRUCT*,pPaint));
	DEF_WIN32U_SYSCALL(NtUserBeginPaint,HDC,(HWND,hWnd),(PAINTSTRUCT*,pPaint));
	DEF_WIN32U_SYSCALL(NtUserMoveWindow,BOOL,(HWND,hWnd),(int,x),(int,y),(u32,width),(u32,height),(BOOL,bRepaint));
	DEF_WIN32U_SYSCALL(NtUserGetForegroundWindow,HWND);
	DEF_WIN32U_SYSCALL(NtUserSetLayeredWindowAttributes,BOOL,(HWND,hwnd),(COLORREF,crKey),(BYTE,bAlpha),(DWORD,dwFlags));
	DEF_WIN32U_SYSCALL(NtUserShowWindow,BOOL,(HWND,hWnd),(int,nCmdShow));
	DEF_WIN32U_SYSCALL(NtUserDestroyWindow,BOOL,(HWND,hWnd));
	DEF_WIN32U_SYSCALL(NtUserGetRawInputData,u32,(HRAWINPUT,hRawInput),(u32,uiCommand),(void*,pData),(u32*,pcbSize),(u32,cbSizeHeader));
	DEF_WIN32U_SYSCALL(NtUserRegisterRawInputDevices,BOOL,(const RAWINPUTDEVICE*,pRawInputDevices),(u32,uiNumDevices),(u32,cbSize));
	DEF_WIN32U_SYSCALL(NtUserGetClassName,u32,(HWND,hWnd),(BOOL,Real),(UnicodeString*,pClassName));
	DEF_WIN32U_SYSCALL(NtUserQueryWindow,usize,(HWND,hWnd),(Windows::WindowQueryID,Index));
	DEF_WIN32U_SYSCALL(NtUserGetKeyState,SHORT,(INT,VirtKey));
	DEF_WIN32U_SYSCALL(NtUserBuildHwndList,NtStatus,(HDESK,hDesk),(HWND,hWndNext),(BOOL,bEnumChildren),(BOOL,bRemoveImmersive),(DWORD,threadID),(u32,max),(HWND*,pList),(u32*,pCnt));
	DEF_WIN32U_SYSCALL(NtUserPeekMessage,BOOL,(PMSG,pMsg),(HWND,hWnd),(u32,MsgFilterMin),(u32,MsgFilterMax),(u32,RemoveMsg),(u32,Unknown));
	DEF_WIN32U_SYSCALL(NtUserGetMessage,BOOL,(PMSG,pMsg),(HWND,hWnd),(u32,MsgFilterMin),(u32,MsgFilterMax));
	DEF_WIN32U_SYSCALL(NtUserGetWindowBand,u32,(HWND,hwnd),(Windows::Band*,pBand));
	DEF_WIN32U_SYSCALL(NtUserSetWindowBand,NtStatus,(HWND,hwnd),(HWND,hwndInsertAfter),(Windows::Band,band));
	DEF_WIN32U_SYSCALL(NtUserSetWindowDisplayAffinity,BOOL,(HWND,hwnd),(Windows::WindowDisplayAffinity,flag));
	DEF_WIN32U_SYSCALL(NtUserGetWindowDisplayAffinity,BOOL,(HWND,hwnd),(Windows::WindowDisplayAffinity*,pFlag));
	DEF_WIN32U_SYSCALL(NtUserGetRawInputDeviceInfo,u32,(HANDLE,hDevice),(u32,uiCommand),(void*,pData),(u32*,pcbSize));
}
#undef DEF_WIN32U_SYSCALL