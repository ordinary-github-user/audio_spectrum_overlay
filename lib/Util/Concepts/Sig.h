#pragma once
#include "../BasicTypes.h"
#include "../Compiler.h"
#include "../Exact.h"
#include "../Macro/MovForward.h"
#include "../StaticAssert.h"
#include "Same.h"
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
template<class T, class   Signature >concept $Sig      =  SigImpl<T, Signature >::value;
template<class T, class...Signatures>concept $SigAnyOf = (SigImpl<T, Signatures>::value + ...) == 1;
template<class T, class...Signatures>concept $SigAllOf = (SigImpl<T, Signatures>::value && ...);

template<class, class>
struct ExactSigImpl { STATIC_ASSERT(false, "invalid function signature"); };
template<class T, class Ret, class...Args>
struct ExactSigImpl<T, Ret(Args...)>
{
	STATIC_ASSERT(((!std::is_lvalue_reference_v<Args> || !std::is_const_v<std::remove_reference_t<Args>>) &&...), "replace const T& with const T");
#if !MSVC//msvc bug
	STATIC_ASSERT(!(__is_empty(T) && requires(T f, Args...args) { FWD(f).operator()(FWD(args)...); } && !requires(T f, Args...args) { T::operator()(FWD(args)...); }), "mark stateless lambda static");
#endif
	static constexpr bool value{ requires(T f, Exact<Args>...args) { { FWD(f)(FWD(args)...) }->$Same<Ret>; } };
};
template<class T, class   Signature >concept $ExactSig      =  ExactSigImpl<T, Signature >::value;
template<class T, class...Signatures>concept $ExactSigAnyOf = (ExactSigImpl<T, Signatures>::value + ...) == 1;
template<class T, class...Signatures>concept $ExactSigAllOf = (ExactSigImpl<T, Signatures>::value && ...);

STATIC_ASSERT($Sig<decltype([]static{}), void()>);
STATIC_ASSERT($Sig<decltype([](int)static->int{return{}; }), int(int)>);
STATIC_ASSERT($Sig<decltype([](u64)static->int{return{}; }), int(u32)>);
STATIC_ASSERT(!$Sig<decltype([](int)static{}), void()>);
STATIC_ASSERT(!$Sig<decltype([](int)static->int{return{}; }), void(int)>);
STATIC_ASSERT(!$Sig<decltype([](int)static->bool{return{}; }), int(int)>);
STATIC_ASSERT(!$Sig<decltype([](int)static->int{return{}; }), bool(int)>);

STATIC_ASSERT($ExactSig<decltype([]static{}), void()>);
STATIC_ASSERT($ExactSig<decltype([](int)static->int{return{}; }), int(int)>);
STATIC_ASSERT(!$ExactSig<decltype([](u64)static->int{return{}; }), int(u32)>);
STATIC_ASSERT(!$ExactSig<decltype([](int)static{}), void()>);
STATIC_ASSERT(!$ExactSig<decltype([](int)static->int{return{}; }), void(int)>);
STATIC_ASSERT(!$ExactSig<decltype([](int)static->bool{return{}; }), int(int)>);
STATIC_ASSERT(!$ExactSig<decltype([](int)static->int{return{}; }), bool(int)>);

template<class T, class...Args>concept      $Callable = requires(T f,       Args ...args){ FWD(f)(FWD(args)...); };
template<class T, class...Args>concept $ExactCallable = requires(T f, Exact<Args>...args){ FWD(f)(FWD(args)...); };