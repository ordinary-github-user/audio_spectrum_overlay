#pragma once
#include "Concepts/Char.h"
#include "DataStructures/FixedStringImpl.h"
#include "DataStructures/TStringImpl.h"
#include "EnumNameImpl.h"
//if enum name contains '_' only, returns itself unmodified
template<auto value, $Char T = c8>
constexpr auto enum_name_fixed_string{ encode<[]static
{
	static constexpr FixedString raw{ enum_name_impl<value>() };
	static constexpr bool no_underscore{ [] static
	{
		for (const c8 p : raw)
			if (p != '_')
				return false;
		return true;
	}() };
	if constexpr (no_underscore)
		return raw;
	else
	{
		static constexpr StaticString name{ [] static
		{
			StaticString result{ raw };
			using Index = std::remove_cvref_t<decltype(result)>::Index;
			while (result[0] == '_')
				result.erase(Index{});
			for (Index i{ Index(result.size() - 1) }; result[i] == '_';)
				result.erase(i--);
			return result;
		}() };
		return FixedString<c8, name.size()>{name};
	}
}(),T>};

template<auto value, $Char T = c8>
constexpr auto enum_name{ tstring<enum_name_fixed_string<value, T>> };