#pragma once
#include "../Util/Crash.h"
#include "../Util/Macro/NoCopyMove.h"
#include "../Util/NoCopyNoMove.h"
#include "../Util/Tags/Uninitialized.h"
#include "../Windows/File/File.h"
#include "../Windows/WinAPI/NTDLL.h"
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