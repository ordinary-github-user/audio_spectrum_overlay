#pragma once
#include "../Compact.h"
#include "../Compiler.h"
#include "../Concepts/CvrefOf.h"
#include "../Concepts/Same.h"
#include "../IndexOf.h"
#include "../Inline.h"
#include "../Iota.h"
#include "../LikelyUnlikely.h"
#include "../Macro/Concat.h"
#include "../Macro/MovForward.h"
#include "../NoUniqueAddress.h"
#include "../StaticAssert.h"
#include "../Traits/Like.h"
#include "../TypeAt.h"

#if GCC
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
#define TUPLE_CONCEPT(Name)\
	namespace details\
	{\
		template<class>struct CAT(Is, Name) { static constexpr bool value{}; };\
		template<class...Ts>struct CAT(Is, Name)<Name<Ts...>> { static constexpr bool value{true}; };\
	}\
	template<class T>concept CAT($, Name) = details::CAT(Is, Name)<std::remove_cvref_t<T>>::value;

#define TUPLE_STD(Name)\
	namespace std\
	{\
		template<class...Ts>\
		struct tuple_size<Name<Ts...>>\
		{\
			static constexpr auto value{ compact<sizeof...(Ts)> };\
		};\
		template<usize index, class...Ts>\
		struct tuple_element<index, Name<Ts...>>\
		{\
			using type = TYPE_AT<index, Ts...>;\
		};\
	}

#define TUPLE_TESTS(Name)\
	STATIC_ASSERT(__is_aggregate(Name<u8, u16, u32, u64>));\
	STATIC_ASSERT(__is_trivially_copyable(Name<u8, u16, u32, u64>));\
	STATIC_ASSERT(Name{} + Name{} == Name{});\
	STATIC_ASSERT([]static{Name a{};return (a += a) == a;}());\
	STATIC_ASSERT([]static{Name a{};return (a += Name{}) == Name{};}());\
	STATIC_ASSERT([]static{Name a{100.,12.};return (a/=Name{5,3}) == Name{20.,4.};}());\
	STATIC_ASSERT([]static{Name a{0,1,12};return (a%=Name{5,5,5}) == Name{0,1,2};}());\
	STATIC_ASSERT(Name{} <= Name{});\
	STATIC_ASSERT(Name{} >= Name{});\
	STATIC_ASSERT(!(Name{} < Name{}));\
	STATIC_ASSERT(!(Name{} > Name{}));\
	STATIC_ASSERT(Name{1, 2.} < Name{2, 2.});\
	STATIC_ASSERT(Name{1, 2.} < Name{1, 3.});\
	STATIC_ASSERT(!(Name{1, 4.} < Name{1, 3.}));\
	STATIC_ASSERT(!(Name{0, 9.} < Name{0, 9.}));\
	STATIC_ASSERT(Name{0, 9.} < Name{1, 0.});\
	STATIC_ASSERT(Name{1, 2.} <= Name{1, 2.});\
	STATIC_ASSERT(Name{1, 2.} <= Name{1, 3.});\
	STATIC_ASSERT(!(Name{1, 4.} <= Name{1, 3.}));\
	STATIC_ASSERT(Name{0, 9.} <= Name{0, 9.});\
	STATIC_ASSERT(Name{0, 9.} <= Name{1, 0.});\
	STATIC_ASSERT(Name{ 1,1.234f, 1234. } - Name{ -1, -1.234f, -1234. } == Name{ 2, 2.468f, 2468. });\
	STATIC_ASSERT((!Name{ true, false, 123, 0 }) == Name{ false, true, false, true });\
	STATIC_ASSERT([]static\
	{\
		const Name x{ 1,1.234f,false };\
		if constexpr (!$Same<Name<int, float, bool>, std::remove_cvref_t<decltype(x)>>)return false;\
		if constexpr (!__is_aggregate(std::remove_cvref_t<decltype(x)>))return false;\
		if (get<0>(x) != 1 || get<int>(x) != 1)return false;\
		if (get<1>(x) != 1.234f || get<float>(x) != 1.234f)return false;\
		if (get<2>(x) != false || get<bool>(x) != false)return false;\
		return true;\
	}());\
	STATIC_ASSERT([]static\
	{\
		const i32 a{1};\
		const i32 b{2};\
		const Name<i32 const&> l{ a };\
		const Name<i32 const&> r{ b };\
		if(l + r != Name{3})return false;\
		return true;\
	}());\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<int>>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const int>>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<volatile int>>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const volatile int>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<int&>>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const int&>>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<volatile int&>>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const volatile int&>>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<int&&>>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const int&&>>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<volatile int&&>>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const volatile int&&>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<int>>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const int>>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<volatile int>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const volatile int>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<int&>>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const int&>>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<volatile int&>>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const volatile int&>>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<int&&>>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const int&&>>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<volatile int&&>>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const volatile int&&>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<int>>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const int>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<volatile int>>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const volatile int>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<int&>>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const int&>>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<volatile int&>>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const volatile int&>>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<int&&>>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const int&&>>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<volatile int&&>>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const volatile int&&>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<int>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const int>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<volatile int>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const volatile int>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<int&>>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const int&>>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<volatile int&>>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const volatile int&>>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<int&&>>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const int&&>>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<volatile int&&>>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const volatile int&&>>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<int>&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const int>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<volatile int>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const volatile int>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<int&>&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const int&>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<volatile int&>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const volatile int&>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<int&&>&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const int&&>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<volatile int&&>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const volatile int&&>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<int>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const int>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<volatile int>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const volatile int>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<int&>&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const int&>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<volatile int&>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const volatile int&>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<int&&>&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const int&&>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<volatile int&&>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const volatile int&&>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<int>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const int>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<volatile int>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const volatile int>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<int&>&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const int&>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<volatile int&>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const volatile int&>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<int&&>&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const int&&>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<volatile int&&>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const volatile int&&>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<int>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const int>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<volatile int>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const volatile int>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<int&>&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const int&>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<volatile int&>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const volatile int&>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<int&&>&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const int&&>&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<volatile int&&>&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const volatile int&&>&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<int>&&>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const int>&&>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<volatile int>&&>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const volatile int>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<int&>&&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const int&>&&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<volatile int&>&&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const volatile int&>&&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<int&&>&&>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const int&&>&&>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<volatile int&&>&&>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<Name<const volatile int&&>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<int>&&>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const int>&&>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<volatile int>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const volatile int>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<int&>&&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const int&>&&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<volatile int&>&&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const volatile int&>&&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<int&&>&&>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const int&&>&&>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<volatile int&&>&&>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const Name<const volatile int&&>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<int>&&>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const int>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<volatile int>&&>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const volatile int>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<int&>&&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const int&>&&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<volatile int&>&&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const volatile int&>&&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<int&&>&&>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const int&&>&&>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<volatile int&&>&&>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<volatile Name<const volatile int&&>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<int>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const int>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<volatile int>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const volatile int>&&>())), const volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<int&>&&>())), int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const int&>&&>())), const int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<volatile int&>&&>())), volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const volatile int&>&&>())), const volatile int&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<int&&>&&>())), int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const int&&>&&>())), const int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<volatile int&&>&&>())), volatile int&&>);\
	STATIC_ASSERT($Same<decltype(get<0>(std::declval<const volatile Name<const volatile int&&>&&>())), const volatile int&&>);

