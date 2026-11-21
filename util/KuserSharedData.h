#pragma once
#include "MsvcAllowNamelessStructUnion.h"
#include "BasicTypes.h"
#include "EnumDefinition.h"
namespace Windows
{
#pragma pack(push, 1)
	struct alignas(4) KsystemTime
	{
		//u32 LowPart;														//0x0
		//i32 High1Time;													//0x4
		//i32 High2Time;													//0x8
		u64 value;
		i32 unused;
	}; STATIC_ASSERT(0xc == sizeof(KsystemTime));
#pragma pack(pop)

	ENUM(NtProductType, u32)
	{
		NtProductWinNt = 1,
		NtProductLanManNt = 2,
		NtProductServer = 3
	};

	ENUM(AlternativeArchitectureType, u32)
	{
		StandardDesign = 0,
		NEC98x86 = 1,
		EndAlternatives = 2
	};

	struct XstateFeature
	{
		u32 Offset;															//0x0
		u32 Size;															//0x4
	}; STATIC_ASSERT(0x8 == sizeof(XstateFeature));

	//0x338 bytes (sizeof)
	struct XstateConfiguration
	{
		u64 EnabledFeatures;												//0x0
		u64 EnabledVolatileFeatures;										//0x8
		u32 Size;															//0x10
		union
		{
			u32 ControlFlags;												//0x14
			struct
			{
				u32 OptimizedSave : 1;										//0x14
				u32 CompactionEnabled : 1;									//0x14
			};
		};
		XstateFeature Features[64];											//0x18
		u64 EnabledSupervisorFeatures;										//0x218
		u64 AlignedFeatures;												//0x220
		u32 AllFeatureSize;													//0x228
		u32 AllFeatures[64];												//0x22c
		u64 EnabledUserVisibleSupervisorFeatures;							//0x330
	};STATIC_ASSERT(0x338 == sizeof(XstateConfiguration));

