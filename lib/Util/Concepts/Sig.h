#pragma once
#include "../Compiler.h"
#include "../Macro/MovForward.h"
#include "../StaticAssert.h"
#include "Same.h"
#if 0
	template<class, class>
	constexpr bool SigImpl{};
	template<class T, class Ret, class...Args>
	constexpr bool SigImpl<T, Ret(Args...)>{ requires(T f, Args...args) { { FWD(f)(FWD(args)...) }->$Same<Ret>; } };
	template<class T, class Signature>concept $Sig = SigImpl<T, Signature>;
	template<class T, class...Signatures>concept $SigAnyOf = (SigImpl<T, Signatures> + ...) == 1;
	template<class T, class...Signatures>concept $SigAllOf = (SigImpl<T, Signatures> && ...);
#else
	template<class, class>
	struct SigImpl { STATIC_ASSERT(false, "invalid function signature"); };
	template<class T, class Ret, class...Args>
	struct SigImpl<T, Ret(Args...)>
	{
		STATIC_ASSERT(((!std::is_lvalue_reference_v<Args> || !std::is_const_v<std::remove_reference_t<Args>>) &&...), "replace const T& with const T");
	#if !MSVC//msvc bug
		STATIC_ASSERT(!(__is_empty(T) && requires(T f, Args...args) { FWD(f).operator()(FWD(args)...); } && !requires(T f, Args...args) { T::operator()(FWD(args)...); }), "mark stateless lambda static");
	#endif
		static constexpr bool value{ requires(T f, Args...args) { { FWD(f)(FWD(args)...) }->$Same<Ret>; } };
	};
	template<class T, class Signature>concept $Sig = SigImpl<T, Signature>::value;
	template<class T, class...Signatures>concept $SigAnyOf = (SigImpl<T, Signatures>::value + ...) == 1;
	template<class T, class...Signatures>concept $SigAllOf = (SigImpl<T, Signatures>::value && ...);
#endif

STATIC_ASSERT($Sig<decltype([]static{}), void()>);
STATIC_ASSERT($Sig<decltype([](int)static->int{return{}; }), int(int)>);
STATIC_ASSERT(!$Sig<decltype([](int)static{}), void()>);
STATIC_ASSERT(!$Sig<decltype([](int)static->int{return{}; }), void(int)>);
STATIC_ASSERT(!$Sig<decltype([](int)static->bool{return{}; }), int(int)>);
STATIC_ASSERT(!$Sig<decltype([](int)static->int{return{}; }), bool(int)>);