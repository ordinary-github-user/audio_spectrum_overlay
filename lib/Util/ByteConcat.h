#pragma once
#include <array>
#include <immintrin.h>
#include "ByteConcatScalar.h"
#include "Compiler.h"
#include "Concepts/Same.h"
#include "Macro/Concat.h"
#include "Macro/ForEach.h"
#include "Macro/Indexes.h"
namespace Bytes
{
#if GCC
	#define BYTE_CONCAT_DISABLE_WARNING_ATTRIBUTE _Pragma("GCC diagnostic ignored \"-Wignored-attributes\"")//ignoring attributes on template argument '__m128i' [-Wignored-attributes]
	#define BYTE_CONCAT_ENABLE_WARNING_ATTRIBUTE _Pragma("GCC diagnostic warning \"-Wignored-attributes\"")
#else
	#define BYTE_CONCAT_DISABLE_WARNING_ATTRIBUTE
	#define BYTE_CONCAT_ENABLE_WARNING_ATTRIBUTE
#endif

#define BYTE_CONCAT_NAME(i) CAT(_,i)
#define BYTE_CONCAT_TYPE_NAME(ret, i) ret CAT(_,i)
#if MSVC
	#define BYTE_CONCAT_COMPILE_TIME(ret, type, union_entry, n, ...)return ret{.union_entry{FOR_EACH_COMMA(BYTE_CONCAT_NAME,__VA_ARGS__)}};
#else
	#define BYTE_CONCAT_COMPILE_TIME(ret, type, union_entry, n, ...)return __builtin_bit_cast(ret, std::array<type,n>{FOR_EACH_COMMA(BYTE_CONCAT_NAME,__VA_ARGS__)});
#endif
#define DEFINE_BYTE_CONCAT_INDEXES(ret, type, union_entry, n, ...)BYTE_CONCAT_DISABLE_WARNING_ATTRIBUTE template<$Same<ret> T>INLINE constexpr ret concat(FOR_EACH_COMMA_EX(BYTE_CONCAT_TYPE_NAME,type,__VA_ARGS__))noexcept{BYTE_CONCAT_COMPILE_TIME(ret,type,union_entry,n,__VA_ARGS__)}BYTE_CONCAT_ENABLE_WARNING_ATTRIBUTE
#define DEFINE_BYTE_CONCAT(ret, type, union_entry, n)DEFINE_BYTE_CONCAT_INDEXES(ret,type,union_entry,n,INDEXES(n))

	DEFINE_BYTE_CONCAT(__m128i, u8,  m128i_u8,  16)
	DEFINE_BYTE_CONCAT(__m128i, u16, m128i_u16, 8)
	DEFINE_BYTE_CONCAT(__m128i, u32, m128i_u32, 4)
	DEFINE_BYTE_CONCAT(__m128i, u64, m128i_u64, 2)

#if defined(__AVX2__)
	DEFINE_BYTE_CONCAT(__m256i, u8,  m256i_u8,  32)
	DEFINE_BYTE_CONCAT(__m256i, u16, m256i_u16, 16)
	DEFINE_BYTE_CONCAT(__m256i, u32, m256i_u32, 8)
	DEFINE_BYTE_CONCAT(__m256i, u64, m256i_u64, 4)
#endif
#if defined(__AVX512F__)
	DEFINE_BYTE_CONCAT(__m512i, u8,  m512i_u8,  64)
	DEFINE_BYTE_CONCAT(__m512i, u16, m512i_u16, 32)
	DEFINE_BYTE_CONCAT(__m512i, u32, m512i_u32, 16)
	DEFINE_BYTE_CONCAT(__m512i, u64, m512i_u64, 8)
#endif
#undef BYTE_CONCAT_NAME
#undef BYTE_CONCAT_TYPE_NAME
#undef BYTE_CONCAT_COMPILE_TIME
#undef DEFINE_BYTE_CONCAT_INDEXES
#undef DEFINE_BYTE_CONCAT
#undef BYTE_CONCAT_DISABLE_WARNING_ATTRIBUTE
#undef BYTE_CONCAT_ENABLE_WARNING_ATTRIBUTE
}