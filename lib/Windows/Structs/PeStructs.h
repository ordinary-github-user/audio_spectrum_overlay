#pragma once
#include <array>
#include "../../Util/BasicTypes.h"
#include "../../Util/ConditionalConst.h"
#include "../../Util/DataStructures/Span.h"
#include "../../Util/DataStructures/StringView.h"
#include "../../Util/EnumDefinition.h"
#include "../../Util/Fold.h"
#include "../../Util/StringFormat/HexToStream.h"
namespace Windows
{
#pragma pack (push, 1)
	template<$AnyOf<u16, u32> SectionNumberType = u16>
	struct ImageSymbol
	{
		union
		{
			c8    ShortName[8];
			struct
			{
				u32   Short;     // if 0, use LongName
				u32   Long;      // offset into string table
			} Name;
			u32   LongName[2];    // PBYTE [2]
		} N;
		u32 Value;
		SectionNumberType SectionNumber;
		u16 Type;
		u8  StorageClass;
		u8  NumberOfAuxSymbols;

		bool isShort()const noexcept { return N.Name.Short != 0; }
		bool isLong()const noexcept { return !isShort(); }

		template<$Size<1> Char>
		StringView<ConditionalConst<c8, std::is_const_v<Char>>> getLongName(Char* pBase)const noexcept
		{
			return {reinterpret_cast<c8*>(pBase) + N.Name.Long};
		}

		template<class Stream, class...Base>
		void toStream(Stream& os, Base&&...pBase)const noexcept
		{
			os << TS("Value = ") << hex(Value);
			os << TS(", SectionNumber = ") << hex(SectionNumber);
			os << TS(", Type = ") << hex(Type);
			os << TS(", StorageClass = ") << hex(StorageClass);
			os << TS(", NumberOfAuxSymbols = ") << hex(NumberOfAuxSymbols);
			if (isLong())
			{
				const u32 offset{ N.Name.Long };
				if constexpr (sizeof...(pBase))
				{
					const StringView8 name{ reinterpret_cast<const char*>(&(pBase,...)[offset]) };
					os << TS(", Name = ") << name;
				}
				else
					os << TS(", Offset = ") << hex(offset);
			}
			else
			{
				const FixedString<c8, 8> nameBuffer{[&] <usize...I>(std::index_sequence<I...>) 
				{
					return FixedString<c8, 8>{(N.ShortName[I] ? N.ShortName[I] : ' ')...};
				}(std::make_index_sequence<8>{})};
				os << TS(", Name = ");
				if constexpr (sizeof(typename Stream::char_type) == 1)
					os << nameBuffer;
				else if constexpr (sizeof(typename Stream::char_type) == 2)
					os << utf8ToUtf16<HaltAtNull::yes>(nameBuffer);
				else
					STATIC_ERROR();
			}
		}

		template<class Stream>
		friend Stream& operator<<(Stream& os, const ImageSymbol& symbol)noexcept
		{
			symbol.toStream(os);
			return os;
		}
	};//STATIC_ASSERT(sizeof(IMAGE_SYMBOL) == sizeof(ImageSymbol<>));
#pragma pack (pop)

