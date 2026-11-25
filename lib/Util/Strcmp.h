#pragma once
#include "DataStructures/FixedStringImpl.h"
#include "StrcmpImpl.h"
namespace Strcmp
{
	template<FixedString s, Encoding encoding, bool case_sensitive>using Equal = std::remove_cvref_t<decltype([]<usize...I>(std::index_sequence<I...>)static
	{
		return EqualImpl<ElementOf<decltype(s)>, encoding, case_sensitive, s[I]...>{};
	}(std::make_index_sequence<s.size()>{}))>;
	template<FixedString s>using Less    = std::remove_cvref_t<decltype([]<usize...I>(std::index_sequence<I...>)static{return LessImpl   <ElementOf<decltype(s)>, s[I]...>{};}(std::make_index_sequence<s.size()>{}))>;
	template<FixedString s>using Greater = std::remove_cvref_t<decltype([]<usize...I>(std::index_sequence<I...>)static{return GreaterImpl<ElementOf<decltype(s)>, s[I]...>{};}(std::make_index_sequence<s.size()>{}))>;
}
template<FixedString s>INLINE constexpr bool iequal_unsafe($CharOfSize<sizeof(s[0])> auto* x)noexcept{ return Strcmp::Equal<s, Strcmp::Encoding::ascii, 0>::match(x); }
template<FixedString s>INLINE constexpr bool equal_unsafe ($CharOfSize<sizeof(s[0])> auto* x)noexcept{ return Strcmp::Equal<s, Strcmp::Encoding::ascii, 1>::match(x); }
template<FixedString s>INLINE constexpr bool iequal       ($StringOfCharSize<sizeof(s[0])> auto&& x)noexcept{ return x.size() == s.size() && Strcmp::Equal<s, Strcmp::Encoding::ascii, 0>::match(x.data()); }
template<FixedString s>INLINE constexpr bool equal        ($StringOfCharSize<sizeof(s[0])> auto&& x)noexcept{ return x.size() == s.size() && Strcmp::Equal<s, Strcmp::Encoding::ascii, 1>::match(x.data()); }
template<FixedString s>INLINE constexpr bool less         ($StringOfCharSize<sizeof(s[0])> auto&& x)noexcept{ return Strcmp::Less   <s>::match(x.begin(), x.end()); }
template<FixedString s>INLINE constexpr bool greater      ($StringOfCharSize<sizeof(s[0])> auto&& x)noexcept{ return Strcmp::Greater<s>::match(x.begin(), x.end()); }
namespace Wtf16
{
	template<FixedString s>INLINE constexpr bool iequal_unsafe($CharOfSize<sizeof(s[0])> auto* x)noexcept{ return Strcmp::Equal<s, Strcmp::Encoding::wtf16, 0>::match(x); }
	template<FixedString s>INLINE constexpr bool equal_unsafe ($CharOfSize<sizeof(s[0])> auto* x)noexcept{ return Strcmp::Equal<s, Strcmp::Encoding::wtf16, 1>::match(x); }
	template<FixedString s>INLINE constexpr bool iequal       ($StringOfCharSize<sizeof(s[0])> auto&& x)noexcept{ return x.size() == s.size() && Strcmp::Equal<s, Strcmp::Encoding::wtf16, 0>::match(x.data()); }
	template<FixedString s>INLINE constexpr bool equal        ($StringOfCharSize<sizeof(s[0])> auto&& x)noexcept{ return x.size() == s.size() && Strcmp::Equal<s, Strcmp::Encoding::wtf16, 1>::match(x.data()); }
}

