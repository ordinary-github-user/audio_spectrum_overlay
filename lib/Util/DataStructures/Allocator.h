#pragma once
#include <memory>
#include "../BasicTypes.h"
#include "../Compiler.h"
#include "../Inline.h"
template<class T>
INLINE constexpr T* allocate(usize size)noexcept
{
	return std::allocator<T>{}.allocate(size);
}

template<class T>
INLINE constexpr void deallocate(T* ptr, usize size)noexcept
{
	std::allocator<T>{}.deallocate(ptr, size);
}