#pragma once
#include "../../Util/Concepts/String.h"
#include "../../Util/DataStructures/Span.h"
#include "../../Util/DataStructures/StringView.h"
#include "../../Util/DataStructures/TString.h"
#include "../../Util/EnumDefinition.h"
#include "../../Util/Macro/Lexi.h"
#include "../../Util/Macro/PaddedStruct.h"
#include "../../Util/MsvcAllowNamelessStructUnion.h"
#include "../../Util/SizedInt.h"
#include "../../Util/StringFormat/HexToStream.h"
#include "../../Util/StringImpl.h"
#include "../WinSock2_NoMinMax.h"
//#include "RegistryStructs.h"
struct UnicodeString: StringImpl
{
	u16 Length;
	u16 MaximumLength;
	c16* Buffer;

	INLINE constexpr UnicodeString($StringOfCharSize<2> auto&& string)noexcept :Length{ static_cast<u16>(string.size() * sizeof(c16)) }, MaximumLength{ static_cast<u16>(string.size() * sizeof(c16)) }, Buffer{const_cast<c16*>(string.data())} {}
	INLINE constexpr UnicodeString(u16 len, u16 maxLen, c16* buffer)noexcept :Length{ len }, MaximumLength{ maxLen }, Buffer{ buffer } {}

	INLINE constexpr u16 size()const noexcept { return Length / sizeof(c16); }

	template<class Self>INLINE constexpr LikePtr<Self, c16> data(this Self&& self) noexcept { return self.Buffer; };
	template<class Self>INLINE constexpr LikePtr<Self, c16> begin(this Self&& self) noexcept { return self.Buffer; };
	template<class Self>INLINE constexpr LikePtr<Self, c16> end(this Self&& self) noexcept { return self.begin() + self.size(); };
	template<class Self>INLINE constexpr Like<Self, c16> operator[](this Self&& self, u8 index) noexcept { return self.Buffer[index]; }

	template<class Stream>
	INLINE constexpr friend Stream& operator<<(Stream& os, const UnicodeString& s)noexcept
	{
		os << StringView16{ s };
		return os;
	}
}; //STATIC_ASSERT(sizeof(UnicodeString) == sizeof(UNICODE_STRING));
struct ObjectAttributes
{
	FLAG_(Attribute, u32)
	{
		inherit                         = 0x00000002,
		permanent                       = 0x00000010,
		exclusive                       = 0x00000020,
		case_insensitive                = 0x00000040,
		openif                          = 0x00000080,
		openlink                        = 0x00000100,
		kernel_handle                   = 0x00000200,
		force_access_check              = 0x00000400,
		ignore_impersonated_devicemap   = 0x00000800,
		dont_reparse                    = 0x00001000
	};
	u32 Length;
	HANDLE RootDirectory;
	UnicodeString* ObjectName;
	Attribute Attributes;
	void* SecurityDescriptor;
	void* SecurityQualityOfService;
	INLINE static constexpr ObjectAttributes create(UnicodeString& name, Attribute attributes = {})noexcept
	{
		return ObjectAttributes
		{
			.Length{sizeof(ObjectAttributes)},
			.RootDirectory{},
			.ObjectName{&name},
			.Attributes{attributes},
			.SecurityDescriptor{},
			.SecurityQualityOfService{}
		};
	}
}; //STATIC_ASSERT(sizeof(ObjectAttributes) == sizeof(OBJECT_ATTRIBUTES));
namespace Windows
{
	DEFINE_ENUM(DirectoryNotifyInformationClass, u32,
		(DirectoryNotifyInformation, 1),	//FILE_NOTIFY_INFORMATION
		DirectoryNotifyExtendedInformation,	//FILE_NOTIFY_EXTENDED_INFORMATION
		DirectoryNotifyFullInformation		//FILE_NOTIFY_FULL_INFORMATION // since 22H2
	);
	FLAG(FileAttributeFlag,u32)
	{
		readonly              = 0x00000001,
		hidden                = 0x00000002,
		system                = 0x00000004,
		directory             = 0x00000010,
		archive               = 0x00000020,
		device                = 0x00000040,
		normal                = 0x00000080,
		temporary             = 0x00000100,
		sparse_file           = 0x00000200,
		reparse_point         = 0x00000400,
		compressed            = 0x00000800,
		offline               = 0x00001000,
		not_content_indexed   = 0x00002000,
		encrypted             = 0x00004000,
		integrity_stream      = 0x00008000,
		virtual_              = 0x00010000,
		no_scrub_data         = 0x00020000,
		ea                    = 0x00040000,
		pinned                = 0x00080000,
		unpinned              = 0x00100000,
		recall_on_open        = 0x00040000,
		recall_on_data_access = 0x00400000
	};
	union FileInternalInformation
	{
		u64 IndexNumber;
		struct
		{
			u64 MftRecordIndex : 48;
			u64 SequenceNumber : 16;
		};
	};
	DEFINE_ENUM(FileAction, u32,
		(added					,0x00000001),	//The file was added to the directory.
		(removed				,0x00000002),	//The file was removed from the directory.
		(modified				,0x00000003),	//The file was modified.This can be a change in the time stamp or attributes.
		(renamed_old_name		,0x00000004),	//The file was renamed and this is the old name.
		(renamed_new_name		,0x00000005),	//The file was renamed and this is the new name.
		(added_stream			,0x00000006),	//A named stream was added to the file.
		(removed_stream			,0x00000007),	//A named stream was removed from the file.
		(modified_stream		,0x00000008),	//A named stream was modified.The modification can be a change to the data or attributes of the named stream.
		(removed_by_delete		,0x00000009),	//An object ID was removed because the file the object ID referred to was deleted.This notification is only sent when the directory being monitored is the special directory "$Extend$ObjId:$O:$INDEX_ALLOCATION".
		(id_not_tunnelled		,0x0000000A),	//An attempt to tunnel object ID information to a file being created or renamed failed because the object ID is in use by another file on the same volume.This notification is only sent when the directory being monitored is the special directory "$Extend$ObjId:$O:$INDEX_ALLOCATION".
		(tunnelled_id_collision	,0x0000000B)	//An attempt to tunnel object ID information to a file being renamed failed because the file already has an object ID.This notification is only sent when the directory being monitored is the special directory "$Extend$ObjId:$O:$INDEX_ALLOCATION".
	);
	struct FileNotifyInformation
	{
		u32 NextEntryOffset;
		FileAction Action;
		u32 FileNameLength;
		c16 FileName[1];
		INLINE StringView16 name()const noexcept
		{
			return { &FileName[0], FileNameLength / sizeof(c16)};
		}
	};
	struct FileNotifyExtendedInformation
	{
		u32 NextEntryOffset;
		FileAction Action;
		u64 CreationTime;
		u64 LastModificationTime;
		u64 LastChangeTime;
		u64 LastAccessTime;
		u64 AllocatedLength;
		u64 FileSize;
		FileAttributeFlag FileAttributes;
		union
		{
			u32 ReparsePointTag;
			u32 EaSize;
		};
		FileInternalInformation FileId;
		FileInternalInformation ParentFileId;
		u32 FileNameLength;
		c16 FileName[1];
		INLINE StringView16 name()const noexcept
		{
			return { &FileName[0], FileNameLength / sizeof(c16)};
		}
	};
	struct FileNotifyFullInformation
	{
		u32 NextEntryOffset;
		FileAction Action;
		u64 CreationTime;
		u64 LastModificationTime;
		u64 LastChangeTime;
		u64 LastAccessTime;
		u64 AllocatedLength;
		u64 FileSize;
		FileAttributeFlag FileAttributes;
		union
		{
			u32 ReparsePointTag;
			u32 EaSize;
		};
		FileInternalInformation FileId;
		FileInternalInformation ParentFileId;
		USHORT FileNameLength;
		BYTE FileNameFlags;
		BYTE Reserved;
		c16 FileName[1];
		INLINE StringView16 name()const noexcept
		{
			return { &FileName[0], FileNameLength / sizeof(c16)};
		}
	};
	struct CrossProcessFlags
	{
		u32 ProcessInJob : 1;
		u32 ProcessInitializing : 1;
		u32 ProcessUsingVEH : 1;
		u32 ProcessUsingVCH : 1;
		u32 ProcessUsingFTH : 1;
		u32 ProcessPreviouslyThrottled : 1;
		u32 ProcessCurrentlyThrottled : 1;
		u32 ProcessImagesHotPatched : 1;
		u32 ReservedBits0 : 24;
	}; STATIC_ASSERT(sizeof(CrossProcessFlags) == 4);

	DEFINE_ENUM(HardErrorResponse, u32, 
		ReturnToCaller,
		NotHandled,
		Abort,
		Cancel,
		Ignore,
		No,
		Ok,
		Retry,
		Yes,
		TryAgain,
		Continue
	);

