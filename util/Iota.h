#pragma once
#include "Compact.h"
#include "ConceptIntegral.h"
#include "ConceptSame.h"
#include "Consteval.h"
#include "Inline.h"
#include "StaticAssert.h"
template<$UInt T, bool Up, $UInt Result = T>
struct IotaIterator
{
	T value;
	INLINE constexpr Result operator*()const noexcept { return static_cast<Result>(value); }
	INLINE constexpr IotaIterator& operator++()noexcept { if constexpr (Up)++value; else --value; return *this; }
	INLINE constexpr bool operator!=(IotaIterator r)const noexcept { return value != r.value; }
	INLINE friend constexpr T operator-(IotaIterator l, IotaIterator r) noexcept{if constexpr (Up)return static_cast<T>(l.value - r.value);else return static_cast<T>(r.value - l.value);}
	INLINE friend constexpr IotaIterator operator-(IotaIterator l, $Int auto r)noexcept { if constexpr (Up) return { static_cast<T>(l.value - r) }; else return { static_cast<T>(l.value + r) }; }
	INLINE friend constexpr IotaIterator operator+(IotaIterator l, $Int auto r)noexcept { if constexpr (Up) return { static_cast<T>(l.value + r) }; else return { static_cast<T>(l.value - r) }; }
	INLINE friend constexpr IotaIterator operator+($Int auto l, IotaIterator r)noexcept { if constexpr (Up) return { static_cast<T>(r.value + l) }; else return { static_cast<T>(r.value - l) }; }
};

template<$UInt T, T Begin, T End, bool Up>
requires(Up ? (Begin <= End || End == 0) : (Begin >= End || End == T(-1)))
struct TIota
{
#if 1
	using Result = Compact<(Begin == End) ? T{}: (Up? static_cast<T>(End - 1): Begin)>;
#else
	using Result = Compact<[]static
	{
		T max{};
		for (T i{ Begin }; i != End; Up ? ++i : --i)
			if (i > max)
				max = i;
		return max;
	}()>;
#endif
	static CONSTEVAL IotaIterator<T, Up, Result> begin()noexcept { return { Begin }; }
	static CONSTEVAL IotaIterator<T, Up, Result> end()noexcept { return { End }; }
	static CONSTEVAL auto size()noexcept { return compact<Up ? T(End - Begin) : T(Begin - End)>; }
	template<usize Index>requires(Index < size())CONSTEVAL friend auto get(TIota)noexcept { return compact<Up ? (Begin + Index) : (Begin - Index)>; }
};
namespace std
{
	template<$UInt T, T Begin, T End, bool Up>
	struct tuple_size<TIota<T, Begin, End, Up>>
	{
		static constexpr auto value{ compact<Up ? T(End - Begin) : T(Begin - End)> };
	};
	template<usize Index, $UInt T, T Begin, T End, bool Up>
	struct tuple_element<Index, TIota<T, Begin, End, Up>>
	{
		using type = Compact<Up ? (Begin + Index) : (Begin - Index)>;
	};
}

template<$UInt T, T Begin, T End>CONSTEVAL TIota<Compact<T(End - 1), T(Begin - 1)>, compact<T(End - 1)>, compact<T(Begin - 1)>, 0> reversed(TIota<T, Begin, End, 1>)noexcept { return {}; }
template<$UInt T, T Begin, T End>CONSTEVAL TIota<Compact<T(End + 1), T(Begin + 1)>, compact<T(End + 1)>, compact<T(Begin + 1)>, 1> reversed(TIota<T, Begin, End, 0>)noexcept { return {}; }

template<u64...args>
requires((sizeof...(args) == 1 || sizeof...(args) == 2))
constexpr auto iota{ []static
{
	using T = Compact<args...>;
	static constexpr T params[]{ args... };
	static constexpr T begin{ (sizeof...(args) == 1) ? T{} : params[0] };
	static constexpr T end  { params[(sizeof...(args) == 2)]};
	/* */if constexpr (begin == end)											return TIota<u8, 0, 0, true>{};
	else if constexpr (begin > T{} && begin < end && end == u64{ 0x100 })		return TIota<u8, static_cast<u8>(begin), 0, true>{};
	else if constexpr (begin > T{} && begin < end && end == u64{ 0x10000 })		return TIota<u16, static_cast<u16>(begin), 0, true>{};
	else if constexpr (begin > T{} && begin < end && end == u64{ 0x100000000 })	return TIota<u32, static_cast<u32>(begin), 0, true>{};
	else																		return TIota<T, begin, end, begin <= end>{};
}() };

