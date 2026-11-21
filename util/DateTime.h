#pragma once
#include "Abs.h"
#include "Assert.h"
#include "BasicTypes.h"
#include "ConceptSame.h"
#include "DecToStream.h"
#include "KuserSharedData.h"
#include "StaticError.h"
#include "UnixTime.h"
#include "Unreachable.h"
#include "WinStructs.h"
enum class TimeZone :u8
{
	local,
	utc
};

INLINE constexpr u8 monthLength(bool isLeapYear, u8 month)noexcept
{
	ASSERT(month <= 11);
	switch (month)
	{
		case 0:return 31;
		case 1:return isLeapYear ? 29 : 28;
		case 2:return 31;
		case 3:return 30;
		case 4:return 31;
		case 5:return 30;
		case 6:return 31;
		case 7:return 31;
		case 8:return 30;
		case 9:return 31;
		case 10:return 30;
		case 11:return 31;
		default:UNREACHABLE;
	}
}

struct FileTimeOffset
{
private:
	i64 m_value;
public:
	constexpr explicit FileTimeOffset(i64 value)noexcept :m_value{ value } {}
	constexpr i64 value()const noexcept { return m_value; }
	static FileTimeOffset timeZoneBias()noexcept
	{
		return FileTimeOffset{ static_cast<i64>(Windows::kuserSharedData().TimeZoneBias.value) };
	}
};

struct FileTimeOffsetInfo
{
	u64 hour;
	u8 minute;
	u8 second;
	u16 millisecond;
	bool sign;

	constexpr FileTimeOffsetInfo(FileTimeOffset offset)noexcept
	{
		sign = offset.value() >= 0;
		const u64 totalMs{ Math::uabs(offset.value()) / u64{TICKSPERMSEC} };
		millisecond = totalMs % 1000;
		second = totalMs / 1000 % 60;
		minute = totalMs / 60000 % 60;
		hour = totalMs / 3600000;
	}
	template<class Stream>
	friend constexpr Stream& operator<<(Stream& os, const FileTimeOffsetInfo& offsetInfo)noexcept
	{
		if (!offsetInfo.sign)os << '-';
		os << offsetInfo.hour << ':';
		os << Dec<u8, 2> {offsetInfo.minute} << ':';
		os << Dec<u8, 2> {offsetInfo.second} << ':';
		os << Dec<u16, 3>{offsetInfo.millisecond};
		return os;
	}
};

template<class Stream>
constexpr Stream& operator<<(Stream& os, FileTimeOffset offset)noexcept
{
	os << FileTimeOffsetInfo{ offset };
	return os;
}

struct FileTime
{
private:
	i64 m_value;
public:
	constexpr FileTime()noexcept :m_value{ 0 } {}
	constexpr explicit FileTime(i64 v)noexcept :m_value{ v } {}
	template<class T>
	constexpr explicit FileTime(UnixTime<T> unixTime)noexcept :m_value{ i64{unixTime.value()}*i64{10000000} + 116444736000000000 } {}

	template<class T>
	constexpr explicit FileTime(UnixTimeInfo<T> dateTime32)noexcept :m_value{ [&] LAMBDA_INLINE
	{
		const T y{dateTime32.year - 1};//Don't include a leap day from the current year
		return y * DAYSPERNORMALYEAR + y / 4 - y / 100 + y / 400 - compact<(EPOCHYEAR - 1) * DAYSPERNORMALYEAR + (EPOCHYEAR - 1) / 4 - (EPOCHYEAR - 1) / 100 + (EPOCHYEAR - 1) / 400>;
	}() }
	{
		const bool isLeapYear{ dateTime32.year % 4 == 0 && (dateTime32.year % 100 != 0 || dateTime32.year % 400 == 0) };
		for (u8 CurMonth{}; CurMonth != dateTime32.month - 1; ++CurMonth)
			m_value += monthLength(isLeapYear, CurMonth);
		m_value += dateTime32.day - 1;
		m_value *= SECSPERDAY;
		m_value += i64{ dateTime32.hour } *SECSPERHOUR + i64{ dateTime32.minute } *SECSPERMIN + dateTime32.second;
		m_value *= TICKSPERSEC;
	}

	template<TimeZone timezone = TimeZone::local>
	static FileTime now()noexcept
	{
		const i64 utc{ static_cast<i64>(Windows::kuserSharedData().SystemTime.value) };
		if constexpr (timezone == TimeZone::utc)
			return FileTime{ utc };
		else if constexpr (timezone == TimeZone::local)
			return FileTime{ utc - FileTimeOffset::timeZoneBias().value() };
	}

	constexpr i64 value()const noexcept { return m_value; }

	template<class T>
	constexpr explicit operator UnixTime<T>()const noexcept { return UnixTime<T>{static_cast<T>((m_value - 116444736000000000) / 10000000)}; }

