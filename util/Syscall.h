#pragma once
#include "Compiler.h"
#include "CallableBytes.h"
#include "Compiler.h"
#include "Consteval.h"
#include "DummyStruct.h"
#include "Identity.h"
#include "Inline.h"
#include "SizedInt.h"
#include <utility>
#define BITCAST_USIZE(x) static_cast<usize>(__builtin_bit_cast(UnsignedBits<sizeof(std::remove_cvref_t<decltype(x)>) * 8>,x))
template<class T>concept $SyscallArg = sizeof(T) <= sizeof(void*) && __is_trivially_copyable(T);
template<class T>concept $SyscallRet = $Same<void, T> || $SyscallArg<T>;
#if BITNESS == 64
	#if MSVC
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 idx, $SyscallArg auto...args)noexcept
		requires(8 + 8 * sizeof...(args) <= 0x7FFFFFFF)
		{
			return __builtin_bit_cast(Ret(*)(Identity<u64, sizeof(args)>..., u32), []static CONSTEVAL
			{
				/* */if constexpr (sizeof...(args) == 0)return &callable_bytes<
					0x8B, 0xC1,													//mov         eax, ecx
					0x0F, 0x05,													//syscall
					0xC3>[0];													//ret
				else if constexpr (sizeof...(args) == 1)return &callable_bytes<
					0x4C, 0x8B, 0xD1,											//mov         r10, rcx
					0x8B, 0xC2,													//mov         eax, edx
					0x0F, 0x05,													//syscall
					0xC3>[0];													//ret
				else if constexpr (sizeof...(args) == 2)return &callable_bytes<
					0x4C, 0x8B, 0xD1,											//mov         r10, rcx
					0x41, 0x8B, 0xC0,											//mov         eax, r8d
					0x0F, 0x05,													//syscall
					0xC3>[0];													//ret
				else if constexpr (sizeof...(args) == 3)return &callable_bytes<
					0x4C, 0x8B, 0xD1,											//mov         r10, rcx
					0x41, 0x8B, 0xC1,											//mov         eax, r9d
					0x0F, 0x05,													//syscall
					0xC3>[0];													//ret
				else if constexpr (8 + 8 * sizeof...(args) <= 0x7F)return &callable_bytes<
					0x4C, 0x8B, 0xD1,											//mov         r10, rcx
					0x8B, 0x44, 0x24, 8 + 8 * sizeof...(args),					//mov         eax, [rsp + 8 + 8 * sizeof...(args)]
					0x0F, 0x05,													//syscall
					0xC3>[0];													//ret
				else if constexpr (8 + 8 * sizeof...(args) <= 0x7FFFFFFF)return &callable_bytes<
					0x4C, 0x8B, 0xD1,											//mov         r10, rcx
					0x8B, 0x84, 0x24,											//mov         eax, [rsp + 8 + 8 * sizeof...(args)]
					(u8)((8 + 8 * sizeof...(args)) >> 0),
					(u8)((8 + 8 * sizeof...(args)) >> 8),
					(u8)((8 + 8 * sizeof...(args)) >> 16),
					(u8)((8 + 8 * sizeof...(args)) >> 24),
					0x0F, 0x05,													//syscall
					0xC3>[0];													//ret
			}())(BITCAST_USIZE(args)..., idx);
		}
	#else
		//https://github.com/JustasMasiulis/inline_syscall
		//https://github.com/JustasMasiulis/inline_syscall/issues/8
		//https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170#callercallee-saved-registers
		//Caller/callee saved registers
		//The x64 ABI considers the registers RAX, RCX, RDX, R8, R9, R10, R11, and XMM0 - XMM5 volatile.
		//When present, the upper portions of YMM0 - YMM15 and ZMM0 - ZMM15 are also volatile.
		//On AVX512VL, the ZMM, YMM, and XMM registers 16 - 31 are also volatile.
		//When AMX support is present, the TMM tile registers are volatile.
		//Consider volatile registers destroyed on function calls unless otherwise safety - provable by analysis such as whole program optimization.
		//The x64 ABI considers registers RBX, RBP, RDI, RSI, RSP, R12, R13, R14, R15, and XMM6 - XMM15 nonvolatile.They must be saved and restored by a function that uses them.
		//https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/floating-point-support-for-64-bit-drivers
		//Windows drivers for x86 and x64 systems that use extended processor features must wrap floating point calculations between calls to KeSaveExtendedProcessorState and KeRestoreExtendedProcessorState in order to avoid errors in concurrent applications that might be using the registers.
		//Legacy MMX / x87 Registers
		//These registers correspond to the XSTATE_MASK_LEGACY_FLOATING_POINT mask and are unavailable to drivers for x64 systems
		//SSE Registers
		//These registers correspond to the XSTATE_MASK_LEGACY_SSE flag and are used by the x64 compiler for floating point operations.
		//Drivers for x86 systems that use these registers must save them before use by passing the XSTATE_MASK_LEGACY or XSTATE_MASK_LEGACY_SSE flag in the KeSaveExtendedProcessorState call
		//and when finished, restore them with KeRestoreExtendedProcessorState.This is unnecessary on x64 systems, but not harmful.For more information about these registers see Using Floating Point in a WDM Driver.
		//AVX Registers
		//These registers correspond to the XSTATE_MASK_GSSE or XSTATE_MASK_AVX masks
		//In Windows 7 with Service Pack 1 (SP1), Windows Server 2008 R2, and newer versions of Windows, both x86 and x64 versions of the operating system preserve the AVX registers across thread(and process) switches.
		//To use the AVX registers in kernel mode, drivers(x86 and x64) must explicitly save and restore the AVX registers.AVX registers cannot be used in an interrupt service routine, and arithmetic exceptions are turned off by default.
		#if defined(__AMX_TILE__)
			#define SYSCALL_CLOBBER_LIST_TMM ,"tmm0", "tmm1", "tmm2", "tmm3", "tmm4", "tmm5", "tmm6", "tmm7"
		#else
			#define SYSCALL_CLOBBER_LIST_TMM
		#endif
		#if defined(__AVX2__)
			#define SYSCALL_CLOBBER_LIST_YMM ,"ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5"
		#else
			#define SYSCALL_CLOBBER_LIST_YMM
		#endif
		#if defined(__AVX512F__)
			#define SYSCALL_CLOBBER_LIST_ZMM ,"zmm0", "zmm1", "zmm2", "zmm3", "zmm4", "zmm5", "zmm16", "zmm17", "zmm18", "zmm19", "zmm20", "zmm21", "zmm22", "zmm23", "zmm24", "zmm25", "zmm26", "zmm27", "zmm28", "zmm29", "zmm30", "zmm31"\
											 ,"xmm16", "xmm17", "xmm18", "xmm19", "xmm20", "xmm21", "xmm22", "xmm23", "xmm24", "xmm25", "xmm26", "xmm27", "xmm28", "xmm29", "xmm30", "xmm31"\
											 ,"ymm16", "ymm17", "ymm18", "ymm19", "ymm20", "ymm21", "ymm22", "ymm23", "ymm24", "ymm25", "ymm26", "ymm27", "ymm28", "ymm29", "ymm30", "ymm31"
		#else
			#define SYSCALL_CLOBBER_LIST_ZMM
		#endif

		#define SYSCALL_CLOBBER_LIST "memory", "cc", "rcx", "r11","xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"\
									 SYSCALL_CLOBBER_LIST_TMM\
									 SYSCALL_CLOBBER_LIST_YMM\
									 SYSCALL_CLOBBER_LIST_ZMM
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id)noexcept
		{
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"syscall"
				: "=a"(rax)
				: "a"(id)
				: SYSCALL_CLOBBER_LIST, "rdx", "r8", "r9", "r10");
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"syscall"
				: "=a"(rax), "=r"(r10)
				: "a"(id), "r"(r10)
				: SYSCALL_CLOBBER_LIST, "rdx", "r8", "r9");
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"syscall"
				: "=a"(rax), "=r"(r10), "=r"(rdx)
				: "a"(id), "r"(r10), "r"(rdx)
				: SYSCALL_CLOBBER_LIST, "r8", "r9");
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"syscall"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8)
				: SYSCALL_CLOBBER_LIST, "r9");
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"syscall"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9)
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $48, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"syscall\n"
				"add $48, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $56, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"syscall\n"
				"add $56, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $64, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"syscall\n"
				"add $64, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $72, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"syscall\n"
				"add $72, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8, $SyscallArg auto _9)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $80, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"movq %[a9], 72(%%rsp)\n"
				"syscall\n"
				"add $80, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8)),
				[a9] "re"(BITCAST_USIZE(_9))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8, $SyscallArg auto _9, $SyscallArg auto _10)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $88, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"movq %[a9], 72(%%rsp)\n"
				"movq %[a10], 80(%%rsp)\n"
				"syscall\n"
				"add $88, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8)),
				[a9] "re"(BITCAST_USIZE(_9)),
				[a10] "re"(BITCAST_USIZE(_10))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8, $SyscallArg auto _9, $SyscallArg auto _10, $SyscallArg auto _11)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $96, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"movq %[a9], 72(%%rsp)\n"
				"movq %[a10], 80(%%rsp)\n"
				"movq %[a11], 88(%%rsp)\n"
				"syscall\n"
				"add $96, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8)),
				[a9] "re"(BITCAST_USIZE(_9)),
				[a10] "re"(BITCAST_USIZE(_10)),
				[a11] "re"(BITCAST_USIZE(_11))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8, $SyscallArg auto _9, $SyscallArg auto _10, $SyscallArg auto _11, $SyscallArg auto _12)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $104, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"movq %[a9], 72(%%rsp)\n"
				"movq %[a10], 80(%%rsp)\n"
				"movq %[a11], 88(%%rsp)\n"
				"movq %[a12], 96(%%rsp)\n"
				"syscall\n"
				"add $104, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8)),
				[a9] "re"(BITCAST_USIZE(_9)),
				[a10] "re"(BITCAST_USIZE(_10)),
				[a11] "re"(BITCAST_USIZE(_11)),
				[a12] "re"(BITCAST_USIZE(_12))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8, $SyscallArg auto _9, $SyscallArg auto _10, $SyscallArg auto _11, $SyscallArg auto _12, $SyscallArg auto _13)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $112, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"movq %[a9], 72(%%rsp)\n"
				"movq %[a10], 80(%%rsp)\n"
				"movq %[a11], 88(%%rsp)\n"
				"movq %[a12], 96(%%rsp)\n"
				"movq %[a13], 104(%%rsp)\n"
				"syscall\n"
				"add $112, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8)),
				[a9] "re"(BITCAST_USIZE(_9)),
				[a10] "re"(BITCAST_USIZE(_10)),
				[a11] "re"(BITCAST_USIZE(_11)),
				[a12] "re"(BITCAST_USIZE(_12)),
				[a13] "re"(BITCAST_USIZE(_13))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8, $SyscallArg auto _9, $SyscallArg auto _10, $SyscallArg auto _11, $SyscallArg auto _12, $SyscallArg auto _13, $SyscallArg auto _14)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $120, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"movq %[a9], 72(%%rsp)\n"
				"movq %[a10], 80(%%rsp)\n"
				"movq %[a11], 88(%%rsp)\n"
				"movq %[a12], 96(%%rsp)\n"
				"movq %[a13], 104(%%rsp)\n"
				"movq %[a14], 112(%%rsp)\n"
				"syscall\n"
				"add $120, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8)),
				[a9] "re"(BITCAST_USIZE(_9)),
				[a10] "re"(BITCAST_USIZE(_10)),
				[a11] "re"(BITCAST_USIZE(_11)),
				[a12] "re"(BITCAST_USIZE(_12)),
				[a13] "re"(BITCAST_USIZE(_13)),
				[a14] "re"(BITCAST_USIZE(_14))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8, $SyscallArg auto _9, $SyscallArg auto _10, $SyscallArg auto _11, $SyscallArg auto _12, $SyscallArg auto _13, $SyscallArg auto _14, $SyscallArg auto _15)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $128, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"movq %[a9], 72(%%rsp)\n"
				"movq %[a10], 80(%%rsp)\n"
				"movq %[a11], 88(%%rsp)\n"
				"movq %[a12], 96(%%rsp)\n"
				"movq %[a13], 104(%%rsp)\n"
				"movq %[a14], 112(%%rsp)\n"
				"movq %[a15], 120(%%rsp)\n"
				"syscall\n"
				"add $128, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8)),
				[a9] "re"(BITCAST_USIZE(_9)),
				[a10] "re"(BITCAST_USIZE(_10)),
				[a11] "re"(BITCAST_USIZE(_11)),
				[a12] "re"(BITCAST_USIZE(_12)),
				[a13] "re"(BITCAST_USIZE(_13)),
				[a14] "re"(BITCAST_USIZE(_14)),
				[a15] "re"(BITCAST_USIZE(_15))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8, $SyscallArg auto _9, $SyscallArg auto _10, $SyscallArg auto _11, $SyscallArg auto _12, $SyscallArg auto _13, $SyscallArg auto _14, $SyscallArg auto _15, $SyscallArg auto _16)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $136, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"movq %[a9], 72(%%rsp)\n"
				"movq %[a10], 80(%%rsp)\n"
				"movq %[a11], 88(%%rsp)\n"
				"movq %[a12], 96(%%rsp)\n"
				"movq %[a13], 104(%%rsp)\n"
				"movq %[a14], 112(%%rsp)\n"
				"movq %[a15], 120(%%rsp)\n"
				"movq %[a16], 128(%%rsp)\n"
				"syscall\n"
				"add $136, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8)),
				[a9] "re"(BITCAST_USIZE(_9)),
				[a10] "re"(BITCAST_USIZE(_10)),
				[a11] "re"(BITCAST_USIZE(_11)),
				[a12] "re"(BITCAST_USIZE(_12)),
				[a13] "re"(BITCAST_USIZE(_13)),
				[a14] "re"(BITCAST_USIZE(_14)),
				[a15] "re"(BITCAST_USIZE(_15)),
				[a16] "re"(BITCAST_USIZE(_16))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		template<$SyscallRet Ret>
		INLINE Ret syscall(u32 id, $SyscallArg auto _1, $SyscallArg auto _2, $SyscallArg auto _3, $SyscallArg auto _4, $SyscallArg auto _5, $SyscallArg auto _6, $SyscallArg auto _7, $SyscallArg auto _8, $SyscallArg auto _9, $SyscallArg auto _10, $SyscallArg auto _11, $SyscallArg auto _12, $SyscallArg auto _13, $SyscallArg auto _14, $SyscallArg auto _15, $SyscallArg auto _16, $SyscallArg auto _17)noexcept
		{
			register u64 r10 asm("r10"){BITCAST_USIZE(_1)};
			register u64 rdx asm("rdx"){BITCAST_USIZE(_2)};
			register u64 r8  asm("r8") {BITCAST_USIZE(_3)};
			register u64 r9  asm("r9") {BITCAST_USIZE(_4)};
			std::conditional_t<__is_same(void, Ret), u64, Ret> rax;
			asm volatile(
				"sub $144, %%rsp\n"
				"movq %[a5], 40(%%rsp)\n"
				"movq %[a6], 48(%%rsp)\n"
				"movq %[a7], 56(%%rsp)\n"
				"movq %[a8], 64(%%rsp)\n"
				"movq %[a9], 72(%%rsp)\n"
				"movq %[a10], 80(%%rsp)\n"
				"movq %[a11], 88(%%rsp)\n"
				"movq %[a12], 96(%%rsp)\n"
				"movq %[a13], 104(%%rsp)\n"
				"movq %[a14], 112(%%rsp)\n"
				"movq %[a15], 120(%%rsp)\n"
				"movq %[a16], 128(%%rsp)\n"
				"movq %[a17], 136(%%rsp)\n"
				"syscall\n"
				"add $144, %%rsp"
				: "=a"(rax), "=r"(r10), "=r"(rdx), "=r"(r8), "=r"(r9)
				: "a"(id), "r"(r10), "r"(rdx), "r"(r8), "r"(r9),
				[a5] "re"(BITCAST_USIZE(_5)),
				[a6] "re"(BITCAST_USIZE(_6)),
				[a7] "re"(BITCAST_USIZE(_7)),
				[a8] "re"(BITCAST_USIZE(_8)),
				[a9] "re"(BITCAST_USIZE(_9)),
				[a10] "re"(BITCAST_USIZE(_10)),
				[a11] "re"(BITCAST_USIZE(_11)),
				[a12] "re"(BITCAST_USIZE(_12)),
				[a13] "re"(BITCAST_USIZE(_13)),
				[a14] "re"(BITCAST_USIZE(_14)),
				[a15] "re"(BITCAST_USIZE(_15)),
				[a16] "re"(BITCAST_USIZE(_16)),
				[a17] "re"(BITCAST_USIZE(_17))
				: SYSCALL_CLOBBER_LIST);
			if constexpr (!__is_same(void, Ret))return rax;
		}
		#undef SYSCALL_CLOBBER_LIST
		#undef SYSCALL_CLOBBER_LIST_TMM
		#undef SYSCALL_CLOBBER_LIST_YMM
		#undef SYSCALL_CLOBBER_LIST_ZMM
	#endif
#else
	template<$SyscallRet Ret>
	INLINE Ret syscallWow64(u32 index, $SyscallArg auto...args)noexcept
	{
		#if MSVC
			return __builtin_bit_cast(Ret(__thiscall*)(u32, Identity<u32, sizeof(args)>...), &callable_bytes<
				0x8b, 0xc1,																		//mov  eax,ecx
				0x64, 0xff, 0x15, 0xc0, 0x00, 0x00, 0x00,										//call dword ptr fs:[C0]
				0xc2, ((sizeof...(args) * 4) & 0xff), (((sizeof...(args) * 4) & 0xff00) >> 8)	//ret  0010
			>)(index, BITCAST_USIZE(args)...);
		#else
			return __builtin_bit_cast(Ret __attribute__((regparm(1)))(*)(u32, Dummy, Identity<u32, sizeof(args)>...), __readfsdword(0xc0))(index, {}, BITCAST_USIZE(args)...);
		#endif
	}
#endif
#undef BITCAST_USIZE