STATIC_ASSERT(1 == sizeof(*iota<0>.begin()));
STATIC_ASSERT(1 == sizeof(*iota<0x100>.begin()));
STATIC_ASSERT(2 == sizeof(*iota<0x101>.begin()));
STATIC_ASSERT(2 == sizeof(*iota<0x10000>.begin()));
STATIC_ASSERT(4 == sizeof(*iota<0x10001>.begin()));
STATIC_ASSERT(4 == sizeof(*iota<0x100000000>.begin()));
STATIC_ASSERT(8 == sizeof(*iota<0x100000001>.begin()));
STATIC_ASSERT(1 == sizeof(iota<0x10000, 0x10000>.begin()));
STATIC_ASSERT(2 == sizeof(iota<0x100>.begin()));
STATIC_ASSERT(4 == sizeof(iota<0x10000>.begin()));
STATIC_ASSERT(8 == sizeof(iota<0x100000000>.begin()));
STATIC_ASSERT(1 == sizeof(iota<123, 0x100>.begin()));
STATIC_ASSERT(2 == sizeof(iota<123, 0x10000>.begin()));
STATIC_ASSERT(4 == sizeof(iota<123, 0x100000000>.begin()));
STATIC_ASSERT(2 == sizeof(reversed(iota<0x100>).begin()));
STATIC_ASSERT(4 == sizeof(reversed(iota<0x10000>).begin()));
STATIC_ASSERT(8 == sizeof(reversed(iota<0x100000000>).begin()));
STATIC_ASSERT(1 == sizeof(reversed(iota<123, 0x100>).begin()));
STATIC_ASSERT(2 == sizeof(reversed(iota<123, 0x10000>).begin()));
STATIC_ASSERT(4 == sizeof(reversed(iota<123, 0x100000000>).begin()));
STATIC_ASSERT($Same<std::remove_const_t<decltype(iota<0x100>)>, TIota<u16, 0, 0x100, true>>);
STATIC_ASSERT($Same<std::remove_const_t<decltype(iota<0x10000>)>, TIota<u32, 0, 0x10000, true>>);
STATIC_ASSERT($Same<std::remove_const_t<decltype(iota<0x100000000>)>, TIota<u64, 0, 0x100000000, true>>);
STATIC_ASSERT($Same<std::remove_const_t<decltype(iota<1, 0x100>)>, TIota<u8, 1, 0, true>>);
STATIC_ASSERT($Same<std::remove_const_t<decltype(iota<1, 0x10000>)>, TIota<u16, 1, 0, true>>);
STATIC_ASSERT($Same<std::remove_const_t<decltype(iota<1, 0x100000000>)>, TIota<u32, 1, 0, true>>);
#define IOTA_TEST0(T, ...) STATIC_ASSERT(all_same<T, std::remove_const_t<decltype(*iota<__VA_ARGS__>.begin())>,std::remove_const_t<decltype(*reversed(iota<__VA_ARGS__>).begin())>>)
IOTA_TEST0(u8, 0);
IOTA_TEST0(u8, 1);
IOTA_TEST0(u8, 2);
IOTA_TEST0(u8, 255);
IOTA_TEST0(u8, 256);
IOTA_TEST0(u8, 255,255);
IOTA_TEST0(u8, 256,256);
IOTA_TEST0(u8, 1, 255);
IOTA_TEST0(u8, 1, 256);
IOTA_TEST0(u8, 2, 255);
IOTA_TEST0(u8, 2, 256);
IOTA_TEST0(u16, 257);
IOTA_TEST0(u16, 1, 257);
IOTA_TEST0(u16, 2, 257);
IOTA_TEST0(u8, 257, 257);
IOTA_TEST0(u8, 258, 258);
#define IOTA_TEST1(...) STATIC_ASSERT($Same<std::remove_const_t<decltype(iota<__VA_ARGS__>)>, std::remove_const_t<decltype(reversed(reversed(iota<__VA_ARGS__>)))>>);
IOTA_TEST1(255);
IOTA_TEST1(256);
IOTA_TEST1(257);
IOTA_TEST1(255,255);
IOTA_TEST1(256,256);
IOTA_TEST1(257,257);
IOTA_TEST1(1,255);
IOTA_TEST1(1,256);
IOTA_TEST1(1,257);
#define IOTA_TEST2(from, to)STATIC_ASSERT([] static				\
{																\
	{															\
		static constexpr usize begin{from};						\
		static constexpr usize end{to};							\
		usize i{ begin };										\
		auto iBegin{ iota<begin,end>.begin() };					\
		const auto iEnd{ iota<begin,end>.end() };				\
		for (;;)												\
		{														\
			if ((i == end) != !(iBegin != iEnd))return false;	\
			if (i == end)break;									\
			if (i != *iBegin)return false;						\
			++i;												\
			++iBegin;											\
		}														\
	}															\
	{															\
		static constexpr usize begin{usize(to - 1)};			\
		static constexpr usize end{usize(from - 1)};			\
		usize i{ begin };										\
		auto iBegin{ reversed(iota<from, to>).begin() };		\
		const auto iEnd{ reversed(iota<from, to>).end() };		\
		for (;;)												\
		{														\
			if ((i == end) != !(iBegin != iEnd))return false;	\
			if (i == end)break;									\
			if (i != *iBegin)return false;						\
			--i;												\
			++iBegin;											\
		}														\
	}															\
	return true;												\
}())
IOTA_TEST2(0, 0);
IOTA_TEST2(1, 1);
IOTA_TEST2(0, 1);
IOTA_TEST2(0, 2);
IOTA_TEST2(255, 255);
IOTA_TEST2(256, 256);
IOTA_TEST2(257, 257);
IOTA_TEST2(254, 255);
IOTA_TEST2(255, 256);
IOTA_TEST2(256, 257);
IOTA_TEST2(0, 255);
IOTA_TEST2(0, 256);
IOTA_TEST2(0, 257);
IOTA_TEST2(1, 255);
IOTA_TEST2(1, 256);
IOTA_TEST2(1, 257);
#undef IOTA_TEST0
#undef IOTA_TEST1
#undef IOTA_TEST2

