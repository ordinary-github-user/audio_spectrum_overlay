#pragma once
#include "WinSock2_NoMinMax.h"
#include <Dwrite.h>//IDWriteTextFormat
#include "LoadLibrary.h"
#include "SetDefaultDllSearchPath.h"
#include "TString.h"
#include "DllCaller.h"
#include "Run.h"

#pragma comment(lib, "Dwrite")

namespace Dwrite
{
	const Run pe{ []STATIC_LAMBDA_INLINE{Windows::loadLibrary<L"Dwrite.dll">(); } };

	DLLCALL_CALLER(DWriteCreateFactory, __stdcall, HRESULT, (DWRITE_FACTORY_TYPE, factoryType), (REFIID, iid),(IUnknown**,pFactory));
	//constexpr DllCaller DWriteCreateFactory{dllCaller<Stateless, HRESULT(__stdcall*)(DWRITE_FACTORY_TYPE,REFIID,IUnknown**)>(TS("DWriteCreateFactory"))};
};