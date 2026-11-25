#pragma once
#include "../Bits.h"
#include "../ByteConcat.h"
#include "../Compact.h"
#include "../Constant.h"
#include "../NBytes.h"
#include "../NoUniqueAddress.h"
#include "../UnsignedDiv.h"
#pragma pack(push, 1)
template<usize, class>struct BitsetImpl;
template<usize N>using Bitset = BitsetImpl<N, std::make_index_sequence<N / 64>>;

template<>
struct BitsetImpl<0, std::index_sequence<>>
{
	static consteval u8 count()noexcept { return {}; }
	static consteval bool none()noexcept { return true; }
	static consteval bool any()noexcept { return false; }
};

template<>
struct BitsetImpl<1, std::index_sequence<>>
{
	NO_UNIQUE_ADDRESS bool bits;
	constexpr u8 count()const noexcept { return bits; }
	constexpr bool none()const noexcept { return !bits; }
	constexpr bool any()const noexcept { return bits; }
	template<usize Index>requires(Index == 0)constexpr bool at()const noexcept { return bits; }
	template<usize Index, bool value>requires(Index == 0)constexpr void set() noexcept { bits = value; }
};

template<usize N, usize...I>
requires(N >= 2 && N <= 64)
struct BitsetImpl<N, std::index_sequence<I...>>
{
private:
	using T = std::remove_pointer_t<decltype([]static
	{
		/* */if constexpr (N <= 8 )return static_cast<u8 *>(nullptr);
		else if constexpr (N <= 16)return static_cast<u16*>(nullptr);
		else if constexpr (N <= 32)return static_cast<u32*>(nullptr);
		else if constexpr (N <= 64)return static_cast<u64*>(nullptr);
	}())>;
public:
	NO_UNIQUE_ADDRESS T bits;
	constexpr Compact<N> count()const noexcept { return Bits::count(bits); }
	constexpr bool none()const noexcept { return bits == T{}; }
	constexpr bool any()const noexcept { return bits != T{}; }
	template<usize Index>
	requires(Index < N)
	constexpr bool at()const noexcept 
	{
		return static_cast<bool>(bits & constant<T(T{ 1 } << Index)>);
	}
	template<usize Index, bool value>
	requires(Index < N)
	constexpr void set() noexcept 
	{
		if constexpr (value)
			bits |= constant<T(T{ 1 } << Index)>;
		else
			bits &= constant<T(~T((T{ 1 } << Index)))>;
	}
};

template<usize N, usize...I>
requires(N > 64 && N % 64 == 0)
struct BitsetImpl<N, std::index_sequence<I...>>
{
	NO_UNIQUE_ADDRESS Bitset<64> bits[N / 64];
	constexpr Compact<N> count()const noexcept { return (Compact<N>{bits[I].count()} + ...); }
	constexpr bool none()const noexcept { return !(bits[I].bits | ...); }
	constexpr bool any()const noexcept { return (bits[I].bits | ...); }
	template<usize Index>
	requires(Index < N)
	constexpr bool at()const noexcept 
	{
		return bits[Index / 64].template at<Index % 64>();
	}
	template<usize Index, bool value>
	requires(Index < N)
	constexpr void set() noexcept 
	{
		bits[Index / 64].template set<Index % 64, value>();
	}
};

template<usize N, usize...I>
requires(N > 64 && N % 64 != 0)
struct BitsetImpl<N, std::index_sequence<I...>>
{
	NO_UNIQUE_ADDRESS Bitset<64> bits[N / 64];
	NO_UNIQUE_ADDRESS Bitset<N % 64> tail;
	constexpr Compact<N> count()const noexcept { return (Compact<N>{bits[I].count()} + ... + tail.count()); }
	constexpr bool none()const noexcept { return !(bits[I].bits | ... | u64{ tail.bits }); }
	constexpr bool any()const noexcept { return (bits[I].bits | ... | u64{ tail.bits }); }
	template<usize Index>
	requires(Index < N)
	constexpr bool at()const noexcept 
	{
		if constexpr (Index >= sizeof...(I) * 64)return tail.template at<Index % 64>();
		else return bits[Index / 64].template at<Index % 64>();
	}
	template<usize Index, bool value>
	requires(Index < N)
	constexpr void set() noexcept 
	{
		if constexpr (Index >= sizeof...(I) * 64)tail.template set<Index % 64, value>();
		else bits[Index / 64].template set<Index % 64, value>();
	}
};
#pragma pack(pop)

