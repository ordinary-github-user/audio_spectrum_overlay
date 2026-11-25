#pragma once
#include "AggregateImpl.h"
#include "Concepts/CArray.h"
#include "Constant.h"
#include "DataStructures/TStringImpl.h"
#include "IndexOf.h"
#include "Macro/ArgCount.h"
#include "Macro/ForEach.h"
#include "Traits/Like.h"
#include "TypeAt.h"
#define REFLECTION_MEMBER(type,name,...) DEPAREN(type) name __VA_ARGS__;
#define REFLECTION_TYPE(type,name,...) ,DEPAREN(type)
#define REFLECTION_NAME(type,name,...) ,decltype(TS(#name))
#define REFLECTION_ACCESSOR(type,name,...) template<class Self> constexpr Like<Self, DEPAREN(type)> get(this Self&& self, decltype(TS(#name)))noexcept{return FWD(self).name;}
#define REFLECTION_ACTION(type,name,...) FWD(action)(FWD(self).name);
#define REFLECTION_ACTION_INDEXED(type,name,...) FWD(action)(FWD(self).name,IV<member_index_of<#name>>);
#define REFLECTION_ACTION_NAMED(type,name,...) FWD(action)(FWD(self).name,TS(#name));

#define REFLECTION_MEMBERS(...)																																				\
FOR_EACH_TUPLE(REFLECTION_MEMBER,__VA_ARGS__)																																\
static constexpr Compact<ARG_COUNT(__VA_ARGS__)> member_count{ARG_COUNT(__VA_ARGS__)};																						\
template<auto I>requires(I<member_count)using member_type = TYPE_AT<I FOR_EACH_TUPLE(REFLECTION_TYPE,__VA_ARGS__)>;															\
template<auto I>requires(I<member_count)static constexpr auto member_name { TYPE_AT<I FOR_EACH_TUPLE(REFLECTION_NAME,__VA_ARGS__)>{}};										\
template<FixedString name>static constexpr auto member_index_of { index_of_type<std::remove_const_t<decltype(tstring<name>)> FOR_EACH_TUPLE(REFLECTION_NAME,__VA_ARGS__)>};	\
FOR_EACH_TUPLE(REFLECTION_ACCESSOR,__VA_ARGS__)																																\
template<auto I, class Self>requires(I<member_count)INLINE constexpr Like<Self, member_type<I>> member_at(this Self&& self)noexcept{return FWD(self).get(member_name<I>);}	\
template<class Self>INLINE constexpr void for_each_member(this Self&& self, auto&& action)noexcept{FOR_EACH_TUPLE(REFLECTION_ACTION,__VA_ARGS__)}							\
template<class Self>INLINE constexpr void for_each_member_indexed(this Self&& self, auto&& action)noexcept{FOR_EACH_TUPLE(REFLECTION_ACTION_INDEXED,__VA_ARGS__)}			\
template<class Self>INLINE constexpr void for_each_member_named(this Self&& self, auto&& action)noexcept{FOR_EACH_TUPLE(REFLECTION_ACTION_NAMED,__VA_ARGS__)}

#define BITFIELD_MEMBER(type,name,size) DEPAREN(type) name:size;
#define BITFIELD_TYPE(type,name,size) ,DEPAREN(type)
#define BITFIELD_NAME(type,name,size) ,decltype(TS(#name))
#define BITFIELD_ACCESSOR(type,name,size) constexpr DEPAREN(type) get(decltype(TS(#name)))const noexcept{return name;}

#define REFLECTION_BITFIELDS_IMPL(struct_start,struct_end,...)																												\
struct_start FOR_EACH_TUPLE(BITFIELD_MEMBER,__VA_ARGS__) struct_end																											\
static constexpr Compact<ARG_COUNT(__VA_ARGS__)> member_count{ARG_COUNT(__VA_ARGS__)};																						\
template<auto I>requires(I<member_count)using member_type = TYPE_AT<I FOR_EACH_TUPLE(BITFIELD_TYPE,__VA_ARGS__)>;															\
template<auto I>requires(I<member_count)static constexpr auto member_name { TYPE_AT<I FOR_EACH_TUPLE(BITFIELD_NAME,__VA_ARGS__)>{}};										\
template<FixedString name>static constexpr auto member_index_of { index_of_type<std::remove_const_t<decltype(tstring<name>)> FOR_EACH_TUPLE(BITFIELD_NAME,__VA_ARGS__)>};	\
FOR_EACH_TUPLE(BITFIELD_ACCESSOR,__VA_ARGS__)																																\
template<auto I>requires(I<member_count)INLINE constexpr member_type<I> member_at()const noexcept{return get(member_name<I>);}

#define REFLECTION_BITFIELDS_ANONYMOUS(...) REFLECTION_BITFIELDS_IMPL(struct{,};,__VA_ARGS__)
#define REFLECTION_BITFIELDS(...) REFLECTION_BITFIELDS_IMPL(,,__VA_ARGS__)

template<class T>concept $ManualReflection = requires{ usize{ std::remove_cvref_t<T>::member_count }; };
template<class T>concept $Reflection = $ManualReflection<T> || ($Aggregate<T> && !$CArray<T>);

namespace details
{
	template<class, usize>struct NameImpl;

	template<$ManualReflection T, usize Index>
	struct NameImpl<T, Index>
	{
		static constexpr auto value{ T::template member_name<Index> };
	};

	template<$Aggregate T, usize Index>
	requires(!$ManualReflection<T>)
	struct NameImpl<T, Index>
	{
		static constexpr auto value{ tstring<Aggregate::name<T, Index>> };
	};

	template<class>struct SizeImpl;

	template<$ManualReflection T>
	struct SizeImpl<T>
	{
		static constexpr auto value{ T::member_count };
	};

	template<$Aggregate T>
	requires(!$ManualReflection<T>)
	struct SizeImpl<T>
	{
		static constexpr auto value{ Aggregate::size<T> };
	};
}

namespace Reflection
{
	template<class T, usize Index>
	constexpr auto name{ details::NameImpl<std::remove_cvref_t<T>, Index>::value };

	template<class T>
	constexpr auto size{ details::SizeImpl<std::remove_cvref_t<T>>::value };

	template<usize Index, $Aggregate T>
	requires(!$ManualReflection<T>)
	INLINE constexpr decltype(auto) at(T&& x)noexcept
	{
#if __cpp_structured_bindings >= 202411
		[[maybe_unused]] auto&& [...xs] = x;
		return static_cast<Like<decltype(x), decltype(xs...[Index])>>(xs...[Index]);
#else
		return details::AtImpl<Aggregate::size<std::remove_cvref_t<T>>, Index>::operator()(FWD(x));
#endif
	};

	template<usize Index>
	INLINE constexpr decltype(auto) at($ManualReflection auto&& x)noexcept
	{
		return FWD(x).template member_at<Index>();
	};

	template<class T, usize Index>
	using type_at = std::remove_cvref_t<decltype(at<Index>(std::declval<T>()))>;
}