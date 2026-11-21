#pragma once
#include "Crash.h"
#include "File.h"
#include "MacroNoCopyMove.h"
#include "NoCopyNoMove.h"
#include "NTDLL.h"
#include "TagUninitialized.h"
namespace Windows
{
	struct Beeper
	{
		NO_COPY_MOVE(Beeper)
	private:
		File m_device;
	public:
		INLINE bool init()noexcept { return m_device.createDevice<LR"(\Device\Beep)">(); }
		INLINE Beeper()noexcept { if (!init())CRASH(); }
		INLINE Beeper(Uninitialized)noexcept {}
		INLINE bool operator()(u32 freq, u32 duration)const noexcept
		{
			struct BeepData
			{
				u32 freq;
				u32 duration;
			};
			BeepData beepData{freq, duration};
			IO_STATUS_BLOCK ioStatus;
			return NT_SUCCESS(NTDLL::NtDeviceIoControlFile(m_device.handle(), 0, 0, 0, &ioStatus, 0x10000, &beepData, sizeof(BeepData), nullptr, 0));
		}
	};
}