	FLAG(ProcessCreationFlag, u32)
	{
		debug_process                     = 0x00000001,
		debug_only_this_process           = 0x00000002,
		create_suspended                  = 0x00000004,
		detached_process                  = 0x00000008,
		create_new_console                = 0x00000010,
		normal_priority_class             = 0x00000020,
		idle_priority_class               = 0x00000040,
		high_priority_class               = 0x00000080,
		realtime_priority_class           = 0x00000100,
		create_new_process_group          = 0x00000200,
		create_unicode_environment        = 0x00000400,
		create_separate_wow_vdm           = 0x00000800,
		create_shared_wow_vdm             = 0x00001000,
		create_forcedos                   = 0x00002000,
		below_normal_priority_class       = 0x00004000,
		above_normal_priority_class       = 0x00008000,
		inherit_parent_affinity           = 0x00010000,
		inherit_caller_priority           = 0x00020000,    // Deprecated
		create_protected_process          = 0x00040000,
		extended_startupinfo_present      = 0x00080000,
		process_mode_background_begin     = 0x00100000,
		process_mode_background_end       = 0x00200000,
		create_secure_process             = 0x00400000,
		create_breakaway_from_job         = 0x01000000,
		create_preserve_code_authz_level  = 0x02000000,
		create_default_error_mode         = 0x04000000,
		create_no_window                  = 0x08000000,
		profile_user                      = 0x10000000,
		profile_kernel                    = 0x20000000,
		profile_server                    = 0x40000000,
		create_ignore_system_default      = 0x80000000
	};
	FLAG(QueryDirectoryFlag, u32)
	{
		restart_scan						= 0x00000001,
		return_single_entry					= 0x00000002,
		index_specified						= 0x00000004,
		return_on_disk_entries_only			= 0x00000008,
		no_cursor_update_query				= 0x00000010
	};
	FLAG(MessageBoxFlag, u32)
	{
		ok							= 0x00000000L,
		okcancel					= 0x00000001L,
		abortretryignore			= 0x00000002L,
		yesnocancel					= 0x00000003L,
		yesno						= 0x00000004L,
		retrycancel					= 0x00000005L,
		canceltrycontinue			= 0x00000006L,
		iconhand					= 0x00000010L,
		iconquestion				= 0x00000020L,
		iconexclamation				= 0x00000030L,
		iconasterisk				= 0x00000040L,
		usericon					= 0x00000080L,
		//defbutton1				= 0x00000000L,
		defbutton2					= 0x00000100L,
		defbutton3					= 0x00000200L,
		defbutton4					= 0x00000300L,
		//applmodal					= 0x00000000L,
		systemmodal					= 0x00001000L,
		taskmodal					= 0x00002000L,
		help						= 0x00004000L,
		nofocus						= 0x00008000L,
		setforeground				= 0x00010000L,
		default_desktop_only		= 0x00020000L,
		topmost						= 0x00040000L,
		right						= 0x00080000L,
		rtlreading					= 0x00100000L,
		//service_notification_nt3x	= 0x00040000L,
		service_notification		= 0x00200000L,
		//iconwarning				= mb_iconexclamation,
		//iconerror					= mb_iconhand,
		//iconinformation			= mb_iconasterisk,
		//iconstop					= mb_iconhand,
	};
	ENUM(ObjectInformationClass,u32)
	{
		basic, // q: OBJECT_BASIC_INFORMATION
		name, // q: OBJECT_NAME_INFORMATION
		type, // q: OBJECT_TYPE_INFORMATION
		types, // q: OBJECT_TYPES_INFORMATION
		handle_flag, // qs: OBJECT_HANDLE_FLAG_INFORMATION
		session, // s: void // change object session // (requires SeTcbPrivilege)
		session_object, // s: void // change object session // (requires SeTcbPrivilege)
	};
	FLAG(MemExecuteOption, u32)
	{
		disable = 0x1,
		enable = 0x2,
		disable_thunk_emulation = 0x4,
		permanent = 0x8,
		execute_dispatch_enable = 0x10,
		image_dispatch_enable = 0x20
	};
	FLAG(RtlUserProcessParameters, u32)
	{
		normalized          = 0x01,
		profile_user        = 0x02,
		profile_kernel      = 0x04,
		profile_server      = 0x08,
		unknown             = 0x10,
		reserve_1mb         = 0x20,
		reserve_16mb        = 0x40,
		case_sensitive      = 0x80,
		disable_heap_checks = 0x100,
		process_or_1        = 0x200,
		process_or_2        = 0x400,
		private_dll_path    = 0x1000,
		local_dll_path      = 0x2000,
		image_key_missing   = 0x4000,
		nx                  = 0x20000
	};
	DEFINE_ENUM(ThreadPriority, u32,
		(above_normal, 1),
		(below_normal, (u32)-1),
		(highest, 2),
		(idle, (u32)-16),
		(lowest, (u32)-2),
		(normal, 0),
		(time_critical, 16)
	)
	DEFINE_ENUM(ProcessPriority, u8,
		(unknown,0),
		(idle,1),
		(normal,2),
		(high,3),
		(realtime,4),
		(below_normal,5),
		(above_normal,6)
	)
	DEFINE_ENUM(WindowDisplayAffinity, u32,
		(none				, 0x00000000),
		(monitor			, 0x00000001),
		(excludefromcapture	, 0x00000011)
	)
	DEFINE_ENUM(Band, u32,
		(default_ , 0),
		(desktop , 1),
		(uiaccess , 2),
		(immersive_ihm , 3),
		(immersive_notification , 4),
		(immersive_appchrome , 5),
		(immersive_mogo , 6),
		(immersive_edgy , 7),
		(immersive_inactivemobody , 8),
		(immersive_inactivedock , 9),
		(immersive_activemobody , 10),
		(immersive_activedock , 11),
		(immersive_background , 12),
		(immersive_search , 13),
		(genuine_windows , 14),
		(immersive_restricted , 15),
		(system_tools , 16),

		//windows 10+
		(lock , 17),
		(abovelock_ux , 18)
	)
	ENUM(FS_INFORMATION_CLASS, u32)
	{
		FileFsVolumeInformation = 1,
		FileFsLabelInformation,
		FileFsSizeInformation,
		FileFsDeviceInformation,
		FileFsAttributeInformation,
		FileFsControlInformation,
		FileFsFullSizeInformation,
		FileFsObjectIdInformation,
		FileFsDriverPathInformation,
		FileFsVolumeFlagsInformation,
		FileFsSectorSizeInformation, // since WIN8
		FileFsDataCopyInformation,
		FileFsMetadataSizeInformation, // since THRESHOLD
	};
	FLAG(TokenAccessFlag, u16)
	{
		assign_primary		= 0x0001,
		duplicate			= 0x0002,
		impersonate			= 0x0004,
		query				= 0x0008,
		query_source		= 0x0010,
		adjust_privileges	= 0x0020,
		adjust_groups		= 0x0040,
		adjust_default		= 0x0080,
		adjust_sessionid	= 0x0100
	};
	FLAG(MemAllocationFlag, u32)
	{
		commit                     = 0x00001000,  
		reserve                    = 0x00002000,  
		replace_placeholder        = 0x00004000,  
		reserve_placeholder        = 0x00040000,  
		reset                      = 0x00080000,  
		top_down                   = 0x00100000,  
		write_watch                = 0x00200000,  
		physical                   = 0x00400000,  
		rotate                     = 0x00800000,  
		different_image_base_ok    = 0x00800000,  
		reset_undo                 = 0x01000000,  
		large_pages                = 0x20000000,  
		pages_4mb                  = 0x80000000
	};
	FLAG(MemFreeFlag, u32)
	{
		coalesce_placeholders      = 0x00000001,
		preserve_placeholder       = 0x00000002,
		decommit                   = 0x00004000,
		release                    = 0x00008000,
		free                       = 0x00010000
	};
	FLAG(PageStateFlag, u32)
	{
		commit                     = 0x00001000,  
		reserve                    = 0x00002000, 
		free                       = 0x00010000 
	};
	FLAG(PageTypeFlag, u32)
	{
		private_				   = 0x00020000,
		mapped					   = 0x00040000,
		image					   = 0x01000000
	};
	FLAG(PageProtectFlag, u32)
	{
		noaccess					= 0x01    	,
		readonly					= 0x02    	,
		readwrite					= 0x04    	,
		writecopy					= 0x08    	,
		execute						= 0x10    	,
		execute_read				= 0x20    	,
		execute_readwrite			= 0x40    	,
		execute_writecopy			= 0x80    	,
		guard						= 0x100    	,
		nocache						= 0x200    	,
		writecombine				= 0x400    	,
		graphics_noaccess           = 0x0800    ,
		graphics_readonly           = 0x1000    ,
		graphics_readwrite          = 0x2000    ,
		graphics_execute            = 0x4000    ,
		graphics_execute_read       = 0x8000    ,
		graphics_execute_readwrite	= 0x10000   ,
		graphics_coherent			= 0x20000   ,
		graphics_nocache			= 0x40000   ,
		enclave_mask				= 0x10000000,  
		enclave_unvalidated			= 0x20000000,
		targets_invalid				= 0x40000000,
		targets_no_update			= 0x40000000,
		revert_to_file_map			= 0x80000000,
		enclave_thread_control		= 0x80000000
	};
	FLAG(ThreadFlag, u32)//THREAD_CREATE_FLAGS_
	{
		none                  = 0x00000000,
		create_suspended      = 0x00000001,// NtCreateUserProcess & NtCreateThreadEx: create the thread in a suspended state instead of allowing it to execute immediately.
		skip_thread_attach    = 0x00000002,// NtCreateThreadEx only: the thread should skip calling loaded modules with DLL_THREAD_ATTACH reason.
		hide_from_debugger    = 0x00000004,// NtCreateThreadEx only: suppress generation of debug events on the thread.
		loader_worker         = 0x00000010,// NtCreateThreadEx only: This flags indicates that the system should set the LoaderWorker flag in TEB. Used internally.
		skip_loader_init      = 0x00000020,// NtCreateThreadEx only: This flags indicates that the system should set the SkipLoaderInit flag in TEB. Used internally.
		bypass_process_freeze = 0x00000040,// NtCreateThreadEx only: the thread should not be suspended when the system suspends or freezes the process.
		initial_thread        = 0x00000080 // ???
	};
	ENUM(SUPERFETCH_INFORMATION_CLASS, u32)
	{
		SuperfetchRetrieveTrace = 1, // q: CHAR[]
		SuperfetchSystemParameters, // q: PF_SYSTEM_SUPERFETCH_PARAMETERS
		SuperfetchLogEvent,
		SuperfetchGenerateTrace,
		SuperfetchPrefetch,
		SuperfetchPfnQuery, // q: PF_PFN_PRIO_REQUEST
		SuperfetchPfnSetPriority,
		SuperfetchPrivSourceQuery, // q: PF_PRIVSOURCE_QUERY_REQUEST
		SuperfetchSequenceNumberQuery, // q: u32
		SuperfetchScenarioPhase, // 10
		SuperfetchWorkerPriority,
		SuperfetchScenarioQuery, // q: PF_SCENARIO_PHASE_INFO
		SuperfetchScenarioPrefetch,
		SuperfetchRobustnessControl,
		SuperfetchTimeControl,
		SuperfetchMemoryListQuery, // q: PF_MEMORY_LIST_INFO
		SuperfetchMemoryRangesQuery, // q: PF_PHYSICAL_MEMORY_RANGE_INFO
		SuperfetchTracingControl,
		SuperfetchTrimWhileAgingControl,
		SuperfetchRepurposedByPrefetch, // q: PF_REPURPOSED_BY_PREFETCH_INFO // rev
	};

	struct ProcessPriorityClass//PROCESS_PRIORITY_CLASS
	{
		u8 Foreground;
		ProcessPriority PriorityClass;
	};

	struct ObjectDirectoryInformation//OBJECT_DIRECTORY_INFORMATION
	{
		UnicodeString Name;
		UnicodeString TypeName;
	};

	struct SUPERFETCH_INFORMATION// Buffer for NtQuery/SetInformationSystem for the Superfetch Class
	{
		u32 Version;
		u32 Magic;
		SUPERFETCH_INFORMATION_CLASS InfoClass;
		void* Data;
		u32 Length;
	};

	struct PF_PHYSICAL_MEMORY_RANGE
	{
		u64 BasePfn;
		u64 PageCount;
	};

	struct PF_MEMORY_RANGE_INFO_V1
	{
		u32 version;
		u32 ranges_count;
		PF_PHYSICAL_MEMORY_RANGE ranges[1];
	};

	struct PHYSICAL_MEMORY_RUN
	{
		u64 BasePage;
		u64 PageCount;
	};

	struct alignas(8) PF_MEMORY_RANGE_INFO_V2
	{
		u32 version;
		u32 flags;
		u32 ranges_count;
		PHYSICAL_MEMORY_RUN ranges[1];
	};

	DEFINE_ENUM(PS_CREATE_STATE, u32,
		PsCreateInitialState,
		PsCreateFailOnFileOpen,
		PsCreateFailOnSectionCreate,
		PsCreateFailExeFormat,
		PsCreateFailMachineMismatch,
		PsCreateFailExeName, // Debugger specified
		PsCreateSuccess
	)

	struct PS_CREATE_INFO
	{
		usize Size;
		PS_CREATE_STATE State;
		union 
		{
			// PsCreateInitialState
			struct 
			{
				union 
				{
					u32 InitFlags;
					struct 
					{
						u8 WriteOutputOnExit : 1;
						u8 DetectManifest : 1;
						u8 IFEOSkipDebugger : 1;
						u8 IFEODoNotPropagateKeyState : 1;
						u8 SpareBits1 : 4;
						u8 SpareBits2 : 8;
						u16 ProhibitedImageCharacteristics : 16;
					};
				};
				ACCESS_MASK AdditionalFileAccess;
			} InitState;

			// PsCreateFailOnSectionCreate
			struct 
			{
				HANDLE FileHandle;
			} FailSection;

			// PsCreateFailExeFormat
			struct 
			{
				u16 DllCharacteristics;
			} ExeFormat;

			// PsCreateFailExeName
			struct 
			{
				HANDLE IFEOKey;
			} ExeName;

			// PsCreateSuccess
			struct 
			{
				union 
				{
					u32 OutputFlags;
					struct 
					{
						u8 ProtectedProcess : 1;
						u8 AddressSpaceOverride : 1;
						u8 DevOverrideEnabled : 1; // from Image File Execution Options
						u8 ManifestDetected : 1;
						u8 ProtectedProcessLight : 1;
						u8 SpareBits1 : 3;
						u8 SpareBits2 : 8;
						u16 SpareBits3 : 16;
					};
				};
				HANDLE FileHandle;
				HANDLE SectionHandle;
				u64 UserProcessParametersNative;
				u32 UserProcessParametersWow64;
				u32 CurrentParameterFlags;
				u64 PebAddressNative;
				u32 PebAddressWow64;
				u64 ManifestAddress;
				u32 ManifestSize;
			} SuccessState;
		};
	};

