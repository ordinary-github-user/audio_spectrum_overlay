#pragma once
#include <array>
#include <immintrin.h>
#include "Algorithm/Sort.h"
#include "Assert.h"
#include "Bits.h"
#include "ByteConcat.h"
#include "Concepts/Iterator.h"
#include "Concepts/Size.h"
#include "Concepts/String.h"
#include "DataStructures/ConstVector.h"
#include "DisableWarning.h"
#include "Iota.h"
#include "Kmask.h"
#include "Macro/Underlying.h"
#include "NBytes.h"
#include "SizedInt.h"
#include "TextEncoding/Wtf16CaseConversion.h"
namespace Strcmp
{
	enum class CmpType:u8
	{
		exact,
		masked,
		paired
	};
	enum class Encoding:u8
	{
		ascii,//including ascii8/ascii16/ascii32
		wtf16
	};
	struct CmpOption
	{
		u8 advance;					//in string length
		u8 size;					//in string length
		std::array<u64, 3> cost;	//[exact, masked, paired]
		constexpr u64 costOf(CmpType cmpType)const noexcept { return cost[UNDERLYING(cmpType)]; }
	};

	constexpr std::array cmpOptions1
	{
		 CmpOption{.advance{1 },.size{1 },.cost{10,20,21}}
		,CmpOption{.advance{2 },.size{2 },.cost{11,21,22}}
		,CmpOption{.advance{3 },.size{4 },.cost{12,22,23}}
		,CmpOption{.advance{4 },.size{4 },.cost{12,22,23}}
		,CmpOption{.advance{5 },.size{8 },.cost{13,23,24}}
		,CmpOption{.advance{6 },.size{8 },.cost{13,23,24}}
		,CmpOption{.advance{7 },.size{8 },.cost{13,23,24}}
		,CmpOption{.advance{8 },.size{8 },.cost{13,23,24}}
		,CmpOption{.advance{9 },.size{16},.cost{14,24,25}}
		,CmpOption{.advance{10},.size{16},.cost{14,24,25}}
		,CmpOption{.advance{11},.size{16},.cost{14,24,25}}
		,CmpOption{.advance{12},.size{16},.cost{14,24,25}}
		,CmpOption{.advance{13},.size{16},.cost{14,24,25}}
		,CmpOption{.advance{14},.size{16},.cost{14,24,25}}
		,CmpOption{.advance{15},.size{16},.cost{14,24,25}}
		,CmpOption{.advance{16},.size{16},.cost{14,24,25}}
#if defined(__AVX2__)
		,CmpOption{.advance{17},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{18},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{19},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{20},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{21},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{22},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{23},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{24},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{25},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{26},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{27},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{28},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{29},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{30},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{31},.size{32},.cost{15,25,26}}
		,CmpOption{.advance{32},.size{32},.cost{15,25,26}}
#endif
#if defined(__AVX512BW__) && defined(__AVX512VL__)
		,CmpOption{.advance{33},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{34},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{35},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{36},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{37},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{38},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{39},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{40},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{41},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{42},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{43},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{44},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{45},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{46},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{47},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{48},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{49},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{50},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{51},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{52},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{53},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{54},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{55},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{56},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{57},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{58},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{59},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{60},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{61},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{62},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{63},.size{64},.cost{16,26,27}}
		,CmpOption{.advance{64},.size{64},.cost{16,26,27}}
#endif
	};
	static_assert(cmpOptions1.size() == 8 ||cmpOptions1.size() == 16 || cmpOptions1.size() == 32 || cmpOptions1.size() == 64);
	template<$SizeAnyOf<1, 2, 4> T>
	constexpr auto cmpOptions{ []static
	{
		if constexpr (sizeof(T) == 1)return cmpOptions1;
		else
		{
			ConstVector<CmpOption, cmpOptions1.size()> result{};
			for (const CmpOption& choice : cmpOptions1)
				if (choice.advance % sizeof(T) == 0)
					result.push_back(CmpOption
					{
						.advance{static_cast<u8>(choice.advance / sizeof(T))},
						.size{static_cast<u8>(choice.size / sizeof(T))},
						.cost{choice.cost}
					});
			return result;
		}
	}() };

