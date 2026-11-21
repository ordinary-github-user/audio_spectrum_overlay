#pragma once
#include "ConceptEnumFlag.h"
#include "Consteval.h"
#include "MacroForEach.h"
#include "Pragma.h"
#include "TStringImpl.h"
#include "Underlying.h"
#include <array>
#define DECL_ENUM_ENTRY_VALUE(name,...)name __VA_OPT__(= __VA_ARGS__)
#define DECL_ENUM_ENTRY(ns,name,...)ns::name
#define ENUM_PRINT_CASE(ns,name,...)case ns::name:os<<TS(#name);break;

#define DEFINE_ENUM_IMPL(friend,name,T,...)enum class name:T{FOR_EACH_DEPAREN_COMMA(DECL_ENUM_ENTRY_VALUE, __VA_ARGS__)};CONSTEVAL friend auto enum_entries(name)noexcept{return std::array{FOR_EACH_DEPAREN_COMMA_EX(DECL_ENUM_ENTRY,name, __VA_ARGS__)};}CONSTEVAL friend void is_enum(name)noexcept;
#define DEFINE_ENUM(...) DEFINE_ENUM_IMPL(,__VA_ARGS__)
#define DEFINE_ENUM_(...)PRAGMA_GCC(GCC diagnostic push)PRAGMA_GCC(GCC diagnostic ignored "-Wnon-template-friend")DEFINE_ENUM_IMPL(friend,__VA_ARGS__)PRAGMA_GCC(GCC diagnostic pop)

#define DEFINE_ENUM_PRINTABLE_IMPL(friend,name,T,...) DEFINE_ENUM_IMPL(friend,name,T,__VA_ARGS__)\
template<class Stream>friend constexpr Stream& operator<<(Stream&os, name e)noexcept             \
{                                                                                                \
    switch(e)                                                                                    \
    {                                                                                            \
        FOR_EACH_DEPAREN_EX(ENUM_PRINT_CASE,name,__VA_ARGS__)                                    \
        default:os<<UNDERLYING(e);                                                               \
    }                                                                                            \
    return os;                                                                                   \
}

#define DEFINE_ENUM_PRINTABLE(...) DEFINE_ENUM_PRINTABLE_IMPL(,__VA_ARGS__)
#define DEFINE_ENUM_PRINTABLE_(...) DEFINE_ENUM_PRINTABLE_IMPL(friend,__VA_ARGS__)

#define ENUM(name,T) enum class name:T;CONSTEVAL void is_enum(name)noexcept;enum class name:T
#define ENUM_(name,T) enum class name:T;CONSTEVAL friend void is_enum(name)noexcept;enum class name:T
#define FLAG(name,T) enum class name:T;CONSTEVAL void is_flag(name)noexcept;enum class name:T
#define FLAG_(name,T) enum class name:T;CONSTEVAL friend void is_flag(name)noexcept;enum class name:T