	struct GetClipboardDataData
	{
		u32 uFmtRet;
		i32 isGlobalHandle;
		union
		{
			HANDLE hLocale;
			HANDLE hPalette;
		};
	};

	struct SetClipboardDataData
	{
		u32 isGlobalHandle;
		u32 value1;
	};

	DEFINE_ENUM(ShutdownAction,u8,
		no_reboot,
		reboot,
		power_off
	)

	DEFINE_ENUM(PS_PROTECTED_SIGNER,u8,
		None,
		Authenticode,
		CodeGen,
		Antimalware,
		Lsa,
		Windows,
		WinTcb,
		WinSystem,
		App
	)

	DEFINE_ENUM(PS_PROTECTED_TYPE,u8,
		None,
		ProtectedLight,
		Protected
	)

	struct PS_PROTECTION
	{
		PS_PROTECTED_TYPE Type : 3;
		u8 Audit : 1;
		PS_PROTECTED_SIGNER Signer : 4;
		DEFINE_LEXI(Type,Audit,Signer)
	}; STATIC_ASSERT(1 == sizeof(PS_PROTECTION));

	template<class Stream>
	constexpr Stream& operator<<(Stream& os, PS_PROTECTION protection)noexcept
	{
		os << protection.Type << ',' << protection.Audit << ',' << protection.Signer;
		return os;
	}

	ENUM(WindowQueryID,u8)
	{
		UNIQUE_PROCESS_ID = 0,
		UNIQUE_THREAD_ID  = 2,
		//ACTIVE            = 2,//ProbablyWrong
		//FOCUS             = 3,
		//ISHUNG            = 4,
		//REAL_ID           = 5,
		//FOREGROUND        = 6,
		//DEFAULT_IME       = 7,
		//DEFAULT_ICONTEXT  = 8,
		//ACTIVE_IME        = 9
	};
	ENUM(TokenInformationClass,u32)
	{
		user = 1,
		groups,
		privileges,
		owner,
		primaryGroup,
		defaultDacl,
		source,
		type,
		impersonationLevel,
		statistics,
		restrictedSids,
		sessionId,
		groupsAndPrivileges,
		sessionReference,
		sandBoxInert,
		auditPolicy,
		origin,
		elevationType,
		linkedToken,
		elevation,
		hasRestrictions,
		accessInformation,
		virtualizationAllowed,
		virtualizationEnabled,
		integrityLevel,
		uIAccess,
		mandatoryPolicy,
		logonSid,
		isAppContainer,
		capabilities,
		appContainerSid,
		appContainerNumber,
		userClaimAttributes,
		deviceClaimAttributes,
		restrictedUserClaimAttributes,
		restrictedDeviceClaimAttributes,
		deviceGroups,
		restrictedDeviceGroups,
		securityAttributes,
		isRestricted,
		processTrustLevel,
		privateNameSpace,
		singletonAttributes,
		bnoIsolation,
		childProcessFlags,
		isLessPrivilegedAppContainer,
		isSandboxed,
		originatingProcessTrustLevel
	};

	ENUM(SECTION_INHERIT,u32)
	{
		ViewShare = 1,	//The view will be mapped into any child processes that are created in the future.
		ViewUnmap = 2	//The view will not be mapped into child processes. Drivers should typically specify ViewUnmap for this parameter.
	};

	ENUM(KWAIT_REASON,u32)
	{
		Executive,
		FreePage,
		PageIn,
		PoolAllocation,
		DelayExecution,
		Suspended,
		UserRequest,
		WrExecutive,
		WrFreePage,
		WrPageIn,
		WrPoolAllocation,
		WrDelayExecution,
		WrSuspended,
		WrUserRequest,
		WrEventPair,
		WrQueue,
		WrLpcReceive,
		WrLpcReply,
		WrVirtualMemory,
		WrPageOut,
		WrRendezvous,
		WrKeyedEvent,
		WrTerminated,
		WrProcessInSwap,
		WrCpuRateControl,
		WrCalloutStack,
		WrKernel,
		WrResource,
		WrPushLock,
		WrMutex,
		WrQuantumEnd,
		WrDispatchInt,
		WrPreempted,
		WrYieldExecution,
		WrFastMutex,
		WrGuardedMutex,
		WrRundown,
		WrAlertByThreadId,
		WrDeferredPreempt
	};

	ENUM(EVENT_TYPE,u32)
	{
		NotificationEvent,
		SynchronizationEvent
	};

	ENUM(TIMER_TYPE, u32)
	{
		NotificationTimer,
		SynchronizationTimer
	};

	ENUM(WAIT_TYPE, u32)
	{
		WaitAll,
		WaitAny,
		WaitNotification
	};

	ENUM(TIMER_SET_INFORMATION_CLASS, u32)
	{
		TimerSetCoalescableTimer
	};

	ENUM(ProcessInfoClass, u32)
	{
		ProcessBasicInformation, // q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
		ProcessQuotaLimits, // qs: QUOTA_LIMITS, QUOTA_LIMITS_EX
		ProcessIoCounters, // q: IO_COUNTERS
		ProcessVmCounters, // q: VM_COUNTERS, VM_COUNTERS_EX, VM_COUNTERS_EX2
		ProcessTimes, // q: KERNEL_USER_TIMES
		ProcessBasePriority, // s: KPRIORITY
		ProcessRaisePriority, // s: u32
		ProcessDebugPort, // q: HANDLE
		ProcessExceptionPort, // s: PROCESS_EXCEPTION_PORT
		ProcessAccessToken, // s: PROCESS_ACCESS_TOKEN
		ProcessLdtInformation, // qs: PROCESS_LDT_INFORMATION // 10
		ProcessLdtSize, // s: PROCESS_LDT_SIZE
		ProcessDefaultHardErrorMode, // qs: u32
		ProcessIoPortHandlers, // (kernel-mode only) // PROCESS_IO_PORT_HANDLER_INFORMATION
		ProcessPooledUsageAndLimits, // q: POOLED_USAGE_AND_LIMITS
		ProcessWorkingSetWatch, // q: PROCESS_WS_WATCH_INFORMATION[]; s: void
		ProcessUserModeIOPL, // qs: u32 (requires SeTcbPrivilege)
		ProcessEnableAlignmentFaultFixup, // s: BOOLEAN
		ProcessPriorityClass, // qs: PROCESS_PRIORITY_CLASS
		ProcessWx86Information, // qs: u32 (requires SeTcbPrivilege) (VdmAllowed)
		ProcessHandleCount, // q: u32, PROCESS_HANDLE_INFORMATION // 20
		ProcessAffinityMask, // s: KAFFINITY
		ProcessPriorityBoost, // qs: u32
		ProcessDeviceMap, // qs: PROCESS_DEVICEMAP_INFORMATION, PROCESS_DEVICEMAP_INFORMATION_EX
		ProcessSessionInformation, // q: PROCESS_SESSION_INFORMATION
		ProcessForegroundInformation, // s: PROCESS_FOREGROUND_BACKGROUND
		ProcessWow64Information, // q: ULONG_PTR
		ProcessImageFileName, // q: UnicodeString
		ProcessLUIDDeviceMapsEnabled, // q: u32
		ProcessBreakOnTermination, // qs: u32
		ProcessDebugObjectHandle, // q: HANDLE // 30
		ProcessDebugFlags, // qs: u32
		ProcessHandleTracing, // q: PROCESS_HANDLE_TRACING_QUERY; s: size 0 disables, otherwise enables
		ProcessIoPriority, // qs: IO_PRIORITY_HINT
		ProcessExecuteFlags, // qs: u32
		ProcessResourceManagement, // ProcessTlsInformation // PROCESS_TLS_INFORMATION
		ProcessCookie, // q: u32
		ProcessImageInformation, // q: SECTION_IMAGE_INFORMATION
		ProcessCycleTime, // q: PROCESS_CYCLE_TIME_INFORMATION // since VISTA
		ProcessPagePriority, // q: PAGE_PRIORITY_INFORMATION
		ProcessInstrumentationCallback, // qs: PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION // 40
		ProcessThreadStackAllocation, // s: PROCESS_STACK_ALLOCATION_INFORMATION, PROCESS_STACK_ALLOCATION_INFORMATION_EX
		ProcessWorkingSetWatchEx, // q: PROCESS_WS_WATCH_INFORMATION_EX[]
		ProcessImageFileNameWin32, // q: UnicodeString
		ProcessImageFileMapping, // q: HANDLE (input)
		ProcessAffinityUpdateMode, // qs: PROCESS_AFFINITY_UPDATE_MODE
		ProcessMemoryAllocationMode, // qs: PROCESS_MEMORY_ALLOCATION_MODE
		ProcessGroupInformation, // q: u16[]
		ProcessTokenVirtualizationEnabled, // s: u32
		ProcessConsoleHostProcess, // q: ULONG_PTR // ProcessOwnerInformation
		ProcessWindowInformation, // q: PROCESS_WINDOW_INFORMATION // 50
		ProcessHandleInformation, // q: PROCESS_HANDLE_SNAPSHOT_INFORMATION // since WIN8
		ProcessMitigationPolicy, // s: PROCESS_MITIGATION_POLICY_INFORMATION
		ProcessDynamicFunctionTableInformation,
		ProcessHandleCheckingMode, // qs: u32; s: 0 disables, otherwise enables
		ProcessKeepAliveCount, // q: PROCESS_KEEPALIVE_COUNT_INFORMATION
		ProcessRevokeFileHandles, // s: PROCESS_REVOKE_FILE_HANDLES_INFORMATION
		ProcessWorkingSetControl, // s: PROCESS_WORKING_SET_CONTROL
		ProcessHandleTable, // q: u32[] // since WINBLUE
		ProcessCheckStackExtentsMode,
		ProcessCommandLineInformation, // q: UnicodeString // 60
		ProcessProtectionInformation, // q: PS_PROTECTION
		ProcessMemoryExhaustion, // PROCESS_MEMORY_EXHAUSTION_INFO // since THRESHOLD
		ProcessFaultInformation, // PROCESS_FAULT_INFORMATION
		ProcessTelemetryIdInformation, // PROCESS_TELEMETRY_ID_INFORMATION
		ProcessCommitReleaseInformation, // PROCESS_COMMIT_RELEASE_INFORMATION
		ProcessDefaultCpuSetsInformation,
		ProcessAllowedCpuSetsInformation,
		ProcessSubsystemProcess,
		ProcessJobMemoryInformation, // PROCESS_JOB_MEMORY_INFO
		ProcessInPrivate, // since THRESHOLD2 // 70
		ProcessRaiseUMExceptionOnInvalidHandleClose, // qs: u32; s: 0 disables, otherwise enables
		ProcessIumChallengeResponse,
		ProcessChildProcessInformation, // PROCESS_CHILD_PROCESS_INFORMATION
		ProcessHighGraphicsPriorityInformation,
		ProcessSubsystemInformation, // q: SUBSYSTEM_INFORMATION_TYPE // since REDSTONE2
		ProcessEnergyValues, // PROCESS_ENERGY_VALUES, PROCESS_EXTENDED_ENERGY_VALUES
		ProcessActivityThrottleState, // PROCESS_ACTIVITY_THROTTLE_STATE
		ProcessActivityThrottlePolicy, // PROCESS_ACTIVITY_THROTTLE_POLICY
		ProcessWin32kSyscallFilterInformation,
		ProcessDisableSystemAllowedCpuSets, // 80
		ProcessWakeInformation, // PROCESS_WAKE_INFORMATION
		ProcessEnergyTrackingState, // PROCESS_ENERGY_TRACKING_STATE
		ProcessManageWritesToExecutableMemory, // MANAGE_WRITES_TO_EXECUTABLE_MEMORY // since REDSTONE3
		ProcessCaptureTrustletLiveDump,
		ProcessTelemetryCoverage,
		ProcessEnclaveInformation,
		ProcessEnableReadWriteVmLogging, // PROCESS_READWRITEVM_LOGGING_INFORMATION
		ProcessUptimeInformation, // PROCESS_UPTIME_INFORMATION
		ProcessImageSection, // q: HANDLE
		ProcessDebugAuthInformation, // since REDSTONE4 // 90
		ProcessSystemResourceManagement, // PROCESS_SYSTEM_RESOURCE_MANAGEMENT
		ProcessSequenceNumber, // q: u64
		ProcessLoaderDetour, // since REDSTONE5
		ProcessSecurityDomainInformation, // PROCESS_SECURITY_DOMAIN_INFORMATION
		ProcessCombineSecurityDomainsInformation, // PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION
		ProcessEnableLogging, // PROCESS_LOGGING_INFORMATION
		ProcessLeapSecondInformation, // PROCESS_LEAP_SECOND_INFORMATION
		ProcessFiberShadowStackAllocation, // PROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION // since 19H1
		ProcessFreeFiberShadowStackAllocation, // PROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION
		ProcessAltSystemCallInformation, // qs: BOOLEAN (kernel-mode only) // since 20H1 // 100
		ProcessDynamicEHContinuationTargets // PROCESS_DYNAMIC_EH_CONTINUATION_TARGETS_INFORMATION
	};

