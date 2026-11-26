#pragma once
#include "lib/Audio/Beep.h"
#include "lib/Audio/Capture.h"
#include "lib/Graphics/Overlay/Overlay.h"
#include "lib/Math/BlackmanHarris.h"
#include "lib/Math/DegreeRadian.h"
#include "lib/Math/Hypot.h"
#include "lib/Math/Kaiser.h"
#include "lib/Math/Lerp.h"
#include "lib/Math/Numeric/CeilFloor.h"
#include "lib/Math/Numeric/Log.h"
#include "lib/Math/Numeric/Pow.h"
#include "lib/Math/Numeric/Round.h"
#include "lib/Math/Numeric/Signbit.h"
#include "lib/Math/SimdMath.h"
#include "lib/Util/DataStructures/DynamicArray.h"
#include "lib/Util/DataStructures/Heap.h"
#include "lib/Util/DataStructures/KeyState.h"
#include "lib/Util/DataStructures/LinearRing.h"
#include "lib/Util/DataStructures/MdArray.h"
#include "lib/Util/DataStructures/Span.h"
#include "lib/Util/DataStructures/StaticString.h"
#include "lib/Util/DataStructures/StaticVector.h"
#include "lib/Util/DirectoryMonitor.h"
#include "lib/Util/Macro/Global.h"
#include "lib/Util/Macro/Lexi.h"
#include "lib/Util/MusicalNotes.h"
#include "lib/Util/Repeating.h"
#include "lib/Util/Sjn.h"
#include "lib/Util/StringFormat/Num2Chars.h"
#include "lib/Util/StringFormat/StringBuilder.h"
#include "lib/Util/VariadicSwitch.h"
#include "lib/Windows/Exit.h"
#include "lib/Windows/File/File.h"
#include "lib/Windows/File/Filepath.h"
#include "lib/Windows/File/StaticFileMonitor.h"
#include "lib/Windows/Input/Keycode.h"
#include "lib/Windows/Input/RawInput.h"
namespace AudioSpectrum
{
#if defined(__AVX512F__)
	constexpr auto simd_width32{ compact<sizeof(__m512) / sizeof(f32)> };
#elif defined(__AVX2__)
	constexpr auto simd_width32{ compact<sizeof(__m256) / sizeof(f32)> };
#else
	constexpr auto simd_width32{ compact<sizeof(__m128) / sizeof(f32)> };
#endif
	constexpr Sjn::ParsingOptions parseOpts{ .RollBackOnFail = 0, .Comment = 1, .KeyVerify = 1, .SlashU = 0, .AllowUnknowns = 0, .AllowMissing = 0, .WtfVerify = 1 };
	constexpr Sjn::PrintingOptions printOpts{ .Pack = 0,.EnumFlagName = 0,.EnumFlagFields = 1 };
	static_assert(is_compatible<parseOpts, printOpts>);

