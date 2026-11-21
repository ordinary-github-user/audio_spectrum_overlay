#pragma once
#include "AlgorithmMinMax.h"
#include "BasicTypes.h"
#include "Compiler.h"
#include "GcdLcm.h"
#include "MacroArgCount.h"
#include "MacroConcat.h"
#include "MacroEmpty.h"
#include "MacroForEach.h"
#include "MacroIf.h"
#include "MacroParenthesized.h"
#include "NextAligned.h"
#include "NoUniqueAddress.h"
#include <array>
#define DECL_PADDED_STRUCT_OFFSET(offset,...) offset
#define DECL_PADDED_STRUCT_SIZE(offset,name,...) sizeof(__VA_ARGS__)
#define DECL_PADDED_STRUCT_END_OFFSET(offset,name,...) offset+sizeof(__VA_ARGS__)
#define DECL_PADDED_STRUCT_ALIGN(offset,name,...) alignof(__VA_ARGS__)
#define DECL_PADDED_STRUCT_ASSERT_ALIGN(offset,name,...) STATIC_ASSERT((offset) % alignof(__VA_ARGS__) == 0, "field mis-aligned: " #name);

#if MSVC
	template<usize N, auto>struct StructPadding { u8 _[N]; };
	template<auto V>struct StructPadding<0, V> {};
	#define DECL_PADDED_STRUCT_ENTRY(offset,name,...) private: NO_UNIQUE_ADDRESS [[maybe_unused]]StructPadding<_paddings<offset>,[]{}> _ ## name ## _padding;_Pragma("pack(push,1)")public:__VA_ARGS__ name;_Pragma("pack(pop)")
#else
	#define DECL_PADDED_STRUCT_ENTRY(offset,name,...) private: NO_UNIQUE_ADDRESS [[maybe_unused]]u8 _ ## name ## _padding[_paddings<offset>];public:__attribute__((packed)) __VA_ARGS__ name;
#endif

#if GCC || CLANG
	#define DISABLE_WARNING_ATTRIBUTE _Pragma("GCC diagnostic push")_Pragma("GCC diagnostic ignored \"-Wattributes\"")//'packed' attribute ignored for field of type '???' [-Wattributes]
	#define ENABLE_WARNING_ATTRIBUTE  _Pragma("GCC diagnostic pop")
	#define DISABLE_WARNING_OFFSETOF  _Pragma("GCC diagnostic push")_Pragma("GCC diagnostic ignored \"-Winvalid-offsetof\"")//offset of on non-standard-layout type '???' [-Winvalid-offsetof]
	#define ENABLE_WARNING_OFFSETOF   _Pragma("GCC diagnostic pop")
#else
	#define DISABLE_WARNING_ATTRIBUTE
	#define ENABLE_WARNING_ATTRIBUTE
	#define DISABLE_WARNING_OFFSETOF
	#define ENABLE_WARNING_OFFSETOF
#endif

template<usize X, usize...Xs>
requires(((X == Xs) + ...) == 1)
constexpr usize index_of_offset{ []static
{
	usize result{};
	(void)(((X != Xs) ? (++result, true) : false) && ...);
	return result;
}() };

#define PROPERTIES(...) __VA_OPT__(FOR_EACH_TUPLE(DECL_PADDED_STRUCT_ASSERT_ALIGN, __VA_ARGS__) private:template<usize offset> static constexpr usize _paddings{(::index_of_offset<offset,FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_OFFSET,__VA_ARGS__)> == 0) ? std::array<usize,ARG_COUNT(__VA_ARGS__)>{FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_OFFSET,__VA_ARGS__)}[0] : std::array<usize,ARG_COUNT(__VA_ARGS__)>{FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_OFFSET,__VA_ARGS__)}[::index_of_offset<offset,FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_OFFSET,__VA_ARGS__)>] - std::array<usize,ARG_COUNT(__VA_ARGS__)>{FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_OFFSET,__VA_ARGS__)}[::index_of_offset<offset,FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_OFFSET,__VA_ARGS__)> - u8{1}] - std::array<usize,ARG_COUNT(__VA_ARGS__)>{FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_SIZE,__VA_ARGS__)}[::index_of_offset<offset,FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_OFFSET,__VA_ARGS__)> - u8{1}]};public:DISABLE_WARNING_ATTRIBUTE FOR_EACH_TUPLE(DECL_PADDED_STRUCT_ENTRY,__VA_ARGS__) ENABLE_WARNING_ATTRIBUTE)

#if MSVC
	#define DECL_PADDED_STRUCT_FINAL_PADDING(size,...)private: NO_UNIQUE_ADDRESS [[maybe_unused]]StructPadding<size __VA_OPT__(-Algorithm::max(FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_END_OFFSET,__VA_ARGS__))),[]{}> _final_padding;
#else
	#define DECL_PADDED_STRUCT_FINAL_PADDING(size,...)private: NO_UNIQUE_ADDRESS [[maybe_unused]]u8 _final_padding[size __VA_OPT__(-Algorithm::max(FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_END_OFFSET,__VA_ARGS__)))];
#endif

#define PADDED_STRUCT_ASSERT_OFFSET(struct_name,offset,name,...) STATIC_ASSERT(offset == __builtin_offsetof(struct_name,name),#struct_name "." #name " offset mismatch");

