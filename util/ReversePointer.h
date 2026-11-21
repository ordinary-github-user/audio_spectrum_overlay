#pragma once
#include "ConceptPointer.h"
#include "Inline.h"
#include <utility>
template<$Pointer P>
struct ReversePointer
{
private:
	using T = std::remove_reference_t<decltype(*P{})>;
	T* p;
public:
	INLINE constexpr explicit ReversePointer(T* p)noexcept { if consteval { this->p = p; } else { this->p = p - 1; } }
	INLINE constexpr ReversePointer& operator++() noexcept { --p; return *this; }
	INLINE constexpr bool operator!=(ReversePointer r)const noexcept { return p != r.p; }
	INLINE constexpr T* operator->()const noexcept { if consteval { return p - 1; } else { return p; } }
	INLINE constexpr T& operator*() const noexcept { if consteval { return *(p - 1); } else { return *p; } }
};