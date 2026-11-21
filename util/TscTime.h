#pragma once
#include "Abs.h"
#include "AlgorithmSort.h"
#include "BasicTypes.h"
#include "CeilFloor.h"
#include "Clamp.h"
#include "Compare.h"
#include "Compiler.h"
#include "ConceptFloating.h"
#include "Constant.h"
#include "Consteval.h"
#include "Inline.h"
#include "IsFinite.h"
#include "KuserSharedData.h"
#include "MacroIf.h"
#include "NTDLL.h"
#include "OptionalImpl.h"
#include "Run.h"
#include "TagUninitialized.h"
#include <ratio>
#if !defined(TSC_USE_RDTSC)
	#define TSC_USE_RDTSC 1
#endif
namespace details::TscImpl
{
	template<class...>struct Frequency;
	template<class...>struct TimePoint;
	template<class...>struct TimePointDiff;
	template<class...>struct Duration;
	template<class...>struct StopWatch;

	//because inheritance pollutes pdb
#define TSC_UNIT_SEC_CONVERSION private:static constexpr T unitPerSec{ T(ratio::den) / T(ratio::num) };static constexpr T secPerUnit{ T(ratio::num) / T(ratio::den) };public:

	template<$Floating T, class ratio>
	requires(ratio::num > 0 && ratio::den > 0 && Math::isFinite(T(ratio::den) / T(ratio::num)))
	struct Frequency<T, ratio>
	{
		TSC_UNIT_SEC_CONVERSION
		T unitPerTsc;
		INLINE Frequency()noexcept { calibrate(); }
	private:
		struct Clocks
		{
			u64 tsc;
			u64 time;
			INLINE static Clocks now(u64 tscIntervalThreshold)noexcept
			{
				for (;;)
				{
					u64 tsc0, tsc1;
					u32 cpu0, cpu1;
					u64 prevTime;
					do
					{
						tsc0 = __rdtscp(&cpu0);
						prevTime = Windows::kuserSharedData().InterruptTime.value;//*(volatile u64*)0x7FFE0008
						tsc1 = __rdtscp(&cpu1);
					} while (cpu0 != cpu1 || tsc1 - tsc0 > tscIntervalThreshold);
					for (;;)
					{
						u32 cpuBefore, cpuAfter;
						const u64 tscBefore{ __rdtscp(&cpuBefore) };
						const u64 now{ Windows::kuserSharedData().InterruptTime.value };//*(volatile u64*)0x7FFE0008
						const u64 tscAfter{ __rdtscp(&cpuAfter) };
						if (cpu0 != cpuBefore || cpu0 != cpuAfter || tscAfter - tscBefore > tscIntervalThreshold || tscBefore - tsc1 > tscIntervalThreshold)
							break;
						else if (now != prevTime)
							return { .tsc{(tscBefore + tscAfter) / 2},.time{now} };
						else
						{
							cpu0 = cpuAfter;
							tsc1 = tscAfter;
						}
					}
				}
			}
		};
	public:
		template<usize N = 64, f64 percentile = 0.9>
		requires(N >= 2 && percentile > 0 && percentile <= 1)
		INLINE static T calibrateImpl()noexcept
		{
			return [] <usize...I>(std::index_sequence<I...>)STATIC_LAMBDA_INLINE
			{
				const u64 tscIntervalThreshold{ [] STATIC_LAMBDA_INLINE
				{
					std::array<u64, 0x1000> tscIntervals;
					for (u64* p{ tscIntervals.data() }; p != tscIntervals.data() + tscIntervals.size();)
					{
						u32 cpu0, cpu1, cpu2, cpu3;
						const u64 before{ __rdtscp(&cpu0) };
						__rdtscp(&cpu1);
						__rdtscp(&cpu2);
						const u64 after{ __rdtscp(&cpu3) };
						if (cpu0 == cpu1 && cpu0 == cpu2 && cpu0 == cpu3)
						{
							*p = after - before;
							++p;
						}
					}
					static constexpr f64 index{ Math::clamp((tscIntervals.size() - 1) * percentile, f64{}, f64{ tscIntervals.size() - 1 }) };
					static constexpr u64 indexFloor{ (u64)Math::floor(index) };
					static constexpr u64 indexCeil{ (u64)Math::ceil(index) };
#if 0//17kb exe size increase
					u64* const first{ tscIntervals.data() };
					u64* const last{ first + tscIntervals.size() };
					std::nth_element(first, first + indexFloor, last);
					const u64 lo{ tscIntervals[indexFloor] };
					if constexpr (indexFloor == indexCeil)
						return lo;
					else
					{
						const u64 hi{ *std::min_element(first + indexCeil, last) };
						return (u64)Math::ceil(lo * constant<indexCeil - index> +hi * constant<index - indexFloor>);
					}
#else
					Algorithm::quicksort_recursive(tscIntervals, Less{});
					if constexpr (indexFloor == indexCeil)
						return tscIntervals[indexFloor];
					else if constexpr (indexFloor + 1 == indexCeil)
						return (u64)Math::ceil(tscIntervals[indexFloor] * constant<indexCeil - index> +tscIntervals[indexCeil] * constant<index - indexFloor>);
#endif
				}() };
				const std::array<Clocks, N> samples{ [tscIntervalThreshold] LAMBDA_INLINE
				{
					std::array<Clocks, N> samples;
					samples[0] = Clocks::now(tscIntervalThreshold);
					for (Clocks* p{ &samples[1] },* pEnd{ samples.data() + N }; p != pEnd;)
						if (const Clocks sample{ Clocks::now(tscIntervalThreshold) }; sample.time > p[-1].time && sample.tsc > p[-1].tsc)
						{
							*p = sample;
							++p;
						}
					const u64 minTsc {samples[0].tsc };
					const u64 minTime{samples[0].time};
					(..., ((samples[I].tsc -= minTsc), (samples[I].time -= minTime)));
					return samples;
				}() };
				const f64 meanTsc { (... + samples[I].tsc ) * constant<1. / N> };
				const f64 meanTime{ (... + samples[I].time) * constant<1. / N> };
				f64 tscVariance{};
				f64 covariance{};
				for (const Clocks& sample: samples)
				{
					const f64 tscCentered{ sample.tsc - meanTsc };
					const f64 timeCentered{ sample.time - meanTime };
					tscVariance += tscCentered * tscCentered;
					covariance += tscCentered * timeCentered;
				}
				return constant<0.0000001 * unitPerSec> *covariance / tscVariance;
			}(std::make_index_sequence<N>{});
		}
		INLINE static Opt<T> askWindowsForCalibrationResult()noexcept
		{
			struct HypervisorSharedPage
			{
				u32 valid;
				u32 padding;
				u64 multiplier;
				i64 bias;
			};
			HypervisorSharedPage* pHypervisorSharedPage;
			if (!NT_SUCCESS(NTDLL::NtQuerySystemInformation(Windows::SystemInformationClass::SystemHypervisorSharedPageInformation, &pHypervisorSharedPage, sizeof(pHypervisorSharedPage), nullptr)))[[unlikely]]
				return {};
			return unitPerSec * pHypervisorSharedPage->multiplier / (18446744073709551616. * static_cast<T>(u64{ 1 } << Windows::kuserSharedData().QpcShift) * Windows::kuserSharedData().QpcFrequency);
		}
		INLINE void calibrate()noexcept
		{
			if (const Opt<T> result{ askWindowsForCalibrationResult() }; result) [[likely]]
				unitPerTsc = *result;
			else
				unitPerTsc = calibrateImpl();
		}
	};

