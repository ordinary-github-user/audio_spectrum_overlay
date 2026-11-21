#pragma once
#include "AlgorithmSort.h"
#include "AlgorithmSum.h"
#include "Compiler.h"
#include "BasicTypes.h"
#include "BitConcat.h"
#include "ByteConcat.h"
#include "ConceptSig.h"
#include "ConceptSize.h"
#include "Constant.h"
#include "ConstVector.h"
#include "FixedStringImpl.h"
#include "Inline.h"
#include "Iota.h"
#include "Kmask.h"
#include "NBytes.h"
#include "OptionalImpl.h"
#include "SizedInt.h"
#include "StaticError.h"
#include "Unreachable.h"
#include <array>
#include <immintrin.h>
namespace BytePattern
{
	struct Cmp
	{
		u8 size;
		std::array<u64,2> cost;//[maskless, masked]
	};

	constexpr std::array cmpsSimdPreload [[maybe_unused]]
	{
		 Cmp{1 ,{10,20}}
		,Cmp{2 ,{11,21}}
		,Cmp{4 ,{12,22}}
#if BITNESS == 64
		,Cmp{8 ,{13,23}}
#endif
#if defined(__AVX512F__)
		,Cmp{16,{20,30}}
		,Cmp{32,{21,31}}
		,Cmp{64,{22,32}}
#else
		,Cmp{16,{20,40}}
	#if defined(__AVX2__)
		,Cmp{32,{21,41}}
	#endif
#endif
	};

	constexpr std::array cmps [[maybe_unused]]
	{
		 Cmp{1 ,{10,20}}
		,Cmp{2 ,{11,21}}
		,Cmp{4 ,{12,22}}
#if BITNESS == 64
		,Cmp{8 ,{13,23}}
#endif
	};

	struct CmpOption
	{
		u8 advance;
		u8 size;
		std::array<u64, 2> cost;//[maskless, masked]
	};

	template<auto table>
	constexpr auto cmpOptions{ []static
	{
		std::array<CmpOption, table.back().size> result{};
		for (const auto i : iota<table.back().size>)
		{
			const u8 advance{ static_cast<u8>(i + 1) };
			u8 j{};
			while (j != table.size() && table[j].size < advance)++j;
			result[i] = CmpOption
			{
				.advance{advance},
				.size{table[j].size},
				.cost{table[j].cost}
			};
		}
		return result;
	}() };

	struct CmpEntry
	{
		usize offset;
		u8 size;
		bool masked;
		u64 cost;
		constexpr f64 efficiency() const noexcept { return (f64)size / (f64)cost; }
		constexpr bool simd() const noexcept { return size > 8; }
	};

	struct Element
	{
		Opt<u8> low, high, byte;
		constexpr Element(Opt<u8> low, Opt<u8> high)noexcept :low{ low }, high{ high }, byte{ (low && high) ? static_cast<u8>(*low | (*high << 4)) : Opt<u8>{} } {}
		constexpr Element(u8 byte)noexcept :low{ static_cast<u8>(byte & 0x0f) }, high{ static_cast<u8>(static_cast<u8>(byte & 0xf0) >> 4) }, byte{ (low && high) ? static_cast<u8>(*low | (*high << 4)) : Opt<u8>{} }{}
		constexpr Element()noexcept :low{}, high{}, byte{} {}
		constexpr bool isWildcard()const noexcept { return !low && !high; }
		constexpr bool isNibble()const noexcept { return low.has_value() ^ high.has_value(); }
		constexpr u8 maskedByte()const noexcept { return static_cast<u8>(low.value_or(u8{}) | static_cast<u8>(high.value_or(u8{}) << 4)); }
		constexpr u8 mask()const noexcept { return (low ? u8(0x0f) : u8(0x0)) | (high ? u8(0xf0) : u8(0x0)); }
		constexpr bool operator==(const Element& r)noexcept { return low == r.low && high == r.high; }
	};

