#pragma once
#include "BasicTypes.h"
#include "Compact.h"
#include "Constant.h"
#include "DataStructures/IntegerSequence.h"
#include "ImmediateValue.h"
#include "Inline.h"
#include "Macro/MovForward.h"
namespace Fold
{
	template<auto...I>INLINE constexpr void repeat_impl(auto&& action, std::index_sequence<I...>)noexcept { (..., (void)(action(), I)); }
	template<class Ret, class T, auto...I>INLINE constexpr Ret construct_impl(auto&& action, std::integer_sequence<T, I...>)noexcept { return { action(IV<I>)... }; }
	template<auto Entries, auto...I>INLINE constexpr void for_each_impl(auto&& action, std::index_sequence<I...>)noexcept { (..., action(IV<Entries[I]>)); }

	template<class T, auto...I>INLINE constexpr auto comma_impl/*     */(auto&& action, std::integer_sequence<T, I...>)noexcept { return(..., action(IV<I>)); }
	template<class T, auto...I>INLINE constexpr auto binary_or_impl/* */(auto&& action, std::integer_sequence<T, I...>)noexcept { return(...| action(IV<I>)); }
	template<class T, auto...I>INLINE constexpr auto binary_and_impl/**/(auto&& action, std::integer_sequence<T, I...>)noexcept { return(...| action(IV<I>)); }
	template<class T, auto...I>INLINE constexpr auto all_impl/*       */(auto&& action, std::integer_sequence<T, I...>)noexcept { return(...&&action(IV<I>)); }
	template<class T, auto...I>INLINE constexpr auto any_impl/*       */(auto&& action, std::integer_sequence<T, I...>)noexcept { return(...||action(IV<I>)); }
	template<class T, auto...I>INLINE constexpr auto product_impl/*   */(auto&& action, std::integer_sequence<T, I...>)noexcept { return(...* action(IV<I>)); }
	template<class T, auto...I>INLINE constexpr auto sum_impl/*       */(auto&& action, std::integer_sequence<T, I...>)noexcept { return(...+ action(IV<I>)); }

	template<usize Count>requires(Count >= 0)INLINE constexpr void repeat(auto&& action)noexcept { repeat_impl(FWD(action), std::make_index_sequence<Count>{}); }

	//template<auto Begin/*Exclusive*/>requires(Begin >= 0)INLINE constexpr void inverse_loop(auto&& action)noexcept{comma_impl(FWD(action), make_integer_range_inverse<Compact<Begin>, 0, Begin>{});}

	template<class Ret, auto End>requires(End >= 0)INLINE constexpr Ret construct(auto&& action)noexcept { return construct_impl<Ret>(FWD(action), std::make_integer_sequence<Compact<End>, End>{}); }

	template<auto Entries>INLINE constexpr void for_each(auto&& action)noexcept { for_each_impl<Entries>(FWD(action), std::make_index_sequence<Entries.size()>{}); }

	template<auto Begin, auto End>requires(Begin >= 0 && End >= 0 && Begin <= End)INLINE constexpr auto comma/*     */(auto&& action)noexcept { return comma_impl/*     */(FWD(action), make_integer_range<Compact<Begin, End>, Begin, End>{}); }
	template</*        */auto End>requires(/*          */End >= 0)/*            */INLINE constexpr auto comma/*     */(auto&& action)noexcept { return comma_impl/*     */(FWD(action), std::make_integer_sequence<Compact<End>, End>{}); }

	template<auto Begin, auto End>requires(Begin >= 0 && End >= 0 && Begin <= End)INLINE constexpr auto binary_or/* */(auto&& action)noexcept { return binary_or_impl/* */(FWD(action), make_integer_range<Compact<Begin, End>, Begin, End>{}); }
	template</*        */auto End>requires(/*          */End >= 0)/*            */INLINE constexpr auto binary_or/* */(auto&& action)noexcept { return binary_or_impl/* */(FWD(action), std::make_integer_sequence<Compact<End>, End>{}); }

	template<auto Begin, auto End>requires(Begin >= 0 && End >= 0 && Begin <= End)INLINE constexpr auto binary_and/**/(auto&& action)noexcept { return binary_and_impl/**/(FWD(action), make_integer_range<Compact<Begin, End>, Begin, End>{}); }
	template</*        */auto End>requires(/*          */End >= 0)/*            */INLINE constexpr auto binary_and/**/(auto&& action)noexcept { return binary_and_impl/**/(FWD(action), std::make_integer_sequence<Compact<End>, End>{}); }
																												   
	template<auto Begin, auto End>requires(Begin >= 0 && End >= 0 && Begin <= End)INLINE constexpr auto all/*       */(auto&& action)noexcept { return all_impl/*       */(FWD(action), make_integer_range<Compact<Begin, End>, Begin, End>{}); }
	template</*        */auto End>requires(/*          */End >= 0)/*            */INLINE constexpr auto all/*       */(auto&& action)noexcept { return all_impl/*       */(FWD(action), std::make_integer_sequence<Compact<End>, End>{}); }

	template<auto Begin, auto End>requires(Begin >= 0 && End >= 0 && Begin <= End)INLINE constexpr auto any/*       */(auto&& action)noexcept { return any_impl/*       */(FWD(action), make_integer_range<Compact<Begin, End>, Begin, End>{}); }
	template</*        */auto End>requires(/*          */End >= 0)/*            */INLINE constexpr auto any/*       */(auto&& action)noexcept { return any_impl/*       */(FWD(action), std::make_integer_sequence<Compact<End>, End>{}); }

	template<auto Begin, auto End>requires(Begin >= 0 && End >= 0 && Begin <= End)INLINE constexpr auto product/*   */(auto&& action)noexcept { return product_impl/*   */(FWD(action), make_integer_range<Compact<Begin, End>, Begin, End>{}); }
	template</*        */auto End>requires(/*          */End >= 0)/*            */INLINE constexpr auto product/*   */(auto&& action)noexcept { return product_impl/*   */(FWD(action), std::make_integer_sequence<Compact<End>, End>{}); }

	template<auto Begin, auto End>requires(Begin >= 0 && End >= 0 && Begin <= End)INLINE constexpr auto sum/*       */(auto&& action)noexcept { return sum_impl/*       */(FWD(action), make_integer_range<Compact<Begin, End>, Begin, End>{}); }
	template</*        */auto End>requires(/*          */End >= 0)/*            */INLINE constexpr auto sum/*       */(auto&& action)noexcept { return sum_impl/*       */(FWD(action), std::make_integer_sequence<Compact<End>, End>{}); }
}