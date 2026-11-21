#pragma once
#include "BasicTypes.h"
#include "DecToStream.h"
#include "TimeConstant.h"
template<$Int T = i32>
struct UnixTimeInfo
{
	T year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	template<class Stream>
	friend constexpr Stream& operator<<(Stream& os, const UnixTimeInfo& dateTime)noexcept
	{
		os << Dec{ dateTime.year } << '-';
		os << Dec<u8, 2> {dateTime.month} << '-';
		os << Dec<u8, 2> {dateTime.day} << ' ';
		os << Dec<u8, 2> {dateTime.hour} << ':';
		os << Dec<u8, 2> {dateTime.minute} << ':';
		os << Dec<u8, 2> {dateTime.second};
		return os;
	}
};

template<class T>
struct UnixTime
{
private:
	T m_value;
public:
	constexpr explicit UnixTime(T v)noexcept :m_value{ v } {}
	constexpr T value()const noexcept { return m_value; }
	constexpr UnixTime(const UnixTimeInfo<T>& info)noexcept :m_value{ [&]LAMBDA_INLINE -> T
	{
		const i32 y   { info.year - (info.month <= 2)};
		const i32 era { (y >= 0 ? y : y - 399) / 400};
		const i32 yoe { y - era * 400};																// [0, 399]
		const i32 doy { (153 * (info.month + (info.month > 2 ? -3 : 9)) + 2) / 5 + info.day - 1};	// [0, 365]
		const i32 doe { yoe * 365 + yoe / 4 - yoe / 100 + doy};										// [0, 146096]
		return static_cast<T>((((((era * 146097 + doe - 719468) * HOURSPERDAY) + info.hour) * MINSPERHOUR) + info.minute) * SECSPERMIN + info.second);
	}() } {}

	constexpr explicit operator UnixTimeInfo<T>()const noexcept
	{
		const T z{ m_value / SECSPERDAY + 719468 };
		const T era{ (z >= 0 ? z : z - 146096) / 146097 };
		const T doe{ z - era * 146097 };
		const T yoe{ (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365 };
		const T y{ yoe + era * 400 };
		const T doy{ doe - (365 * yoe + yoe / 4 - yoe / 100) };
		const T mp{ (5 * doy + 2) / 153 };
		const T d{ doy - (153 * mp + 2) / 5 + 1 };
		const T m{ mp + (mp < 10 ? 3 : -9) };
		const T year{ y + (m <= 2) };
		const u8 month{ static_cast<u8>(m) };
		const u8 day{ static_cast<u8>(d) };
		const u8 hour{ static_cast<u8>(m_value % SECSPERDAY / SECSPERHOUR) };
		const u8 minute{ static_cast<u8>(m_value / SECSPERMIN % MINSPERHOUR) };
		const u8 second{ static_cast<u8>(m_value % SECSPERMIN) };
		return { static_cast<T>(year),month,day,hour,minute,second };
	}

	template<class Stream>
	friend constexpr Stream& operator<<(Stream& os, UnixTime unixTime)noexcept { return os << static_cast<UnixTimeInfo<T>>(unixTime); }
};
