#pragma once
#include "../Concepts/Sig.h"
#include "../ElementOf.h"
#include "../Inline.h"
#include "../Unreachable.h"
namespace Algorithm
{
	template<class Container>
	INLINE constexpr decltype(&std::declval<Container>()[0]) find(Container&& container, const ElementOf<Container>& value)noexcept
	{
		for (auto& entry : container)
			if (entry == value)
				return __builtin_addressof(entry);
		return nullptr;
	}

	template<class Container>
	INLINE constexpr decltype(&std::declval<Container>()[0]) find_if(Container&& container, $Sig<bool(const ElementOf<Container>)> auto&& match)noexcept
	{
		for (auto& entry : container)
			if (match(entry))
				return __builtin_addressof(entry);
		return nullptr;
	}

	template<class Container>
	INLINE constexpr decltype(&std::declval<Container>()[0]) rfind_if(Container&& container, $Sig<bool(const ElementOf<Container>)> auto&& match)noexcept
	{
		auto begin{ container.rbegin() };
		auto end{ container.rend() };
		for (; begin != end; ++begin)
			if (match(*begin))
				return __builtin_addressof(*begin);
		return nullptr;
	}

	template<class Container>
	INLINE constexpr decltype(std::declval<Container>().size()) find_index_unsafe(Container&& container, const ElementOf<Container>& value)noexcept
	{
		const auto size{ container.size() };
		for (decltype(container.size()) i{};i!=size;++i)
			if (container[i] == value)
				return i;
		UNREACHABLE;
	}
}