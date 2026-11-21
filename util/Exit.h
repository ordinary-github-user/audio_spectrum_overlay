#pragma once
#include "NTDLL.h"
#include "Inline.h"
#include "Unreachable.h"
namespace Windows
{
	[[noreturn]]INLINE void exit(u32 code = 0) noexcept
	{
		NTDLL::NtTerminateProcess(NTDLL::NtCurrentProcess(), code);
		UNREACHABLE;
	}
}