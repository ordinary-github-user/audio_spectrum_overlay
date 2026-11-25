#pragma once
#include "../Util/Inline.h"
#include "../Util/Unreachable.h"
#include "WinAPI/NTDLL.h"
namespace Windows
{
	[[noreturn]]INLINE void exit(u32 code = 0) noexcept
	{
		NTDLL::NtTerminateProcess(NTDLL::NtCurrentProcess(), code);
		UNREACHABLE;
	}
}