	struct ImageImportByName 
	{
		u16 Hint;
		c8  Name[1];
		INLINE StringView8 name()const noexcept { return { &Name[0] }; }
	};
	DEFINE_ENUM(ImageDirectoryEntry, u32,
		(export_,		0),
		(import_,		1),
		(resource,		2),
		(exception,		3),
		(security,		4),
		(basereloc,		5),
		(debug,			6),
		(architecture,	7),
		(globalptr,		8),
		(tls,			9),
		(load_config,	10),
		(bound_import,	11),
		(iat,			12),
		(delay_import,	13),
		(com_descriptor,14),
		(reserved,		15)
	)
	DEFINE_ENUM(ImageDebugType, u32,
		(unknown               , 0 ),
		(coff                  , 1 ),
		(codeview              , 2 ),
		(fpo                   , 3 ),
		(misc                  , 4 ),
		(exception             , 5 ),
		(fixup                 , 6 ),
		(omap_to_src           , 7 ),
		(omap_from_src         , 8 ),
		(borland               , 9 ),
		(reserved10            , 10),
		(clsid                 , 11),
		(vc_feature            , 12),
		(pogo                  , 13),
		(iltcg                 , 14),
		(mpx                   , 15),
		(repro                 , 16),
		(embedded_portable_pdb , 17),
		(pdb_checksum          , 19),
		(ex_dllcharacteristics , 20)
	)
	DEFINE_ENUM(ImageFileMachine,u16,
		(unknown     , 0),
		(target_host , 0x0001),	// Useful for indicating we want to interact with the host and not a WoW guest.
		(i386        , 0x014c),	// Intel 386.
		(r3000       , 0x0162),	// MIPS little-endian, 0x160 big-endian
		(r4000       , 0x0166),	// MIPS little-endian
		(r10000      , 0x0168),	// MIPS little-endian
		(wcemipsv2   , 0x0169),	// MIPS little-endian WCE v2
		(alpha       , 0x0184),	// Alpha_AXP
		(sh3         , 0x01a2),	// SH3 little-endian
		(sh3dsp      , 0x01a3),	
		(sh3e        , 0x01a4),	// SH3E little-endian
		(sh4         , 0x01a6),	// SH4 little-endian
		(sh5         , 0x01a8),	// SH5
		(arm         , 0x01c0),	// ARM Little-Endian
		(thumb       , 0x01c2),	// ARM Thumb/Thumb-2 Little-Endian
		(armnt       , 0x01c4),	// ARM Thumb-2 Little-Endian
		(am33        , 0x01d3),	
		(powerpc     , 0x01F0),	// IBM PowerPC Little-Endian
		(powerpcfp   , 0x01f1),	
		(ia64        , 0x0200),	// Intel 64
		(mips16      , 0x0266),	// MIPS
		(alpha64     , 0x0284),	// ALPHA64
		(mipsfpu     , 0x0366),	// MIPS
		(mipsfpu16   , 0x0466),	// MIPS
								// axp64       = alpha64,
		(tricore     , 0x0520),	// Infineon
		(cef         , 0x0CEF),	
		(ebc         , 0x0EBC),	// EFI Byte Code
		(amd64       , 0x8664),	// AMD64 (K8)
		(m32r        , 0x9041),	// M32R little-endian
		(arm64       , 0xAA64),	// ARM64 Little-Endian
		(cee         , 0xC0EE)
	)
	DEFINE_ENUM(ImageSubsystem,u16,
		(unknown					, 0 ),//Unknown subsystem.
		(native						, 1 ),//Image doesn't require a subsystem.
		(windows_gui				, 2 ),//Image runs in the Windows GUI subsystem.
		(windows_cui				, 3 ),//Image runs in the Windows character subsystem.
		(os2_cui					, 5 ),//image runs in the OS/2 character subsystem.
		(posix_cui					, 7 ),//image runs in the Posix character subsystem.
		(native_windows				, 8 ),//image is a native Win9x driver.
		(windows_ce_gui				, 9 ),//Image runs in the Windows CE subsystem.
		(efi_application			, 10),
		(efi_boot_service_driver	, 11),
		(efi_runtime_driver			, 12),
		(efi_rom					, 13),
		(xbox						, 14),
		(windows_boot_application	, 16),
		(xbox_code_catalog			, 17)
	)
	FLAG(ImageSectionFlag,u32)
	{
		type_reg                   = 0x00000000,  // Reserved.
		type_dsect                 = 0x00000001,  // Reserved.
		type_noload                = 0x00000002,  // Reserved.
		type_group                 = 0x00000004,  // Reserved.
		type_no_pad                = 0x00000008,  // Reserved.
		type_copy                  = 0x00000010,  // Reserved.
		cnt_code                   = 0x00000020,  // Section contains code.
		cnt_initialized_data       = 0x00000040,  // Section contains initialized data.
		cnt_uninitialized_data     = 0x00000080,  // Section contains uninitialized data.
		lnk_other                  = 0x00000100,  // Reserved.
		lnk_info                   = 0x00000200,  // Section contains comments or some other type of information.
		type_over                  = 0x00000400,  // Reserved.
		lnk_remove                 = 0x00000800,  // Section contents will not become part of image.
		lnk_comdat                 = 0x00001000,  // Section contents comdat.
		no_defer_spec_exc          = 0x00004000,  // Reset speculative exceptions handling bits in the TLB entries for this section.
		gprel                      = 0x00008000,  // Section content can be accessed relative to GP
		mem_fardata                = 0x00008000,
		mem_sysheap                = 0x00010000,
		mem_purgeable              = 0x00020000,
		mem_16bit                  = 0x00020000,
		mem_locked                 = 0x00040000,
		mem_preload                = 0x00080000,
		align_1bytes               = 0x00100000,  //
		align_2bytes               = 0x00200000,  //
		align_4bytes               = 0x00300000,  //
		align_8bytes               = 0x00400000,  //
		align_16bytes              = 0x00500000,  // Default alignment if no others are specified.
		align_32bytes              = 0x00600000,  //
		align_64bytes              = 0x00700000,  //
		align_128bytes             = 0x00800000,  //
		align_256bytes             = 0x00900000,  //
		align_512bytes             = 0x00A00000,  //
		align_1024bytes            = 0x00B00000,  //
		align_2048bytes            = 0x00C00000,  //
		align_4096bytes            = 0x00D00000,  //
		align_8192bytes            = 0x00E00000,  //
		align_mask                 = 0x00F00000,
		lnk_nreloc_ovfl            = 0x01000000,  // Section contains extended relocations.
		mem_discardable            = 0x02000000,  // Section can be discarded.
		mem_not_cached             = 0x04000000,  // Section is not cachable.
		mem_not_paged              = 0x08000000,  // Section is not pageable.
		mem_shared                 = 0x10000000,  // Section is shareable.
		mem_execute                = 0x20000000,  // Section is executable.
		mem_read                   = 0x40000000,  // Section is readable.
		mem_write                  = 0x80000000  // Section is writeable.
	};

