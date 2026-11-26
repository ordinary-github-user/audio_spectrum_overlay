#pragma once
#include "BasicTypes.h"
#include "Compact.h"
#include "Compiler.h"
#include "Compiler.h"
#include "StaticAssert.h"
#include "TypeAt.h"
#include "TypeList.h"
//http://functionalcpp.wordpress.com/2013/08/05/function-traits/

struct FunctionFeatures
{
	bool is_noexcept;
	//bool is_variadic;
};

template<FunctionFeatures, class, class>struct FunctionTraitsImpl;
template<FunctionFeatures F, class C, class R, class... Ts>
struct FunctionTraitsImpl<F, C, R(Ts...)>
{
	using Class = C;
	using Ret = R;
	static constexpr auto arity{ compact<sizeof...(Ts)> };
	using Args = TypeList<Ts...>;
	template<usize I>requires(I<arity)using Arg = TYPE_AT<I,Ts...>;
	static constexpr bool is_const{ std::is_const_v<std::remove_reference_t<Class>> };
	static constexpr bool is_volatile{ std::is_volatile_v<std::remove_reference_t<Class>> };
	static constexpr bool is_lvalue_reference{ std::is_lvalue_reference_v<Class> };
	static constexpr bool is_rvalue_reference{ std::is_rvalue_reference_v<Class> };
	static constexpr bool is_noexcept{ F.is_noexcept };
	//static constexpr bool is_variadic{ F.is_variadic };
};

template<class>struct FunctionTraits;
template<class F>requires(requires{ typename FunctionTraits<F>::Ret; })struct FunctionTraits<F& >: FunctionTraits<F>{};
template<class F>requires(requires{ typename FunctionTraits<F>::Ret; })struct FunctionTraits<F&&>: FunctionTraits<F>{};
template<class R, class... Ts>struct FunctionTraits<R(Ts...)>:         FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},void, R(Ts...)>{};
template<class R, class... Ts>struct FunctionTraits<R(Ts...)noexcept> :FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},void, R(Ts...)>{};

#if (MSVC || CLANG_CL) && (BITNESS==32)
	template<class R, class... Ts>struct FunctionTraits<R(__stdcall*)(Ts...)>: FunctionTraits<R(Ts...)>{};
	template<class R, class... Ts>struct FunctionTraits<R(__cdecl*)(Ts...)>: FunctionTraits<R(Ts...)>{};
	template<class R, class... Ts>struct FunctionTraits<R(__stdcall*)(Ts...) noexcept>: FunctionTraits<R(Ts...)noexcept>{};
	template<class R, class... Ts>struct FunctionTraits<R(__cdecl*)(Ts...) noexcept>: FunctionTraits<R(Ts...)noexcept>{};
#else
	template<class R, class... Ts>struct FunctionTraits<R(*)(Ts...)>: FunctionTraits<R(Ts...)>{};
	template<class R, class... Ts>struct FunctionTraits<R(*)(Ts...) noexcept>: FunctionTraits<R(Ts...)noexcept>{};
#endif

template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)>:							FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},C                 , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)&>:							FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},C&                , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)&&>:							FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},C&&               , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const>:						FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},const C           , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const&>:						FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},const C&          , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const&&>:					FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},const C&&         , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)volatile>:					FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},volatile C        , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)volatile&>:					FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},volatile C&       , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)volatile&&>:					FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},volatile C&&      , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const volatile>:				FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},const volatile C  , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const volatile&>:			FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},const volatile C& , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const volatile&&>:			FunctionTraitsImpl<FunctionFeatures{.is_noexcept=0},const volatile C&&, R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...) noexcept>:					FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},C                 , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...) & noexcept>:				FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},C&                , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...) && noexcept>:				FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},C&&               , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const noexcept>:				FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},const C           , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const& noexcept>:			FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},const C&          , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const&& noexcept>:			FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},const C&&         , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)volatile noexcept>:			FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},volatile C        , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)volatile& noexcept>:			FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},volatile C&       , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)volatile&& noexcept>:		FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},volatile C&&      , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const volatile noexcept>:	FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},const volatile C  , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const volatile& noexcept>:	FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},const volatile C& , R(Ts...)>{};
template<class C, class R, class... Ts>struct FunctionTraits<R(C::*)(Ts...)const volatile&& noexcept>:	FunctionTraitsImpl<FunctionFeatures{.is_noexcept=1},const volatile C&&, R(Ts...)>{};

template<class Callable>
requires requires{&Callable::operator(); }
struct FunctionTraits<Callable>: FunctionTraits<decltype(&Callable::operator())>{};

namespace details::FunctionTraitsTest
{
#if !MSVC//msvc bug
	STATIC_ASSERT($Same<FunctionTraits<decltype([](i8, i16, i32)static noexcept{})>::Args, TypeList<i8, i16, i32>>);
	STATIC_ASSERT($Same<FunctionTraits<decltype([](i8, i16, i32)noexcept {})>::Args, TypeList<i8, i16, i32>>);
	STATIC_ASSERT($Same<FunctionTraits<decltype([](i8, i16, i32)static{})>::Args, TypeList<i8, i16, i32>>);
	STATIC_ASSERT($Same<FunctionTraits<decltype([](i8, i16, i32) {})>::Args, TypeList<i8, i16, i32>>);
	STATIC_ASSERT($Same<FunctionTraits<decltype([]()static{})>::Args, TypeList<>>);
	STATIC_ASSERT($Same<FunctionTraits<decltype([]() {})>::Args, TypeList<>>);
#endif
	STATIC_ASSERT($Same<FunctionTraits<void()noexcept>::Args, TypeList<>>);
	struct Test0 { constexpr i32 operator()(i8, i16, i32) const noexcept { return{}; } };
	STATIC_ASSERT($Same<FunctionTraits<Test0>::Args, TypeList<i8, i16, i32>>);
	struct Test1 { static constexpr i32 operator()(i8, i16, i32) noexcept { return{}; } };
	STATIC_ASSERT($Same<FunctionTraits<Test1>::Args, TypeList<i8, i16, i32>>);
}