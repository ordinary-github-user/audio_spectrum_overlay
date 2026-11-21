#pragma once
#include "BasicTypes.h"
#include "Inline.h"
#include <memory>
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