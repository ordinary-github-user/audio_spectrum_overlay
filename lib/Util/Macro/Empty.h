#pragma once
#include "Concat.h"
#define EMPTY_IMPL() 1
#define EMPTY_IMPL_() 0
#define EMPTY(...) CAT(EMPTY_IMPL,__VA_OPT__(_))()

#define NOT_EMPTY_IMPL() 0
#define NOT_EMPTY_IMPL_() 1
#define NOT_EMPTY(...) CAT(NOT_EMPTY_IMPL,__VA_OPT__(_))()