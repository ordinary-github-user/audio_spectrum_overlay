#pragma once
#define NO_COPY_MOVE(T) T(T&&) = delete;T& operator=(T&&) = delete;T(const T&) = delete;T& operator=(const T&) = delete;