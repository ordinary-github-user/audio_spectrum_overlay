#pragma once
namespace Overflow
{
	template<$SInt T>requires(sizeof(T) <= sizeof(u64))constexpr T sub  (T l, T r)noexcept { return static_cast<T>(static_cast<i64>(l) - static_cast<i64>(r)); }
	template<$SInt T>requires(sizeof(T) <= sizeof(u64))constexpr T add  (T l, T r)noexcept { return static_cast<T>(static_cast<i64>(l) + static_cast<i64>(r)); }
	template<$UInt T>requires(sizeof(T) <= sizeof(u64))constexpr T mullo(T l, T r)noexcept { return static_cast<T>(static_cast<u64>(l) * static_cast<u64>(r));}
}