#define STRUCT_IMPL0(name,align,size,...) struct alignas(align) name{PROPERTIES(__VA_ARGS__)DECL_PADDED_STRUCT_FINAL_PADDING(size __VA_OPT__(,__VA_ARGS__))};STATIC_ASSERT(size==sizeof(name),"sizeof(" #name ") != " #size);DISABLE_WARNING_OFFSETOF FOR_EACH_TUPLE_EX(PADDED_STRUCT_ASSERT_OFFSET,name,__VA_ARGS__)ENABLE_WARNING_OFFSETOF
#define STRUCT_IMPL1(name,align,...) struct alignas(align) name{PROPERTIES(__VA_ARGS__)};STATIC_ASSERT(sizeof(name)==next_aligned_greater_equal<align>(Algorithm::max(FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_END_OFFSET,__VA_ARGS__))),"sizeof(" #name ") mismatch");DISABLE_WARNING_OFFSETOF FOR_EACH_TUPLE_EX(PADDED_STRUCT_ASSERT_OFFSET,name,__VA_ARGS__)ENABLE_WARNING_OFFSETOF
#define STRUCT0(name,align,arg,...) CAT(STRUCT_IMPL,PARENTHESIZED(arg))(name,align,arg __VA_OPT__(,__VA_ARGS__))
#define STRUCT1(name,align,...) struct alignas(align) name{__VA_OPT__(DECL_PADDED_STRUCT_FINAL_PADDING(__VA_ARGS__))};__VA_OPT__(STATIC_ASSERT(sizeof(name)==__VA_ARGS__);)
#define STRUCT_UNALIGNED(name,...) CAT(STRUCT,EMPTY(__VA_ARGS__))(name,1 __VA_OPT__(,__VA_ARGS__))
#define STRUCT_ALIGNED(name,align,...) CAT(STRUCT,EMPTY(__VA_ARGS__))(name,align __VA_OPT__(,__VA_ARGS__))

#define STRUCT_AUTO_ALIGNED_IMPL0(name,size,...) CAT(STRUCT,EMPTY(__VA_ARGS__))(name,IF(EMPTY(__VA_ARGS__),1,Math::lcm(FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_ALIGN,__VA_ARGS__))),size __VA_OPT__(,__VA_ARGS__))
#define STRUCT_AUTO_ALIGNED_IMPL1(name,...) CAT(STRUCT,EMPTY(__VA_ARGS__))(name,IF(EMPTY(__VA_ARGS__),1,Math::lcm(FOR_EACH_TUPLE_COMMA(DECL_PADDED_STRUCT_ALIGN,__VA_ARGS__))) __VA_OPT__(,__VA_ARGS__))
#define STRUCT_AUTO_ALIGNED0(name,arg,...) CAT(STRUCT_AUTO_ALIGNED_IMPL,PARENTHESIZED(arg))(name,arg __VA_OPT__(,__VA_ARGS__))
#define STRUCT_AUTO_ALIGNED1(name) struct alignas(1) name{};
#define STRUCT_AUTO_ALIGNED(name,...) CAT(STRUCT_AUTO_ALIGNED,EMPTY(__VA_ARGS__))(name __VA_OPT__(,__VA_ARGS__))

#if 0
#include <array>
#include <meta>
#include <string_view>
#include <algorithm>
struct PaddedMember
{
	usize offset;
	std::meta::info type;
	std::string_view name;
};
template<class T, usize...totalSize>
requires(sizeof...(totalSize) <= 1)
consteval std::meta::info define_padded(std::initializer_list<PaddedMember> members)
{
	for (const auto& member : members)
		if (member.offset % alignment_of(member.type))
			__builtin_unreachable();//member misaligned
	if (!std::is_sorted(members.begin(), members.end(), [](const PaddedMember& l, const PaddedMember& r)static{return l.offset < r.offset; }))
		__builtin_unreachable();//members not sorted by offset
	std::vector<std::meta::info> specs;
	specs.reserve(members.size() + 1);
	usize prevEnd{};
	for (const PaddedMember& member : members)
	{
		if (prevEnd > member.offset)__builtin_unreachable();//prev member overlaps current member
		else if (const usize padSize{ member.offset - prevEnd }; padSize)
			specs.push_back(data_member_spec(substitute(^^std::array, { ^^u8,std::meta::reflect_constant(padSize) })));
		specs.push_back(data_member_spec(member.type, { .name{member.name} }));
		prevEnd = member.offset + size_of(member.type);
	}
	if constexpr (sizeof...(totalSize))
	{
		/* */if (prevEnd > (totalSize, ...))__builtin_unreachable();//last member exceed totalSize
		else if (prevEnd < (totalSize, ...))
			specs.push_back(data_member_spec(substitute(^^std::array, { ^^u8,std::meta::reflect_constant((...,totalSize) - prevEnd) })));
	}
	return define_aggregate(^^T, specs);
}
struct ClassInfo;
consteval
{
	define_padded<ClassInfo, 0x30>
	({
		{0x08, ^^u64, "pBinaryName"},
		{0x10, ^^u64, "pModuleName"},
		{0x18, ^^u32, "size"},
		{0x1C, ^^u16, "fieldCount"},
		{0x22, ^^u8 , "alignment"},
		{0x28, ^^u64, "pFields"}
	});
}
static_assert(__builtin_offsetof(ClassInfo, pBinaryName) == 0x08);
static_assert(__builtin_offsetof(ClassInfo, pModuleName) == 0x10);
static_assert(__builtin_offsetof(ClassInfo, size) == 0x18);
static_assert(__builtin_offsetof(ClassInfo, fieldCount) == 0x1C);
static_assert(__builtin_offsetof(ClassInfo, alignment) == 0x22);
static_assert(__builtin_offsetof(ClassInfo, pFields) == 0x28);
static_assert(sizeof(ClassInfo) == 0x30);
#endif