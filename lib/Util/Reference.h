#pragma once
#include <functional>
#include <utility>
#include "Concepts/Printable.h"
template<class T>
using Ref = std::reference_wrapper<T>;

template<class Stream, $Printable<Stream> T>
constexpr Stream& operator<<(Stream& os, const Ref<T> ref)noexcept
{
	os << ref.get();
	return os;
}