#if META && TEMPLATE_FOR && VARIADIC_STRUCTURED_BINDING
	#include <meta>
	#define TUPLE_COMPARE_OPERATOR(op)\
	template<class Self>INLINE constexpr bool operator op(this Self&& self, auto&& r)noexcept\
	requires([]static{static constexpr auto [...I]{iota<sizeof...(Ts)>};return std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(self) op get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) != get<I>(r)}->$Same<bool>;});}())\
	{\
		if constexpr(sizeof...(Ts) == 0)return false;\
		else\
		{\
			template for (constexpr auto J : iota<sizeof...(Ts) - 1>) { if (get<J>(self) != get<J>(r))[[likely]]return get<J>(self) op get<J>(r); }\
			return get<sizeof...(Ts) - 1>(self) op get<sizeof...(Ts) - 1>(r);\
		}\
	}
	#define TUPLE_COMPARE_OPERATOR_EQUAL(op, op_eq)\
	template<class Self>INLINE constexpr bool operator op_eq(this Self&& self, auto&& r)noexcept\
	requires([]static{static constexpr auto [...I]{iota<sizeof...(Ts)>};return std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(self) op_eq get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) op get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) != get<I>(r)}->$Same<bool>;});}())\
	{\
		if constexpr(sizeof...(Ts) == 0)return true;\
		else\
		{\
			template for(constexpr auto J: iota<sizeof...(Ts) - 1>){if(get<J>(self) != get<J>(r))[[likely]]return get<J>(self) op get<J>(r);}\
			return get<sizeof...(Ts) - 1>(self) op_eq get<sizeof...(Ts) - 1>(r);\
		}\
	}
	#define TUPLE_UNARY_OPERATOR(Name, op)template<class Self>INLINE constexpr auto operator op(this Self&& self)noexcept requires([]static{static constexpr auto [...I]{iota<sizeof...(Ts)>};return (...&&requires{{op get<I>(FWD(self))}->$Not<void>;});}()){static constexpr auto [...I]{iota<sizeof...(Ts)>};return Name<std::remove_cvref_t<decltype(op get<I>(FWD(self)))>...>{(op get<I>(FWD(self)))...};}
	#define TUPLE_BINARY_OPERATOR(Name, op)template<class Self>INLINE constexpr auto operator op(this Self&& self, auto&& r)noexcept requires([]static{static constexpr auto [...I]{iota<sizeof...(Ts)>};return std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(FWD(self)) op get<I>(FWD(r))}->$Not<void>;});}()){static constexpr auto [...I]{iota<sizeof...(Ts)>};return Name<std::remove_cvref_t<decltype(get<I>(FWD(self)) op get<I>(FWD(r)))>...>{(get<I>(FWD(self)) op get<I>(FWD(r)))...};}
	#define TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, op)template<class Self>INLINE constexpr Like<Self, Self> operator op(this Self&& self, auto&& r)noexcept requires([]static{static constexpr auto [...I]{iota<sizeof...(Ts)>};return std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{get<I>(FWD(self)) op get<I>(FWD(r));});}()){static constexpr auto [...I]{iota<sizeof...(Ts)>};(...,(get<I>(FWD(self)) op get<I>(FWD(r))));return FWD(self);}
	#define TUPLE_IMPL(Name)\
	template<class...Ts>\
	struct Name\
	{\
		struct Impl;\
		consteval{define_aggregate(^^Impl, { data_member_spec(^^Ts,{.name{"_"},.no_unique_address{true}})...});}\
		Impl impl;\
		template<class T, class Self>requires((usize{} + ... + $Same<Ts,T>) == 1 && $Same<Name, std::remove_cvref_t<Self>>) friend constexpr LikeImpl<Self, T>::type get(Self&& self)noexcept{ return FWD(self).impl.[:std::meta::nonstatic_data_members_of(^^Impl, std::meta::access_context::unchecked())[index_of_type<T, Ts...>]:]; }\
		template<usize index, class Self>requires(index < sizeof...(Ts) && $Same<Name, std::remove_cvref_t<Self>>) friend constexpr LikeImpl<Self, TYPE_AT<index, Ts...>>::type get(Self&& self)noexcept { return FWD(self).impl.[:std::meta::nonstatic_data_members_of(^^Impl, std::meta::access_context::unchecked())[index]:]; }\
		template<class Stream>\
		requires((...&&requires(Stream& os, const Ts x) { os << x; }))\
		friend constexpr Stream& operator<<(Stream& os, Name const& tuple)noexcept\
		{\
			os << '<';\
			template for(constexpr auto I: iota<sizeof...(Ts)>)\
			{\
				os << get<I>(tuple);\
				if constexpr (I != sizeof...(Ts) - 1)\
					os << ',';\
			}\
			os << '>';\
			return os;\
		}\
		TUPLE_UNARY_OPERATOR(Name, +)\
		TUPLE_UNARY_OPERATOR(Name, -)\
		TUPLE_UNARY_OPERATOR(Name, !)\
		TUPLE_UNARY_OPERATOR(Name, ~)\
		TUPLE_BINARY_OPERATOR(Name, +)\
		TUPLE_BINARY_OPERATOR(Name, -)\
		TUPLE_BINARY_OPERATOR(Name, *)\
		TUPLE_BINARY_OPERATOR(Name, /)\
		TUPLE_BINARY_OPERATOR(Name, %)\
		TUPLE_BINARY_OPERATOR(Name, &)\
		TUPLE_BINARY_OPERATOR(Name, |)\
		TUPLE_BINARY_OPERATOR(Name, ^)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, +=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, -=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, *=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, /=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, %=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, &=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, |=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, ^=)\
		TUPLE_COMPARE_OPERATOR(<)\
		TUPLE_COMPARE_OPERATOR(>)\
		TUPLE_COMPARE_OPERATOR_EQUAL(<, <=)\
		TUPLE_COMPARE_OPERATOR_EQUAL(>, >=)\
		template<class Self>INLINE constexpr bool operator==(this Self&& self, auto&& r)noexcept requires([]static{static constexpr auto [...I]{iota<sizeof...(Ts)>};return std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(FWD(self)) == get<I>(FWD(r))}->$Same<bool>;});}()){static constexpr auto [...I]{iota<sizeof...(Ts)>};return (...&&(get<I>(FWD(self)) == get<I>(FWD(r))));}\
		template<class Self>INLINE constexpr bool operator!=(this Self&& self, auto&& r)noexcept requires([]static{static constexpr auto [...I]{iota<sizeof...(Ts)>};return std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(FWD(self)) != get<I>(FWD(r))}->$Same<bool>;});}()){static constexpr auto [...I]{iota<sizeof...(Ts)>};return (...||(get<I>(FWD(self)) != get<I>(FWD(r))));}\
	};\
	template<class...Ts>Name(Ts...) -> Name<Ts...>;\
	TUPLE_CONCEPT(Name)\
	TUPLE_STD(Name)\
	TUPLE_TESTS(Name)
