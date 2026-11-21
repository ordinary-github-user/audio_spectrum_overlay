#pragma once
#include <array>
#include "ConceptSame.h"
#include "BasicTypes.h"
#include "Inline.h"
namespace details
{
	template<auto...>struct make_array_sequence;

	template<auto End>struct make_array_sequence<End>
	{
		static constexpr auto value{ [] static
		{
			using T = decltype(End);
			std::array<T, static_cast<usize>(End)> result{};
			for (T i{}; i != End; ++i)
				result[i] = i;
			return result;
		}() };
	};

	template<auto Begin, auto End>
	requires($Same<decltype(Begin), decltype(End)>)
	struct make_array_sequence<Begin, End>
	{
		static constexpr auto value{ [] static
		{
			using T = decltype(End);
			std::array<T, static_cast<usize>(End - Begin)> result{};
			for (T i{Begin}; i != End; ++i)
				result[i - Begin] = i;
			return result;
		}() };
	};
}
template<auto...X>
constexpr auto array_sequence{ details::make_array_sequence<X...>::value };