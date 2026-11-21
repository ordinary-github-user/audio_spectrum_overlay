#pragma once
template<class T, class U>
constexpr const T& truncate(const U& value)
{
	return *reinterpret_cast<const T*>(&value);
}