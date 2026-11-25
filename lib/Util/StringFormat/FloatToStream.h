#pragma once
#include "../Algorithm/Copy.h"
#include "../Algorithm/Fill.h"
#include "../DataStructures/Span.h"
#include "Num2Chars.h"
template<class, u8...>struct Float;

template<$Floating F>
struct Float<F>
{
private:
	F m_value;
public:
	INLINE constexpr Float(F value)noexcept :m_value{ value } {}
	INLINE constexpr F value()const noexcept { return m_value; }
};

template<$Floating F>
struct Float<F, 0, 0, 0>
{
private:
	F m_value;
	u8 m_precision;
public:
	INLINE constexpr Float(F value, u8 precision)noexcept :m_value{ value }, m_precision{ precision } {}
	INLINE constexpr F value()const noexcept { return m_value; }
	INLINE constexpr u8 precision()const noexcept { return m_precision; }
};

template<$Floating F>
struct Float<F, 0, 0, 1>
{
private:
	F m_value;
	u8 m_precision;
	u8 m_length;
public:
	INLINE constexpr Float(F value, u8 precision, u8 length)noexcept :m_value{ value }, m_precision{ precision }, m_length{ length } {}
	INLINE constexpr F value()const noexcept { return m_value; }
	INLINE constexpr u8 precision()const noexcept { return m_precision; }
	INLINE constexpr u8 length()const noexcept { return m_length; }
};

template<$Floating F, u8 P>
struct Float<F, P>
{
private:
	F m_value;
public:
	INLINE constexpr Float(F value)noexcept :m_value{ value } {}
	INLINE constexpr F value()const noexcept { return m_value; }
	INLINE static CONSTEVAL u8 precision()noexcept { return P; }
};

template<$Floating F, u8 P, u8 L>
struct Float<F, P, L>
{
private:
	F m_value;
public:
	INLINE constexpr Float(F value)noexcept :m_value{ value } {}
	INLINE constexpr F value()const noexcept { return m_value; }
	INLINE static CONSTEVAL u8 precision()noexcept { return P; }
	INLINE static CONSTEVAL u8 length()noexcept { return L; }
};

template<$Floating F>Float(F) -> Float<F>;
template<$Floating F>Float(F, u8) -> Float<F, 0, 0, 0>;
template<$Floating F>Float(F, u8, u8) -> Float<F, 0, 0, 1>;

template<u8...Args, $Floating F>
INLINE constexpr Float<F, Args...> flt(F x)noexcept { return { x }; }

template<class Stream, $Floating F>
constexpr Stream& operator<<(Stream& os, Float<F> f)noexcept
{
#if 1
	typename Stream::char_type buf[compact<f2chars_required_buffer_size<F>>];
	os.write(buf, f2chars(f.value(), buf));
#else
	const StaticString s{ f2chars<typename Stream::char_type>(f.value()) };
	os.write(s.data(), s.size());
#endif
	return os;
}
template<class Stream, class F, u8...Args>
constexpr Stream& operator<<(Stream& os, const Float<F, Args...>& f)noexcept
requires(requires{f.precision(); } && !requires{f.length(); })
{
	static constexpr auto buffer_size{ compact<f2chars_required_buffer_size<F> +f.precision()> };//todo: dynamic f.precision()
	typename Stream::char_type buf[buffer_size];
	os.write(buf, f2chars_precision(f.value(), f.precision(), buf));
	return os;
}

template<class Stream, class F, u8...Args>
constexpr Stream& operator<<(Stream& os, const Float<F, Args...>& f)noexcept
requires(requires{f.precision(); } && requires{f.length(); })
{
	static constexpr auto buffer_size{ compact<f2chars_required_buffer_size<F> +f.precision()> };//todo: dynamic f.precision()
	using Size = std::remove_cvref_t<decltype(buffer_size)>;
	typename Stream::char_type buf[buffer_size];
	const Size fullSize{ static_cast<Size>(f2chars_precision(f.value(), f.precision(), buf) - buf) };
	if (fullSize >= f.length())
		os.write(buf, f.length());
	else
	{
		typename Stream::char_type buf2[f.length()];//todo: dynamic f.length()
		Algorithm::fill(Span{ &buf2[0], buf2 + f.length() - u8(fullSize) }, ' ');
		Algorithm::copy_assign(Span{ buf, fullSize }, buf2 + f.length() - u8(fullSize));
		os.write(buf2, f.length());
	}
	return os;
}