	DEFINE_ENUM(WindowFunction, u8, rect, triangle, hann, kaiser, blackman_harris);
	DEFINE_ENUM(DisplayMode, u8, 
		complex,								//{real,imag}
		abs,									//hypot(complex)
		harmonic_product,						//f1 * f2 * f3 ... fn
		harmonic_product_normalized,			//harmonic_product^(1 / n)
		harmonic_product_skip_f0,				//f2 * f3 * ... f(n + 1)
		harmonic_product_skip_f0_normalized,	//harmonic_product_skip_f0^(1 / n)
		harmonic_product_odd,					//f1 * f3 * ... f(2n - 1)
		harmonic_product_odd_normalized			//harmonic_product_odd^(1 / n)
	);
	constexpr bool is_harmonic  (DisplayMode mode)noexcept { return Switch<Enum<DisplayMode>::entries>(mode, [](auto E)static consteval{return enum_name_fixed_string<E()>.template starts_with<"harmonic">(); }); }
	constexpr bool is_normalized(DisplayMode mode)noexcept { return Switch<Enum<DisplayMode>::entries>(mode, [](auto E)static consteval{return enum_name_fixed_string<E()>.template ends_with<"normalized">(); }); }
	constexpr bool is_odd       (DisplayMode mode)noexcept { return Switch<Enum<DisplayMode>::entries>(mode, [](auto E)static consteval{return enum_name_fixed_string<E()>.template starts_with<"harmonic_product_odd">(); }); }
	constexpr bool is_skip_f0   (DisplayMode mode)noexcept { return Switch<Enum<DisplayMode>::entries>(mode, [](auto E)static consteval{return enum_name_fixed_string<E()>.template starts_with<"harmonic_product_skip_f0">(); }); }
	constexpr u16 max_harmonic_multiple(DisplayMode mode, u8 nHarmonic)noexcept
	{
		ASSERT(is_harmonic(mode));
		switch (mode)
		{
			case DisplayMode::harmonic_product:						[[fallthrough]];
			case DisplayMode::harmonic_product_normalized:			return nHarmonic;
			case DisplayMode::harmonic_product_skip_f0:				[[fallthrough]];
			case DisplayMode::harmonic_product_skip_f0_normalized:	return static_cast<u16>(nHarmonic + 1);
			case DisplayMode::harmonic_product_odd:					[[fallthrough]];
			case DisplayMode::harmonic_product_odd_normalized:		return static_cast<u16>(nHarmonic * 2 - 1);
			default: UNREACHABLE;
		}
	}
	struct ConfigState
	{
		u8 valid : 1;
		u8 initialized : 1;
	};
	struct Config
	{
		struct Font
		{
			f32 size{0.05f};
			String16 familyName{ L"consolas" };
			bool valid()const noexcept { return Math::isFinite(size) && size > 0; }
		};
		struct Fps
		{
			bool enable{ true };
			Math::float2 position{ 0.045f, 0.04f };
			Font font;
			bool valid()const noexcept
			{
				return position[0] >= 0 && position[0] <= 1
					&& position[1] >= 0 && position[1] <= 1
					&& font.valid();
			}
		};
		struct Keybinds
		{
			DynamicArray<KeyId> exit{ KeyId::pause_break };
			DynamicArray<KeyId> boostDbInc{ KeyId::cap, KeyId::wheel_up };
			DynamicArray<KeyId> boostDbDec{ KeyId::cap, KeyId::wheel_down };
			DynamicArray<KeyId> showCursorNote{ KeyId::cap };
		};
		template<$Floating T>
		struct Pitch
		{
			T hz;
			DEFINE_LEXI(hz)
			template<auto...args>
			friend constexpr bool parse_impl(Pitch& result, Span<const c8>& input)noexcept
			{
				const c8 quote{ input[0] };
				if (quote != '"' && quote != '\'' && quote != '`')
					return Sjn::parse_impl<args...>(result.hz, input);
				const c8* const pBegin{ input.begin() + 1 };
				const c8* p{ pBegin };
				for (;; ++p)
				{
					if (p == input.end()) [[unlikely]] return false;
					if (*p == quote)break;
				}
				if (!note2f_impl(StringView8{ pBegin, p }, result.hz)) [[unlikely]] return false;
				input.m_pBegin = p + 1;
				return true;
			}
			friend consteval void sjn_print_colon(Pitch)noexcept;
			template<auto...args>
			friend constexpr void print_impl(auto& os, Pitch x)noexcept
			{
				const T midi{ f2midi(x.hz) };
				const T midi_rounded{ Math::round(midi) };
				if (midi == midi_rounded) [[likely]]
					Sjn::print_impl<args...>(os, midi2note<typename std::remove_cvref_t<decltype(os)>::char_type>(static_cast<i64>(midi_rounded)));
				else[[unlikely]]
					Sjn::print_impl<args...>(os, x.hz);
			}
		};
		struct Source
		{
			struct Frequency { Pitch<f32> min, max; };
			AudioCaptureType type{ AudioCaptureType::output };
			WindowFunction windowFunction{ WindowFunction::triangle };
			Frequency frequency
			{
			#if MSVC//msvc bug
				.min{ []static consteval {return note2f<"C3">;}() },
				.max{ []static consteval {return note2f<"C7">;}() }
			#else
				.min{ note2f<"C3"> },
				.max{ note2f<"C7"> }
			#endif
			};
			u8 nBinsPerOctave{ 128 };
			u8 nCyclePerWindow{ 16 };
			f32 boostDb{ 0 };
			bool valid()const noexcept
			{
				return nBinsPerOctave != 0
					&& Pitch<f32>{ 1 } <= frequency.min && frequency.min < frequency.max && Math::isFinite(frequency.max.hz)
					&& nCyclePerWindow != 0
					&& Math::isFinite(boostDb);
			}
		};
		struct Window 
		{
			struct Maximum
			{
				u8 n{ 4 };
				Font font;
			};
			bool enable{ true };
			usize sourceId{ 0 };
			//todo: monitorId
			f32 left{ 0 };	//[0,1]
			f32 right{ 1 };	//[0,1]
			f32 base;		//[0,1]
			DisplayMode displayMode;
			u8 nHarmonic;
			f32 scale;
			Opt<usize, usize(-1)> autoScaleWindowId{};
			f32 strokeWidth{ 0.003f };
			Hsla<f32> color;
			Maximum maximum{};
			bool valid()const noexcept
			{
				return left >= 0 && left <= 1
					&& right > left && right <= 1
					&& base >= 0 && base <= 1
					&& Math::isFinite(scale)
					&& (!is_harmonic(displayMode) || (nHarmonic != 0 && max_harmonic_multiple(displayMode, nHarmonic) <= u8(-1)))
					&& maximum.font.valid();
			}
		};
		bool beep{};
		Duration wait{ 100._ms };
		Keybinds keybinds{};
		DynamicArray<Source> sources{ Source{} };
		f32 globalScale{ 1.f };
		DynamicArray<Window> windows
		{
			Window{.base = 0.50f,.displayMode = DisplayMode::complex					,.nHarmonic=0,.scale = 10000		,.autoScaleWindowId =      {},.color{0.00f, 1.f,0.85f,.8f}},
			Window{.base = 0.05f,.displayMode = DisplayMode::harmonic_product			,.nHarmonic=5,.scale =-16000000000	,.autoScaleWindowId = usize{},.color{0.47f, 1.f,0.85f,.8f}},
			Window{.base = 0.95f,.displayMode = DisplayMode::harmonic_product_skip_f0	,.nHarmonic=4,.scale = 160000000	,.autoScaleWindowId = usize{},.color{0.11f, 1.f,0.85f,.8f}},
			Window{.base = 0.95f,.displayMode = DisplayMode::harmonic_product_odd		,.nHarmonic=4,.scale = 160000000	,.autoScaleWindowId = usize{},.color{0.36f, 1.f,0.85f,.8f}}
		};
		Fps fps{};
		bool valid()const noexcept
		{
			if (!Math::isFinite(globalScale) || globalScale <= 0.f) [[unlikely]] return false;
			if (!Math::isFinite(wait.count())) [[unlikely]] return false;
			for (const auto& source : sources)
				if (!source.valid()) [[unlikely]]
					return false;
			for (const auto& window : windows)
			{
				if (!window.valid() || window.sourceId >= sources.size()) [[unlikely]]
					return false;
				if (window.autoScaleWindowId)
					if (const auto id{ *window.autoScaleWindowId }; id == windows.index_of(window) || id >= windows.size() || !windows[id].enable || windows[id].autoScaleWindowId) [[unlikely]]
						return false;
			}
			if (!fps.valid()) [[unlikely]]
				return false;
			return true;
		}
	};