	FLAG(ImageGuardFlag,u32)
	{
		cf_instrumented                    = 0x00000100, // Module performs control flow integrity checks using system-supplied support
		cfw_instrumented                   = 0x00000200, // Module performs control flow and write integrity checks
		cf_function_table_present          = 0x00000400, // Module contains valid control flow target metadata
		security_cookie_unused             = 0x00000800, // Module does not make use of the /GS security cookie
		protect_delayload_iat              = 0x00001000, // Module supports read only delay load IAT
		delayload_iat_in_its_own_section   = 0x00002000, // Delayload import table in its own .didat section (with nothing else in it) that can be freely reprotected
		cf_export_suppression_info_present = 0x00004000, // Module contains suppressed export information. This also infers that the address taken
		cf_enable_export_suppression       = 0x00008000, // Module enables suppression of exports
		cf_longjump_table_present          = 0x00010000, // Module contains longjmp target information
		rf_instrumented                    = 0x00020000, // Module contains return flow instrumentation and metadata
		rf_enable                          = 0x00040000, // Module requests that the OS enable return flow protection
		rf_strict                          = 0x00080000, // Module requests that the OS enable return flow protection in strict mode
		retpoline_present                  = 0x00100000, // Module was built with retpoline support
		eh_continuation_table_present      = 0x00400000 // Module contains EH continuation target information
	};

	FLAG(ImageDllCharacteristicsFlag,u16)
	{
		high_entropy_va			= 0x0020,	  // Image can handle a high entropy 64-bit virtual address space.
		dynamic_base			= 0x0040,     // DLL can move.
		force_integrity			= 0x0080,     // Code Integrity Image
		nx_compat				= 0x0100,     // Image is NX compatible
		no_isolation			= 0x0200,     // Image understands isolation and doesn't want it
		no_seh					= 0x0400,     // Image does not use SEH.  No SE handler may reside in this image
		no_bind					= 0x0800,     // Do not bind this image.
		appcontainer			= 0x1000,     // Image should execute in an AppContainer
		wdm_driver				= 0x2000,     // Driver uses WDM model
		guard_cf				= 0x4000,     // Image supports Control Flow Guard.
		terminal_server_aware   = 0x8000
	};

