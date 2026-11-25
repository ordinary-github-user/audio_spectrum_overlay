#pragma once
#include <DWrite.h>//IDWriteTextFormat
#include "../../Util/DataStructures/TString.h"
#include "../../Util/Run.h"
#include "../DllCaller.h"
#include "../LoadLibrary.h"
#include "../WinSock2_NoMinMax.h"
#include "SetDefaultDllSearchPath.h"

#pragma comment(lib, "Dwrite")

namespace Dwrite
{
	const Run pe{ []STATIC_LAMBDA_INLINE{Windows::loadLibrary<L"Dwrite.dll">(); } };

	DLLCALL_CALLER(DWriteCreateFactory, __stdcall, HRESULT, (DWRITE_FACTORY_TYPE, factoryType), (REFIID, iid),(IUnknown**,pFactory));
	//constexpr DllCaller DWriteCreateFactory{dllCaller<Stateless, HRESULT(__stdcall*)(DWRITE_FACTORY_TYPE,REFIID,IUnknown**)>(TS("DWriteCreateFactory"))};
};