#pragma once
#include "../../Util/DataStructures/TString.h"
#include "../DllCaller.h"
#include "../MappedPeView.h"
#include "../TibTebPeb.h"
#include "../WinSock2_NoMinMax.h"

namespace Combase
{
	DLLCALL_CALLER(CoTaskMemFree, __stdcall, void, (LPVOID, pv));
	DLLCALL_CALLER(PropVariantClear, __stdcall, HRESULT, (PROPVARIANT*, p));
};