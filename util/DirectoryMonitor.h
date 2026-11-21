#pragma once
#include "ConceptSig.h"
#include "EnumDefinition.h"
#include "File.h"
#include "IterationControl.h"
#include "MacroMovForward.h"
#include "MacroNoCopyMove.h"
#include "Unreachable.h"
#include "UnsignedCast.h"
namespace Windows
{
	template<$AnyOf<FileNotifyInformation, FileNotifyExtendedInformation, FileNotifyFullInformation> Record = FileNotifyInformation, bool subtree = false, u32 buffer_size = 0x4000>
	struct DirectoryMonitor
	{
		NO_COPY_MOVE(DirectoryMonitor)
		DEFINE_ENUM_(Status, u8, pending, no_access, error);
		INLINE DirectoryMonitor()noexcept = default;
	private:
		File m_directory;
		IO_STATUS_BLOCK m_ioStatus;
		alignas(4) u8 m_pBuffer[buffer_size];
		Status m_status{ Status::error };
		INLINE Status rearm()noexcept
		{
			const NtStatus status{ NTDLL::NtNotifyChangeDirectoryFileEx(
				m_directory.handle(),
				nullptr,
				nullptr,
				nullptr,
				&m_ioStatus,
				m_pBuffer,
				buffer_size,
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
				subtree,
				[]static CONSTEVAL
				{
					/* */if constexpr ($Same<Record, FileNotifyInformation/*    */>)return DirectoryNotifyInformationClass::DirectoryNotifyInformation;
					else if constexpr ($Same<Record, FileNotifyExtendedInformation>)return DirectoryNotifyInformationClass::DirectoryNotifyExtendedInformation;
					else if constexpr ($Same<Record, FileNotifyFullInformation/**/>)return DirectoryNotifyInformationClass::DirectoryNotifyFullInformation;
				}()) };
			return NT_SUCCESS(status) ? Status::pending : Status::error;
		}
	public:
		INLINE Status update(NtPathView path, $SigAnyOf<void(const Record), IterationControl(const Record)> auto&&...action)noexcept
		requires(sizeof...(action) <= 1)
		{
			switch (m_status)
			{
				case Status::no_access: [[fallthrough]];
				case Status::error: [[unlikely]]
				{
					if (!m_directory.openImpl(path, FILE_LIST_DIRECTORY | SYNCHRONIZE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_DIRECTORY_FILE))
						return (m_status = Status::no_access);
					return (m_status = rearm());
				}
				case Status::pending: [[likely]]
				{
					i64 zeroTimeout{};
					const NtStatus waitStatus{ NTDLL::NtWaitForSingleObject(m_directory.handle(), false, &zeroTimeout) };
					if (waitStatus == NtStatus::timeout) [[likely]]
						return (m_status = Status::pending);
					if (waitStatus != NtStatus::success)
						return (m_status = Status::error);
					if (__builtin_bit_cast(NtStatus, m_ioStatus.Status) != NtStatus::success)
						return (m_status = Status::error);
					if constexpr (sizeof...(action) == 1)
						if (m_ioStatus.Information > __builtin_offsetof(Record, FileName))
						{
							const u8* const pEnd{ &m_pBuffer[0] + Algorithm::min<usize>(buffer_size, UNSIGNED(m_ioStatus.Information)) - __builtin_offsetof(Record, FileName) };
							for (const u8* p{ &m_pBuffer[0] }; p < pEnd;)
							{
								const Record& record{ *(const Record*)p };
								if constexpr ((...&&$Sig<decltype(action), void(const Record)>))
									(..., action(record));
								else if ((...&&(action(record) == IterationControl::stop)))
									break;
								if (!record.NextEntryOffset) [[unlikely]] break;
								p += record.NextEntryOffset;
							}
						}
					return (m_status = rearm());
				}
				default:UNREACHABLE;
			}
		}
	};
}