	template<$Floating T, class ratio>
	requires(ratio::num > 0 && ratio::den > 0 && Math::isFinite(T(ratio::den) / T(ratio::num)))
	inline const Frequency<T, ratio> g_frequency{};

	template<$Floating T, class ratio>
	requires(ratio::num > 0 && ratio::den > 0 && Math::isFinite(T(ratio::den) / T(ratio::num)))
	struct TimePointDiff<T, ratio>
	{
		TSC_UNIT_SEC_CONVERSION
	private:
		i64 m_data;
	public:
		INLINE explicit TimePointDiff(Uninitialized)noexcept {}
		INLINE CONSTEVAL explicit TimePointDiff()noexcept : m_data{} {}
		INLINE constexpr explicit TimePointDiff(i64 value)noexcept : m_data{ value } {}
		INLINE constexpr i64 count()const noexcept { return m_data; }//in tsc
		INLINE friend constexpr TimePointDiff operator+(TimePointDiff l, TimePointDiff r) noexcept { return TimePointDiff{ l.m_data + r.m_data }; }
		INLINE friend constexpr TimePointDiff operator-(TimePointDiff l, TimePointDiff r) noexcept { return TimePointDiff{ l.m_data - r.m_data }; }
		INLINE friend constexpr TimePointDiff& operator+=(TimePointDiff& l, TimePointDiff r) noexcept { l.m_data += r.m_data; return l; }
		INLINE friend constexpr TimePointDiff& operator-=(TimePointDiff& l, TimePointDiff r) noexcept { l.m_data -= r.m_data; return l; }
		INLINE T toUnits() const noexcept { return static_cast<T>(m_data) * g_frequency<T,ratio>.unitPerTsc; }
		INLINE T toSeconds() const noexcept { return toUnits() * secPerUnit; }
		INLINE static TimePointDiff seconds(T seconds)noexcept { return TimePointDiff{ static_cast<i64>(seconds * unitPerSec / g_frequency<T,ratio>.unitPerTsc) }; }
		INLINE friend constexpr Duration<T, ratio> operator/(TimePointDiff l, T r)noexcept{return Duration<T, ratio>{ l } / r;}
		template<class Stream>
		INLINE friend constexpr Stream& operator<<(Stream& os, const TimePointDiff timePointDiff)noexcept { os << Duration<T, ratio>{ timePointDiff }; return os; }
	};
	
