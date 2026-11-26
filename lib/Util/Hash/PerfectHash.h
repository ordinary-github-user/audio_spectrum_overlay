#pragma once
#include "../ByteConcatScalar.h"
#include "../Compiler.h"
#include "../Concepts/Char.h"
#include "../Constant.h"
#include "../DataStructures/ConstVector.h"
#include "../DataStructures/DiffMatrix.h"
#include "../ElementAt.h"
#include "../Macro/Consteval.h"
#include "../Macro/If.h"
#include "../NextPower2.h"
#include "../SizedInt.h"
#include "../StringViewImpl.h"
namespace PerfectHashImpl
{
	using Cost = u64;
	static constexpr Cost shiftor_cost{ 50 };
	static constexpr Cost xor_cost{ 5 };
	static constexpr Cost clamp_cost{ 30 };//Front/Back past the shortest key: s[min(i, size - 1)] costs a compare + cmov on the address, not a divide
	static constexpr Cost size_cost{ 20 };//s.size() is already in a register
	//Extra cost the minimal stage adds when the raw hash is wider than 1 byte: a 1-byte hash indexes a 256-entry table directly,
	//a wider one needs pext or a multiply + shift first (+2..3 cycles measured). On 32-bit targets a hash wider than 4 bytes
	//also turns that multiply into a 3-multiply sequence.
	static constexpr Cost stage_cost{ 60 };
	static constexpr Cost wide_stage_cost{ sizeof(void*) < 8 ? 150 : 0 };
	template<$Char T>constexpr Cost minimal_stage_cost(u8 count)noexcept { return count * sizeof(T) <= 1 ? 0 : stage_cost + (count * sizeof(T) > 4 ? wide_stage_cost : 0); }
	template<$Char T>
	struct Size
	{
		static constexpr bool valid{ true };
		static constexpr u8 count{ 1 };
		static constexpr Cost cost{ size_cost };
		INLINE static constexpr UnsignedBytes<sizeof(T)> apply(auto&& s)noexcept { return static_cast<UnsignedBytes<sizeof(T)>>(s.size()); }
	};
	template<$Char T, usize index, u8 Count, usize InputMinLen>//Count: number of T
	struct Front
	{
		static constexpr bool valid{ sizeof(T) * Count <= sizeof(u64) && ((InputMinLen >= index + Count) || Count == 1) };
		static constexpr u8 count{ Count };
		static constexpr Cost cost{ 100 + (index != 0) + Count + ((InputMinLen >= index + Count) ? 0 : clamp_cost) };
		INLINE static constexpr UnsignedBytes<sizeof(T)* Count> apply(auto&& s)noexcept
		requires(valid)
		{
			using R = UnsignedBytes<sizeof(T)* Count>;
			using U = UnsignedBytes<sizeof(T)>;
			if constexpr (InputMinLen >= index + Count)
			{
				if consteval
				{
					/* */if constexpr (Count == 1) return Bytes::concat<R>((U)s[index]);
					else if constexpr (Count == 2) return Bytes::concat<R>((U)s[index], (U)s[index + 1]);
					else if constexpr (Count == 4) return Bytes::concat<R>((U)s[index], (U)s[index + 1], (U)s[index + 2], (U)s[index + 3]);
					else if constexpr (Count == 8) return Bytes::concat<R>((U)s[index], (U)s[index + 1], (U)s[index + 2], (U)s[index + 3], (U)s[index + 4], (U)s[index + 5], (U)s[index + 6], (U)s[index + 7]);
				}
				else { return *reinterpret_cast<const R*>(&s[index]); }
			}
			else if constexpr (Count == 1)
			{
				const auto size{ s.size() };
				using Index = std::remove_cvref_t<decltype(size)>;
				return static_cast<U>(s[size > index ? Index{ index } : Index(size - 1)]);
			}
		}
	};
	template<$Char T, usize index, u8 Count, usize InputMinLen>//Count: number of T
	struct Back
	{
		static constexpr bool valid{ sizeof(T) * Count <= sizeof(u64) && ((InputMinLen >= index + Count) || Count == 1) };
		static constexpr u8 count{ Count };
		static constexpr Cost cost{ 110 + Count + ((InputMinLen >= index + Count) ? 0 : clamp_cost) };
		INLINE static constexpr UnsignedBytes<sizeof(T)* Count> apply(auto&& s)noexcept
		requires(valid)
		{
			using R = UnsignedBytes<sizeof(T)* Count>;
			using U = UnsignedBytes<sizeof(T)>;
			if constexpr (InputMinLen >= index + Count)
			{
				const auto start{ static_cast<std::remove_cvref_t<decltype(s.size())>>(s.size() - compact<Count + index>) };
				if consteval
				{
					/* */if constexpr (Count == 1) return Bytes::concat<R>((U)s[start]);
					else if constexpr (Count == 2) return Bytes::concat<R>((U)s[start], (U)s[start + 1]);
					else if constexpr (Count == 4) return Bytes::concat<R>((U)s[start], (U)s[start + 1], (U)s[start + 2], (U)s[start + 3]);
					else if constexpr (Count == 8) return Bytes::concat<R>((U)s[start], (U)s[start + 1], (U)s[start + 2], (U)s[start + 3], (U)s[start + 4], (U)s[start + 5], (U)s[start + 6], (U)s[start + 7]);
				}
				else { return *reinterpret_cast<const R*>(&s[start]); }
			}
			else if constexpr (Count == 1)
			{
				const auto size{ s.size() };
				using Index = std::remove_cvref_t<decltype(size)>;
				return static_cast<U>(s[size > index ? Index(size - compact<1 + index>) : Index{}]);
			}
		}
	};
	template<$Char T, u8 Count, usize InputMinLen>//Count: number of T
	struct Mid
	{
		static constexpr bool valid{ sizeof(T) * Count <= sizeof(u64) && InputMinLen / 2 + Count <= InputMinLen };
		static constexpr u8 count{ Count };
		static constexpr Cost cost{ 115 + Count };
		INLINE static constexpr UnsignedBytes<sizeof(T)* Count> apply(auto&& s)noexcept
		requires(valid)
		{
			using R = UnsignedBytes<sizeof(T)* Count>;
			using U = UnsignedBytes<sizeof(T)>;
			const auto mid{ static_cast<std::remove_cvref_t<decltype(s.size())>>(s.size() / 2) };
			if consteval
			{
				/* */if constexpr (Count == 1) return Bytes::concat<R>((U)s[mid]);
				else if constexpr (Count == 2) return Bytes::concat<R>((U)s[mid], (U)s[mid + 1]);
				else if constexpr (Count == 4) return Bytes::concat<R>((U)s[mid], (U)s[mid + 1], (U)s[mid + 2], (U)s[mid + 3]);
				else if constexpr (Count == 8) return Bytes::concat<R>((U)s[mid], (U)s[mid + 1], (U)s[mid + 2], (U)s[mid + 3], (U)s[mid + 4], (U)s[mid + 5], (U)s[mid + 6], (U)s[mid + 7]);
			}
			else { return *reinterpret_cast<const R*>(&s[mid]); }
		}
	};
	template<class...Ts>
	struct Xor
	{
		static constexpr bool valid{ (...&&Ts::valid) };
		static constexpr u8 count{ Algorithm::max(Ts::count...) };
		static constexpr Cost cost{ ((xor_cost * (sizeof...(Ts) - 1)) + ... + Ts::cost) };
		INLINE static constexpr auto apply(auto&& s)noexcept
		requires(valid)
		{
			using Ret = UnsignedBytes<Algorithm::max(sizeof(Ts::apply(FWD(s)))...)>;
			return static_cast<Ret>((...^Ret{ Ts::apply(FWD(s)) }));
		}
	};
	template<class...>struct HasherImpl;
	template<$Char T, class...Feature, usize...I>
	struct HasherImpl<T, std::index_sequence<I...>, Feature...>
	{
		static constexpr Cost cost{ (Feature::cost + ...) + (sizeof...(Feature) - 1) * shiftor_cost };
		static constexpr auto bytes{ compact<(... + usize{ Feature::count }) * sizeof(T)>};
		using Result = UnsignedBytes<next_power2_greater_equal(bytes)>;
		INLINE static constexpr Result operator()(IF(MSVC, StringView<const T>, $StringOfCharSize<sizeof(T)> auto&&) input)noexcept//msvc bug: if input is auto&&, HasherImpl{}(s) always return 0, only HasherImpl::operator()(s) works
		{
			static constexpr std::array<u8, sizeof...(Feature)> offsets{ [] static//in bytes
			{
				std::array<u8, sizeof...(Feature)> result;//uninitialized
				u8 acc{};
				(..., ((result[I] = acc), (acc += sizeof(T) * Feature::count)));
				return result;
			}() };
			return (... | static_cast<Result>((static_cast<Result>(Feature::apply(FWD(input))) << constant<offsets[I] * 8>)));
		}
	};
	template<$Char T, class...Feature>
	using Hasher = HasherImpl<T, std::make_index_sequence<sizeof...(Feature)>, Feature...>;

