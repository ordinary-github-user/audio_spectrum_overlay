#pragma once
#include "../AllUnique.h"
#include "../BasicTypes.h"
#include "../Compiler.h"
#include "../Concepts/Same.h"
#include "../Inline.h"
#include "../Iota.h"
#include "../Meta.h"
#include "ArgCount.h"
#include "Empty.h"
#include "ForEach.h"
#include "If.h"
#include "Last.h"
#include "PopBack.h"
#if TEMPLATE_FOR && META
	#define DEFINE_LEXI(...)\
	INLINE constexpr bool operator<(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>> && []static\
	{\
		static constexpr auto N{ field_count<std::remove_cvref_t<decltype(l)>> };\
		if constexpr(N == 0)return true;\
		else\
		{\
			template for (constexpr auto i : iota<N-1>)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
			template for (constexpr auto i : iota<N  >)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) <  (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
			return true;\
		}\
	}())\
	{\
		static constexpr auto N{field_count<std::remove_cvref_t<decltype(l)>>};\
		if constexpr(N == 0)return false;\
		else\
		{\
			template for (constexpr auto i : iota<N - 1>)if((l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]))[[likely]]return (l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) < (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]);\
			return (l.[:field_at<N - 1, std::remove_cvref_t<decltype(l)>>:]) < (r.[:field_at<N - 1, std::remove_cvref_t<decltype(l)>>:]);\
		}\
	}\
	INLINE constexpr bool operator>(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>> && []static\
	{\
		static constexpr auto N{ field_count<std::remove_cvref_t<decltype(l)>> };\
		if constexpr(N == 0)return true;\
		else\
		{\
			template for (constexpr auto i : iota<N-1>)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
			template for (constexpr auto i : iota<N  >)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) >  (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
			return true;\
		}\
	}())\
	{\
		static constexpr auto N{field_count<std::remove_cvref_t<decltype(l)>>};\
		if constexpr(N == 0)return false;\
		else\
		{\
			template for (constexpr auto i : iota<N - 1>)if((l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]))[[likely]]return (l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) > (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]);\
			return (l.[:field_at<N - 1, std::remove_cvref_t<decltype(l)>>:]) > (r.[:field_at<N - 1, std::remove_cvref_t<decltype(l)>>:]);\
		}\
	}\
	INLINE constexpr bool operator<=(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>> && []static\
	{\
		static constexpr auto N{ field_count<std::remove_cvref_t<decltype(l)>> };\
		if constexpr(N == 0)return true;\
		else\
		{\
			template for (constexpr auto i : iota<N-1>)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
			template for (constexpr auto i : iota<N-1>)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) <  (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
			if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {l.[:field_at<N-1,std::remove_cvref_t<decltype(l)>>:] <= r.[:field_at<N-1,std::remove_cvref_t<decltype(l)>>:]}->$Same<bool>; })return false;\
			return true;\
		}\
	}())\
	{\
		static constexpr auto N{field_count<std::remove_cvref_t<decltype(l)>>};\
		if constexpr(N == 0)return true;\
		else\
		{\
			template for (constexpr auto i : iota<N - 1>)if((l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]))[[likely]]return (l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) < (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]);\
			return (l.[:field_at<N - 1, std::remove_cvref_t<decltype(l)>>:]) <= (r.[:field_at<N - 1, std::remove_cvref_t<decltype(l)>>:]);\
		}\
	}\
	INLINE constexpr bool operator>=(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>> && []static\
	{\
		static constexpr auto N{ field_count<std::remove_cvref_t<decltype(l)>> };\
		if constexpr(N == 0)return true;\
		else\
		{\
			template for (constexpr auto i : iota<N-1>)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
			template for (constexpr auto i : iota<N-1>)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) >  (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
			if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {l.[:field_at<N-1,std::remove_cvref_t<decltype(l)>>:] >= r.[:field_at<N-1,std::remove_cvref_t<decltype(l)>>:]}->$Same<bool>; })return false;\
			return true;\
		}\
	}())\
	{\
		static constexpr auto N{field_count<std::remove_cvref_t<decltype(l)>>};\
		if constexpr(N == 0)return true;\
		else\
		{\
			template for (constexpr auto i : iota<N - 1>)if((l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]))[[likely]]return (l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) > (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]);\
			return (l.[:field_at<N - 1, std::remove_cvref_t<decltype(l)>>:]) >= (r.[:field_at<N - 1, std::remove_cvref_t<decltype(l)>>:]);\
		}\
	}\
	INLINE constexpr bool operator==(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>> && []static\
	{\
		static constexpr auto N{ field_count<std::remove_cvref_t<decltype(l)>> };\
		template for (constexpr auto i : iota<N>)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) == (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
		return true;\
	}())\
	{\
		static constexpr auto [...i]{iota<field_count<std::remove_cvref_t<decltype(l)>>>};\
		return (...&&((l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) == (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])));\
	}\
	INLINE constexpr bool operator!=(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>> && []static\
	{\
		static constexpr auto N{ field_count<std::remove_cvref_t<decltype(l)>> };\
		template for (constexpr auto i : iota<N>)if constexpr(!requires(const std::remove_cvref_t<decltype(l)> l, const std::remove_cvref_t<decltype(l)> r){ {(l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])}->$Same<bool>; })return false;\
		return true;\
	}())\
	{\
		static constexpr auto [...i]{iota<field_count<std::remove_cvref_t<decltype(l)>>>};\
		return (...||((l.[:field_at<i,std::remove_cvref_t<decltype(l)>>:]) != (r.[:field_at<i,std::remove_cvref_t<decltype(l)>>:])));\
	}
