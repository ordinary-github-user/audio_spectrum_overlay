#pragma once
#include "Compiler.h"
#if MSVC
	static_assert(L'あ' == 0x3042, "add msvc flag /utf-8");
#endif