	struct KuserSharedData
	{
		u32 TickCountLowDeprecated;											//0x0
		u32 TickCountMultiplier;											//0x4
		volatile KsystemTime InterruptTime;									//0x8
		volatile KsystemTime SystemTime;									//0x14
		volatile KsystemTime TimeZoneBias;									//0x20
		u16 ImageNumberLow;													//0x2c
		u16 ImageNumberHigh;												//0x2e
		std::array<c16, 260> NtSystemRoot;									//0x30
		u32 MaxStackTraceDepth;												//0x238
		u32 CryptoExponent;													//0x23c
		u32 TimeZoneId;														//0x240
		u32 LargePageMinimum;												//0x244
		u32 AitSamplingValue;												//0x248
		u32 AppCompatFlag;													//0x24c
		u64 RNGSeedVersion;													//0x250
		u32 GlobalValidationRunlevel;										//0x258
		volatile i32 TimeZoneBiasStamp;										//0x25c
		u32 NtBuildNumber;													//0x260 Windows 10 only
		NtProductType NtProductType;										//0x264
		u8 ProductTypeIsValid;												//0x268
		u8 Reserved0[1];													//0x269
		u16 NativeProcessorArchitecture;									//0x26a
		u32 NtMajorVersion;													//0x26c
		u32 NtMinorVersion;													//0x270
		u8 ProcessorFeatures[64];											//0x274
		u32 Reserved1;														//0x2b4
		u32 Reserved3;														//0x2b8
		volatile u32 TimeSlip;												//0x2bc
		AlternativeArchitectureType AlternativeArchitecture;				//0x2c0
		u32 BootId;															//0x2c4
		i64 SystemExpirationDate;											//0x2c8
		u32 SuiteMask;														//0x2d0
		u8 KdDebuggerEnabled;												//0x2d4
		union
		{
			u8 MitigationPolicies;											//0x2d5
			struct
			{
				u8 NXSupportPolicy : 2;										//0x2d5
				u8 SEHValidationPolicy : 2;									//0x2d5
				u8 CurDirDevicesSkippedForDlls : 2;							//0x2d5
				u8 Reserved : 2;											//0x2d5
			};
		};
		u16 CyclesPerYield;													//0x2d6
		volatile u32 ActiveConsoleId;										//0x2d8
		volatile u32 DismountCount;											//0x2dc
		u32 ComPlusPackage;													//0x2e0
		u32 LastSystemRITEventTickCount;									//0x2e4
		u32 NumberOfPhysicalPages;											//0x2e8
		u8 SafeBootMode;													//0x2ec
		u8 VirtualizationFlags;												//0x2ed
		u8 Reserved12[2];													//0x2ee
		union
		{
			u32 SharedDataFlags;											//0x2f0
			struct
			{
				u32 DbgErrorPortPresent : 1;								//0x2f0
				u32 DbgElevationEnabled : 1;								//0x2f0
				u32 DbgVirtEnabled : 1;										//0x2f0
				u32 DbgInstallerDetectEnabled : 1;							//0x2f0
				u32 DbgLkgEnabled : 1;										//0x2f0
				u32 DbgDynProcessorEnabled : 1;								//0x2f0
				u32 DbgConsoleBrokerEnabled : 1;							//0x2f0
				u32 DbgSecureBootEnabled : 1;								//0x2f0
				u32 DbgMultiSessionSku : 1;									//0x2f0
				u32 DbgMultiUsersInSessionSku : 1;							//0x2f0
				u32 DbgStateSeparationEnabled : 1;							//0x2f0
				u32 SpareBits : 21;											//0x2f0
			};
		};
		u32 DataFlagsPad[1];												//0x2f4
		u64 TestRetInstruction;												//0x2f8
		i64 QpcFrequency;													//0x300
		u32 SystemCall;														//0x308
		union
		{
			u32 AllFlags;													//0x30c
			u32 Win32Process : 1;											//0x30c
			u32 Sgx2Enclave : 1;											//0x30c
			u32 VbsBasicEnclave : 1;										//0x30c
			u32 SpareBits : 29;												//0x30c
		} UserCetAvailableEnvironments;										//0x30c
		u64 SystemCallPad[2];												//0x310
#pragma pack(push, 1)
		union alignas(4)
		{
			volatile KsystemTime TickCount;									//0x320
			volatile u64 TickCountQuad;										//0x320
			u32 ReservedTickCountOverlay[3];								//0x320
		};
#pragma pack(pop)
		u32 TickCountPad[1];												//0x32c
		u32 Cookie;															//0x330
		u32 CookiePad[1];													//0x334
		i64 ConsoleSessionForegroundProcessId;								//0x338
		u64 TimeUpdateLock;													//0x340
		u64 BaselineSystemTimeQpc;											//0x348
		u64 BaselineInterruptTimeQpc;										//0x350
		u64 QpcSystemTimeIncrement;											//0x358
		u64 QpcInterruptTimeIncrement;										//0x360
		u8 QpcSystemTimeIncrementShift;										//0x368
		u8 QpcInterruptTimeIncrementShift;									//0x369
		u16 UnparkedProcessorCount;											//0x36a
		u32 EnclaveFeatureMask[4];											//0x36c
		u32 TelemetryCoverageRound;											//0x37c
		u16 UserModeGlobalLogger[16];										//0x380
		u32 ImageFileExecutionOptions;										//0x3a0
		u32 LangGenerationCount;											//0x3a4
		u64 Reserved4;														//0x3a8
		volatile u64 InterruptTimeBias;										//0x3b0
		volatile u64 QpcBias;												//0x3b8
		u32 ActiveProcessorCount;											//0x3c0
		volatile u8 ActiveGroupCount;										//0x3c4
		u8 Reserved9;														//0x3c5
		union
		{
			u16 QpcData;													//0x3c6
			struct
			{
				volatile u8 QpcBypassEnabled;								//0x3c6
				u8 QpcShift;												//0x3c7
			};
		};
		i64 TimeZoneBiasEffectiveStart;										//0x3c8
		i64 TimeZoneBiasEffectiveEnd;										//0x3d0
		XstateConfiguration XState;											//0x3d8
		KsystemTime FeatureConfigurationChangeStamp;						//0x710
		u32 Spare;															//0x71c
	};STATIC_ASSERT(0x720 == sizeof(KuserSharedData));
	constexpr usize kuserSharedDataUserModeAddress{ 0x7FFE0000 };
	constexpr u64 kuserSharedDataKernelModeAddress{ 0xFFFFF78000000000 };
	const KuserSharedData& kuserSharedData()noexcept { return *reinterpret_cast<const KuserSharedData*>(kuserSharedDataUserModeAddress); }
}