#pragma once
#include "../Util/Compiler.h"
#include "../Util/Crash.h"
#include "../Util/DataStructures/TStringImpl.h"
#include "../Util/Macro/Global.h"
#include "MappedPeView.h"
#include "TibTebPeb.h"
namespace details
{
	void* getFunctionAddress(StringView8 functionName)noexcept
	{
		for (const Windows::LdrDataTableEntry& entry : Windows::LocalModules{})
		{
			const Windows::MappedPeView pe{ entry.DllBase };
			void* const pFunc{ pe.getFunctionLocalAddressByName<void*>(StringView8{functionName}) };
			if (pFunc && !pe.isFunctionAddressForwarded(pFunc))
				return pFunc;
		}
		CRASH();
		UNREACHABLE;
	}
	#define DLLCALL_GET_ADDRESS(function_name) details::getFunctionAddress(OString{TS(#function_name)})
}

#define DLLCALL_ARG(type,name) name
#define DLLCALL_TYPE_ARG(type,name) type name
#define DLLCALL_TYPE(type,name) type

#define DLLCALL_CALLER_CACHED(function_name,convention,ret,...)																																		\
struct function_name ## Caller																																										\
{																																																	\
private:																																															\
	ret(convention*m_ptr)(FOR_EACH_TUPLE_COMMA(DLLCALL_TYPE, __VA_ARGS__));																															\
public:																																																\
	INLINE function_name ## Caller(void* ptr = DLLCALL_GET_ADDRESS(function_name))noexcept :m_ptr{ __builtin_bit_cast(ret(convention*)(FOR_EACH_TUPLE_COMMA(DLLCALL_TYPE, __VA_ARGS__)),ptr) } {}	\
	INLINE ret operator()(FOR_EACH_TUPLE_COMMA(DLLCALL_TYPE_ARG, __VA_ARGS__))const noexcept { return m_ptr(FOR_EACH_TUPLE_COMMA(DLLCALL_ARG,__VA_ARGS__)); }										\
	template<class R = ret, class...Ts>INLINE R overload(Ts...args)const noexcept { return __builtin_bit_cast(R(convention*)(Ts...),m_ptr)(args...); }												\
	INLINE ret(convention*address()const noexcept)(FOR_EACH_TUPLE_COMMA(DLLCALL_TYPE, __VA_ARGS__)){return m_ptr;}																					\
}GLOBAL function_name

#define DLLCALL_CALLER(function_name,convention,ret,...)																																												\
struct function_name ## Caller																																																			\
{																																																										\
	INLINE static ret(convention*address()noexcept)(FOR_EACH_TUPLE_COMMA(DLLCALL_TYPE, __VA_ARGS__)){return __builtin_bit_cast(ret(convention*)(FOR_EACH_TUPLE_COMMA(DLLCALL_TYPE, __VA_ARGS__)),DLLCALL_GET_ADDRESS(function_name));}	\
	INLINE static ret operator()(FOR_EACH_TUPLE_COMMA(DLLCALL_TYPE_ARG, __VA_ARGS__)) noexcept { return address()(FOR_EACH_TUPLE_COMMA(DLLCALL_ARG,__VA_ARGS__)); }																		\
	template<class R = ret, class...Ts>INLINE static R overload(Ts...args) noexcept { return __builtin_bit_cast(R(convention*)(Ts...),address())(args...); }																			\
}GLOBAL function_name
