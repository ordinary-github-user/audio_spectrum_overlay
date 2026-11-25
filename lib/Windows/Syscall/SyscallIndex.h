#pragma once
#include "../../Math/Numeric/Abs.h"
#include "../../Util/BasicTypes.h"
#include "../../Util/Compact.h"
#include "../../Util/Compiler.h"
#include "../../Util/Crash.h"
#include "../../Util/DataStructures/BytePatternImpl.h"
#include "../../Util/DataStructures/TStringImpl.h"
#include "../MappedPeView.h"
#if BITNESS == 64
	namespace Windows::SyscallIndexGrabber
	{
#if 0
		[[nodiscard]] INLINE bool hasValidSig(const void* pFunc) noexcept
		{
			//4C 8B D1       mov     r10, rcx
			//B8 5B 00 00 00 mov     eax, 5Bh
			return *reinterpret_cast<const u32*>(pFunc) == 0xB8D18B4C;
		}

		[[nodiscard]] INLINE u32 indexAt(const void* pFunc)noexcept
		{
			return *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(pFunc) + 4);
		}
#endif
		template<FixedString dll>
		requires(dll == "ntdll" || dll == "win32u")//use Zw for ntdll, use Nt for win32u, ntdll!NtGetTickCount is not a syscall
		[[nodiscard]] INLINE u32 get(const void* pFunc, MappedPeView pe) noexcept
		{
			static constexpr auto prefix{ (dll == "ntdll") ? FixedString{"Zw"} : FixedString{"Nt"}};
			static constexpr auto base{ compact<(dll == "ntdll") ? 0 : 0x1000> };
			u32 position{};
			pe.exports([&](const void* addr, StringView8 name) LAMBDA_INLINE
			{
				if (addr < pFunc && name.starts_with<prefix>())
					++position;
			});
			return base + position;
		}
	}
#else
	namespace Windows
	{
		struct Wow64SyscallIndex
		{
			struct Result
			{
				u32 index;
				//u32 pWow64SystemServiceCall;
				u16 paramCount;
			};
			const u8* m_funcAddr;
			INLINE Wow64SyscallIndex(const void* addr)noexcept : m_funcAddr{ reinterpret_cast<const u8*>(addr) } {}

			INLINE bool get(Result& result)const noexcept
			{
				//win32u.NtUserSelectPalette - B8 1D100C00           - mov eax,000C101D
				//win32u.NtUserSelectPalette+5- BA 1063B675          - mov edx,win32u.gDispatchTableValues+F0
				//win32u.NtUserSelectPalette+A- FF D2                - call edx
				//win32u.NtUserSelectPalette+C- C2 0C00              - ret 000C
				if (byte_pattern_simd_preload<"B8 ? ? ? ? BA ? ? ? ? FF D2 C2 ? ?">.match(m_funcAddr))
				{
					result = 
					{
						.index{ *reinterpret_cast<const u32*>(m_funcAddr + 1) },
						//.pWow64SystemServiceCall{ *reinterpret_cast<const u32*>(m_funcAddr + 6) },
						.paramCount{ static_cast<u16>(*reinterpret_cast<const u16*>(m_funcAddr + 13) / 4) }
					};
					return true;
				}
				//win32u.NtUserGetForegroundWindow - B8 3C100100 - mov eax, 0001103C
				//win32u.NtUserGetForegroundWindow + 5 - BA 1063B675 - mov edx, win32u.gDispatchTableValues + F0
				//win32u.NtUserGetForegroundWindow + A - FF D2 - call edx
				//win32u.NtUserGetForegroundWindow + C - C3 - ret
				else if (byte_pattern_simd_preload<"B8 ? ? ? ? BA ? ? ? ? FF D2 C3">.match(m_funcAddr))
				{
					result = 
					{ 
						.index{ *reinterpret_cast<const u32*>(m_funcAddr + 1) },
						//.pWow64SystemServiceCall{ *reinterpret_cast<const u32*>(m_funcAddr + 6) },
						.paramCount{}
					};
					return true;
				}
				//Win8.1
				//_ZwQueryInformationProcess@20 proc near
				//B8 18 00 00 00                   mov     eax, 18h        ; NtQueryInformationProcess
				//64 FF 15 C0 00 00 00             call    large dword ptr fs:0C0h
				//C2 14 00                         retn    14h
				//else if (BYTE_PATTERN("B8 ? ? ? ? 64 FF 15 C0 00 00 00 C2 ? ?").match(m_funcAddr))
				//	return Result{ *reinterpret_cast<const u32*>(m_funcAddr + 1), reinterpret_cast<u32>(Windows::thisTeb().pHeavensGate()), static_cast<u16>(*reinterpret_cast<const u16*>(m_funcAddr + 13) / 4) };
				//.text:4B2F1790                     ; __stdcall ZwQueryInformationProcess(x, x, x, x, x)
				//.text:4B2F1790                                     public _ZwQueryInformationProcess@20
				//.text:4B2F1790                     _ZwQueryInformationProcess@20 proc near ; CODE XREF: RtlDispatchException(x,x)+9F↑p
				//.text:4B2F1790                                                             ; RtlDecodePointer(x)+22AB3↓p ...
				//.text:4B2F1790 B8 19 00 00 00                      mov     eax, 19h        ; NtQueryInformationProcess
				//.text:4B2F1795 E8 00 00 00 00                      call    $+5
				//.text:4B2F179A 5A                                  pop     edx
				//.text:4B2F179B 80 7A 14 4B                         cmp     byte ptr [edx+14h], 4Bh ; 'K'
				//.text:4B2F179F 75 0E                               jnz     short loc_4B2F17AF
				//.text:4B2F17A1 64 FF 15 C0 00 00 00                call    large dword ptr fs:0C0h
				//.text:4B2F17A8 C2 14 00                            retn    14h
				//.text:4B2F17A8                     ; ---------------------------------------------------------------------------
				//.text:4B2F17AB 00 00 28 4B                         dd 4B280000h
				//.text:4B2F17AF                     ; ---------------------------------------------------------------------------
				//.text:4B2F17AF
				//.text:4B2F17AF                     loc_4B2F17AF:                           ; CODE XREF: ZwQueryInformationProcess(x,x,x,x,x)+F↑j
				//.text:4B2F17AF BA 70 71 30 4B                      mov     edx, offset _Wow64SystemServiceCall@0 ; Wow64SystemServiceCall()
				//.text:4B2F17B4 FF D2                               call    edx ; Wow64SystemServiceCall() ; Wow64SystemServiceCall()
				//.text:4B2F17B6 C2 14 00                            retn    14h
				//.text:4B2F17B6                     _ZwQueryInformationProcess@20 endp
				else if (byte_pattern_simd_preload<"B8 ? ? ? ? E8 00 00 00 00 5A 80 7A 14 4B 75 0E 64 FF 15 C0 00 00 00 C2 14 00 ? ? ? ? BA ? ? ? ? FF D2 C2 ? ?">.match(m_funcAddr))
				{
					result = 
					{
						.index{ *reinterpret_cast<const u32*>(m_funcAddr + 1) },
						//.pWow64SystemServiceCall{ *reinterpret_cast<const u32*>(m_funcAddr + 32) },
						.paramCount{ static_cast<u16>(*reinterpret_cast<const u16*>(m_funcAddr + 39) / 4) }
					};
					return true;
				}	
				return false;
			}

			INLINE Result operator()()const noexcept
			{
				Result result;
				if (!get(result))
					CRASH();
				return result;
			}
		};
	}
#endif