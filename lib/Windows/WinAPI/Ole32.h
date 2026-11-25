#pragma once
#include "../../Util/DataStructures/TString.h"
#include "../../Util/Run.h"
#include "../LoadLibrary.h"
#include "../WinSock2_NoMinMax.h"
#include "SetDefaultDllSearchPath.h"

namespace Ole32
{
	const Run pe{ []STATIC_LAMBDA_INLINE{Windows::loadLibrary<L"ole32.dll">(); } };

	DLLCALL_CALLER(CoInitializeEx, __stdcall, HRESULT, (void*, pvReserved), (u32, dwCoInit));
	DLLCALL_CALLER(CoCreateInstance, __stdcall, HRESULT, (REFCLSID, rclsid), (IUnknown*, pUnkOuter), (u32, dwClsContext), (REFIID, riid), (void**, ppv));
	DLLCALL_CALLER(CoUninitialize, __stdcall, HRESULT);
};