#else
	#if CLANG
		#pragma clang diagnostic ignored "-Wmissing-braces"//clang wants Tuple{{123},{"asdf"}} instead of Tuple{123,"asdf"}
	#endif
	#if TEMPLATE_FOR
		#define TUPLE_COMPARE_OPERATOR(op)\
		template<class Self>INLINE constexpr bool operator op(this Self&& self, auto&& r)noexcept\
		requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(self) op get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) != get<I>(r)}->$Same<bool>;}))\
		{\
			if constexpr(sizeof...(Ts) == 0)return false;\
			else\
			{\
				template for (constexpr auto J : iota<sizeof...(Ts) - 1>) { if (get<J>(self) != get<J>(r))[[likely]]return get<J>(self) op get<J>(r); }\
				return get<sizeof...(Ts) - 1>(self) op get<sizeof...(Ts) - 1>(r);\
			}\
		}
	#elif VARIADIC_STRUCTURED_BINDING
		#define TUPLE_COMPARE_OPERATOR(op)\
		template<class Self>INLINE constexpr bool operator op(this Self&& self, auto&& r)noexcept\
		requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(self) op get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) != get<I>(r)}->$Same<bool>;}))\
		{\
			/* */if constexpr(sizeof...(Ts) == 0)return false;\
			else if constexpr(sizeof...(Ts) == 1)return get<0>(self) op get<0>(r);\
			else\
			{\
				static constexpr auto [...J] {iota<sizeof...(Ts) - 1>};\
				bool result;\
				return (...&&\
				(\
					LIKELY(get<J>(self) != get<J>(r))?\
					((result = (get<J>(self) op get<J>(r))),false):\
					true\
				))? get<sizeof...(Ts) - 1>(self) op get<sizeof...(Ts) - 1>(r) :result;\
			}\
		}
	#else
		#define TUPLE_COMPARE_OPERATOR(op)\
		template<class Self>INLINE constexpr bool operator op(this Self&& self, auto&& r)noexcept\
		requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(self) op get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) != get<I>(r)}->$Same<bool>;}))\
		{\
			/* */if constexpr(sizeof...(Ts) == 0)return false;\
			else if constexpr(sizeof...(Ts) == 1)return get<0>(self) op get<0>(r);\
			else\
			{\
				static constexpr auto impl{[]<usize...J>(std::index_sequence<J...>)static\
				{\
					return[](auto&& l, auto&& r)STATIC_LAMBDA_INLINE\
					{\
						bool result;\
						return (...&&\
						(\
							LIKELY(get<J>(l) != get<J>(r))?\
							((result = (get<J>(l) op get<J>(r))),false):\
							true\
						))? get<sizeof...(Ts) - 1>(l) op get<sizeof...(Ts) - 1>(r) :result;\
					};\
				}(std::make_index_sequence<sizeof...(Ts) - 1>{})};\
				return impl(FWD(self), FWD(r));\
			}\
		}
	#endif
	#if TEMPLATE_FOR
		#define TUPLE_COMPARE_OPERATOR_EQUAL(op, op_eq)\
		template<class Self>INLINE constexpr bool operator op_eq(this Self&& self, auto&& r)noexcept\
		requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(self) op_eq get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) op get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) != get<I>(r)}->$Same<bool>;}))\
		{\
			if constexpr(sizeof...(Ts) == 0)return true;\
			else\
			{\
				template for(constexpr auto J: iota<sizeof...(Ts) - 1>){if(get<J>(self) != get<J>(r))[[likely]]return get<J>(self) op get<J>(r);}\
				return get<sizeof...(Ts) - 1>(self) op_eq get<sizeof...(Ts) - 1>(r);\
			}\
		}
	#elif VARIADIC_STRUCTURED_BINDING
		#define TUPLE_COMPARE_OPERATOR_EQUAL(op, op_eq)\
		template<class Self>INLINE constexpr bool operator op_eq(this Self&& self, auto&& r)noexcept\
		requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(self) op_eq get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) op get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) != get<I>(r)}->$Same<bool>;}))\
		{\
			/* */if constexpr(sizeof...(Ts) == 0)return true;\
			else if constexpr(sizeof...(Ts) == 1)return get<0>(self) op_eq get<0>(r);\
			else\
			{\
				static constexpr auto [...J] {iota<sizeof...(Ts) - 1>};\
				bool result;\
				return (...&&\
				(\
					LIKELY(get<J>(self) != get<J>(r))?\
					((result = (get<J>(self) op get<J>(r))),false):\
					true\
				))? get<sizeof...(Ts) - 1>(self) op_eq get<sizeof...(Ts) - 1>(r) :result;\
			}\
		}
	#else
		#define TUPLE_COMPARE_OPERATOR_EQUAL(op, op_eq)\
		template<class Self>INLINE constexpr bool operator op_eq(this Self&& self, auto&& r)noexcept\
		requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(self) op_eq get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) op get<I>(r)}->$Same<bool>;}) && (...&&requires{{get<I>(self) != get<I>(r)}->$Same<bool>;}))\
		{\
			/* */if constexpr(sizeof...(Ts) == 0)return true;\
			else if constexpr(sizeof...(Ts) == 1)return get<0>(self) op_eq get<0>(r);\
			else\
			{\
				static constexpr auto impl{[]<usize...J>(std::index_sequence<J...>)static\
				{\
					return[](auto&& l, auto&& r)STATIC_LAMBDA_INLINE\
					{\
						bool result;\
						return (...&&\
						(\
							LIKELY(get<J>(l) != get<J>(r))?\
							((result = (get<J>(l) op get<J>(r))),false):\
							true\
						))? get<sizeof...(Ts) - 1>(l) op_eq get<sizeof...(Ts) - 1>(r) :result;\
					};\
				}(std::make_index_sequence<sizeof...(Ts) - 1>{})};\
				return impl(FWD(self), FWD(r));\
			}\
		}
	#endif
	#define TUPLE_UNARY_OPERATOR(Name, op)template<class Self>INLINE constexpr auto operator op(this Self&& self)noexcept requires((...&&requires{{op get<I>(FWD(self))}->$Not<void>;})){return Name{(op get<I>(FWD(self)))...};}
	#define TUPLE_BINARY_OPERATOR(Name, op)template<class Self>INLINE constexpr auto operator op(this Self&& self, auto&& r)noexcept requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(FWD(self)) op get<I>(FWD(r))}->$Not<void>;})){return Name{(get<I>(FWD(self)) op get<I>(FWD(r)))...};}
	#define TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, op)template<class Self>INLINE constexpr Like<Self, Self> operator op(this Self&& self, auto&& r)noexcept requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{get<I>(FWD(self)) op get<I>(FWD(r));})){(...,(get<I>(FWD(self)) op get<I>(FWD(r))));return FWD(self);}

	#define TUPLE_IMPL(Name)\
	template<class T, usize Index>\
	struct CAT(Name, Element)\
	{\
		NO_UNIQUE_ADDRESS T value;\
		INLINE friend constexpr bool operator==(CAT(Name, Element) const&,CAT(Name, Element) const&)noexcept = default;\
		INLINE friend constexpr bool operator!=(CAT(Name, Element) const&,CAT(Name, Element) const&)noexcept = default;\
	};\
	template<class...>struct CAT(Name, Impl);\
	template<class...Ts>using Name = CAT(Name, Impl)<std::index_sequence_for<Ts...>, Ts...>;\
	template<usize... I, class... Ts>\
	struct CAT(Name, Impl)<std::index_sequence<I...>, Ts...> : CAT(Name, Element)<Ts, I>...\
	{\
		template<class T, class Self>requires((usize{} + ... + $Same<Ts,T>) == 1 && $Same<CAT(Name, Impl), std::remove_cvref_t<Self>>) friend constexpr LikeImpl<Self, T>::type get(Self&& self)noexcept{ return FWD(self).CAT(Name, Element)<T, index_of_type<T, Ts...>>::value; }\
		template<usize index, class Self>requires(index < sizeof...(Ts) && $Same<CAT(Name, Impl), std::remove_cvref_t<Self>>) friend constexpr LikeImpl<Self, TYPE_AT<index, Ts...>>::type get(Self&& self)noexcept { return FWD(self).CAT(Name, Element)<TYPE_AT<index, Ts...>, index>::value; }\
		template<class Stream>\
		requires((...&&requires(Stream& os, const Ts x) { os << x; }))\
		friend constexpr Stream& operator<<(Stream& os, CAT(Name, Impl) const& tuple)noexcept\
		{\
			os << '<';\
			(..., [&]LAMBDA_INLINE\
			{\
				os << get<I>(tuple);\
				if constexpr (I != sizeof...(Ts) - 1)\
					os << ',';\
			}());\
			os << '>';\
			return os;\
		}\
		TUPLE_UNARY_OPERATOR(Name, +)\
		TUPLE_UNARY_OPERATOR(Name, -)\
		TUPLE_UNARY_OPERATOR(Name, !)\
		TUPLE_UNARY_OPERATOR(Name, ~)\
		TUPLE_BINARY_OPERATOR(Name, +)\
		TUPLE_BINARY_OPERATOR(Name, -)\
		TUPLE_BINARY_OPERATOR(Name, *)\
		TUPLE_BINARY_OPERATOR(Name, /)\
		TUPLE_BINARY_OPERATOR(Name, %)\
		TUPLE_BINARY_OPERATOR(Name, &)\
		TUPLE_BINARY_OPERATOR(Name, |)\
		TUPLE_BINARY_OPERATOR(Name, ^)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, +=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, -=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, *=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, /=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, %=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, &=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, |=)\
		TUPLE_ARITHMETIC_ASSIGN_OPERATOR(Name, ^=)\
		TUPLE_COMPARE_OPERATOR(<)\
		TUPLE_COMPARE_OPERATOR(>)\
		TUPLE_COMPARE_OPERATOR_EQUAL(<, <=)\
		TUPLE_COMPARE_OPERATOR_EQUAL(>, >=)\
		template<class Self>INLINE constexpr bool operator==(this Self&& self, auto&& r)noexcept requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(FWD(self)) == get<I>(FWD(r))}->$Same<bool>;})){return (...&&(get<I>(FWD(self)) == get<I>(FWD(r))));}\
		template<class Self>INLINE constexpr bool operator!=(this Self&& self, auto&& r)noexcept requires(std::tuple_size_v<std::remove_cvref_t<decltype(r)>> == sizeof...(Ts) && (...&&requires{{get<I>(FWD(self)) != get<I>(FWD(r))}->$Same<bool>;})){return (...||(get<I>(FWD(self)) != get<I>(FWD(r))));}\
	};\
	template<class...Ts>CAT(Name, Impl)(Ts...) -> CAT(Name, Impl)<std::index_sequence_for<Ts...>, Ts...>;\
	TUPLE_CONCEPT(Name)\
	TUPLE_STD(Name)\
	TUPLE_TESTS(Name)
