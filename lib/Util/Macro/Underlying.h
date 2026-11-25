#pragma once
#include <utility>
#define UNDERLYING(...) static_cast<__underlying_type(std::remove_cvref_t<decltype(__VA_ARGS__)>)>(__VA_ARGS__)