	ENUM(ThreadInfoClass, u32)
	{
		ThreadBasicInformation, // q: THREAD_BASIC_INFORMATION
		ThreadTimes, // q: KERNEL_USER_TIMES
		ThreadPriority, // s: KPRIORITY
		ThreadBasePriority, // s: i32
		ThreadAffinityMask, // s: KAFFINITY
		ThreadImpersonationToken, // s: HANDLE
		ThreadDescriptorTableEntry, // q: DESCRIPTOR_TABLE_ENTRY (or WOW64_DESCRIPTOR_TABLE_ENTRY)
		ThreadEnableAlignmentFaultFixup, // s: BOOLEAN
		ThreadEventPair,
		ThreadQuerySetWin32StartAddress, // q: PVOID
		ThreadZeroTlsCell, // 10
		ThreadPerformanceCount, // q: u64
		ThreadAmILastThread, // q: u32
		ThreadIdealProcessor, // s: u32
		ThreadPriorityBoost, // qs: u32
		ThreadSetTlsArrayAddress,
		ThreadIsIoPending, // q: u32
		ThreadHideFromDebugger, // s: void
		ThreadBreakOnTermination, // qs: u32
		ThreadSwitchLegacyState,
		ThreadIsTerminated, // q: u32 // 20
		ThreadLastSystemCall, // q: THREAD_LAST_SYSCALL_INFORMATION
		ThreadIoPriority, // qs: IO_PRIORITY_HINT
		ThreadCycleTime, // q: THREAD_CYCLE_TIME_INFORMATION
		ThreadPagePriority, // q: u32
		ThreadActualBasePriority,
		ThreadTebInformation, // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_SET_CONTEXT)
		ThreadCSwitchMon,
		ThreadCSwitchPmu,
		ThreadWow64Context, // q: WOW64_CONTEXT
		ThreadGroupInformation, // q: GROUP_AFFINITY // 30
		ThreadUmsInformation, // q: THREAD_UMS_INFORMATION
		ThreadCounterProfiling,
		ThreadIdealProcessorEx, // q: PROCESSOR_NUMBER
		ThreadCpuAccountingInformation, // since WIN8
		ThreadSuspendCount, // since WINBLUE
		ThreadHeterogeneousCpuPolicy, // q: KHETERO_CPU_POLICY // since THRESHOLD
		ThreadContainerId, // q: GUID
		ThreadNameInformation, // qs: THREAD_NAME_INFORMATION
		ThreadSelectedCpuSets,
		ThreadSystemThreadInformation, // q: SYSTEM_THREAD_INFORMATION // 40
		ThreadActualGroupAffinity, // since THRESHOLD2
		ThreadDynamicCodePolicyInfo,
		ThreadExplicitCaseSensitivity, // qs: u32; s: 0 disables, otherwise enables
		ThreadWorkOnBehalfTicket,
		ThreadSubsystemInformation, // q: SUBSYSTEM_INFORMATION_TYPE // since REDSTONE2
		ThreadDbgkWerReportActive,
		ThreadAttachContainer,
		ThreadManageWritesToExecutableMemory, // MANAGE_WRITES_TO_EXECUTABLE_MEMORY // since REDSTONE3
		ThreadPowerThrottlingState, // THREAD_POWER_THROTTLING_STATE
		ThreadWorkloadClass // THREAD_WORKLOAD_CLASS // since REDSTONE5 // 50
	};

	struct AnsiiString
	{
		u16 Length;
		u16 MaximumLength;
		c8* Buffer;
		constexpr u16 size()const noexcept { return Length / sizeof(c8); }
		constexpr const c8* data()const noexcept { return Buffer; }
		constexpr const c8& operator[](usize index)const noexcept { return Buffer[index]; }
	};

	struct ListEntry
	{
		ListEntry* Flink;
		ListEntry* Blink;
	};

	struct PebLdrData
	{
		u32 Length;
		u32 Initialized;
		void* SsHandle;
		ListEntry InLoadOrderModuleList;
		ListEntry InMemoryOrderModuleList;
		ListEntry InInitializationOrderModuleList;
		usize EntryInProgress;
		u32 ShutdownInProgress;
		usize ShutdownThreadId;
	};

	struct RtlBalancedNode
	{
		union
		{
			struct RtlBalancedNode* Children[2];                             //0x0
			struct
			{
				struct RtlBalancedNode* Left;                                //0x0
				struct RtlBalancedNode* Right;                               //0x8
			};
		};
		union
		{
			struct
			{
				u8 Red : 1;                                                    //0x10
				u8 Balance : 2;                                                //0x10
			};
			usize ParentValue;                                              //0x10
		};
	};

	DEFINE_ENUM(LdrHotPatchState, u32,
		(BaseImage, 0),
		(NotApplied, 1),
		(AppliedReverse, 2),
		(AppliedForward, 3),
		(FailedToPatch, 4),
		(StateMax, 5)
	);

	DEFINE_ENUM(LdrDllLoadReason, u32,
		(staticdependency, 0),
		(staticforwarderdependency, 1),
		(dynamicforwarderdependency, 2),
		(delayloaddependency, 3),
		(dynamicload, 4),
		(asimageload, 5),
		(asdataload, 6),
		(enclaveprimary, 7),
		(enclavedependency, 8),
		(patchimage, 9),
		(unknown, u32(-1))
	);

	struct LdrDataTableEntry
	{
		ListEntry InLoadOrderLinks;
		ListEntry InMemoryOrderLinks;
		ListEntry InInitializationOrderLinks;
		void* DllBase;
		void* EntryPoint;
		u32 SizeOfImage;
		UnicodeString FullDllName;
		UnicodeString BaseDllName;
		union
		{
			u8 FlagGroup[4];
			u32 Flags;
			struct
			{
				u32 PackagedBinary : 1;
				u32 MarkedForRemoval : 1;
				u32 ImageDll : 1;
				u32 LoadNotificationsSent : 1;
				u32 TelemetryEntryProcessed : 1;
				u32 ProcessStaticImport : 1;
				u32 InLegacyLists : 1;
				u32 InIndexes : 1;
				u32 ShimDll : 1;
				u32 InExceptionTable : 1;
				u32 ReservedFlags1 : 2;
				u32 LoadInProgress : 1;
				u32 LoadConfigProcessed : 1;
				u32 EntryProcessed : 1;
				u32 ProtectDelayLoad : 1;
				u32 ReservedFlags3 : 2;
				u32 DontCallForThreads : 1;
				u32 ProcessAttachCalled : 1;
				u32 ProcessAttachFailed : 1;
				u32 CorDeferredValidate : 1;
				u32 CorImage : 1;
				u32 DontRelocate : 1;
				u32 CorILOnly : 1;
				u32 ChpeImage : 1;
				u32 ChpeEmulatorImage : 1;
				u32 ReservedFlags5 : 1;
				u32 Redirected : 1;
				u32 ReservedFlags6 : 2;
				u32 CompatDatabaseProcessed : 1;
			};
		};
		u16 ObsoleteLoadCount;
		u16 TlsIndex;
		ListEntry HashLinks;
		u32 TimeDateStamp;
		struct ACTIVATION_CONTEXT* EntryPointActivationContext;
		void* Lock;
		struct LDR_DDAG_NODE* DdagNode;
		ListEntry NodeModuleLink;
		struct LDRP_LOAD_CONTEXT* LoadContext;
		void* ParentDllBase;
		void* SwitchBackContext;
		RtlBalancedNode BaseAddressIndexNode;
		RtlBalancedNode MappingInfoIndexNode;
		usize OriginalBase;
		u64 LoadTime;
		u32 BaseNameHashValue;
		LdrDllLoadReason LoadReason;
		u32 ImplicitPathOptions;
		u32 ReferenceCount;
		u32 DependentLoadFlags;
		u8 SigningLevel;
		u32 CheckSum;
		void* ActivePatchImageBase;
		LdrHotPatchState HotPatchState;

		LdrDataTableEntry() = delete;

		INLINE StringView16 name()const noexcept { return BaseDllName; }
		INLINE StringView16 path()const noexcept { return FullDllName; }
		INLINE LdrDataTableEntry* pNext()const noexcept{ return reinterpret_cast<LdrDataTableEntry*>(InLoadOrderLinks.Flink); }
		INLINE LdrDataTableEntry* pLast()const noexcept{ return reinterpret_cast<LdrDataTableEntry*>(InLoadOrderLinks.Blink); }

		template<class Stream>
		friend Stream& operator<<(Stream& os, const LdrDataTableEntry& e)noexcept
		{
			os << TS("Base = ") << hex(e.DllBase);
			os << TS(", Size = ") << hex(e.SizeOfImage);
			os << TS(", Entry = ") << hex(e.EntryPoint);
			os << TS(", Reason = ") << e.LoadReason;
			//os << TS(", Name = ") << e.name();
			os << TS(", Path = ") << e.path();
			return os;
		}
	};

	ENUM(SYSTEM_FIRMWARE_TABLE_ACTION, u32)
	{
		SystemFirmwareTable_Enumerate,
		SystemFirmwareTable_Get
	};

	struct SYSTEM_FIRMWARE_TABLE_INFORMATION
	{
		u32 ProviderSignature;
		SYSTEM_FIRMWARE_TABLE_ACTION Action;
		u32 TableID;
		u32 TableBufferLength;
		u8 TableBuffer[1];
	};