	template<class T>
	struct CmpEntry
	{
		T offset;				//in string length
		u8 size;				//in bytes
		CmpType cmpType;
		u64 cost;
		constexpr f64 efficiency() const noexcept { return (f64)size / (f64)cost; }
		constexpr bool simd()const noexcept { return size > 8 || (size >= 4 && cmpType == CmpType::paired); }
	};

	template<class T>
	struct DpEntry
	{
		u64 totalCost;
		CmpEntry<T> lastChoice;
	};

	template<Encoding encoding, $SizeAnyOf<1, 2, 4> Char>constexpr Char tolower(Char c)noexcept requires(encoding != Encoding::wtf16 || sizeof(Char) == 2) { if constexpr (encoding == Encoding::ascii) return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c; else if constexpr (encoding == Encoding::wtf16)return static_cast<Char>(Wtf16::tolower(static_cast<u16>(c))); }
	template<Encoding encoding, $SizeAnyOf<1, 2, 4> Char>constexpr Char toupper(Char c)noexcept requires(encoding != Encoding::wtf16 || sizeof(Char) == 2) { if constexpr (encoding == Encoding::ascii) return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c; else if constexpr (encoding == Encoding::wtf16)return static_cast<Char>(Wtf16::toupper(static_cast<u16>(c))); }
	template<Encoding encoding, $SizeAnyOf<1, 2, 4> Char>constexpr CmpType cmp_type(Char c)noexcept requires(encoding != Encoding::wtf16 || sizeof(Char) == 2)
	{
		using U = UnsignedBytes<sizeof(Char)>;
		const Char lower{ tolower<encoding>(c) };
		const Char upper{ toupper<encoding>(c) };
		if (lower == upper)return CmpType::exact;
		else return (1 == Bits::count(static_cast<U>(lower ^ upper))) ? CmpType::masked : CmpType::paired;
	}
	template<Encoding encoding, $SizeAnyOf<1, 2, 4> Char>constexpr UnsignedBytes<sizeof(Char)> maskof(Char c)noexcept requires(encoding != Encoding::wtf16 || sizeof(Char) == 2)
	{
		ASSERT(cmp_type<encoding>(c) != CmpType::paired);
		using U = UnsignedBytes<sizeof(Char)>;
		return static_cast<U>(~static_cast<U>(tolower<encoding>(c) ^ toupper<encoding>(c)));
	}
	template<Encoding encoding, $SizeAnyOf<1, 2, 4> Char>constexpr UnsignedBytes<sizeof(Char)> expected_after_masking(Char c)noexcept requires(encoding != Encoding::wtf16 || sizeof(Char) == 2)
	{
		ASSERT(cmp_type<encoding>(c) != CmpType::paired);
		using U = UnsignedBytes<sizeof(Char)>;
		const U lower{ static_cast<U>(tolower<encoding>(c)) };
		const U upper{ static_cast<U>(toupper<encoding>(c)) };
		return static_cast<U>(~static_cast<U>(lower ^ upper) & upper);
	}

	template<$Char T, Encoding encoding, bool case_sensitive, T...cs>
	requires(encoding != Encoding::wtf16 || sizeof(T) == 2)
	struct EqualImpl
	{
		static constexpr std::array<T, sizeof...(cs)> s{ cs... };
		using U = UnsignedBytes<sizeof(T)>;
		using Index = Compact<s.size()>;