template<$UInt T, bool Up>
struct Iota//[start, finish)
{
	const T start;
	const T finish;
	INLINE constexpr IotaIterator<T, Up> begin()const noexcept { return { start }; }
	INLINE constexpr IotaIterator<T, Up> end()const noexcept { return { finish }; }
	INLINE friend constexpr Iota<T, !Up> reversed(const Iota& range)noexcept requires( Up) { return { .start{T(range.finish - 1)}, .finish{T(range.start - 1)} }; }
	INLINE friend constexpr Iota<T, !Up> reversed(const Iota& range)noexcept requires(!Up) { return { .start{T(range.finish + 1)}, .finish{T(range.start + 1)} }; }
	INLINE friend constexpr bool operator==(const Iota&, const Iota&)noexcept = default;
};

template<$UInt T, T>struct IotaDown;
template<$UInt T, T>struct IotaUp;

template<$UInt T, T start>
struct IotaUp//[start, finish)
{
	const T finish;
	INLINE constexpr IotaIterator<T, 1> begin()const noexcept { return { start }; }
	INLINE constexpr IotaIterator<T, 1> end()const noexcept { return { finish }; }
	INLINE constexpr T size()const noexcept { return static_cast<T>(finish - start); }
	INLINE friend constexpr IotaDown<T, static_cast<T>(start - 1)> reversed(IotaUp range)noexcept { return { .start{T(range.finish - 1)} }; }
	INLINE friend constexpr bool operator==(const IotaUp&, const IotaUp&)noexcept = default;
};

template<$UInt T, T finish>
struct IotaDown//[start, finish)
{
	const T start;
	INLINE constexpr IotaIterator<T, 0> begin()const noexcept { return { start }; }
	INLINE constexpr IotaIterator<T, 0> end()const noexcept { return { finish }; }
	INLINE friend constexpr IotaUp<T, static_cast<T>(finish + 1)> reversed(IotaDown range)noexcept { return { .finish{T(range.start + 1)} }; }
	INLINE friend constexpr bool operator==(const IotaDown&, const IotaDown&)noexcept = default;
};

template<$UInt T>INLINE constexpr Iota    <T, 1> iota_up  (T min, T max)noexcept { return { .start{min}, .finish{max} }; }
template<$UInt T>INLINE constexpr Iota    <T, 0> iota_down(T max, T min)noexcept { return { .start{max}, .finish{min} }; }
template<$UInt T>INLINE constexpr IotaUp  <T, 0> iota_up  (T max)       noexcept { return {              .finish{max} }; }
template<$UInt T>INLINE constexpr IotaDown<T, 0> iota_down(T max)       noexcept { return { .start{max} }; }

