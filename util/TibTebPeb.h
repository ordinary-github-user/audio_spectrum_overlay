#pragma once
#include "AlgorithmIequal.h"
#include "ApiSetMap.h"
#include "Compiler.h"
#include "BitCast.h"
#include "DisableWarning.h"
#include "Inline.h"
#include "MacroEqual.h"
#include "MacroIf.h"
#include "PaddedStruct.h"
#include "Pragma.h"
#include "StringView.h"
#include "TString.h"
#include "WinSock2_NoMinMax.h"
#include "WinStructs.h"
#include <bit>
#include <intrin.h>
#include <winternl.h>//CLIENT_ID
namespace Windows
{
	struct ThisPeb;
	STRUCT_UNALIGNED(ThisTeb,
		(IF(EQUAL(BITNESS,64),0x00	,0x00 ), pExceptionList, EXCEPTION_REGISTRATION_RECORD*),
		(IF(EQUAL(BITNESS,64),0x08	,0x04 ), stackBase, void*),
		(IF(EQUAL(BITNESS,64),0x10	,0x08 ), stackLimit, void*),
		(IF(EQUAL(BITNESS,64),0x18	,0x0c ), subSystemTib, void*),
		(IF(EQUAL(BITNESS,64),0x20	,0x10 ), fiberData, void*),
		(IF(EQUAL(BITNESS,64),0x28	,0x14 ), arbitraryUserPointer, void*),
		(IF(EQUAL(BITNESS,64),0x30	,0x18 ), pSelf, ThisTeb*),
		(IF(EQUAL(BITNESS,64),0x38	,0x1c ), pEnvironment, void*),
		(IF(EQUAL(BITNESS,64),0x40	,0x20 ), clientID, CLIENT_ID),
		(IF(EQUAL(BITNESS,64),0x58	,0x2c ), pThreadLocalStorage, void*),
		(IF(EQUAL(BITNESS,64),0x60	,0x30 ), pPeb, ThisPeb*),
		(IF(EQUAL(BITNESS,64),0x68	,0x34 ), lastErrorValue, u32),
		(IF(EQUAL(BITNESS,64),0x100	,0xC0 ), pHeavensGate, void*),
		(IF(EQUAL(BITNESS,64),0x7D8	,0x6B4), realClientID, CLIENT_ID),
		(IF(EQUAL(BITNESS,64),0x17B8,0xFAC), hCurrentTransaction, HANDLE)
	);

	PRAGMA_GCC(GCC diagnostic push)PRAGMA_GCC(GCC diagnostic ignored "-Winvalid-offsetof")
	struct ThisTib
	{
	private:
		template<$Size<1> T>INLINE static T read(u32 offset)noexcept { return BITCAST(T,IF(EQUAL(BITNESS,64),__readgsbyte ,__readfsbyte )(offset)); }
		template<$Size<2> T>INLINE static T read(u32 offset)noexcept { return BITCAST(T,IF(EQUAL(BITNESS,64),__readgsword ,__readfsword )(offset)); }
		template<$Size<4> T>INLINE static T read(u32 offset)noexcept { return BITCAST(T,IF(EQUAL(BITNESS,64),__readgsdword,__readfsdword)(offset)); }
		template<$Size<8> T>INLINE static T read(u32 offset)noexcept { return BITCAST(T,IF(EQUAL(BITNESS,64),__readgsqword,__readfsqword)(offset)); }
		INLINE static void write(u32 offset, $Size<1> auto x)noexcept { IF(EQUAL(BITNESS,64),__writegsbyte ,__writefsbyte )(offset, (__builtin_bit_cast(u8 ,x))); }
		INLINE static void write(u32 offset, $Size<2> auto x)noexcept { IF(EQUAL(BITNESS,64),__writegsword ,__writefsword )(offset, (__builtin_bit_cast(u16,x))); }
		INLINE static void write(u32 offset, $Size<4> auto x)noexcept { IF(EQUAL(BITNESS,64),__writegsdword,__writefsdword)(offset, (__builtin_bit_cast(u32,x))); }
		INLINE static void write(u32 offset, $Size<8> auto x)noexcept { IF(EQUAL(BITNESS,64),__writegsqword,__writefsqword)(offset, (__builtin_bit_cast(u64,x))); }
	public:
#define DEFINE_TIB_ENTRY(x) INLINE static std::remove_cvref_t<decltype(std::declval<ThisTeb>().x)> x()noexcept{return read<std::remove_cvref_t<decltype(std::declval<ThisTeb>().x)>>(__builtin_offsetof(ThisTeb, x));}\
							INLINE static void x ## _set(std::remove_cvref_t<decltype(std::declval<ThisTeb>().x)> value)noexcept{return write(__builtin_offsetof(ThisTeb, x), value);}
		DEFINE_TIB_ENTRY(pExceptionList);
		DEFINE_TIB_ENTRY(stackBase);
		DEFINE_TIB_ENTRY(stackLimit);
		DEFINE_TIB_ENTRY(subSystemTib);
		DEFINE_TIB_ENTRY(fiberData);
		DEFINE_TIB_ENTRY(arbitraryUserPointer);
		DEFINE_TIB_ENTRY(pSelf);
		DEFINE_TIB_ENTRY(pEnvironment);
		//DEFINE_TIB_ENTRY(clientID);
		DEFINE_TIB_ENTRY(pThreadLocalStorage);
		DEFINE_TIB_ENTRY(pPeb);
		DEFINE_TIB_ENTRY(lastErrorValue);
		DEFINE_TIB_ENTRY(pHeavensGate);
		//DEFINE_TIB_ENTRY(realClientID);
		DEFINE_TIB_ENTRY(hCurrentTransaction);
#undef DEFINE_TIB_ENTRY