#else
	namespace details
	{
		template<usize N>struct LexiString
		{
			c8 data[N];
			consteval LexiString(const c8(&x)[N + 1])noexcept { for (auto i{ N }; i--;) { data[i] = x[i]; } }
			consteval friend bool operator==(const LexiString&, const LexiString&)noexcept = default;
		};
		template<usize N>
		LexiString(const c8(&)[N]) -> LexiString<N - 1>;
	}
	#define DEFINE_LEXI(_0,...) IF(EMPTY(__VA_ARGS__),DEFINE_LEXI1,DEFINE_LEXI_VARIADIC)(_0 __VA_OPT__(,__VA_ARGS__))
	#define DEFINE_LEXI1(_0)\
	INLINE constexpr bool operator<(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{DEFINE_LEXI_REQUIRES_L(_0)}){DEFINE_LEXI_ASSERT_FIELDS(_0)return l._0<r._0;};\
	INLINE constexpr bool operator>(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{DEFINE_LEXI_REQUIRES_G(_0)}){DEFINE_LEXI_ASSERT_FIELDS(_0)return l._0>r._0;};\
	INLINE constexpr bool operator<=(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{DEFINE_LEXI_REQUIRES_LE(_0)}){DEFINE_LEXI_ASSERT_FIELDS(_0)return l._0<=r._0;};\
	INLINE constexpr bool operator>=(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{DEFINE_LEXI_REQUIRES_GE(_0)}){DEFINE_LEXI_ASSERT_FIELDS(_0)return l._0>=r._0;};\
	INLINE constexpr bool operator==(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{DEFINE_LEXI_REQUIRES_E(_0)}){DEFINE_LEXI_ASSERT_FIELDS(_0)return l._0==r._0;};\
	INLINE constexpr bool operator!=(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{DEFINE_LEXI_REQUIRES_NE(_0)}){DEFINE_LEXI_ASSERT_FIELDS(_0)return l._0!=r._0;};
	#define DEFINE_LEXI_VARIADIC(...)\
	INLINE constexpr bool operator<(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{FOR_EACH(DEFINE_LEXI_REQUIRES_L,__VA_ARGS__)FOR_EACH(DEFINE_LEXI_REQUIRES_NE,POP_BACK(__VA_ARGS__))}){DEFINE_LEXI_ASSERT_FIELDS(__VA_ARGS__)FOR_EACH(DEFINE_LEXI_L,POP_BACK(__VA_ARGS__))return l.LAST(__VA_ARGS__)<r.LAST(__VA_ARGS__);};\
	INLINE constexpr bool operator>(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{FOR_EACH(DEFINE_LEXI_REQUIRES_G,__VA_ARGS__)FOR_EACH(DEFINE_LEXI_REQUIRES_NE,POP_BACK(__VA_ARGS__))}){DEFINE_LEXI_ASSERT_FIELDS(__VA_ARGS__)FOR_EACH(DEFINE_LEXI_G,POP_BACK(__VA_ARGS__))return l.LAST(__VA_ARGS__)>r.LAST(__VA_ARGS__);};\
	INLINE constexpr bool operator<=(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{FOR_EACH(DEFINE_LEXI_REQUIRES_L,POP_BACK(__VA_ARGS__))FOR_EACH(DEFINE_LEXI_REQUIRES_NE,POP_BACK(__VA_ARGS__))DEFINE_LEXI_REQUIRES_LE(LAST(__VA_ARGS__))}){DEFINE_LEXI_ASSERT_FIELDS(__VA_ARGS__)FOR_EACH(DEFINE_LEXI_L,POP_BACK(__VA_ARGS__))return l.LAST(__VA_ARGS__)<=r.LAST(__VA_ARGS__);};\
	INLINE constexpr bool operator>=(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{FOR_EACH(DEFINE_LEXI_REQUIRES_G,POP_BACK(__VA_ARGS__))FOR_EACH(DEFINE_LEXI_REQUIRES_NE,POP_BACK(__VA_ARGS__))DEFINE_LEXI_REQUIRES_GE(LAST(__VA_ARGS__))}){DEFINE_LEXI_ASSERT_FIELDS(__VA_ARGS__)FOR_EACH(DEFINE_LEXI_G,POP_BACK(__VA_ARGS__))return l.LAST(__VA_ARGS__)>=r.LAST(__VA_ARGS__);};\
	INLINE constexpr bool operator==(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{FOR_EACH(DEFINE_LEXI_REQUIRES_E,__VA_ARGS__)}){DEFINE_LEXI_ASSERT_FIELDS(__VA_ARGS__)return FOR_EACH_OP(DEFINE_LEXI_E,&&,__VA_ARGS__);};\
	INLINE constexpr bool operator!=(this auto&&l, auto&&r)noexcept requires($Same<std::remove_cvref_t<decltype(l)>,std::remove_cvref_t<decltype(r)>>&&requires{FOR_EACH(DEFINE_LEXI_REQUIRES_NE,__VA_ARGS__)}){DEFINE_LEXI_ASSERT_FIELDS(__VA_ARGS__)return FOR_EACH_OP(DEFINE_LEXI_NE,||,__VA_ARGS__);};
	#define DEFINE_LEXI_L(x) if(l.x!=r.x)[[likely]]return l.x<r.x;
	#define DEFINE_LEXI_G(x) if(l.x!=r.x)[[likely]]return l.x>r.x;
	#define DEFINE_LEXI_E(x) (l.x==r.x)
	#define DEFINE_LEXI_NE(x) (l.x!=r.x)
	#define DEFINE_LEXI_MAKE_FIXED_STRING(x) details::LexiString{#x}
	#define DEFINE_LEXI_REQUIRES_L(x) {l.x<r.x}->$Same<bool>;
	#define DEFINE_LEXI_REQUIRES_G(x) {l.x>r.x}->$Same<bool>;
	#define DEFINE_LEXI_REQUIRES_LE(x) {l.x<=r.x}->$Same<bool>;
	#define DEFINE_LEXI_REQUIRES_GE(x) {l.x>=r.x}->$Same<bool>;
	#define DEFINE_LEXI_REQUIRES_E(x) {l.x==r.x}->$Same<bool>;
	#define DEFINE_LEXI_REQUIRES_NE(x) {l.x!=r.x}->$Same<bool>;
	#define DEFINE_LEXI_ASSERT_FIELDS(...) static_assert(all_unique<FOR_EACH_COMMA(DEFINE_LEXI_MAKE_FIXED_STRING,__VA_ARGS__)>);
#endif