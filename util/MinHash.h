#pragma once
#include "Bits.h"
#include "CompileTimeRandomValue.h"
#include "ModularMultInverse.h"
#include "PerfectHash.h"
#include "Pow2.h"
#include "Splitmix.h"
#if !defined(MIN_HASH_PEXT)
	#define MIN_HASH_PEXT (BMI2&&!MSVC)
#endif
namespace MinHashImpl
{
	template<class Hasher, class T, FixedString...Strings>
	struct Table
	{
		static constexpr auto N{ compact<sizeof...(Strings)> };
		using Result = Compact<N - 1>;
		static constexpr auto table_size{ u64{ 1 } << (sizeof(typename Hasher::Result) * 8) };
		static constexpr std::array<u8, table_size> table{ [] <usize...I>(std::index_sequence<I...>)static
		{
			std::array<u8, table_size> result{};
			(..., (result[Hasher::operator()(Strings)] = I));
			return result;
		}(std::make_index_sequence<N>{}) };
		INLINE static constexpr Result operator()(IF(MSVC, StringView<const T>, $StringOfCharSize<sizeof(T)> auto&&) input)noexcept
		{
			return table[Hasher::operator()(input)];
		}
		STATIC_ASSERT([]<usize...I>(std::index_sequence<I...>)static
		{
			return (... && (operator()(Strings) == I));
		}(std::make_index_sequence<N>{}));
	};
	template<$UInt T>
	struct SequentialParameters
	{
		bool valid;
		bool ascending;
		T h0;
		T interval;
		constexpr operator bool()const noexcept { return valid; }
	};
	template<class Hasher, FixedString...Strings>
	constexpr SequentialParameters<typename Hasher::Result> sequential_parameters{ [] static->SequentialParameters<typename Hasher::Result>
	{
		static constexpr typename Hasher::Result raw[]{ Hasher::operator()(Strings)... };
		if constexpr (sizeof...(Strings) <= 1)return { .valid{ true }, .ascending{}, .h0{ raw[0] }, .interval{}  };
		else
		{
			static constexpr u64 interval{ u64{ raw[1] } - u64{ raw[0] } };
			for (auto i{ compact<sizeof...(Strings) - 1> }; i--;)
				if (raw[i + 1] != raw[i] + interval)
					return {};
			return {
				.valid{ true },
				.ascending{ raw[1] > raw[0] },
				.h0{ raw[0] },
				.interval{ raw[1] > raw[0] ? raw[1] - raw[0] : raw[0] - raw[1] }
			};
		}
	}() };
	template<SequentialParameters param, class Hasher, class T, FixedString...Strings>
	struct Sequential
	{
		static constexpr auto N{ compact<sizeof...(Strings)> };
		using Result = Compact<N - 1>;
		INLINE static constexpr Result operator()(IF(MSVC, StringView<const T>, $StringOfCharSize<sizeof(T)> auto&&) input)noexcept
		{
			using H = typename Hasher::Result;
			using U = std::conditional_t<(sizeof(H) < 4), u32, H>;
			if constexpr (N <= 1)return {};
			else if constexpr (N == 2)
			{
				static constexpr H raw[2]{ Hasher::operator()(Strings)... };
				static constexpr H only1{ static_cast<H>(raw[1] & ~raw[0]) }, only0{ static_cast<H>(raw[0] & ~raw[1]) };//bits set in just one of the two
				static constexpr auto top{ compact<sizeof(H) * 8 - 1> };
				/* */if constexpr ((only1 & 1) != 0)/*     */return static_cast<Result>(   Hasher::operator()(input) & 1);												//and
				else if constexpr (((only1 >> top) & 1) != 0)return static_cast<Result>(   Hasher::operator()(input) >> top);											//shr (H unsigned: already 0 or 1)
				else if constexpr ((only0 & 1) != 0)/*     */return static_cast<Result>( !(Hasher::operator()(input) & 1));												//not, and
				else if constexpr (only1 != 0)/*           */return static_cast<Result>(  (Hasher::operator()(input) >> constant<Bits::rightZeroCount(only1)>) & 1);	//shr, and
#if 1
				else/*                                     */return Hasher::operator()(input) != constant<raw[0]>;
#else
				else /*                                    */return static_cast<Result>(!((Hasher::operator()(input) >> constant<Bits::rightZeroCount(only0)>) & 1));	//shr, not, and
#endif
			}
			else if constexpr (Math::isPowerOf2(param.interval))
			{
				if constexpr (param.ascending)return static_cast<Result>(static_cast<H>(Hasher::operator()(input) - param.h0) / param.interval);
				else/*                      */return static_cast<Result>(static_cast<U>(U{ param.h0 } - U{ Hasher::operator()(input) }) / param.interval);
			}
			else
			{
				static constexpr u8 shift{ Bits::rightZeroCount(param.interval) };
				static constexpr U inv{ Math::modular_inverse(static_cast<U>(param.interval >> shift)) };
				if constexpr (N <= 1)return {};
				else if constexpr (param.ascending)return static_cast<Result>(((U{ Hasher::operator()(input) } - U{ param.h0 }) * inv) >> shift);
#if 1
				else /*                          */return static_cast<Result>(((U{ Hasher::operator()(input) } - U{ param.h0 }) * constant<U{} - inv>) >> shift);
#else
				else /*                          */return static_cast<Result>(((U{ param.h0 } - U{ Hasher::operator()(input) }) * inv) >> shift);//clang bad code gen
#endif
			}
		}
		STATIC_ASSERT([]<usize...I>(std::index_sequence<I...>)static
		{
			return (... && (operator()(Strings) == I));
		}(std::make_index_sequence<N>{}));
	};
#if MIN_HASH_PEXT
	template<u8 max_bits, class Hasher, FixedString...Strings>//max table size = 2^max_bits
	requires(max_bits < sizeof(void*) * 8)
	constexpr u64 pext_mask{ [] static -> u64//mask with the fewest bits that separates every key; 0 if every such mask has more than max_bits bits
	{
		static constexpr usize N{ sizeof...(Strings) };
		if constexpr (N > (usize{ 1 } << max_bits))
			return 0;
		else
		{
			using Cls = UnsignedBits<max_bits>;//holds pext(raw[i], mask) up to bit order, at most max_bits bits
			using Count = Compact<N>;
			static constexpr u64 raw[N]{ Hasher::operator()(Strings)... };
			static constexpr auto P{ compact<N * (N - 1) / 2> };
			using Pos = std::remove_const_t<decltype(P)>;
			u64 pairs[P];//raw[i] ^ raw[j] = the bits separating keys i and j, fewest bits first
			{
				usize start[66]{};
				for (usize i{}; i != N; ++i)
					for (usize j{ i + 1 }; j != N; ++j)
						++start[Bits::count(raw[i] ^ raw[j]) + 1];
				for (usize k{}; k != 65; ++k)
					start[k + 1] += start[k];
				for (usize i{}; i != N; ++i)
					for (usize j{ i + 1 }; j != N; ++j)
						pairs[start[Bits::count(raw[i] ^ raw[j])]++] = raw[i] ^ raw[j];
			}
			//Complete search: returns a mask of at most `limit` bits if one exists, 0 only if none does
			const auto search{ [&](this auto&& self, u8 limit, u64 mask, u64 excluded, u8 depth, Pos pos, const Cls* cls)//clang bug: no "-> u64", the body must be instantiated to deduce it
			{
				while (pos != P && (pairs[pos] & mask)) ++pos;
				if (pos == P) return mask;//every pair is separated
				if (depth == limit) return u64{};
				Count count[usize{ 1 } << max_bits];//cls[i] is pext(raw[i], mask) up to bit order
				for (usize c{}; c != usize{ 1 } << depth; ++c) count[c] = 0;
				for (usize i{}; i != N; ++i)
					if (++count[cls[i]] > (usize{ 1 } << (limit - depth))) return u64{};//too many keys agree on every chosen bit
				u64 tried{};
				for (u64 bits{ pairs[pos] & ~mask & ~excluded }; bits; bits &= bits - 1)
				{
					const u64 bit{ bits & (~bits + 1) };
					Cls next[N];
					for (usize i{}; i != N; ++i)
						next[i] = static_cast<Cls>(cls[i] << 1 | ((raw[i] & bit) != 0));
					if (const u64 found{ self(limit, mask | bit, excluded | tried, static_cast<u8>(depth + 1), static_cast<Pos>(pos + 1), next) }) return found;
					tried |= bit;//masks containing it were all covered by this branch
				}
				return u64{};
			} };
			//Iterative deepening: every smaller limit was searched exhaustively and failed, so the first hit has the minimum bit count.
			//Fewer than bitLength(N - 1) bits cannot give N distinct values, so start there.
			const Cls none[N]{};
			for (u8 limit{ Bits::bitLength(N - 1) }; limit <= max_bits; ++limit)
				if (const u64 found{ search(limit, {}, {}, {}, {}, none) })
					return found;
			return 0;
		}
	}() };
	template<u64 mask, class Hasher, class T, FixedString...Strings>
	requires(sizeof(typename Hasher::Result) > 1)
	struct Pext
	{
		static constexpr auto N{ compact<sizeof...(Strings)> };
		using Result = Compact<N - 1>;
		static constexpr std::array<Result, u64{ 1 } << Bits::count(mask)> table{[] <usize...I>(std::index_sequence<I...>)static
		{
			std::array<Result, u64{ 1 } << Bits::count(mask)> result{};
			(..., (result[Bits::pext<mask>(Hasher::operator()(Strings))] = I));
			return result;
		}(std::make_index_sequence<N>{}) };
		INLINE static constexpr Result operator()(IF(MSVC, StringView<const T>, $StringOfCharSize<sizeof(T)> auto&&) input)noexcept
		{
			return table[Bits::pext<mask>(Hasher::operator()(FWD(input)))];
		}
		STATIC_ASSERT([]<usize...I>(std::index_sequence<I...>)static
		{
			return (... && (operator()(Strings) == I));
		}(std::make_index_sequence<N>{}));
	};
#endif
	template<$UInt auto mul, u8 bits>
	NO_SIDE_EFFECTS INLINE constexpr UnsignedBits<bits> mulshift(std::remove_const_t<decltype(mul)> x)noexcept
	{
		return static_cast<UnsignedBits<bits>>((x * mul) >> compact<sizeof(mul) * 8 - bits>);
	}

