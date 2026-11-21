#pragma once
#include "BasicTypes.h"
#include "Compact.h"
#include "Compiler.h"
#include "MacroIf.h"
#include "NextAligned.h"
#include "Pragma.h"
#if CLANG
	template<usize>struct DiffMatrix;
	PRAGMA_CLANG(clang diagnostic push); PRAGMA_CLANG(clang diagnostic ignored "-Wbit-int-extension");
	template<usize N>
	requires(N * (N - 1) / 2 <= __BITINT_MAXWIDTH__)
	struct DiffMatrix<N>
	{
		using Index = Compact<N>;
	private:
		static constexpr usize nBits{ N * (N - 1) / 2 };
		using Bits = unsigned _BitInt(nBits < 1 ? 1 : nBits);
	public:
		Bits bits{};
		//constexpr bool operator[](Index x, Index y)const noexcept { return static_cast<bool>((bits >> (x * (2 * N - 1 - x) / 2 + (y - x - 1))) & 1u); }
		constexpr void set(Index x, Index y, bool value) noexcept
		{
			if (x >= y)UNREACHABLE;
			const Bits m{ Bits{ 1 } << (x * (2 * N - 1 - x) / 2 + (y - x - 1)) };
			if (value) bits |= m; else bits &= ~m;
		}
		constexpr void fill()noexcept { bits = static_cast<Bits>(~Bits{}); }
		constexpr bool empty()const noexcept { return bits == 0; }
		constexpr bool perfect()const noexcept { return bits == static_cast<Bits>(~Bits{}); }
		constexpr friend DiffMatrix operator|(const DiffMatrix& l, const DiffMatrix& r)noexcept { DiffMatrix d; d.bits = l.bits | r.bits; return d; }
		constexpr friend bool perfect_union(const DiffMatrix& l, const DiffMatrix& r)noexcept { return static_cast<Bits>(l.bits | r.bits) == static_cast<Bits>(~Bits{}); }//(l | r).perfect()
		constexpr friend bool covers(const DiffMatrix& l, const DiffMatrix& r)noexcept { return static_cast<Bits>(r.bits & ~l.bits) == 0; }//(l | r) == l
		constexpr friend bool operator==(const DiffMatrix&, const DiffMatrix&)noexcept = default;
		constexpr friend bool operator!=(const DiffMatrix&, const DiffMatrix&)noexcept = default;
	};
	PRAGMA_CLANG(clang diagnostic pop);
#endif
template<usize N>
IF(CLANG, requires(N * (N - 1) / 2 > __BITINT_MAXWIDTH__)struct DiffMatrix<N>, struct DiffMatrix)
{
	using Index = Compact<N>;
private:
	using Chunk = u64;
	static constexpr auto nBits{ compact<N * (N - 1) / 2> };
	static constexpr u8 chunkSize{ sizeof(Chunk) * 8 };
	static constexpr auto nChunks{ compact<next_aligned_greater_equal<chunkSize>(usize{ nBits }) / chunkSize> };
	using ChunkIndex = Compact<nChunks>;
public:
	Chunk chunks[nChunks]{};
	//constexpr bool operator[](Index x, Index y)const noexcept
	//{
	//	const auto index{ x * (2 * N - 1 - x) / 2 + (y - x - 1) };
	//	return static_cast<bool>(chunks[index / chunkSize] & (Chunk{ 1 } << (index % chunkSize)));
	//}
	constexpr void set(Index x, Index y, bool value) noexcept
	{
		if (x >= y)UNREACHABLE;
		const auto index{ x * (2 * N - 1 - x) / 2 + (y - x - 1) };
		if(value)chunks[index / chunkSize] |= Chunk(Chunk{ 1 } << (index % chunkSize));
		else chunks[index / chunkSize] &= Chunk(~Chunk(Chunk{ 1 } << (index % chunkSize)));
	}
	constexpr void fill()noexcept
	{
		for (ChunkIndex i{}; i != nChunks; ++i) chunks[i] = Chunk(-1);
		if constexpr (nBits % chunkSize) chunks[nChunks - 1] = Chunk(Chunk{ 1 } << (nBits % chunkSize)) - 1;
	}
	constexpr bool empty()const noexcept { for (auto x : chunks)if (x)return false; return true; }
	constexpr bool perfect()const noexcept
	{
		static constexpr Chunk lastMask{ nBits % chunkSize ? Chunk(Chunk{ 1 } << (nBits % chunkSize)) - 1 : Chunk(-1) };
		for (ChunkIndex i{}; i != nChunks - 1; ++i)
			if (chunks[i] != Chunk(-1))
				return false;
		return chunks[nChunks - 1] == lastMask;
	}
	constexpr friend DiffMatrix operator|(const DiffMatrix& l, const DiffMatrix& r)noexcept
	{
		DiffMatrix result;
		for (ChunkIndex i{ nChunks }; i--;)
			result.chunks[i] = l.chunks[i] | r.chunks[i];
		return result;
	}
	constexpr friend bool perfect_union(const DiffMatrix& l, const DiffMatrix& r)noexcept//(l | r).perfect()
	{
		static constexpr Chunk lastMask{ nBits % chunkSize ? Chunk(Chunk{ 1 } << (nBits % chunkSize)) - 1 : Chunk(-1) };
		for (ChunkIndex i{}; i != nChunks - 1; ++i)
			if (Chunk(l.chunks[i] | r.chunks[i]) != Chunk(-1))
				return false;
		return Chunk(l.chunks[nChunks - 1] | r.chunks[nChunks - 1]) == lastMask;
	}
	constexpr friend bool covers(const DiffMatrix& l, const DiffMatrix& r)noexcept//(l | r) == l
	{
		for (ChunkIndex i{}; i != nChunks; ++i)
			if (r.chunks[i] & ~l.chunks[i])
				return false;
		return true;
	}
	constexpr friend bool operator==(const DiffMatrix&, const DiffMatrix&)noexcept = default;
	constexpr friend bool operator!=(const DiffMatrix&, const DiffMatrix&)noexcept = default;
};