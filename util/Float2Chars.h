#pragma once
#include "ConceptFloating.h"
#include "dragonbox.h"
#include "IEEE754.h"
#include "Int2Chars.h"
template<$Floating F>
constexpr auto f2chars_required_buffer_size{compact<[]static
{
	static constexpr auto dot_size{1};
	static constexpr auto neg_size{1};
	static constexpr auto exponent_size{ sizeof(F) == 4 ? 38 : 308 };//todo: need testing
	static constexpr auto significand_size{ udec2chars_required_buffer_size<UnsignedBytes<sizeof(F)>> };
	return dot_size + neg_size + exponent_size + significand_size;
}()>};
INLINE constexpr auto* f2chars($Floating auto f, $Char auto* p)noexcept
{
	if consteval { p[f2chars_required_buffer_size<decltype(f)> -1] = {}; }//test if buffer is big enough
	const IEEE754 ieee754{ f };
	if (ieee754.is_negative())*p++ = '-';
	if (!ieee754.is_finite()) [[unlikely]]
	{
		if (ieee754.mantissa) { p[0] = 'n'; p[1] = 'a'; p[2] = 'n'; }//ieee754.is_nan()
		else/*             */ { p[0] = 'i'; p[1] = 'n'; p[2] = 'f'; }
		p += 3;
	}
	else
	{
		if (ieee754.is_zero())
			*p++ = '0';
		else
		{
			auto [significand, exponent] { jkj::dragonbox::to_decimal(f, jkj::dragonbox::policy::sign::ignore) };
			using Significand = std::remove_cvref_t<decltype(significand)>;
			using Exponent = std::remove_cvref_t<decltype(exponent)>;
			STATIC_ASSERT($UInt<Significand> && sizeof(Significand) == sizeof(f) && $Same<i32, Exponent>);
			if (exponent >= 0)
			{
				//123 = {significand = 123, exponent = 0}
				//1230 = {significand = 123, exponent = 1}
				p = udec2chars(significand, p);
				while (exponent--)
					*p++ = '0';
			}
			else
			{
				//12.3   = {significand = 123, exponent = -1}
				//0.123  = {significand = 123, exponent = -3}
				//0.0123 = {significand = 123, exponent = -4}
				const StaticString sig{ udec2chars(significand) };
				using Index = IndexOf<decltype(sig)>;
				if (const i32 int_digits{ sig.size() + exponent }; int_digits > 0)
				{
					for (Index i{}; i != static_cast<Index>(int_digits); ++i)
						*p++ = sig[i];
					*p++ = '.';
					for (Index i{ static_cast<Index>(int_digits) }; i != sig.size(); ++i)
						*p++ = sig[i];
				}
				else
				{
					*p++ = '0';
					*p++ = '.';
					for (auto i{ -int_digits }; i--;)
						*p++ = '0';
					for (Index i{}; i != sig.size(); ++i)
						*p++ = sig[i];
				}
			}
		}
	}
	return p;
}
template<$Char Char = c8>
constexpr auto f2chars($Floating auto f)noexcept
{
	StaticString<Char, f2chars_required_buffer_size<decltype(f)>> result;
	result.modify_size(static_cast<IndexOf<decltype(result)>>(f2chars(f, result.data()) - result.data()));
	return result;
}
INLINE constexpr auto* f2chars_precision($Floating auto f, u8 precision, $Char auto* p)noexcept
{
	if consteval { p[f2chars_required_buffer_size<decltype(f)> +precision - 1] = {}; }//test if buffer is big enough
	const IEEE754 ieee754{ f };
	if (ieee754.is_negative())*p++ = '-';
	if (!ieee754.is_finite()) [[unlikely]]
	{
		if (ieee754.mantissa) { *p++ = 'n'; *p++ = 'a'; *p++ = 'n'; }//ieee754.is_nan()
		else/* */ { *p++ = 'i'; *p++ = 'n'; *p++ = 'f'; }
	}
	else
	{
		if (ieee754.is_zero())
		{
			*p++ = '0';
			if (precision)
			{
				*p++ = '.';
				while (precision--)*p++ = '0';
			}
		}
		else
		{
			auto [significand, exponent] { jkj::dragonbox::to_decimal(f, jkj::dragonbox::policy::sign::ignore) };
			using Significand = std::remove_cvref_t<decltype(significand)>;
			using Exponent = std::remove_cvref_t<decltype(exponent)>;
			STATIC_ASSERT($UInt<Significand> && sizeof(Significand) == sizeof(f) && $Same<i32, Exponent>);
			if (exponent >= 0)
			{
				//123 = {significand = 123, exponent = 0}
				//1230 = {significand = 123, exponent = 1}
				p = udec2chars(significand, p);
				while (exponent--)
					*p++ = '0';
				if (precision)
				{
					*p++ = '.';
					while (precision--)*p++ = '0';
				}
			}
			else
			{
				//12.3   = {significand = 123, exponent = -1}
				//0.123  = {significand = 123, exponent = -3}
				//0.0123 = {significand = 123, exponent = -4}
				if (-exponent > precision)
				{
					const u32 excess_digits{ static_cast<u32>(-exponent - precision) };
					if (excess_digits <= std::numeric_limits<Significand>::digits10)//prevent overflow/division by zero if excess_digits is too large for the integer type
					{
						const Significand divisor{ pow10_table<Significand, std::numeric_limits<Significand>::digits10 + 1>[excess_digits] };
						const Significand remainder{ significand % divisor };
						significand /= divisor;
						exponent += excess_digits;
						if (remainder >= divisor / 2)
							significand++;
					}
					else//if the number is vastly smaller than the precision (e.g. 1e-100 with precision 2), it effectively rounds to 0.
					{
						significand = 0;
						exponent += excess_digits;
					}
				}
				const StaticString sig{ udec2chars(significand) };
				using Index = IndexOf<decltype(sig)>;
				const i32 int_digits{ sig.size() + exponent};
				if (int_digits > 0)
					for (Index i{}; i != static_cast<Index>(int_digits); ++i)
						*p++ = sig[i];
				else
					*p++ = '0';
				if (precision)
				{
					*p++ = '.';
					u32 fractional_zeros{ (int_digits < 0) ? static_cast<u32>(-int_digits) : u32{} };
					u8 src_idx{ (int_digits > 0) ? static_cast<u8>(int_digits) : u8{} };
					while (precision)
					{
						if (fractional_zeros)
						{
							*p++ = '0';
							--fractional_zeros;
						}
						else if (src_idx < sig.size())
							*p++ = sig[src_idx++];
						else
							*p++ = '0';
						--precision;
					}
				}
			}
		}
	}
	return p;
}
template<u8 precision, $Char Char = c8>
constexpr auto f2chars_precision($Floating auto f)noexcept
{
	StaticString<Char, f2chars_required_buffer_size<decltype(f)> +precision> result;
	result.modify_size(static_cast<IndexOf<decltype(result)>>(f2chars_precision(f, precision, result.data()) - result.data()));
	return result;
}
#include <string_view>
namespace details::num2charsTests
{
	template<FixedString s>CONSTEVAL bool verify_f2chars($Floating auto f)noexcept { return std::string_view{ s } == std::string_view{ f2chars(f) }; }
	template<FixedString s, u8 precision>CONSTEVAL bool verify_f2chars_precision($Floating auto f)noexcept { return std::string_view{ s } == std::string_view{ f2chars_precision<precision>(f) }; }
	STATIC_ASSERT(verify_f2chars<"inf">(IEEE754<f32>::infinity));
	STATIC_ASSERT(verify_f2chars<"nan">(IEEE754<f32>::nan));
	STATIC_ASSERT(verify_f2chars<"-inf">(-IEEE754<f32>::infinity));
	STATIC_ASSERT(verify_f2chars<"-nan">(-IEEE754<f32>::nan));
	STATIC_ASSERT(verify_f2chars<"inf">(IEEE754<f64>::infinity));
	STATIC_ASSERT(verify_f2chars<"nan">(IEEE754<f64>::nan));
	STATIC_ASSERT(verify_f2chars<"-inf">(-IEEE754<f64>::infinity));
	STATIC_ASSERT(verify_f2chars<"-nan">(-IEEE754<f64>::nan));
	STATIC_ASSERT(verify_f2chars_precision<"inf", 5>(IEEE754<f32>::infinity));
	STATIC_ASSERT(verify_f2chars_precision<"-inf", 2>(-IEEE754<f32>::infinity));
	STATIC_ASSERT(verify_f2chars_precision<"nan", 3>(IEEE754<f32>::nan));
	STATIC_ASSERT(verify_f2chars_precision<"-nan", 0>(-IEEE754<f32>::nan));
#define F2S_TEST(f) STATIC_ASSERT(verify_f2chars<#f>(f32(f)));STATIC_ASSERT(verify_f2chars<#f>(f64(f)));STATIC_ASSERT(verify_f2chars<"-" #f>(-f32(f)));STATIC_ASSERT(verify_f2chars<"-" #f>(-f64(f)))
	F2S_TEST(0);
	F2S_TEST(1);
	F2S_TEST(12);
	F2S_TEST(123);
	F2S_TEST(1234);
	F2S_TEST(123.4);
	F2S_TEST(12.34);
	F2S_TEST(1.234);
	F2S_TEST(0.1234);
	F2S_TEST(0.01234);
	F2S_TEST(0.001234);
#undef F2S_TEST
#define FIXED_TEST(str, val, prec) STATIC_ASSERT(verify_f2chars_precision<str, prec>(f32(val))); STATIC_ASSERT(verify_f2chars_precision<str, prec>(f64(val))); STATIC_ASSERT(verify_f2chars_precision<"-" str, prec>(-f32(val))); STATIC_ASSERT(verify_f2chars_precision<"-" str, prec>(-f64(val)))
	FIXED_TEST("1.20", 1.2, 2);
	FIXED_TEST("1.200", 1.2, 3);
	FIXED_TEST("123.00", 123., 2);
	FIXED_TEST("0.00", 0., 2);
	FIXED_TEST("1.23", 1.23, 2);
	FIXED_TEST("0.1234", 0.1234, 4);
	FIXED_TEST("1.23", 1.234, 2);
	FIXED_TEST("1.23", 1.231, 2);
	FIXED_TEST("0.1", 0.12, 1);
	FIXED_TEST("1.24", 1.236, 2);
	FIXED_TEST("1.24", 1.239, 2);
	FIXED_TEST("0.2", 0.16, 1);
	FIXED_TEST("1.30", 1.299, 2);
	FIXED_TEST("2.00", 1.999, 2);
	FIXED_TEST("10.0", 9.99, 1);
	FIXED_TEST("1.0", 0.99, 1);
	FIXED_TEST("12", 12.1, 0);
	FIXED_TEST("13", 12.9, 0);
	FIXED_TEST("13", 12.5, 0);
	FIXED_TEST("13.1", 13.05, 1);
	FIXED_TEST("13.0", 13.04999, 1);
	FIXED_TEST("0.01", 0.012, 2);
	FIXED_TEST("0.0010", 0.001, 4);
	FIXED_TEST("0.00", 0.001, 2);
	FIXED_TEST("0", 1e-100, 0);
	FIXED_TEST("0.0", 1e-100, 1);
	FIXED_TEST("0.00", 1e-25, 2);
	FIXED_TEST("0", 1e-22, 0);
	FIXED_TEST("0.00000", 1e-70, 5);
	FIXED_TEST("0", 1e-65, 0);
	FIXED_TEST("10.0", 9.99, 1);
	FIXED_TEST("1.00", 0.999, 2);
	FIXED_TEST("0.10000000000000000000", 0.1, 20);
#undef FIXED_TEST
}