#pragma once
#include <utility>
#include "Concepts/Sig.h"
#include "Inline.h"
#include "Macro/Concat.h"
#include "Macro/MovForward.h"
#include "Macro/NoCopyMove.h"
#include "NoUniqueAddress.h"
#include "StaticAssert.h"
template<$Sig<void()> Action>
requires(!std::is_reference_v<Action>)
struct [[nodiscard]] ScopeGuard
{
	NO_COPY_MOVE(ScopeGuard)
private:
	NO_UNIQUE_ADDRESS Action m_action;
public:
	INLINE constexpr ~ScopeGuard()noexcept { m_action(); }
	INLINE constexpr ScopeGuard(Action&& action)noexcept :m_action{ MOV(action) } {}
};

#define SCOPE_GUARD_IMPL(Capture,Static,...) __VA_OPT__(ScopeGuard CAT(_SCOPE_GUARD, __COUNTER__){[Capture]LAMBDA_INLINE Static{__VA_ARGS__}};)
#define SCOPE_GUARD(...) SCOPE_GUARD_IMPL(&,,__VA_ARGS__)
#define SCOPE_GUARD_STATIC(...) SCOPE_GUARD_IMPL(,static,__VA_ARGS__)