#pragma once
#include "BasicTypes.h"
template<u8>struct SignedBitsImpl;
template<u8 Bits> requires(              Bits <= 8 ) struct SignedBitsImpl<Bits> { using type = i8; };
template<u8 Bits> requires(Bits >= 9  && Bits <= 16) struct SignedBitsImpl<Bits> { using type = i16; };
template<u8 Bits> requires(Bits >= 17 && Bits <= 32) struct SignedBitsImpl<Bits> { using type = i32; };
template<u8 Bits> requires(Bits >= 33 && Bits <= 64) struct SignedBitsImpl<Bits> { using type = i64; };
template<u8 Bits> using SignedBits = typename SignedBitsImpl<Bits>::type;
template<u8 Bytes> using SignedBytes = typename SignedBitsImpl<Bytes * 8>::type;

template<u8>struct UnsignedBitsImpl;
template<u8 Bits> requires(              Bits <= 8 ) struct UnsignedBitsImpl<Bits> { using type = u8;  };
template<u8 Bits> requires(Bits >= 9  && Bits <= 16) struct UnsignedBitsImpl<Bits> { using type = u16; };
template<u8 Bits> requires(Bits >= 17 && Bits <= 32) struct UnsignedBitsImpl<Bits> { using type = u32; };
template<u8 Bits> requires(Bits >= 33 && Bits <= 64) struct UnsignedBitsImpl<Bits> { using type = u64; };
template<u8 Bits> using UnsignedBits = typename UnsignedBitsImpl<Bits>::type;
template<u8 Bytes> using UnsignedBytes = typename UnsignedBitsImpl<Bytes * 8>::type;