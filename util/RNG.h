#pragma once
#include "ArraySequence.h"
#include "BasicTypes.h"
#include "ElementOf.h"
#include "FixedStringImpl.h"
#include "Fold.h"
#include "MacroGlobal.h"
#include "ObfuString.h"
#include "RandomDevice.h"
#include "StaticStringImpl.h"
#include "UniformFloatDistribution.h"
#include "UniformIntDistribution.h"
#include "VariadicSwitch.h"
#include "Xoshiro.h"

GLOBAL Xoshiro256ss g_rng{ RandomDevice<u64>{}() };

//inclusive inclusive
//template<class T>
//T rand(T min, T max)noexcept
//{
//	if constexpr ($Same<c8, T> || $Same<signed c8, T>)
//		return static_cast<T>(rand<i16>(min, max));
//	else if constexpr ($Same<u8, T>)
//		return static_cast<T>(rand<u16>(min, max));
//	else if constexpr (std::is_floating_point_v<T>)
//		return std::uniform_real_distribution<T>{min, max}(g_rng);
//	else if constexpr (std::is_integral_v<T> && !$Same<T, bool>)
//		return std::uniform_int_distribution<T>{min, max}(g_rng);
//}

//template<class F>
//bool randBool(F probTrue)noexcept
//{
//	STATIC_ASSERT(std::is_floating_point_v<F>);
//	return std::bernoulli_distribution{probTrue}(g_rng);
//}

[[nodiscard]] INLINE bool randBool()noexcept
{
	return g_rng() & 1;
}
//inclusive inclusive
template<class T, T min, T max>
requires(!$Same<T, bool>&& std::is_integral_v<T>&& min <= max)
[[nodiscard]] INLINE T randInt()noexcept
{
	return UniformIntDistribution<T, min, max>{}(g_rng);
}

template<class T>
[[nodiscard]] INLINE T randFloat(T min, T max)noexcept
{
	return UniformFloatDistribution<T>(min, max)(g_rng);
}

template<class T, T min, T max>
[[nodiscard]] INLINE T randFloat()noexcept 
{
	return UniformFloatDistribution<T, min, max>{}(g_rng);
}
//inclusive inclusive
template<$AnyOf<c8,c16> T, T min, T max>
[[nodiscard]] INLINE T randChar()noexcept
{
	using type = UnsignedBits<sizeof(T) * 8>;
	return static_cast<T>(randInt<type, static_cast<type>(min), static_cast<type>(max)>());
}

template<FixedString alphabet>
[[nodiscard]] INLINE ElementOf<decltype(alphabet)> randChar() noexcept
{
	return Switch<array_sequence<alphabet.size()>>(randInt<Compact<alphabet.size() - 1>, 0, alphabet.size() - 1> (), [&](auto I)LAMBDA_INLINE
	{
		return constant<alphabet[I]>;
	});
}

template<u8 minSize, u8 maxSize, FixedString alphabet>
[[nodiscard]] INLINE auto randString()noexcept
{
	using T = ElementOf<decltype(alphabet)>;
	if constexpr (minSize != maxSize)
	{
		StaticString<T, maxSize> result{ uninitialized, randInt<Compact<minSize, maxSize>, minSize, maxSize>() };
		for (T& c : result)c = randChar<alphabet>();
		return result;
	}
	else
	{
		FixedString<T, maxSize> result;
		for (T& c : result)c = randChar<alphabet>();
		return result;
	}
}

template<u8 minSize, u8 maxSize, FixedString alphabet = FixedString{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_ "}>[[nodiscard]] INLINE auto randString8 ()noexcept{return randString<minSize, maxSize, alphabet>(); }
template<u8 minSize, u8 maxSize, FixedString alphabet = FixedString{L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_ "}>[[nodiscard]] INLINE auto randString16()noexcept{return randString<minSize, maxSize, alphabet>(); }