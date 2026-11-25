#pragma once
#include <atomic>
#include "../Util/Compiler.h"
#include "../Util/DataStructures/FixedString.h"
#include "../Util/DataStructures/ObfuString.h"
#include "../Util/Inline.h"
#include "WinAPI/NTDLL.h"
namespace Windows
{
	template<FixedString path>
	INLINE void* loadLibrary()noexcept
	{
		const ObfuString oPath{ tstring<path> };
		UnicodeString uPath{ oPath };
		void* pBase{};
		NTDLL::LdrLoadDll(nullptr, nullptr, &uPath, &pBase);
		return pBase;
	}
}

//18363
//65 48 8B 04 25 30 00 00 00                                      mov     rax, gs:30h
//B9 00 20 00 00                                                  mov     ecx, 2000h
//66 85 88 EE 17 00 00                                            test    [rax+17EEh], cx
//0F 85 EC B8 08 00                                               jnz     loc_1800AD02A
//48 8B 4F 08                                                     mov     rcx, [rdi+8]    ; pPath
//4C 8D 44 24 40                                                  lea     r8, [rsp+0E8h+buffer] ; pResult
//48 8B D6                                                        mov     rdx, rsi        ; pSearchPath
//E8 C9 00 00 00                                                  call    LdrpInitializeDllPath
//4C 8D 4C 24 30                                                  lea     r9, [rsp+0E8h+pEntry] ; pResult
//44 8B C3                                                        mov     r8d, ebx        ; null
//48 8D 54 24 40                                                  lea     rdx, [rsp+0E8h+buffer] ; buffer
//48 8B CF                                                        mov     rcx, rdi        ; pPath
//E8 9C 0A 00 00                                                  call    LdrpLoadDll
//80 BC 24 BC 00 00 00 00                                         cmp     [rsp+0E8h+var_2C], 0
//8B D8                                                           mov     ebx, eax
//75 63                                                           jnz     short loc_1800217D3
//85 DB                                                           test    ebx, ebx
//78 11                                                           js      short loc_180021785
//48 8B 4C 24 30                                                  mov     rcx, [rsp+0E8h+pEntry] ; pEntry
//48 8B 51 30                                                     mov     rdx, [rcx+30h]
//49 89 16                                                        mov     [r14], rdx
//E8 73 9F FF FF                                                  call    LdrpDereferenceModule

//26040
//65 48 8B 04 25 30 00 00 00                                      mov     rax, gs:30h
//B9 00 20 00 00                                                  mov     ecx, 2000h
//66 85 88 EE 17 00 00                                            test    [rax+17EEh], cx
//0F 85 DB 00 00 00                                               jnz     loc_18002D681
//48 8B 77 08                                                     mov     rsi, [rdi+8]
//40 F6 C5 01                                                     test    bpl, 1
//0F 84 E6 00 00 00                                               jz      loc_18002D69A
//83 E5 FE                                                        and     ebp, 0FFFFFFFEh
//48 89 74 24 60                                                  mov     [rsp+0E8h+var_88], rsi
//89 6C 24 58                                                     mov     [rsp+0E8h+var_90], ebp
//4C 8D 4C 24 30                                                  lea     r9, [rsp+0E8h+pEntry]
//44 8B C3                                                        mov     r8d, ebx
//48 8D 54 24 40                                                  lea     rdx, [rsp+0E8h+var_A8]
//48 8B CF                                                        mov     rcx, rdi
//E8 2B 61 04 00                                                  call    LdrpLoadDll
//80 BC 24 BC 00 00 00 00                                         cmp     [rsp+0E8h+var_2C], 0
//8B D8                                                           mov     ebx, eax
//0F 85 A6 00 00 00                                               jnz     loc_18002D68B
//85 DB                                                           test    ebx, ebx
//78 11                                                           js      short loc_18002D5FA
//48 8B 4C 24 30                                                  mov     rcx, [rsp+0E8h+pEntry] ; pEntry
//48 8B 51 30                                                     mov     rdx, [rcx+30h]
//49 89 16                                                        mov     [r14], rdx
//E8 76 63 04 00                                                  call    LdrpDereferenceModule

//22621
//65 48 8B 04 25 30 00 00 00                                      mov     rax, gs:30h
//B9 00 20 00 00                                                  mov     ecx, 2000h
//66 85 88 EE 17 00 00                                            test    [rax+17EEh], cx
//0F 85 46 E7 08 00                                               jnz     loc_1800B901A
//48 8B 4F 08                                                     mov     rcx, [rdi+8]    ; pPath
//4C 8D 44 24 40                                                  lea     r8, [rsp+0E8h+buffer] ; pResult
//48 8B D6                                                        mov     rdx, rsi        ; pSearchPath
//E8 AF 08 00 00                                                  call    LdrpInitializeDllPath
//4C 8D 4C 24 30                                                  lea     r9, [rsp+0E8h+pResult] ; pResult
//44 8B C3                                                        mov     r8d, ebx        ; null
//48 8D 54 24 40                                                  lea     rdx, [rsp+0E8h+buffer] ; buffer
//48 8B CF                                                        mov     rcx, rdi        ; pPath
//E8 42 E9 FE FF                                                  call    LdrpLoadDll
//80 BC 24 BC 00 00 00 00                                         cmp     [rsp+0E8h+buffer.IsInitedMaybe], 0
//8B D8                                                           mov     ebx, eax
//75 6A                                                           jnz     short loc_18002A970
//85 DB                                                           test    ebx, ebx
//78 11                                                           js      short loc_18002A91B
//48 8B 4C 24 30                                                  mov     rcx, [rsp+0E8h+pResult] ; pEntry
//48 8B 51 30                                                     mov     rdx, [rcx+30h]
//49 89 16                                                        mov     [r14], rdx
//E8 A1 7D FF FF                                                  call    LdrpDereferenceModule