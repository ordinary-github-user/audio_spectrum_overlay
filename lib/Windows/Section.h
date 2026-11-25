#pragma once
#include <bit>
#include "../Util/Macro/NoCopyMove.h"
#include "WinAPI/NTDLL.h"
namespace Windows
{
	struct Section
	{
		NO_COPY_MOVE(Section)
		HANDLE m_handle;
		INLINE Section(HANDLE h = nullptr)noexcept :m_handle{ h } {}
		INLINE bool create(HANDLE hFile, u64* pMaxSize = nullptr, u32 desiredAccess = SECTION_ALL_ACCESS, PageProtectFlag protect = PageProtectFlag::readonly, u32 attributes = SEC_IMAGE)noexcept
		{
			return NT_SUCCESS(NTDLL::NtCreateSection(&m_handle, desiredAccess, nullptr, pMaxSize, protect, attributes, hFile));
		}
		INLINE bool map(HANDLE hProcess, auto& address, usize& size, PageProtectFlag accessProtection = PageProtectFlag::readonly, MemAllocationFlag allocationType = {})const noexcept
		requires($Same<usize, std::remove_reference_t<decltype(address)>> || std::is_pointer_v<std::remove_reference_t<decltype(address)>>)
		{
			//return NT_SUCCESS(NTDLL::NtMapViewOfSection(m_handle, hProcess, &address, 0, 0, nullptr, &size, inherit, {}, accessProtection));
			return NT_SUCCESS(NTDLL::NtMapViewOfSectionEx(m_handle, hProcess, reinterpret_cast<usize*>(&address), {}, &size, allocationType, accessProtection, nullptr, {}));
		}
		INLINE static bool unmap(HANDLE hProcess, auto address) noexcept
		requires($Same<usize, decltype(address)> || std::is_pointer_v<decltype(address)>)
		{
			return NT_SUCCESS(NTDLL::NtUnmapViewOfSection(hProcess, std::bit_cast<usize>(address)));
		}
		INLINE ~Section()noexcept 
		{
			if (m_handle) 
				NTDLL::NtClose(m_handle); 
		}
	};
}