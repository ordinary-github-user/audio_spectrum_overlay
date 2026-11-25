#pragma once
#include "../Util/Inline.h"
#include "ImageBase.h"
namespace Windows
{
	//template<class T = HINSTANCE>INLINE T exeBase()noexcept { return reinterpret_cast<T>(thisPeb().imageBase()); };
	template<class T = HINSTANCE>INLINE T exeBase()noexcept { return reinterpret_cast<T>(&__ImageBase); };
}