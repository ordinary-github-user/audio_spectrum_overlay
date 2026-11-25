#pragma once
#include "DataStructures/Ring.h"
#include "Macro/NoCopyMove.h"
#include "Time/TscTime.h"
template<usize N = 256>
requires(N >= 2)
struct UpdateRate
{
	NO_COPY_MOVE(UpdateRate)
private:
	TimePointDiff m_dt{uninitialized};
	FixedRing<TimePoint, N> m_timePoints{ TimePoint::now() };
public:
	INLINE UpdateRate()noexcept = default;//m_dt is uninitialized

	INLINE void reset(TimePoint now = TimePoint::now())noexcept { m_timePoints.reset(now); }//m_dt is uninitialized

	INLINE TimePoint now()const noexcept { return m_timePoints.back(); }

	INLINE void update(TimePoint now = TimePoint::now())noexcept { m_dt = now - m_timePoints.back(); m_timePoints.emplace_back(now); }

	INLINE TimePointDiff dt()const noexcept { return m_dt; }

	template<$Floating T = f64>
	INLINE T fps()const noexcept { return T{ 1 } / dt().toSeconds(); }

	template<$Floating T = f64>
	INLINE T smoothFps()const noexcept { return T{ N - 1 } / (m_timePoints.back() - m_timePoints.front()).toSeconds(); }

	template<class Stream>
	INLINE friend Stream& operator<<(Stream& os, const UpdateRate<N>& updateRate)noexcept
	{
		os << updateRate.smoothFps();
		return os;
	}
};