	FLAG(ImageFileFlag,u16)
	{
		relocs_stripped			= 0x0001,//Relocation info stripped from file.
		executable_image		= 0x0002,//File is executable  (i.e. no unresolved external references).
		line_nums_stripped		= 0x0004,//Line nunbers stripped from file.
		local_syms_stripped		= 0x0008,//Local symbols stripped from file.
		aggresive_ws_trim		= 0x0010,//Aggressively trim working set
		large_address_aware		= 0x0020,//App can handle >2gb addresses
		bytes_reversed_lo		= 0x0080,//Bytes of machine word are reversed.
		_32bit_machine			= 0x0100,//32 bit word machine.
		debug_stripped			= 0x0200,//Debugging info stripped from file in .DBG file
		removable_run_from_swap	= 0x0400,//If Image is on removable media, copy and run from the swap file.
		net_run_from_swap		= 0x0800,//If Image is on Net, copy and run from the swap file.
		system					= 0x1000,//System File.
		dll						= 0x2000,//File is a DLL.
		up_system_only			= 0x4000,//File should only be run on a UP machine
		bytes_reversed_hi		= 0x8000 //Bytes of machine word are reversed.
	};

	struct PdbInfo
	{
		std::array<c8, 4> header;
		std::array<u8, 16> signature;
		u32 age;
		c8 fileName[1];
	};

	struct ImageDosHeader//IMAGE_DOS_HEADER;
	{
		u16                 e_magic;      // Magic number
		u16                 e_cblp;       // Bytes on last page of file
		u16                 e_cp;         // Pages in file
		u16                 e_crlc;       // Relocations
		u16                 e_cparhdr;    // Size of header in paragraphs
		u16                 e_minalloc;   // Minimum extra paragraphs needed
		u16                 e_maxalloc;   // Maximum extra paragraphs needed
		u16                 e_ss;         // Initial (relative) SS value
		u16                 e_sp;         // Initial SP value
		u16                 e_csum;       // Checksum
		u16                 e_ip;         // Initial IP value
		u16                 e_cs;         // Initial (relative) CS value
		u16                 e_lfarlc;     // File address of relocation table
		u16                 e_ovno;       // Overlay number
		std::array<u16, 4>  e_res;        // Reserved words
		u16                 e_oemid;      // OEM identifier (for e_oeminfo)
		u16                 e_oeminfo;    // OEM information; e_oemid specific
		std::array<u16, 10> e_res2;       // Reserved words
		u32                 e_lfanew;     // File address of new exe header

		std::array<u8, 0x3a>& padding()noexcept { return *reinterpret_cast<std::array<u8, 0x3a>*>(&e_cblp); }
		const std::array<u8, 0x3a>& padding()const noexcept { return *reinterpret_cast<const std::array<u8, 0x3a>*>(&e_cblp); }
	};
	struct ImageSectionHeader//using ImageSectionHeader = IMAGE_SECTION_HEADER;
	{
		std::array<c8, 8>		Name;
		u32						VirtualSize;
		u32						VirtualAddress;
		u32						SizeOfRawData;
		u32						PointerToRawData;
		u32						PointerToRelocations;
		u32						PointerToLinenumbers;
		u16						NumberOfRelocations;
		u16						NumberOfLinenumbers;
		ImageSectionFlag		Characteristics;

		INLINE StringView8 name()const noexcept
		{
			const c8* pEnd{ Name.data() };
			for (; pEnd != Name.data() + Name.size() && *pEnd; ++pEnd);
			return { Name.data(), pEnd };
		}
	};

	struct ImageFileHeader//using ImageFileHeader = _IMAGE_FILE_HEADER;
	{
		ImageFileMachine   Machine;
		u16   NumberOfSections;
		u32   TimeDateStamp;
		u32   PointerToSymbolTable;
		u32   NumberOfSymbols;
		u16   SizeOfOptionalHeader;
		ImageFileFlag   Characteristics;
	};

	struct ImageImportDescriptor//using ImageImportDescriptor = _IMAGE_IMPORT_DESCRIPTOR;
	{
		union {
			u32   Characteristics;            // 0 for terminating null import descriptor
			u32   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
		};
		u32   TimeDateStamp;                  // 0 if not bound,
											  // -1 if bound, and real date\time stamp
											  //     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
											  // O.W. date/time stamp of DLL bound to (Old BIND)

		u32   ForwarderChain;                 // -1 if no forwarders
		u32   Name;
		u32   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
	};

	struct ImageDataDirectory//using ImageDataDirectory = IMAGE_DATA_DIRECTORY;
	{
		u32   VirtualAddress;
		u32   Size;
	};