	template<$Floating T, class ratio>
	requires(ratio::num > 0 && ratio::den > 0 && Math::isFinite(T(ratio::den) / T(ratio::num)))
	struct TimePoint<T, ratio>
	{
		TSC_UNIT_SEC_CONVERSION
	private:
		u64 m_data;
	public:
		INLINE IF(MSVC, constexpr, CONSTEVAL) TimePoint()noexcept : m_data{} {}
		INLINE constexpr TimePoint(u64 tsc)noexcept : m_data{ tsc } {}
		INLINE static TimePoint now()noexcept 
		{
	#if TSC_USE_RDTSC
			return __rdtsc();
	#else
			u32 _;
			return __rdtscp(&_);
	#endif
		}
		INLINE constexpr u64 count()const noexcept { return m_data; }//in tsc
		INLINE friend constexpr TimePointDiff<T, ratio> operator-(TimePoint l, TimePoint r) noexcept { return TimePointDiff<T, ratio>{ static_cast<i64>(l.m_data) - static_cast<i64>(r.m_data) }; }
		INLINE friend constexpr TimePoint operator-(TimePoint l, TimePointDiff<T, ratio> r) noexcept { return TimePoint{ l.m_data - r.count() }; }
		INLINE friend constexpr TimePoint operator+(TimePoint l, TimePointDiff<T, ratio> r) noexcept { return TimePoint{ l.m_data + r.count() }; }
		INLINE friend constexpr TimePoint operator+(TimePointDiff<T, ratio> l, TimePoint r) noexcept { return TimePoint{ l.count() + r.m_data }; }
		INLINE friend constexpr TimePoint& operator-=(TimePoint& l, TimePointDiff<T, ratio> r) noexcept { l.m_data -= r.count(); return l; }
	};
	template<$Floating T, class ratio>
	requires(ratio::num > 0 && ratio::den > 0 && Math::isFinite(T(ratio::den) / T(ratio::num)))
	struct Duration<T, ratio>
	{
		TSC_UNIT_SEC_CONVERSION
	private:
		T m_data;//unit
		INLINE constexpr explicit Duration(T unit)noexcept : m_data{ unit } {}
	public:
		INLINE Duration(TimePointDiff<T, ratio> timePointDiff)noexcept :m_data{ timePointDiff.count() * g_frequency<T,ratio>.unitPerTsc} {}
		INLINE IF(MSVC, constexpr, CONSTEVAL) Duration()noexcept : m_data{} {}
		INLINE constexpr explicit operator bool()const noexcept { return m_data != 0; }
		INLINE constexpr explicit operator T()const noexcept { return m_data; }
		INLINE constexpr explicit operator T&()noexcept { return m_data; }
		INLINE constexpr T count()const noexcept { return m_data; }//in unit
		INLINE constexpr T toNanoseconds()  const noexcept { return m_data * constant<secPerUnit * T(1000000000.)>; }
		INLINE constexpr T toMicroseconds() const noexcept { return m_data * constant<secPerUnit * T(1000000.)>; }
		INLINE constexpr T toMilliseconds() const noexcept { return m_data * constant<secPerUnit * T(1000.)>; }
		INLINE constexpr T toSeconds()      const noexcept { return m_data * constant<secPerUnit * T(1.)>; }
		INLINE constexpr T toMinutes()      const noexcept { return m_data * constant<secPerUnit * T(0.01666666666666666666666666666)>; }
		INLINE constexpr T toHours()        const noexcept { return m_data * constant<secPerUnit * T(0.0002777777777777777777777777)>; }
		INLINE static constexpr Duration nanoseconds	(T ns) noexcept { return Duration{ ns  * constant<unitPerSec * T(0.000000001)> }; }
		INLINE static constexpr Duration microseconds	(T mcs)noexcept { return Duration{ mcs * constant<unitPerSec * T(0.000001)> }; }
		INLINE static constexpr Duration milliseconds	(T ms) noexcept { return Duration{ ms  * constant<unitPerSec * T(0.001)> }; }
		INLINE static constexpr Duration seconds		(T s)  noexcept { return Duration{ s   * constant<unitPerSec * T(1)> }; }
		INLINE static constexpr Duration minutes		(T m)  noexcept { return Duration{ m   * constant<unitPerSec * T(60)> }; }
		INLINE static constexpr Duration hours			(T h)  noexcept { return Duration{ h   * constant<unitPerSec * T(3600)> }; }
		INLINE static constexpr Duration infinity() noexcept { return Duration{ std::numeric_limits<T>::infinity() }; }
		INLINE friend constexpr Duration& operator+=(Duration& l, Duration r) noexcept { l.m_data += r.m_data; return l; }
		INLINE friend constexpr Duration& operator-=(Duration& l, Duration r) noexcept { l.m_data -= r.m_data; return l; }
		INLINE friend constexpr Duration operator+(Duration l, Duration r) noexcept { return Duration{ l.m_data + r.m_data }; }
		INLINE friend constexpr Duration operator-(Duration l, Duration r) noexcept { return Duration{ l.m_data - r.m_data }; }
		INLINE friend constexpr Duration& operator*=(Duration& l, T r) noexcept { l.m_data *= r; return l; }
		INLINE friend constexpr Duration& operator/=(Duration& l, T r) noexcept { l.m_data /= r; return l; }
		INLINE friend constexpr Duration operator*(Duration l, T r) noexcept { return Duration{ l.m_data * r }; }
		INLINE friend constexpr Duration operator/(Duration l, T r) noexcept { return Duration{ l.m_data / r }; }
		INLINE friend constexpr Duration operator*(T l, Duration r) noexcept { return Duration{ l * r.m_data }; }
		INLINE friend constexpr T operator/(Duration l, Duration r) noexcept { return l.m_data / r.m_data; }
		INLINE friend constexpr Duration abs(Duration x) noexcept { return Duration{ Math::abs(x.m_data) }; }
		template<class Stream>
		INLINE friend constexpr Stream& operator<<(Stream& os, Duration duration)noexcept
		{
			static constexpr FixedString unit_text{ [] static
			{
				/* */if constexpr (std::ratio_equal_v<ratio, std::nano>)		return FixedString{"ns"};
				else if constexpr (std::ratio_equal_v<ratio, std::micro>)		return FixedString{"us"};
				else if constexpr (std::ratio_equal_v<ratio, std::milli>)		return FixedString{"ms"};
				else if constexpr (std::ratio_equal_v<ratio, std::ratio<1>>)	return FixedString{"s"};
				else if constexpr (std::ratio_equal_v<ratio, std::ratio<60>>)	return FixedString{"min"};
				else if constexpr (std::ratio_equal_v<ratio, std::ratio<3600>>)	return FixedString{"h"};
				else STATIC_ERROR(unit not supported);
			}()};
			os << duration.count() << unit_text;
			return os;
		}
	};
	