	template<usize N>
	struct DpEntry
	{
		u64 cost;
		ConstVector<CmpEntry, N> choices;
	};

	template<std::array Elements, std::array CostTable>
	requires requires{Elements.size(); }
	struct Sequence
	{
		//static constexpr usize Elements.size(){ Elements.size() };
		static constexpr std::array<Element, Elements.size()> elements{ Elements };

		static constexpr ConstVector<CmpEntry, Elements.size()> entries{ []static
		{
			std::array<DpEntry<Elements.size()>, Elements.size() + 1> dp{};//dp[i] = min cost of [0,i)
			for (usize i{1}; i <= Elements.size(); ++i)
			{
				if (Elements[i - 1].isWildcard())
				{
					dp[i] = dp[i - 1];
					continue;
				}
				bool first{true};
				for (const CmpOption& info : cmpOptions<CostTable>)
				{
					if (i < info.advance || i - info.advance + info.size > Elements.size())continue;
					bool masked{};
					for (usize j{ i - info.advance }; j != i - info.advance + info.size; ++j)
						if (!Elements[j].byte)
						{
							masked = true;
							break;
						}
					const u64 newCost{ dp[i - info.advance].cost + info.cost[masked] };
					if (first || newCost < dp[i].cost)
					{
						dp[i].cost = newCost;
						dp[i].choices = dp[i - info.advance].choices;
						dp[i].choices.push_back({i - info.advance, info.size, masked, info.cost[masked] });
						first = false;
					}
				}
			}
			Algorithm::sort(dp.back().choices, [](const CmpEntry& l, const CmpEntry& r) static
			{
				if (!l.simd() && r.simd())return true;
				if (l.simd() && !r.simd())return false;
				if (l.efficiency() > r.efficiency())return true;
				if (l.efficiency() < r.efficiency())return false;
				return l.offset < r.offset;
				//return (!l.masked && r.masked) || (l.masked == r.masked && (l.size > r.size || (l.size == r.size && l.offset < r.offset)));
			});
			return dp[Elements.size()].choices;
		}() };

		static constexpr u64 cost{ Algorithm::sum<u64>(entries, [](const CmpEntry& e) static{return e.cost; }) };
	private:
#if defined(__AVX512F__)
		//masked, no nibbles
		template<CmpEntry entry, usize...J>
		requires(entry.masked && (entry.size > 8) && !(elements[entry.offset + J].isNibble() || ...))
		INLINE static constexpr bool matchEntry($Size<1> auto* ptr, std::index_sequence<J...>)noexcept
		{
			static constexpr NBits <entry.size> mask { Bits ::concat<NBits <entry.size>>(elements[entry.offset + J].byte.has_value()...)};
			static constexpr NBytes<entry.size> bytes{ Bytes::concat<NBytes<entry.size>>(elements[entry.offset + J].maskedByte()...    )};
			/* */if constexpr (entry.size == 16) return kmask_zero(   _mm_mask_cmpneq_epu8_mask(mask,    _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + entry.offset)), bytes));
			else if constexpr (entry.size == 32) return kmask_zero(_mm256_mask_cmpneq_epu8_mask(mask, _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset)), bytes));
			else if constexpr (entry.size == 64) return kmask_zero(_mm512_mask_cmpneq_epu8_mask(mask, _mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr + entry.offset)), bytes));
		}
		//masked, yes nibbles
		template<CmpEntry entry, usize...J>
		requires(entry.masked && (entry.size <= 8 || (elements[entry.offset + J].isNibble() || ...)))
		INLINE static constexpr bool matchEntry($Size<1> auto* ptr, std::index_sequence<J...>)noexcept
		{
			static constexpr NBytes<entry.size> mask { Bytes::concat<NBytes<entry.size>>(elements[entry.offset + J].mask()...      ) };
			static constexpr NBytes<entry.size> bytes{ Bytes::concat<NBytes<entry.size>>(elements[entry.offset + J].maskedByte()...) };
			/* */if constexpr (entry.size <= 8 ) return (*reinterpret_cast<const NBytes<entry.size>*>(ptr + entry.offset) & mask) == bytes;
			else if constexpr (entry.size == 16) return kmask_zero(   _mm_cmpneq_epi64_mask(   _mm_and_si128(   _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + entry.offset)), mask), bytes));
			else if constexpr (entry.size == 32) return kmask_zero(_mm256_cmpneq_epi64_mask(_mm256_and_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset)), mask), bytes));
			else if constexpr (entry.size == 64) return kmask_zero(_mm512_cmpneq_epi64_mask(_mm512_and_si512(_mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr + entry.offset)), mask), bytes));
		}
		//not masked
		template<CmpEntry entry, usize...J>
		requires(!entry.masked)
		INLINE static constexpr bool matchEntry($Size<1> auto* ptr, std::index_sequence<J...>)noexcept
		{
			static constexpr NBytes<entry.size> bytes{ Bytes::concat<NBytes<entry.size>>(*elements[entry.offset + J].byte...) };
			/* */if constexpr (entry.size <= 8) return *reinterpret_cast<const NBytes<entry.size>*>(ptr + entry.offset) == bytes;
			else if constexpr (entry.size == 16) return kmask_zero(   _mm_cmpneq_epi64_mask(   _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + entry.offset)), bytes));
			else if constexpr (entry.size == 32) return kmask_zero(_mm256_cmpneq_epi64_mask(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset)), bytes));
			else if constexpr (entry.size == 64) return kmask_zero(_mm512_cmpneq_epi64_mask(_mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr + entry.offset)), bytes));
		}