	friend constexpr bool operator>(FileTime l, FileTime r)noexcept { return l.value() > r.value(); }
	friend constexpr bool operator<(FileTime l, FileTime r)noexcept { return l.value() < r.value(); }
	friend constexpr bool operator>=(FileTime l, FileTime r)noexcept { return l.value() >= r.value(); }
	friend constexpr bool operator<=(FileTime l, FileTime r)noexcept { return l.value() <= r.value(); }
	friend constexpr bool operator==(FileTime l, FileTime r)noexcept { return l.value() == r.value(); }
	friend constexpr bool operator!=(FileTime l, FileTime r)noexcept { return l.value() != r.value(); }
	friend constexpr FileTime operator+(FileTime time, FileTimeOffset offset)noexcept { return FileTime{ time.value() + offset.value() }; }
	friend constexpr FileTime operator+(FileTimeOffset offset, FileTime time)noexcept { return time + offset; }
	friend constexpr FileTime operator-(FileTime time, FileTimeOffset offset)noexcept { return FileTime{ time.value() - offset.value() }; }
};


struct FileTimeInfo
{
	u32 year{};
	u8 month{};
	u8 day{};
	u8 hour{};
	u8 minute{};
	u8 second{};
	u16 ms{};
	u16 us{};
	u16 ns{};

	constexpr FileTimeInfo(FileTime fileTime)noexcept
	{
		ASSERT(fileTime.value() >= 0);

		const u64 value{ static_cast<u64>(fileTime.value()) };
		const u64 subsecondTicks{ value % TICKSPERSEC };
		ms = static_cast<u16>(subsecondTicks / TICKSPERMSEC);
		us = static_cast<u16>(subsecondTicks / 10 % 1000);
		ns = static_cast<u16>(subsecondTicks % 10 * 100);
		const u64 Time{ value / TICKSPERSEC };
		u32 Days{ static_cast<u32>(Time / SECSPERDAY) };
		u64 SecondsInDay{ Time % SECSPERDAY };
		hour = static_cast<u8>(SecondsInDay / SECSPERHOUR);
		SecondsInDay %= SECSPERHOUR;
		minute = static_cast<u8>(SecondsInDay / SECSPERMIN);
		second = static_cast<u8>(SecondsInDay % SECSPERMIN);
		const u32 cleaps{ (3 * ((4 * Days + 1227) / DAYSPERQUADRICENTENNIUM) + 3) / 4 };
		Days += 28188 + cleaps;
		const u32 years{ (20 * Days - 2442) / (5 * DAYSPERNORMALQUADRENNIUM) };
		const u32 yearday{ Days - years * DAYSPERNORMALQUADRENNIUM / 4 };
		const u8 months{ static_cast<u8>(64 * yearday / 1959) };

		if (months < 14)
		{
			month = months - 1;
			year = years + 1524;
		}
		else
		{
			month = months - 13;
			year = years + 1525;
		}

		day = static_cast<u8>(yearday - 1959 * months / 64);
	}

	constexpr explicit operator FileTime()const noexcept
	{
		u64 result{ [&] LAMBDA_INLINE
		{
			const u32 y{ year - 1 };
			return u64{ y } * DAYSPERNORMALYEAR + y / 4 - y / 100 + y / 400 - compact<(EPOCHYEAR - 1) * DAYSPERNORMALYEAR + (EPOCHYEAR - 1) / 4 - (EPOCHYEAR - 1) / 100 + (EPOCHYEAR - 1) / 400>;
		}() };

		const bool isLeapYear{ year % 4 == 0 && (year % 100 != 0 || year % 400 == 0) };

		for (u8 CurMonth{}; CurMonth != month - 1; ++CurMonth)
			result += monthLength(isLeapYear, CurMonth);

		result += day - 1;
		result *= SECSPERDAY;
		result += u64{ hour } * SECSPERHOUR;
		result += u64{ minute } * SECSPERMIN;
		result += second;
		result *= TICKSPERSEC;
		result += u64{ ms } * TICKSPERMSEC;
		result += u64{ us } * 10;
		result += u64{ ns } / 100;
		return FileTime{ static_cast<i64>(result) };
	}

	template<class T>
	constexpr explicit operator UnixTime<T>()const noexcept
	{
		return UnixTime<T>{ static_cast<T>((FileTime{ *this }.value() - i64{ 116444736000000000 }) / i64{ 10000000 }) };
	}
};

template<class Stream>
constexpr Stream& operator<<(Stream& os, const FileTimeInfo& dateTime)noexcept
{
	os << Dec{ dateTime.year } << '-';
	os << Dec<u8, 2> {dateTime.month} << '-';
	os << Dec<u8, 2> {dateTime.day} << ' ';
	os << Dec<u8, 2> {dateTime.hour} << ':';
	os << Dec<u8, 2> {dateTime.minute} << ':';
	os << Dec<u8, 2> {dateTime.second} << ':';
	os << Dec<u16, 3>{dateTime.ms} << ':';
	os << Dec<u16, 3>{dateTime.us} << ':';
	os << Dec<u16, 3>{dateTime.ns};
	return os;
}

template<class Stream>
constexpr Stream& operator<<(Stream& os, const FileTime& fileTime)noexcept { return os << FileTimeInfo{ fileTime }; }