#pragma once
#include "AlgorithmFill.h"
#include "BytePatternImpl.h"
#include "BytePatternSearchResult.h"
#include "ConceptLinearContainer.h"
#include "ConstVector.h"
#include "DisableWarning.h"
#include "Iota.h"
#include "MacroForEach.h"
#include "MacroIndexes.h"
namespace BytePattern
{
	struct SubSequence
	{
		usize iBegin{};
		usize length{};
	};
	struct JumpTable
	{
		bool valid;
		usize badCharOffset;
		usize defaultJumpDistance;
		ConstVector<u8, 0x100> badChars;
		std::array<usize, 0x100> data;
	};

	template<auto Elements, isize Offset>
	struct BoyerMooreScanner
	{
		using Pattern = Sequence<Elements, BytePattern::cmpsSimdPreload>;
		static constexpr isize offset{ Offset };
	private:
		static constexpr SubSequence longestSubSequenceWithoutWildcard{ []static
		{
			usize resultIndex{};
			usize resultLength{};
			usize iBegin{};
			usize length{};
			for (const auto index : iota<Elements.size()>)
			{
				if (!Elements[index].byte)
				{
					iBegin = index + 1;
					length = 0;
				}
				else
				{
					++length;
					if (length > resultLength)
					{
						resultIndex = iBegin;
						resultLength = length;
					}
				}
			}
			return SubSequence{ resultIndex, resultLength };
		}() };

		static constexpr JumpTable jumpTable{ [] static
		{
			constexpr usize beginIndex{ longestSubSequenceWithoutWildcard.iBegin };
			constexpr usize length{ longestSubSequenceWithoutWildcard.length };
			constexpr usize endIndex{ beginIndex + length - 1 };
			constexpr bool valid{length != 0};
			JumpTable result{
				.valid{valid},
				.badCharOffset{endIndex},
				.defaultJumpDistance{length},
				.badChars{},
				.data{}
			};
			if constexpr (valid)
			{
				Algorithm::fill(result.data, result.defaultJumpDistance);
				for (const auto i : iota<beginIndex, endIndex>)
					result.data[*Elements[i].byte] = endIndex - i;
				for (const auto badChar : iota<result.data.size()>)
					if (result.data[badChar] != result.defaultJumpDistance)
						result.badChars.push_back(badChar);
			}
			return result;
		}() };
	public:
#if 1
	#define BOYER_MOORE_JUMP_CASE(p,I) case I: {p += compact<jumpTable.data[I]>;break;}
	#define BOYER_MOORE_JUMP(p,bacChar)switch (badChar){FOR_EACH_EX(BOYER_MOORE_JUMP_CASE,p,INDEXES(256))}
#else
	#define BOYER_MOORE_JUMP(p,bacChar)Switch<jumpTable.badChars>(badChar,[&](auto I)LAMBDA_INLINE{ p += compact<jumpTable.data[I]>; },[&]LAMBDA_INLINE{ p += compact<jumpTable.defaultJumpDistance>; });
#endif

		template<$Size<1> Uint8>
		INLINE static constexpr Uint8* search_unbounded(Uint8* pBegin)noexcept
		{
			for (;;)
			{
				const u8 badChar{ pBegin[jumpTable.badCharOffset] };
				if (*Elements[jumpTable.badCharOffset].byte == badChar && FastestCmpWithSkips<Pattern{}, jumpTable.badCharOffset>::match(pBegin)) [[unlikely]]
					return pBegin + Offset;
				BOYER_MOORE_JUMP(pBegin, badChar);
			}
		}

		template<$Size<1> Uint8, $SigAnyOf<void(Uint8* const), bool(Uint8* const)>...Action>
		INLINE static constexpr Result<Uint8, Action...> search(Uint8* pBegin, usize size, Action&&...action)noexcept
		requires(sizeof...(action) <= 1)
		{
			if (size >= Elements.size()) [[likely]]//prevent pEnd underflow
			{
				Uint8* const pEnd{ pBegin + size - Elements.size() };
				while (pBegin <= pEnd) [[likely]]
				{
					const u8 badChar{ static_cast<u8>(pBegin[jumpTable.badCharOffset]) };
					if (*Elements[jumpTable.badCharOffset].byte == badChar && FastestCmpWithSkips<Pattern{}, jumpTable.badCharOffset>::match(pBegin)) [[unlikely]]
					{
						if constexpr (sizeof...(action) == 0)
							return pBegin + Offset;
						else if constexpr (($Sig<Action, void(Uint8* const)> && ...))
							(action(pBegin + Offset), ...);
						else
						{
							if (!(action(pBegin + Offset) && ...))
								return false;
						}
					}
					BOYER_MOORE_JUMP(pBegin, badChar);
				}
			}
			/* */if constexpr (sizeof...(action) == 0)return nullptr;
			else if constexpr ($Same<bool, Result<Uint8, Action...>>)return true;
		}
#undef BOYER_MOORE_JUMP_CASE
#undef BOYER_MOORE_JUMP
		template<$Size<1> Uint8, $SigAnyOf<void(Uint8* const), bool(Uint8* const)>...Action>INLINE static constexpr Result<Uint8, Action...> search(Uint8* pBegin, Uint8* pEnd, Action&&...action)noexcept { return search(pBegin, static_cast<usize>(pEnd - pBegin), action...); }
		template<$LinearContainerAnyOf<c8, u8> Container>INLINE static constexpr auto search(Container&& bytes, $SigAnyOf<void(ElementOf<Container>*), bool(ElementOf<Container>*)> auto&&...action)noexcept { return search(bytes.data(), bytes.size(), action...); }
	};
}

template<FixedString str, isize offset = 0>constexpr auto boyer_moore_byte_scanner{ BytePattern::BoyerMooreScanner<BytePattern::ida_pattern<str>, offset>{} };
template<FixedString str, isize offset = 0>constexpr auto boyer_moore_string_scanner{ BytePattern::BoyerMooreScanner<BytePattern::string_pattern<str>, offset>{} };