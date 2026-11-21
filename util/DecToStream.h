#pragma once
#include "AlgorithmReverse.h"
#include "BasicTypes.h"
#include "ConceptSame.h"
#include "Fold.h"
#include "Num2Chars.h"
template<$Int T, u8 FixedLength = 0, c8 Padding = '0'>
requires(!$Same<bool, T>)
struct Dec
{
private:
	T m_value;
public:
	INLINE constexpr Dec(T value)noexcept :m_value{ value } {}
	INLINE Dec(const void* ptr)noexcept requires($Same<usize, T>):m_value{ reinterpret_cast<usize>(ptr) }{}
	INLINE constexpr T value()const noexcept { return m_value; }
};

template<$Int T>Dec(T)->Dec<T>;
template<class T>Dec(T*)->Dec<usize>;

template<u8 FixedLength, c8 Padding = ' ', class T>
requires(!$SInt<T> || !FixedLength || Padding == ' ')
INLINE constexpr Dec<std::conditional_t<std::is_pointer_v<T>, usize, T>, FixedLength, Padding> dec(T x)noexcept { return { x }; }

//u8  max = 255
//u16 max = 65535
//u32 max = 4294967295
//u64 max = 18446744073709551615
//i8  max = 127
//i16 max = 32767
//i32 max = 2147483647
//i64 max = 9223372036854775807
//u8  min = 0
//u16 min = 0
//u32 min = 0
//u64 min = 0
//i8  min = -128
//i16 min = -32768
//i32 min = -2147483648
//i64 min = -9223372036854775808
//u8  max = 255
//u16 max = 65535
//u32 max = 4294967295
//u64 max = 18446744073709551615

template<class Stream, class T, u8 FixedLength, c8 Padding>
requires(!$SInt<T> || !FixedLength || Padding == ' ')
INLINE constexpr Stream& operator<<(Stream& os, Dec<T, FixedLength, Padding> dec)noexcept
{
	using Char = typename Stream::char_type;
	if constexpr (!FixedLength)
	{
		if constexpr ($SInt<T>)
		{
			const StaticString str{ sdec2chars<Char>(dec.value()) };
			os.write(str.data(), str.size());
		}
		else
		{
			const StaticString str{ udec2chars<Char>(dec.value()) };
			os.write(str.data(), str.size());
		}
	}
	else//todo
	{
		if constexpr ($SInt<T>)
		{
			T value{ dec.value() };
			const bool negative{ (value < 0) };
			std::array<Char, FixedLength> buffer
			{
				Fold::construct<std::array<Char, FixedLength>, FixedLength>([&](auto) LAMBDA_INLINE
				{
						// Pull least-significant digit each step without negation/abs.
						const auto d{value % 10};// d ? [-9, 9]
						const Char ch{ static_cast<Char>(negative ? ('0' - d) : ('0' + d)) };
						value /= 10; // truncates toward zero
						return ch;
					})
			};
			Algorithm::reverse(buffer);
			if constexpr (FixedLength > 1)
			{
				// Convert leading zeros to spaces (keep the last digit intact).
				u8 i{};
				for (; i < FixedLength - 1 && buffer[i] == '0'; ++i)
					buffer[i] = ' ';
				if (negative)
				{
					if (i == 0)
						buffer[0] = '-';// No room for a separate sign; overwrite the MS digit.
					else
						buffer[static_cast<u8>(i - 1)] = '-';// Place sign just before first non-space digit.
				}
			}
			else // FixedLength == 1
			{
				if (negative)// Width 1: show '-' for negatives, otherwise the single digit already in buffer[0].
					buffer[0] = '-';
			}
			os.write(buffer.data(), buffer.size());
		}
		else
		{
			T value{ dec.value() };
			std::array<Char, FixedLength> buffer{ Fold::construct<std::array<Char, FixedLength>, FixedLength>([&](auto)LAMBDA_INLINE
			{
				const Char result{ static_cast<Char>((value % 10) + '0')};
				value /= 10;
				return result;
			}) };
			Algorithm::reverse(buffer);//TODO: optimize
			if constexpr (Padding != '0' && FixedLength > 1)
			{
				for (auto it{ buffer.data() }; it != buffer.data() + FixedLength - 1; ++it)
					if (*it == '0')
						*it = Padding;
					else
						break;
			}
			os.write(buffer.data(), buffer.size());
		}
	}
	return os;
}