	struct ImageDebugDirectory//using ImageDebugDirectory = IMAGE_DEBUG_DIRECTORY;
	{
		u32   Characteristics;
		u32   TimeDateStamp;
		u16   MajorVersion;
		u16   MinorVersion;
		ImageDebugType   Type;
		u32   SizeOfData;
		u32   AddressOfRawData;
		u32   PointerToRawData;

		constexpr Span<const u8> rawData(const u8* pBase)const noexcept { return { pBase + PointerToRawData, SizeOfData }; }
		constexpr Span<u8> rawData(u8* pBase)const noexcept { return { pBase + PointerToRawData, SizeOfData }; }
	};

	struct ImageExportDirectory//using ImageExportDirectory = IMAGE_EXPORT_DIRECTORY;
	{
		u32   Characteristics;
		u32   TimeDateStamp;
		u16   MajorVersion;
		u16   MinorVersion;
		u32   Name;
		u32   Base;
		u32   NumberOfFunctions;
		u32   NumberOfNames;
		u32   AddressOfFunctions;     // RVA from base of image
		u32   AddressOfNames;         // RVA from base of image
		u32   AddressOfNameOrdinals;  // RVA from base of image
	};

	struct ImageResourceDirectory//using ImageResourceDirectory = IMAGE_RESOURCE_DIRECTORY;
	{
		u32   Characteristics;
		u32   TimeDateStamp;
		u16   MajorVersion;
		u16   MinorVersion;
		u16   NumberOfNamedEntries;
		u16   NumberOfIdEntries;
	};

	struct ImageResourceDirectoryEntry//using ImageResourceDirectoryEntry = IMAGE_RESOURCE_DIRECTORY_ENTRY;
	{
		union {
			struct {
				u32 NameOffset : 31;
				u32 NameIsString : 1;
			} u0;
			u32   Name;
			u16    Id;
		};
		union {
			u32   OffsetToData;
			struct {
				u32   OffsetToDirectory : 31;
				u32   DataIsDirectory : 1;
			} u1;
		};
	};

	template<u8 Bits>union ImageThunkData;//template<u8 Bits>using ImageThunkData = std::conditional_t<Bits == 64, IMAGE_THUNK_DATA64, IMAGE_THUNK_DATA32>;

	template<>
	union ImageThunkData<64>
	{
		u64 ForwarderString;  // PBYTE 
		u64 Function;         // PDWORD
		u64 Ordinal;
		u64 AddressOfData;    // PIMAGE_IMPORT_BY_NAME
	};

	template<>
	union ImageThunkData<32>
	{
		u32 ForwarderString;      // PBYTE 
		u32 Function;             // PDWORD
		u32 Ordinal;
		u32 AddressOfData;        // PIMAGE_IMPORT_BY_NAME
	};

	struct ImageLoadConfigCodeIntegrity
	{
		u16   Flags;          // Flags to indicate if CI information is available, etc.
		u16   Catalog;        // 0xFFFF means not available
		u32   CatalogOffset;
		u32   Reserved;       // Additional bitmask to be defined later
	};

	template<u8 Bits>struct ImageLoadConfigDirectory;//template<u8 Bits>using ImageLoadConfigDirectory = std::conditional_t<Bits == 64, IMAGE_LOAD_CONFIG_DIRECTORY64, IMAGE_LOAD_CONFIG_DIRECTORY32>;

