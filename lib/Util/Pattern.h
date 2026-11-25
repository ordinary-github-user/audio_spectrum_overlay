#pragma once
#include "Compiler.h"
#include "Concepts/String.h"
#include "Inline.h"
#include "Macro/If.h"
#include "Strcmp.h"
#include "TypeAt.h"
namespace Pattern
{
	struct Ignored
	{
		static constexpr u8 size{ 1 };
		INLINE static constexpr bool match(auto const*) noexcept { return true; }
	};
	template<FixedString s, Strcmp::Encoding encoding = Strcmp::Encoding::ascii, bool case_sensitive = true>
	struct Exact
	{
		static constexpr auto size{ s.size() };
		INLINE static constexpr bool match(auto const* p)noexcept { return Strcmp::Equal<s, encoding, case_sensitive>::match(p); }
	};
	template<class...Ts>
	requires(0 != sizeof...(Ts))
	struct Any
	{
		static constexpr auto size{ (u8{},...,Ts::size) }; STATIC_ASSERT(((size == Ts::size)&&...));
		INLINE static constexpr bool match(auto const* p) noexcept { return (Ts::match(p) || ...); }
	};
	template<class...>struct ConcatImpl;
	template<usize...I, class...P>
	struct ConcatImpl<std::index_sequence<I...>, P...>
	{
		using Index = Compact<(0 + ... + P::size)>;
		static constexpr Index size{ (0 + ... + P::size) };
		template<usize J>
		requires(J < sizeof...(P))
		static constexpr Index offset_at{ [] static -> Index
		{
			if constexpr (J == 0)
				return 0;
			else
				return offset_at<J - 1> + TYPE_AT<IF(MSVC,J ? J - 1 : 0,J - 1), P...>::size;//msvc bug
		}() };
		INLINE static constexpr bool match($Char auto const* p)noexcept
		{
			return (... && TYPE_AT<I, P...>::match(p + offset_at<I>));
		}
		INLINE static constexpr bool match($String auto const& v)noexcept
		{
			return size == v.size() && match(v.data());
		}
	};
	template<class...P>
	using Concat = ConcatImpl<std::make_index_sequence<sizeof...(P)>, P...>;

	template<auto InclusiveMin, auto InclusiveMax>
	requires($Same<decltype(InclusiveMin), decltype(InclusiveMax)>)
	struct Range
	{
		static constexpr u8 size{ 1 };
		INLINE static constexpr bool match(auto const* p) noexcept { return p[0] >= InclusiveMin && p[0] <= InclusiveMax; }
	};

	using Number = Range<'0', '9'>;
	using Lowercase = Range<'a', 'z'>;
	using Uppercase = Range<'A', 'Z'>;
};