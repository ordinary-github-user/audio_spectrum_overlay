#pragma once
#include <utility>
template<class T, class Stream>
concept $Printable = requires(Stream& s, const T x) { s << x; };