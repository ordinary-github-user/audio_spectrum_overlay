#pragma once
//https://devblogs.microsoft.com/cppblog/msvc-preprocessor-progress-towards-conformance/
#if 1
	#define PP_CONFORMANCE_CHECK_IMPL(first, ...)  #first
	#define PP_CONFORMANCE_CHECK(...) PP_CONFORMANCE_CHECK_IMPL(__VA_ARGS__)
	static_assert(sizeof(PP_CONFORMANCE_CHECK(0, 0)) == 2, "add msvc flag /Zc:preprocessor");
	#undef PP_CONFORMANCE_CHECK
	#undef PP_CONFORMANCE_CHECK_IMPL
#else//clang-cl defines _MSVC_TRADITIONAL as well, but ignores /Zc:preprocessor
	#if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
		#error add msvc flag /Zc:preprocessor
	#endif
#endif