#pragma once
#include "Compact.h"
#include "Compiler.h"
#if META
	#include <meta>
	template<class T>
	constexpr auto field_count{ compact<std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()).size()> };
	template<usize index, class T>
	constexpr std::meta::info field_at{ std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked())[index] };
#endif