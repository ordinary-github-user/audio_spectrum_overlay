#pragma once
#include <utility>
#include "ConceptSame.h"
#define FLIP_BOOL(...) do{static_assert($Same<bool, std::remove_cvref_t<decltype(__VA_ARGS__)>>);__VA_ARGS__=!__VA_ARGS__;}while(0)