#pragma once
#include <D2D1.h>
#include "../../Util/DataStructures/TString.h"
#include "../../Util/Run.h"
#include "../DllCaller.h"
#include "../LoadLibrary.h"
#include "../WinSock2_NoMinMax.h"
#include "SetDefaultDllSearchPath.h"
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxguid")
namespace D2d1
{
	const Run pe{ []STATIC_LAMBDA_INLINE{Windows::loadLibrary<L"d2d1.dll">(); } };

	DLLCALL_CALLER(D2D1CreateFactory, __stdcall, HRESULT, (D2D1_FACTORY_TYPE, factoryType), (REFIID, riid), (const D2D1_FACTORY_OPTIONS*, pFactoryOptions), (void**, ppIFactory));
};