		INLINE static u32 processID() noexcept { return read<u32>(__builtin_offsetof(ThisTeb, clientID) + __builtin_offsetof(CLIENT_ID, UniqueProcess)); }
		INLINE static u32 threadID() noexcept { return read<u32>(__builtin_offsetof(ThisTeb, clientID) + __builtin_offsetof(CLIENT_ID, UniqueThread)); }
		INLINE static u32 realProcessID() noexcept { return read<u32>(__builtin_offsetof(ThisTeb, realClientID) + __builtin_offsetof(CLIENT_ID, UniqueProcess)); }
		INLINE static u32 realThreadID() noexcept { return read<u32>(__builtin_offsetof(ThisTeb, realClientID) + __builtin_offsetof(CLIENT_ID, UniqueThread)); }
	};
	PRAGMA_GCC(GCC diagnostic pop)

	//INLINE ThisTeb& thisTeb()noexcept { return *ThisTib::pSelf(); }//use ThisTib instead

	struct ThisCurDir
	{
		UnicodeString DosPath;
		void* Handle;
	};

	struct ThisString
	{
		u16 Length;
		u16 MaximumLength;
		c8* Buffer;
	};

	struct ThisDriveLetterCurdir
	{
		u16 Flags;
		u16 Length;
		u32 TimeStamp;
		ThisString DosPath;
	};

	struct ThisProcessParameters
	{
		u32 MaximumLength;
		u32 Length;
		u32 Flags;
		u32 DebugFlags;
		void* ConsoleHandle;
		u32 ConsoleFlags;
		void* StandardInput;
		void* StandardOutput;
		void* StandardError;
		ThisCurDir CurrentDirectory;
		UnicodeString DllPath;
		UnicodeString ImagePathName;
		UnicodeString CommandLine;
		void* Environment;
		u32 StartingX;
		u32 StartingY;
		u32 CountX;
		u32 CountY;
		u32 CountCharsX;
		u32 CountCharsY;
		u32 FillAttribute;
		u32 WindowFlags;
		u32 ShowWindowFlags;
		UnicodeString WindowTitle;
		UnicodeString DesktopInfo;
		UnicodeString ShellInfo;
		UnicodeString RuntimeData;
		std::array<ThisDriveLetterCurdir, 32> CurrentDirectories;
		usize EnvironmentSize;
		usize EnvironmentVersion;
	};

	STRUCT_UNALIGNED(ThisPeb,
		(IF(EQUAL(BITNESS, 64), 0x2, 0x2), beingDebugged, u8),
		(IF(EQUAL(BITNESS, 64), 0x10, 0x8), imageBase, u8*),
		(IF(EQUAL(BITNESS, 64), 0x18, 0xC), pLdr, PebLdrData*),
		(IF(EQUAL(BITNESS, 64), 0x20, 0x10), pProcessParameters, ThisProcessParameters*),
		(IF(EQUAL(BITNESS, 64), 0x30, 0x18), processHeap, void*),
		(IF(EQUAL(BITNESS, 64), 0x50, 0x28), crossProcessFlags, CrossProcessFlags),
		(IF(EQUAL(BITNESS, 64), 0x68, 0x38), pApiSetMap, void*),
		(IF(EQUAL(BITNESS, 64), 0xB8, 0x64), numberOfProcessors, u32),
		(IF(EQUAL(BITNESS, 64), 0xBC, 0x68), ntGlobalFlag, u32),
		(IF(EQUAL(BITNESS, 64), 0x118, 0xA4), osMajorVersion, u32),
		(IF(EQUAL(BITNESS, 64), 0x11C, 0xA8), osMinorVersion, u32),
		(IF(EQUAL(BITNESS, 64), 0x120, 0xAC), osBuildNumber, u16),
		(IF(EQUAL(BITNESS, 64), 0x122, 0xAE), osCsdVersion, u16),
		(IF(EQUAL(BITNESS, 64), 0x2C0, 0x1D4), sessionId, u32),
		(IF(EQUAL(BITNESS, 64), 0x2E8, 0x1F0), osCSDVersionStr, UnicodeString)
	);

