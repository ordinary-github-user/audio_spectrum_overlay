#pragma once
#include "../Util/BasicTypes.h"
#include "WinAPI/NTDLL.h"
namespace Windows
{
	INLINE u32 cookie()noexcept
	{
		u32 result;
		NTDLL::NtQueryInformationProcess(NTDLL::NtCurrentProcess(), Windows::ProcessInfoClass::ProcessCookie, &result, sizeof(result), nullptr);
		return result;
	}
}