	template<>
	struct ImageLoadConfigDirectory<64>
	{
		u32								Size;
		u32								TimeDateStamp;
		u16								MajorVersion;
		u16								MinorVersion;
		u32								GlobalFlagsClear;
		u32								GlobalFlagsSet;
		u32								CriticalSectionDefaultTimeout;
		u64								DeCommitFreeBlockThreshold;
		u64								DeCommitTotalFreeThreshold;
		u64								LockPrefixTable;                // VA
		u64								MaximumAllocationSize;
		u64								VirtualMemoryThreshold;
		u64								ProcessAffinityMask;
		u32								ProcessHeapFlags;
		u16								CSDVersion;
		u16								DependentLoadFlags;
		u64								EditList;                       // VA
		u64								SecurityCookie;                 // VA
		u64								SEHandlerTable;                 // VA
		u64								SEHandlerCount;
		u64								GuardCFCheckFunctionPointer;    // VA
		u64								GuardCFDispatchFunctionPointer; // VA
		u64								GuardCFFunctionTable;           // VA
		u64								GuardCFFunctionCount;
		ImageGuardFlag					GuardFlags;
		ImageLoadConfigCodeIntegrity	CodeIntegrity;
		u64								GuardAddressTakenIatEntryTable; // VA
		u64								GuardAddressTakenIatEntryCount;
		u64								GuardLongJumpTargetTable;       // VA
		u64								GuardLongJumpTargetCount;
		u64								DynamicValueRelocTable;         // VA
		u64								CHPEMetadataPointer;            // VA
		u64								GuardRFFailureRoutine;          // VA
		u64								GuardRFFailureRoutineFunctionPointer; // VA
		u32								DynamicValueRelocTableOffset;
		u16								DynamicValueRelocTableSection;
		u16								Reserved2;
		u64								GuardRFVerifyStackPointerFunctionPointer; // VA
		u32								HotPatchTableOffset;
		u32								Reserved3;
		u64								EnclaveConfigurationPointer;     // VA
		u64								VolatileMetadataPointer;         // VA
		u64								GuardEHContinuationTable;        // VA
		u64								GuardEHContinuationCount;
		u64								GuardXFGCheckFunctionPointer;    // VA
		u64								GuardXFGDispatchFunctionPointer; // VA
		u64								GuardXFGTableDispatchFunctionPointer; // VA
		u64								CastGuardOsDeterminedFailureMode; // VA
		u64								GuardMemcpyFunctionPointer; // VA
		u64								UmaFunctionPointers;
	};
	//STATIC_ASSERT(sizeof(ImageLoadConfigDirectory<64>) == sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64));

	template<>
	struct ImageLoadConfigDirectory<32>
	{
		u32								Size;
		u32								TimeDateStamp;
		u16								MajorVersion;
		u16								MinorVersion;
		u32								GlobalFlagsClear;
		u32								GlobalFlagsSet;
		u32								CriticalSectionDefaultTimeout;
		u32								DeCommitFreeBlockThreshold;
		u32								DeCommitTotalFreeThreshold;
		u32								LockPrefixTable;                // VA
		u32								MaximumAllocationSize;
		u32								VirtualMemoryThreshold;
		u32								ProcessHeapFlags;
		u32								ProcessAffinityMask;
		u16								CSDVersion;
		u16								DependentLoadFlags;
		u32								EditList;                       // VA
		u32								SecurityCookie;                 // VA
		u32								SEHandlerTable;                 // VA
		u32								SEHandlerCount;
		u32								GuardCFCheckFunctionPointer;    // VA
		u32								GuardCFDispatchFunctionPointer; // VA
		u32								GuardCFFunctionTable;           // VA
		u32								GuardCFFunctionCount;
		ImageGuardFlag					GuardFlags;
		ImageLoadConfigCodeIntegrity	CodeIntegrity;
		u32								GuardAddressTakenIatEntryTable; // VA
		u32								GuardAddressTakenIatEntryCount;
		u32								GuardLongJumpTargetTable;       // VA
		u32								GuardLongJumpTargetCount;
		u32								DynamicValueRelocTable;         // VA
		u32								CHPEMetadataPointer;
		u32								GuardRFFailureRoutine;          // VA
		u32								GuardRFFailureRoutineFunctionPointer; // VA
		u32								DynamicValueRelocTableOffset;
		u16								DynamicValueRelocTableSection;
		u16								Reserved2;
		u32								GuardRFVerifyStackPointerFunctionPointer; // VA
		u32								HotPatchTableOffset;
		u32								Reserved3;
		u32								EnclaveConfigurationPointer;    // VA
		u32								VolatileMetadataPointer;        // VA
		u32								GuardEHContinuationTable;       // VA
		u32								GuardEHContinuationCount;
		u32								GuardXFGCheckFunctionPointer;    // VA
		u32								GuardXFGDispatchFunctionPointer; // VA
		u32								GuardXFGTableDispatchFunctionPointer; // VA
		u32								CastGuardOsDeterminedFailureMode; // VA
		u32								GuardMemcpyFunctionPointer; // VA
		u32								UmaFunctionPointers;
	}; 
	//STATIC_ASSERT(sizeof(ImageLoadConfigDirectory<32>) == sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32));

	template<u8 Bits>struct ImageOptionalHeader;//template<u8 Bits>using ImageOptionalHeader = std::conditional_t<Bits == 64, IMAGE_OPTIONAL_HEADER64, IMAGE_OPTIONAL_HEADER32>;