	ENUM(SystemInformationClass, u32)
	{
		SystemBasicInformation, // q: SYSTEM_BASIC_INFORMATION
		SystemProcessorInformation, // q: SYSTEM_PROCESSOR_INFORMATION
		SystemPerformanceInformation, // q: SYSTEM_PERFORMANCE_INFORMATION
		SystemTimeOfDayInformation, // q: SYSTEM_TIMEOFDAY_INFORMATION
		SystemPathInformation, // not implemented
		SystemProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
		SystemCallCountInformation, // q: SYSTEM_CALL_COUNT_INFORMATION
		SystemDeviceInformation, // q: SYSTEM_DEVICE_INFORMATION
		SystemProcessorPerformanceInformation, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION (EX in: u16 ProcessorGroup)
		SystemFlagsInformation, // q: SYSTEM_FLAGS_INFORMATION
		SystemCallTimeInformation, // not implemented // SYSTEM_CALL_TIME_INFORMATION // 10
		SystemModuleInformation, // q: RTL_PROCESS_MODULES
		SystemLocksInformation, // q: RTL_PROCESS_LOCKS
		SystemStackTraceInformation, // q: RTL_PROCESS_BACKTRACES
		SystemPagedPoolInformation, // not implemented
		SystemNonPagedPoolInformation, // not implemented
		SystemHandleInformation, // q: SYSTEM_HANDLE_INFORMATION
		SystemObjectInformation, // q: SYSTEM_OBJECTTYPE_INFORMATION mixed with SYSTEM_OBJECT_INFORMATION
		SystemPageFileInformation, // q: SYSTEM_PAGEFILE_INFORMATION
		SystemVdmInstemulInformation, // q: SYSTEM_VDM_INSTEMUL_INFO
		SystemVdmBopInformation, // not implemented // 20
		SystemFileCacheInformation, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemCache)
		SystemPoolTagInformation, // q: SYSTEM_POOLTAG_INFORMATION
		SystemInterruptInformation, // q: SYSTEM_INTERRUPT_INFORMATION (EX in: u16 ProcessorGroup)
		SystemDpcBehaviorInformation, // q: SYSTEM_DPC_BEHAVIOR_INFORMATION; s: SYSTEM_DPC_BEHAVIOR_INFORMATION (requires SeLoadDriverPrivilege)
		SystemFullMemoryInformation, // not implemented // SYSTEM_MEMORY_USAGE_INFORMATION
		SystemLoadGdiDriverInformation, // s (kernel-mode only)
		SystemUnloadGdiDriverInformation, // s (kernel-mode only)
		SystemTimeAdjustmentInformation, // q: SYSTEM_QUERY_TIME_ADJUST_INFORMATION; s: SYSTEM_SET_TIME_ADJUST_INFORMATION (requires SeSystemtimePrivilege)
		SystemSummaryMemoryInformation, // not implemented // SYSTEM_MEMORY_USAGE_INFORMATION
		SystemMirrorMemoryInformation, // s (requires license value "Kernel-MemoryMirroringSupported") (requires SeShutdownPrivilege) // 30
		SystemPerformanceTraceInformation, // q; s: (type depends on EVENT_TRACE_INFORMATION_CLASS)
		SystemObsolete0, // not implemented
		SystemExceptionInformation, // q: SYSTEM_EXCEPTION_INFORMATION
		SystemCrashDumpStateInformation, // s: SYSTEM_CRASH_DUMP_STATE_INFORMATION (requires SeDebugPrivilege)
		SystemKernelDebuggerInformation, // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION
		SystemContextSwitchInformation, // q: SYSTEM_CONTEXT_SWITCH_INFORMATION
		SystemRegistryQuotaInformation, // q: SYSTEM_REGISTRY_QUOTA_INFORMATION; s (requires SeIncreaseQuotaPrivilege)
		SystemExtendServiceTableInformation, // s (requires SeLoadDriverPrivilege) // loads win32k only
		SystemPrioritySeperation, // s (requires SeTcbPrivilege)
		SystemVerifierAddDriverInformation, // s (requires SeDebugPrivilege) // 40
		SystemVerifierRemoveDriverInformation, // s (requires SeDebugPrivilege)
		SystemProcessorIdleInformation, // q: SYSTEM_PROCESSOR_IDLE_INFORMATION (EX in: u16 ProcessorGroup)
		SystemLegacyDriverInformation, // q: SYSTEM_LEGACY_DRIVER_INFORMATION
		SystemCurrentTimeZoneInformation, // q; s: RTL_TIME_ZONE_INFORMATION
		SystemLookasideInformation, // q: SYSTEM_LOOKASIDE_INFORMATION
		SystemTimeSlipNotification, // s: HANDLE (NtCreateEvent) (requires SeSystemtimePrivilege)
		SystemSessionCreate, // not implemented
		SystemSessionDetach, // not implemented
		SystemSessionInformation, // not implemented (SYSTEM_SESSION_INFORMATION)
		SystemRangeStartInformation, // q: SYSTEM_RANGE_START_INFORMATION // 50
		SystemVerifierInformation, // q: SYSTEM_VERIFIER_INFORMATION; s (requires SeDebugPrivilege)
		SystemVerifierThunkExtend, // s (kernel-mode only)
		SystemSessionProcessInformation, // q: SYSTEM_SESSION_PROCESS_INFORMATION
		SystemLoadGdiDriverInSystemSpace, // s: SYSTEM_GDI_DRIVER_INFORMATION (kernel-mode only) (same as SystemLoadGdiDriverInformation)
		SystemNumaProcessorMap, // q: SYSTEM_NUMA_INFORMATION
		SystemPrefetcherInformation, // q; s: PREFETCHER_INFORMATION // PfSnQueryPrefetcherInformation
		SystemExtendedProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
		SystemRecommendedSharedDataAlignment, // q: u32 // KeGetRecommendedSharedDataAlignment
		SystemComPlusPackage, // q; s: u32
		SystemNumaAvailableMemory, // q: SYSTEM_NUMA_INFORMATION // 60
		SystemProcessorPowerInformation, // q: SYSTEM_PROCESSOR_POWER_INFORMATION (EX in: u16 ProcessorGroup)
		SystemEmulationBasicInformation, // q: SYSTEM_BASIC_INFORMATION
		SystemEmulationProcessorInformation, // q: SYSTEM_PROCESSOR_INFORMATION
		SystemExtendedHandleInformation, // q: SYSTEM_HANDLE_INFORMATION_EX
		SystemLostDelayedWriteInformation, // q: u32
		SystemBigPoolInformation, // q: SYSTEM_BIGPOOL_INFORMATION
		SystemSessionPoolTagInformation, // q: SYSTEM_SESSION_POOLTAG_INFORMATION
		SystemSessionMappedViewInformation, // q: SYSTEM_SESSION_MAPPED_VIEW_INFORMATION
		SystemHotpatchInformation, // q; s: SYSTEM_HOTPATCH_CODE_INFORMATION
		SystemObjectSecurityMode, // q: u32 // 70
		SystemWatchdogTimerHandler, // s: SYSTEM_WATCHDOG_HANDLER_INFORMATION // (kernel-mode only)
		SystemWatchdogTimerInformation, // q: SYSTEM_WATCHDOG_TIMER_INFORMATION // (kernel-mode only)
		SystemLogicalProcessorInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION (EX in: u16 ProcessorGroup)
		SystemWow64SharedInformationObsolete, // not implemented
		SystemRegisterFirmwareTableInformationHandler, // s: SYSTEM_FIRMWARE_TABLE_HANDLER // (kernel-mode only)
		SystemFirmwareTableInformation, // SYSTEM_FIRMWARE_TABLE_INFORMATION
		SystemModuleInformationEx, // q: RTL_PROCESS_MODULE_INFORMATION_EX
		SystemVerifierTriageInformation, // not implemented
		SystemSuperfetchInformation, // q; s: SUPERFETCH_INFORMATION // PfQuerySuperfetchInformation
		SystemMemoryListInformation, // q: SYSTEM_MEMORY_LIST_INFORMATION; s: SYSTEM_MEMORY_LIST_COMMAND (requires SeProfileSingleProcessPrivilege) // 80
		SystemFileCacheInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (same as SystemFileCacheInformation)
		SystemThreadPriorityClientIdInformation, // s: SYSTEM_THREAD_CID_PRIORITY_INFORMATION (requires SeIncreaseBasePriorityPrivilege)
		SystemProcessorIdleCycleTimeInformation, // q: SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION[] (EX in: u16 ProcessorGroup)
		SystemVerifierCancellationInformation, // SYSTEM_VERIFIER_CANCELLATION_INFORMATION // name:wow64:whNT32QuerySystemVerifierCancellationInformation
		SystemProcessorPowerInformationEx, // not implemented
		SystemRefTraceInformation, // q; s: SYSTEM_REF_TRACE_INFORMATION // ObQueryRefTraceInformation
		SystemSpecialPoolInformation, // q; s: SYSTEM_SPECIAL_POOL_INFORMATION (requires SeDebugPrivilege) // MmSpecialPoolTag, then MmSpecialPoolCatchOverruns != 0
		SystemProcessIdInformation, // q: SYSTEM_PROCESS_ID_INFORMATION
		SystemErrorPortInformation, // s (requires SeTcbPrivilege)
		SystemBootEnvironmentInformation, // q: SYSTEM_BOOT_ENVIRONMENT_INFORMATION // 90
		SystemHypervisorInformation, // q: SYSTEM_HYPERVISOR_QUERY_INFORMATION
		SystemVerifierInformationEx, // q; s: SYSTEM_VERIFIER_INFORMATION_EX
		SystemTimeZoneInformation, // q; s: RTL_TIME_ZONE_INFORMATION (requires SeTimeZonePrivilege)
		SystemImageFileExecutionOptionsInformation, // s: SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION (requires SeTcbPrivilege)
		SystemCoverageInformation, // q: COVERAGE_MODULES s: COVERAGE_MODULE_REQUEST // ExpCovQueryInformation (requires SeDebugPrivilege)
		SystemPrefetchPatchInformation, // SYSTEM_PREFETCH_PATCH_INFORMATION
		SystemVerifierFaultsInformation, // s: SYSTEM_VERIFIER_FAULTS_INFORMATION (requires SeDebugPrivilege)
		SystemSystemPartitionInformation, // q: SYSTEM_SYSTEM_PARTITION_INFORMATION
		SystemSystemDiskInformation, // q: SYSTEM_SYSTEM_DISK_INFORMATION
		SystemProcessorPerformanceDistribution, // q: SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION (EX in: u16 ProcessorGroup) // 100
		SystemNumaProximityNodeInformation, // q; s: SYSTEM_NUMA_PROXIMITY_MAP
		SystemDynamicTimeZoneInformation, // q; s: RTL_DYNAMIC_TIME_ZONE_INFORMATION (requires SeTimeZonePrivilege)
		SystemCodeIntegrityInformation, // q: SYSTEM_CODEINTEGRITY_INFORMATION // SeCodeIntegrityQueryInformation
		SystemProcessorMicrocodeUpdateInformation, // s: SYSTEM_PROCESSOR_MICROCODE_UPDATE_INFORMATION
		SystemProcessorBrandString, // q: CHAR[] // HaliQuerySystemInformation -> HalpGetProcessorBrandString, info class 23
		SystemVirtualAddressInformation, // q: SYSTEM_VA_LIST_INFORMATION[]; s: SYSTEM_VA_LIST_INFORMATION[] (requires SeIncreaseQuotaPrivilege) // MmQuerySystemVaInformation
		SystemLogicalProcessorAndGroupInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX (EX in: LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType) // since WIN7 // KeQueryLogicalProcessorRelationship
		SystemProcessorCycleTimeInformation, // q: SYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION[] (EX in: u16 ProcessorGroup)
		SystemStoreInformation, // q; s: SYSTEM_STORE_INFORMATION (requires SeProfileSingleProcessPrivilege) // SmQueryStoreInformation
		SystemRegistryAppendString, // s: SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS // 110
		SystemAitSamplingValue, // s: u32 (requires SeProfileSingleProcessPrivilege)
		SystemVhdBootInformation, // q: SYSTEM_VHD_BOOT_INFORMATION
		SystemCpuQuotaInformation, // q; s: PS_CPU_QUOTA_QUERY_INFORMATION
		SystemNativeBasicInformation, // q: SYSTEM_BASIC_INFORMATION
		SystemErrorPortTimeouts, // SYSTEM_ERROR_PORT_TIMEOUTS
		SystemLowPriorityIoInformation, // q: SYSTEM_LOW_PRIORITY_IO_INFORMATION
		SystemTpmBootEntropyInformation, // q: TPM_BOOT_ENTROPY_NT_RESULT // ExQueryTpmBootEntropyInformation
		SystemVerifierCountersInformation, // q: SYSTEM_VERIFIER_COUNTERS_INFORMATION
		SystemPagedPoolInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypePagedPool)
		SystemSystemPtesInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemPtes) // 120
		SystemNodeDistanceInformation, // q: u16[4*NumaNodes] // (EX in: u16 NodeNumber)
		SystemAcpiAuditInformation, // q: SYSTEM_ACPI_AUDIT_INFORMATION // HaliQuerySystemInformation -> HalpAuditQueryResults, info class 26
		SystemBasicPerformanceInformation, // q: SYSTEM_BASIC_PERFORMANCE_INFORMATION // name:wow64:whNtQuerySystemInformation_SystemBasicPerformanceInformation
		SystemQueryPerformanceCounterInformation, // q: SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION // since WIN7 SP1
		SystemSessionBigPoolInformation, // q: SYSTEM_SESSION_POOLTAG_INFORMATION // since WIN8
		SystemBootGraphicsInformation, // q; s: SYSTEM_BOOT_GRAPHICS_INFORMATION (kernel-mode only)
		SystemScrubPhysicalMemoryInformation, // q; s: MEMORY_SCRUB_INFORMATION
		SystemBadPageInformation,
		SystemProcessorProfileControlArea, // q; s: SYSTEM_PROCESSOR_PROFILE_CONTROL_AREA
		SystemCombinePhysicalMemoryInformation, // s: MEMORY_COMBINE_INFORMATION, MEMORY_COMBINE_INFORMATION_EX, MEMORY_COMBINE_INFORMATION_EX2 // 130
		SystemEntropyInterruptTimingInformation, // q; s: SYSTEM_ENTROPY_TIMING_INFORMATION
		SystemConsoleInformation, // q; s: SYSTEM_CONSOLE_INFORMATION
		SystemPlatformBinaryInformation, // q: SYSTEM_PLATFORM_BINARY_INFORMATION (requires SeTcbPrivilege)
		SystemPolicyInformation, // q: SYSTEM_POLICY_INFORMATION (Warbird/Encrypt/Decrypt/Execute)
		SystemHypervisorProcessorCountInformation, // q: SYSTEM_HYPERVISOR_PROCESSOR_COUNT_INFORMATION
		SystemDeviceDataInformation, // q: SYSTEM_DEVICE_DATA_INFORMATION
		SystemDeviceDataEnumerationInformation, // q: SYSTEM_DEVICE_DATA_INFORMATION
		SystemMemoryTopologyInformation, // q: SYSTEM_MEMORY_TOPOLOGY_INFORMATION
		SystemMemoryChannelInformation, // q: SYSTEM_MEMORY_CHANNEL_INFORMATION
		SystemBootLogoInformation, // q: SYSTEM_BOOT_LOGO_INFORMATION // 140
		SystemProcessorPerformanceInformationEx, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX // (EX in: u16 ProcessorGroup) // since WINBLUE
		SystemCriticalProcessErrorLogInformation,
		SystemSecureBootPolicyInformation, // q: SYSTEM_SECUREBOOT_POLICY_INFORMATION
		SystemPageFileInformationEx, // q: SYSTEM_PAGEFILE_INFORMATION_EX
		SystemSecureBootInformation, // q: SYSTEM_SECUREBOOT_INFORMATION
		SystemEntropyInterruptTimingRawInformation,
		SystemPortableWorkspaceEfiLauncherInformation, // q: SYSTEM_PORTABLE_WORKSPACE_EFI_LAUNCHER_INFORMATION
		SystemFullProcessInformation, // q: SYSTEM_PROCESS_INFORMATION with SYSTEM_PROCESS_INFORMATION_EXTENSION (requires admin)
		SystemKernelDebuggerInformationEx, // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX
		SystemBootMetadataInformation, // 150
		SystemSoftRebootInformation, // q: u32
		SystemElamCertificateInformation, // s: SYSTEM_ELAM_CERTIFICATE_INFORMATION
		SystemOfflineDumpConfigInformation, // q: OFFLINE_CRASHDUMP_CONFIGURATION_TABLE_V2
		SystemProcessorFeaturesInformation, // q: SYSTEM_PROCESSOR_FEATURES_INFORMATION
		SystemRegistryReconciliationInformation, // s: NULL (requires admin) (flushes registry hives)
		SystemEdidInformation, // q: SYSTEM_EDID_INFORMATION
		SystemManufacturingInformation, // q: SYSTEM_MANUFACTURING_INFORMATION // since THRESHOLD
		SystemEnergyEstimationConfigInformation, // q: SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION
		SystemHypervisorDetailInformation, // q: SYSTEM_HYPERVISOR_DETAIL_INFORMATION
		SystemProcessorCycleStatsInformation, // q: SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION (EX in: u16 ProcessorGroup) // 160
		SystemVmGenerationCountInformation,
		SystemTrustedPlatformModuleInformation, // q: SYSTEM_TPM_INFORMATION
		SystemKernelDebuggerFlags, // SYSTEM_KERNEL_DEBUGGER_FLAGS
		SystemCodeIntegrityPolicyInformation, // q; s: SYSTEM_CODEINTEGRITYPOLICY_INFORMATION
		SystemIsolatedUserModeInformation, // q: SYSTEM_ISOLATED_USER_MODE_INFORMATION
		SystemHardwareSecurityTestInterfaceResultsInformation,
		SystemSingleModuleInformation, // q: SYSTEM_SINGLE_MODULE_INFORMATION
		SystemAllowedCpuSetsInformation, // s: SYSTEM_WORKLOAD_ALLOWED_CPU_SET_INFORMATION
		SystemVsmProtectionInformation, // q: SYSTEM_VSM_PROTECTION_INFORMATION (previously SystemDmaProtectionInformation)
		SystemInterruptCpuSetsInformation, // q: SYSTEM_INTERRUPT_CPU_SET_INFORMATION // 170
		SystemSecureBootPolicyFullInformation, // q: SYSTEM_SECUREBOOT_POLICY_FULL_INFORMATION
		SystemCodeIntegrityPolicyFullInformation,
		SystemAffinitizedInterruptProcessorInformation, // (requires SeIncreaseBasePriorityPrivilege)
		SystemRootSiloInformation, // q: SYSTEM_ROOT_SILO_INFORMATION
		SystemCpuSetInformation, // q: SYSTEM_CPU_SET_INFORMATION // since THRESHOLD2
		SystemCpuSetTagInformation, // q: SYSTEM_CPU_SET_TAG_INFORMATION
		SystemWin32WerStartCallout,
		SystemSecureKernelProfileInformation, // q: SYSTEM_SECURE_KERNEL_HYPERGUARD_PROFILE_INFORMATION
		SystemCodeIntegrityPlatformManifestInformation, // q: SYSTEM_SECUREBOOT_PLATFORM_MANIFEST_INFORMATION // since REDSTONE
		SystemInterruptSteeringInformation, // q: in: SYSTEM_INTERRUPT_STEERING_INFORMATION_INPUT, out: SYSTEM_INTERRUPT_STEERING_INFORMATION_OUTPUT // NtQuerySystemInformationEx // 180
		SystemSupportedProcessorArchitectures, // p: in opt: HANDLE, out: SYSTEM_SUPPORTED_PROCESSOR_ARCHITECTURES_INFORMATION[] // NtQuerySystemInformationEx
		SystemMemoryUsageInformation, // q: SYSTEM_MEMORY_USAGE_INFORMATION
		SystemCodeIntegrityCertificateInformation, // q: SYSTEM_CODEINTEGRITY_CERTIFICATE_INFORMATION
		SystemPhysicalMemoryInformation, // q: SYSTEM_PHYSICAL_MEMORY_INFORMATION // since REDSTONE2
		SystemControlFlowTransition, // (Warbird/Encrypt/Decrypt/Execute)
		SystemKernelDebuggingAllowed, // s: u32
		SystemActivityModerationExeState, // SYSTEM_ACTIVITY_MODERATION_EXE_STATE
		SystemActivityModerationUserSettings, // SYSTEM_ACTIVITY_MODERATION_USER_SETTINGS
		SystemCodeIntegrityPoliciesFullInformation,
		SystemCodeIntegrityUnlockInformation, // SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION // 190
		SystemIntegrityQuotaInformation,
		SystemFlushInformation, // q: SYSTEM_FLUSH_INFORMATION
		SystemProcessorIdleMaskInformation, // q: ULONG_PTR[ActiveGroupCount] // since REDSTONE3
		SystemSecureDumpEncryptionInformation,
		SystemWriteConstraintInformation, // SYSTEM_WRITE_CONSTRAINT_INFORMATION
		SystemKernelVaShadowInformation, // SYSTEM_KERNEL_VA_SHADOW_INFORMATION
		SystemHypervisorSharedPageInformation, // SYSTEM_HYPERVISOR_SHARED_PAGE_INFORMATION // since REDSTONE4
		SystemFirmwareBootPerformanceInformation,
		SystemCodeIntegrityVerificationInformation, // SYSTEM_CODEINTEGRITYVERIFICATION_INFORMATION
		SystemFirmwarePartitionInformation, // SYSTEM_FIRMWARE_PARTITION_INFORMATION // 200
		SystemSpeculationControlInformation, // SYSTEM_SPECULATION_CONTROL_INFORMATION // (CVE-2017-5715) REDSTONE3 and above.
		SystemDmaGuardPolicyInformation, // SYSTEM_DMA_GUARD_POLICY_INFORMATION
		SystemEnclaveLaunchControlInformation, // SYSTEM_ENCLAVE_LAUNCH_CONTROL_INFORMATION
		SystemWorkloadAllowedCpuSetsInformation, // SYSTEM_WORKLOAD_ALLOWED_CPU_SET_INFORMATION // since REDSTONE5
		SystemCodeIntegrityUnlockModeInformation, // SYSTEM_CODEINTEGRITY_UNLOCK_INFORMATION
		SystemLeapSecondInformation, // SYSTEM_LEAP_SECOND_INFORMATION
		SystemFlags2Information, // q: SYSTEM_FLAGS_INFORMATION
		SystemSecurityModelInformation, // SYSTEM_SECURITY_MODEL_INFORMATION // since 19H1
		SystemCodeIntegritySyntheticCacheInformation,
		SystemFeatureConfigurationInformation, // SYSTEM_FEATURE_CONFIGURATION_INFORMATION // since 20H1 // 210
		SystemFeatureConfigurationSectionInformation, // SYSTEM_FEATURE_CONFIGURATION_SECTIONS_INFORMATION
		SystemFeatureUsageSubscriptionInformation, // SYSTEM_FEATURE_USAGE_SUBSCRIPTION_DETAILS
		SystemSecureSpeculationControlInformation, // SECURE_SPECULATION_CONTROL_INFORMATION
		SystemSpacesBootInformation, // since 20H2
		SystemFwRamdiskInformation, // SYSTEM_FIRMWARE_RAMDISK_INFORMATION
		SystemWheaIpmiHardwareInformation,
		SystemDifSetRuleClassInformation, // SYSTEM_DIF_VOLATILE_INFORMATION
		SystemDifClearRuleClassInformation,
		SystemDifApplyPluginVerificationOnDriver, // SYSTEM_DIF_PLUGIN_DRIVER_INFORMATION
		SystemDifRemovePluginVerificationOnDriver, // SYSTEM_DIF_PLUGIN_DRIVER_INFORMATION // 220
		SystemShadowStackInformation, // SYSTEM_SHADOW_STACK_INFORMATION
		SystemBuildVersionInformation, // q: in: u32 (LayerNumber), out: SYSTEM_BUILD_VERSION_INFORMATION // NtQuerySystemInformationEx // 222
		SystemPoolLimitInformation, // SYSTEM_POOL_LIMIT_INFORMATION (requires SeIncreaseQuotaPrivilege)
		SystemCodeIntegrityAddDynamicStore,
		SystemCodeIntegrityClearDynamicStores,
		SystemDifPoolTrackingInformation,
		SystemPoolZeroingInformation, // q: SYSTEM_POOL_ZEROING_INFORMATION
		SystemDpcWatchdogInformation, // q; s: SYSTEM_DPC_WATCHDOG_CONFIGURATION_INFORMATION
		SystemDpcWatchdogInformation2, // q; s: SYSTEM_DPC_WATCHDOG_CONFIGURATION_INFORMATION_V2
		SystemSupportedProcessorArchitectures2, // q: in opt: HANDLE, out: SYSTEM_SUPPORTED_PROCESSOR_ARCHITECTURES_INFORMATION[] // NtQuerySystemInformationEx // 230
		SystemSingleProcessorRelationshipInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX // (EX in: PROCESSOR_NUMBER Processor)
		SystemXfgCheckFailureInformation, // q: SYSTEM_XFG_FAILURE_INFORMATION
		SystemIommuStateInformation, // SYSTEM_IOMMU_STATE_INFORMATION // since 22H1
		SystemHypervisorMinrootInformation, // SYSTEM_HYPERVISOR_MINROOT_INFORMATION
		SystemHypervisorBootPagesInformation, // SYSTEM_HYPERVISOR_BOOT_PAGES_INFORMATION
		SystemPointerAuthInformation, // SYSTEM_POINTER_AUTH_INFORMATION
		SystemSecureKernelDebuggerInformation,
		SystemOriginalImageFeatureInformation, // q: in: SYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_INPUT, out: SYSTEM_ORIGINAL_IMAGE_FEATURE_INFORMATION_OUTPUT // NtQuerySystemInformationEx
	};

	ENUM(FileInformationClass, u32)
	{
		FileDirectoryInformation = 1, // FILE_DIRECTORY_INFORMATION
		FileFullDirectoryInformation, // FILE_FULL_DIR_INFORMATION
		FileBothDirectoryInformation, // FILE_BOTH_DIR_INFORMATION
		FileBasicInformation, // FILE_BASIC_INFORMATION
		FileStandardInformation, // FILE_STANDARD_INFORMATION
		FileInternalInformation, // FileInternalInformation
		FileEaInformation, // FILE_EA_INFORMATION
		FileAccessInformation, // FILE_ACCESS_INFORMATION
		FileNameInformation, // FILE_NAME_INFORMATION
		FileRenameInformation, // FILE_RENAME_INFORMATION // 10
		FileLinkInformation, // FILE_LINK_INFORMATION
		FileNamesInformation, // FILE_NAMES_INFORMATION
		FileDispositionInformation, // FILE_DISPOSITION_INFORMATION
		FilePositionInformation, // FILE_POSITION_INFORMATION
		FileFullEaInformation, // FILE_FULL_EA_INFORMATION
		FileModeInformation, // FILE_MODE_INFORMATION
		FileAlignmentInformation, // FILE_ALIGNMENT_INFORMATION
		FileAllInformation, // FILE_ALL_INFORMATION
		FileAllocationInformation, // FILE_ALLOCATION_INFORMATION
		FileEndOfFileInformation, // FILE_END_OF_FILE_INFORMATION // 20
		FileAlternateNameInformation, // FILE_NAME_INFORMATION
		FileStreamInformation, // FILE_STREAM_INFORMATION
		FilePipeInformation, // FILE_PIPE_INFORMATION
		FilePipeLocalInformation, // FILE_PIPE_LOCAL_INFORMATION
		FilePipeRemoteInformation, // FILE_PIPE_REMOTE_INFORMATION
		FileMailslotQueryInformation, // FILE_MAILSLOT_QUERY_INFORMATION
		FileMailslotSetInformation, // FILE_MAILSLOT_SET_INFORMATION
		FileCompressionInformation, // FILE_COMPRESSION_INFORMATION
		FileObjectIdInformation, // FILE_OBJECTID_INFORMATION
		FileCompletionInformation, // FILE_COMPLETION_INFORMATION // 30
		FileMoveClusterInformation, // FILE_MOVE_CLUSTER_INFORMATION
		FileQuotaInformation, // FILE_QUOTA_INFORMATION
		FileReparsePointInformation, // FILE_REPARSE_POINT_INFORMATION
		FileNetworkOpenInformation, // FILE_NETWORK_OPEN_INFORMATION
		FileAttributeTagInformation, // FILE_ATTRIBUTE_TAG_INFORMATION
		FileTrackingInformation, // FILE_TRACKING_INFORMATION
		FileIdBothDirectoryInformation, // FILE_ID_BOTH_DIR_INFORMATION
		FileIdFullDirectoryInformation, // FILE_ID_FULL_DIR_INFORMATION
		FileValidDataLengthInformation, // FILE_VALID_DATA_LENGTH_INFORMATION
		FileShortNameInformation, // FILE_NAME_INFORMATION // 40
		FileIoCompletionNotificationInformation, // FILE_IO_COMPLETION_NOTIFICATION_INFORMATION // since VISTA
		FileIoStatusBlockRangeInformation, // FILE_IOSTATUSBLOCK_RANGE_INFORMATION
		FileIoPriorityHintInformation, // FILE_IO_PRIORITY_HINT_INFORMATION
		FileSfioReserveInformation, // FILE_SFIO_RESERVE_INFORMATION
		FileSfioVolumeInformation, // FILE_SFIO_VOLUME_INFORMATION
		FileHardLinkInformation, // FILE_LINKS_INFORMATION
		FileProcessIdsUsingFileInformation, // FILE_PROCESS_IDS_USING_FILE_INFORMATION
		FileNormalizedNameInformation, // FILE_NAME_INFORMATION
		FileNetworkPhysicalNameInformation, // FILE_NETWORK_PHYSICAL_NAME_INFORMATION
		FileIdGlobalTxDirectoryInformation, // FILE_ID_GLOBAL_TX_DIR_INFORMATION // since WIN7 // 50
		FileIsRemoteDeviceInformation, // FILE_IS_REMOTE_DEVICE_INFORMATION
		FileUnusedInformation,
		FileNumaNodeInformation, // FILE_NUMA_NODE_INFORMATION
		FileStandardLinkInformation, // FILE_STANDARD_LINK_INFORMATION
		FileRemoteProtocolInformation, // FILE_REMOTE_PROTOCOL_INFORMATION
		FileRenameInformationBypassAccessCheck, // (kernel-mode only); FILE_RENAME_INFORMATION // since WIN8
		FileLinkInformationBypassAccessCheck, // (kernel-mode only); FILE_LINK_INFORMATION
		FileVolumeNameInformation, // FILE_VOLUME_NAME_INFORMATION
		FileIdInformation, // FILE_ID_INFORMATION
		FileIdExtdDirectoryInformation, // FILE_ID_EXTD_DIR_INFORMATION // 60
		FileReplaceCompletionInformation, // FILE_COMPLETION_INFORMATION // since WINBLUE
		FileHardLinkFullIdInformation, // FILE_LINK_ENTRY_FULL_ID_INFORMATION
		FileIdExtdBothDirectoryInformation, // FILE_ID_EXTD_BOTH_DIR_INFORMATION // since THRESHOLD
		FileDispositionInformationEx, // FILE_DISPOSITION_INFO_EX // since REDSTONE
		FileRenameInformationEx, // FILE_RENAME_INFORMATION_EX
		FileRenameInformationExBypassAccessCheck, // (kernel-mode only); FILE_RENAME_INFORMATION_EX
		FileDesiredStorageClassInformation, // FILE_DESIRED_STORAGE_CLASS_INFORMATION // since REDSTONE2
		FileStatInformation, // FILE_STAT_INFORMATION
		FileMemoryPartitionInformation, // FILE_MEMORY_PARTITION_INFORMATION // since REDSTONE3
		FileStatLxInformation, // FILE_STAT_LX_INFORMATION // since REDSTONE4 // 70
		FileCaseSensitiveInformation, // FILE_CASE_SENSITIVE_INFORMATION
		FileLinkInformationEx, // FILE_LINK_INFORMATION_EX // since REDSTONE5
		FileLinkInformationExBypassAccessCheck, // (kernel-mode only); FILE_LINK_INFORMATION_EX
		FileStorageReserveIdInformation, // FILE_SET_STORAGE_RESERVE_ID_INFORMATION
		FileCaseSensitiveInformationForceAccessCheck // FILE_CASE_SENSITIVE_INFORMATION
	};

	FLAG(FileRenameFlag, u32)
	{
		replace_if_exists                       = 0x00000001, // since REDSTONE
		posix_semantics                         = 0x00000002,
		suppress_pin_state_inheritance          = 0x00000004, // since REDSTONE3
		suppress_storage_reserve_inheritance    = 0x00000008, // since REDSTONE5
		no_increase_available_space             = 0x00000010,
		no_decrease_available_space             = 0x00000020,
		preserve_available_space                = 0x00000030,
		ignore_readonly_attribute               = 0x00000040,
		force_resize_target_sr                  = 0x00000080, // since 19H1
		force_resize_source_sr                  = 0x00000100,
		force_resize_sr                         = 0x00000180
	};

	template<usize N>
	struct FILE_RENAME_INFORMATION_EX
	{
		FileRenameFlag Flags;
		HANDLE RootDirectory;
		u32 FileNameLength;
		c16 FileName[N];
	};

	struct FILE_DISPOSITION_INFORMATION_EX
	{
		FLAG_(Flags, u32)
		{
			FILE_DISPOSITION_DO_NOT_DELETE = 0x00000000,				//Specifies the system should not delete a file.
			FILE_DISPOSITION_DELETE = 0x00000001,						//Specifies the system should delete a file.
			FILE_DISPOSITION_POSIX_SEMANTICS = 0x00000002,				//Specifies the system should perform a POSIX - style delete.See more info in Remarks.
			FILE_DISPOSITION_FORCE_IMAGE_SECTION_CHECK = 0x00000004,	//Specifies the system should force an image section check.
			FILE_DISPOSITION_ON_CLOSE = 0x00000008,						//Specifies if the system sets or clears the on - close state.
			FILE_DISPOSITION_IGNORE_READONLY_ATTRIBUTE = 0x00000010,	//Allows read - only files to be deleted.See more info in Remarks.
		};
		Flags flags;
	};

	struct FILE_DISPOSITION_INFORMATION
	{
		bool deleteFile;
	};

	struct SYSTEM_TIMEOFDAY_INFORMATION
	{
		u64 BootTime;
		u64 CurrentTime;
		u64 TimeZoneBias;
		u32 TimeZoneId;
		u32 Reserved;
		u64 BootTimeBias;
		u64 SleepTimeBias;
	};

	struct SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX
	{
		void* Object;
		usize UniqueProcessId;
		usize HandleValue;
		u32 GrantedAccess;
		u16 CreatorBackTraceIndex;
		u16 ObjectTypeIndex;
		u32 HandleAttributes;
		u32 Reserved;
	};

	struct SYSTEM_BASIC_INFORMATION
	{
		u32 Reserved;
		u32 TimerResolution;
		u32 PageSize;
		u32 NumberOfPhysicalPages;
		u32 LowestPhysicalPageNumber;
		u32 HighestPhysicalPageNumber;
		u32 AllocationGranularity;
		usize MinimumUserModeAddress;
		usize MaximumUserModeAddress;
		usize ActiveProcessorsAffinityMask;
		u8 NumberOfProcessors;
	};

	struct SYSTEM_HANDLE_INFORMATION_EX
	{
		usize NumberOfHandles;
		usize Reserved;
		SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];
	};

	ENUM(MEMORY_INFORMATION_CLASS, u32)
	{
		MemoryBasicInformation
	};

	struct PS_ATTRIBUTE
	{
		usize Attribute;
		usize Size;
		const void* ValuePtr;
		usize* ReturnLength;
	};

	template<usize N = 1>
	struct PS_ATTRIBUTE_LIST
	{
		usize TotalLength;
		PS_ATTRIBUTE Attributes[N];
	};

	ENUM(PS_ATTRIBUTE_NUM, u32)
	{
		PsAttributeParentProcess, // in HANDLE
		PsAttributeDebugPort, // in HANDLE
		PsAttributeToken, // in HANDLE
		PsAttributeClientId, // out PCLIENT_ID
		PsAttributeTebAddress, // out PTEB *
		PsAttributeImageName, // in PWSTR
		PsAttributeImageInfo, // out PSECTION_IMAGE_INFORMATION
		PsAttributeMemoryReserve, // in PPS_MEMORY_RESERVE
		PsAttributePriorityClass, // in u8
		PsAttributeErrorMode, // in u32
		PsAttributeStdHandleInfo, // 10, in PPS_STD_HANDLE_INFO
		PsAttributeHandleList, // in HANDLE*
		PsAttributeGroupAffinity, // in PGROUP_AFFINITY
		PsAttributePreferredNode, // in PUSHORT
		PsAttributeIdealProcessor, // in PPROCESSOR_NUMBER
		PsAttributeUmsThread, // ? in PUMS_CREATE_THREAD_ATTRIBUTES
		PsAttributeMitigationOptions, // in u8
		PsAttributeProtectionLevel, // in u32
		PsAttributeSecureProcess, // since THRESHOLD
		PsAttributeJobList,
		PsAttributeChildProcessPolicy, // since THRESHOLD2
		PsAttributeAllApplicationPackagesPolicy, // since REDSTONE
		PsAttributeWin32kFilter,
		PsAttributeSafeOpenPromptOriginClaim,
		PsAttributeBnoIsolation, // PS_BNO_ISOLATION_PARAMETERS
		PsAttributeDesktopAppPolicy, // in u32
		PsAttributeChpe // since REDSTONE3
	};
	
	constexpr u32 PS_ATTRIBUTE_NUMBER_MASK{ 0x0000ffff };
	constexpr u32 PS_ATTRIBUTE_THREAD{ 0x00010000 }; // may be used with thread creation
	constexpr u32 PS_ATTRIBUTE_INPUT{ 0x00020000 }; // input only
	constexpr u32 PS_ATTRIBUTE_ADDITIVE{ 0x00040000 }; // "accumulated" e.g. bitmasks, counters, etc.
	//#define PsAttributeValue(Number, Thread, Input, Additive) (((Number) & PS_ATTRIBUTE_NUMBER_MASK) | ((Thread) ? PS_ATTRIBUTE_THREAD : 0) | ((Input) ? PS_ATTRIBUTE_INPUT : 0) | ((Additive) ? PS_ATTRIBUTE_ADDITIVE : 0))

	constexpr u32 PsAttributeValue(u32 number, bool thread, bool input, bool additive)noexcept
	{
		return (number & PS_ATTRIBUTE_NUMBER_MASK)
			| (thread ? PS_ATTRIBUTE_THREAD : 0)
			| (input ? PS_ATTRIBUTE_INPUT : 0)
			| (additive ? PS_ATTRIBUTE_ADDITIVE : 0);
	}

	namespace External
	{
		template<u8 Bits>
		using Pointer = UnsignedBits<Bits>;

		template<u8 Bits>
		struct UnicodeString
		{
			u16 Length;
			u16 MaximumLength;
			Pointer<Bits> Buffer;
			constexpr u16 size()const noexcept { return Length / sizeof(c16); }
		};

		template<u8 Bits>
		struct ListEntry
		{
			Pointer<Bits> Flink;
			Pointer<Bits> Blink;
		};

		template<u8 Bits>
		struct PebLdrData
		{
			u32 Length;
			u32 Initialized;
			Pointer<Bits> SsHandle;
			ListEntry<Bits> InLoadOrderModuleList;
			ListEntry<Bits> InMemoryOrderModuleList;
			ListEntry<Bits> InInitializationOrderModuleList;
			UnsignedBits<Bits> EntryInProgress;
			u32 ShutdownInProgress;
			UnsignedBits<Bits> ShutdownThreadId;
			constexpr Pointer<Bits> pHead()const noexcept { return InLoadOrderModuleList.Flink; }
			constexpr Pointer<Bits> pTail()const noexcept { return InLoadOrderModuleList.Blink; }
		};

		template<u8 Bits>
		struct LdrDataTableEntry
		{
			ListEntry<Bits> InLoadOrderLinks;
			ListEntry<Bits> InMemoryOrderLinks;
			ListEntry<Bits> InInitializationOrderLinks;
			Pointer<Bits> DllBase;
			Pointer<Bits> EntryPoint;
			u32 SizeOfImage;
			UnicodeString<Bits> FullDllName;
			UnicodeString<Bits> BaseDllName;
			constexpr Pointer<Bits> pNext()const { return InLoadOrderLinks.Flink; }
			constexpr Pointer<Bits> pLast()const { return InLoadOrderLinks.Blink; }
		};

		template<u8 Bits>
		struct CURDIR
		{
			UnicodeString<Bits> DosPath;
			Pointer<Bits> Handle;
		};
		
		template<u8 Bits>
		struct STRING
		{
			u16 Length;
			u16 MaximumLength;
			Pointer<Bits> Buffer;
		};

		template<u8 Bits>
		struct DriveLetterCurdir
		{
			u16 Flags;
			u16 Length;
			u32 TimeStamp;
			STRING<Bits> DosPath;
		};

		template<u8 Bits>
		struct ProcessParameters
		{
			u32 MaximumLength;
			u32 Length;
			u32 Flags;//Currently only one flag is known: PPF_NORMALIZED = 1
			u32 DebugFlags;
			Pointer<Bits> ConsoleHandle;
			u32 ConsoleFlags;
			Pointer<Bits> StandardInput;
			Pointer<Bits> StandardOutput;
			Pointer<Bits> StandardError;
			CURDIR<Bits> CurrentDirectory;
			UnicodeString<Bits> DllPath;
			UnicodeString<Bits> ImagePathName;
			UnicodeString<Bits> CommandLine;
			Pointer<Bits> Environment;
			u32 StartingX;
			u32 StartingY;
			u32 CountX;
			u32 CountY;
			u32 CountCharsX;
			u32 CountCharsY;
			u32 FillAttribute;
			u32 WindowFlags;
			u32 ShowWindowFlags;
			UnicodeString<Bits> WindowTitle;
			UnicodeString<Bits> DesktopInfo;
			UnicodeString<Bits> ShellInfo;
			UnicodeString<Bits> RuntimeData;
			std::array<DriveLetterCurdir<Bits>, 32> CurrentDirectories;
			Pointer<Bits> EnvironmentSize;
			Pointer<Bits> EnvironmentVersion;
			Pointer<Bits> PackageDependencyData;
			u32 ProcessGroupId;
			u32 LoaderThreads;
			UnicodeString<Bits> RedirectionDllName;
			UnicodeString<Bits> HeapPartitionName;
			Pointer<Bits> DefaultThreadpoolCpuSetMasks;
			u32 DefaultThreadpoolCpuSetMaskCount;
			u32 DefaultThreadpoolThreadMaximum;
		};

		template<u8 Bits>
		struct Peb
		{
			PROPERTIES(
				(((Bits==64)?0x2 :0x2 ), beingDebugged		, u8),
				(((Bits==64)?0x10:0x8 ), imageBase			, UnsignedBits<Bits>),
				(((Bits==64)?0x18:0xC ), pLdr				, UnsignedBits<Bits>),
				(((Bits==64)?0x20:0x10), pProcessParameters	, UnsignedBits<Bits>),
				(((Bits==64)?0x30:0x18), processHeap		, UnsignedBits<Bits>),
				(((Bits==64)?0x50:0x28), crossProcessFlags	, CrossProcessFlags)
			)
		};
	}
	FLAG(NtCreateProcessExFlag, u32)
	{
		request_breakaway = 1,
		no_debug_inherit = 2,
		inherit_handles = 4,
		large_pages = 8
		//all_flags = request_breakaway | no_debug_inherit | inherit_handles | large_pages
	};
	
	FLAG(RegOption, u32)
	{
		non_volatile    = 0x00000000, // Key is preserved when system is rebooted
		volatile_       = 0x00000001, // Key is not preserved when system is rebooted
		create_link     = 0x00000002, // Created key is a symbolic link
		backup_restore  = 0x00000004, // open for backup or restore special access rules privilege required
		open_link       = 0x00000008, // Open symbolic link
		dont_virtualize = 0x00000010, // Disable Open/Read/Write virtualization for this open and the resulting handle.
	};

	DEFINE_ENUM(KeyValueInformationClass,u32,
		basic,
		full,
		partial,
		full_align64,
		partial_align64,
		layer,
	);

	DEFINE_ENUM(RegType,u32,
		(none                       , 0 ), // No value type
		(sz                         , 1 ), // Unicode nul terminated string
		(expand_sz                  , 2 ), // Unicode nul terminated string
		(binary                     , 3 ), // Free form binary
		(dword                      , 4 ), // 32-bit number
		//(dword_little_endian        , 4 ), // 32-bit number (same as REG_DWORD)
		(dword_big_endian           , 5 ), // 32-bit number
		(link                       , 6 ), // Symbolic Link (unicode)
		(multi_sz                   , 7 ), // Multiple Unicode strings
		(resource_list              , 8 ), // Resource list in the resource map
		(full_resource_descriptor   , 9 ), // Resource list in the hardware description
		(resource_requirements_list , 10),
		(qword                      , 11)  // 64-bit number
		//(QWORD_LITTLE_ENDIAN      , 11)  // 64-bit number (same as REG_QWORD)
	);

	template<usize Size = 0x100>
	struct KeyValueBasicInformation
	{
		u32     TitleIndex;
		RegType Type;
		u32     NameLength;
		c16     Name[Size];

		StringView16 name()const noexcept { return { &Name[0], NameLength / sizeof(c16) }; }
	};
	template<class Stream, usize Size>
	requires($Same<c16, typename Stream::char_type>)
	Stream& operator<<(Stream& os, const KeyValueBasicInformation<Size>& info)noexcept
	{
		os << TS("TitleIndex = ") << info.TitleIndex;
		os << TS(", Type = ") << info.Type;
		os << TS(", Name = ") << info.name();
		return os;
	}

	struct KEY_VALUE_FULL_INFORMATION
	{
		u32     TitleIndex;
		RegType Type;
		u32     DataOffset;
		u32     DataLength;
		u32     NameLength;
		c16     Name[1];

		StringView16 name()const noexcept { return { &Name[0], NameLength / sizeof(c16) }; }
	};
	template<class Stream>
	requires($Same<c16, typename Stream::char_type>)
	Stream& operator<<(Stream& os, const KEY_VALUE_FULL_INFORMATION& info)noexcept
	{
		os << TS("TitleIndex = ") << info.TitleIndex;
		os << TS(", Type = ") << info.Type;
		os << TS(", DataOffset = ") << info.DataOffset;
		os << TS(", DataLength = ") << info.DataLength;
		os << TS(", Name = ") << info.name();
		return os;
	}

	template<usize Size = 0x100>
	struct KeyValuePartialInformation
	{
		u32     TitleIndex;
		RegType Type;
		u32     DataLength;
		u8      Data[Size];
		Span<u8> dataAsBytes()noexcept { return { &Data[0], DataLength }; }
		Span<const u8> dataAsBytes()const noexcept { return { &Data[0], DataLength }; }
		template<class T>
		const T& as()const noexcept { return *reinterpret_cast<const T*>(&Data[0]); }
	};

	template<>
	struct KeyValuePartialInformation<0>
	{
		u32     TitleIndex;
		RegType Type;
		u32     DataLength;
	};
	
	template<class Stream, usize Size>
	requires($Same<c16, typename Stream::char_type>)
	Stream& operator<<(Stream& os, const KeyValuePartialInformation<Size>& info)noexcept
	{
		os << TS("TitleIndex = ") << info.TitleIndex;
		os << TS(", Type = ") << info.Type;
		os << TS(", Data = ");
		switch (info.Type)
		{
			case RegType::sz:
			case RegType::expand_sz:
			{
				os << StringView16{ reinterpret_cast<const c16*>(&info.Data[0]) , info.DataLength / sizeof(c16) };
				break;
			}
			default: os << info.dataAsBytes();
		}
		return os;
	}
}