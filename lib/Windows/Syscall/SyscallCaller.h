#pragma once
#include "../../Util/Compiler.h"
#include "../../Util/Compiler.h"
#include "../../Util/DataStructures/FixedStringImpl.h"
#include "../../Util/Macro/ArgCount.h"
#include "../../Util/Macro/ForEach.h"
#include "../../Util/Macro/Global.h"
#include "../MappedPeView.h"
#include "../TibTebPeb.h"
#include "Syscall.h"
#include "SyscallIndex.h"
#define SYSCALL_COMMA_ARG(type,name) ,name
#define SYSCALL_TYPE_ARG(type,name) type name

#if BITNESS==64
	namespace details
	{
		template<FixedString funcName, FixedString dll>
		INLINE u32 getSyscallIndex()noexcept
		{
			for (const Windows::LdrDataTableEntry* pModuleEntry{ reinterpret_cast<Windows::LdrDataTableEntry*>(Windows::thisPeb().pLdr->InLoadOrderModuleList.Flink) };;pModuleEntry = pModuleEntry->pNext())//search until crash
			{
				const Windows::MappedPeView pe{ pModuleEntry->DllBase };
				const void* pFunc{ pe.getFunctionLocalAddressByNameEx<const void*>([](StringView8 name)STATIC_LAMBDA_INLINE {return equal<funcName>(name); }) };
				if (pFunc)
					return Windows::SyscallIndexGrabber::get<dll>(pFunc, pe);
			}
			UNREACHABLE;
		}
	}

	#define SYSCALL_CALLER_CACHED(function_name, dll, ret, ...)																														\
	struct function_name ## Caller																																					\
	{																																												\
	private:																																										\
		u32 m_id;																																									\
	public:																																											\
		INLINE u32 index()const noexcept { return m_id; }																															\
		INLINE function_name ## Caller(u32 id = details::getSyscallIndex<#function_name, #dll>())noexcept :m_id{ id } {}															\
		INLINE ret operator()(FOR_EACH_TUPLE_COMMA(SYSCALL_TYPE_ARG, __VA_ARGS__))const noexcept { return syscall<ret>(index() FOR_EACH_TUPLE(SYSCALL_COMMA_ARG,__VA_ARGS__)); }	\
		template<class R = ret, $SyscallArg...Args>INLINE R overload(Args...args)const noexcept { return syscall<R, Args...>(index(), args...); }									\
	}GLOBAL function_name

	#define SYSCALL_CALLER(function_name, dll, ret, ...)																															\
	struct function_name ## Caller																																					\
	{																																												\
		INLINE static u32 index() noexcept { return details::getSyscallIndex<#function_name, #dll>(); }																				\
		INLINE static ret operator()(FOR_EACH_TUPLE_COMMA(SYSCALL_TYPE_ARG, __VA_ARGS__)) noexcept { return syscall<ret>(index() FOR_EACH_TUPLE(SYSCALL_COMMA_ARG,__VA_ARGS__)); }	\
		template<class R = ret, $SyscallArg...Args>INLINE static R overload(Args...args) noexcept { return syscall<R, Args...>(index(), args...); }									\
	}GLOBAL function_name
#else
	#define SYSCALL_CALLER_CACHED(function_name, ret, ...)																																						\
	struct function_name ## Caller																																												\
	{																																																			\
	private:																																																	\
		u32 m_id;																																																\
	public:																																																		\
		INLINE u32 index()const noexcept { return m_id; }																																						\
		INLINE function_name ## Caller()noexcept 																																								\
		{																																																		\
			for (const Windows::LdrDataTableEntry& moduleEntry : Windows::LocalModules{})																														\
			{																																																	\
				const void* pFunc{ Windows::MappedPeView{ moduleEntry.DllBase }.getFunctionLocalAddressByNameEx<const void*>([](StringView8 name)STATIC_LAMBDA_INLINE{return equal<#function_name>(name); }) };	\
				if (pFunc)																																														\
				{																																																\
					const Windows::Wow64SyscallIndex::Result res{ Windows::Wow64SyscallIndex{pFunc}() };																										\
					if (res.paramCount != ARG_COUNT(__VA_ARGS__))																																				\
						CRASH();																																												\
					m_id = res.index;																																											\
					return;																																														\
				}																																																\
			}																																																	\
		}																																																		\
		INLINE ret operator()(FOR_EACH_TUPLE_COMMA(SYSCALL_TYPE_ARG, __VA_ARGS__))const noexcept { return syscallWow64<ret>(index() FOR_EACH_TUPLE(SYSCALL_COMMA_ARG,__VA_ARGS__)); }							\
		template<class R = ret, $SyscallArg...Args>INLINE R overload(Args...args)const noexcept { return syscallWow64<R, Args...>(index(), args...); }															\
	}GLOBAL function_name
#endif