#if 0
#if defined(__AVX512VPOPCNTDQ__)
	static constexpr u8 packed_bitset_simd_width{ 64 };
#elif defined(__AVX2__)
	static constexpr u8 packed_bitset_simd_width{ 32 };
#else
	static constexpr u8 packed_bitset_simd_width{ 16 };
#endif

template<usize, class>struct PackedBitsetImpl;

template<usize N>
using PackedBitset = PackedBitsetImpl<N, std::make_index_sequence<[]static -> usize
{
	if constexpr (N == 0)return {};
	else
	{
		static constexpr usize byte_count    { unsigned_div_ceil<usize>(N, 8) };
		static constexpr usize max_block_size{ [] static -> usize
		{
			for (usize size{ packed_bitset_simd_width }; size; size /= 2)
				if (size <= byte_count)
					return size;
		}()};
		static constexpr usize tail_byte_count{ byte_count % max_block_size };
		return byte_count / max_block_size + (tail_byte_count != 0);
	}
}()>>;


template<>
struct PackedBitsetImpl<0, std::index_sequence<>>
{
	static consteval u8 count()noexcept { return {}; }
	static consteval bool none()noexcept { return true; }
	static consteval bool any()noexcept { return false; }
};

template<>
struct PackedBitsetImpl<1, std::index_sequence<0>>
{
	NO_UNIQUE_ADDRESS bool bits;
	constexpr u8 count()const noexcept { return bits; }
	constexpr bool none()const noexcept { return !bits; }
	constexpr bool any()const noexcept { return bits; }
	template<usize Index>requires(Index == 0)constexpr bool at()const noexcept { return bits; }
	template<usize Index, bool value>requires(Index == 0)constexpr void set() noexcept { bits = value; }
};


template<usize N, usize...I>
requires(N > 1)
struct PackedBitsetImpl<N, std::index_sequence<I...>>
{
	NO_UNIQUE_ADDRESS u8 bytes[unsigned_div_ceil<usize>(N, 8)];

	struct Block
	{
		usize index;//in bytes
		u8 size;	//in bytes
		u8 offset;	//in bytes
		//index = 8, size = 4, offset = 0: *(const u32*)(bytes+8)
		//index = 7, size = 4, offset = 1: *(const u32*)(bytes+7) & 0xffffff00
	};
	static constexpr std::array<Block, sizeof...(I)> blocks{[] static
	{
		static constexpr usize byte_count     { unsigned_div_ceil<usize>(N, 8) };
		static constexpr usize max_block_size { [] static->usize{for (usize size{ packed_bitset_simd_width }; size; size /= 2)if (size <= byte_count)return size; }() };
		static constexpr usize tail_byte_count{ byte_count % max_block_size };
		static constexpr usize tail_size      { !tail_byte_count ? max_block_size : [] static->usize{for (usize size{ packed_bitset_simd_width / 2 }; size; size /= 2) if (tail_byte_count > size) return size * 2; return usize{ 1 }; }() };
		return std::array<Block, sizeof...(I)>{
		{ 
			.index{ (I == sizeof...(I) - 1) ? byte_count - tail_size :I * max_block_size },
			.size{ (I == sizeof...(I) - 1) ? tail_size : max_block_size },
			.offset{ (I == sizeof...(I) - 1 && tail_byte_count) ? tail_size - tail_byte_count : 0 }
		}... };
	}() };

	constexpr Compact<N> count()const noexcept 
	{
		
	}
	constexpr bool none()const noexcept 
	{
		if consteval
		{
			for (const u8 byte : bytes)
				if (byte)
					return false;
			return true;
		}
		else { return (...&&!*(const NBytes<blocks[I].size>*)(bytes + blocks[I].index)); }
	}
	constexpr bool any()const noexcept 
	{
		if consteval
		{
			for (const u8 byte : bytes)
				if (byte)
					return true;
			return false;
		}
		else { return (...||*(const NBytes<blocks[I].size>*)(bytes + blocks[I].index)); }
	}

	template<usize Index>
	requires(Index < N)
	constexpr bool at()const noexcept 
	{
		return static_cast<bool>(bytes[Index / 8] & constant<u8(u8{ 1 } << (Index % 8))>);
	}

	template<usize Index, bool value>
	requires(Index < N)
	constexpr void set() noexcept 
	{
		if constexpr (value)
			bytes[Index / 8] |= constant<u8(u8{ 1 } << (Index % 8))>;
		else
			bytes[Index / 8] &= constant<u8(~u8(u8{ 1 } << (Index % 8)))>;
	}
};
#endif