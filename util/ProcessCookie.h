#pragma once
#include "NTDLL.h"
#include "BasicTypes.h"
namespace Windows
{
	INLINE u32 cookie()noexcept
	{
		u32 result;
		NTDLL::NtQueryInformationProcess(NTDLL::NtCurrentProcess(), Windows::ProcessInfoClass::ProcessCookie, &result, sizeof(result), nullptr);
		return result;
	}
}