#else
		//masked
		template<CmpEntry entry, usize...J>
		requires(entry.masked)
		INLINE static constexpr bool matchEntry($Size<1> auto* ptr, std::index_sequence<J...>)noexcept
		{
			static constexpr NBytes<entry.size> mask { Bytes::concat<NBytes<entry.size>>(elements[entry.offset + J].mask()...      ) };
			static constexpr NBytes<entry.size> bytes{ Bytes::concat<NBytes<entry.size>>(elements[entry.offset + J].maskedByte()...) };
			/* */if constexpr (entry.size <= 8 ) { return (*reinterpret_cast<const NBytes<entry.size>*>(ptr + entry.offset) & mask) == bytes; }
			else if constexpr (entry.size == 16) { const __m128i x{ _mm_xor_si128   (_mm_and_si128   (_mm_loadu_si128   (reinterpret_cast<const __m128i*>(ptr + entry.offset)), mask), bytes) }; return _mm_testz_si128   (x, x); }
			else if constexpr (entry.size == 32) { const __m256i x{ _mm256_xor_si256(_mm256_and_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset)), mask), bytes) }; return _mm256_testz_si256(x, x);}
		}
		//not masked
		template<CmpEntry entry, usize...J>
		requires(!entry.masked)
		INLINE static constexpr bool matchEntry($Size<1> auto* ptr, std::index_sequence<J...>)noexcept
		{
			static constexpr NBytes<entry.size> bytes{ Bytes::concat<NBytes<entry.size>>(*elements[entry.offset + J].byte...) };
			/* */if constexpr (entry.size <= 8 ) { return *reinterpret_cast<const NBytes<entry.size>*>(ptr + entry.offset) == bytes; }
			else if constexpr (entry.size == 16) { const __m128i x{ _mm_xor_si128   (_mm_loadu_si128   (reinterpret_cast<const __m128i*>(ptr + entry.offset)), bytes) }; return _mm_testz_si128   (x, x); }
			else if constexpr (entry.size == 32) { const __m256i x{ _mm256_xor_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset)), bytes) }; return _mm256_testz_si256(x, x); }
		}