STATIC_ASSERT(0==(less<"24">(FixedString{""})));
STATIC_ASSERT(1==(greater<"24">(FixedString{""})));
STATIC_ASSERT(0==(less<"5771">(FixedString{""})));
STATIC_ASSERT(1==(greater<"5771">(FixedString{""})));
STATIC_ASSERT(0==(less<"68">(FixedString{"61"})));
STATIC_ASSERT(1==(greater<"68">(FixedString{"61"})));
STATIC_ASSERT(1==(less<"506">(FixedString{"91616"})));
STATIC_ASSERT(0==(greater<"506">(FixedString{"91616"})));
STATIC_ASSERT(0==(less<"70965">(FixedString{"540"})));
STATIC_ASSERT(1==(greater<"70965">(FixedString{"540"})));
STATIC_ASSERT(1==(less<"13">(FixedString{"388"})));
STATIC_ASSERT(0==(greater<"13">(FixedString{"388"})));
STATIC_ASSERT(0==(less<"77">(FixedString{"73283"})));
STATIC_ASSERT(1==(greater<"77">(FixedString{"73283"})));
STATIC_ASSERT(0==(less<"4118">(FixedString{""})));
STATIC_ASSERT(1==(greater<"4118">(FixedString{""})));
STATIC_ASSERT(1==(less<"6">(FixedString{"8"})));
STATIC_ASSERT(0==(greater<"6">(FixedString{"8"})));
STATIC_ASSERT(1==(less<"6">(FixedString{"93399"})));
STATIC_ASSERT(0==(greater<"6">(FixedString{"93399"})));
STATIC_ASSERT(0==(less<"533">(FixedString{""})));
STATIC_ASSERT(1==(greater<"533">(FixedString{""})));
STATIC_ASSERT(1==(less<"0918">(FixedString{"351"})));
STATIC_ASSERT(0==(greater<"0918">(FixedString{"351"})));
STATIC_ASSERT(1==(less<"3">(FixedString{"95"})));
STATIC_ASSERT(0==(greater<"3">(FixedString{"95"})));
STATIC_ASSERT(0==(less<"6199">(FixedString{"0563"})));
STATIC_ASSERT(1==(greater<"6199">(FixedString{"0563"})));
STATIC_ASSERT(0==(less<"183">(FixedString{""})));
STATIC_ASSERT(1==(greater<"183">(FixedString{""})));
STATIC_ASSERT(0==(less<"546">(FixedString{"0971"})));
STATIC_ASSERT(1==(greater<"546">(FixedString{"0971"})));
STATIC_ASSERT(0==(less<"456">(FixedString{"09"})));
STATIC_ASSERT(1==(greater<"456">(FixedString{"09"})));
STATIC_ASSERT(0==(less<"7265">(FixedString{"1977"})));
STATIC_ASSERT(1==(greater<"7265">(FixedString{"1977"})));
STATIC_ASSERT(0==(less<"8719">(FixedString{"01"})));
STATIC_ASSERT(1==(greater<"8719">(FixedString{"01"})));
STATIC_ASSERT(1==(less<"1">(FixedString{"75925"})));
STATIC_ASSERT(0==(greater<"1">(FixedString{"75925"})));
STATIC_ASSERT(0==(less<"80">(FixedString{"3"})));
STATIC_ASSERT(1==(greater<"80">(FixedString{"3"})));
STATIC_ASSERT(0==(less<"84">(FixedString{"3322"})));
STATIC_ASSERT(1==(greater<"84">(FixedString{"3322"})));
STATIC_ASSERT(0==(less<"787">(FixedString{"01013"})));
STATIC_ASSERT(1==(greater<"787">(FixedString{"01013"})));
STATIC_ASSERT(0==(less<"82">(FixedString{"483"})));
STATIC_ASSERT(1==(greater<"82">(FixedString{"483"})));
STATIC_ASSERT(0==(less<"487">(FixedString{"2"})));
STATIC_ASSERT(1==(greater<"487">(FixedString{"2"})));
STATIC_ASSERT(1==(less<"32781">(FixedString{"47"})));
STATIC_ASSERT(0==(greater<"32781">(FixedString{"47"})));
STATIC_ASSERT(1==(less<"">(FixedString{"28"})));
STATIC_ASSERT(0==(greater<"">(FixedString{"28"})));
STATIC_ASSERT(0==(less<"349">(FixedString{"1"})));
STATIC_ASSERT(1==(greater<"349">(FixedString{"1"})));
STATIC_ASSERT(0==(less<"784">(FixedString{"68685"})));
STATIC_ASSERT(1==(greater<"784">(FixedString{"68685"})));
STATIC_ASSERT(0==(less<"89">(FixedString{"8048"})));
STATIC_ASSERT(1==(greater<"89">(FixedString{"8048"})));
STATIC_ASSERT(0==(less<"5305">(FixedString{""})));
STATIC_ASSERT(1==(greater<"5305">(FixedString{""})));
STATIC_ASSERT(1==(less<"40077">(FixedString{"98"})));
STATIC_ASSERT(0==(greater<"40077">(FixedString{"98"})));
STATIC_ASSERT(0==(less<"35207">(FixedString{""})));
STATIC_ASSERT(1==(greater<"35207">(FixedString{""})));
STATIC_ASSERT(0==(less<"1">(FixedString{""})));
STATIC_ASSERT(1==(greater<"1">(FixedString{""})));
STATIC_ASSERT(1==(less<"">(FixedString{"12805"})));
STATIC_ASSERT(0==(greater<"">(FixedString{"12805"})));
STATIC_ASSERT(0==(less<"">(FixedString{""})));
STATIC_ASSERT(0==(greater<"">(FixedString{""})));
STATIC_ASSERT(1==(less<"129">(FixedString{"6798"})));
STATIC_ASSERT(0==(greater<"129">(FixedString{"6798"})));
STATIC_ASSERT(1==(less<"128">(FixedString{"7"})));
STATIC_ASSERT(0==(greater<"128">(FixedString{"7"})));
STATIC_ASSERT(1==(less<"4165">(FixedString{"57199"})));
STATIC_ASSERT(0==(greater<"4165">(FixedString{"57199"})));
STATIC_ASSERT(1==(less<"01326">(FixedString{"1"})));
STATIC_ASSERT(0==(greater<"01326">(FixedString{"1"})));
STATIC_ASSERT(0==(less<"9651">(FixedString{"298"})));
STATIC_ASSERT(1==(greater<"9651">(FixedString{"298"})));
STATIC_ASSERT(1==(less<"">(FixedString{"91"})));
STATIC_ASSERT(0==(greater<"">(FixedString{"91"})));
STATIC_ASSERT(0==(less<"838">(FixedString{""})));
STATIC_ASSERT(1==(greater<"838">(FixedString{""})));
STATIC_ASSERT(1==(less<"4022">(FixedString{"694"})));
STATIC_ASSERT(0==(greater<"4022">(FixedString{"694"})));
STATIC_ASSERT(0==(less<"20950">(FixedString{""})));
STATIC_ASSERT(1==(greater<"20950">(FixedString{""})));
STATIC_ASSERT(1==(less<"273">(FixedString{"7"})));
STATIC_ASSERT(0==(greater<"273">(FixedString{"7"})));
STATIC_ASSERT(0==(less<"2">(FixedString{""})));
STATIC_ASSERT(1==(greater<"2">(FixedString{""})));
STATIC_ASSERT(0==(less<"830">(FixedString{"762"})));
STATIC_ASSERT(1==(greater<"830">(FixedString{"762"})));
STATIC_ASSERT(1==(less<"4548">(FixedString{"52582"})));
STATIC_ASSERT(0==(greater<"4548">(FixedString{"52582"})));
STATIC_ASSERT(1==(less<"47800">(FixedString{"5"})));
STATIC_ASSERT(0==(greater<"47800">(FixedString{"5"})));
STATIC_ASSERT(1==(less<"67">(FixedString{"950"})));
STATIC_ASSERT(0==(greater<"67">(FixedString{"950"})));
STATIC_ASSERT(1==(less<"0">(FixedString{"7"})));
STATIC_ASSERT(0==(greater<"0">(FixedString{"7"})));
STATIC_ASSERT(1==(less<"3">(FixedString{"5459"})));
STATIC_ASSERT(0==(greater<"3">(FixedString{"5459"})));
STATIC_ASSERT(1==(less<"0984">(FixedString{"1180"})));
STATIC_ASSERT(0==(greater<"0984">(FixedString{"1180"})));
STATIC_ASSERT(1==(less<"">(FixedString{"4401"})));
STATIC_ASSERT(0==(greater<"">(FixedString{"4401"})));
STATIC_ASSERT(0==(less<"7">(FixedString{"085"})));
STATIC_ASSERT(1==(greater<"7">(FixedString{"085"})));
STATIC_ASSERT(0==(less<"5396">(FixedString{"098"})));
STATIC_ASSERT(1==(greater<"5396">(FixedString{"098"})));
STATIC_ASSERT(1==(less<"57">(FixedString{"76"})));
STATIC_ASSERT(0==(greater<"57">(FixedString{"76"})));
STATIC_ASSERT(1==(less<"">(FixedString{"08255"})));
STATIC_ASSERT(0==(greater<"">(FixedString{"08255"})));
STATIC_ASSERT(0==(less<"8696">(FixedString{"70060"})));
STATIC_ASSERT(1==(greater<"8696">(FixedString{"70060"})));
STATIC_ASSERT(0==(less<"3465">(FixedString{"040"})));
STATIC_ASSERT(1==(greater<"3465">(FixedString{"040"})));
STATIC_ASSERT(1==(less<"070">(FixedString{"7"})));
STATIC_ASSERT(0==(greater<"070">(FixedString{"7"})));
STATIC_ASSERT(1==(less<"1061">(FixedString{"7"})));
STATIC_ASSERT(0==(greater<"1061">(FixedString{"7"})));
STATIC_ASSERT(0==(less<"90">(FixedString{""})));
STATIC_ASSERT(1==(greater<"90">(FixedString{""})));
STATIC_ASSERT(1==(less<"57131">(FixedString{"87489"})));
STATIC_ASSERT(0==(greater<"57131">(FixedString{"87489"})));
STATIC_ASSERT(0==(less<"606">(FixedString{"5"})));
STATIC_ASSERT(1==(greater<"606">(FixedString{"5"})));
STATIC_ASSERT(0==(less<"910">(FixedString{"11"})));
STATIC_ASSERT(1==(greater<"910">(FixedString{"11"})));
STATIC_ASSERT(0==(less<"9916">(FixedString{""})));
STATIC_ASSERT(1==(greater<"9916">(FixedString{""})));
STATIC_ASSERT(0==(less<"7543">(FixedString{""})));
STATIC_ASSERT(1==(greater<"7543">(FixedString{""})));
STATIC_ASSERT(0==(less<"4833">(FixedString{"3809"})));
STATIC_ASSERT(1==(greater<"4833">(FixedString{"3809"})));
STATIC_ASSERT(0==(less<"6">(FixedString{"50816"})));
STATIC_ASSERT(1==(greater<"6">(FixedString{"50816"})));
STATIC_ASSERT(1==(less<"054">(FixedString{"1"})));
STATIC_ASSERT(0==(greater<"054">(FixedString{"1"})));
STATIC_ASSERT(1==(less<"22">(FixedString{"4"})));
STATIC_ASSERT(0==(greater<"22">(FixedString{"4"})));
STATIC_ASSERT(0==(less<"655">(FixedString{"36"})));
STATIC_ASSERT(1==(greater<"655">(FixedString{"36"})));
STATIC_ASSERT(1==(less<"1">(FixedString{"86"})));
STATIC_ASSERT(0==(greater<"1">(FixedString{"86"})));
STATIC_ASSERT(0==(less<"79742">(FixedString{""})));
STATIC_ASSERT(1==(greater<"79742">(FixedString{""})));
STATIC_ASSERT(1==(less<"6086">(FixedString{"9432"})));
STATIC_ASSERT(0==(greater<"6086">(FixedString{"9432"})));
STATIC_ASSERT(1==(less<"6485">(FixedString{"791"})));
STATIC_ASSERT(0==(greater<"6485">(FixedString{"791"})));
STATIC_ASSERT(1==(less<"">(FixedString{"2319"})));
STATIC_ASSERT(0==(greater<"">(FixedString{"2319"})));
STATIC_ASSERT(0==(less<"9139">(FixedString{"54"})));
STATIC_ASSERT(1==(greater<"9139">(FixedString{"54"})));
STATIC_ASSERT(1==(less<"">(FixedString{"35475"})));
STATIC_ASSERT(0==(greater<"">(FixedString{"35475"})));
STATIC_ASSERT(0==(less<"50">(FixedString{"080"})));
STATIC_ASSERT(1==(greater<"50">(FixedString{"080"})));
STATIC_ASSERT(1==(less<"743">(FixedString{"79788"})));
STATIC_ASSERT(0==(greater<"743">(FixedString{"79788"})));
STATIC_ASSERT(1==(less<"73">(FixedString{"9"})));
STATIC_ASSERT(0==(greater<"73">(FixedString{"9"})));
STATIC_ASSERT(1==(less<"">(FixedString{"33"})));
STATIC_ASSERT(0==(greater<"">(FixedString{"33"})));
STATIC_ASSERT(0==(less<"847">(FixedString{"27613"})));
STATIC_ASSERT(1==(greater<"847">(FixedString{"27613"})));
STATIC_ASSERT(1==(less<"">(FixedString{"9014"})));
STATIC_ASSERT(0==(greater<"">(FixedString{"9014"})));
STATIC_ASSERT(0==(less<"1561">(FixedString{""})));
STATIC_ASSERT(1==(greater<"1561">(FixedString{""})));
STATIC_ASSERT(0==(less<"5587">(FixedString{""})));
STATIC_ASSERT(1==(greater<"5587">(FixedString{""})));
STATIC_ASSERT(0==(less<"67309">(FixedString{"11"})));
STATIC_ASSERT(1==(greater<"67309">(FixedString{"11"})));
STATIC_ASSERT(1==(less<"46475">(FixedString{"9985"})));
STATIC_ASSERT(0==(greater<"46475">(FixedString{"9985"})));
STATIC_ASSERT(0==(less<"58">(FixedString{"01"})));
STATIC_ASSERT(1==(greater<"58">(FixedString{"01"})));
STATIC_ASSERT(1==(less<"21">(FixedString{"7433"})));
STATIC_ASSERT(0==(greater<"21">(FixedString{"7433"})));
STATIC_ASSERT(0==(less<"9">(FixedString{"71"})));
STATIC_ASSERT(1==(greater<"9">(FixedString{"71"})));
STATIC_ASSERT(1==(less<"19">(FixedString{"8"})));
STATIC_ASSERT(0==(greater<"19">(FixedString{"8"})));
STATIC_ASSERT(0==(less<"57">(FixedString{"3"})));
STATIC_ASSERT(1==(greater<"57">(FixedString{"3"})));
STATIC_ASSERT(1==(less<"8814">(FixedString{"96"})));
STATIC_ASSERT(0==(greater<"8814">(FixedString{"96"})));
STATIC_ASSERT(1==(less<"1">(FixedString{"3222"})));
STATIC_ASSERT(0==(greater<"1">(FixedString{"3222"})));
STATIC_ASSERT(0==(less<"4">(FixedString{"049"})));
STATIC_ASSERT(1==(greater<"4">(FixedString{"049"})));
STATIC_ASSERT(1==(less<"1186">(FixedString{"3"})));
STATIC_ASSERT(0==(greater<"1186">(FixedString{"3"})));
#if 0//compile time += 1min
	#define STRCMP_EQUAL_TEST(cost128, cost256, cost512, str, encoding, casesentitivity)STATIC_ASSERT((Strcmp::cmpOptions1.size()==16?cost128:(Strcmp::cmpOptions1.size()==32?cost256:cost512))==Strcmp::Equal<str, Strcmp::Encoding::encoding, casesentitivity>::cost)
	STRCMP_EQUAL_TEST(0, 0, 0, "", ascii, false); // 001: c8, len=0
	STRCMP_EQUAL_TEST(12, 12, 12, U"R", ascii, true); // 002: c32, len=1
	STRCMP_EQUAL_TEST(11, 11, 11, u"7", wtf16, false); // 003: c16, len=1
	STRCMP_EQUAL_TEST(21, 21, 21, "0m", ascii, false); // 004: c8, len=2
	STRCMP_EQUAL_TEST(12, 12, 12, u"YB", wtf16, true); // 005: c16, len=2
	STRCMP_EQUAL_TEST(25, 25, 25, U"3qa", ascii, true); // 006: c32, len=3
	STRCMP_EQUAL_TEST(33, 33, 33, u"NR_", wtf16, false); // 007: c16, len=3
	STRCMP_EQUAL_TEST(22, 22, 22, "a3aq", ascii, false); // 008: c8, len=4
	STRCMP_EQUAL_TEST(13, 13, 13, u"Y_Yx", wtf16, true); // 009: c16, len=4
	STRCMP_EQUAL_TEST(26, 26, 26, U"qm7_q", ascii, true); // 010: c32, len=5
	STRCMP_EQUAL_TEST(34, 34, 34, u"0Nmqa", wtf16, false); // 011: c16, len=5
	STRCMP_EQUAL_TEST(43, 43, 43, "NR0RR3", ascii, false); // 012: c8, len=6
	STRCMP_EQUAL_TEST(25, 25, 25, u"R5N3aq", wtf16, true); // 013: c16, len=6
	STRCMP_EQUAL_TEST(28, 28, 28, U"qqR_Bmm", ascii, true); // 014: c32, len=7
	STRCMP_EQUAL_TEST(46, 46, 46, u"_B-N3qY", wtf16, false); // 015: c16, len=7
	STRCMP_EQUAL_TEST(23, 23, 23, "_qqBYx_m", ascii, false); // 016: c8, len=8
	STRCMP_EQUAL_TEST(14, 14, 14, u"-B0-RB5_", wtf16, true); // 017: c16, len=8
	STRCMP_EQUAL_TEST(40, 27, 27, U"xm_7x_RR7", ascii, true); // 018: c32, len=9
	STRCMP_EQUAL_TEST(35, 35, 35, u"33NNYY357", wtf16, false); // 019: c16, len=9
	STRCMP_EQUAL_TEST(44, 44, 44, "m-xNa0xmma", ascii, false); // 020: c8, len=10
	STRCMP_EQUAL_TEST(26, 26, 26, u"0733_aBqY-", wtf16, true); // 021: c16, len=10
	STRCMP_EQUAL_TEST(42, 29, 29, U"RBB7R5Bmq0Y", ascii, true); // 022: c32, len=11
	STRCMP_EQUAL_TEST(47, 47, 47, u"a7B7q5Bm7B5", wtf16, false); // 023: c16, len=11
	STRCMP_EQUAL_TEST(45, 45, 45, "B00a0N7ax3mY", ascii, false); // 024: c8, len=12
	STRCMP_EQUAL_TEST(27, 27, 27, u"aaaN0q3mBq5R", wtf16, true); // 025: c16, len=12
	STRCMP_EQUAL_TEST(54, 30, 30, U"a0aYNRa--R3Ya", ascii, true); // 026: c32, len=13
	STRCMP_EQUAL_TEST(48, 48, 48, u"RBB-5_R-35-70", wtf16, false); // 027: c16, len=13
	STRCMP_EQUAL_TEST(46, 46, 46, "x_x7--0Bxa7Rqx", ascii, false); // 028: c8, len=14
	STRCMP_EQUAL_TEST(28, 28, 28, u"0Y3BNa_qYN-mNq", wtf16, true); // 029: c16, len=14
	STRCMP_EQUAL_TEST(56, 30, 30, U"x53m05NY3a-Yq_x", ascii, true); // 030: c32, len=15
	STRCMP_EQUAL_TEST(48, 48, 48, u"B0357B0_Ym7Baa3", wtf16, false); // 031: c16, len=15
	STRCMP_EQUAL_TEST(24, 24, 24, "x30BaB-B7aq57Nmx", ascii, false); // 032: c8, len=16
	STRCMP_EQUAL_TEST(28, 15, 15, u"N3x-R55x3-7N-xmY", wtf16, true); // 033: c16, len=16
	STRCMP_EQUAL_TEST(68, 42, 28, U"-5maB_YY57-m--m5x", ascii, true); // 034: c32, len=17
	STRCMP_EQUAL_TEST(59, 46, 46, u"R5Nma30_33a_0x-7R", wtf16, false); // 035: c16, len=17
	STRCMP_EQUAL_TEST(35, 35, 35, "_-377_5m-3Y03Nq-Ra", ascii, false); // 036: c8, len=18
	STRCMP_EQUAL_TEST(40, 27, 27, u"NY70mqq_N__03a0x55", wtf16, true); // 037: c16, len=18
	STRCMP_EQUAL_TEST(70, 44, 30, U"5xN_xxamRqxa0N-3R-x", ascii, true); // 038: c32, len=19
	STRCMP_EQUAL_TEST(61, 48, 48, u"xBmx73N7_7_7-q5Rq37", wtf16, false); // 039: c16, len=19
	STRCMP_EQUAL_TEST(46, 46, 46, "xqx-Raa0mR07m-0a-R5x", ascii, false); // 040: c8, len=20
	STRCMP_EQUAL_TEST(41, 28, 28, u"5x0m5YY-333xR_YaqmYR", wtf16, true); // 041: c16, len=20
	STRCMP_EQUAL_TEST(82, 45, 31, U"mB3--3-x--7-maR3RBq03", ascii, true); // 042: c32, len=21
	STRCMP_EQUAL_TEST(72, 49, 49, u"-0_0Yx-B_YRB-35RR07x3", wtf16, false); // 043: c16, len=21
	STRCMP_EQUAL_TEST(47, 47, 47, "RYxa_557_aNR0B_-35BYxx", ascii, false); // 044: c8, len=22
	STRCMP_EQUAL_TEST(42, 29, 29, u"NYmx03Y___7-a7Y_5N7xRR", wtf16, true); // 045: c16, len=22
	STRCMP_EQUAL_TEST(84, 45, 31, U"_YNxN_B7q_7aqx0BBBqxN__", ascii, true); // 046: c32, len=23
	STRCMP_EQUAL_TEST(72, 49, 49, u"35Rm3N73YBNR-RR30NB_R5B", wtf16, false); // 047: c16, len=23
	STRCMP_EQUAL_TEST(47, 47, 47, "70xR0-5_YRm3YN333B0N7xa7", ascii, false); // 048: c8, len=24
	STRCMP_EQUAL_TEST(42, 29, 29, u"Ba-77-0-N3BxNY-aa5NY0RB5", wtf16, true); // 049: c16, len=24
	STRCMP_EQUAL_TEST(96, 57, 32, U"__xm5-R7xmm0Y350-qaYmBNN3", ascii, true); // 050: c32, len=25
	STRCMP_EQUAL_TEST(93, 50, 50, u"Ba3503a3a_B7-7NNY3_75R0YY", wtf16, false); // 051: c16, len=25
	STRCMP_EQUAL_TEST(48, 48, 48, "xaa-aR_RamYm0B7_350RamBYmR", ascii, false); // 052: c8, len=26
	STRCMP_EQUAL_TEST(54, 30, 30, u"aaqm-x-03aY500Yq3-0_0-7xYa", wtf16, true); // 053: c16, len=26
	STRCMP_EQUAL_TEST(98, 59, 32, U"Y-YqNxYq_5a0-N_R_xaaB7x3N-a", ascii, true); // 054: c32, len=27
	STRCMP_EQUAL_TEST(95, 50, 50, u"Y_3B707xqY-_Ba5RaN-_3507Na7", wtf16, false); // 055: c16, len=27
	STRCMP_EQUAL_TEST(48, 48, 48, "RRN___50qma305-qaqBR3N0x5B_3", ascii, false); // 056: c8, len=28
	STRCMP_EQUAL_TEST(55, 30, 30, u"YY5_mxmaaRxx-a3Rq3Rx5BB_mY07", wtf16, true); // 057: c16, len=28
	STRCMP_EQUAL_TEST(110, 60, 32, U"705B3qBY5--Raxx_BN-_a5RmmmqaN", ascii, true); // 058: c32, len=29
	STRCMP_EQUAL_TEST(96, 50, 50, u"7_N0N33mx0_RB-00RYN50aNBY30xB", wtf16, false); // 059: c16, len=29
	STRCMP_EQUAL_TEST(48, 48, 48, "-x-_NY0m0qR05Rq0R5_q0x_0--5a__", ascii, false); // 060: c8, len=30
	STRCMP_EQUAL_TEST(56, 30, 30, u"R3RBNYRYxmqxN-a_aaqq573Y0xqY53", wtf16, true); // 061: c16, len=30
	STRCMP_EQUAL_TEST(112, 60, 32, U"m30_7-350mNBN-N555_3-_5q0qm7Yx0", ascii, true); // 062: c32, len=31
	STRCMP_EQUAL_TEST(96, 50, 50, u"__xaN3R_0ma530xx3R5Ra5m770_NmaN", wtf16, false); // 063: c16, len=31
	STRCMP_EQUAL_TEST(48, 25, 25, "NY0aq753aBN_3YRa-x0-RY__amB03N_3", ascii, false); // 064: c8, len=32
	STRCMP_EQUAL_TEST(56, 30, 16, u"YBY_qN_x_-5qaqY_YBRR7R-305a5B7_5", wtf16, true); // 065: c16, len=32
	STRCMP_EQUAL_TEST(124, 72, 44, U"7N7N_-B5B_R5m37Y-07Nqx7a3Yx57Ba-7", ascii, true); // 066: c32, len=33
	STRCMP_EQUAL_TEST(107, 61, 47, u"RqRRx307Y-Ym73a--qxNBmx_q7R0qxx5x", wtf16, false); // 067: c16, len=33
	STRCMP_EQUAL_TEST(59, 36, 36, "7B5-ammN7-_0-q_0--B-BB7Nm5x3q5mN-_", ascii, false); // 068: c8, len=34
	STRCMP_EQUAL_TEST(68, 42, 28, u"qm5_q303__0BNma530-7Y3-7YRmxY5Nm73", wtf16, true); // 069: c16, len=34
	STRCMP_EQUAL_TEST(126, 74, 46, U"-5777R0YqaR-qa0BBm-5_3-5_50Y5YRamNY", ascii, true); // 070: c32, len=35
	STRCMP_EQUAL_TEST(119, 73, 49, u"__3xxYaq_Y_3_N5-xNqBNB7B_-q_BR7333m", wtf16, false); // 071: c16, len=35
	STRCMP_EQUAL_TEST(70, 47, 47, "x3RB3RB3__75xRRa0-N00_3BY_Baama_3N0R", ascii, false); // 072: c8, len=36
	STRCMP_EQUAL_TEST(69, 43, 29, u"_R3553R0qYN70Bm-N5x375Y3YNRY75mN0Yq3", wtf16, true); // 073: c16, len=36
	STRCMP_EQUAL_TEST(138, 75, 47, U"N7Yx570BBqRxR-YBm737xRxam70N3Na7RB5_B", ascii, true); // 074: c32, len=37
	STRCMP_EQUAL_TEST(120, 74, 50, u"3Yq-aY-0Nq5R0-3-qRxYmaN-Y-Bmx53q77BNN", wtf16, false); // 075: c16, len=37
	STRCMP_EQUAL_TEST(71, 48, 48, "BmN77NNB5-xR5mYY0a55qmBN_3B-N73Bx0_N73", ascii, false); // 076: c8, len=38
	STRCMP_EQUAL_TEST(70, 44, 30, u"xmma5507qR555aq3B_Yx7R5NxxB5B-qa7qBYa5", wtf16, true); // 077: c16, len=38
	STRCMP_EQUAL_TEST(140, 75, 47, U"0-mmmqRB7YY307__a_77NNNY0aBNRNmRBx5-0NB", ascii, true); // 078: c32, len=39
	STRCMP_EQUAL_TEST(120, 74, 50, u"mNR_7B3_a-Rx7-Nx7xBq0NB__500YR733-RBq03", wtf16, false); // 079: c16, len=39
	STRCMP_EQUAL_TEST(71, 48, 48, "Bq3N7aYBq075a0x3YRxBxB5_mNa_B5amxYYRN-BN", ascii, false); // 080: c8, len=40
	STRCMP_EQUAL_TEST(70, 44, 30, u"NNq-05N05_xx3BYxxma7Bq_N-xa5_mBx07ax3mmR", wtf16, true); // 081: c16, len=40
	STRCMP_EQUAL_TEST(152, 87, 48, U"3am7R5m5axR-qx7_YYYY30Bqx07_0_NN_5YqYmB3N", ascii, true); // 082: c32, len=41
	STRCMP_EQUAL_TEST(131, 75, 51, u"33-xNmY_x-mm3aRYa3Y757q_N775m-B_qq_R770YY", wtf16, false); // 083: c16, len=41
	STRCMP_EQUAL_TEST(72, 49, 49, "-7_a_5_B5B0x3BY7BY_0_37aRq7xNq5NBqq35R3B-R", ascii, false); // 084: c8, len=42
	STRCMP_EQUAL_TEST(82, 45, 31, u"-7_0qa_a-3RqYmaYa0qYq7-33aY_7BRaaY3x3aN5xm", wtf16, true); // 085: c16, len=42
	STRCMP_EQUAL_TEST(154, 89, 48, U"q_mqBRxY_q37xq3Nq-7Y_mR7Rm5N5m5-Bq53-3_RBx0", ascii, true); // 086: c32, len=43
	STRCMP_EQUAL_TEST(143, 75, 51, u"577R_a-N3-m_N0aN00BY_5BR3_BBm0BRR35a03053-a", wtf16, false); // 087: c16, len=43
	STRCMP_EQUAL_TEST(72, 49, 49, "7qYNaRqNa_aqq3x7qma7-Y57aq_YmYNm_533Ya0-N0xx", ascii, false); // 088: c8, len=44
	STRCMP_EQUAL_TEST(83, 45, 31, u"mN-NY0B-qNYB073RBmBY7357RYxNNN577-mR-3mY0YB0", wtf16, true); // 089: c16, len=44
	STRCMP_EQUAL_TEST(166, 90, 48, U"0-_0B7Nm0Rm37qxqqa3RY-_BYx753-_Nx_xBBx3R-3RmN", ascii, true); // 090: c32, len=45
	STRCMP_EQUAL_TEST(144, 75, 51, u"Y77RNNYa-5qm033mRBBRNN5mYm_35__3RBxRxYR75xqa7", wtf16, false); // 091: c16, len=45
	STRCMP_EQUAL_TEST(72, 49, 49, "aaRmxB_7qY5B_75m7RNYN7BNmNqq55-37-BYR_7x5_a77-", ascii, false); // 092: c8, len=46
	STRCMP_EQUAL_TEST(84, 45, 31, u"-5aBm5303NmR3RaYxxNYq-_m7amYR5qa-q_xa7N-aq5m3Y", wtf16, true); // 093: c16, len=46
	STRCMP_EQUAL_TEST(168, 90, 48, U"_0-3Y3N_m-0N5-5N-m-N-5_N0RNmqqma0xmR0-33Y7NxqN_", ascii, true); // 094: c32, len=47
	STRCMP_EQUAL_TEST(144, 75, 51, u"Y_0q0B7_0BY00q5-0RRxRR--xmqRa5RaNR5Y70Bm7a0Y0mx", wtf16, false); // 095: c16, len=47
	STRCMP_EQUAL_TEST(72, 49, 49, "7maax3R3xN_BR00BaRNm7m5xN3am-__R_R7N3-xRmqN3_xa7", ascii, false); // 096: c8, len=48
	STRCMP_EQUAL_TEST(84, 45, 31, u"m7xY0N5BqaY_35BYx05q7aqxm50__YqR7BRaaR_qmq7axRmY", wtf16, true); // 097: c16, len=48
	STRCMP_EQUAL_TEST(180, 102, 60, U"aB_B5_YNx0m3x3N_axY-amYa50qqqm3m75xYm0-axaBx3R_BN", ascii, true); // 098: c32, len=49
	STRCMP_EQUAL_TEST(155, 86, 52, u"BmYamB33BYRxa0R50q5_Y37Y05N5N-xN_0x33qNaBmB3YN35B", wtf16, false); // 099: c16, len=49
	STRCMP_EQUAL_TEST(96, 57, 32, u"5aNxRmqRYNB035R3x5xm3573B375mY3BN_aRxN73qB5xm3YmY0", wtf16, true); // 100: c16, len=50
	#undef STRCMP_EQUAL_TEST
#endif