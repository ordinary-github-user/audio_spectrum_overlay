#pragma once
#include "Compiler.h"
#include "DataStructures/FixedStringImpl.h"
#include "DataStructures/StaticStringImpl.h"
#include "FunctionName.h"
#include "Macro/Consteval.h"
namespace details
{
	CONSTEVAL bool not_cpp_identifier(c8 x)noexcept
	{
		return x == ' ' || x == '!' || x == '*' || x == '&' || x == '^' || x == '~' || x == ',' || x == '.' || x == ':' || x == '<' || x == '>' || x == '(' || x == ')' || x == '[' || x == ']' || x == '{' || x == '}';
	}
}
template<class T>
CONSTEVAL auto nameof_impl()noexcept
{
	static constexpr FixedString functionName{ FUNCTION_NAME };
	static constexpr StaticString typeName{ [] static CONSTEVAL
	{
		StaticString result{ functionName };
#if MSVC
		result.erase(result.rfind('>'), result.end()); result.erase(result.begin(), result.find('<') + 1);
#else
		result.erase(result.rfind(']'), result.end()); result.erase(result.begin(), result.find('=') + 2);
#endif
		for (c8* p{ result.begin() }; p != result.end();)//"int &&" -> int&&
		{
			if (p[0] == ' ' && (p == result.begin() || p + 1 == result.end() || details::not_cpp_identifier(p[-1]) || details::not_cpp_identifier(p[1])))
				result.erase(p, 1);
			else
				++p;
		}
#if MSVC
		for (c8* p{ result.begin() };;)
		{
			const usize size{ static_cast<usize>(result.end() - p) };
			/* */if (size >= 7 && p[0] == 's' && p[1] == 't' && p[2] == 'r' && p[3] == 'u' && p[4] == 'c' && p[5] == 't' && p[6] == ' ')result.erase(p, 7);//remain.starts_with<"struct ">()
			else if (size >= 6 && p[0] == 'u' && p[1] == 'n' && p[2] == 'i' && p[3] == 'o' && p[4] == 'n' && p[5] == ' ')result.erase(p, 6);//remain.starts_with<"union ">()
			else if (size >= 6 && p[0] == 'c' && p[1] == 'l' && p[2] == 'a' && p[3] == 's' && p[4] == 's' && p[5] == ' ')result.erase(p, 6);//remain.starts_with<"class ">()
			else if (size >= 5 && p[0] == 'e' && p[1] == 'n' && p[2] == 'u' && p[3] == 'm' && p[4] == ' ')result.erase(p, 5);//remain.starts_with<"enum ">()
			for (;;)
			{
				if (p == result.end())
					return result;
				++p;
				if (details::not_cpp_identifier(p[-1]))break;
			}
		}
#else
		return result;
#endif
	}() };
	return FixedString<c8, typeName.size()>{ typeName };
}

template<class T>constexpr FixedString nameof{ nameof_impl<T>() };