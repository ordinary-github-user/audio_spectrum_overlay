#pragma once
#include "Compiler.h"
#include <utility>
#if CLANG || GCC
	template<class T, class   T0>concept $Same  = __is_same(T, T0);
	template<class T, class...Ts>concept $AnyOf = (... || __is_same(T, Ts));
	template<class T, class...Ts>concept $Not   = (... && !__is_same(T, Ts));
	template<class T, class...Ts>constexpr bool all_same{ (... && __is_same(T, Ts)) };
#else
	template<class T, class   T0>concept $Same  = std::is_same_v<T, T0>;
	template<class T, class...Ts>concept $AnyOf = (...|| std::is_same_v<T, Ts>);
	template<class T, class...Ts>concept $Not   = (...&&!std::is_same_v<T, Ts>);
	template<class T, class...Ts>constexpr bool all_same{ (... && std::is_same_v<T, Ts>) };
#endif