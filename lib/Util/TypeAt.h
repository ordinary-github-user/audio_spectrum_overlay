#pragma once
#include "BasicTypes.h"
#include "Compiler.h"
#if GCC//gcc bug: https://github.com/facebook/folly/issues/1991
	namespace details
	{
		template <usize N, class... Ts>
		struct type_pack_element { using type = __type_pack_element<N, Ts...>; };
	}
	template <usize N, class... Ts>
	requires(N < sizeof...(Ts))
	using TYPE_AT = typename details::type_pack_element<N, Ts...>::type;
#elif HAS_BUILTIN(__type_pack_element)
	#define TYPE_AT __type_pack_element
#elif defined(__cpp_pack_indexing)
	template <usize N, class... Ts>
	requires(N < sizeof...(Ts))
	using TYPE_AT = Ts...[N];
#else
	//https://github.com/facebook/folly/blob/main/folly/Traits.h
	namespace details
	{
		template <usize N, class T>
		struct pack_element_indexed_type { using type = T; };

		template <class...>
		struct pack_element_set;

		template <usize... N, class... T>
		struct pack_element_set<std::index_sequence<N...>, T...> : pack_element_indexed_type<N, T>... {};

		template <usize N>
		struct pack_element_test
		{
			template <class T>
			static pack_element_indexed_type<N, T> impl(pack_element_indexed_type<N, T>*);
		};
	}

	template <usize N, class... Ts>
	requires(N < sizeof...(Ts))
	using TYPE_AT = typename decltype(details::pack_element_test<N>::impl(static_cast<details::pack_element_set<std::make_index_sequence<sizeof...(Ts)>, Ts...>*>(nullptr)))::type;
#endif