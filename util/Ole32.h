#pragma once
#include "WinSock2_NoMinMax.h"
#include "LoadLibrary.h"
#include "SetDefaultDllSearchPath.h"
#include "TString.h"
#include "ImportFunction.h"
#include "Run.h"

namespace Ole32
{
	const Run pe{ []STATIC_LAMBDA_INLINE{Windows::loadLibrary<L"ole32.dll">(); } };

	DLLCALL_CALLER(CoInitializeEx, __stdcall, HRESULT, (void*, pvReserved), (u32, dwCoInit));
	DLLCALL_CALLER(CoCreateInstance, __stdcall, HRESULT, (REFCLSID, rclsid), (IUnknown*, pUnkOuter), (u32, dwClsContext), (REFIID, riid), (void**, ppv));
	DLLCALL_CALLER(CoUninitialize, __stdcall, HRESULT);

	//constexpr DllCaller CoInitializeEx{ dllCaller<Stateless, HRESULT(__stdcall*)(void* pvReserved, u32 dwCoInit)>(TS("CoInitializeEx")) };
	//constexpr DllCaller CoCreateInstance{ dllCaller<Stateless, HRESULT(__stdcall*)(REFCLSID rclsid,IUnknown* pUnkOuter,u32 dwClsContext,REFIID riid,void** ppv)>(TS("CoCreateInstance")) };
	//constexpr DllCaller CoUninitialize{ dllCaller<Stateless, HRESULT(__stdcall*)()>(TS("CoUninitialize")) };
};