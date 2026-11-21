#pragma once
#include "Inline.h"
#include "KuserSharedData.h"
#include "Win32u.h"
namespace Windows
{
	//return false means error
	INLINE bool getMessage(MSG& msg, HWND hwnd)noexcept
	{
		return Win32u::NtUserGetMessage(&msg, hwnd, 0, 0) != -1;
	}
	//return false means no msg yet
	INLINE bool peekMessage(MSG& msg, HWND hwnd)noexcept
	{
#if 1
		return Win32u::NtUserPeekMessage(&msg, hwnd, 0, 0, PM_REMOVE, 0) != 0;
#else
		return	kuserSharedData().NtBuildNumber >= 10240 ?
				Win32u::NtUserPeekMessage(&msg, hwnd, 0, 0, PM_REMOVE, 0) != 0:
				Win32u::NtUserPeekMessage.overload(&msg, hwnd, 0, 0, PM_REMOVE) != 0;
#endif
	}
	//INLINE void dispatchMessage(MSG& msg)noexcept { Win32u::NtUserDispatchMessage(&msg); }
}