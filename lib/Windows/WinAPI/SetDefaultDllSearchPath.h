#pragma once
#include "../../Util/Compiler.h"
#include "../../Util/Run.h"
#include "NTDLL.h"
namespace Windows::SetDefaultDllDirectoriesSystem32
{
	const Run runSetDefaultDllDirectoriesSystem32{ [] STATIC_LAMBDA_INLINE
	{
		if (thisPeb().osBuildNumber >= 9200)//Windows 8.0 or above
		{
			NTDLL::LdrSetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);
		}
	} };
}
//26040
//                                                                public LdrSetDefaultDllDirectories
//85 C9                                                           test    ecx, ecx
//74 23                                                           jz      short loc_1800E7387
//8B 05 92 14 0D 00                                               mov     eax, cs:LdrpPolicyBits
//83 E0 04                                                        and     eax, 4
//83 C8 7A                                                        or      eax, 7Ah
//C1 E0 08                                                        shl     eax, 8
//F7 D0                                                           not     eax
//0F BA E8 08                                                     bts     eax, 8
//85 C1                                                           test    ecx, eax
//75 0A                                                           jnz     short loc_1800E7387
//89 0D E5 5E 0D 00                                               mov     cs:LdrpDefaultDllDirectories, ecx
//33 C0                                                           xor     eax, eax
//C3                                                              retn
//CC                                                              db 0CCh
//B8 0D 00 00 C0                                                  mov     eax, 0C000000Dh
//C3                                                              retn

//22621
//85 C9                                                           test    ecx, ecx
//74 23                                                           jz      short loc_180078A07
//8B 05 AA 8C 10 00                                               mov     eax, cs:LdrpPolicyBits
//83 E0 04                                                        and     eax, 4
//83 C8 7A                                                        or      eax, 7Ah
//C1 E0 08                                                        shl     eax, 8
//F7 D0                                                           not     eax
//0F BA E8 08                                                     bts     eax, 8
//85 C1                                                           test    ecx, eax
//75 0A                                                           jnz     short loc_180078A07
//89 0D 41 C2 10 00                                               mov     cs:LdrpDefaultDllDirectories, ecx
//33 C0                                                           xor     eax, eax
//C3                                                              retn
//CC                                                              db 0CCh
//B8 0D 00 00 C0                                                  mov     eax, 0C000000Dh
//C3                                                              retn

//18363
//85 C9                                                           test    ecx, ecx
//74 22                                                           jz      short loc_18007E926
//8B 05 0A 3E 0E 00                                               mov     eax, cs:LdrpPolicyBits
//83 E0 04                                                        and     eax, 4
//83 C8 7A                                                        or      eax, 7Ah
//C1 E0 08                                                        shl     eax, 8
//F7 D0                                                           not     eax
//0F BA E8 08                                                     bts     eax, 8
//85 C1                                                           test    ecx, eax
//75 09                                                           jnz     short loc_18007E926
//89 0D F9 76 0E 00                                               mov     cs:LdrpDefaultDllDirectories, ecx
//33 C0                                                           xor     eax, eax
//C3                                                              retn
//B8 0D 00 00 C0                                                  mov     eax, 0C000000Dh
//C3                                                              retn