	using Input = RawInput<{.keyboard = 1, .mouse = 1, .mouseMove = 0, .lockState = 0, .allowFakeInput = 1 }>;
	using CfgMonitor = Windows::DynamicFileMonitor<Windows::NtPath>;
	GLOBAL CfgMonitor cfgMonitor{};
	GLOBAL Overlay<Input, Direct2D::Graphics> overlay;
	GLOBAL Config cfg;
	GLOBAL ConfigState cfgState{.valid=1,.initialized=0};
	GLOBAL UpdateRate<> updateRate;
	GLOBAL Direct2D::TextFormat textFormatFps{ cfg.fps.font.size * overlay.size()[1], cfg.fps.font.familyName };

	struct DotProductArrays
	{
		using Index = u32;
		struct Bin
		{
			const f32* s;			//aligned to 64/32/16 on avx512/avx256/default, front pad (next_aligned_greater_equal<simd_width32>(nSamples) - nSamples) f32s
			const f32* c;			//aligned to 64/32/16 on avx512/avx256/default, front pad (next_aligned_greater_equal<simd_width32>(nSamples) - nSamples) f32s
			usize size;				//s and c share the same size
		};
		DynamicArray<f32, Index> storage;//storage.data() is aligned to 64/32/16 on avx512/avx256/default, guaranteed by allocator
		DynamicArray<Bin, Index> factors;
		template<class Self>constexpr Like<Self, Bin> operator[](this Self&& self, Index index) noexcept { return FWD_LIKE(self, self.factors[index]); }
	};
	struct HarmonicIndex
	{
		u32 prev;
		u32 next;
		f32 ratio;
	};
	struct Source 
	{
		AudioCapture capture;
		LinearRing<f32> signal;
		DynamicArray<Math::float2, u32> frequencyBins;
		DynamicArray<f32, u32> absFrequencyBins;
		MdArray<HarmonicIndex, u32, u8> harmonicIndexes;
		DotProductArrays dotProductArrays;
		f32 binRatio;//binRatio^binsPerOctave=2
		f32 nOctavesPerBin;
		f32 fMin, fMaxDisplay, nOctavesDisplay;
		f32 boost{ 1.f };
		u8 nCyclePerWindow;
		u8 nHarmonicMax{};
		bool redraw{ true };
		INLINE u32 samplesPerSec()const noexcept { return capture.format().nSamplesPerSec; }
		INLINE u32 nChannels()const noexcept { return capture.format().nChannels; }
		INLINE f32 nyquist()const noexcept { return samplesPerSec() * .5f; }
		INLINE u32 binCountInternal()const noexcept { return frequencyBins.size(); }
		INLINE u32 binCountDisplay()const noexcept { return harmonicIndexes.extend<0>(); }
		INLINE f32 bin2f(f32 index)const noexcept { return fMin * Math::pow(binRatio, index); }
		INLINE f32 bin2f(u32 index)const noexcept { return bin2f(f32(index)); }
		INLINE f32 f2bin(f32 f)const noexcept { return Math::log2(f / fMin) / nOctavesPerBin; }
		INLINE f32 f2octaveDistToFmin(f32 f)const noexcept { return Math::log2(f / fMin); }
#if 1
		INLINE u32 f2NSamples(f32 f)const noexcept { return (u32)Math::ceil(samplesPerSec() * nCyclePerWindow / f); }
#else
		INLINE u32 f2NSamples(f32 f)const noexcept { return next_aligned_greater_equal<simd_width32>((u32)Math::ceil(samplesPerSec() * nCyclePerWindow / f)); }
#endif
		INLINE f32 f2delaySec(f32 f)const noexcept { return (f32)f2NSamples(f) / samplesPerSec(); }
		INLINE f32 f2x(f32 f)const noexcept { return Math::log2(f / fMin) / nOctavesDisplay; }
		INLINE f32 x2f(f32 x)const noexcept { return fMin * Math::pow(fMaxDisplay / fMin, x); }
		INLINE void setBoostDb(f32 boostDb)noexcept
		{
			if (const f32 newBoost{ Math::pow(10.f, boostDb / 20.f) }; boost != newBoost)
			{
				const f32 factor{ newBoost/boost };
				for (f32& x : signal)x *= factor;
				boost = newBoost;
			}
		}
		INLINE bool update(bool cfgChanged)noexcept
		{
			bool hasNewFrames{};
			if (!capture.read([&](const u8* pData, u32 frames, AudioCaptureResult type)LAMBDA_INLINE
				{
					hasNewFrames |= (frames != 0);
					if (type == AudioCaptureResult::normal) [[likely]]
					{
						f32 const* p{ reinterpret_cast<const f32*>(pData) };
						for (const auto i : iota_up(frames))
						{
							f32 acc{};
							for (const auto c : iota_up(nChannels())) acc += p[i * nChannels() + c];
							signal.emplace_back(acc * boost / nChannels());
						}
					}
					else [[unlikely]]
						signal.append(repeating<0.f>(frames));
				}))
				return false;
			redraw = cfgChanged || hasNewFrames;
			if (redraw)
				for (const auto iBin : iota_up(binCountInternal()))
				{
					const DotProductArrays::Bin& dotProducts{ dotProductArrays[iBin] };
					const f32* __restrict pC{ dotProducts.c };
					const f32* __restrict pS{ dotProducts.s };
					Math::float2& __restrict result{ frequencyBins[iBin] };
					const f32* pSignal{ signal.end() - dotProducts.size };
					const f32* pSignalEnd{ signal.end() };
#if defined(__AVX512F__)
					{
						__m512 accReal{ _mm512_setzero_ps() };
						__m512 accImag{ _mm512_setzero_ps() };
						for (; pSignal != pSignalEnd; pSignal += simd_width32, pC += simd_width32, pS += simd_width32)
						{
							const __m512 vSignal{ _mm512_loadu_ps(pSignal) };
							accReal = _mm512_fmadd_ps(_mm512_loadu_ps(pC), vSignal, accReal);
							accImag = _mm512_fmadd_ps(_mm512_loadu_ps(pS), vSignal, accImag);
						}
						result[0] = _mm512_reduce_add_ps(accReal);
						result[1] = _mm512_reduce_add_ps(accImag);
					}
#elif defined(__AVX2__)
					{
						__m256 accReal{ _mm256_setzero_ps() };
						__m256 accImag{ _mm256_setzero_ps() };
						for (; pSignal != pSignalEnd; pSignal += simd_width32, pC += simd_width32, pS += simd_width32)
						{
							const __m256 vSignal{ _mm256_loadu_ps(pSignal) };
							accReal = _mm256_fmadd_ps(_mm256_loadu_ps(pC), vSignal, accReal);
							accImag = _mm256_fmadd_ps(_mm256_loadu_ps(pS), vSignal, accImag);
						}
						result[0] = _mm256_reduce_add_ps(accReal);
						result[1] = _mm256_reduce_add_ps(accImag);
					}
#else
					{
						__m128 accReal{ _mm_setzero_ps() };
						__m128 accImag{ _mm_setzero_ps() };
						for (; pSignal != pSignalEnd; pSignal += simd_width32, pC += simd_width32, pS += simd_width32)
						{
							const __m128 vSignal{ _mm_loadu_ps(pSignal) };
							accReal = _mm_fmadd_ps(_mm_loadu_ps(pC), vSignal, accReal);
							accImag = _mm_fmadd_ps(_mm_loadu_ps(pS), vSignal, accImag);
						}
						result[0] = _mm_reduce_add_ps(accReal);
						result[1] = _mm_reduce_add_ps(accImag);
					}
#endif
					absFrequencyBins[iBin] = Math::hypot(result[0], result[1]);
				}
			return true;
		}
	};
	struct Window
	{
		bool enable;
		Direct2D::TextFormat textFormat{uninitialized};
		Source const* pSource;//never null
		Window const* pAutoScaleTarget;//nullable
		f32 left;
		f32 right;
		f32 base;
		f32 strokeWidth;
		Rgba<f32> color;
		DynamicArray<Math::float2, u32> points;
		DisplayMode displayMode;
		u8 nHarmonic;
		u8 nMaximums;
		f32 scale;
		Direct2D::SimplePathGeometryHollow geometry;
		f32 maxHeight;
		struct LocalMaximum
		{
			f32 h;
			f32 iBin;
			DEFINE_LEXI(h, iBin)
		};
		Heap<StaticVector<LocalMaximum, 0x100>, Less> maximums;
		void render_maximums()noexcept
		{
			for (const LocalMaximum& maximum : maximums)
			{
				const f32 f{ pSource->bin2f(maximum.iBin) };
				const f32 x{ pSource->f2x(f) };
				const Direct2D::TextLayout layout{ f2closestNote<c16>(f), textFormat };
				const f32 ratio{ maximum.h / maxHeight };
				const Rgba<f32> c{ [&] LAMBDA_INLINE
				{
					Rgba<f32> result{ color };
					result.a = Math::isFinite(ratio) ? Math::clamp(ratio, 0.f, 1.f): 0.f;
					return result;
				}()};
				overlay.graphics().drawTextLayout<Direct2D::Alignment{ Direct2D::Alignment::Horizontal::center, Direct2D::Alignment::Vertical::center }>({ Math::bezier(x, left, right), base }, layout, c);
			}
		}
		template<DisplayMode mode>
		void render_impl()noexcept
		{
			Math::float2* __restrict p{ points.data() };
			if constexpr (mode == DisplayMode::complex)
			{
				for (const auto iBin : iota_up(pSource->binCountDisplay()))
				{
					const f32 h{ pSource->frequencyBins[iBin][0] };
					maxHeight = Algorithm::max(maxHeight, Math::abs(h));
					*p++ = Math::float2{ Math::bezier(pSource->f2x(pSource->bin2f(iBin)), left, right), h };
				}
				for (const auto iBin : reversed(iota_up(pSource->binCountDisplay())))
				{
					const f32 h{ pSource->frequencyBins[iBin][1] };
					maxHeight = Algorithm::max(maxHeight, Math::abs(h));
					*p++ = Math::float2{ Math::bezier(pSource->f2x(pSource->bin2f(iBin)), left, right), h };
				}
			}
			else for (const auto iBin : iota_up(pSource->binCountDisplay()))
			{
				f32 h;
				if constexpr (mode == DisplayMode::abs)
					h = pSource->absFrequencyBins[iBin];
				else if constexpr (is_harmonic(mode))
				{
					if constexpr (is_skip_f0(mode))h = 1.f;
					else h = pSource->absFrequencyBins[iBin];
					const HarmonicIndex* pJBin{ pSource->harmonicIndexes.ptr(iBin, is_odd(mode)) };
					const HarmonicIndex* pJBinEnd;
					/* */if constexpr (mode == DisplayMode::harmonic_product || mode == DisplayMode::harmonic_product_normalized)	pJBinEnd = pJBin + nHarmonic - 1;
					else if constexpr (is_skip_f0(mode))																			pJBinEnd = pJBin + nHarmonic;
					else if constexpr (is_odd(mode))																				pJBinEnd = pJBin + (nHarmonic - 1) * 2;
					else STATIC_ERROR();
					for (; pJBin != pJBinEnd; pJBin += constant<is_odd(mode) ? 2 : 1>)
						h *= Math::bezier(pJBin->ratio, pSource->absFrequencyBins[pJBin->prev], pSource->absFrequencyBins[pJBin->next]);
					if constexpr (is_normalized(mode))h = Math::pow(h, 1.f / nHarmonic);
				}
				else STATIC_ERROR();
				if (h > maxHeight)maxHeight = h;
				*p++ = Math::float2{ Math::bezier(pSource->f2x(pSource->bin2f(iBin)), left, right), h };
			}
		}
		void render()noexcept
		{
			if (pSource->redraw || (pAutoScaleTarget && pAutoScaleTarget->pSource->redraw))
			{
				maximums.clear();
				maxHeight = {};
				switch (displayMode)
				{
					case DisplayMode::complex:								render_impl<DisplayMode::complex>();							break;
					case DisplayMode::abs:									render_impl<DisplayMode::abs>();								break;
					case DisplayMode::harmonic_product:						render_impl<DisplayMode::harmonic_product>();					break;
					case DisplayMode::harmonic_product_normalized:			render_impl<DisplayMode::harmonic_product_normalized>();		break;
					case DisplayMode::harmonic_product_skip_f0:				render_impl<DisplayMode::harmonic_product_skip_f0>();			break;
					case DisplayMode::harmonic_product_skip_f0_normalized:	render_impl<DisplayMode::harmonic_product_skip_f0_normalized>();break;
					case DisplayMode::harmonic_product_odd:					render_impl<DisplayMode::harmonic_product_odd>();				break;
					case DisplayMode::harmonic_product_odd_normalized:		render_impl<DisplayMode::harmonic_product_odd_normalized>();	break;
					default:UNREACHABLE;
				}
				if (nMaximums)
				{
					if (is_harmonic(displayMode))
					{
						u32 iFront{};
						f32 prevH{ points.front()[1] };
						bool rise{};
						for (const auto iBin : iota_up(u32{ 1 }, pSource->binCountDisplay()))
						{
							const f32 h{ points[iBin][1] };
							if (h > prevH)
							{
								iFront = iBin;
								rise = true;
							}
							else if (h < prevH && rise)
							{
								const f32 hFront{ points[iFront][1] };
								const f32 iBinMid{ (iFront + iBin - 1) * 0.5f };
								if (maximums.size() < nMaximums)
									maximums.emplace(hFront, iBinMid);
								else if (maximums.root().h < hFront)
									maximums.replaceRoot(hFront, iBinMid);
								rise = false;
							}
							prevH = h;
						}
					}
					else//abs or complex
					{
						u32 iFront{};
						f32 prevH{ pSource->absFrequencyBins[0] };
						bool rise{};
						for (const auto iBin : iota_up(u32{ 1 }, pSource->binCountDisplay()))
						{
							const f32 h{ pSource->absFrequencyBins[iBin] };
							if (h > prevH)
							{
								iFront = iBin;
								rise = true;
							}
							else if (h < prevH && rise)
							{
								const f32 hFront{ pSource->absFrequencyBins[iFront] };
								const f32 iBinMid{ (iFront + iBin - 1) * 0.5f };
								if (maximums.size() < nMaximums)
									maximums.emplace(hFront, iBinMid);
								else if (maximums.root().h < hFront)
									maximums.replaceRoot(hFront, iBinMid);
								rise = false;
							}
							prevH = h;
						}
					}
				}
				const f32 finalScale{ [&] LAMBDA_INLINE
				{
					const f32 result
					{
						pAutoScaleTarget ?
						(Math::signbit(scale) ? -1.f : 1.f) * Math::abs(pAutoScaleTarget->scale) * cfg.globalScale * pAutoScaleTarget->maxHeight / maxHeight :
						cfg.globalScale * scale
					};
					return Math::isFinite(result) ? result : 0.f;
				}() };
				for (Math::float2& point : points)
					point[1] = base - point[1] * finalScale;
				DTOR(geometry);
				CTOR(geometry) { points, D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN };
			}
			overlay.graphics().drawSimplePathGeometry(geometry, strokeWidth, color);
		}
	};
	GLOBAL DynamicArray<Source> sources;
	GLOBAL DynamicArray<Window> windows;
	GLOBAL Windows::Beeper beeper;
	GLOBAL KeyStates
	<
		[](KeyId key)static{return Input::key(key).isPressed(); }, 
		DynamicKeybind<"exit"			,[]static -> auto&{return cfg.keybinds.exit; }>,
		DynamicKeybind<"boostDbInc"		,[]static -> auto&{return cfg.keybinds.boostDbInc; }>,
		DynamicKeybind<"boostDbDec"		,[]static -> auto&{return cfg.keybinds.boostDbDec; }>,
		DynamicKeybind<"showCursorNote"	,[]static -> auto&{return cfg.keybinds.showCursorNote; }>
	> keystates;
	inline bool loadCfgImpl()noexcept
	{
		const Opt<DynamicArray<u8>> chars{ readFile(cfgMonitor.path()) };
		if (!chars) [[unlikely]]
			return false;
		if (!chars->size()) [[unlikely]]
			return false;
		const u8* pBegin{ chars->data() + ((chars->size() >= 3 && byte_pattern_simd_preload<"EF BB BF">.match(chars->data())) ? u8{3} : u8{0}) };
		const u8* pEnd{ chars->end() };
		auto oldCfg{ cfg };
		const auto rollback{ [&]
		{
			DTOR(cfg);
			CTOR(cfg) { MOV(oldCfg) };
			sources.clear();
			windows.clear();
		} };
		if (!Sjn::parse<parseOpts>(cfg, Span{ pBegin, pEnd }) || !cfg.valid()) [[unlikely]]
			return (rollback(), false);
		const u32 w{ overlay.size()[0] };
		const u32 h{ overlay.size()[1] };
		if (const f32 fontSize{ cfg.fps.font.size * h }; textFormatFps.size() != fontSize || textFormatFps.name() != cfg.fps.font.familyName)
		{
			DTOR(textFormatFps);
			CTOR(textFormatFps) { fontSize, cfg.fps.font.familyName };
		}
		const bool sourcesResized{ sources.size() != cfg.sources.size() };
		const bool windowsResized{ windows.size() != cfg.windows.size() };
		if (sourcesResized)sources.resize(cfg.sources.size());
		if (windowsResized)windows.resize(cfg.windows.size());
		for (const auto i : iota_up(sources.size()))
		{
			Source& source{ sources[i] };
			const Config::Source* pOldSourceCfg{ i >= oldCfg.sources.size() ? nullptr : &oldCfg.sources[i] };
			const Config::Source& newSourceCfg{ cfg.sources[i] };
			if (!source.capture.initialized() ||
				!pOldSourceCfg ||
				pOldSourceCfg->type != newSourceCfg.type)
			{
				DTOR(source.capture);
				CTOR(source.capture);
				if (!((newSourceCfg.type == AudioCaptureType::input ? source.capture.init<AudioCaptureType::input>() : source.capture.init<AudioCaptureType::output>()) &&
					source.capture.start() &&
					source.capture.isF32()))
					return (rollback(), false);
				// LOG(source.capture);
			}
			const u8 nHarmonicMax{ [&] LAMBDA_INLINE
			{
				u8 result{ 1 };
				for (const Config::Window& window : cfg.windows)
					if (window.enable && window.sourceId == i && is_harmonic(window.displayMode))
						result = Algorithm::max(result, (u8)max_harmonic_multiple(window.displayMode, window.nHarmonic));
				return result;
			}() };
			const bool reallocate
			{
				!cfgState.initialized ||
				!cfgState.valid ||
				!pOldSourceCfg ||
				pOldSourceCfg->type != newSourceCfg.type ||
				pOldSourceCfg->frequency.min != newSourceCfg.frequency.min ||
				pOldSourceCfg->frequency.max != newSourceCfg.frequency.max ||
				pOldSourceCfg->nBinsPerOctave != newSourceCfg.nBinsPerOctave ||
				pOldSourceCfg->nCyclePerWindow != newSourceCfg.nCyclePerWindow ||
				source.nHarmonicMax != nHarmonicMax
			};
			if (reallocate)
			{
				source.nCyclePerWindow = newSourceCfg.nCyclePerWindow;
				source.nHarmonicMax = nHarmonicMax;
				source.fMin = newSourceCfg.frequency.min.hz;
				source.nOctavesPerBin = 1.f / newSourceCfg.nBinsPerOctave;
				source.binRatio = Math::exp2(source.nOctavesPerBin);

				const f32 displayLimit{ Algorithm::min(newSourceCfg.frequency.max.hz, source.nyquist()) };
				if (displayLimit < source.fMin) [[unlikely]] return (rollback(), false);
				const u32 lastDisplayBin{ (u32)Math::ceil(source.f2bin(displayLimit)) };
				source.fMaxDisplay = source.bin2f(lastDisplayBin);
				source.nOctavesDisplay = Math::log2(source.fMaxDisplay / source.fMin);
				const u32 displayBinCount{ lastDisplayBin + 1 };
				const f32 maxInternalF{ source.fMaxDisplay * source.nHarmonicMax };
				if (!Math::isFinite(maxInternalF) || maxInternalF > source.nyquist()) [[unlikely]] return (rollback(), false);
				const u32 lastInternalBin{ source.nHarmonicMax <= 1 ? lastDisplayBin : (u32)Math::ceil(source.f2bin(maxInternalF)) };
				if (const u32 lastNyquistBin{ (u32)Math::floor(source.f2bin(source.nyquist())) }; lastInternalBin > lastNyquistBin) [[unlikely]] return (rollback(), false);

				source.frequencyBins.resize_uninitialized(lastInternalBin + 1);
				source.absFrequencyBins.resize_uninitialized(lastInternalBin + 1);
				source.harmonicIndexes.resize_uninitialized(displayBinCount, u8(source.nHarmonicMax - 1));
				source.dotProductArrays.factors.resize_uninitialized(source.binCountInternal());
				u32 dotProductStorageSize{};
				for (const auto iBin : iota_up(source.binCountInternal()))
				{
					const u32 nSamples{ source.f2NSamples(source.bin2f(iBin)) };
					dotProductStorageSize += next_aligned_greater_equal<simd_width32>(nSamples) * 2;
				}
				source.dotProductArrays.storage.resize_uninitialized(dotProductStorageSize);
				source.signal.resize(next_aligned_greater_equal<simd_width32>(source.f2NSamples(source.fMin)));

				for (const auto iBin : iota_up(source.binCountDisplay()))
				{
					const f32 f{ source.bin2f(iBin) };
					for (const auto iHarmonic : iota_up(u8{ 2 }, u8(source.nHarmonicMax + 1)))
					{
						const f32 fH{ f * iHarmonic };
						const f32 bin{ source.f2bin(fH) };
						const u32 iPrev{ (u32)Math::floor(bin) };
						const u32 iNext{ (u32)Math::ceil(bin) };
						const f32 ratio{ (iPrev == iNext) ? 0.f : (bin - iPrev) };//Math::invLerpClamped(bin, iPrev, iNext)
						source.harmonicIndexes[iBin, u8(iHarmonic - 2)] = HarmonicIndex
						{
							.prev{iPrev},
							.next{iNext},
							.ratio{ratio}
						};
					}
				}
			}
			source.setBoostDb(newSourceCfg.boostDb);
			if (reallocate || pOldSourceCfg->windowFunction != newSourceCfg.windowFunction)
			{
				u32 storageOffset{};
				for (const auto iBin : iota_up(source.binCountInternal()))
				{
					const f32 f{ source.bin2f(iBin) };
					const u32 nSamples{ source.f2NSamples(f) };
					const u32 alignedNSamples{ next_aligned_greater_equal<simd_width32>(nSamples) };
					f32* const pS{ source.dotProductArrays.storage.data() + storageOffset };
					storageOffset += alignedNSamples;
					f32* const pC{ source.dotProductArrays.storage.data() + storageOffset };
					storageOffset += alignedNSamples;
					source.dotProductArrays[iBin] = DotProductArrays::Bin{ .s{pS}, .c{pC}, .size{alignedNSamples} };
					for (f32& s : Span{ pS, alignedNSamples - nSamples })s = {};//pad front
					for (f32& c : Span{ pC, alignedNSamples - nSamples })c = {};//pad front
					f64 factorSum{};
					for (const auto iSample : iota_up(nSamples))
					{
						const f32 sec{ static_cast<f32>(iSample) / source.samplesPerSec() };
						const Math::Rad<f32> theta{ Math::Rad<f32>::circle<> *f * sec };
						const SinCos<f32> sc{ Math::sincos(theta) };
#if 1
						const f32 fraction{ (iSample + 1) / static_cast<f32>(nSamples + 1) };// [1 / (nSamples + 1), nSamples / (nSamples + 1)]
#else
						const f32 fraction{ iSample / static_cast<f32>(nSamples - 1) };
#endif
						f32 factor;
						switch (newSourceCfg.windowFunction)
						{
							case WindowFunction::rect:factor = 1.f;break;
							case WindowFunction::triangle:factor = 1.f - Math::abs(2.f * fraction - 1.f);break;
							case WindowFunction::hann:factor = Math::sqr(Math::sin(Math::Rad<f32>::circle<0.5f> *fraction));break;
							case WindowFunction::kaiser:factor = Math::kaiser(fraction);break;
							case WindowFunction::blackman_harris:factor = Math::blackmanHarris(fraction);break;
							default:UNREACHABLE;
						}
						factorSum += factor;
						pS[alignedNSamples - nSamples + iSample] = -sc.s / nSamples * factor;
						pC[alignedNSamples - nSamples + iSample] = +sc.c / nSamples * factor;
					}
					const f32 normalization{ static_cast<f32>(nSamples / factorSum) };
					for (const auto iSample : iota_up(nSamples))
					{
						pS[alignedNSamples - nSamples + iSample] *= normalization;
						pC[alignedNSamples - nSamples + iSample] *= normalization;
					}
				}
			}
		}
		for (const auto i : iota_up(windows.size()))
		{
			Window& window{ windows[i] };
			const Config::Window& newWindowCfg{ cfg.windows[i] };
			window.enable = newWindowCfg.enable;
			window.pSource = &sources[newWindowCfg.sourceId];
			window.pAutoScaleTarget = newWindowCfg.autoScaleWindowId ? &windows[*newWindowCfg.autoScaleWindowId] : nullptr;
			window.left = static_cast<f32>(w) * newWindowCfg.left;
			window.right = static_cast<f32>(w) * newWindowCfg.right;
			window.base = static_cast<f32>(h) * newWindowCfg.base;
			window.strokeWidth = static_cast<f32>(h) * newWindowCfg.strokeWidth;
			window.color = Rgba<f32>{ newWindowCfg.color };
			window.displayMode = newWindowCfg.displayMode;
			window.nHarmonic = newWindowCfg.nHarmonic;
			window.nMaximums = newWindowCfg.maximum.n;
			window.scale = newWindowCfg.scale;
			const u32 nPoints{ window.displayMode == DisplayMode::complex ? window.pSource->binCountDisplay() * 2 : window.pSource->binCountDisplay() };
			if (window.points.size() != nPoints)window.points.resize_uninitialized(nPoints);
			if (!window.textFormat || window.textFormat.name() != newWindowCfg.maximum.font.familyName || window.textFormat.size() != newWindowCfg.maximum.font.size * h)
			{
				DTOR(window.textFormat);
				CTOR(window.textFormat) { newWindowCfg.maximum.font.size* h, newWindowCfg.maximum.font.familyName };
			}
		}
		keystates.reset();
		return true;
	}
	inline bool saveConfig()noexcept
	{
		StringBuilderImpl<c8, GrowableArray<c8>> sb;
		Sjn::print<printOpts>(sb, cfg);
		const bool succeed{ writeFile(cfgMonitor.path(), sb.str()) };
		if (succeed) [[likely]]
			cfgMonitor.update();
		else [[unlikely]] if (cfg.beep)
			beeper(500, 500);
		return succeed;
	}
	inline void loadCfg(bool& cfgChanged)noexcept
	{
		switch (cfgMonitor.update())
		{
			case CfgMonitor::Status::no_access: [[unlikely]]
			{
				if (!cfgState.valid)
					break;
				saveConfig();
				[[fallthrough]];
			}
			case CfgMonitor::Status::modified: [[unlikely]]
			{
				cfgChanged = true;
				const bool success{ loadCfgImpl() };
				cfgState = { .valid{success}, .initialized{success} };
				if (cfg.beep)beeper(success ? 1000_u32 : 500_u32, 500);
				break;
			}
			case CfgMonitor::Status::not_modified:break;
			default:UNREACHABLE;
		}
	}
	inline void reset()noexcept
	{
		sources.clear();
		windows.clear();
		keystates.reset();
	}
	INLINE void loop(Windows::$Path auto&& cfgPath)noexcept
	{
		if (!cfgMonitor.init(FWD(cfgPath)))[[unlikely]]return;
		for (;;)
		{
			updateRate.update();
			overlay.peek();
			overlay.beginFrame();
			bool cfgChanged{}; 
			loadCfg(cfgChanged);
			if (!cfgState.initialized)
				reset();
			else
			{
				keystates.update();
				if (keystates.justPressed<"exit">()) [[unlikely]] Windows::exit();
				bool cfgChangedByHotkey{};
				if (const bool boostDbInc{ keystates.justPressed<"boostDbInc">() }, boostDbDec{ keystates.justPressed<"boostDbDec">() }; boostDbInc != boostDbDec) [[unlikely]]
				{
					cfgChangedByHotkey = true;
					for (const auto i : iota_up(cfg.sources.size()))
						sources[i].setBoostDb(cfg.sources[i].boostDb += (boostDbInc ? 1.f : -1.f));
				}
				if (cfgChangedByHotkey)
				{
					saveConfig();
					cfgChanged = true;
				}
				for (;;)
				{
					for (Source& source : sources)
						if (!source.update(cfgChanged)) [[unlikely]]
						{
							cfgState.initialized = false;
							goto end_frame;
						}
					if (keystates.pressed<"showCursorNote">() || 
						Duration{ TimePoint::now() - updateRate.now() } > cfg.wait ||
						[&]LAMBDA_INLINE{ for (Source& source : sources)if (source.redraw)return true; return false; }())
						break;
				}
				for (Window& window : windows)
					if(window.enable && !window.pAutoScaleTarget)
						window.render();
				for (Window& window : windows)
					if (window.enable && window.pAutoScaleTarget)
						window.render();
				for (Window& window : windows)
					if (window.enable)
						window.render_maximums();
				if (keystates.pressed<"showCursorNote">())
				{
					const Math::float2 mousePos{ (f32)overlay.msg().pt.x,(f32)overlay.msg().pt.y };
					Window* pWindow{};
					f32 dist;
					for (Window& window : windows)
						if (window.enable && window.left <= mousePos[0] && window.right >= mousePos[0] && (!pWindow || Math::abs(window.base - mousePos[1]) < dist))
						{
							dist = Math::abs(window.base - mousePos[1]);
							pWindow = &window;
						}
					if (pWindow)
					{
						const f32 f{ pWindow->pSource->x2f(Math::invLerp(mousePos[0], pWindow->left, pWindow->right)) };
						const Direct2D::TextLayout layout{ f2closestNote<c16>(f), pWindow->textFormat };
						overlay.graphics().drawTextLayout<{ Direct2D::Alignment::Horizontal::center, Direct2D::Alignment::Vertical::center }>(mousePos, layout, pWindow->color);
					}
				}
			}
		end_frame:
			if(cfg.fps.enable)
			{
				const u32 w{ overlay.size()[0] };
				const u32 h{ overlay.size()[1] };
				const Direct2D::TextLayout layout{ f2chars<c16>(Math::round(updateRate.smoothFps())), textFormatFps };
				overlay.graphics().drawTextLayout<{ Direct2D::Alignment::Horizontal::center, Direct2D::Alignment::Vertical::center }>({ w * cfg.fps.position[0], h * cfg.fps.position[1] }, layout, cfgState.initialized ? Rgba<f32>{1, 1, 1, 1} : Rgba<f32>{1,0,0,1});
			}
			overlay.endFrame();
		}
	}
}