#endif
		template<usize...I>
		INLINE static constexpr bool matchImpl($Size<1> auto* ptr, std::index_sequence<I...>)noexcept
		{
			return (matchEntry<entries[I]>(ptr, std::make_index_sequence<entries[I].size>{})&&...);
		}
	public:
		INLINE static constexpr bool match($Size<1> auto* ptr)noexcept
		{
			if consteval
			{
				for (const auto i: iota<elements.size()>)
				{
					const Element& e{ elements[i] };
					const u8 byte{ static_cast<u8>(ptr[i]) };
					const u8 low{ static_cast<u8>(byte & 0x0f) };
					const u8 high{ static_cast<u8>(static_cast<u8>(byte & 0xf0) >> 4) };
					if (e.low)
					{
						if (low != *e.low)return false;
					}
					if (e.high)
					{
						if (high != *e.high)return false;
					}
				}
				return true;
			}
			else
			{
				return matchImpl(ptr, std::make_index_sequence<entries.size()>{});
			}
		}
	};

	template<auto E0, auto CostTable0, auto E1, auto CostTable1>constexpr bool operator< (Sequence<E0, CostTable0> l, Sequence<E1, CostTable1> r)noexcept { return l.cost <  r.cost; }
	template<auto E0, auto CostTable0, auto E1, auto CostTable1>constexpr bool operator> (Sequence<E0, CostTable0> l, Sequence<E1, CostTable1> r)noexcept { return l.cost >  r.cost; }
	template<auto E0, auto CostTable0, auto E1, auto CostTable1>constexpr bool operator<=(Sequence<E0, CostTable0> l, Sequence<E1, CostTable1> r)noexcept { return l.cost <= r.cost; }
	template<auto E0, auto CostTable0, auto E1, auto CostTable1>constexpr bool operator>=(Sequence<E0, CostTable0> l, Sequence<E1, CostTable1> r)noexcept { return l.cost >= r.cost; }
	template<auto E0, auto CostTable0, auto E1, auto CostTable1>constexpr bool operator==(Sequence<E0, CostTable0> l, Sequence<E1, CostTable1> r)noexcept { return l.cost == r.cost; }
	template<auto E0, auto CostTable0, auto E1, auto CostTable1>constexpr bool operator!=(Sequence<E0, CostTable0> l, Sequence<E1, CostTable1> r)noexcept { return l.cost != r.cost; }

	template<std::array Elements, std::array CostTable, usize...Indexes>
	using deleteElement = Sequence<[]static
	{
		std::array<Element, Elements.size()> new_result{ Elements };
		((new_result[Indexes] = Element{}), ...);
		return new_result;
	}(), CostTable>;
	template<usize Index, std::array Elements, std::array CostTable>
	constexpr auto fastestCmpWithSkipsImpl(Sequence<Elements, CostTable> s)noexcept
	{
		static constexpr auto s0{ s };
		static constexpr auto s1{ deleteElement<Elements, CostTable, Index>{} };
		/* */if constexpr (s0 <= s1)return s0;
		else if constexpr (s1 <= s0)return s1;
		else STATIC_ERROR();
	}
	template<usize I0, usize I1, std::array Elements, std::array CostTable>
	constexpr auto fastestCmpWithSkipsImpl(Sequence<Elements, CostTable> s)noexcept
	{
		static constexpr auto s0{ s };
		static constexpr auto s1{ deleteElement<Elements, CostTable, I0>{} };
		static constexpr auto s2{ deleteElement<Elements, CostTable, I1>{} };
		static constexpr auto s3{ deleteElement<Elements, CostTable, I0,I1>{} };
		/* */if constexpr (s0 <= s1 && s0 <= s2 && s0 <= s3)return s0;
		else if constexpr (s1 <= s0 && s1 <= s2 && s1 <= s3)return s1;
		else if constexpr (s2 <= s0 && s2 <= s1 && s2 <= s3)return s2;
		else if constexpr (s3 <= s0 && s3 <= s1 && s3 <= s2)return s3;
		else STATIC_ERROR();
	}
	template<auto Seq, usize...Indexes>using FastestCmpWithSkips = decltype(fastestCmpWithSkipsImpl<Indexes...>(Seq));

	namespace details
	{
		constexpr bool isHexDigit(c8 c)noexcept{return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
		constexpr u8 char2hexDigit(c8 c)noexcept
		{
			if (c >= '0' && c <= '9') return c - '0';
			if (c >= 'a' && c <= 'f') return c - 'a' + 10;
			if (c >= 'A' && c <= 'F') return c - 'A' + 10;
			UNREACHABLE;
		}
	}

	template<FixedString str>
	constexpr std::array ida_pattern{ []static
	{
		static constexpr auto vec{[] static
		{
			constexpr usize size{ str.size()};
			const c8* pBegin{ str.data()};
			const c8* pEnd{ pBegin + size };
			ConstVector<Element, size> result;
			bool expectSpace{};
			for (const c8* p{ pBegin }; p != pEnd;)
			{
				const c8 c{ *p };
				if (c == ' ')
				{
					if (!expectSpace)UNREACHABLE;
					expectSpace = false;
					++p;
					if (p == pEnd)UNREACHABLE;							//" " last char being ' ' is not allowed
				}
				else
				{
					if (expectSpace)UNREACHABLE;
					expectSpace = true;
					if (c == '?')
					{
						if (p + 1 == pEnd)								//"?"
						{
							result.push_back({});
							break;
						}
						else
						{
							const c8 c1{ p[1] };
							if (c1 == ' ')								//"? "
							{
								p += 1;
								result.push_back({});
							}
							else if (c1 == '?')							//"??"
							{
								p += 2;
								result.push_back({});
							}
							else if (details::isHexDigit(c1))			//"?0"
							{
								result.push_back(Element{ details::char2hexDigit(c1), {} });
								p += 2;
							}
							else
								UNREACHABLE;
						}
					}
					else
					{
						if (!details::isHexDigit(c))UNREACHABLE;
						if (p + 1 == pEnd)								//"0"	single digit is not allowed
							UNREACHABLE;
						else
						{
							const c8 c1{ p[1] };
							if (c1 == ' ')								//"0 "	single digit is not allowed
								UNREACHABLE;
							else if (c1 == '?')							//"0?"
							{
								result.push_back(Element{ {}, details::char2hexDigit(c) });
								p += 2;
							}
							else if (details::isHexDigit(c1))			//"00"
							{
								result.push_back(static_cast<u8>(details::char2hexDigit(c) * 0x10 + details::char2hexDigit(c1)));
								p += 2;
							}
							else
								UNREACHABLE;
						}
					}
				}
			}
			return result;
		}()};
		return [] <usize...I>(std::index_sequence<I...>)static
		{
			return std::array<Element, vec.size()>{ vec[I]... };
		}(std::make_index_sequence<vec.size()>{});
	}()};

	template<FixedString str>
	requires(sizeof(typename decltype(str)::char_type) == 1)
	constexpr auto string_pattern{ [] <usize...I>(std::index_sequence<I...>)static
	{
		return std::array<Element, str.size()>{static_cast<u8>(str[I])...};
	}(std::make_index_sequence<str.size()>{}) };
}

template<FixedString str, auto CostTable>constexpr auto byte_pattern{ BytePattern::Sequence<BytePattern::ida_pattern<str>, CostTable>{} };
template<FixedString str, auto CostTable>constexpr auto string_pattern{ BytePattern::Sequence<BytePattern::string_pattern<str>, CostTable>{} };

//template<FixedString str>constexpr auto byte_pattern_simd{ byte_pattern<str, BytePattern::cmpsSimd> };
//template<FixedString str>constexpr auto string_pattern_simd{ string_pattern<str, BytePattern::cmpsSimd> };
template<FixedString str>constexpr auto byte_pattern_simd_preload{ byte_pattern<str, BytePattern::cmpsSimdPreload> };
template<FixedString str>constexpr auto string_pattern_simd_preload{ string_pattern<str, BytePattern::cmpsSimdPreload> };