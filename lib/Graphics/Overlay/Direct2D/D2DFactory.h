#pragma once
#include "../../../Windows/WinAPI/D2D1.h"
namespace Direct2D
{
	ID2D1Factory1* pFactory{ [] static
	{
		ID2D1Factory1* result{};
		if (S_OK != D2d1::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), nullptr,reinterpret_cast<void**>(&result)))
			CRASH();
		return result;
	}() };
}