#pragma once
#define STATIC_ERROR(...) static_assert(0 __VA_OPT__(, #__VA_ARGS__))