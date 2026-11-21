#pragma once
#include "WinSock2_NoMinMax.h"
#include "MappedPeView.h"
#include "TibTebPeb.h"
#include "TString.h"
#include "DllCaller.h"

namespace Combase
{
	DLLCALL_CALLER(CoTaskMemFree, __stdcall, void, (LPVOID, pv));
	DLLCALL_CALLER(PropVariantClear, __stdcall, HRESULT, (PROPVARIANT*, p));
};