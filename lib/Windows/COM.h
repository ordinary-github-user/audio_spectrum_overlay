#pragma once
#include "WinAPI/Ole32.h"
namespace Windows::COM
{
	INLINE bool init(u32 flags = COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY) { return SUCCEEDED(Ole32::CoInitializeEx(nullptr, flags)); }
	INLINE bool createInstance(REFCLSID rclsid, IUnknown* pUnkOuter, u32 dwClsContext, REFIID riid, void** ppv)noexcept { return SUCCEEDED(Ole32::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv)); }
	INLINE void uninit()noexcept { Ole32::CoUninitialize(); }
}