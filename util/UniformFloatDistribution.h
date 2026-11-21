#pragma once
#include "Assert.h"
#include "ConceptFloating.h"
#include "IsFinite.h"
#include "MacroMovForward.h"
#include "Pragma.h"
#include "StaticAssert.h"
#include <utility>
template<class, auto...>struct UniformFloatDistribution;
template<$Floating T, T min, T max>
requires(min <= max && Math::isFinite(min) && Math::isFinite(max))
struct UniformFloatDistribution<T, min, max>
{
	template<class Generator>
	requires(std::remove_cvref_t<Generator>::min() == 0)
	static constexpr T operator()(Generator&& generator) noexcept
	{
		PRAGMA_MSVC(warning(push)); PRAGMA_MSVC(warning(disable : 4056));
		static constexpr T distance{ max - min };
		PRAGMA_MSVC(warning(pop));
		T result;
		if constexpr (Math::isFinite(distance))
		{
			static constexpr T ratio{ distance / static_cast<T>(std::remove_cvref_t<Generator>::max()) };
			result = FWD(generator)() * ratio + min;
		}
		else
		{
			const T t{ static_cast<T>(FWD(generator)()) / static_cast<T>(std::remove_cvref_t<Generator>::max()) };
			result = min * (T{ 1 } - t) + max * t;
		}
		if (result >= max) [[unlikely]] return max;
		if (result <= min) [[unlikely]] return min;
		return result;
	}
};
template<$Floating T>
struct UniformFloatDistribution<T>
{
	T min;
	T max;
	constexpr UniformFloatDistribution(T min, T max)noexcept :min{ min }, max{ max } {}
	template<class Generator>
	requires(std::remove_cvref_t<Generator>::min() == 0)
	constexpr T operator()(Generator&& generator)const noexcept
	{
		ASSERT(min <= max && Math::isFinite(min) && Math::isFinite(max));
		const T t{ static_cast<T>(FWD(generator)()) / static_cast<T>(std::remove_cvref_t<Generator>::max()) };
		const T result{ min * (T{ 1 } - t) + max * t };
		if (result >= max) [[unlikely]] return max;
		if (result <= min) [[unlikely]] return min;
		return result;
	}
};