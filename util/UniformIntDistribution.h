#pragma once
#include "BasicTypes.h"
#include "ConceptIntegral.h"
#include "SizedInt.h"
#include "MacroMovForward.h"
#include <limits>
#include <utility>
template<$UInt T, T...>
struct UniformUintDistribution;

//Max: Inclusive
template<$UInt T, T Max>
struct UniformUintDistribution<T, Max>
{
	template<class Generator>
	static constexpr T operator()(Generator&& generator) noexcept
	{
		using G = typename std::remove_cvref_t<Generator>::result_type;
		STATIC_ASSERT(sizeof(G) >= sizeof(T));
		STATIC_ASSERT(std::remove_cvref_t<Generator>::min() == 0);
		if constexpr (Max == 0)
			return 0;
		else
		{
			G v{ FWD(generator)() };
			if constexpr (Max == std::numeric_limits<G>::max())
			{
				STATIC_ASSERT(std::numeric_limits<G>::max() == std::remove_cvref_t<Generator>::max());
				return v;
			}
			else
			{
				static constexpr G distance{ G{Max} + 1 };
				STATIC_ASSERT(std::remove_cvref_t<Generator>::max() >= distance);
				static constexpr G discardThreshold{ std::remove_cvref_t<Generator>::max() / distance * distance };
				while (v >= discardThreshold) [[unlikely]]
					v = FWD(generator)();
				return v % distance;
			}
		}
	}
};

template<$UInt T>
struct UniformUintDistribution<T>
{
private:
	T max;
public:
	constexpr UniformUintDistribution(T max)noexcept:max{max}{}
	template<class Generator>
	constexpr T operator()(Generator&& generator)const noexcept
	{
		using G = typename std::remove_cvref_t<Generator>::result_type;
		STATIC_ASSERT(sizeof(G) >= sizeof(T));
		STATIC_ASSERT(std::remove_cvref_t<Generator>::min() == 0);
		G v{ FWD(generator)() };
		if (max == std::numeric_limits<G>::max()) [[unlikely]]
			return static_cast<T>(v);
		else
		{
			const G distance{ G{max} + 1 };
			const G discardThreshold{ std::remove_cvref_t<Generator>::max() / distance * distance };
			while (v >= discardThreshold) [[unlikely]]
				v = FWD(generator)();
			return static_cast<T>(v % distance);
		}
	}
};

template<$Int T, T...>
struct UniformIntDistribution;

//Min: Inclusive, Max: Inclusive
template<$Int T, T Min, T Max>
requires(Min <= Max)
struct UniformIntDistribution<T,Min,Max>
{
	template<class Generator>
	static constexpr T operator()(Generator&& generator) noexcept
	{
		using U = UnsignedBytes<sizeof(T)>;
		static constexpr U distance{ static_cast<U>(static_cast<U>(Max) - static_cast<U>(Min)) };
		STATIC_ASSERT(std::remove_cvref_t<Generator>::min() == 0 && std::remove_cvref_t<Generator>::max() >= distance);
		return UniformUintDistribution<UnsignedBytes<sizeof(T)>, distance>{}(FWD(generator)) + Min;
	}
};

//Min: Inclusive, Max: Inclusive
template<$Int T>
struct UniformIntDistribution<T>
{
private:
	T min;
	T max;
public:
	constexpr UniformIntDistribution(T min, T max)noexcept:min{min},max{max}{}
	template<class Generator>
	constexpr T operator()(Generator&& generator)const noexcept
	{
		using U = UnsignedBytes<sizeof(T)>;
		const U distance{ static_cast<U>(static_cast<U>(max) - static_cast<U>(min)) };
		STATIC_ASSERT(std::remove_cvref_t<Generator>::min() == 0);
		return UniformUintDistribution<UnsignedBytes<sizeof(T)>>{distance}(FWD(generator)) + min;
	}
};