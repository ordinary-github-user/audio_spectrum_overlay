#pragma once
#include "FunctionName.h"
#include "FixedStringImpl.h"
#include "StaticStringImpl.h"
template<auto value>
consteval auto enum_name_impl()noexcept
{
	static constexpr FixedString functionName{ FUNCTION_NAME };
	static constexpr StaticString name{ []static CONSTEVAL
	{
		StaticString result{ functionName };
#if CLANG || GCC
		//clang
		//auto enum_name_impl() [value = E::a]
		//auto enum_name_impl() [value = (E)123]
		//gcc
		//constexpr auto enum_name_impl() [with auto value = E::c]
		//constexpr auto enum_name_impl() [with auto value = (E)123]
		result.erase(result.end() - 1, result.end());
		result.erase(result.begin(), result.rfind(' ') + 1);
		if (const c8* p{ result.rfind(')') }; p)
			result.erase(result.begin(), p + 1);
#elif MSVC
		//auto __cdecl enum_name_impl<E::c>(void) noexcept
		//auto __cdecl enum_name_impl<(enum E)0x7b>(void) noexcept
		result.erase(result.rfind('>'), result.end());
		result.erase(result.begin(), result.rfind('<') + 1);
		if (const c8* p{ result.rfind(')') }; p)
			result.erase(result.begin(), p + 1);
#endif
		if (const c8* p{ result.rfind(':') }; p)
			result.erase(result.begin(), p + 1);
		return result;
	}() };
	return FixedString<c8, name.size()>{name};
}