	template<$Floating T, class ratio>
	requires(ratio::num > 0 && ratio::den > 0 && Math::isFinite(T(ratio::den) / T(ratio::num)))
	struct StopWatch<T, ratio>
	{
		TSC_UNIT_SEC_CONVERSION
	private:
		TimePoint<T, ratio> m_startTimePoint;
	public:
		INLINE StopWatch()noexcept :m_startTimePoint{ TimePoint<T, ratio>::now() } {}
		INLINE TimePointDiff<T, ratio> timeElapsed()const noexcept { return TimePoint<T, ratio>::now() - m_startTimePoint; }
		INLINE void reset() noexcept { m_startTimePoint = TimePoint<T, ratio>::now(); }
		INLINE TimePoint<T, ratio> timeStarted()const noexcept { return m_startTimePoint; }
	};
	template<class...L, class...R>INLINE constexpr bool operator==(TimePoint<L...> l, TimePoint<R...> r)noexcept { return l.count() == r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator!=(TimePoint<L...> l, TimePoint<R...> r)noexcept { return l.count() != r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator> (TimePoint<L...> l, TimePoint<R...> r)noexcept { return l.count() >  r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator< (TimePoint<L...> l, TimePoint<R...> r)noexcept { return l.count() <  r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator>=(TimePoint<L...> l, TimePoint<R...> r)noexcept { return l.count() >= r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator<=(TimePoint<L...> l, TimePoint<R...> r)noexcept { return l.count() <= r.count(); }

	template<class...L, class...R>INLINE constexpr bool operator==(TimePointDiff<L...> l, TimePointDiff<R...> r)noexcept { return l.count() == r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator!=(TimePointDiff<L...> l, TimePointDiff<R...> r)noexcept { return l.count() != r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator> (TimePointDiff<L...> l, TimePointDiff<R...> r)noexcept { return l.count() >  r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator< (TimePointDiff<L...> l, TimePointDiff<R...> r)noexcept { return l.count() <  r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator>=(TimePointDiff<L...> l, TimePointDiff<R...> r)noexcept { return l.count() >= r.count(); }
	template<class...L, class...R>INLINE constexpr bool operator<=(TimePointDiff<L...> l, TimePointDiff<R...> r)noexcept { return l.count() <= r.count(); }
	

#define TSC_DURATION_OPERATOR(op)\
	template<class L, class ratioL, class R, class ratioR>\
	INLINE constexpr bool operator op(Duration<L, ratioL> l, Duration<R, ratioR> r) noexcept\
	{\
		using T = std::common_type_t<L, R>;\
		     if constexpr (std::ratio_equal_v<ratioL, ratioR>)return l.count() op r.count();\
		else if constexpr (std::ratio_less_v<ratioL, ratioR>) return l.count() op r.count() * constant<T(f128(ratioR::num) * ratioL::den / (f128(ratioR::den) * ratioL::num))>;\
		else                                                  return l.count() * constant<T(f128(ratioL::num) * ratioR::den / (f128(ratioL::den) * ratioR::num))> op r.count();\
	}
	TSC_DURATION_OPERATOR(==)
	TSC_DURATION_OPERATOR(!=)
	TSC_DURATION_OPERATOR(>)
	TSC_DURATION_OPERATOR(<)
	TSC_DURATION_OPERATOR(>=)
	TSC_DURATION_OPERATOR(<=)
#undef TSC_DURATION_OPERATOR

	STATIC_ASSERT([]static
	{
		using Ms32 = Duration<f32, std::milli>;
		using Sec64 = Duration<f64, std::ratio<1>>;
		using Min64 = Duration<f64, std::ratio<60>>;
		using Hour64 = Duration<f64, std::ratio<3600>>;
		//operator==(Duration, Duration) is not reliable, because floating point mul in conversion
		if(!(Ms32::milliseconds(1500.0f) == Sec64::seconds(1.5)))return false;
		if(!(Hour64::hours(1.0) == Min64::minutes(60.0)))return false;
		if(!(Ms32::milliseconds(1499.f) < Sec64::seconds(1.5)))return false;
		if(!(Ms32::milliseconds(1501.f) > Sec64::seconds(1.5)))return false;
		if(!(Min64::minutes(2.0) > Sec64::seconds(119.0)))return false;
		if(!(Min64::minutes(-1.0) < Sec64::seconds(-59.0)))return false;
		if(!(Sec64::infinity() > Hour64::hours(1.0)))return false;
		return true;
	}());
}
#undef TSC_UNIT_SEC_CONVERSION
template<$Floating T = f64, class ratio = std::ratio<1>>
requires(ratio::num > 0 && ratio::den > 0 && Math::isFinite(T(ratio::den) / T(ratio::num)))
struct Tsc
{
	using Frequency = details::TscImpl::Frequency<T, ratio>;
	using TimePointDiff = details::TscImpl::TimePointDiff<T, ratio>;
	using TimePoint = details::TscImpl::TimePoint<T, ratio>;
	using Duration = details::TscImpl::Duration<T, ratio>;
	using StopWatch = details::TscImpl::StopWatch<T, ratio>;
};
using TimePointDiff = Tsc<>::TimePointDiff;
using TimePoint = Tsc<>::TimePoint;
using Duration = Tsc<>::Duration;
using StopWatch = Tsc<>::StopWatch;
CONSTEVAL Duration operator ""_s(f128 sec)noexcept { return Duration::seconds(static_cast<f64>(sec)); }
CONSTEVAL Duration operator ""_ms(f128 ms)noexcept { return Duration::milliseconds(static_cast<f64>(ms)); }
CONSTEVAL Duration operator ""_us(f128 ms)noexcept { return Duration::microseconds(static_cast<f64>(ms)); }
CONSTEVAL Duration operator ""_ns(f128 ms)noexcept { return Duration::nanoseconds(static_cast<f64>(ms)); }

//i64 RtlQueryPerformanceCounter(u64* pResult)
//{
//	u64 tsc;
//	if ((kuserSharedData.QpcBypassEnabled & 1) == 0)
//		goto SlowPath;
//	if ((kuserSharedData.QpcBypassEnabled & 2) != 0)
//	{
//		if (RtlpHypervisorSharedUserVa && RtlpHypervisorSharedUserVa->valid)
//		{
//			if (kuserSharedData.QpcBypassEnabled >= 0)
//			{
//				if ((kuserSharedData.QpcBypassEnabled & 0x20) != 0)
//					_mm_lfence();
//				else if ((kuserSharedData.QpcBypassEnabled & 0x10) != 0)
//					_mm_mfence();
//				tsc = __rdtsc();
//			}
//			else
//			{
//				u32 cpuId;
//				tsc = __rdtscp(&cpuId);
//			}
//			*pResult = (u64)(RtlpHypervisorSharedUserVa->bias + ((tsc * (u128)RtlpHypervisorSharedUserVa->multiplier) >> 64) + kuserSharedData.QpcBias) >> kuserSharedData.QpcShift;
//			return 1;
//		}
//	SlowPath:
//		u64 Counter;
//		NtQueryPerformanceCounter(&Counter, 0);
//		*pResult = Counter;
//		return 1;
//	}
//	if (kuserSharedData.QpcBypassEnabled >= 0)
//	{
//		if ((kuserSharedData.QpcBypassEnabled & 0x20) != 0)
//			_mm_lfence();
//		else if ((kuserSharedData.QpcBypassEnabled & 0x10) != 0)
//			_mm_mfence();
//		tsc = __rdtsc();
//	}
//	else
//	{
//		u32 cpuId;
//		tsc = __rdtscp(&cpuId);
//	}
//	*pResult = (tsc+kuserSharedData.QpcBias) >> kuserSharedData.QpcShift;
//	return 1;
//}