	template<>
	struct ImageOptionalHeader<64>
	{
		u16   Magic;
		u8    MajorLinkerVersion;
		u8    MinorLinkerVersion;
		u32   SizeOfCode;
		u32   SizeOfInitializedData;
		u32   SizeOfUninitializedData;
		u32   AddressOfEntryPoint;
		u32   BaseOfCode;
		u64   ImageBase;
		u32   SectionAlignment;
		u32   FileAlignment;
		u16   MajorOperatingSystemVersion;
		u16   MinorOperatingSystemVersion;
		u16   MajorImageVersion;
		u16   MinorImageVersion;
		u16   MajorSubsystemVersion;
		u16   MinorSubsystemVersion;
		u32   Win32VersionValue;
		u32   SizeOfImage;
		u32   SizeOfHeaders;
		u32   CheckSum;
		ImageSubsystem   Subsystem;
		ImageDllCharacteristicsFlag   DllCharacteristics;
		u64   SizeOfStackReserve;
		u64   SizeOfStackCommit;
		u64   SizeOfHeapReserve;
		u64   SizeOfHeapCommit;
		u32   LoaderFlags;
		u32   NumberOfRvaAndSizes;
		std::array<ImageDataDirectory, 16> DataDirectory;
	};

	template<>
	struct ImageOptionalHeader<32>
	{
		u16   Magic;
		u8    MajorLinkerVersion;
		u8    MinorLinkerVersion;
		u32   SizeOfCode;
		u32   SizeOfInitializedData;
		u32   SizeOfUninitializedData;
		u32   AddressOfEntryPoint;
		u32   BaseOfCode;
		u32   BaseOfData;
		u32   ImageBase;
		u32   SectionAlignment;
		u32   FileAlignment;
		u16   MajorOperatingSystemVersion;
		u16   MinorOperatingSystemVersion;
		u16   MajorImageVersion;
		u16   MinorImageVersion;
		u16   MajorSubsystemVersion;
		u16   MinorSubsystemVersion;
		u32   Win32VersionValue;
		u32   SizeOfImage;
		u32   SizeOfHeaders;
		u32   CheckSum;
		ImageSubsystem   Subsystem;
		ImageDllCharacteristicsFlag   DllCharacteristics;
		u32   SizeOfStackReserve;
		u32   SizeOfStackCommit;
		u32   SizeOfHeapReserve;
		u32   SizeOfHeapCommit;
		u32   LoaderFlags;
		u32   NumberOfRvaAndSizes;
		std::array<ImageDataDirectory, 16> DataDirectory;
	};

	template<u8 Bits>//template<u8 Bits>using ImageNtHeader = std::conditional_t<Bits == 64, IMAGE_NT_HEADERS64, IMAGE_NT_HEADERS32>;
	struct ImageNtHeader
	{
		u32 Signature;
		ImageFileHeader FileHeader;
		ImageOptionalHeader<Bits> OptionalHeader;
	};
	DEFINE_ENUM(RelocationType, u16,
		(absolute           , 0 ),
		(high               , 1 ),
		(low                , 2 ),
		(highlow            , 3 ),
		(highadj            , 4 ),
		(machine_specific_5 , 5 ),
		(reserved           , 6 ),
		(machine_specific_7 , 7 ),
		(machine_specific_8 , 8 ),
		(machine_specific_9 , 9 ),
		(dir64              , 10)
	);
	struct RelocationEntry { u16 Offset : 12; RelocationType Type : 4; };
	struct RelocationBlock
	{
		u32 PageAddress;
		u32 BlockSize;
		INLINE u32 count()const noexcept { return (BlockSize - 8) / sizeof(RelocationEntry); }
	};

	//#define IMAGE_SNAP_BY_ORDINAL64(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG64) != 0)
	//#define IMAGE_SNAP_BY_ORDINAL32(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG32) != 0)
	//#define IMAGE_ORDINAL_FLAG64 0x8000000000000000
	//#define IMAGE_ORDINAL_FLAG32 0x80000000
	template<u8 Bits>
	constexpr UnsignedBits<Bits> imageOridinalFlag{[]
	{
		if constexpr (Bits == 64)return 0x8000000000000000;
		else if constexpr (Bits == 32)return 0x80000000;
	}() };
}