		static constexpr ConstVector<CmpEntry<Index>, s.size()> entries{ []static ->ConstVector<CmpEntry<Index>, s.size()>
		{
			if constexpr (s.empty())return {};
			else
			{
				DpEntry<Index> dp[s.size() + 1]{};//dp[i] = min cost of [0,i)
	#if !defined(__INTELLISENSE__)
				for (const auto i : iota<1, s.size() + 1>)
				{
					bool first{ true };
					for (const CmpOption& choice : cmpOptions<T>)
					{
						if (i < choice.advance || static_cast<usize>(i - choice.advance + choice.size) > s.size())continue;
						CmpType cmpType{ CmpType::exact };
						if constexpr (!case_sensitive)
							for (Index j{ Index(i - choice.advance) }; j != i - choice.advance + choice.size; ++j)
							{
								const CmpType cmp{ cmp_type<encoding>(s[j]) };//cant switch until named loop
								if (cmp == CmpType::paired)
								{
									cmpType = CmpType::paired;
									break;
								}
								else if (cmp == CmpType::masked)
									cmpType = CmpType::masked;
							}
						const u64 newChoiceCost{ choice.costOf(cmpType) };
						const u64 newTotalCost{ dp[i - choice.advance].totalCost + newChoiceCost };
						if (first || newTotalCost < dp[i].totalCost)
						{
							dp[i] = DpEntry<Index>
							{
								.totalCost{newTotalCost},
								.lastChoice
								{
									.offset{Index(i - choice.advance)},
									.size{u8(sizeof(T) * choice.size)},
									.cmpType{cmpType},
									.cost{newChoiceCost}
								}
							};
							first = false;
						}
					}
				}
	#endif
				ConstVector<CmpEntry<Index>, s.size()> result;
				const DpEntry<Index>* pEntry{ dp + s.size() };
				while(pEntry->lastChoice.size)
				{
					result.push_back(pEntry->lastChoice);
					pEntry = dp + pEntry->lastChoice.offset;
				}
				Algorithm::sort(result,[](const CmpEntry<Index>& l, const CmpEntry<Index>& r) static
				{
					if (!l.simd() && r.simd())return true;
					if (l.simd() && !r.simd())return false;
					if (l.efficiency() > r.efficiency())return true;
					if (l.efficiency() < r.efficiency())return false;
					return l.offset < r.offset;
				});
				return result;
			}
		}() };
		static constexpr u64 cost{ [] static
		{
			u64 result{};
			for (const auto& e : entries)
				result += e.cost;
			return result;
		}()};
	private:

		template<CmpEntry entry, usize...J>
		requires(entry.cmpType == CmpType::masked)
		INLINE static constexpr bool matchEntry($Size<sizeof(T)> auto* ptr, std::index_sequence<J...>)noexcept
		{
			static constexpr NBytes<entry.size> mask    { Bytes::concat<NBytes<entry.size>>(maskof/*            */<encoding>(s[entry.offset + J])...) };
			static constexpr NBytes<entry.size> expected{ Bytes::concat<NBytes<entry.size>>(expected_after_masking<encoding>(s[entry.offset + J])...) };
			/* */if constexpr (entry.size <= 8 ) return (*reinterpret_cast<const NBytes<entry.size>*>(ptr + entry.offset) & mask) == expected;
#if defined(__AVX512BW__) && defined(__AVX512VL__)
			else if constexpr (entry.size == 16) return kmask_zero(   _mm_cmpneq_epi64_mask(   _mm_and_si128(   _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + entry.offset)), mask), expected));
			else if constexpr (entry.size == 32) return kmask_zero(_mm256_cmpneq_epi64_mask(_mm256_and_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset)), mask), expected));
			else if constexpr (entry.size == 64) return kmask_zero(_mm512_cmpneq_epi64_mask(_mm512_and_si512(_mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr + entry.offset)), mask), expected));
#else
			else if constexpr (entry.size == 16) { const __m128i x{    _mm_xor_si128(_mm_and_si128   (_mm_loadu_si128   (reinterpret_cast<const __m128i*>(ptr + entry.offset)), mask), expected) }; return _mm_testz_si128   (x, x); }
			else if constexpr (entry.size == 32) { const __m256i x{ _mm256_xor_si256(_mm256_and_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset)), mask), expected) }; return _mm256_testz_si256(x, x); }
#endif
		}
		template<CmpEntry entry, usize...J>
		requires(entry.cmpType == CmpType::exact)
		INLINE static constexpr bool matchEntry($Size<sizeof(T)> auto* ptr, std::index_sequence<J...>)noexcept
		{
			static constexpr NBytes<entry.size> bytes{ Bytes::concat<NBytes<entry.size>>((s[entry.offset + J])...) };
			/* */if constexpr (entry.size <= 8 ) return *reinterpret_cast<const NBytes<entry.size>*>(ptr + entry.offset) == bytes;
#if defined(__AVX512BW__) && defined(__AVX512VL__)
			else if constexpr (entry.size == 16) return kmask_zero(   _mm_cmpneq_epi64_mask(   _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + entry.offset)), bytes));
			else if constexpr (entry.size == 32) return kmask_zero(_mm256_cmpneq_epi64_mask(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset)), bytes));
			else if constexpr (entry.size == 64) return kmask_zero(_mm512_cmpneq_epi64_mask(_mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr + entry.offset)), bytes));