	template<$Char T, usize InputMinLen, usize MaxLen, FixedString...Strings>
	requires(sizeof...(Strings) >= 2 && (...&&$Same<T, typename std::remove_cvref_t<decltype(Strings)>::char_type>) && InputMinLen != 0 && (...&&(Strings.size() >= InputMinLen)))
	struct Finder
	{
		using Length = Compact<MaxLen>;
		static constexpr auto N{ compact<sizeof...(Strings)> };
		using KeyId = Compact<N>;
		//One read of a candidate feature: Front/Back read `count` T at `index` from the start/end, Mid reads `count` T from the
		//middle, Size is the length. A candidate feature xors 1..max_parts reads (e.g. Front<0> ^ Back<3> ^ Size).
		struct Part
		{
			enum class Type : u8 { size, front, back, mid };
			Type type;
			u8 count;//number of T (1 for size)
			Length index;//front/back only
			static constexpr Part front(Length index, u8 count)noexcept { return { Type::front, count, index }; }
			static constexpr Part back(Length index, u8 count)noexcept { return { Type::back, count, index }; }
			static constexpr Part size()noexcept { return { Type::size, 1, 0 }; }
			static constexpr Part mid(u8 count)noexcept { return { Type::mid, count, 0 }; }
		};
		static constexpr u8 max_parts{ 4 };
		struct FeatureDesc
		{
			using Index = Compact<max_parts>;
			Part m_data[max_parts];
			Index m_size;
			constexpr FeatureDesc($Same<Part> auto...m_data)noexcept :m_data{ m_data... }, m_size{ sizeof...(m_data) } {}
			template<class Self>constexpr Like<Self, Part> front(this Self&& self) noexcept { return FWD(self).m_data[0]; }
			template<class Self>constexpr Like<Self, Part> back(this Self&& self) noexcept { return FWD(self).m_data[self.m_size - 1]; }
			template<class Self>constexpr Like<Self, Part> operator[](this Self&& self, Index index) noexcept { return FWD(self).m_data[index]; }
			template<class Self>constexpr LikePtr<Self, Part> data(this Self&& self) noexcept { return __builtin_addressof(self.m_data[0]); }
			template<class Self>constexpr LikePtr<Self, Part> begin(this Self&& self) noexcept { return __builtin_addressof(self.m_data[0]); }
			template<class Self>constexpr LikePtr<Self, Part> end(this Self&& self) noexcept { return self.begin() + self.m_size; }
			constexpr Index size()const noexcept { return m_size; }
			constexpr u8 count()const noexcept
			{
				u8 result{};
				for (const Part& p : *this)
					if (p.count > result)
						result = p.count;
				return result;
			}
			constexpr bool valid()const noexcept
			{
				for (const Part& p: *this)
					switch (p.type)
					{
						case Part::Type::front: [[fallthrough]];
						case Part::Type::back: if ((sizeof(T) * p.count > sizeof(u64)) || !(InputMinLen >= p.index + p.count || p.count == 1))return false; break;
						case Part::Type::mid:  if ((sizeof(T) * p.count > sizeof(u64)) || InputMinLen / 2 + p.count > InputMinLen)return false; break;
						case Part::Type::size: break;//always valid
					}
				return true;
			}
			constexpr Cost cost()const noexcept
			{
				Cost result{ xor_cost * (m_size - 1) };
				for (const Part& p : *this)
					switch (p.type)
					{
						case Part::Type::size:  result += size_cost; break;
						case Part::Type::front: result += 100 + (p.index != 0) + p.count + (InputMinLen >= p.index + p.count ? 0 : clamp_cost); break;
						case Part::Type::back:  result += 110 +                  p.count + (InputMinLen >= p.index + p.count ? 0 : clamp_cost); break;
						case Part::Type::mid:   result += 115 + p.count; break;
					}
				return result;
			}
			//result[i] = value of Feature::apply(keys[i]) (compile-time branch), zero-extended to u64, for all N keys at once.
			//Parts outside, keys inside: each part is decoded once per candidate instead of once per key
			constexpr std::array<u64, N> values()const noexcept
			{
				static constexpr StringView<const T> keys[]{ Strings... };
				using U = UnsignedBytes<sizeof(T)>;
				std::array<u64, N> result;
				for (auto i{ N }; i--;) result[i] = 0;
				for (const Part& p : *this)
				{
					const typename Part::Type type{ p.type };
					const u8 count{ p.count };
					const Length index{ p.index };
					if (type == Part::Type::size)
						for (auto i{ N }; i--;)
							result[i] ^= u64{ static_cast<U>(keys[i].size()) };
					else if (type != Part::Type::mid && InputMinLen < index + count)//clamped, count == 1
						for (auto i{ N }; i--;)
						{
							const Length size{ static_cast<Length>(keys[i].size()) };
							result[i] ^= u64{ static_cast<U>(keys[i][type == Part::Type::front ? (size > index ? index : size - 1) : (size > index ? size - 1 - index : 0)]) };
						}
					else for (auto i{ N }; i--;)
					{
						const Length size{ static_cast<Length>(keys[i].size()) };
						const T* s{ keys[i].data() + (type == Part::Type::front ? index : type == Part::Type::back ? size - (count + index) : size / 2)};
						for (u8 j{}; j != count; ++j) result[i] ^= u64{ static_cast<U>(s[j]) } << (j * sizeof(T) * 8);
					}
				}
				return result;
			}
		};
#if defined(__INTELLISENSE__)
		template<Part p>
		static consteval auto part_type()noexcept
		{
			/* */if constexpr (p.type == Part::Type::front)return Front<T, p.index, p.count, InputMinLen>{};
			else if constexpr (p.type == Part::Type::back) return Back<T, p.index, p.count, InputMinLen>{};
			else if constexpr (p.type == Part::Type::size) return Size<T>{};
			else /*                                     */ return Mid<T, p.count, InputMinLen>{};
		}
		template<class...Ts>static Xor<Ts...> xor_of(Ts...)noexcept;
		template<class OnePart>static OnePart xor_of(OnePart)noexcept;
		template<FeatureDesc f, usize...I>
		static auto feature_type(std::index_sequence<I...>)noexcept -> decltype(xor_of(part_type<f[I]>()...));
		template<FeatureDesc f>
		using FeatureType = decltype(feature_type<f>(std::make_index_sequence<f.size()>{}));
#else
		template<Part p>
		using type_of = decltype([]static
		{
			/* */if constexpr (p.type == Part::Type::front)return Front<T, p.index, p.count, InputMinLen>{};
			else if constexpr (p.type == Part::Type::back) return Back<T, p.index, p.count, InputMinLen>{};
			else if constexpr (p.type == Part::Type::size) return Size<T>{};
			else /*                                     */ return Mid<T, p.count, InputMinLen>{};
		}());
		template<FeatureDesc f>
		using FeatureType = decltype([]<usize...I>(std::index_sequence<I...>)static
		{
			if constexpr (sizeof...(I) == 1)
				return type_of<f[I...]>{};
			else
				return Xor<type_of<f[I]>...>{};
		}(std::make_index_sequence<f.size()>{}));
#endif
		//Every valid candidate feature, in the order the search considers them (among equally good choices the earlier one wins).
		//Returns how many there are. New kinds of features go here: any xor of up to
		//max_parts reads, each with its own type, offset and width. Compile time grows with the number of candidates (the
		//dominance pruning in features is quadratic in it), so prefer families linear in MaxLen.
		static constexpr usize enumerate_candidates($Same<FeatureDesc> auto*...pOut)noexcept
		{
			usize result{};
			const auto add{ [&](const FeatureDesc& f) 
			{
				if (!f.valid())return; 
				if constexpr(sizeof...(pOut)!=0) 
					(..., pOut)[result] = f;
				++result; 
			} };
			static constexpr u8 counts[]{ 1, 2, 4, 8 };
			add(FeatureDesc{ Part::size() });
			for (const u8 c : counts) add(FeatureDesc{ Part::front(0, c), Part::back(0, c) });
			for (const u8 c : counts) add(FeatureDesc{ Part::front(0, c), Part::size() });
			for (const u8 c : counts) add(FeatureDesc{ Part::front(0, c), Part::mid(c) });
			for (const u8 c : counts) add(FeatureDesc{ Part::back(0, c), Part::size() });
			for (const u8 c : counts) add(FeatureDesc{ Part::back(0, c), Part::mid(c) });
			for (const u8 c : counts) add(FeatureDesc{ Part::size(), Part::mid(c) });
			for (const u8 c : counts) add(FeatureDesc{ Part::front(0, c), Part::back(0, c), Part::size() });
			for (const u8 c : counts) add(FeatureDesc{ Part::front(0, c), Part::back(0, c), Part::mid(c) });
			for (const u8 c : counts) add(FeatureDesc{ Part::front(0, c), Part::size(), Part::mid(c) });
			for (const u8 c : counts) add(FeatureDesc{ Part::back(0, c), Part::size(), Part::mid(c) });
			for (const u8 c : counts) add(FeatureDesc{ Part::front(0, c), Part::back(0, c), Part::size(), Part::mid(c) });
			for (const u8 c : counts) add(FeatureDesc{ Part::mid(c) });
			for (const u8 c : counts) for (Length i{}; i != MaxLen; ++i) add(FeatureDesc{ Part::front(i, c) });
			for (const u8 c : counts) for (Length i{}; i != MaxLen; ++i) add(FeatureDesc{ Part::back(i, c) });
			//Optional: the first or last T xored with one more T at any offset (e.g. Front<0> ^ Front<8>). On 55 real key sets this
			//gave 1 set a 1-byte hash instead of 2 bytes, for about twice the compile time; every offset pair (quadratic in MaxLen)
			//found nothing more and took 16x as long.
			//for (Length i{ 1 }; i != MaxLen; ++i) add(FeatureDesc{ Part::front(0, 1), Part::front(i, 1) });
			//for (Length i{ 1 }; i != MaxLen; ++i) add(FeatureDesc{ Part::back(0, 1), Part::back(i, 1) });
			//for (Length i{ 1 }; i != MaxLen; ++i) add(FeatureDesc{ Part::front(0, 1), Part::back(i, 1) });
			//for (Length i{ 1 }; i != MaxLen; ++i) add(FeatureDesc{ Part::front(i, 1), Part::back(0, 1) });
			return result;
		}
		static constexpr auto nCandidates{ compact<enumerate_candidates()> };
		using CandidateId = Compact<nCandidates>;
		static constexpr std::array<FeatureDesc, nCandidates> candidates{ [] static
		{
			std::array<FeatureDesc, nCandidates> result{};
			enumerate_candidates(result.data());
			return result;
		}() };
		static constexpr auto nPairs{ compact<usize{ N } * (N - 1) / 2> };
		using Score = Compact<nPairs>;//number of pairs the feature separates
		struct Diffed { DiffMatrix<N> diff; Score score; };
		struct UFeature
		{
			DiffMatrix<N> diff;
			Cost cost;
			FeatureDesc desc;
			u8 count;
			Score score;
		};
		static constexpr Diffed diff_of(const FeatureDesc& f)noexcept
		{
			//group equal values with an open-addressing table (O(N), no sort): bucket holds the last member of its group (+1),
			//prev[] chains the earlier members, so every colliding pair is visited exactly once
			static constexpr u8 bucketBits{ static_cast<u8>(Bits::bitLength(usize{ N }) + 1) };
			static constexpr auto mask{ compact<(usize{ 1 } << bucketBits) - 1> };
			using Slot = Compact<mask>;
			const std::array<u64, N> v{ f.values() };
			KeyId prev[N];
			KeyId bucket[mask + 1]{};
			Diffed result{ {}, nPairs }; result.diff.fill();
			for (KeyId i{}; i != N; ++i)
			{
				const u64 x{ v[i] };
				Slot h{ static_cast<Slot>((x * 0x9E3779B97F4A7C15uLL) >> (64 - bucketBits)) };
				while (bucket[h] && v[bucket[h] - 1] != x) h = (h + 1) & mask;
				prev[i] = bucket[h];
				for (KeyId j{ bucket[h] }; j; j = prev[j - 1])
				{
					result.diff.set(static_cast<KeyId>(j - 1), static_cast<KeyId>(i), false);
					--result.score;
				}
				bucket[h] = static_cast<KeyId>(i + 1);
			}
			return result;
		}
		static constexpr std::array features{ [] static
		{
			static constexpr auto arr{ [] static
			{
				ConstVector<UFeature, nCandidates> all;
				//(cost, count) of the perfect features found so far: a later candidate that is no cheaper in both would be
				//removed by the pruning below anyway (it is dominated, or equivalent and later in the list), so its diff is skipped
				Cost perfectCost[nCandidates];
				u8 perfectCount[nCandidates];
				CandidateId nPerfect{};
				for (const FeatureDesc& f : candidates)
				{
					const Cost cost{ f.cost() };
					const u8 count{ f.count() };
					bool dominated{};
					for (CandidateId p{}; !dominated && p != nPerfect; ++p)
						dominated = perfectCost[p] <= cost && perfectCount[p] <= count;
					if (dominated)continue;
					const Diffed d{ diff_of(f) };
					if (!d.score)continue;//diff.empty()
					if (d.score == nPairs) { perfectCost[nPerfect] = cost; perfectCount[nPerfect] = count; ++nPerfect; }
					all.push_back(UFeature{ d.diff, cost, f, count, d.score });
				}
				//drop dominated features. Same outcome as erasing from the list while scanning it (a scans survivors in order and
				//removes every other survivor it dominates), but with flags on flat arrays: no element shifting, no container calls
				const CandidateId n{ all.size() };
				const UFeature* const pAll{ all.data() };
				bool removed[nCandidates]{};
				for (CandidateId a{}; a != n; ++a)
				{
					if (removed[a])continue;
					const UFeature& fa{ pAll[a] };
					for (CandidateId b{}; b != n; ++b)
					{
						if (b == a || removed[b])continue;
						const UFeature& fb{ pAll[b] };
						//(fa.diff | fb.diff) == fa.diff, decided by the pair counts whenever possible
						if (fa.cost <= fb.cost && fa.count <= fb.count && fb.score <= fa.score &&
							(fa.score == nPairs || (fb.score == fa.score ? fa.diff == fb.diff : covers(fa.diff, fb.diff))))
							removed[b] = true;
					}
				}
				ConstVector<UFeature, nCandidates> result;
				for (CandidateId i{}; i != n; ++i)
					if (!removed[i])
						result.push_back(all[i]);
				return result;
			}() };
			std::array<UFeature, arr.size()> result;
			for (auto i{ compact<result.size()> }; i--;)
				result[i] = arr[i];
			return result;
		}() };
		static CONSTEVAL auto operator()()noexcept
		{
			using FeatureId = Compact<features.size()>;
			using Result = ConstVector<FeatureId, features.size()>;
			static constexpr Result arr{ [] static
			{
				//Branch and bound over feature combinations: same visiting order, pruning and tie-breaking as before, but the
				//candidate lists live in one flat buffer per depth instead of ConstVectors passed by value at every node
				static constexpr FeatureId nFeatures{ static_cast<FeatureId>(features.size()) };
				static constexpr u8 maxDepth{ sizeof(u64) / sizeof(T) };//every feature has count >= 1
				static constexpr FeatureId none{ nFeatures };
				FeatureId pool[maxDepth + 1][nFeatures];//pool[d]: choices at depth d
				FeatureId poolSize[maxDepth + 1]{};
				FeatureId chosen[maxDepth];
				u8 nChosen{};
				FeatureId best[maxDepth];
				u8 nBest{};
				Cost bestCost{ Cost(-1) };
				u8 bestCount{ u8(-1) };
				for (FeatureId i{ nFeatures }; i--;) pool[0][i] = i;
				poolSize[0] = nFeatures;
				[&] (this auto&& self, u8 depth, const DiffMatrix<N> IF(MSVC,,&) totalDiff, u8 totalCount, Cost totalCost)//clang bug: do not add "->void"; msvc bug
				{
					if(false)return;//clang bug: not an integral constant expression
					for (FeatureId c{}; c != poolSize[depth]; ++c)
					{
						const FeatureId     id{ pool[depth][c] };
						const UFeature&     feature{ features[id] };
						const u8            newCount{ u8(totalCount + feature.count) };//no need to check if newCount exceed limit, because choices are pre-filtered
						const Cost          newCost{ Cost(totalCost + feature.cost + ((nChosen != 0) * shiftor_cost) + minimal_stage_cost<T>(newCount) - minimal_stage_cost<T>(totalCount)) };
						const DiffMatrix<N> newDiff{ totalDiff | feature.diff };
						if (newDiff == totalDiff || newCost > bestCost || (newCost == bestCost && (!newDiff.perfect() || newCount > bestCount)))continue;
						if (newDiff.perfect())
						{
							for (u8 k{}; k != nChosen; ++k) best[k] = chosen[k];
							best[nChosen] = id;
							nBest = static_cast<u8>(nChosen + 1);
							bestCost = newCost;
							bestCount = newCount;
							continue;
						}
						//pre-filter future choices into pool[depth + 1]
						FeatureId* next{ pool[depth + 1] };
						FeatureId nNext{};
						for (FeatureId k{ static_cast<FeatureId>(c + 1) }; k != poolSize[depth]; ++k)
						{
							const FeatureId p{ pool[depth][k] };
							const u8 futureCount{ u8(newCount + features[p].count) };
							const Cost futureCost{ Cost(newCost + features[p].cost + shiftor_cost + minimal_stage_cost<T>(futureCount) - minimal_stage_cost<T>(newCount)) };
							if (futureCount <= sizeof(u64) / sizeof(T) && (futureCost < bestCost || (futureCost == bestCost && perfect_union(newDiff, features[p].diff) && futureCount < bestCount)))
								next[nNext++] = p;
						}
						//if smallest 2 newChoices cant merge into perfect diff, and cost sum is greater than best cost, then delete
						FeatureId iCheapest0{ none };
						FeatureId iCheapest1{ none };
						for (FeatureId k{}; k != nNext; ++k)
						{
							const FeatureId idx{ next[k] };
							const Cost cst{ features[idx].cost };
							if (iCheapest0 == none || cst < features[iCheapest0].cost) { iCheapest1 = iCheapest0; iCheapest0 = idx; }
							else if (iCheapest1 == none || cst < features[iCheapest1].cost) iCheapest1 = idx;
						}
						FeatureId nFiltered{};//filter in place
						for (FeatureId k{}; k != nNext; ++k)
						{
							const FeatureId idx{ next[k] };
							const UFeature& f{ features[idx] };
							if (perfect_union(newDiff, f.diff)) { next[nFiltered++] = idx; continue; }
							const FeatureId iOther{ idx == iCheapest0 ? iCheapest1 : iCheapest0 };
							if (iOther == none) continue;
							const u8 futureCount{ u8(newCount + f.count + 1) };//"+1" is lower bound
							if (newCost + f.cost + features[iOther].cost + shiftor_cost * 2 + minimal_stage_cost<T>(futureCount) - minimal_stage_cost<T>(newCount) <= bestCost)next[nFiltered++] = idx;
						}
						poolSize[depth + 1] = nFiltered;
						chosen[nChosen++] = id;
						self(depth + 1, newDiff, newCount, newCost);
						--nChosen;
					}
				}({}, {}, {}, {});
				Result result;
				for (u8 k{}; k != nBest; ++k) result.push_back(best[k]);
				return result;
			}() };
			STATIC_ASSERT([]static
			{
				DiffMatrix<N> diff{};
				for (const FeatureId id : arr)
					diff = diff | features[id].diff;
				return diff.perfect();
			}());
			return[] <usize...I>(std::index_sequence<I...>)static
			{
				return HasherImpl<T, std::index_sequence<I...>, FeatureType<features[arr[I]].desc>...>{};
			}(std::make_index_sequence<arr.size()>{});
		}
	};
}
template<usize InputMinLen, FixedString...Strings>
#if !MSVC
	requires(sizeof...(Strings) != 0 && InputMinLen != 0 && (...&&(Strings.size() >= InputMinLen)))//msvc bug: error C1001: Internal compiler error
