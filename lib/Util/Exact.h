#pragma once
#include "Concepts/Same.h"
#if 1
	template<class T>
	struct Exact
	{
		template<$Same<T> U>constexpr operator U const volatile&  ()const volatile& noexcept;
		template<$Same<T> U>constexpr operator U const         &  ()const         & noexcept;
		template<$Same<T> U>constexpr operator U       volatile&  ()      volatile& noexcept;
		template<$Same<T> U>constexpr operator U               &  ()              & noexcept;
		template<$Same<T> U>constexpr operator U const volatile&& ()const volatile&&noexcept;
		template<$Same<T> U>constexpr operator U const         && ()const         &&noexcept;
		template<$Same<T> U>constexpr operator U       volatile&& ()      volatile&&noexcept;
		template<$Same<T> U>constexpr operator U               && ()              &&noexcept;
	};
#else
	template<class T>
	struct Exact
	{
		template<$Same<T> U>
		constexpr operator U()noexcept;
	};
#endif