	template<$AnyOf<u32, u64> T>
	struct MulShiftParameters
	{
		bool valid;
		u8 bits;
		T mul;
		constexpr operator bool()const noexcept { return valid; }
	};
	template<u8 max_bits, u64 expected_trials, class Hasher, FixedString...Strings>//smallest table where expected_trials trials succeed with 50%+ probability
	requires(max_bits < sizeof(void*) * 8 && sizeof...(Strings) > 1 && expected_trials > 0)
	constexpr MulShiftParameters<UnsignedBytes<(sizeof(typename Hasher::Result) <= 4 ? 4 : 8)>> mulshift_parameters{ [] static
	{
		using T = UnsignedBytes<(sizeof(typename Hasher::Result) <= 4 ? 4 : 8)>;
		static constexpr auto N{ compact<sizeof...(Strings)> };
		static constexpr T raw[N]{ Hasher::operator()(Strings)... };
		static constexpr u8 bits{ [] static
		{
			for (u8 b{ Bits::bitLength(usize{ N } - 1) }; b <= max_bits; ++b)
			{
				const f64 m{ 2 * static_cast<f64>(u64{ 1 } << (b - 1)) };
				f64 p{ 1 };
				for (Compact<N> i{ 1 }; i != N && p > 0; ++i)
					p *= 1 - static_cast<f64>(i) / m;
				//p = success rate of single trial
				f64 q{ 1 - p }, fail{ 1 };//fail = (1-p)^T via square-and-multiply; factors <= 1 so we can stop once fail <= 1/2
				for (auto e{ expected_trials }; e != 0 && fail > 0.5; e >>= 1, q *= q)
					if (e & 1)
						fail *= q;
				if (fail <= 0.5)//1 - (1-p)^T >= 1/2
					return b;
			}
			return static_cast<u8>(max_bits + 1);//doesn't fit
		}()};
		if constexpr (bits > max_bits)return MulShiftParameters<T>{};
		else
		{
			static constexpr u8 shift{ sizeof(T) * 8 - bits };
			T rng{ static_cast<T>(BUILDTIME_HASH_VALUE64) };
			for (;;)
			{
				const T mul{ splitmix(rng) | 1 };
				bool seen[usize{ 1 } << bits]{};
				bool ok{ true };
				for (Compact<N> i{}; ok && i != N; ++i)
				{
					const T slot{ (raw[i] * mul) >> shift };
					ok = !seen[slot];
					seen[slot] = true;
				}
				if (ok)
					return MulShiftParameters<T>{.valid{ 1 }, .bits{ bits }, .mul{ mul } };
			}
		}
	}() };
	template<MulShiftParameters param, class Hasher, class T, FixedString...Strings>
	requires(sizeof(typename Hasher::Result) > 1)
	struct MulShift
	{
		static constexpr auto N{ compact<sizeof...(Strings)> };
		using Result = Compact<N - 1>;
		static constexpr std::array<Result, usize{ 1 } << param.bits> table{ [] <usize...I>(std::index_sequence<I...>)static
		{
			std::array<Result, usize{ 1 } << param.bits> result{};
			(..., (result[mulshift<param.mul, param.bits>(Hasher::operator()(Strings))] = I));
			return result;
		}(std::make_index_sequence<N>{}) };
		INLINE static constexpr Result operator()(IF(MSVC, StringView<const T>, $StringOfCharSize<sizeof(T)> auto&&) input)noexcept
		{
			return table[mulshift<param.mul, param.bits>(Hasher::operator()(input))];
		}
		STATIC_ASSERT([]<usize...I>(std::index_sequence<I...>)static
		{
			return (... && (operator()(Strings) == I));
		}(std::make_index_sequence<N>{}));
	};
	//Minimal perfect hash: the i-th of Strings... maps to exactly i, and every input maps into [0, N).
	//CHM construction (Czech, Havas, Majewski 1992): key i is an edge (a(raw), b(raw)) of a random bipartite graph;
	//if that graph is a forest, g can be filled so that g[a] + g[b] == i (mod N).
	//Runtime: 2 multiplies, 2 independent loads from a (2 << bits)-entry table, add, cmov.
	template<$AnyOf<u32, u64> T>
	struct ChmParameters
	{
		bool valid;
		u8 bits;//2^bits vertices per side, table size = 2 << bits
		T mulA;
		T mulB;
		constexpr operator bool()const noexcept { return valid; }
	};
	constexpr f64 exp_neg(f64 x)noexcept//e^-x for x >= 0 (std::exp is constexpr only since C++26)
	{
		u8 squarings{};
		for (; x > 0.0625; x /= 2) ++squarings;//e^-x = (e^-(x/2))^2
		f64 result{ 1 }, term{ 1 };
		for (u8 i{ 1 }; i != 12; ++i) result += (term *= -x / i);
		for (; squarings; --squarings) result *= result;
		return result;
	}
	template<u8 max_bits, u64 expected_trials, class Hasher, FixedString...Strings>//smallest table where expected_trials trials succeed with 50%+ probability
	requires(max_bits < sizeof(void*) * 8 && sizeof...(Strings) > 1 && expected_trials > 0)
	constexpr ChmParameters<UnsignedBytes<(sizeof(typename Hasher::Result) <= 4 ? 4 : 8)>> chm_parameters{ [] static
	{
		using T = UnsignedBytes<(sizeof(typename Hasher::Result) <= 4 ? 4 : 8)>;
		static constexpr auto N{ compact<sizeof...(Strings)> };
		static constexpr T raw[N]{ Hasher::operator()(Strings)... };
		static constexpr u8 bits{ [] static
		{
			const f64 n{ static_cast<f64>(N) };
			//a forest on 2 << b vertices has fewer than 2 << b edges; the table has 2 << b entries, so b < max_bits
			for (u8 b{ static_cast<u8>(Bits::bitLength(usize{ N }) - 1) }; b < max_bits; ++b)
			{
				//p = success rate of single trial = P(random bipartite graph with N edges and m = 2^b vertices per side is a forest)
				const f64 m{ static_cast<f64>(u64{ 1 } << b) };
				f64 p{ 1 };
				for (Compact<N> i{ 1 }; i != N; ++i)//no 2-cycle (two keys on the same edge): birthday problem over m^2 edges
					p *= 1 - static_cast<f64>(i) / (m * m);
				//longer cycles (only even lengths exist): E[2k-cycles] = (m)_k^2 (N)_2k / (2k m^4k)
				f64 term{ 1 }, cycles{};
				for (u64 k{ 1 }; 2 * k <= N && cycles <= 64; ++k)
				{
					term *= (m - k + 1) / m * ((m - k + 1) / m) * ((n - 2 * k + 2) / m) * ((n - 2 * k + 1) / m);
					if (k != 1) cycles += term / static_cast<f64>(2 * k);
				}
				if (cycles > 64) continue;//p < 2e-28: no trial count reaches 50%
				p *= exp_neg(cycles);//Poisson approximation: accurate for N < m, pessimistic past that (cycles come in clusters)
				f64 q{ 1 - p }, fail{ 1 };//fail = (1-p)^T via square-and-multiply; factors <= 1 so we can stop once fail <= 1/2
				for (auto e{ expected_trials }; e != 0 && fail > 0.5; e >>= 1, q *= q)
					if (e & 1)
						fail *= q;
				if (fail <= 0.5)//1 - (1-p)^T >= 1/2
					return b;
			}
			return max_bits;//doesn't fit
		}() };
		if constexpr (bits >= max_bits)return ChmParameters<T>{};
		else
		{
			static constexpr u8 shift{ sizeof(T) * 8 - bits };
			static constexpr usize side{ usize{ 1 } << bits };
			using Vertex = Compact<2 * side - 1>;
			T rng{ static_cast<T>(BUILDTIME_HASH_VALUE64) };
			for (;;)
			{
				const T mulA{ splitmix(rng) | 1 }, mulB{ splitmix(rng) | 1 };
				//union-find over 2 * side vertices: a forest iff no key joins two vertices that are already connected
				Vertex parent[2 * side];
				for (usize v{}; v != 2 * side; ++v) parent[v] = static_cast<Vertex>(v);
				const auto find{ [&parent](Vertex v) { while (parent[v] != v) v = parent[v] = parent[parent[v]]; return v; } };
				bool ok{ true };
				for (Compact<N> i{}; ok && i != N; ++i)
				{
					const Vertex ra{ find(static_cast<Vertex>((raw[i] * mulA) >> shift)) };
					const Vertex rb{ find(static_cast<Vertex>(side + ((raw[i] * mulB) >> shift))) };
					parent[ra] = rb;
					ok = ra != rb;//else: cycle
				}
				if (ok)
					return ChmParameters<T>{.valid{ true }, .bits{ bits }, .mulA{ mulA }, .mulB{ mulB } };
			}
		}
	}() };
	template<ChmParameters param, class Hasher, class T, FixedString...Strings>
	struct CHM
	{
		static constexpr auto N{ compact<sizeof...(Strings)> };
		using Result = Compact<N - 1>;
		static constexpr usize side{ usize{ 1 } << param.bits };
		static constexpr std::array<Result, 2 * side> table{ [] static
		{
			using Mul = std::remove_const_t<decltype(param.mulA)>;
			static constexpr Mul raw[N]{ Hasher::operator()(Strings)... };
			usize ea[N], eb[N];//key i is the edge between left vertex ea[i] and right vertex eb[i]
			usize start[2 * side + 1]{};//adjacency lists (CSR)
			for (usize i{}; i != N; ++i)
			{
				ea[i] = mulshift<param.mulA, param.bits>(raw[i]);
				eb[i] = side + mulshift<param.mulB, param.bits>(raw[i]);
				++start[ea[i] + 1];
				++start[eb[i] + 1];
			}
			for (usize v{}; v != 2 * side; ++v) start[v + 1] += start[v];
			usize adj[2 * N];
			{
				usize next[2 * side];
				for (usize v{}; v != 2 * side; ++v) next[v] = start[v];
				for (usize i{}; i != N; ++i) { adj[next[ea[i]]++] = i; adj[next[eb[i]]++] = i; }
			}
			std::array<Result, 2 * side> g{};//vertices no key touches keep g = 0
			bool seen[2 * side]{};
			usize stack[2 * side];
			for (usize root{}; root != 2 * side; ++root)//walk each tree from a root with g = 0
			{
				if (seen[root]) continue;
				seen[root] = true;
				usize top{};
				stack[top++] = root;
				while (top)
					for (usize u{ stack[--top] }, k{ start[u] }; k != start[u + 1]; ++k)
					{
						const usize i{ adj[k] }, w{ ea[i] == u ? eb[i] : ea[i] };
						if (seen[w]) continue;//the parent: the graph is a forest
						seen[w] = true;
						g[w] = static_cast<Result>((i + N - g[u]) % N);//g[u] + g[w] == i (mod N)
						stack[top++] = w;
					}
			}
			return g;
		}() };
		INLINE static constexpr Result operator()(IF(MSVC, StringView<const T>, $StringOfCharSize<sizeof(T)> auto&&) input)noexcept
		{
			const auto raw{ Hasher::operator()(FWD(input)) };
			const usize sum{ usize{ table[mulshift<param.mulA, param.bits>(raw)] } + usize{ table[side + mulshift<param.mulB, param.bits>(raw)] } };
			if constexpr (Math::isPowerOf2(N)) return static_cast<Result>(sum & compact<N - 1>);
			else/*                           */return static_cast<Result>(sum >= N ? sum - N : sum);
		}
		STATIC_ASSERT([]<usize...I>(std::index_sequence<I...>)static
		{
			return (... && (operator()(Strings) == I));
		}(std::make_index_sequence<N>{}));
	};
	template<class PerfHash, class Char, FixedString...Strings>
	consteval auto select()noexcept//msvc bug: dont replace with lambda
	{
		static constexpr u8 max_bits{ 10 };//max table size = 2^max_bits
#if 1
		static constexpr SequentialParameters seq{ sequential_parameters<PerfHash, Strings...> };
		if constexpr (seq)return Sequential<seq, PerfHash, Char, Strings...>{};
		else if constexpr (sizeof(typename PerfHash::Result) == 1)return Table<PerfHash, Char, Strings...>{};
		else
		{
		#if MIN_HASH_PEXT
			static constexpr u64 mask{ pext_mask<max_bits, PerfHash, Strings...> };
			if constexpr (mask != 0)return Pext<mask, PerfHash, Char, Strings...>{};
			else
		#endif
			{
				static constexpr MulShiftParameters ms{ mulshift_parameters<max_bits, 0x2000, PerfHash, Strings...> };
				if constexpr (ms)return MulShift<ms, PerfHash, Char, Strings...>{};
				else
				{
					static constexpr ChmParameters chm{ chm_parameters<max_bits, 0x2000, PerfHash, Strings...> };
					if constexpr (chm)return CHM<chm, PerfHash, Char, Strings...>{};
					else STATIC_ERROR(too many strings for a table of 2^max_bits entries: raise max_bits);
				}
			}
		}
#else
		static constexpr ChmParameters chm{ chm_parameters<max_bits, 0x2000, PerfHash, Strings...> };
		return CHM<chm, PerfHash, Char, Strings...>{};
#endif
	}
}
template<bool AssumeMinLen, FixedString...Strings>
#if !MSVC
	requires(sizeof...(Strings) != 0 && (...&&!Strings.empty()))
#endif
using MinHash = decltype(MinHashImpl::select<PerfectHash<AssumeMinLen, Strings...>, typename std::remove_cvref_t<decltype(element_at<0>(Strings...))>::char_type, Strings...>());
STATIC_ASSERT([]static
{
	using Hasher = MinHash<true, "apple", "banana", "bananana", "cherry", "date", "elderberry", "fig", "grape", "honeydew">;
	if constexpr (Hasher::operator()(StringView{ "apple" }) != 0)return false;
	if constexpr (Hasher::operator()(StringView{ "banana" }) != 1)return false;
	if constexpr (Hasher::operator()(StringView{ "honeydew" }) != 8)return false;
	return true;
}());
#undef MIN_HASH_PEXT