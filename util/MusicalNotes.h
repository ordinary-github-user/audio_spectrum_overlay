#pragma once
#include "ConceptFloating.h"
#include "FixedStringImpl.h"
#include "Int2Chars.h"
#include "Log.h"
#include "MacroEmpty.h"
#include "MacroIf.h"
#include "Pow.h"
#include "Round.h"
#include "StaticAssert.h"
#include "StaticStringImpl.h"
#include <charconv>
template<$Floating T>
INLINE constexpr T f2midi(T f)noexcept
{
	ASSERT(f > 0);
	return T{ 69 } + T{ 12 } * Math::log2(f / T{ 440 });//MIDI:69 = 440Hz = A4
}
template<$Char T>
INLINE constexpr StaticString<T, sdec2chars_required_buffer_size<i64> +2> midi2note($SInt auto midi)noexcept
{
	StaticString<T, sdec2chars_required_buffer_size<i64> +2> result;
	T* p{ result.data() };
	const i64 octave{ (midi - (midi % 12 + 12) % 12) / 12 - 1 };//floor(midi / 12) - 1
	const u8 k{ (u8)((midi % 12 + 12) % 12) };
	switch (k)
	{
		case 0:  *p++ = 'C';				break;
		case 1:  *p++ = 'C'; *p++ = '#';	break;
		case 2:  *p++ = 'D';				break;
		case 3:  *p++ = 'D'; *p++ = '#';	break;
		case 4:  *p++ = 'E';				break;
		case 5:  *p++ = 'F';				break;
		case 6:  *p++ = 'F'; *p++ = '#';	break;
		case 7:  *p++ = 'G';				break;
		case 8:  *p++ = 'G'; *p++ = '#';	break;
		case 9:  *p++ = 'A';				break;
		case 10: *p++ = 'A'; *p++ = '#';	break;
		case 11: *p++ = 'B';				break;
		default:UNREACHABLE;
	}
	p = sdec2chars(octave, p);
	result.modify_size(result.index_of(p));
	return result;
}
template<$Char T>
INLINE constexpr StaticString<T, sdec2chars_required_buffer_size<i64> +2> f2closestNote($Floating auto f)noexcept
{
	const i64 midi{ static_cast<i64>(Math::round(f2midi(f))) };
	return midi2note<T>(midi);
}
template<$Floating T>
constexpr bool note2f_impl($String auto&& note, T& result)noexcept
{
	auto p{ note.begin() };
	const auto pEnd{ note.end() };
	if (p == pEnd) [[unlikely]] return false;
	i8 k;
	switch (*p)
	{
		case 'C':case 'c': k = 0;  break;
		case 'D':case 'd': k = 2;  break;
		case 'E':case 'e': k = 4;  break;
		case 'F':case 'f': k = 5;  break;
		case 'G':case 'g': k = 7;  break;
		case 'A':case 'a': k = 9;  break;
		case 'B':case 'b': k = 11; break;
		default: [[unlikely]] return false;
	}
	++p;
	if (p == pEnd) [[unlikely]] return false;
	switch (*p)
	{
		case '#':case 's':	++k; ++p; break;
		case 'b':			--k; ++p; break;
	}
	i64 octave;
	const std::from_chars_result status{ std::from_chars(p, pEnd, octave) };
	if (status.ec != std::errc{} || status.ptr != pEnd) [[unlikely]] return false;
	const i64 midi{ (octave + 1) * 12 + k };
	result = T{ 440 } * Math::exp2(static_cast<T>(midi - 69) / T{ 12 });
	return true;
}
template<FixedString note, $Floating T = f32>
constexpr auto note2f{ [] static->T//msvc bug: do not replace auto with T
{
	T result;
	if (!note2f_impl(note, result)) UNREACHABLE;
	return result;
}() };
//https://en.wikipedia.org/wiki/Piano_key_frequencies
STATIC_ASSERT([]static
{
	static constexpr auto almost_equal{ [] ($Floating auto l, $Floating auto r)static
	{
		using T = std::remove_cvref_t<decltype(l)>;
		static constexpr T error{ T(0.0001) };
		const auto diff{ l - r };
		return diff > -error && diff < error;
	} };
	return almost_equal(note2f<"A-1"> , 13.75000f)
		&& almost_equal(note2f<"A#-1">, 14.56762f)
		&& almost_equal(note2f<"Bb-1">, 14.56762f)
		&& almost_equal(note2f<"B-1"> , 15.43385f)
		&& almost_equal(note2f<"C-0"> , 16.35160f)
		&& almost_equal(note2f<"C0">  , 16.35160f)
		&& almost_equal(note2f<"C#0"> , 17.32391f)
		&& almost_equal(note2f<"D0">  , 18.35405f)
		&& almost_equal(note2f<"D#0"> , 19.44544f)
		&& almost_equal(note2f<"Eb0"> , 19.44544f)
		&& almost_equal(note2f<"E0">  , 20.60172f)
		&& almost_equal(note2f<"F0">  , 21.82676f)
		&& almost_equal(note2f<"F#0"> , 23.12465f)
		&& almost_equal(note2f<"Gb0"> , 23.12465f)
		&& almost_equal(note2f<"G0">  , 24.49971f)
		&& almost_equal(note2f<"G#0"> , 25.95654f)
		&& almost_equal(note2f<"Ab0"> , 25.95654f)
		&& almost_equal(note2f<"A0">  , 27.50000f)
		&& almost_equal(note2f<"C8">  , 4186.009f)
		&& almost_equal(note2f<"C9">  , 8372.018f);
}());
STATIC_ASSERT([] <FixedString...note>static{return (...&&equal<note>(f2closestNote<c8>(note2f<note>)));}.template operator()
<
	"C2",
	"D2",
	"D#2",
	"E2",
	"F2",
	"F#2",
	"G2",
	"G#2",
	"A2",
	"C1",
	"D1",
	"D#1",
	"E1",
	"F1",
	"F#1",
	"G1",
	"G#1",
	"A1",
	"C0",
	"D0",
	"D#0",
	"E0",
	"F0",
	"F#0",
	"G0",
	"G#0",
	"A0",
	"C-1",
	"D-1",
	"D#-1",
	"E-1",
	"F-1",
	"F#-1",
	"G-1",
	"G#-1",
	"A-1",
	"C-2",
	"D-2",
	"D#-2",
	"E-2",
	"F-2",
	"F#-2",
	"G-2",
	"G#-2",
	"A-2"
>());