	INLINE ThisPeb& thisPeb()noexcept { return *ThisTib::pPeb(); }

	struct LocalModules
	{
		struct Iterator
		{
			INLINE Iterator(LdrDataTableEntry* ptr)noexcept :m_ptr{ ptr } {}
			INLINE Iterator& operator++() noexcept { m_ptr = m_ptr->pNext(); return *this; }

			INLINE bool operator!=(Iterator rhs)const noexcept { return m_ptr != rhs.m_ptr; }
			INLINE bool operator==(Iterator rhs)const noexcept { return m_ptr == rhs.m_ptr; }
			INLINE LdrDataTableEntry& operator*() const noexcept { return *m_ptr; }
		private:
			LdrDataTableEntry* m_ptr;
		};

		LdrDataTableEntry* m_pBegin;
		LdrDataTableEntry* m_pEnd;

		INLINE LocalModules()noexcept :m_pBegin{ reinterpret_cast<LdrDataTableEntry*>(thisPeb().pLdr->InLoadOrderModuleList.Flink) }, m_pEnd{ reinterpret_cast<LdrDataTableEntry*>(m_pBegin->InLoadOrderLinks.Blink) } {}

		INLINE Iterator begin()const noexcept { return m_pBegin; }
		INLINE Iterator end()const noexcept { return m_pEnd; }

		INLINE const LdrDataTableEntry* getPtrEntry($Sig<bool(const LdrDataTableEntry)> auto&& match)const noexcept
		{
			for (const LdrDataTableEntry& entry : *this)
				if (match(entry))
					return &entry;
			return nullptr;
		}

		static INLINE LdrDataTableEntry& exe()noexcept { return *reinterpret_cast<LdrDataTableEntry*>(thisPeb().pLdr->InLoadOrderModuleList.Flink); }
		static INLINE LdrDataTableEntry& ntdll()noexcept { return *exe().pNext(); }
		static INLINE void* exeBase()noexcept { return exe().DllBase; }
		static INLINE void* ntdllBase()noexcept { return ntdll().DllBase; }

		INLINE const LdrDataTableEntry* getPtrEntry($TString auto moduleName)const noexcept
		{
			return getPtrEntry([&](const LdrDataTableEntry& entry)LAMBDA_INLINE
			{
				return entry.BaseDllName.Buffer != nullptr && iequal(moduleName, entry.BaseDllName);
			});
		}
		INLINE const LdrDataTableEntry* getPtrEntry(StringView8 moduleName)const noexcept
		{
			return getPtrEntry([&](const LdrDataTableEntry& entry)LAMBDA_INLINE
			{
				return entry.BaseDllName.Buffer != nullptr && modNameEqual(entry.BaseDllName, moduleName);
			});
		}

		INLINE void* getModuleBase($TString auto moduleName)const noexcept
		{
			const LdrDataTableEntry* pEntry{ getPtrEntry(moduleName) };
			return pEntry ? pEntry->DllBase : nullptr;
		}

		INLINE void* getModuleBase(StringView8 moduleName)const noexcept
		{
			const LdrDataTableEntry* pEntry{ getPtrEntry(moduleName) };
			return pEntry ? pEntry->DllBase : nullptr;
		}

		template<class Stream>
		requires(sizeof(typename Stream::char_type) == sizeof(c16))
		friend Stream& operator<<(Stream& os, const LocalModules& localModules) noexcept
		{
			bool isFirst{true};
			for (const LdrDataTableEntry& entry : localModules)
			{
				if (isFirst)
					isFirst = false;
				else
					os << '\n';
				os << entry;
			}
			return os;
		}

	private:

		INLINE static bool modNameEqual(StringView16 lhs, StringView8 rhs) noexcept
		{
			if (lhs.size() > 4 && iequal<L".dll">(lhs.substr(lhs.size() - 4)))lhs = lhs.substr(0, lhs.size() - 4);//TODO
			if (rhs.size() > 4 && iequal< ".dll">(rhs.substr(rhs.size() - 4)))rhs = rhs.substr(0, rhs.size() - 4);

			if (lhs.size() != rhs.size())return false;

			const c16* pL{ lhs.data() };
			const c16* pLEnd{ pL + lhs.size() };
			const c8* pR{ rhs.data() };
			for (; pL != pLEnd; ++pL, ++pR)
			{
				const c16 l{ *pL };
				const c8 r{ *pR };

				if (Algorithm::tolower(l) != Algorithm::tolower(r))
					return false;
			}
			return true;
		}
	};
}