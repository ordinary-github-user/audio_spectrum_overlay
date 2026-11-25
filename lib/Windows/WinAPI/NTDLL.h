#pragma once
#include "../../Util/Constant.h"
#include "../../Util/DataStructures/TString.h"
#include "../DllCaller.h"
#include "../NtStatus.h"
#include "../Structs/WinStructs.h"
#include "../Syscall/SyscallCaller.h"
#include "../WinSock2_NoMinMax.h"

#if !defined(DISABLE_SYSCALL)
	#define DISABLE_SYSCALL 0
#endif

#if DISABLE_SYSCALL
	#define DEF_NTDLL_SYSCALL(name, ret, ...) DLLCALL_CALLER_CACHED(name,__stdcall,ret __VA_OPT__(,__VA_ARGS__))
#else
	#if BITNESS==64
		#define DEF_NTDLL_SYSCALL(name, ret, ...) SYSCALL_CALLER_CACHED(name,ntdll,ret __VA_OPT__(,__VA_ARGS__))
	#else
		#define DEF_NTDLL_SYSCALL(name, ret, ...) SYSCALL_CALLER_CACHED(name,ret __VA_OPT__(,__VA_ARGS__))
	#endif
#endif
namespace NTDLL
{
	INLINE HANDLE NtCurrentProcess() noexcept { return HANDLE(-1); }
	INLINE HANDLE NtCurrentThread() noexcept { return HANDLE(-2); }
	DLLCALL_CALLER_CACHED(NtdllDefWindowProc_W, __stdcall, LRESULT, (HWND, hWnd), (UINT, Msg), (WPARAM, wParam), (LPARAM, lParam));
	DLLCALL_CALLER_CACHED(LdrSetDefaultDllDirectories, __stdcall, NtStatus, (u32, flags));
	DLLCALL_CALLER_CACHED(LdrLoadDll, __stdcall, NtStatus, (c16*,pSearchPath), (u32*,pDllCharacteristics),(UnicodeString*, pPath),(void**,pBase));
	DEF_NTDLL_SYSCALL(NtQueryInformationProcess, NtStatus, (HANDLE, hProcess), (Windows::ProcessInfoClass, infoClass), (void*, pProcessInformation), (u32, size),(u32*,pReturnLength));
	DEF_NTDLL_SYSCALL(NtUnmapViewOfSection, NtStatus, (HANDLE, hProcess), (usize, address));
	DEF_NTDLL_SYSCALL(NtMapViewOfSectionEx,NtStatus,(HANDLE, hSection), (HANDLE, hProcess), (usize*, pBaseAddress), (u64*, pSectionOffset), (usize*, pViewSize), (Windows::MemAllocationFlag, allocationType), (Windows::PageProtectFlag, pageProtect), (MEM_EXTENDED_PARAMETER*, pExtendedParameters), (u32, paramCount));//1803
	DEF_NTDLL_SYSCALL(NtReadFile, NtStatus, (HANDLE, hFile), (HANDLE, hEvent), (PIO_APC_ROUTINE, pApcRoutine), (void*, pApcContext), (IO_STATUS_BLOCK*, pIoStatusBlock),(void*, pBuffer),(u32, length),(u64*, pByteOffset),(u32*, pKey));
	DEF_NTDLL_SYSCALL(NtWriteFile, NtStatus, (HANDLE, hFile), (HANDLE, hEvent), (PIO_APC_ROUTINE, pApcRoutine), (void*, pApcContext), (IO_STATUS_BLOCK*, pIoStatusBlock),(const void*, pBuffer),(u32, length),(u64*, pByteOffset),(u32*, pKey));
	DEF_NTDLL_SYSCALL(NtQueryInformationFile, NtStatus, (HANDLE, hFile), (IO_STATUS_BLOCK*, pIoStatusBlock), (void*, pFileInformation), (u32, length), (Windows::FileInformationClass, fileInformationClass));
	DEF_NTDLL_SYSCALL(NtQuerySystemInformation, NtStatus, (Windows::SystemInformationClass, infoClass), (void*, pInfo), (u32, infoLength), (u32*, retLength));
	DEF_NTDLL_SYSCALL(NtCreateFile, NtStatus, (HANDLE*, pHandle), (ACCESS_MASK, accessMask), (ObjectAttributes*, pObjectAttributes), (IO_STATUS_BLOCK*, pIoStatusBlock),(LARGE_INTEGER*, pAllocationSize),(u32, fileAttributes),(u32, shareAccess),(u32, createDisposition),(u32, createOptions),(void*, eaBuffer),(u32, eaLength));
	DEF_NTDLL_SYSCALL(NtOpenFile, NtStatus, (HANDLE*, pHandle), (ACCESS_MASK, accessMask), (ObjectAttributes*, pObjectAttributes), (IO_STATUS_BLOCK*, pIoStatusBlock),(u32, shareAccess),(u32, openOptions));
	DEF_NTDLL_SYSCALL(NtClose, NtStatus, (HANDLE, handle));
	DEF_NTDLL_SYSCALL(NtQueryDirectoryFileEx, NtStatus, (HANDLE, hFile), (HANDLE, hEvent), (PIO_APC_ROUTINE, pApcRoutine), (void*, pApcContext), (IO_STATUS_BLOCK*, pIoStatusBlock), (void*, pFileInformation), (u32, length), (Windows::FileInformationClass, fileInformationClass), (Windows::QueryDirectoryFlag, queryFlags), (UnicodeString*, pFileName));//1709
	DEF_NTDLL_SYSCALL(NtAllocateVirtualMemoryEx, NtStatus, (HANDLE, hProcess), (usize*, pAddr), (usize*, pRegionSize), (Windows::MemAllocationFlag, allocationType), (Windows::PageProtectFlag, protect), (MEM_EXTENDED_PARAMETER*, pExtendedParameters), (u32, paramCount));//1803
	DEF_NTDLL_SYSCALL(NtFreeVirtualMemory, NtStatus, (HANDLE, hProcess), (void**, pAddr), (usize*, pRegionSize), (Windows::MemFreeFlag, freeType));
	DEF_NTDLL_SYSCALL(NtTerminateProcess, NtStatus, (HANDLE, hProc), (u32, exitCode));
	DEF_NTDLL_SYSCALL(NtWaitForSingleObject, NtStatus, (HANDLE, handle), (bool, alertable), (i64*, pTimeout));
	DEF_NTDLL_SYSCALL(NtDeviceIoControlFile, NtStatus, (HANDLE, hFile),(HANDLE, hEvent), (PIO_APC_ROUTINE, pApcRoutine), (void*, pApcContext),(IO_STATUS_BLOCK*, pIoStatusBlock),(u32, ioControlCode),(void*, pInputBuffer),(u32, inputLength),(void*, pOutputBuffer),(u32, outputLength));
	DEF_NTDLL_SYSCALL(NtSetInformationFile, NtStatus, (HANDLE, hFile), (IO_STATUS_BLOCK*, pIoStatusBlock),(void*, pInfo),(u32, size),(Windows::FileInformationClass, fileInformationClass));
	DEF_NTDLL_SYSCALL(NtCreateSection, NtStatus, (HANDLE*, pSectionHandle), (ACCESS_MASK, DesiredAccess), (ObjectAttributes*, pObjectAttributes),(u64*, MaximumSize),(Windows::PageProtectFlag, SectionPageProtection),(u32, AllocationAttributes),(HANDLE, FileHandle));
	DEF_NTDLL_SYSCALL(NtDeleteFile, NtStatus, (ObjectAttributes*, pObjectAttributes));
	DEF_NTDLL_SYSCALL(NtNotifyChangeDirectoryFileEx, NtStatus, (HANDLE, FileHandle),(HANDLE, Event),(PIO_APC_ROUTINE, ApcRoutine),(void*, ApcContext),(IO_STATUS_BLOCK*, IoStatusBlock),(void*, Buffer),(u32, Length),(u32, CompletionFilter),(BOOLEAN, WatchTree),(Windows::DirectoryNotifyInformationClass, infoClass));
};
#undef DEF_NTDLL_SYSCALL