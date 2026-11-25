#pragma once
#include "../Util/Assert.h"
#include "../Util/Compiler.h"
#include "../Util/Concepts/Floating.h"
#include "../Util/Concepts/Same.h"
#include "../Util/ElementAt.h"
#include "../Util/Inline.h"
#include "../Util/Iota.h"
#include "../Util/TypeAt.h"
#include "Numeric/IsNaN.h"
namespace Math
{
	template<class T>
	[[nodiscard]] INLINE constexpr T bezier($Floating auto ratio, const T& start, const T& end)noexcept
	{
		return start + (end - start) * ratio;
	}

	template<class T>
	[[nodiscard]] INLINE constexpr T bezierClamped($Floating auto ratio, const T& start, const T& end)noexcept
	{
		/* */if (ratio <= 0)return start;
		else if (ratio >= 1)return end;
		else return bezier(ratio, start, end);
	}

	template<class...T>
	requires(sizeof...(T) > 2 && all_same<T...>)
	[[nodiscard]] INLINE constexpr TYPE_AT<0, T...> bezier($Floating auto ratio, const T&...points)noexcept
	{
#if VARIADIC_STRUCTURED_BINDING
		static constexpr auto [...I] {iota<sizeof...(points) - 1>};
	#if __cpp_pack_indexing >= 202311L
		return bezier(ratio, (points...[I] + (points...[I + 1] - points...[I]) * ratio)...);
	#else
		return bezier(ratio, (element_at<I>(points...) + (element_at<I + 1>(points...) - element_at<I>(points...)) * ratio)...);
	#endif
#else
		return[&]<usize...I>(std::index_sequence<I...>)LAMBDA_INLINE
		{
			return bezier(ratio, (element_at<I>(points...) + (element_at<I + 1>(points...) - element_at<I>(points...)) * ratio)...);
		}(std::make_index_sequence<sizeof...(points) - 1>{});
#endif
	}

	template<class...T>
	requires(sizeof...(T) > 2 && all_same<T...>)
	[[nodiscard]] INLINE constexpr TYPE_AT<0, T...> bezierClamped($Floating auto ratio, const T&...points)noexcept
	{
		/* */if (ratio <= 0)return element_at<0>(points...);
		else if (ratio >= 1)return element_at<sizeof...(points) - 1>(points...);
		else return bezier(ratio, points...);
	}

	template<class T>
	[[nodiscard]] INLINE constexpr T lerpDiff($Floating auto ratio, const T& start, const T& diff)noexcept
	{
		return start + diff * ratio;
	}

	template<class T>
	[[nodiscard]] INLINE constexpr T lerpDiffClamped($Floating auto ratio, const T& start, const T& diff)noexcept
	{
		/* */if (ratio <= 0)return start;
		else if (ratio >= 1)return start + diff;
		else return lerpDiff(ratio, start, diff);
	}

	template<$Floating T>
	[[nodiscard]] INLINE constexpr decltype(std::declval<T>() / std::declval<T>()) invLerp(const T& value, const T& start, const T& end)noexcept
	{
		ASSERT(start < end);
		return (value - start) / (end - start);
	}

	template<class T>
	[[nodiscard]] INLINE constexpr decltype(std::declval<T>() / std::declval<T>()) invLerpClamped(const T& value, const T& start, const T& end)noexcept
	{
		ASSERT(start <= end);
		using Ret = decltype(std::declval<T>() / std::declval<T>());
		const Ret result{ (value - start) / (end - start) };
		if (isNaN(result)) [[unlikely]] return Ret{ 0 };
		if (result <= Ret{ 0 }) [[unlikely]] return Ret{ 0 };
		if (result >= Ret{ 1 }) [[unlikely]] return Ret{ 1 };
		return result;
	}

	template<$Floating Input, class...Output>
	requires(sizeof...(Output) >= 2 && all_same<Output...>)
	[[nodiscard]] INLINE constexpr TYPE_AT<0, Output...> remap(Input input, Input iMin, Input iMax, const Output&...output)noexcept
	{
		ASSERT(iMin <= iMax);
		if (iMin == iMax)
			return input < iMin ? element_at<0>(output...) : element_at<sizeof...(Output) - 1>(output...);
		return bezier(invLerp(input, iMin, iMax), output...);
	}

	template<$Floating Input, class...Output>
	requires(sizeof...(Output) >= 2 && all_same<Output...>)
	[[nodiscard]] INLINE constexpr TYPE_AT<0, Output...> remapClamped(Input input, Input iMin, Input iMax, const Output&...output)noexcept
	{
		ASSERT(iMin <= iMax);
		return bezier(invLerpClamped(input, iMin, iMax), output...);
	}
}