#endif
#pragma pack(push, 1)
TUPLE_IMPL(Packed)
#pragma pack(pop)
TUPLE_IMPL(Tuple)
#undef TUPLE_IMPL
#undef TUPLE_UNARY_OPERATOR
#undef TUPLE_BINARY_OPERATOR
#undef TUPLE_ARITHMETIC_ASSIGN_OPERATOR
#undef TUPLE_COMPARE_OPERATOR
#undef TUPLE_COMPARE_OPERATOR_EQUAL
#undef TUPLE_TESTS
#undef TUPLE_STD
#undef TUPLE_CONCEPT
STATIC_ASSERT(sizeof(Tuple<u8, u16, u32, u64>) == 16);
STATIC_ASSERT(sizeof(Packed<u8, u16, u32, u64>) == sizeof(u8) + sizeof(u16) + sizeof(u32) + sizeof(u64));
#if HAS_BUILTIN(__builtin_is_implicit_lifetime)
	STATIC_ASSERT(__builtin_is_implicit_lifetime(Packed<u8, u16, u32, u64>));
	STATIC_ASSERT(__builtin_is_implicit_lifetime(Tuple<u8, u16, u32, u64>));
#endif
STATIC_ASSERT([](auto...)static
{
	static constexpr Packed packed{ 1,1.234f,false };
#if VARIADIC_STRUCTURED_BINDING
	[[maybe_unused]] auto [..._] {packed};
#endif
#if TEMPLATE_FOR
	template for (constexpr auto _ : packed) {}
#endif
	return true;
}());