#endif
using PerfectHashEx = decltype(PerfectHashImpl::Finder<typename std::remove_cvref_t<decltype(element_at<0>(Strings...))>::char_type, InputMinLen, Algorithm::max(usize{ Strings.size() }...), Strings...>::operator()());
template<bool AssumeMinLen, FixedString...Strings>
#if !MSVC
	requires(sizeof...(Strings) != 0 && (...&&!Strings.empty()))//msvc bug: error C1001: Internal compiler error
#endif
using PerfectHash = decltype(PerfectHashImpl::Finder<typename std::remove_cvref_t<decltype(element_at<0>(Strings...))>::char_type, AssumeMinLen ? Algorithm::min(usize{ Strings.size() }...) : usize{ 1 }, Algorithm::max(usize{ Strings.size() }...), Strings...>::operator()());
STATIC_ASSERT([]static
{
	using Hasher = PerfectHash<
		true,				//inputs are assumed to be at least as long as shortest strings
		L"apple",			//which is "apple"
		L"banana",			//which has .size() of 5
		L"bananana"			//input with .size() below 5 is ub
	>;
	using Hash = Hasher::Result;
	constexpr Hash h0{ Hasher{}(StringView{L"apple"}) };				//doesn't have to be StringView, std::string/std::string_view works too
	constexpr Hash h1{ Hasher{}(StringView{L"banana"}) };				//or any type that satisfy $StringOfCharSize<sizeof(T)>, sizeof(T) is 1 in this case
	constexpr Hash h2{ Hasher::operator()(StringView{L"bananana"}) };	//operator() is static, you dont have to create a Hasher object
	if constexpr (h0 == h1)return false;
	if constexpr (h0 == h2)return false;
	if constexpr (h1 == h2)return false;
	return true;
}());
STATIC_ASSERT([]static
{
	using namespace PerfectHashImpl;
	if constexpr (Hasher<c8, Front<c8, 0, 1, 999>>::cost + Hasher<c8, Back<c8, 0, 1, 999>>::cost >= Hasher<c8, Front<c8, 0, 1, 999>, Back<c8, 0, 1, 999>>::cost)return false;//sum of individual cost is smaller than merged cost, because additional bit_or + bit_shift
	if constexpr (Hasher<c8, Front<c8, 0, 2, 999>>::cost + Hasher<c8, Back<c8, 0, 2, 999>>::cost >= Hasher<c8, Front<c8, 0, 2, 999>, Back<c8, 0, 2, 999>>::cost)return false;//sum of individual cost is smaller than merged cost, because additional bit_or + bit_shift
	if constexpr (Hasher<c8, Front<c8, 0, 4, 999>>::cost + Hasher<c8, Back<c8, 0, 4, 999>>::cost >= Hasher<c8, Front<c8, 0, 4, 999>, Back<c8, 0, 4, 999>>::cost)return false;//sum of individual cost is smaller than merged cost, because additional bit_or + bit_shift
	if constexpr (Hasher<c8, Xor<Front<c8, 0, 1, 999>, Back<c8, 0, 1, 999>>>::cost >= Hasher<c8, Front<c8, 0, 1, 999>, Back<c8, 0, 1, 999>>::cost)return false;//xor is cheaper than bit_or + bit_shift
	if constexpr (Hasher<c8, Xor<Front<c8, 0, 2, 999>, Back<c8, 0, 2, 999>>>::cost >= Hasher<c8, Front<c8, 0, 2, 999>, Back<c8, 0, 2, 999>>::cost)return false;//xor is cheaper than bit_or + bit_shift
	if constexpr (Hasher<c8, Xor<Front<c8, 0, 4, 999>, Back<c8, 0, 4, 999>>>::cost >= Hasher<c8, Front<c8, 0, 4, 999>, Back<c8, 0, 4, 999>>::cost)return false;//xor is cheaper than bit_or + bit_shift
	if constexpr (Hasher<c8, Size<c8>>::cost < PerfectHash<true, "enable", "vertical", "horizontal">::cost)return false;
	if constexpr (Hasher<c8, Front<c8, 0, 2, 2>, Back<c8, 0, 2, 2>>::cost < PerfectHash<true, "abstract", "any", "as", "assert", "asserts", "async", "await", "bigint", "boolean", "break", "case", "catch", "class", "const", "constructor", "continue", "debugger", "declare", "default", "delete", "do", "else", "enum", "export", "extends", "false", "finally", "for", "from", "function", "get", "global", "if", "implements", "import", "in", "infer", "instanceof", "interface", "intrinsic", "is", "keyof", "let", "module", "namespace", "never", "new", "null", "number", "object", "of", "out", "override", "package", "private", "protected", "public", "readonly", "require", "return", "set", "static", "string", "super", "switch", "symbol", "this", "throw", "true", "try", "type", "typeof", "undefined", "unique", "unknown", "var", "void", "while", "with", "yield">::cost)return false;
	if constexpr (Hasher<c8, Xor<Front<c8, 0, 4, 8>, Back<c8, 0, 4, 8>>>::cost < PerfectHash<true, "swapHandAsDoubleSwap", "onReload", "onSilencerInstall", "onSniperShot", "onHealthShot", "onGrenade", "doubleSwapWait", "doubleSwapRepeat", "reloadWait0", "reloadWait1", "reloadRepeat", "silencerWait0", "silencerWait1", "silencerRepeat", "sniperWait0", "sniperWait1", "sniperRepeat", "commandMinInterval">::cost)return false;
	return true;
}());