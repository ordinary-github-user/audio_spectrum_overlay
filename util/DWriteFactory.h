#pragma once
#include <dwrite_3.h>
#include "DWrite.h"
namespace Direct2D
{
	IDWriteFactory* pDwriteFactory{ [] static
	{
		IDWriteFactory* result{};
		if (FAILED(Dwrite::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&result))))
			CRASH();
		return result;
	}() };
}