STATIC_ASSERT(iota_up<u8>(0)     == reversed(reversed(iota_up<u8>(0))));
STATIC_ASSERT(iota_up<u8>(100)   == reversed(reversed(iota_up<u8>(100))));
STATIC_ASSERT(iota_up<u8>(255)   == reversed(reversed(iota_up<u8>(255))));
STATIC_ASSERT(iota_up<u8>(0,100) == reversed(reversed(iota_up<u8>(0,100))));
STATIC_ASSERT(iota_up<u8>(0,255) == reversed(reversed(iota_up<u8>(0,255))));
STATIC_ASSERT(iota_up<u8>(1,255) == reversed(reversed(iota_up<u8>(1,255))));
#define IOTA_RUNTIME_TEST1(to)STATIC_ASSERT([] static			\
{																\
	using T = Compact<to>;										\
	{															\
		static constexpr auto end{compact<to>};					\
		constexpr auto range{iota_up<T>(end)};					\
		usize i{ 0 };											\
		auto iBegin{ range.begin() };							\
		const auto iEnd{ range.end() };							\
		for (;;)												\
		{														\
			if ((i == end) != !(iBegin != iEnd))return false;	\
			if (i == end)break;									\
			if (i != *iBegin)return false;						\
			++i;												\
			++iBegin;											\
		}														\
	}															\
	{															\
		static constexpr usize begin{usize(to - 1)};			\
		static constexpr usize end{usize(-1)};					\
		constexpr auto range{reversed(iota_up<T>(to))};			\
		usize i{ begin };										\
		auto iBegin{ range.begin() };							\
		const auto iEnd{ range.end() };							\
		for (;;)												\
		{														\
			if ((i == end) != !(iBegin != iEnd))return false;	\
			if (i == end)break;									\
			if (i != *iBegin)return false;						\
			--i;												\
			++iBegin;											\
		}														\
	}															\
	return true;												\
}())
IOTA_RUNTIME_TEST1(0);
IOTA_RUNTIME_TEST1(1);
IOTA_RUNTIME_TEST1(2);
IOTA_RUNTIME_TEST1(255);
IOTA_RUNTIME_TEST1(256);
IOTA_RUNTIME_TEST1(257);

#define IOTA_RUNTIME_TEST2(from, to)STATIC_ASSERT([] static		\
{																\
	using T = Compact<from, to>;								\
	{															\
		static constexpr usize begin{from};						\
		static constexpr usize end{to};							\
		constexpr auto range{iota_up<T>(begin, end)};			\
		usize i{ begin };										\
		auto iBegin{ range.begin() };							\
		const auto iEnd{ range.end() };							\
		for (;;)												\
		{														\
			if ((i == end) != !(iBegin != iEnd))return false;	\
			if (i == end)break;									\
			if (i != *iBegin)return false;						\
			++i;												\
			++iBegin;											\
		}														\
	}															\
	{															\
		static constexpr usize begin{usize(to - 1)};			\
		static constexpr usize end{usize(from - 1)};			\
		constexpr auto range{reversed(iota_up<T>(from, to))};	\
		usize i{ begin };										\
		auto iBegin{ range.begin() };							\
		const auto iEnd{ range.end() };							\
		for (;;)												\
		{														\
			if ((i == end) != !(iBegin != iEnd))return false;	\
			if (i == end)break;									\
			if (i != *iBegin)return false;						\
			--i;												\
			++iBegin;											\
		}														\
	}															\
	return true;												\
}())
IOTA_RUNTIME_TEST2(0, 0);
IOTA_RUNTIME_TEST2(1, 1);
IOTA_RUNTIME_TEST2(0, 1);
IOTA_RUNTIME_TEST2(0, 2);
IOTA_RUNTIME_TEST2(255, 255);
IOTA_RUNTIME_TEST2(256, 256);
IOTA_RUNTIME_TEST2(257, 257);
IOTA_RUNTIME_TEST2(254, 255);
IOTA_RUNTIME_TEST2(255, 256);
IOTA_RUNTIME_TEST2(256, 257);
IOTA_RUNTIME_TEST2(0, 255);
IOTA_RUNTIME_TEST2(0, 256);
IOTA_RUNTIME_TEST2(0, 257);
IOTA_RUNTIME_TEST2(1, 255);
IOTA_RUNTIME_TEST2(1, 256);
IOTA_RUNTIME_TEST2(1, 257);
#undef IOTA_RUNTIME_TEST1
#undef IOTA_RUNTIME_TEST2