#else
			else if constexpr (entry.size == 16) { const __m128i x{    _mm_xor_si128(_mm_loadu_si128   (reinterpret_cast<const __m128i*>(ptr + entry.offset)), bytes) }; return _mm_testz_si128   (x, x); }
			else if constexpr (entry.size == 32) { const __m256i x{ _mm256_xor_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset)), bytes) }; return _mm256_testz_si256(x, x); }
#endif
		}
		template<CmpEntry entry, usize...J>
		requires(entry.cmpType == CmpType::paired)
		INLINE static constexpr bool matchEntry($Size<sizeof(T)> auto* ptr, std::index_sequence<J...>)noexcept
		{
			static constexpr auto lower{ [] static
			{
				/* */if constexpr (entry.size == 4) return Bytes::concat<__m128i>(Bytes::concat<u32>(tolower<encoding>(s[entry.offset + J])...), u32{}, u32{}, u32{});
				else if constexpr (entry.size == 8) return Bytes::concat<__m128i>(Bytes::concat<u64>(tolower<encoding>(s[entry.offset + J])...), u64{});
				else/*                           */ return Bytes::concat<NBytes<entry.size>>(tolower<encoding>(s[entry.offset + J])...);
			}() };
			static constexpr auto upper{ [] static
			{
				/* */if constexpr (entry.size == 4) return Bytes::concat<__m128i>(Bytes::concat<u32>(toupper<encoding>(s[entry.offset + J])...), u32{}, u32{}, u32{});
				else if constexpr (entry.size == 8) return Bytes::concat<__m128i>(Bytes::concat<u64>(toupper<encoding>(s[entry.offset + J])...), u64{});
				else/*                           */ return Bytes::concat<NBytes<entry.size>>(toupper<encoding>(s[entry.offset + J])...);
			}() };
			/* */if constexpr (entry.size == 2 )return (...&&(ptr[entry.offset + J] == constant<tolower<encoding>(s[entry.offset + J])> || ptr[entry.offset + J] == constant<toupper<encoding>(s[entry.offset + J])>));
#if defined(__AVX512BW__) && defined(__AVX512VL__)
			else if constexpr (entry.size == 4 ) { const auto x{   _mm_loadu_si32 (reinterpret_cast<const __m128i*>(ptr + entry.offset))}; return _ktestz_mask8_u8 (   _mm_cmpneq_epi16_mask(x, lower),    _mm_cmpneq_epi16_mask(x, upper)); }
			else if constexpr (entry.size == 8 ) { const auto x{   _mm_loadl_epi64(reinterpret_cast<const __m128i*>(ptr + entry.offset))}; return _ktestz_mask8_u8 (   _mm_cmpneq_epi16_mask(x, lower),    _mm_cmpneq_epi16_mask(x, upper)); }
			else if constexpr (entry.size == 16) { const auto x{   _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + entry.offset))}; return _ktestz_mask8_u8 (   _mm_cmpneq_epi16_mask(x, lower),    _mm_cmpneq_epi16_mask(x, upper)); }
			else if constexpr (entry.size == 32) { const auto x{_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset))}; return _ktestz_mask16_u8(_mm256_cmpneq_epi16_mask(x, lower), _mm256_cmpneq_epi16_mask(x, upper)); }
			else if constexpr (entry.size == 64) { const auto x{_mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr + entry.offset))}; return _ktestz_mask32_u8(_mm512_cmpneq_epi16_mask(x, lower), _mm512_cmpneq_epi16_mask(x, upper)); }
