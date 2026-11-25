#pragma once
#include "Inline.h"
#include "Macro/MovForward.h"
#if defined(__cpp_static_call_operator)
	#define DEFINE_OPERATOR(name, op) struct name { INLINE static constexpr bool operator()(auto&& l, auto&& r) noexcept { return FWD(l) op FWD(r); } }
#else
	#define DEFINE_OPERATOR(name, op) struct name { INLINE constexpr bool operator()(auto&& l, auto&& r)const noexcept { return FWD(l) op FWD(r); } }
#endif
DEFINE_OPERATOR(Less, < );
DEFINE_OPERATOR(LessEqual, <= );
DEFINE_OPERATOR(Greater, > );
DEFINE_OPERATOR(GreaterEqual, >= );
#undef DEFINE_OPERATOR