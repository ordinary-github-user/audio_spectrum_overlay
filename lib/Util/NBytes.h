#pragma once
#include <immintrin.h>
#include "BasicTypes.h"
template<u8>struct NBytesImpl;
template<>struct NBytesImpl<1> { using type = u8; };
template<>struct NBytesImpl<2> { using type = u16; };
template<>struct NBytesImpl<4> { using type = u32; };
template<>struct NBytesImpl<8> { using type = u64; };
template<>struct NBytesImpl<16> { using type = __m128i; };
template<>struct NBytesImpl<32> { using type = __m256i; };
template<>struct NBytesImpl<64> { using type = __m512i; };
template<u8 N>using NBytes = NBytesImpl<N>::type;

template<u8>struct NBitsImpl;
template<>struct NBitsImpl<8> { using type = u8; };
template<>struct NBitsImpl<16> { using type = u16; };
template<>struct NBitsImpl<32> { using type = u32; };
template<>struct NBitsImpl<64> { using type = u64; };
template<u8 N>using NBits = NBitsImpl<N>::type;