#else
			else if constexpr (entry.size == 4 ) { const auto x{   _mm_loadu_si32 (reinterpret_cast<const __m128i*>(ptr + entry.offset))}; return u16(-1) == (u16)_mm_movemask_epi8   (_mm_or_si128(_mm_cmpeq_epi16   (x, lower), _mm_cmpeq_epi16   (x, upper))); }
			else if constexpr (entry.size == 8 ) { const auto x{   _mm_loadl_epi64(reinterpret_cast<const __m128i*>(ptr + entry.offset))}; return u16(-1) == (u16)_mm_movemask_epi8   (_mm_or_si128(_mm_cmpeq_epi16   (x, lower), _mm_cmpeq_epi16   (x, upper))); }
			else if constexpr (entry.size == 16) { const auto x{   _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + entry.offset))}; return u16(-1) == (u16)_mm_movemask_epi8   (_mm_or_si128(_mm_cmpeq_epi16   (x, lower), _mm_cmpeq_epi16   (x, upper))); }
			else if constexpr (entry.size == 32) { const auto x{_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + entry.offset))}; return u32(-1) == (u32)_mm256_movemask_epi8(_mm_or_si256(_mm256_cmpeq_epi16(x, lower), _mm256_cmpeq_epi16(x, upper))); }
#endif
		}
		template<auto...I>
		INLINE static constexpr bool matchImpl($Size<sizeof(T)> auto* ptr, std::index_sequence<I...>)noexcept
		{
			return (... && matchEntry<entries[I]>(ptr, std::make_index_sequence<entries[I].size / sizeof(T)>{}));
		}
	public:
		INLINE static constexpr bool match($Size<sizeof(T)> auto* ptr)noexcept
		{
			if consteval
			{
				for (auto i{ s.size() }; i--;)
					if (case_sensitive ? 
						(__builtin_bit_cast(U, ptr[i]) != __builtin_bit_cast(U, s[i])) : 
						(__builtin_bit_cast(U, ptr[i]) != __builtin_bit_cast(U, tolower<encoding>(s[i])) && __builtin_bit_cast(U, ptr[i]) != __builtin_bit_cast(U, toupper<encoding>(s[i]))))
						return false;
				return true;
			}
			else { return matchImpl(ptr, std::make_index_sequence<entries.size()>{}); }
		}
	};

	template<$Char T, T...c>
	struct LessImpl
	{
		INLINE static constexpr bool match($IteratorOf<T> auto p, $IteratorOf<T> auto pEnd)noexcept
		{
			using U = UnsignedBytes<sizeof(T)>;
#if __cpp_expansion_statements >= 202506L
			template for (constexpr auto I : iota<sizeof...(c)>)
			{
			#if GCC//gcc bug
				(void)I;
			#endif
				if (p == pEnd) return false;
				if (__builtin_bit_cast(U, c...[I]) > __builtin_bit_cast(U, *p)) return false;
				if (__builtin_bit_cast(U, c...[I]) < __builtin_bit_cast(U, *p)) return true;
				++p;
			}
			return p != pEnd;
#else
			bool result;
			return (...&&(
				(p == pEnd || __builtin_bit_cast(U, c) > __builtin_bit_cast(U, *p) ? (result = false), false : true) &&
				(__builtin_bit_cast(U, c) < __builtin_bit_cast(U, *p) ? (result = true), false : true) &&
				(++p, true)
			)) ? p != pEnd : result;
#endif
		}
	};
	template<$Char T, T...c>
	struct GreaterImpl
	{
		INLINE static constexpr bool match($IteratorOf<T> auto p, $IteratorOf<T> auto pEnd)noexcept
		{
			using U = UnsignedBytes<sizeof(T)>;
#if __cpp_expansion_statements >= 202506L
			template for (constexpr auto I : iota<sizeof...(c)>)
			{
			#if GCC // gcc bug
				(void)I;
			#endif
				if (p == pEnd) return true;
				if (__builtin_bit_cast(U, c...[I]) > __builtin_bit_cast(U, *p)) return true;
				if (__builtin_bit_cast(U, c...[I]) < __builtin_bit_cast(U, *p)) return false;
				++p;
			}
			return false;
#else
			bool result;
			return (...&&(
				(p == pEnd || __builtin_bit_cast(U, c) > __builtin_bit_cast(U, *p) ? (result = true), false : true) &&
				(__builtin_bit_cast(U, c) < __builtin_bit_cast(U, *p) ? (result = false), false : true) &&
				(++p, true)
			)) ? false : result;
#endif
		}
	};
}