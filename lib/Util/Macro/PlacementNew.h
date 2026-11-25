#pragma once
#include <utility>
#include "../Compiler.h"
#include "Concat.h"
#define CTOR_AT(...) new(__VA_ARGS__)std::remove_reference<decltype(*(__VA_ARGS__))>::type
#define CTOR(...) CTOR_AT(__builtin_addressof(__VA_ARGS__))
#define DTOR_AT(...) do{if constexpr (!__is_trivially_destructible(std::remove_cvref_t<decltype(*(__VA_ARGS__))>)){ using CAT(_DTOR_TYPE_,__LINE__) = std::remove_cvref_t<decltype(*(__VA_ARGS__))>; (__VA_ARGS__)->~CAT(_DTOR_TYPE_,__LINE__)();}}while(0)
#define DTOR(...) DTOR_AT(__builtin_addressof(__VA_ARGS__))
#if CONSTEXPR_PLACEMENT_NEW
	#define CTOR_CONST_AT(pDest, ...) new(pDest)std::remove_reference<decltype(*(pDest))>::type{__VA_ARGS__}
#else
	#define CTOR_CONST_AT(...) std::construct_at(__VA_ARGS__)//because do{if consteval}while(0) make it a statement, instead of a expression. cant fold unless lambda
#endif
#define CTOR_CONST(dest, ...) CTOR_CONST_AT(__builtin_addressof(dest) __VA_OPT__(,__VA_ARGS__))