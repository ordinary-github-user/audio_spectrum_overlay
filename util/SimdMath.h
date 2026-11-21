#pragma once
//https://github.com/Tencent/ncnn/blob/master/src/layer/x86/sse_mathfun.h
//https://github.com/Tencent/ncnn/blob/master/src/layer/x86/avx_mathfun.h
//https://github.com/Tencent/ncnn/blob/master/src/layer/x86/avx512_mathfun.h
/*
   AVX implementation of sin, cos, sincos, exp and log
   Based on "sse_mathfun.h", by Julien Pommier
   http://gruntthepeon.free.fr/ssemath/
   Copyright (C) 2012 Giovanni Garberoglio
   Interdisciplinary Laboratory for Computational Science (LISC)
   Fondazione Bruno Kessler and University of Trento via Sommarive, 18
   I-38123 Trento (Italy)
  This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
  (this is the zlib license)
*/
#include "Inline.h"
#include "Compiler.h"
#include <immintrin.h>
#include <emmintrin.h>
// Tencent is pleased to support the open source community by making ncnn available.
// Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
// https://opensource.org/licenses/BSD-3-Clause
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

#include <math.h>
#if SSE2
	#if AVX2
		#if __XOP__
			#ifdef _MSC_VER
				#include <ammintrin.h>
			#else
				#include <x86intrin.h>
			#endif
		#endif
	#endif
#endif

#if SSE2
	template<u8 i>
	INLINE f32 get(__m128 x)noexcept
	{
		if constexpr (i == 0)
			return _mm_cvtss_f32(x);
		else
			return _mm_cvtss_f32(_mm_shuffle_ps(x, x, _MM_SHUFFLE(i, i, i, i)));
	}

	INLINE f32 _mm_reduce_add_ps(__m128 x128)
	{
		const __m128 x64 = _mm_add_ps(x128, _mm_movehl_ps(x128, x128));
		const __m128 x32 = _mm_add_ss(x64, _mm_shuffle_ps(x64, x64, 0x55));
		return _mm_cvtss_f32(x32);
	}

	INLINE f32 _mm_reduce_max_ps(__m128 x128)
	{
		const __m128 x64 = _mm_max_ps(x128, _mm_movehl_ps(x128, x128));
		const __m128 x32 = _mm_max_ss(x64, _mm_shuffle_ps(x64, x64, 0x55));
		return _mm_cvtss_f32(x32);
	}

	INLINE int _mm_reduce_add_epi32(__m128i x)
	{
		__m128i hi64 = _mm_unpackhi_epi64(x, x);
		__m128i sum64 = _mm_add_epi32(hi64, x);
		__m128i hi32 = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));
		__m128i sum32 = _mm_add_epi32(sum64, hi32);
		return _mm_cvtsi128_si32(sum32);
	}

	INLINE int32_t float2int8_sse(const __m128& _v0)
	{
		// _MM_ROUND_NEAREST round to even
		// simulate round to nearest via +/-0.5 with round to zero
		__m128 _p5 = _mm_set1_ps(0.5f);
		__m128 _signmask = _mm_castsi128_ps(_mm_set1_epi32(1 << 31));
		__m128 _sign0 = _mm_and_ps(_v0, _signmask);
		__m128 _v0_p5 = _mm_or_ps(_p5, _sign0);
		__m128 _v0_adj = _mm_add_ps(_v0, _v0_p5);
		__m128i _v0_i = _mm_cvttps_epi32(_v0_adj);

		__m128i _v0_s16 = _mm_packs_epi32(_v0_i, _v0_i);

		_v0_s16 = _mm_min_epi16(_v0_s16, _mm_set1_epi16(127));
		_v0_s16 = _mm_max_epi16(_v0_s16, _mm_set1_epi16(-127));

		__m128i _v8 = _mm_packs_epi16(_v0_s16, _v0_s16);

	#if defined(__x86_64__) || defined(_M_X64)
		return (int32_t)_mm_cvtsi128_si64(_v8);
	#else
		return _mm_cvtsi128_si32(_v8);
	#endif
	}

	INLINE int64_t float2int8_sse(const __m128& _v0, const __m128& _v1)
	{
		// _MM_ROUND_NEAREST round to even
		// simulate round to nearest via +/-0.5 with round to zero
		__m128 _p5 = _mm_set1_ps(0.5f);
		__m128 _signmask = _mm_castsi128_ps(_mm_set1_epi32(1 << 31));
		__m128 _sign0 = _mm_and_ps(_v0, _signmask);
		__m128 _sign1 = _mm_and_ps(_v1, _signmask);
		__m128 _v0_p5 = _mm_or_ps(_p5, _sign0);
		__m128 _v1_p5 = _mm_or_ps(_p5, _sign1);
		__m128 _v0_adj = _mm_add_ps(_v0, _v0_p5);
		__m128 _v1_adj = _mm_add_ps(_v1, _v1_p5);
		__m128i _v0_i = _mm_cvttps_epi32(_v0_adj);
		__m128i _v1_i = _mm_cvttps_epi32(_v1_adj);

		__m128i _v01_s16 = _mm_packs_epi32(_v0_i, _v1_i);

		_v01_s16 = _mm_min_epi16(_v01_s16, _mm_set1_epi16(127));
		_v01_s16 = _mm_max_epi16(_v01_s16, _mm_set1_epi16(-127));

		__m128i _v8 = _mm_packs_epi16(_v01_s16, _v01_s16);

	#if defined(__x86_64__) || defined(_M_X64)
		return _mm_cvtsi128_si64(_v8);
	#else
		int64_t v8[2];
		_mm_storeu_si128((__m128i*)v8, _v8);
		return v8[0];
	#endif
	}

	INLINE __m128i float2int8_sse(const __m128& _v0, const __m128& _v1, const __m128& _v2, const __m128& _v3)
	{
		// _MM_ROUND_NEAREST round to even
		// simulate round to nearest via +/-0.5 with round to zero
		__m128 _p5 = _mm_set1_ps(0.5f);
		__m128 _signmask = _mm_castsi128_ps(_mm_set1_epi32(1 << 31));
		__m128 _sign0 = _mm_and_ps(_v0, _signmask);
		__m128 _sign1 = _mm_and_ps(_v1, _signmask);
		__m128 _sign2 = _mm_and_ps(_v2, _signmask);
		__m128 _sign3 = _mm_and_ps(_v3, _signmask);
		__m128 _v0_p5 = _mm_or_ps(_p5, _sign0);
		__m128 _v1_p5 = _mm_or_ps(_p5, _sign1);
		__m128 _v2_p5 = _mm_or_ps(_p5, _sign2);
		__m128 _v3_p5 = _mm_or_ps(_p5, _sign3);
		__m128 _v0_adj = _mm_add_ps(_v0, _v0_p5);
		__m128 _v1_adj = _mm_add_ps(_v1, _v1_p5);
		__m128 _v2_adj = _mm_add_ps(_v2, _v2_p5);
		__m128 _v3_adj = _mm_add_ps(_v3, _v3_p5);
		__m128i _v0_i = _mm_cvttps_epi32(_v0_adj);
		__m128i _v1_i = _mm_cvttps_epi32(_v1_adj);
		__m128i _v2_i = _mm_cvttps_epi32(_v2_adj);
		__m128i _v3_i = _mm_cvttps_epi32(_v3_adj);

		__m128i _v01_s16 = _mm_packs_epi32(_v0_i, _v1_i);
		__m128i _v23_s16 = _mm_packs_epi32(_v2_i, _v3_i);

		_v01_s16 = _mm_min_epi16(_v01_s16, _mm_set1_epi16(127));
		_v23_s16 = _mm_min_epi16(_v23_s16, _mm_set1_epi16(127));
		_v01_s16 = _mm_max_epi16(_v01_s16, _mm_set1_epi16(-127));
		_v23_s16 = _mm_max_epi16(_v23_s16, _mm_set1_epi16(-127));

		__m128i _v8 = _mm_packs_epi16(_v01_s16, _v23_s16);

		return _v8;
	}

	#ifndef __FMA__
		INLINE __m128 _mm_comp_fmadd_ps(const __m128& _a, const __m128& _b, const __m128& _c)
		{
			return _mm_add_ps(_mm_mul_ps(_a, _b), _c);
		}
		INLINE __m128 _mm_comp_fnmadd_ps(const __m128& _a, const __m128& _b, const __m128& _c)
		{
			return _mm_sub_ps(_c, _mm_mul_ps(_a, _b));
		}
	#else
		INLINE __m128 _mm_comp_fmadd_ps(const __m128& _a, const __m128& _b, const __m128& _c)
		{
			return _mm_fmadd_ps(_a, _b, _c);
		}
		INLINE __m128 _mm_comp_fnmadd_ps(const __m128& _a, const __m128& _b, const __m128& _c)
		{
			// return -a * b + c
			return _mm_fnmadd_ps(_a, _b, _c);
		}
	#endif // !__FMA__

	#if AVX2
		#ifndef __FMA__
			INLINE __m256 _mm256_comp_fmadd_ps(const __m256& _a, const __m256& _b, const __m256& _c)
			{
				return _mm256_add_ps(_mm256_mul_ps(_a, _b), _c);
			}
			INLINE __m256 _mm256_comp_fnmadd_ps(const __m256& _a, const __m256& _b, const __m256& _c)
			{
				return _mm256_sub_ps(_c, _mm256_mul_ps(_a, _b));
			}
		#else
			INLINE __m256 _mm256_comp_fmadd_ps(const __m256& _a, const __m256& _b, const __m256& _c)
			{
				return _mm256_fmadd_ps(_a, _b, _c);
			}
			INLINE __m256 _mm256_comp_fnmadd_ps(const __m256& _a, const __m256& _b, const __m256& _c)
			{
				// return -a * b + c
				return _mm256_fnmadd_ps(_a, _b, _c);
			}
		#endif

		INLINE __m256 _mm256_fmadd_1_ps(const __m256& a, const __m256& b, f32 c)
		{
			return _mm256_comp_fmadd_ps(b, _mm256_set1_ps(c), a);
		}

		INLINE __m256 _mm256_fmrsub_1_ps(const __m256& a, const __m256& b, f32 c)
		{
			// return a - b * c
			return _mm256_comp_fnmadd_ps(b, _mm256_set1_ps(c), a);
		}

		INLINE void transpose8x12_ps(__m256& _r0, __m256& _r1, __m256& _r2, __m256& _r3, __m256& _r4, __m256& _r5, __m256& _r6, __m256& _r7,
									 __m256& _r8, __m256& _r9, __m256& _ra, __m256& _rb)
		{
			__m256 _tmp0 = _mm256_unpacklo_ps(_r0, _r1);
			__m256 _tmp1 = _mm256_unpackhi_ps(_r0, _r1);
			__m256 _tmp2 = _mm256_unpacklo_ps(_r2, _r3);
			__m256 _tmp3 = _mm256_unpackhi_ps(_r2, _r3);
			__m256 _tmp4 = _mm256_unpacklo_ps(_r4, _r5);
			__m256 _tmp5 = _mm256_unpackhi_ps(_r4, _r5);
			__m256 _tmp6 = _mm256_unpacklo_ps(_r6, _r7);
			__m256 _tmp7 = _mm256_unpackhi_ps(_r6, _r7);
			__m256 _tmp8 = _mm256_unpacklo_ps(_r8, _r9);
			__m256 _tmp9 = _mm256_unpackhi_ps(_r8, _r9);
			__m256 _tmpa = _mm256_unpacklo_ps(_ra, _rb);
			__m256 _tmpb = _mm256_unpackhi_ps(_ra, _rb);

			__m256 _tmpc = _mm256_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmpd = _mm256_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(3, 2, 3, 2));
			__m256 _tmpe = _mm256_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmpf = _mm256_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(3, 2, 3, 2));
			__m256 _tmpg = _mm256_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmph = _mm256_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(3, 2, 3, 2));
			__m256 _tmpi = _mm256_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmpj = _mm256_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(3, 2, 3, 2));
			__m256 _tmpk = _mm256_shuffle_ps(_tmp8, _tmpa, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmpl = _mm256_shuffle_ps(_tmp8, _tmpa, _MM_SHUFFLE(3, 2, 3, 2));
			__m256 _tmpm = _mm256_shuffle_ps(_tmp9, _tmpb, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmpn = _mm256_shuffle_ps(_tmp9, _tmpb, _MM_SHUFFLE(3, 2, 3, 2));

			_r0 = _mm256_permute2f128_ps(_tmpc, _tmpg, _MM_SHUFFLE(0, 2, 0, 0));
			_r1 = _mm256_permute2f128_ps(_tmpk, _tmpd, _MM_SHUFFLE(0, 2, 0, 0));
			_r2 = _mm256_permute2f128_ps(_tmph, _tmpl, _MM_SHUFFLE(0, 2, 0, 0));
			_r3 = _mm256_permute2f128_ps(_tmpe, _tmpi, _MM_SHUFFLE(0, 2, 0, 0));
			_r4 = _mm256_permute2f128_ps(_tmpm, _tmpf, _MM_SHUFFLE(0, 2, 0, 0));
			_r5 = _mm256_permute2f128_ps(_tmpj, _tmpn, _MM_SHUFFLE(0, 2, 0, 0));
			_r6 = _mm256_permute2f128_ps(_tmpc, _tmpg, _MM_SHUFFLE(0, 3, 0, 1));
			_r7 = _mm256_permute2f128_ps(_tmpk, _tmpd, _MM_SHUFFLE(0, 3, 0, 1));
			_r8 = _mm256_permute2f128_ps(_tmph, _tmpl, _MM_SHUFFLE(0, 3, 0, 1));
			_r9 = _mm256_permute2f128_ps(_tmpe, _tmpi, _MM_SHUFFLE(0, 3, 0, 1));
			_ra = _mm256_permute2f128_ps(_tmpm, _tmpf, _MM_SHUFFLE(0, 3, 0, 1));
			_rb = _mm256_permute2f128_ps(_tmpj, _tmpn, _MM_SHUFFLE(0, 3, 0, 1));
		}

		INLINE void transpose8x8_ps(__m256& _r0, __m256& _r1, __m256& _r2, __m256& _r3, __m256& _r4, __m256& _r5, __m256& _r6, __m256& _r7)
		{
			__m256 _tmp0 = _mm256_unpacklo_ps(_r0, _r1);
			__m256 _tmp1 = _mm256_unpackhi_ps(_r0, _r1);
			__m256 _tmp2 = _mm256_unpacklo_ps(_r2, _r3);
			__m256 _tmp3 = _mm256_unpackhi_ps(_r2, _r3);
			__m256 _tmp4 = _mm256_unpacklo_ps(_r4, _r5);
			__m256 _tmp5 = _mm256_unpackhi_ps(_r4, _r5);
			__m256 _tmp6 = _mm256_unpacklo_ps(_r6, _r7);
			__m256 _tmp7 = _mm256_unpackhi_ps(_r6, _r7);

			__m256 _tmp8 = _mm256_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmp9 = _mm256_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(3, 2, 3, 2));
			__m256 _tmpa = _mm256_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmpb = _mm256_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(3, 2, 3, 2));
			__m256 _tmpc = _mm256_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmpd = _mm256_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(3, 2, 3, 2));
			__m256 _tmpe = _mm256_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmpf = _mm256_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(3, 2, 3, 2));

			_r0 = _mm256_permute2f128_ps(_tmp8, _tmpc, _MM_SHUFFLE(0, 2, 0, 0));
			_r1 = _mm256_permute2f128_ps(_tmp9, _tmpd, _MM_SHUFFLE(0, 2, 0, 0));
			_r2 = _mm256_permute2f128_ps(_tmpa, _tmpe, _MM_SHUFFLE(0, 2, 0, 0));
			_r3 = _mm256_permute2f128_ps(_tmpb, _tmpf, _MM_SHUFFLE(0, 2, 0, 0));
			_r4 = _mm256_permute2f128_ps(_tmp8, _tmpc, _MM_SHUFFLE(0, 3, 0, 1));
			_r5 = _mm256_permute2f128_ps(_tmp9, _tmpd, _MM_SHUFFLE(0, 3, 0, 1));
			_r6 = _mm256_permute2f128_ps(_tmpa, _tmpe, _MM_SHUFFLE(0, 3, 0, 1));
			_r7 = _mm256_permute2f128_ps(_tmpb, _tmpf, _MM_SHUFFLE(0, 3, 0, 1));
		}

		INLINE void transpose8x4_ps(__m256& _r0, __m256& _r1, __m256& _r2, __m256& _r3)
		{
			__m256 _tmp0 = _mm256_unpacklo_ps(_r0, _r1);
			__m256 _tmp1 = _mm256_unpackhi_ps(_r0, _r1);
			__m256 _tmp2 = _mm256_unpacklo_ps(_r2, _r3);
			__m256 _tmp3 = _mm256_unpackhi_ps(_r2, _r3);

			__m256 _tmp4 = _mm256_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmp5 = _mm256_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(3, 2, 3, 2));
			__m256 _tmp6 = _mm256_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(1, 0, 1, 0));
			__m256 _tmp7 = _mm256_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(3, 2, 3, 2));

			_r0 = _mm256_permute2f128_ps(_tmp4, _tmp5, _MM_SHUFFLE(0, 2, 0, 0));
			_r1 = _mm256_permute2f128_ps(_tmp6, _tmp7, _MM_SHUFFLE(0, 2, 0, 0));
			_r2 = _mm256_permute2f128_ps(_tmp4, _tmp5, _MM_SHUFFLE(0, 3, 0, 1));
			_r3 = _mm256_permute2f128_ps(_tmp6, _tmp7, _MM_SHUFFLE(0, 3, 0, 1));
		}

		INLINE void transpose8x2_ps(__m256& _r0, __m256& _r1)
		{
			__m256 _tmp0 = _mm256_unpacklo_ps(_r0, _r1);
			__m256 _tmp1 = _mm256_unpackhi_ps(_r0, _r1);

			_r0 = _mm256_permute2f128_ps(_tmp0, _tmp1, _MM_SHUFFLE(0, 2, 0, 0));
			_r1 = _mm256_permute2f128_ps(_tmp0, _tmp1, _MM_SHUFFLE(0, 3, 0, 1));
		}

		INLINE void transpose8x8_epi16(__m128i& _r0, __m128i& _r1, __m128i& _r2, __m128i& _r3, __m128i& _r4, __m128i& _r5, __m128i& _r6, __m128i& _r7)
		{
			__m128i _tmp0 = _mm_unpacklo_epi16(_r0, _r1);
			__m128i _tmp1 = _mm_unpackhi_epi16(_r0, _r1);
			__m128i _tmp2 = _mm_unpacklo_epi16(_r2, _r3);
			__m128i _tmp3 = _mm_unpackhi_epi16(_r2, _r3);
			__m128i _tmp4 = _mm_unpacklo_epi16(_r4, _r5);
			__m128i _tmp5 = _mm_unpackhi_epi16(_r4, _r5);
			__m128i _tmp6 = _mm_unpacklo_epi16(_r6, _r7);
			__m128i _tmp7 = _mm_unpackhi_epi16(_r6, _r7);

			__m128i _tmp8 = _mm_unpacklo_epi32(_tmp0, _tmp2);
			__m128i _tmp9 = _mm_unpackhi_epi32(_tmp0, _tmp2);
			__m128i _tmpa = _mm_unpacklo_epi32(_tmp1, _tmp3);
			__m128i _tmpb = _mm_unpackhi_epi32(_tmp1, _tmp3);
			__m128i _tmpc = _mm_unpacklo_epi32(_tmp4, _tmp6);
			__m128i _tmpd = _mm_unpackhi_epi32(_tmp4, _tmp6);
			__m128i _tmpe = _mm_unpacklo_epi32(_tmp5, _tmp7);
			__m128i _tmpf = _mm_unpackhi_epi32(_tmp5, _tmp7);

			_r0 = _mm_unpacklo_epi64(_tmp8, _tmpc);
			_r1 = _mm_unpackhi_epi64(_tmp8, _tmpc);
			_r2 = _mm_unpacklo_epi64(_tmp9, _tmpd);
			_r3 = _mm_unpackhi_epi64(_tmp9, _tmpd);
			_r4 = _mm_unpacklo_epi64(_tmpa, _tmpe);
			_r5 = _mm_unpackhi_epi64(_tmpa, _tmpe);
			_r6 = _mm_unpacklo_epi64(_tmpb, _tmpf);
			_r7 = _mm_unpackhi_epi64(_tmpb, _tmpf);
		}

		INLINE __m256 HorizontalSums(__m256& v0, __m256& v1, __m256& v2, __m256& v3, __m256& v4, __m256& v5, __m256& v6, __m256& v7)
		{
			const __m256 s01 = _mm256_hadd_ps(v0, v1);
			const __m256 s23 = _mm256_hadd_ps(v2, v3);
			const __m256 s45 = _mm256_hadd_ps(v4, v5);
			const __m256 s67 = _mm256_hadd_ps(v6, v7);
			const __m256 s0123 = _mm256_hadd_ps(s01, s23);
			const __m256 s4556 = _mm256_hadd_ps(s45, s67);

			// inter-lane shuffle
			const __m256 vb0 = _mm256_blend_ps(s0123, s4556, 0xF0);
			const __m256 vb1 = _mm256_permute2f128_ps(s0123, s4556, 0x21);

			return _mm256_add_ps(vb0, vb1);
		}

		INLINE __m128 HorizontalSums(__m256& v0, __m256& v1, __m256& v2, __m256& v3)
		{
			const __m256 s01 = _mm256_hadd_ps(v0, v1);
			const __m256 s23 = _mm256_hadd_ps(v2, v3);
			const __m256 s0123 = _mm256_hadd_ps(s01, s23);

			return _mm_add_ps(_mm256_extractf128_ps(s0123, 1),
							  _mm256_castps256_ps128(s0123));
		}

		INLINE __m128 HorizontalSums(__m256& v0, __m256& v1, __m256& v2)
		{
			const __m256 v3 = _mm256_set1_ps(0.0f);
			const __m256 s01 = _mm256_hadd_ps(v0, v1);
			const __m256 s23 = _mm256_hadd_ps(v2, v3);
			const __m256 s0123 = _mm256_hadd_ps(s01, s23);

			return _mm_add_ps(_mm256_extractf128_ps(s0123, 1),
							  _mm256_castps256_ps128(s0123));
		}

		INLINE f32 _mm256_reduce_add_ps(__m256 x)
		{
			/* ( x3+x7, x2+x6, x1+x5, x0+x4 ) */
			const __m128 x128 = _mm_add_ps(_mm256_extractf128_ps(x, 1), _mm256_castps256_ps128(x));
			/* ( -, -, x1+x3+x5+x7, x0+x2+x4+x6 ) */
			const __m128 x64 = _mm_add_ps(x128, _mm_movehl_ps(x128, x128));
			/* ( -, -, -, x0+x1+x2+x3+x4+x5+x6+x7 ) */
			const __m128 x32 = _mm_add_ss(x64, _mm_shuffle_ps(x64, x64, 0x55));
			/* Conversion to f32 is a no-op on x86-64 */
			return _mm_cvtss_f32(x32);
		}

		INLINE f32 _mm256_reduce_max_ps(__m256 x)
		{
			const __m128 x128 = _mm_max_ps(_mm256_extractf128_ps(x, 1), _mm256_castps256_ps128(x));
			const __m128 x64 = _mm_max_ps(x128, _mm_movehl_ps(x128, x128));
			const __m128 x32 = _mm_max_ss(x64, _mm_shuffle_ps(x64, x64, 0x55));
			return _mm_cvtss_f32(x32);
		}

		INLINE int64_t float2int8_avx(const __m256& _v0)
		{
			// _MM_FROUND_TO_NEAREST_INT round to even
			// simulate round to nearest via +/-0.5 with round to zero
			__m256 _p5 = _mm256_set1_ps(0.5f);
			__m256 _signmask = _mm256_castsi256_ps(_mm256_set1_epi32(1 << 31));
			__m256 _sign = _mm256_and_ps(_v0, _signmask);
			__m256 _v0_p5 = _mm256_or_ps(_p5, _sign);
			__m256 _v0_adj = _mm256_add_ps(_v0, _v0_p5);
			__m256i _v0_i = _mm256_cvttps_epi32(_v0_adj);

		#if AVX2
			__m256i _v01_s16 = _mm256_packs_epi32(_v0_i, _v0_i);
			_v01_s16 = _mm256_permute4x64_epi64(_v01_s16, 0xd8);

			__m128i _v01_s16low = _mm256_extractf128_si256(_v01_s16, 0);
		#else
			__m128i _v0_i_low = _mm256_extractf128_si256(_v0_i, 0);
			__m128i _v0_i_high = _mm256_extractf128_si256(_v0_i, 1);

			__m128i _v01_s16low = _mm_packs_epi32(_v0_i_low, _v0_i_high);
		#endif

			_v01_s16low = _mm_min_epi16(_v01_s16low, _mm_set1_epi16(127));
			_v01_s16low = _mm_max_epi16(_v01_s16low, _mm_set1_epi16(-127));

			__m128i _v8 = _mm_packs_epi16(_v01_s16low, _v01_s16low);

		#if defined(__x86_64__) || defined(_M_X64)
			return _mm_cvtsi128_si64(_v8);
		#else
			int64_t v8[2];
			_mm_storeu_si128((__m128i*)v8, _v8);
			return v8[0];
		#endif
		}

		INLINE __m128i float2int8_avx(const __m256& _v0, const __m256& _v1)
		{
			// _MM_FROUND_TO_NEAREST_INT round to even
			// simulate round to nearest via +/-0.5 with round to zero
			__m256 _p5 = _mm256_set1_ps(0.5f);
			__m256 _signmask = _mm256_castsi256_ps(_mm256_set1_epi32(1 << 31));
			__m256 _sign0 = _mm256_and_ps(_v0, _signmask);
			__m256 _sign1 = _mm256_and_ps(_v1, _signmask);
			__m256 _v0_p5 = _mm256_or_ps(_p5, _sign0);
			__m256 _v1_p5 = _mm256_or_ps(_p5, _sign1);
			__m256 _v0_adj = _mm256_add_ps(_v0, _v0_p5);
			__m256 _v1_adj = _mm256_add_ps(_v1, _v1_p5);
			__m256i _v0_i = _mm256_cvttps_epi32(_v0_adj);
			__m256i _v1_i = _mm256_cvttps_epi32(_v1_adj);

		#if AVX2
			__m256i _v01_s16 = _mm256_packs_epi32(_v0_i, _v1_i);
			_v01_s16 = _mm256_permute4x64_epi64(_v01_s16, 0xd8);

			_v01_s16 = _mm256_min_epi16(_v01_s16, _mm256_set1_epi16(127));
			_v01_s16 = _mm256_max_epi16(_v01_s16, _mm256_set1_epi16(-127));

			__m256i _v8 = _mm256_packs_epi16(_v01_s16, _v01_s16);
			_v8 = _mm256_permute4x64_epi64(_v8, 0xd8);

			return _mm256_extractf128_si256(_v8, 0);
		#else
			__m128i _v0_i_low = _mm256_extractf128_si256(_v0_i, 0);
			__m128i _v0_i_high = _mm256_extractf128_si256(_v0_i, 1);
			__m128i _v1_i_low = _mm256_extractf128_si256(_v1_i, 0);
			__m128i _v1_i_high = _mm256_extractf128_si256(_v1_i, 1);

			__m128i _v01_s16low = _mm_packs_epi32(_v0_i_low, _v0_i_high);
			__m128i _v01_s16high = _mm_packs_epi32(_v1_i_low, _v1_i_high);

			_v01_s16low = _mm_min_epi16(_v01_s16low, _mm_set1_epi16(127));
			_v01_s16high = _mm_min_epi16(_v01_s16high, _mm_set1_epi16(127));
			_v01_s16low = _mm_max_epi16(_v01_s16low, _mm_set1_epi16(-127));
			_v01_s16high = _mm_max_epi16(_v01_s16high, _mm_set1_epi16(-127));

			__m128i _v8 = _mm_packs_epi16(_v01_s16low, _v01_s16high);
			return _v8;
		#endif
		}

		INLINE void _mm256_comp_fmadd_ps4(__m256& _sum,
										  const __m256& _w0, const __m256& _w1, const __m256& _w2, const __m256& _w3,
										  const __m256& _v0, const __m256& _v1, const __m256& _v2, const __m256& _v3)
		{
			__m256 _mul0 = _mm256_mul_ps(_w0, _v0);
			__m256 _mul1 = _mm256_mul_ps(_w1, _v1);
			__m256 _sum01 = _mm256_add_ps(_mul0, _mul1);
			__m256 _mul2 = _mm256_mul_ps(_w2, _v2);
			__m256 _mul3 = _mm256_mul_ps(_w3, _v3);
			__m256 _sum23 = _mm256_add_ps(_mul2, _mul3);
			__m256 _sum0123 = _mm256_add_ps(_sum01, _sum23);
			_sum = _mm256_add_ps(_sum, _sum0123);
		}

		INLINE void _mm256_comp_fmadd_ps8(__m256& _sum,
										  const __m256& _w0, const __m256& _w1, const __m256& _w2, const __m256& _w3, const __m256& _w4, const __m256& _w5, const __m256& _w6, const __m256& _w7,
										  const __m256& _v0, const __m256& _v1, const __m256& _v2, const __m256& _v3, const __m256& _v4, const __m256& _v5, const __m256& _v6, const __m256& _v7)
		{
			_mm256_comp_fmadd_ps4(_sum, _w0, _w1, _w2, _w3, _v0, _v1, _v2, _v3);
			_mm256_comp_fmadd_ps4(_sum, _w4, _w5, _w6, _w7, _v4, _v5, _v6, _v7);
		}

		#if AVX512F
			INLINE void transpose16x16_ps(__m512& _r0, __m512& _r1, __m512& _r2, __m512& _r3, __m512& _r4, __m512& _r5, __m512& _r6, __m512& _r7,
										  __m512& _r8, __m512& _r9, __m512& _ra, __m512& _rb, __m512& _rc, __m512& _rd, __m512& _re, __m512& _rf)
			{
				__m512 _tmp0 = _mm512_unpacklo_ps(_r0, _r1);
				__m512 _tmp1 = _mm512_unpackhi_ps(_r0, _r1);
				__m512 _tmp2 = _mm512_unpacklo_ps(_r2, _r3);
				__m512 _tmp3 = _mm512_unpackhi_ps(_r2, _r3);
				__m512 _tmp4 = _mm512_unpacklo_ps(_r4, _r5);
				__m512 _tmp5 = _mm512_unpackhi_ps(_r4, _r5);
				__m512 _tmp6 = _mm512_unpacklo_ps(_r6, _r7);
				__m512 _tmp7 = _mm512_unpackhi_ps(_r6, _r7);
				__m512 _tmp8 = _mm512_unpacklo_ps(_r8, _r9);
				__m512 _tmp9 = _mm512_unpackhi_ps(_r8, _r9);
				__m512 _tmpa = _mm512_unpacklo_ps(_ra, _rb);
				__m512 _tmpb = _mm512_unpackhi_ps(_ra, _rb);
				__m512 _tmpc = _mm512_unpacklo_ps(_rc, _rd);
				__m512 _tmpd = _mm512_unpackhi_ps(_rc, _rd);
				__m512 _tmpe = _mm512_unpacklo_ps(_re, _rf);
				__m512 _tmpf = _mm512_unpackhi_ps(_re, _rf);

				__m512 _tmpg = _mm512_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmph = _mm512_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpi = _mm512_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpj = _mm512_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpk = _mm512_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpl = _mm512_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpm = _mm512_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpn = _mm512_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpo = _mm512_shuffle_ps(_tmp8, _tmpa, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpp = _mm512_shuffle_ps(_tmp8, _tmpa, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpq = _mm512_shuffle_ps(_tmp9, _tmpb, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpr = _mm512_shuffle_ps(_tmp9, _tmpb, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmps = _mm512_shuffle_ps(_tmpc, _tmpe, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpt = _mm512_shuffle_ps(_tmpc, _tmpe, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpu = _mm512_shuffle_ps(_tmpd, _tmpf, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpv = _mm512_shuffle_ps(_tmpd, _tmpf, _MM_SHUFFLE(3, 2, 3, 2));

				_tmp0 = _mm512_shuffle_f32x4(_tmpg, _tmpk, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp1 = _mm512_shuffle_f32x4(_tmpo, _tmps, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp2 = _mm512_shuffle_f32x4(_tmph, _tmpl, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp3 = _mm512_shuffle_f32x4(_tmpp, _tmpt, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp4 = _mm512_shuffle_f32x4(_tmpi, _tmpm, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp5 = _mm512_shuffle_f32x4(_tmpq, _tmpu, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp6 = _mm512_shuffle_f32x4(_tmpj, _tmpn, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp7 = _mm512_shuffle_f32x4(_tmpr, _tmpv, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp8 = _mm512_shuffle_f32x4(_tmpg, _tmpk, _MM_SHUFFLE(3, 1, 3, 1));
				_tmp9 = _mm512_shuffle_f32x4(_tmpo, _tmps, _MM_SHUFFLE(3, 1, 3, 1));
				_tmpa = _mm512_shuffle_f32x4(_tmph, _tmpl, _MM_SHUFFLE(3, 1, 3, 1));
				_tmpb = _mm512_shuffle_f32x4(_tmpp, _tmpt, _MM_SHUFFLE(3, 1, 3, 1));
				_tmpc = _mm512_shuffle_f32x4(_tmpi, _tmpm, _MM_SHUFFLE(3, 1, 3, 1));
				_tmpd = _mm512_shuffle_f32x4(_tmpq, _tmpu, _MM_SHUFFLE(3, 1, 3, 1));
				_tmpe = _mm512_shuffle_f32x4(_tmpj, _tmpn, _MM_SHUFFLE(3, 1, 3, 1));
				_tmpf = _mm512_shuffle_f32x4(_tmpr, _tmpv, _MM_SHUFFLE(3, 1, 3, 1));

				_r0 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(2, 0, 2, 0));
				_r1 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(2, 0, 2, 0));
				_r2 = _mm512_shuffle_f32x4(_tmp4, _tmp5, _MM_SHUFFLE(2, 0, 2, 0));
				_r3 = _mm512_shuffle_f32x4(_tmp6, _tmp7, _MM_SHUFFLE(2, 0, 2, 0));
				_r4 = _mm512_shuffle_f32x4(_tmp8, _tmp9, _MM_SHUFFLE(2, 0, 2, 0));
				_r5 = _mm512_shuffle_f32x4(_tmpa, _tmpb, _MM_SHUFFLE(2, 0, 2, 0));
				_r6 = _mm512_shuffle_f32x4(_tmpc, _tmpd, _MM_SHUFFLE(2, 0, 2, 0));
				_r7 = _mm512_shuffle_f32x4(_tmpe, _tmpf, _MM_SHUFFLE(2, 0, 2, 0));
				_r8 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(3, 1, 3, 1));
				_r9 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(3, 1, 3, 1));
				_ra = _mm512_shuffle_f32x4(_tmp4, _tmp5, _MM_SHUFFLE(3, 1, 3, 1));
				_rb = _mm512_shuffle_f32x4(_tmp6, _tmp7, _MM_SHUFFLE(3, 1, 3, 1));
				_rc = _mm512_shuffle_f32x4(_tmp8, _tmp9, _MM_SHUFFLE(3, 1, 3, 1));
				_rd = _mm512_shuffle_f32x4(_tmpa, _tmpb, _MM_SHUFFLE(3, 1, 3, 1));
				_re = _mm512_shuffle_f32x4(_tmpc, _tmpd, _MM_SHUFFLE(3, 1, 3, 1));
				_rf = _mm512_shuffle_f32x4(_tmpe, _tmpf, _MM_SHUFFLE(3, 1, 3, 1));
			}

			INLINE void transpose16x12_ps(__m512& _r0, __m512& _r1, __m512& _r2, __m512& _r3, __m512& _r4, __m512& _r5, __m512& _r6, __m512& _r7,
										  __m512& _r8, __m512& _r9, __m512& _ra, __m512& _rb)
			{
				__m512 _tmp0 = _mm512_unpacklo_ps(_r0, _r1);
				__m512 _tmp1 = _mm512_unpackhi_ps(_r0, _r1);
				__m512 _tmp2 = _mm512_unpacklo_ps(_r2, _r3);
				__m512 _tmp3 = _mm512_unpackhi_ps(_r2, _r3);
				__m512 _tmp4 = _mm512_unpacklo_ps(_r4, _r5);
				__m512 _tmp5 = _mm512_unpackhi_ps(_r4, _r5);
				__m512 _tmp6 = _mm512_unpacklo_ps(_r6, _r7);
				__m512 _tmp7 = _mm512_unpackhi_ps(_r6, _r7);
				__m512 _tmp8 = _mm512_unpacklo_ps(_r8, _r9);
				__m512 _tmp9 = _mm512_unpackhi_ps(_r8, _r9);
				__m512 _tmpa = _mm512_unpacklo_ps(_ra, _rb);
				__m512 _tmpb = _mm512_unpackhi_ps(_ra, _rb);

				__m512 _tmpc = _mm512_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpd = _mm512_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpe = _mm512_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpf = _mm512_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpg = _mm512_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmph = _mm512_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpi = _mm512_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpj = _mm512_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpk = _mm512_shuffle_ps(_tmp8, _tmpa, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpl = _mm512_shuffle_ps(_tmp8, _tmpa, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpm = _mm512_shuffle_ps(_tmp9, _tmpb, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpn = _mm512_shuffle_ps(_tmp9, _tmpb, _MM_SHUFFLE(3, 2, 3, 2));

				_tmp0 = _mm512_shuffle_f32x4(_tmpc, _tmpg, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp1 = _mm512_shuffle_f32x4(_tmpk, _tmpd, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp2 = _mm512_shuffle_f32x4(_tmph, _tmpl, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp3 = _mm512_shuffle_f32x4(_tmpe, _tmpi, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp4 = _mm512_shuffle_f32x4(_tmpm, _tmpf, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp5 = _mm512_shuffle_f32x4(_tmpj, _tmpn, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp6 = _mm512_shuffle_f32x4(_tmpc, _tmpg, _MM_SHUFFLE(3, 1, 3, 1));
				_tmp7 = _mm512_shuffle_f32x4(_tmpk, _tmpd, _MM_SHUFFLE(3, 1, 3, 1));
				_tmp8 = _mm512_shuffle_f32x4(_tmph, _tmpl, _MM_SHUFFLE(3, 1, 3, 1));
				_tmp9 = _mm512_shuffle_f32x4(_tmpe, _tmpi, _MM_SHUFFLE(3, 1, 3, 1));
				_tmpa = _mm512_shuffle_f32x4(_tmpm, _tmpf, _MM_SHUFFLE(3, 1, 3, 1));
				_tmpb = _mm512_shuffle_f32x4(_tmpj, _tmpn, _MM_SHUFFLE(3, 1, 3, 1));

				_r0 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(2, 0, 2, 0));
				_r1 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(2, 0, 2, 0));
				_r2 = _mm512_shuffle_f32x4(_tmp4, _tmp5, _MM_SHUFFLE(2, 0, 2, 0));
				_r3 = _mm512_shuffle_f32x4(_tmp6, _tmp7, _MM_SHUFFLE(2, 0, 2, 0));
				_r4 = _mm512_shuffle_f32x4(_tmp8, _tmp9, _MM_SHUFFLE(2, 0, 2, 0));
				_r5 = _mm512_shuffle_f32x4(_tmpa, _tmpb, _MM_SHUFFLE(2, 0, 2, 0));
				_r6 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(3, 1, 3, 1));
				_r7 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(3, 1, 3, 1));
				_r8 = _mm512_shuffle_f32x4(_tmp4, _tmp5, _MM_SHUFFLE(3, 1, 3, 1));
				_r9 = _mm512_shuffle_f32x4(_tmp6, _tmp7, _MM_SHUFFLE(3, 1, 3, 1));
				_ra = _mm512_shuffle_f32x4(_tmp8, _tmp9, _MM_SHUFFLE(3, 1, 3, 1));
				_rb = _mm512_shuffle_f32x4(_tmpa, _tmpb, _MM_SHUFFLE(3, 1, 3, 1));
			}

			INLINE void transpose16x8_ps(__m512& _r0, __m512& _r1, __m512& _r2, __m512& _r3, __m512& _r4, __m512& _r5, __m512& _r6, __m512& _r7)
			{
				__m512 _tmp0 = _mm512_unpacklo_ps(_r0, _r1);
				__m512 _tmp1 = _mm512_unpackhi_ps(_r0, _r1);
				__m512 _tmp2 = _mm512_unpacklo_ps(_r2, _r3);
				__m512 _tmp3 = _mm512_unpackhi_ps(_r2, _r3);
				__m512 _tmp4 = _mm512_unpacklo_ps(_r4, _r5);
				__m512 _tmp5 = _mm512_unpackhi_ps(_r4, _r5);
				__m512 _tmp6 = _mm512_unpacklo_ps(_r6, _r7);
				__m512 _tmp7 = _mm512_unpackhi_ps(_r6, _r7);

				__m512 _tmp8 = _mm512_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmp9 = _mm512_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpa = _mm512_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpb = _mm512_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpc = _mm512_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpd = _mm512_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmpe = _mm512_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmpf = _mm512_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(3, 2, 3, 2));

				_tmp0 = _mm512_shuffle_f32x4(_tmp8, _tmpc, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp1 = _mm512_shuffle_f32x4(_tmp9, _tmpd, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp2 = _mm512_shuffle_f32x4(_tmpa, _tmpe, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp3 = _mm512_shuffle_f32x4(_tmpb, _tmpf, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp4 = _mm512_shuffle_f32x4(_tmp8, _tmpc, _MM_SHUFFLE(3, 1, 3, 1));
				_tmp5 = _mm512_shuffle_f32x4(_tmp9, _tmpd, _MM_SHUFFLE(3, 1, 3, 1));
				_tmp6 = _mm512_shuffle_f32x4(_tmpa, _tmpe, _MM_SHUFFLE(3, 1, 3, 1));
				_tmp7 = _mm512_shuffle_f32x4(_tmpb, _tmpf, _MM_SHUFFLE(3, 1, 3, 1));

				_r0 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(2, 0, 2, 0));
				_r1 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(2, 0, 2, 0));
				_r2 = _mm512_shuffle_f32x4(_tmp4, _tmp5, _MM_SHUFFLE(2, 0, 2, 0));
				_r3 = _mm512_shuffle_f32x4(_tmp6, _tmp7, _MM_SHUFFLE(2, 0, 2, 0));
				_r4 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(3, 1, 3, 1));
				_r5 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(3, 1, 3, 1));
				_r6 = _mm512_shuffle_f32x4(_tmp4, _tmp5, _MM_SHUFFLE(3, 1, 3, 1));
				_r7 = _mm512_shuffle_f32x4(_tmp6, _tmp7, _MM_SHUFFLE(3, 1, 3, 1));
			}

			INLINE void transpose16x4_ps(__m512& _r0, __m512& _r1, __m512& _r2, __m512& _r3)
			{
				__m512 _tmp0 = _mm512_unpacklo_ps(_r0, _r1);
				__m512 _tmp1 = _mm512_unpackhi_ps(_r0, _r1);
				__m512 _tmp2 = _mm512_unpacklo_ps(_r2, _r3);
				__m512 _tmp3 = _mm512_unpackhi_ps(_r2, _r3);

				__m512 _tmp4 = _mm512_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmp5 = _mm512_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(3, 2, 3, 2));
				__m512 _tmp6 = _mm512_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(1, 0, 1, 0));
				__m512 _tmp7 = _mm512_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(3, 2, 3, 2));

				_tmp0 = _mm512_shuffle_f32x4(_tmp4, _tmp5, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp1 = _mm512_shuffle_f32x4(_tmp6, _tmp7, _MM_SHUFFLE(2, 0, 2, 0));
				_tmp2 = _mm512_shuffle_f32x4(_tmp4, _tmp5, _MM_SHUFFLE(3, 1, 3, 1));
				_tmp3 = _mm512_shuffle_f32x4(_tmp6, _tmp7, _MM_SHUFFLE(3, 1, 3, 1));

				_r0 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(2, 0, 2, 0));
				_r1 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(2, 0, 2, 0));
				_r2 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(3, 1, 3, 1));
				_r3 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(3, 1, 3, 1));
			}

			INLINE void transpose16x2_ps(__m512& _r0, __m512& _r1)
			{
				__m512 _tmp0 = _mm512_unpacklo_ps(_r0, _r1);
				__m512 _tmp1 = _mm512_unpackhi_ps(_r0, _r1);

				__m512 _tmp2 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(2, 0, 2, 0));
				__m512 _tmp3 = _mm512_shuffle_f32x4(_tmp0, _tmp1, _MM_SHUFFLE(3, 1, 3, 1));

				_r0 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(2, 0, 2, 0));
				_r1 = _mm512_shuffle_f32x4(_tmp2, _tmp3, _MM_SHUFFLE(3, 1, 3, 1));
			}

			INLINE void transpose8x16_ps(__m256& _r0, __m256& _r1, __m256& _r2, __m256& _r3, __m256& _r4, __m256& _r5, __m256& _r6, __m256& _r7,
										 __m256& _r8, __m256& _r9, __m256& _ra, __m256& _rb, __m256& _rc, __m256& _rd, __m256& _re, __m256& _rf)
			{
				__m256 _tmp0 = _mm256_unpacklo_ps(_r0, _r1);
				__m256 _tmp1 = _mm256_unpackhi_ps(_r0, _r1);
				__m256 _tmp2 = _mm256_unpacklo_ps(_r2, _r3);
				__m256 _tmp3 = _mm256_unpackhi_ps(_r2, _r3);
				__m256 _tmp4 = _mm256_unpacklo_ps(_r4, _r5);
				__m256 _tmp5 = _mm256_unpackhi_ps(_r4, _r5);
				__m256 _tmp6 = _mm256_unpacklo_ps(_r6, _r7);
				__m256 _tmp7 = _mm256_unpackhi_ps(_r6, _r7);
				__m256 _tmp8 = _mm256_unpacklo_ps(_r8, _r9);
				__m256 _tmp9 = _mm256_unpackhi_ps(_r8, _r9);
				__m256 _tmpa = _mm256_unpacklo_ps(_ra, _rb);
				__m256 _tmpb = _mm256_unpackhi_ps(_ra, _rb);
				__m256 _tmpc = _mm256_unpacklo_ps(_rc, _rd);
				__m256 _tmpd = _mm256_unpackhi_ps(_rc, _rd);
				__m256 _tmpe = _mm256_unpacklo_ps(_re, _rf);
				__m256 _tmpf = _mm256_unpackhi_ps(_re, _rf);

				__m256 _tmpg = _mm256_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(1, 0, 1, 0));
				__m256 _tmph = _mm256_shuffle_ps(_tmp0, _tmp2, _MM_SHUFFLE(3, 2, 3, 2));
				__m256 _tmpi = _mm256_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(1, 0, 1, 0));
				__m256 _tmpj = _mm256_shuffle_ps(_tmp1, _tmp3, _MM_SHUFFLE(3, 2, 3, 2));
				__m256 _tmpk = _mm256_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(1, 0, 1, 0));
				__m256 _tmpl = _mm256_shuffle_ps(_tmp4, _tmp6, _MM_SHUFFLE(3, 2, 3, 2));
				__m256 _tmpm = _mm256_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(1, 0, 1, 0));
				__m256 _tmpn = _mm256_shuffle_ps(_tmp5, _tmp7, _MM_SHUFFLE(3, 2, 3, 2));
				__m256 _tmpo = _mm256_shuffle_ps(_tmp8, _tmpa, _MM_SHUFFLE(1, 0, 1, 0));
				__m256 _tmpp = _mm256_shuffle_ps(_tmp8, _tmpa, _MM_SHUFFLE(3, 2, 3, 2));
				__m256 _tmpq = _mm256_shuffle_ps(_tmp9, _tmpb, _MM_SHUFFLE(1, 0, 1, 0));
				__m256 _tmpr = _mm256_shuffle_ps(_tmp9, _tmpb, _MM_SHUFFLE(3, 2, 3, 2));
				__m256 _tmps = _mm256_shuffle_ps(_tmpc, _tmpe, _MM_SHUFFLE(1, 0, 1, 0));
				__m256 _tmpt = _mm256_shuffle_ps(_tmpc, _tmpe, _MM_SHUFFLE(3, 2, 3, 2));
				__m256 _tmpu = _mm256_shuffle_ps(_tmpd, _tmpf, _MM_SHUFFLE(1, 0, 1, 0));
				__m256 _tmpv = _mm256_shuffle_ps(_tmpd, _tmpf, _MM_SHUFFLE(3, 2, 3, 2));

				_r0 = _mm256_permute2f128_ps(_tmpg, _tmpk, _MM_SHUFFLE(0, 2, 0, 0));
				_r1 = _mm256_permute2f128_ps(_tmpo, _tmps, _MM_SHUFFLE(0, 2, 0, 0));
				_r2 = _mm256_permute2f128_ps(_tmph, _tmpl, _MM_SHUFFLE(0, 2, 0, 0));
				_r3 = _mm256_permute2f128_ps(_tmpp, _tmpt, _MM_SHUFFLE(0, 2, 0, 0));
				_r4 = _mm256_permute2f128_ps(_tmpi, _tmpm, _MM_SHUFFLE(0, 2, 0, 0));
				_r5 = _mm256_permute2f128_ps(_tmpq, _tmpu, _MM_SHUFFLE(0, 2, 0, 0));
				_r6 = _mm256_permute2f128_ps(_tmpj, _tmpn, _MM_SHUFFLE(0, 2, 0, 0));
				_r7 = _mm256_permute2f128_ps(_tmpr, _tmpv, _MM_SHUFFLE(0, 2, 0, 0));
				_r8 = _mm256_permute2f128_ps(_tmpg, _tmpk, _MM_SHUFFLE(0, 3, 0, 1));
				_r9 = _mm256_permute2f128_ps(_tmpo, _tmps, _MM_SHUFFLE(0, 3, 0, 1));
				_ra = _mm256_permute2f128_ps(_tmph, _tmpl, _MM_SHUFFLE(0, 3, 0, 1));
				_rb = _mm256_permute2f128_ps(_tmpp, _tmpt, _MM_SHUFFLE(0, 3, 0, 1));
				_rc = _mm256_permute2f128_ps(_tmpi, _tmpm, _MM_SHUFFLE(0, 3, 0, 1));
				_rd = _mm256_permute2f128_ps(_tmpq, _tmpu, _MM_SHUFFLE(0, 3, 0, 1));
				_re = _mm256_permute2f128_ps(_tmpj, _tmpn, _MM_SHUFFLE(0, 3, 0, 1));
				_rf = _mm256_permute2f128_ps(_tmpr, _tmpv, _MM_SHUFFLE(0, 3, 0, 1));
			}

			INLINE void transpose16x16_epi16(__m256i& _r0, __m256i& _r1, __m256i& _r2, __m256i& _r3, __m256i& _r4, __m256i& _r5, __m256i& _r6, __m256i& _r7,
											 __m256i& _r8, __m256i& _r9, __m256i& _ra, __m256i& _rb, __m256i& _rc, __m256i& _rd, __m256i& _re, __m256i& _rf)
			{
				__m256i _tmp0 = _mm256_unpacklo_epi16(_r0, _r1);
				__m256i _tmp1 = _mm256_unpackhi_epi16(_r0, _r1);
				__m256i _tmp2 = _mm256_unpacklo_epi16(_r2, _r3);
				__m256i _tmp3 = _mm256_unpackhi_epi16(_r2, _r3);
				__m256i _tmp4 = _mm256_unpacklo_epi16(_r4, _r5);
				__m256i _tmp5 = _mm256_unpackhi_epi16(_r4, _r5);
				__m256i _tmp6 = _mm256_unpacklo_epi16(_r6, _r7);
				__m256i _tmp7 = _mm256_unpackhi_epi16(_r6, _r7);
				__m256i _tmp8 = _mm256_unpacklo_epi16(_r8, _r9);
				__m256i _tmp9 = _mm256_unpackhi_epi16(_r8, _r9);
				__m256i _tmpa = _mm256_unpacklo_epi16(_ra, _rb);
				__m256i _tmpb = _mm256_unpackhi_epi16(_ra, _rb);
				__m256i _tmpc = _mm256_unpacklo_epi16(_rc, _rd);
				__m256i _tmpd = _mm256_unpackhi_epi16(_rc, _rd);
				__m256i _tmpe = _mm256_unpacklo_epi16(_re, _rf);
				__m256i _tmpf = _mm256_unpackhi_epi16(_re, _rf);

				__m256i _tmpg = _mm256_unpacklo_epi32(_tmp0, _tmp2);
				__m256i _tmph = _mm256_unpackhi_epi32(_tmp0, _tmp2);
				__m256i _tmpi = _mm256_unpacklo_epi32(_tmp1, _tmp3);
				__m256i _tmpj = _mm256_unpackhi_epi32(_tmp1, _tmp3);
				__m256i _tmpk = _mm256_unpacklo_epi32(_tmp4, _tmp6);
				__m256i _tmpl = _mm256_unpackhi_epi32(_tmp4, _tmp6);
				__m256i _tmpm = _mm256_unpacklo_epi32(_tmp5, _tmp7);
				__m256i _tmpn = _mm256_unpackhi_epi32(_tmp5, _tmp7);
				__m256i _tmpo = _mm256_unpacklo_epi32(_tmp8, _tmpa);
				__m256i _tmpp = _mm256_unpackhi_epi32(_tmp8, _tmpa);
				__m256i _tmpq = _mm256_unpacklo_epi32(_tmp9, _tmpb);
				__m256i _tmpr = _mm256_unpackhi_epi32(_tmp9, _tmpb);
				__m256i _tmps = _mm256_unpacklo_epi32(_tmpc, _tmpe);
				__m256i _tmpt = _mm256_unpackhi_epi32(_tmpc, _tmpe);
				__m256i _tmpu = _mm256_unpacklo_epi32(_tmpd, _tmpf);
				__m256i _tmpv = _mm256_unpackhi_epi32(_tmpd, _tmpf);

				_tmp0 = _mm256_unpacklo_epi64(_tmpg, _tmpk);
				_tmp1 = _mm256_unpackhi_epi64(_tmpg, _tmpk);
				_tmp2 = _mm256_unpacklo_epi64(_tmph, _tmpl);
				_tmp3 = _mm256_unpackhi_epi64(_tmph, _tmpl);
				_tmp4 = _mm256_unpacklo_epi64(_tmpi, _tmpm);
				_tmp5 = _mm256_unpackhi_epi64(_tmpi, _tmpm);
				_tmp6 = _mm256_unpacklo_epi64(_tmpj, _tmpn);
				_tmp7 = _mm256_unpackhi_epi64(_tmpj, _tmpn);
				_tmp8 = _mm256_unpacklo_epi64(_tmpo, _tmps);
				_tmp9 = _mm256_unpackhi_epi64(_tmpo, _tmps);
				_tmpa = _mm256_unpacklo_epi64(_tmpp, _tmpt);
				_tmpb = _mm256_unpackhi_epi64(_tmpp, _tmpt);
				_tmpc = _mm256_unpacklo_epi64(_tmpq, _tmpu);
				_tmpd = _mm256_unpackhi_epi64(_tmpq, _tmpu);
				_tmpe = _mm256_unpacklo_epi64(_tmpr, _tmpv);
				_tmpf = _mm256_unpackhi_epi64(_tmpr, _tmpv);

				_r0 = _mm256_permute2x128_si256(_tmp0, _tmp8, _MM_SHUFFLE(0, 2, 0, 0));
				_r1 = _mm256_permute2x128_si256(_tmp1, _tmp9, _MM_SHUFFLE(0, 2, 0, 0));
				_r2 = _mm256_permute2x128_si256(_tmp2, _tmpa, _MM_SHUFFLE(0, 2, 0, 0));
				_r3 = _mm256_permute2x128_si256(_tmp3, _tmpb, _MM_SHUFFLE(0, 2, 0, 0));
				_r4 = _mm256_permute2x128_si256(_tmp4, _tmpc, _MM_SHUFFLE(0, 2, 0, 0));
				_r5 = _mm256_permute2x128_si256(_tmp5, _tmpd, _MM_SHUFFLE(0, 2, 0, 0));
				_r6 = _mm256_permute2x128_si256(_tmp6, _tmpe, _MM_SHUFFLE(0, 2, 0, 0));
				_r7 = _mm256_permute2x128_si256(_tmp7, _tmpf, _MM_SHUFFLE(0, 2, 0, 0));
				_r8 = _mm256_permute2x128_si256(_tmp0, _tmp8, _MM_SHUFFLE(0, 3, 0, 1));
				_r9 = _mm256_permute2x128_si256(_tmp1, _tmp9, _MM_SHUFFLE(0, 3, 0, 1));
				_ra = _mm256_permute2x128_si256(_tmp2, _tmpa, _MM_SHUFFLE(0, 3, 0, 1));
				_rb = _mm256_permute2x128_si256(_tmp3, _tmpb, _MM_SHUFFLE(0, 3, 0, 1));
				_rc = _mm256_permute2x128_si256(_tmp4, _tmpc, _MM_SHUFFLE(0, 3, 0, 1));
				_rd = _mm256_permute2x128_si256(_tmp5, _tmpd, _MM_SHUFFLE(0, 3, 0, 1));
				_re = _mm256_permute2x128_si256(_tmp6, _tmpe, _MM_SHUFFLE(0, 3, 0, 1));
				_rf = _mm256_permute2x128_si256(_tmp7, _tmpf, _MM_SHUFFLE(0, 3, 0, 1));
			}

			INLINE void transpose16x8_epi16(__m256i& _r0, __m256i& _r1, __m256i& _r2, __m256i& _r3, __m256i& _r4, __m256i& _r5, __m256i& _r6, __m256i& _r7)
			{
				__m256i _tmp0 = _mm256_unpacklo_epi16(_r0, _r1);
				__m256i _tmp1 = _mm256_unpackhi_epi16(_r0, _r1);
				__m256i _tmp2 = _mm256_unpacklo_epi16(_r2, _r3);
				__m256i _tmp3 = _mm256_unpackhi_epi16(_r2, _r3);
				__m256i _tmp4 = _mm256_unpacklo_epi16(_r4, _r5);
				__m256i _tmp5 = _mm256_unpackhi_epi16(_r4, _r5);
				__m256i _tmp6 = _mm256_unpacklo_epi16(_r6, _r7);
				__m256i _tmp7 = _mm256_unpackhi_epi16(_r6, _r7);

				__m256i _tmpg = _mm256_unpacklo_epi32(_tmp0, _tmp2);
				__m256i _tmph = _mm256_unpackhi_epi32(_tmp0, _tmp2);
				__m256i _tmpi = _mm256_unpacklo_epi32(_tmp1, _tmp3);
				__m256i _tmpj = _mm256_unpackhi_epi32(_tmp1, _tmp3);
				__m256i _tmpk = _mm256_unpacklo_epi32(_tmp4, _tmp6);
				__m256i _tmpl = _mm256_unpackhi_epi32(_tmp4, _tmp6);
				__m256i _tmpm = _mm256_unpacklo_epi32(_tmp5, _tmp7);
				__m256i _tmpn = _mm256_unpackhi_epi32(_tmp5, _tmp7);

				_tmp0 = _mm256_unpacklo_epi64(_tmpg, _tmpk);
				_tmp1 = _mm256_unpackhi_epi64(_tmpg, _tmpk);
				_tmp2 = _mm256_unpacklo_epi64(_tmph, _tmpl);
				_tmp3 = _mm256_unpackhi_epi64(_tmph, _tmpl);
				_tmp4 = _mm256_unpacklo_epi64(_tmpi, _tmpm);
				_tmp5 = _mm256_unpackhi_epi64(_tmpi, _tmpm);
				_tmp6 = _mm256_unpacklo_epi64(_tmpj, _tmpn);
				_tmp7 = _mm256_unpackhi_epi64(_tmpj, _tmpn);

				_r0 = _mm256_permute2x128_si256(_tmp0, _tmp1, _MM_SHUFFLE(0, 2, 0, 0));
				_r1 = _mm256_permute2x128_si256(_tmp2, _tmp3, _MM_SHUFFLE(0, 2, 0, 0));
				_r2 = _mm256_permute2x128_si256(_tmp4, _tmp5, _MM_SHUFFLE(0, 2, 0, 0));
				_r3 = _mm256_permute2x128_si256(_tmp6, _tmp7, _MM_SHUFFLE(0, 2, 0, 0));
				_r4 = _mm256_permute2x128_si256(_tmp0, _tmp1, _MM_SHUFFLE(0, 3, 0, 1));
				_r5 = _mm256_permute2x128_si256(_tmp2, _tmp3, _MM_SHUFFLE(0, 3, 0, 1));
				_r6 = _mm256_permute2x128_si256(_tmp4, _tmp5, _MM_SHUFFLE(0, 3, 0, 1));
				_r7 = _mm256_permute2x128_si256(_tmp6, _tmp7, _MM_SHUFFLE(0, 3, 0, 1));
			}

			INLINE f32 _mm512_comp_reduce_add_ps(__m512 x)
			{
				const __m256 x256 = _mm256_add_ps(_mm512_castps512_ps256(x), _mm512_extractf32x8_ps(x, 1));
				const __m128 x128 = _mm_add_ps(_mm256_castps256_ps128(x256), _mm256_extractf128_ps(x256, 1));
				const __m128 x64 = _mm_add_ps(x128, _mm_movehl_ps(x128, x128));
				const __m128 x32 = _mm_add_ss(x64, _mm_shuffle_ps(x64, x64, 0x55));
				return _mm_cvtss_f32(x32);
			}

			INLINE f32 _mm512_comp_reduce_max_ps(__m512 x)
			{
				const __m256 x256 = _mm256_max_ps(_mm512_castps512_ps256(x), _mm512_extractf32x8_ps(x, 1));
				const __m128 x128 = _mm_max_ps(_mm256_castps256_ps128(x256), _mm256_extractf128_ps(x256, 1));
				const __m128 x64 = _mm_max_ps(x128, _mm_movehl_ps(x128, x128));
				const __m128 x32 = _mm_max_ss(x64, _mm_shuffle_ps(x64, x64, 0x55));
				return _mm_cvtss_f32(x32);
			}
		#endif // AVX512F
	#endif // AVX2
#endif // SSE2

/* SIMD (SSE1+MMX or SSE2) implementation of sin, cos, exp and log
   Inspired by Intel Approximate Math library, and based on the
   corresponding algorithms of the cephes math library
   The default is to use the SSE1 version. If you define SSE2 the
   the SSE2 intrinsics will be used in place of the MMX intrinsics. Do
   not expect any significant performance improvement with SSE2.
*/

/* Copyright (C) 2007  Julien Pommier
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgment in the product documentation would be
	 appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
  (this is the zlib license)
*/

#include <xmmintrin.h>

/* yes I know, the top of this file is quite ugly */

#ifdef _MSC_VER /* visual c++ */
	#define ALIGN16_BEG __declspec(align(16))
	#define ALIGN16_END
#else /* gcc or icc */
	#define ALIGN16_BEG
	#define ALIGN16_END __attribute__((aligned(16)))
#endif

/* declare some SSE constants -- why can't I figure a better way to do that? */
#define _PS_CONST(Name, Val) static constexpr ALIGN16_BEG f32 _ps_##Name[4] ALIGN16_END = {Val, Val, Val, Val}
#define _PI32_CONST(Name, Val) static constexpr ALIGN16_BEG int _pi32_##Name[4] ALIGN16_END = {Val, Val, Val, Val}
#define _PS_CONST_TYPE(Name, Type, Val) static constexpr ALIGN16_BEG Type _ps_##Name[4] ALIGN16_END = {Val, Val, Val, Val}

_PS_CONST(1, 1.0f);
_PS_CONST(0p5, 0.5f);
/* the smallest non denormalized f32 number */
_PS_CONST_TYPE(min_norm_pos, int, 0x00800000);
_PS_CONST_TYPE(mant_mask, int, 0x7f800000);
_PS_CONST_TYPE(inv_mant_mask, int, ~0x7f800000);

_PS_CONST_TYPE(sign_mask, int, (int)0x80000000);
_PS_CONST_TYPE(inv_sign_mask, int, ~0x80000000);

_PI32_CONST(1, 1);
_PI32_CONST(inv1, ~1);
_PI32_CONST(2, 2);
_PI32_CONST(4, 4);
_PI32_CONST(0x7f, 0x7f);

_PS_CONST(cephes_SQRTHF, 0.707106781186547524f);
_PS_CONST(cephes_log_p0, 7.0376836292E-2f);
_PS_CONST(cephes_log_p1, -1.1514610310E-1f);
_PS_CONST(cephes_log_p2, 1.1676998740E-1f);
_PS_CONST(cephes_log_p3, -1.2420140846E-1f);
_PS_CONST(cephes_log_p4, +1.4249322787E-1f);
_PS_CONST(cephes_log_p5, -1.6668057665E-1f);
_PS_CONST(cephes_log_p6, +2.0000714765E-1f);
_PS_CONST(cephes_log_p7, -2.4999993993E-1f);
_PS_CONST(cephes_log_p8, +3.3333331174E-1f);
_PS_CONST(cephes_log_q1, -2.12194440e-4f);
_PS_CONST(cephes_log_q2, 0.693359375f);

#if !SSE2
	typedef union xmm_mm_union
	{
		__m128 xmm;
		__m64 mm[2];
	} xmm_mm_union;

	#define COPY_XMM_TO_MM(xmm_, mm0_, mm1_) \
		{                                    \
			xmm_mm_union u;                  \
			u.xmm = xmm_;                    \
			mm0_ = u.mm[0];                  \
			mm1_ = u.mm[1];                  \
		}

	#define COPY_MM_TO_XMM(mm0_, mm1_, xmm_) \
		{                                    \
			xmm_mm_union u;                  \
			u.mm[0] = mm0_;                  \
			u.mm[1] = mm1_;                  \
			xmm_ = u.xmm;                    \
		}
#endif

/* natural logarithm computed for 4 simultaneous f32
   return NaN for x <= 0
*/
INLINE __m128 log_ps(__m128 x)
{
#if SSE2
	__m128i emm0;
#else
	__m64 mm0, mm1;
#endif
	__m128 one = *(__m128*)_ps_1;

	__m128 invalid_mask = _mm_cmple_ps(x, _mm_setzero_ps());

	x = _mm_max_ps(x, *(__m128*)_ps_min_norm_pos); /* cut off denormalized stuff */

#if !SSE2
	/* part 1: x = frexpf(x, &e); */
	COPY_XMM_TO_MM(x, mm0, mm1);
	mm0 = _mm_srli_pi32(mm0, 23);
	mm1 = _mm_srli_pi32(mm1, 23);
#else
	emm0 = _mm_srli_epi32(_mm_castps_si128(x), 23);
#endif
	/* keep only the fractional part */
	x = _mm_and_ps(x, *(__m128*)_ps_inv_mant_mask);
	x = _mm_or_ps(x, *(__m128*)_ps_0p5);

#if !SSE2
	/* now e=mm0:mm1 contain the really base-2 exponent */
	mm0 = _mm_sub_pi32(mm0, *(__m64*)_pi32_0x7f);
	mm1 = _mm_sub_pi32(mm1, *(__m64*)_pi32_0x7f);
	__m128 e = _mm_cvtpi32x2_ps(mm0, mm1);
	_mm_empty(); /* bye bye mmx */
#else
	emm0 = _mm_sub_epi32(emm0, *(__m128i*)_pi32_0x7f);
	__m128 e = _mm_cvtepi32_ps(emm0);
#endif

	e = _mm_add_ps(e, one);

	/* part2:
	   if( x < SQRTHF ) {
		 e -= 1;
		 x = x + x - 1.0;
	   } else { x = x - 1.0; }
	*/
	__m128 mask = _mm_cmplt_ps(x, *(__m128*)_ps_cephes_SQRTHF);
	__m128 tmp = _mm_and_ps(x, mask);
	x = _mm_sub_ps(x, one);
	e = _mm_sub_ps(e, _mm_and_ps(one, mask));
	x = _mm_add_ps(x, tmp);

	__m128 z = _mm_mul_ps(x, x);

	__m128 y = *(__m128*)_ps_cephes_log_p0;
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_log_p1);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_log_p2);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_log_p3);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_log_p4);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_log_p5);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_log_p6);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_log_p7);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_log_p8);
	y = _mm_mul_ps(y, x);

	y = _mm_mul_ps(y, z);

	y = _mm_comp_fmadd_ps(e, *(__m128*)_ps_cephes_log_q1, y);

	//y = -z * 0.5 + y
	y = _mm_comp_fnmadd_ps(z, *(__m128*)_ps_0p5, y);

	x = _mm_add_ps(x, y);
	x = _mm_comp_fmadd_ps(e, *(__m128*)_ps_cephes_log_q2, x);
	x = _mm_or_ps(x, invalid_mask); // negative arg will be NAN
	return x;
}

_PS_CONST(exp_hi, 88.3762626647949f);
_PS_CONST(exp_lo, -88.3762626647949f);

_PS_CONST(cephes_LOG2EF, 1.44269504088896341f);
_PS_CONST(cephes_exp_C1, 0.693359375f);
_PS_CONST(cephes_exp_C2, -2.12194440e-4f);

_PS_CONST(cephes_exp_p0, 1.9875691500E-4f);
_PS_CONST(cephes_exp_p1, 1.3981999507E-3f);
_PS_CONST(cephes_exp_p2, 8.3334519073E-3f);
_PS_CONST(cephes_exp_p3, 4.1665795894E-2f);
_PS_CONST(cephes_exp_p4, 1.6666665459E-1f);
_PS_CONST(cephes_exp_p5, 5.0000001201E-1f);

INLINE __m128 exp_ps(__m128 x)
{
	__m128 tmp = _mm_setzero_ps(), fx;
#if SSE2
	__m128i emm0;
#else
	__m64 mm0, mm1;
#endif
	__m128 one = *(__m128*)_ps_1;

	x = _mm_min_ps(x, *(__m128*)_ps_exp_hi);
	x = _mm_max_ps(x, *(__m128*)_ps_exp_lo);

	/* express exp(x) as exp(g + n*log(2)) */
	fx = _mm_mul_ps(x, *(__m128*)_ps_cephes_LOG2EF);
	fx = _mm_add_ps(fx, *(__m128*)_ps_0p5);

	/* how to perform a floorf with SSE: just below */
#if !SSE2
	/* step 1 : cast to int */
	tmp = _mm_movehl_ps(tmp, fx);
	mm0 = _mm_cvttps_pi32(fx);
	mm1 = _mm_cvttps_pi32(tmp);
	/* step 2 : cast back to f32 */
	tmp = _mm_cvtpi32x2_ps(mm0, mm1);
#else
	emm0 = _mm_cvttps_epi32(fx);
	tmp = _mm_cvtepi32_ps(emm0);
#endif
	/* if greater, substract 1 */
	__m128 mask = _mm_cmpgt_ps(tmp, fx);
	mask = _mm_and_ps(mask, one);
	fx = _mm_sub_ps(tmp, mask);

	// x = x - fx * exp_C1
	x = _mm_comp_fnmadd_ps(fx, *(__m128*)_ps_cephes_exp_C1, x);
	// x = x - fx * exp_C2
	x = _mm_comp_fnmadd_ps(fx, *(__m128*)_ps_cephes_exp_C2, x);

	tmp = _mm_mul_ps(x, x);

	__m128 y = *(__m128*)_ps_cephes_exp_p0;
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_exp_p1);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_exp_p2);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_exp_p3);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_exp_p4);
	y = _mm_comp_fmadd_ps(y, x, *(__m128*)_ps_cephes_exp_p5);
	y = _mm_comp_fmadd_ps(y, tmp, x);

	y = _mm_add_ps(y, one);

	/* build 2^n */
#if !SSE2
	z = _mm_movehl_ps(z, fx);
	mm0 = _mm_cvttps_pi32(fx);
	mm1 = _mm_cvttps_pi32(z);
	mm0 = _mm_add_pi32(mm0, *(__m64*)_pi32_0x7f);
	mm1 = _mm_add_pi32(mm1, *(__m64*)_pi32_0x7f);
	mm0 = _mm_slli_pi32(mm0, 23);
	mm1 = _mm_slli_pi32(mm1, 23);

	__m128 pow2n;
	COPY_MM_TO_XMM(mm0, mm1, pow2n);
	_mm_empty();
#else
	emm0 = _mm_cvttps_epi32(fx);
	emm0 = _mm_add_epi32(emm0, *(__m128i*)_pi32_0x7f);
	emm0 = _mm_slli_epi32(emm0, 23);
	__m128 pow2n = _mm_castsi128_ps(emm0);
#endif
	y = _mm_mul_ps(y, pow2n);
	return y;
}

_PS_CONST(tanh_hi, 9.0f);
_PS_CONST(tanh_lo, -9.0f);

_PS_CONST(cephes_tanh_p0, -2.76076847742355E-16f);
_PS_CONST(cephes_tanh_p1, 2.00018790482477E-13f);
_PS_CONST(cephes_tanh_p2, -8.60467152213735E-11f);
_PS_CONST(cephes_tanh_p3, 5.12229709037114E-08f);
_PS_CONST(cephes_tanh_p4, 1.48572235717979E-05f);
_PS_CONST(cephes_tanh_p5, 6.37261928875436E-04f);
_PS_CONST(cephes_tanh_p6, 4.89352455891786E-03f);
_PS_CONST(cephes_tanh_p7, 1.19825839466702e-06f);
_PS_CONST(cephes_tanh_p8, 1.18534705686654e-04f);
_PS_CONST(cephes_tanh_p9, 2.26843463243900e-03f);

// an approximation of tanh
inline __m128 tanh_ps(const __m128 x)
{
	__m128 value = x;
	value = _mm_max_ps(*(__m128*)_ps_tanh_lo, value);
	value = _mm_min_ps(*(__m128*)_ps_tanh_hi, value);

	__m128 value_squared = _mm_mul_ps(value, value);

	__m128 p;
	p = _mm_comp_fmadd_ps(value_squared, *(__m128*)_ps_cephes_tanh_p0, *(__m128*)_ps_cephes_tanh_p1);
	p = _mm_comp_fmadd_ps(p, value_squared, *(__m128*)_ps_cephes_tanh_p2);
	p = _mm_comp_fmadd_ps(p, value_squared, *(__m128*)_ps_cephes_tanh_p3);
	p = _mm_comp_fmadd_ps(p, value_squared, *(__m128*)_ps_cephes_tanh_p4);
	p = _mm_comp_fmadd_ps(p, value_squared, *(__m128*)_ps_cephes_tanh_p5);
	p = _mm_comp_fmadd_ps(p, value_squared, *(__m128*)_ps_cephes_tanh_p6);
	p = _mm_mul_ps(p, value);

	__m128 q;
	q = _mm_comp_fmadd_ps(value_squared, *(__m128*)_ps_cephes_tanh_p7, *(__m128*)_ps_cephes_tanh_p8);
	q = _mm_comp_fmadd_ps(q, value_squared, *(__m128*)_ps_cephes_tanh_p9);
	q = _mm_comp_fmadd_ps(q, value_squared, *(__m128*)_ps_cephes_tanh_p6);

	__m128 dst = _mm_div_ps(p, q);
	return dst;
}

_PS_CONST(minus_cephes_DP1, -0.78515625f);
_PS_CONST(minus_cephes_DP2, -2.4187564849853515625e-4f);
_PS_CONST(minus_cephes_DP3, -3.77489497744594108e-8f);
_PS_CONST(sincof_p0, -1.9515295891E-4f);
_PS_CONST(sincof_p1, 8.3321608736E-3f);
_PS_CONST(sincof_p2, -1.6666654611E-1f);
_PS_CONST(coscof_p0, 2.443315711809948E-005f);
_PS_CONST(coscof_p1, -1.388731625493765E-003f);
_PS_CONST(coscof_p2, 4.166664568298827E-002f);
_PS_CONST(cephes_FOPI, 1.27323954473516f); // 4 / M_PI

/* evaluation of 4 sines at onces, using only SSE1+MMX intrinsics so
   it runs also on old athlons XPs and the pentium III of your grand
   mother.
   The code is the exact rewriting of the cephes sinf function.
   Precision is excellent as long as x < 8192 (I did not bother to
   take into account the special handling they have for greater values
   -- it does not return garbage for arguments over 8192, though, but
   the extra precision is missing).
   Note that it is such that sinf((f32)M_PI) = 8.74e-8, which is the
   surprising but correct result.
   Performance is also surprisingly good, 1.33 times faster than the
   macos vsinf SSE2 function, and 1.5 times faster than the
   __vrs4_sinf of amd's ACML (which is only available in 64 bits). Not
   too bad for an SSE1 function (with no special tuning) !
   However the latter libraries probably have a much better handling of NaN,
   Inf, denormalized and other special arguments..
   On my core 1 duo, the execution of this function takes approximately 95 cycles.
   From what I have observed on the experiments with Intel AMath lib, switching to an
   SSE2 version would improve the perf by only 10%.
   Since it is based on SSE intrinsics, it has to be compiled at -O2 to
   deliver full speed.
*/
INLINE __m128 sin_ps(__m128 x)
{   // any x
	__m128 xmm1, xmm2 = _mm_setzero_ps(), xmm3, sign_bit, y;

#if SSE2
	__m128i emm0, emm2;
#else
	__m64 mm0, mm1, mm2, mm3;
#endif
	sign_bit = x;
	/* take the absolute value */
	x = _mm_and_ps(x, *(__m128*)_ps_inv_sign_mask);
	/* extract the sign bit (upper one) */
	sign_bit = _mm_and_ps(sign_bit, *(__m128*)_ps_sign_mask);

	/* scale by 4/Pi */
	y = _mm_mul_ps(x, *(__m128*)_ps_cephes_FOPI);

#if SSE2
	/* store the integer part of y in mm0 */
	emm2 = _mm_cvttps_epi32(y);
	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = _mm_add_epi32(emm2, *(__m128i*)_pi32_1);
	emm2 = _mm_and_si128(emm2, *(__m128i*)_pi32_inv1);
	y = _mm_cvtepi32_ps(emm2);

	/* get the swap sign flag */
	emm0 = _mm_and_si128(emm2, *(__m128i*)_pi32_4);
	emm0 = _mm_slli_epi32(emm0, 29);
	/* get the polynom selection mask
	   there is one polynom for 0 <= x <= Pi/4
	   and another one for Pi/4<x<=Pi/2
	   Both branches will be computed.
	*/
	emm2 = _mm_and_si128(emm2, *(__m128i*)_pi32_2);
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

	__m128 swap_sign_bit = _mm_castsi128_ps(emm0);
	__m128 poly_mask = _mm_castsi128_ps(emm2);
	sign_bit = _mm_xor_ps(sign_bit, swap_sign_bit);

#else
	/* store the integer part of y in mm0:mm1 */
	xmm2 = _mm_movehl_ps(xmm2, y);
	mm2 = _mm_cvttps_pi32(y);
	mm3 = _mm_cvttps_pi32(xmm2);
	/* j=(j+1) & (~1) (see the cephes sources) */
	mm2 = _mm_add_pi32(mm2, *(__m64*)_pi32_1);
	mm3 = _mm_add_pi32(mm3, *(__m64*)_pi32_1);
	mm2 = _mm_and_si64(mm2, *(__m64*)_pi32_inv1);
	mm3 = _mm_and_si64(mm3, *(__m64*)_pi32_inv1);
	y = _mm_cvtpi32x2_ps(mm2, mm3);
	/* get the swap sign flag */
	mm0 = _mm_and_si64(mm2, *(__m64*)_pi32_4);
	mm1 = _mm_and_si64(mm3, *(__m64*)_pi32_4);
	mm0 = _mm_slli_pi32(mm0, 29);
	mm1 = _mm_slli_pi32(mm1, 29);
	/* get the polynom selection mask */
	mm2 = _mm_and_si64(mm2, *(__m64*)_pi32_2);
	mm3 = _mm_and_si64(mm3, *(__m64*)_pi32_2);
	mm2 = _mm_cmpeq_pi32(mm2, _mm_setzero_si64());
	mm3 = _mm_cmpeq_pi32(mm3, _mm_setzero_si64());
	__m128 swap_sign_bit, poly_mask;
	COPY_MM_TO_XMM(mm0, mm1, swap_sign_bit);
	COPY_MM_TO_XMM(mm2, mm3, poly_mask);
	sign_bit = _mm_xor_ps(sign_bit, swap_sign_bit);
	_mm_empty(); /* good-bye mmx */
#endif

	/* The magic pass: "Extended precision modular arithmetic"
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(__m128*)_ps_minus_cephes_DP1;
	xmm2 = *(__m128*)_ps_minus_cephes_DP2;
	xmm3 = *(__m128*)_ps_minus_cephes_DP3;
	x = _mm_comp_fmadd_ps(y, xmm1, x);
	x = _mm_comp_fmadd_ps(y, xmm2, x);
	x = _mm_comp_fmadd_ps(y, xmm3, x);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = *(__m128*)_ps_coscof_p0;
	__m128 z = _mm_mul_ps(x, x);

	y = _mm_comp_fmadd_ps(y, z, *(__m128*)_ps_coscof_p1);
	y = _mm_comp_fmadd_ps(y, z, *(__m128*)_ps_coscof_p2);
	y = _mm_mul_ps(y, z);
	y = _mm_mul_ps(y, z);
	y = _mm_comp_fnmadd_ps(z, *(__m128*)_ps_0p5, y);
	y = _mm_add_ps(y, *(__m128*)_ps_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	__m128 y2 = *(__m128*)_ps_sincof_p0;
	y2 = _mm_comp_fmadd_ps(y2, z, *(__m128*)_ps_sincof_p1);
	y2 = _mm_comp_fmadd_ps(y2, z, *(__m128*)_ps_sincof_p2);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_comp_fmadd_ps(y2, x, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	y2 = _mm_and_ps(xmm3, y2); //, xmm3);
	y = _mm_andnot_ps(xmm3, y);
	y = _mm_add_ps(y, y2);
	/* update the sign */
	y = _mm_xor_ps(y, sign_bit);
	return y;
}

/* almost the same as sin_ps */
INLINE __m128 cos_ps(__m128 x)
{   // any x
	__m128 xmm1, xmm2 = _mm_setzero_ps(), xmm3, y;
#if SSE2
	__m128i emm0, emm2;
#else
	__m64 mm0, mm1, mm2, mm3;
#endif
	/* take the absolute value */
	x = _mm_and_ps(x, *(__m128*)_ps_inv_sign_mask);

	/* scale by 4/Pi */
	y = _mm_mul_ps(x, *(__m128*)_ps_cephes_FOPI);

#if SSE2
	/* store the integer part of y in mm0 */
	emm2 = _mm_cvttps_epi32(y);
	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = _mm_add_epi32(emm2, *(__m128i*)_pi32_1);
	emm2 = _mm_and_si128(emm2, *(__m128i*)_pi32_inv1);
	y = _mm_cvtepi32_ps(emm2);

	emm2 = _mm_sub_epi32(emm2, *(__m128i*)_pi32_2);

	/* get the swap sign flag */
	emm0 = _mm_andnot_si128(emm2, *(__m128i*)_pi32_4);
	emm0 = _mm_slli_epi32(emm0, 29);
	/* get the polynom selection mask */
	emm2 = _mm_and_si128(emm2, *(__m128i*)_pi32_2);
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

	__m128 sign_bit = _mm_castsi128_ps(emm0);
	__m128 poly_mask = _mm_castsi128_ps(emm2);
#else
	/* store the integer part of y in mm0:mm1 */
	xmm2 = _mm_movehl_ps(xmm2, y);
	mm2 = _mm_cvttps_pi32(y);
	mm3 = _mm_cvttps_pi32(xmm2);

	/* j=(j+1) & (~1) (see the cephes sources) */
	mm2 = _mm_add_pi32(mm2, *(__m64*)_pi32_1);
	mm3 = _mm_add_pi32(mm3, *(__m64*)_pi32_1);
	mm2 = _mm_and_si64(mm2, *(__m64*)_pi32_inv1);
	mm3 = _mm_and_si64(mm3, *(__m64*)_pi32_inv1);

	y = _mm_cvtpi32x2_ps(mm2, mm3);

	mm2 = _mm_sub_pi32(mm2, *(__m64*)_pi32_2);
	mm3 = _mm_sub_pi32(mm3, *(__m64*)_pi32_2);

	/* get the swap sign flag in mm0:mm1 and the
	   polynom selection mask in mm2:mm3 */

	mm0 = _mm_andnot_si64(mm2, *(__m64*)_pi32_4);
	mm1 = _mm_andnot_si64(mm3, *(__m64*)_pi32_4);
	mm0 = _mm_slli_pi32(mm0, 29);
	mm1 = _mm_slli_pi32(mm1, 29);

	mm2 = _mm_and_si64(mm2, *(__m64*)_pi32_2);
	mm3 = _mm_and_si64(mm3, *(__m64*)_pi32_2);

	mm2 = _mm_cmpeq_pi32(mm2, _mm_setzero_si64());
	mm3 = _mm_cmpeq_pi32(mm3, _mm_setzero_si64());

	__m128 sign_bit, poly_mask;
	COPY_MM_TO_XMM(mm0, mm1, sign_bit);
	COPY_MM_TO_XMM(mm2, mm3, poly_mask);
	_mm_empty(); /* good-bye mmx */
#endif
	/* The magic pass: "Extended precision modular arithmetic"
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(__m128*)_ps_minus_cephes_DP1;
	xmm2 = *(__m128*)_ps_minus_cephes_DP2;
	xmm3 = *(__m128*)_ps_minus_cephes_DP3;
	x = _mm_comp_fmadd_ps(y, xmm1, x);
	x = _mm_comp_fmadd_ps(y, xmm2, x);
	x = _mm_comp_fmadd_ps(y, xmm3, x);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = *(__m128*)_ps_coscof_p0;
	__m128 z = _mm_mul_ps(x, x);

	y = _mm_comp_fmadd_ps(y, z, *(__m128*)_ps_coscof_p1);
	y = _mm_comp_fmadd_ps(y, z, *(__m128*)_ps_coscof_p2);
	y = _mm_mul_ps(y, z);
	y = _mm_mul_ps(y, z);
	y = _mm_comp_fnmadd_ps(z, *(__m128*)_ps_0p5, y);
	y = _mm_add_ps(y, *(__m128*)_ps_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	__m128 y2 = *(__m128*)_ps_sincof_p0;
	y2 = _mm_comp_fmadd_ps(y2, z, *(__m128*)_ps_sincof_p1);
	y2 = _mm_comp_fmadd_ps(y2, z, *(__m128*)_ps_sincof_p2);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_comp_fmadd_ps(y2, x, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	y2 = _mm_and_ps(xmm3, y2); //, xmm3);
	y = _mm_andnot_ps(xmm3, y);
	y = _mm_add_ps(y, y2);
	/* update the sign */
	y = _mm_xor_ps(y, sign_bit);

	return y;
}

/* since sin_ps and cos_ps are almost identical, sincos_ps could replace both of them..
   it is almost as fast, and gives you a free cosine with your sine */
INLINE void sincos_ps(__m128 x, __m128* s, __m128* c)
{
	__m128 xmm1, xmm2, xmm3 = _mm_setzero_ps(), sign_bit_sin, y;
	__m128i emm0, emm2, emm4;
	sign_bit_sin = x;
	/* take the absolute value */
	x = _mm_and_ps(x, *(__m128*)_ps_inv_sign_mask);
	/* extract the sign bit (upper one) */
	sign_bit_sin = _mm_and_ps(sign_bit_sin, *(__m128*)_ps_sign_mask);

	/* scale by 4/Pi */
	y = _mm_mul_ps(x, *(__m128*)_ps_cephes_FOPI);

	/* store the integer part of y in emm2 */
	emm2 = _mm_cvttps_epi32(y);

	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = _mm_add_epi32(emm2, *(__m128i*)_pi32_1);
	emm2 = _mm_and_si128(emm2, *(__m128i*)_pi32_inv1);
	y = _mm_cvtepi32_ps(emm2);

	emm4 = emm2;

	/* get the swap sign flag for the sine */
	emm0 = _mm_and_si128(emm2, *(__m128i*)_pi32_4);
	emm0 = _mm_slli_epi32(emm0, 29);
	__m128 swap_sign_bit_sin = _mm_castsi128_ps(emm0);

	/* get the polynom selection mask for the sine*/
	emm2 = _mm_and_si128(emm2, *(__m128i*)_pi32_2);
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());
	__m128 poly_mask = _mm_castsi128_ps(emm2);

	/* The magic pass: "Extended precision modular arithmetic"
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(__m128*)_ps_minus_cephes_DP1;
	xmm2 = *(__m128*)_ps_minus_cephes_DP2;
	xmm3 = *(__m128*)_ps_minus_cephes_DP3;
	x = _mm_comp_fmadd_ps(y, xmm1, x);
	x = _mm_comp_fmadd_ps(y, xmm2, x);
	x = _mm_comp_fmadd_ps(y, xmm3, x);

	emm4 = _mm_sub_epi32(emm4, *(__m128i*)_pi32_2);
	emm4 = _mm_andnot_si128(emm4, *(__m128i*)_pi32_4);
	emm4 = _mm_slli_epi32(emm4, 29);
	__m128 sign_bit_cos = _mm_castsi128_ps(emm4);

	sign_bit_sin = _mm_xor_ps(sign_bit_sin, swap_sign_bit_sin);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	__m128 z = _mm_mul_ps(x, x);
	y = *(__m128*)_ps_coscof_p0;

	y = _mm_comp_fmadd_ps(y, z, *(__m128*)_ps_coscof_p1);
	y = _mm_comp_fmadd_ps(y, z, *(__m128*)_ps_coscof_p2);
	y = _mm_mul_ps(y, z);
	y = _mm_mul_ps(y, z);
	y = _mm_comp_fnmadd_ps(z, *(__m128*)_ps_0p5, y);
	y = _mm_add_ps(y, *(__m128*)_ps_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	__m128 y2 = *(__m128*)_ps_sincof_p0;
	y2 = _mm_comp_fmadd_ps(y2, z, *(__m128*)_ps_sincof_p1);
	y2 = _mm_comp_fmadd_ps(y2, z, *(__m128*)_ps_sincof_p2);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_comp_fmadd_ps(y2, x, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	__m128 ysin2 = _mm_and_ps(xmm3, y2);
	__m128 ysin1 = _mm_andnot_ps(xmm3, y);
	y2 = _mm_sub_ps(y2, ysin2);
	y = _mm_sub_ps(y, ysin1);

	xmm1 = _mm_add_ps(ysin1, ysin2);
	xmm2 = _mm_add_ps(y, y2);

	/* update the sign */
	*s = _mm_xor_ps(xmm1, sign_bit_sin);
	*c = _mm_xor_ps(xmm2, sign_bit_cos);
}

INLINE __m128 tan_ps(__m128 x)
{
	__m128 ysin, ycos;
	__m128 eps = _mm_set1_ps(1E-8f);
	sincos_ps(x, &ysin, &ycos);
	__m128 mask = _mm_cmpeq_ps(ycos, _mm_setzero_ps());
	__m128 _tmp = _mm_and_ps(eps, mask);
	ycos = _mm_add_ps(ycos, _tmp);
	__m128 ytan = _mm_div_ps(ysin, ycos);
	return ytan;
}

INLINE __m128 pow_ps(__m128 a, __m128 b)
{
	// pow(x, m) = exp(m * log(x))
	return exp_ps(_mm_mul_ps(b, log_ps(a)));
}

/* yes I know, the top of this file is quite ugly */

#ifdef _MSC_VER /* visual c++ */
	#define ALIGN32_BEG __declspec(align(32))
	#define ALIGN32_END
#else /* gcc or icc */
	#define ALIGN32_BEG
	#define ALIGN32_END __attribute__((aligned(32)))
#endif

#define _PI32AVX_CONST(Name, Val) static constexpr ALIGN32_BEG int _pi32avx_##Name[4] ALIGN32_END = {Val, Val, Val, Val}

_PI32AVX_CONST(1, 1);
_PI32AVX_CONST(inv1, ~1);
_PI32AVX_CONST(2, 2);
_PI32AVX_CONST(4, 4);

/* declare some AVX constants -- why can't I figure a better way to do that? */
#define _PS256_CONST(Name, Val) static constexpr ALIGN32_BEG f32 _ps256_##Name[8] ALIGN32_END = {Val, Val, Val, Val, Val, Val, Val, Val}
#define _PI32_CONST256(Name, Val) static constexpr ALIGN32_BEG int _pi32_256_##Name[8] ALIGN32_END = {Val, Val, Val, Val, Val, Val, Val, Val}
#define _PS256_CONST_TYPE(Name, Type, Val) static constexpr ALIGN32_BEG Type _ps256_##Name[8] ALIGN32_END = {Val, Val, Val, Val, Val, Val, Val, Val}

_PS256_CONST(1, 1.0f);
_PS256_CONST(0p5, 0.5f);
/* the smallest non denormalized f32 number */
_PS256_CONST_TYPE(min_norm_pos, int, 0x00800000);
_PS256_CONST_TYPE(mant_mask, int, 0x7f800000);
_PS256_CONST_TYPE(inv_mant_mask, int, ~0x7f800000);

_PS256_CONST_TYPE(sign_mask, int, (int)0x80000000);
_PS256_CONST_TYPE(inv_sign_mask, int, ~0x80000000);

_PI32_CONST256(0, 0);
_PI32_CONST256(1, 1);
_PI32_CONST256(inv1, ~1);
_PI32_CONST256(2, 2);
_PI32_CONST256(4, 4);
_PI32_CONST256(0x7f, 0x7f);

_PS256_CONST(cephes_SQRTHF, 0.707106781186547524f);
_PS256_CONST(cephes_log_p0, 7.0376836292E-2f);
_PS256_CONST(cephes_log_p1, -1.1514610310E-1f);
_PS256_CONST(cephes_log_p2, 1.1676998740E-1f);
_PS256_CONST(cephes_log_p3, -1.2420140846E-1f);
_PS256_CONST(cephes_log_p4, +1.4249322787E-1f);
_PS256_CONST(cephes_log_p5, -1.6668057665E-1f);
_PS256_CONST(cephes_log_p6, +2.0000714765E-1f);
_PS256_CONST(cephes_log_p7, -2.4999993993E-1f);
_PS256_CONST(cephes_log_p8, +3.3333331174E-1f);
_PS256_CONST(cephes_log_q1, -2.12194440e-4f);
_PS256_CONST(cephes_log_q2, 0.693359375f);

#if !AVX2
typedef union imm_xmm_union
{
	__m256i imm;
	__m128i xmm[2];
} imm_xmm_union;

#define COPY_IMM_TO_XMM(imm_, xmm0_, xmm1_)      \
    {                                            \
        ALIGN32_BEG imm_xmm_union u ALIGN32_END; \
        u.imm = imm_;                            \
        xmm0_ = u.xmm[0];                        \
        xmm1_ = u.xmm[1];                        \
    }

#define COPY_XMM_TO_IMM(xmm0_, xmm1_, imm_)      \
    {                                            \
        ALIGN32_BEG imm_xmm_union u ALIGN32_END; \
        u.xmm[0] = xmm0_;                        \
        u.xmm[1] = xmm1_;                        \
        imm_ = u.imm;                            \
    }

#define AVX2_BITOP_USING_SSE2(fn)                                      \
    INLINE __m256i _mm256_comp_##fn(__m256i x, int a) \
    {                                                                  \
        /* use SSE2 instruction to perform the bitop AVX2 */           \
        __m128i x1, x2;                                                \
        __m256i ret;                                                   \
        COPY_IMM_TO_XMM(x, x1, x2);                                    \
        x1 = _mm_##fn(x1, a);                                          \
        x2 = _mm_##fn(x2, a);                                          \
        COPY_XMM_TO_IMM(x1, x2, ret);                                  \
        return (ret);                                                  \
    }
#define AVX2_INTOP_USING_SSE2(fn)                                          \
    INLINE __m256i _mm256_comp_##fn(__m256i x, __m256i y) \
    {                                                                      \
        /* use SSE2 instructions to perform the AVX2 integer operation */  \
        __m128i x1, x2;                                                    \
        __m128i y1, y2;                                                    \
        __m256i ret;                                                       \
        COPY_IMM_TO_XMM(x, x1, x2);                                        \
        COPY_IMM_TO_XMM(y, y1, y2);                                        \
        x1 = _mm_##fn(x1, y1);                                             \
        x2 = _mm_##fn(x2, y2);                                             \
        COPY_XMM_TO_IMM(x1, x2, ret);                                      \
        return (ret);                                                      \
    }
#else
#define AVX2_BITOP_USING_SSE2(fn)                                      \
    INLINE __m256i _mm256_comp_##fn(__m256i x, int a)                  \
    {                                                                  \
        return _mm256_##fn(x, a);                                      \
    }
#define AVX2_INTOP_USING_SSE2(fn)                                          \
    INLINE __m256i _mm256_comp_##fn(__m256i x, __m256i y)                  \
    {                                                                      \
        return _mm256_##fn(x, y);                                          \
    }
#endif

AVX2_BITOP_USING_SSE2(slli_epi32)
AVX2_BITOP_USING_SSE2(srli_epi32)
AVX2_INTOP_USING_SSE2(cmpeq_epi32)
AVX2_INTOP_USING_SSE2(sub_epi32)
AVX2_INTOP_USING_SSE2(add_epi32)

// Replace 256 bit operations with 128 bit ones when AVX2 is disabled
#if !AVX2
	AVX2_INTOP_USING_SSE2(and_si128)
	AVX2_INTOP_USING_SSE2(andnot_si128)
#endif

/* natural logarithm computed for 8 simultaneous f32
   return NaN for x <= 0
*/
INLINE __m256 log256_ps(__m256 x)
{
	__m256i imm0;
	__m256 one = *(__m256*)_ps256_1;

	//__m256 invalid_mask = _mm256_cmple_ps(x, _mm256_setzero_ps());
	__m256 invalid_mask = _mm256_cmp_ps(x, _mm256_setzero_ps(), _CMP_LE_OS);

	x = _mm256_max_ps(x, *(__m256*)_ps256_min_norm_pos); /* cut off denormalized stuff */

	// can be done with AVX2
	imm0 = _mm256_comp_srli_epi32(_mm256_castps_si256(x), 23);

	/* keep only the fractional part */
	x = _mm256_and_ps(x, *(__m256*)_ps256_inv_mant_mask);
	x = _mm256_or_ps(x, *(__m256*)_ps256_0p5);

	// this is again another AVX2 instruction
	imm0 = _mm256_comp_sub_epi32(imm0, *(__m256i*)_pi32_256_0x7f);
	__m256 e = _mm256_cvtepi32_ps(imm0);

	e = _mm256_add_ps(e, one);

	/* part2:
	   if( x < SQRTHF ) {
		 e -= 1;
		 x = x + x - 1.0;
	   } else { x = x - 1.0; }
	*/
	//__m256 mask = _mm256_cmplt_ps(x, *(__m256*)_ps256_cephes_SQRTHF);
	__m256 mask = _mm256_cmp_ps(x, *(__m256*)_ps256_cephes_SQRTHF, _CMP_LT_OS);
	__m256 tmp = _mm256_and_ps(x, mask);
	x = _mm256_sub_ps(x, one);
	e = _mm256_sub_ps(e, _mm256_and_ps(one, mask));
	x = _mm256_add_ps(x, tmp);

	__m256 z = _mm256_mul_ps(x, x);

	__m256 y = *(__m256*)_ps256_cephes_log_p0;
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_log_p1);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_log_p2);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_log_p3);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_log_p4);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_log_p5);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_log_p6);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_log_p7);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_log_p8);
	y = _mm256_mul_ps(y, x);

	y = _mm256_mul_ps(y, z);

	y = _mm256_comp_fmadd_ps(e, *(__m256*)_ps256_cephes_log_q1, y);

	//y = -z * 0.5 + y
	y = _mm256_comp_fnmadd_ps(z, *(__m256*)_ps256_0p5, y);

	x = _mm256_add_ps(x, y);
	x = _mm256_comp_fmadd_ps(e, *(__m256*)_ps256_cephes_log_q2, x);
	y = _mm256_or_ps(x, invalid_mask); // negative arg will be NAN
	return y;
}

_PS256_CONST(exp_hi, 88.3762626647949f);
_PS256_CONST(exp_lo, -88.3762626647949f);

_PS256_CONST(cephes_LOG2EF, 1.44269504088896341f);
_PS256_CONST(cephes_exp_C1, 0.693359375f);
_PS256_CONST(cephes_exp_C2, -2.12194440e-4f);

_PS256_CONST(cephes_exp_p0, 1.9875691500E-4f);
_PS256_CONST(cephes_exp_p1, 1.3981999507E-3f);
_PS256_CONST(cephes_exp_p2, 8.3334519073E-3f);
_PS256_CONST(cephes_exp_p3, 4.1665795894E-2f);
_PS256_CONST(cephes_exp_p4, 1.6666665459E-1f);
_PS256_CONST(cephes_exp_p5, 5.0000001201E-1f);

INLINE __m256 exp256_ps(__m256 x)
{
	__m256 tmp = _mm256_setzero_ps(), fx;
	__m256i imm0;
	__m256 one = *(__m256*)_ps256_1;

	x = _mm256_min_ps(x, *(__m256*)_ps256_exp_hi);
	x = _mm256_max_ps(x, *(__m256*)_ps256_exp_lo);

	/* express exp(x) as exp(g + n*log(2)) */
	fx = _mm256_comp_fmadd_ps(x, *(__m256*)_ps256_cephes_LOG2EF, *(__m256*)_ps256_0p5);

	/* how to perform a floorf with SSE: just below */
	//imm0 = _mm256_cvttps_epi32(fx);
	//tmp  = _mm256_cvtepi32_ps(imm0);

	tmp = _mm256_floor_ps(fx);

	/* if greater, subtract 1 */
	//__m256 mask = _mm256_cmpgt_ps(tmp, fx);
	__m256 mask = _mm256_cmp_ps(tmp, fx, _CMP_GT_OS);
	mask = _mm256_and_ps(mask, one);
	fx = _mm256_sub_ps(tmp, mask);

	// x = x - fx * exp_C1
	x = _mm256_comp_fnmadd_ps(fx, *(__m256*)_ps256_cephes_exp_C1, x);
	// x = x - fx * exp_C2
	x = _mm256_comp_fnmadd_ps(fx, *(__m256*)_ps256_cephes_exp_C2, x);

	tmp = _mm256_mul_ps(x, x);

	__m256 y = *(__m256*)_ps256_cephes_exp_p0;
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_exp_p1);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_exp_p2);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_exp_p3);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_exp_p4);
	y = _mm256_comp_fmadd_ps(y, x, *(__m256*)_ps256_cephes_exp_p5);
	y = _mm256_comp_fmadd_ps(y, tmp, x);
	y = _mm256_add_ps(y, one);

	/* build 2^n */
	imm0 = _mm256_cvttps_epi32(fx);
	// another two AVX2 instructions
	imm0 = _mm256_comp_add_epi32(imm0, *(__m256i*)_pi32_256_0x7f);
	imm0 = _mm256_comp_slli_epi32(imm0, 23);
	__m256 pow2n = _mm256_castsi256_ps(imm0);
	y = _mm256_mul_ps(y, pow2n);
	return y;
}

_PS256_CONST(tanh_hi, 9.0f);
_PS256_CONST(tanh_lo, -9.0f);

_PS256_CONST(cephes_tanh_p0, -2.76076847742355E-16f);
_PS256_CONST(cephes_tanh_p1, 2.00018790482477E-13f);
_PS256_CONST(cephes_tanh_p2, -8.60467152213735E-11f);
_PS256_CONST(cephes_tanh_p3, 5.12229709037114E-08f);
_PS256_CONST(cephes_tanh_p4, 1.48572235717979E-05f);
_PS256_CONST(cephes_tanh_p5, 6.37261928875436E-04f);
_PS256_CONST(cephes_tanh_p6, 4.89352455891786E-03f);

_PS256_CONST(cephes_tanh_p7, 1.19825839466702e-06f);
_PS256_CONST(cephes_tanh_p8, 1.18534705686654e-04f);
_PS256_CONST(cephes_tanh_p9, 2.26843463243900e-03f);

// an approximation of tanh
inline __m256 tanh256_ps(const __m256 x)
{
	__m256 value = x;
	value = _mm256_max_ps(*(__m256*)_ps256_tanh_lo, value);
	value = _mm256_min_ps(*(__m256*)_ps256_tanh_hi, value);

	__m256 value_squared = _mm256_mul_ps(value, value);

	__m256 p;
	p = _mm256_comp_fmadd_ps(value_squared, *(__m256*)_ps256_cephes_tanh_p0, *(__m256*)_ps256_cephes_tanh_p1);
	p = _mm256_comp_fmadd_ps(p, value_squared, *(__m256*)_ps256_cephes_tanh_p2);
	p = _mm256_comp_fmadd_ps(p, value_squared, *(__m256*)_ps256_cephes_tanh_p3);
	p = _mm256_comp_fmadd_ps(p, value_squared, *(__m256*)_ps256_cephes_tanh_p4);
	p = _mm256_comp_fmadd_ps(p, value_squared, *(__m256*)_ps256_cephes_tanh_p5);
	p = _mm256_comp_fmadd_ps(p, value_squared, *(__m256*)_ps256_cephes_tanh_p6);
	p = _mm256_mul_ps(p, value);

	__m256 q;
	q = _mm256_comp_fmadd_ps(value_squared, *(__m256*)_ps256_cephes_tanh_p7, *(__m256*)_ps256_cephes_tanh_p8);
	q = _mm256_comp_fmadd_ps(q, value_squared, *(__m256*)_ps256_cephes_tanh_p9);
	q = _mm256_comp_fmadd_ps(q, value_squared, *(__m256*)_ps256_cephes_tanh_p6);

	__m256 dst = _mm256_div_ps(p, q);
	return dst;
}

_PS256_CONST(minus_cephes_DP1, -0.78515625f);
_PS256_CONST(minus_cephes_DP2, -2.4187564849853515625e-4f);
_PS256_CONST(minus_cephes_DP3, -3.77489497744594108e-8f);
_PS256_CONST(sincof_p0, -1.9515295891E-4f);
_PS256_CONST(sincof_p1, 8.3321608736E-3f);
_PS256_CONST(sincof_p2, -1.6666654611E-1f);
_PS256_CONST(coscof_p0, 2.443315711809948E-005f);
_PS256_CONST(coscof_p1, -1.388731625493765E-003f);
_PS256_CONST(coscof_p2, 4.166664568298827E-002f);
_PS256_CONST(cephes_FOPI, 1.27323954473516f); // 4 / M_PI

/* evaluation of 8 sines at onces using AVX intrisics

   The code is the exact rewriting of the cephes sinf function.
   Precision is excellent as long as x < 8192 (I did not bother to
   take into account the special handling they have for greater values
   -- it does not return garbage for arguments over 8192, though, but
   the extra precision is missing).

   Note that it is such that sinf((f32)M_PI) = 8.74e-8, which is the
   surprising but correct result.

*/
INLINE __m256 sin256_ps(__m256 x)
{   // any x
	__m256 xmm1, xmm2 = _mm256_setzero_ps(), xmm3, sign_bit, y;
	__m256i imm0, imm2;

#if !AVX2
	__m128i imm0_1, imm0_2;
	__m128i imm2_1, imm2_2;
#endif

	sign_bit = x;
	/* take the absolute value */
	x = _mm256_and_ps(x, *(__m256*)_ps256_inv_sign_mask);
	/* extract the sign bit (upper one) */
	sign_bit = _mm256_and_ps(sign_bit, *(__m256*)_ps256_sign_mask);

	/* scale by 4/Pi */
	y = _mm256_mul_ps(x, *(__m256*)_ps256_cephes_FOPI);

	/*
	  Here we start a series of integer operations, which are in the
	  realm of AVX2.
	  If we don't have AVX, let's perform them using SSE2 directives
	*/

#if AVX2
	/* store the integer part of y in mm0 */
	imm2 = _mm256_cvttps_epi32(y);
	/* j=(j+1) & (~1) (see the cephes sources) */
	// another two AVX2 instruction
	imm2 = _mm256_comp_add_epi32(imm2, *(__m256i*)_pi32_256_1);
	imm2 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_inv1);
	y = _mm256_cvtepi32_ps(imm2);

	/* get the swap sign flag */
	imm0 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_4);
	imm0 = _mm256_comp_slli_epi32(imm0, 29);
	/* get the polynom selection mask
	   there is one polynom for 0 <= x <= Pi/4
	   and another one for Pi/4<x<=Pi/2

	   Both branches will be computed.
	*/
	imm2 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_2);
	imm2 = _mm256_cmpeq_epi32(imm2, *(__m256i*)_pi32_256_0);
#else
	/* we use SSE2 routines to perform the integer ops */
	COPY_IMM_TO_XMM(_mm256_cvttps_epi32(y), imm2_1, imm2_2);

	imm2_1 = _mm_add_epi32(imm2_1, *(__m128i*)_pi32avx_1);
	imm2_2 = _mm_add_epi32(imm2_2, *(__m128i*)_pi32avx_1);

	imm2_1 = _mm_and_si128(imm2_1, *(__m128i*)_pi32avx_inv1);
	imm2_2 = _mm_and_si128(imm2_2, *(__m128i*)_pi32avx_inv1);

	COPY_XMM_TO_IMM(imm2_1, imm2_2, imm2);
	y = _mm256_cvtepi32_ps(imm2);

	imm0_1 = _mm_and_si128(imm2_1, *(__m128i*)_pi32avx_4);
	imm0_2 = _mm_and_si128(imm2_2, *(__m128i*)_pi32avx_4);

	imm0_1 = _mm_slli_epi32(imm0_1, 29);
	imm0_2 = _mm_slli_epi32(imm0_2, 29);

	COPY_XMM_TO_IMM(imm0_1, imm0_2, imm0);

	imm2_1 = _mm_and_si128(imm2_1, *(__m128i*)_pi32avx_2);
	imm2_2 = _mm_and_si128(imm2_2, *(__m128i*)_pi32avx_2);

	imm2_1 = _mm_cmpeq_epi32(imm2_1, _mm_setzero_si128());
	imm2_2 = _mm_cmpeq_epi32(imm2_2, _mm_setzero_si128());

	COPY_XMM_TO_IMM(imm2_1, imm2_2, imm2);
#endif

	__m256 swap_sign_bit = _mm256_castsi256_ps(imm0);
	__m256 poly_mask = _mm256_castsi256_ps(imm2);
	sign_bit = _mm256_xor_ps(sign_bit, swap_sign_bit);

	/* The magic pass: "Extended precision modular arithmetic"
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(__m256*)_ps256_minus_cephes_DP1;
	xmm2 = *(__m256*)_ps256_minus_cephes_DP2;
	xmm3 = *(__m256*)_ps256_minus_cephes_DP3;
	x = _mm256_comp_fmadd_ps(y, xmm1, x);
	x = _mm256_comp_fmadd_ps(y, xmm2, x);
	x = _mm256_comp_fmadd_ps(y, xmm3, x);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = *(__m256*)_ps256_coscof_p0;
	__m256 z = _mm256_mul_ps(x, x);

	y = _mm256_comp_fmadd_ps(y, z, *(__m256*)_ps256_coscof_p1);
	y = _mm256_comp_fmadd_ps(y, z, *(__m256*)_ps256_coscof_p2);
	y = _mm256_mul_ps(y, z);
	y = _mm256_mul_ps(y, z);
	// y = y - z * 0.5
	y = _mm256_comp_fnmadd_ps(z, *(__m256*)_ps256_0p5, y);
	y = _mm256_add_ps(y, *(__m256*)_ps256_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	__m256 y2 = *(__m256*)_ps256_sincof_p0;
	y2 = _mm256_comp_fmadd_ps(y2, z, *(__m256*)_ps256_sincof_p1);
	y2 = _mm256_comp_fmadd_ps(y2, z, *(__m256*)_ps256_sincof_p2);
	y2 = _mm256_mul_ps(y2, z);
	y2 = _mm256_comp_fmadd_ps(y2, x, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	y2 = _mm256_and_ps(xmm3, y2); //, xmm3);
	y = _mm256_andnot_ps(xmm3, y);
	y = _mm256_add_ps(y, y2);
	/* update the sign */
	y = _mm256_xor_ps(y, sign_bit);

	return y;
}

/* almost the same as sin_ps */
INLINE __m256 cos256_ps(__m256 x)
{   // any x
	__m256 xmm1, xmm2 = _mm256_setzero_ps(), xmm3, y;
	__m256i imm0, imm2;

#if !AVX2
	__m128i imm0_1, imm0_2;
	__m128i imm2_1, imm2_2;
#endif

	/* take the absolute value */
	x = _mm256_and_ps(x, *(__m256*)_ps256_inv_sign_mask);

	/* scale by 4/Pi */
	y = _mm256_mul_ps(x, *(__m256*)_ps256_cephes_FOPI);

#if AVX2
	/* store the integer part of y in mm0 */
	imm2 = _mm256_cvttps_epi32(y);
	/* j=(j+1) & (~1) (see the cephes sources) */
	imm2 = _mm256_comp_add_epi32(imm2, *(__m256i*)_pi32_256_1);
	imm2 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_inv1);
	y = _mm256_cvtepi32_ps(imm2);
	imm2 = _mm256_comp_sub_epi32(imm2, *(__m256i*)_pi32_256_2);

	/* get the swap sign flag */
	imm0 = _mm256_andnot_si256(imm2, *(__m256i*)_pi32_256_4);
	imm0 = _mm256_comp_slli_epi32(imm0, 29);
	/* get the polynom selection mask */
	imm2 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_2);
	imm2 = _mm256_cmpeq_epi32(imm2, *(__m256i*)_pi32_256_0);
#else

	/* we use SSE2 routines to perform the integer ops */
	COPY_IMM_TO_XMM(_mm256_cvttps_epi32(y), imm2_1, imm2_2);

	imm2_1 = _mm_add_epi32(imm2_1, *(__m128i*)_pi32avx_1);
	imm2_2 = _mm_add_epi32(imm2_2, *(__m128i*)_pi32avx_1);

	imm2_1 = _mm_and_si128(imm2_1, *(__m128i*)_pi32avx_inv1);
	imm2_2 = _mm_and_si128(imm2_2, *(__m128i*)_pi32avx_inv1);

	COPY_XMM_TO_IMM(imm2_1, imm2_2, imm2);
	y = _mm256_cvtepi32_ps(imm2);

	imm2_1 = _mm_sub_epi32(imm2_1, *(__m128i*)_pi32avx_2);
	imm2_2 = _mm_sub_epi32(imm2_2, *(__m128i*)_pi32avx_2);

	imm0_1 = _mm_andnot_si128(imm2_1, *(__m128i*)_pi32avx_4);
	imm0_2 = _mm_andnot_si128(imm2_2, *(__m128i*)_pi32avx_4);

	imm0_1 = _mm_slli_epi32(imm0_1, 29);
	imm0_2 = _mm_slli_epi32(imm0_2, 29);

	COPY_XMM_TO_IMM(imm0_1, imm0_2, imm0);

	imm2_1 = _mm_and_si128(imm2_1, *(__m128i*)_pi32avx_2);
	imm2_2 = _mm_and_si128(imm2_2, *(__m128i*)_pi32avx_2);

	imm2_1 = _mm_cmpeq_epi32(imm2_1, _mm_setzero_si128());
	imm2_2 = _mm_cmpeq_epi32(imm2_2, _mm_setzero_si128());

	COPY_XMM_TO_IMM(imm2_1, imm2_2, imm2);
#endif

	__m256 sign_bit = _mm256_castsi256_ps(imm0);
	__m256 poly_mask = _mm256_castsi256_ps(imm2);

	/* The magic pass: "Extended precision modular arithmetic"
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(__m256*)_ps256_minus_cephes_DP1;
	xmm2 = *(__m256*)_ps256_minus_cephes_DP2;
	xmm3 = *(__m256*)_ps256_minus_cephes_DP3;
	x = _mm256_comp_fmadd_ps(y, xmm1, x);
	x = _mm256_comp_fmadd_ps(y, xmm2, x);
	x = _mm256_comp_fmadd_ps(y, xmm3, x);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = *(__m256*)_ps256_coscof_p0;
	__m256 z = _mm256_mul_ps(x, x);

	y = _mm256_comp_fmadd_ps(y, z, *(__m256*)_ps256_coscof_p1);
	y = _mm256_comp_fmadd_ps(y, z, *(__m256*)_ps256_coscof_p2);
	y = _mm256_mul_ps(y, z);
	y = _mm256_mul_ps(y, z);
	// y = y - z * 0.5
	y = _mm256_comp_fnmadd_ps(z, *(__m256*)_ps256_0p5, y);
	y = _mm256_add_ps(y, *(__m256*)_ps256_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	__m256 y2 = *(__m256*)_ps256_sincof_p0;
	y2 = _mm256_mul_ps(y2, z);
	y2 = _mm256_add_ps(y2, *(__m256*)_ps256_sincof_p1);
	y2 = _mm256_mul_ps(y2, z);
	y2 = _mm256_add_ps(y2, *(__m256*)_ps256_sincof_p2);
	y2 = _mm256_mul_ps(y2, z);
	y2 = _mm256_mul_ps(y2, x);
	y2 = _mm256_add_ps(y2, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	y2 = _mm256_and_ps(xmm3, y2); //, xmm3);
	y = _mm256_andnot_ps(xmm3, y);
	y = _mm256_add_ps(y, y2);
	/* update the sign */
	y = _mm256_xor_ps(y, sign_bit);

	return y;
}

/* since sin256_ps and cos256_ps are almost identical, sincos256_ps could replace both of them..
   it is almost as fast, and gives you a free cosine with your sine */
INLINE void sincos256_ps(__m256 x, __m256* s, __m256* c)
{
	__m256 xmm1, xmm2, xmm3 = _mm256_setzero_ps(), sign_bit_sin, y;
	__m256i imm0, imm2, imm4;

#if !AVX2
	__m128i imm0_1, imm0_2;
	__m128i imm2_1, imm2_2;
	__m128i imm4_1, imm4_2;
#endif

	sign_bit_sin = x;
	/* take the absolute value */
	x = _mm256_and_ps(x, *(__m256*)_ps256_inv_sign_mask);
	/* extract the sign bit (upper one) */
	sign_bit_sin = _mm256_and_ps(sign_bit_sin, *(__m256*)_ps256_sign_mask);

	/* scale by 4/Pi */
	y = _mm256_mul_ps(x, *(__m256*)_ps256_cephes_FOPI);

#if AVX2
	/* store the integer part of y in imm2 */
	imm2 = _mm256_cvttps_epi32(y);

	/* j=(j+1) & (~1) (see the cephes sources) */
	imm2 = _mm256_comp_add_epi32(imm2, *(__m256i*)_pi32_256_1);
	imm2 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_inv1);

	y = _mm256_cvtepi32_ps(imm2);
	imm4 = imm2;

	/* get the swap sign flag for the sine */
	imm0 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_4);
	imm0 = _mm256_comp_slli_epi32(imm0, 29);
	//__m256 swap_sign_bit_sin = _mm256_castsi256_ps(imm0);

	/* get the polynom selection mask for the sine*/
	imm2 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_2);
	imm2 = _mm256_cmpeq_epi32(imm2, *(__m256i*)_pi32_256_0);
	//__m256 poly_mask = _mm256_castsi256_ps(imm2);
#else
	/* we use SSE2 routines to perform the integer ops */
	COPY_IMM_TO_XMM(_mm256_cvttps_epi32(y), imm2_1, imm2_2);

	imm2_1 = _mm_add_epi32(imm2_1, *(__m128i*)_pi32avx_1);
	imm2_2 = _mm_add_epi32(imm2_2, *(__m128i*)_pi32avx_1);

	imm2_1 = _mm_and_si128(imm2_1, *(__m128i*)_pi32avx_inv1);
	imm2_2 = _mm_and_si128(imm2_2, *(__m128i*)_pi32avx_inv1);

	COPY_XMM_TO_IMM(imm2_1, imm2_2, imm2);
	y = _mm256_cvtepi32_ps(imm2);

	imm4_1 = imm2_1;
	imm4_2 = imm2_2;

	imm0_1 = _mm_and_si128(imm2_1, *(__m128i*)_pi32avx_4);
	imm0_2 = _mm_and_si128(imm2_2, *(__m128i*)_pi32avx_4);

	imm0_1 = _mm_slli_epi32(imm0_1, 29);
	imm0_2 = _mm_slli_epi32(imm0_2, 29);

	COPY_XMM_TO_IMM(imm0_1, imm0_2, imm0);

	imm2_1 = _mm_and_si128(imm2_1, *(__m128i*)_pi32avx_2);
	imm2_2 = _mm_and_si128(imm2_2, *(__m128i*)_pi32avx_2);

	imm2_1 = _mm_cmpeq_epi32(imm2_1, _mm_setzero_si128());
	imm2_2 = _mm_cmpeq_epi32(imm2_2, _mm_setzero_si128());

	COPY_XMM_TO_IMM(imm2_1, imm2_2, imm2);
#endif
	__m256 swap_sign_bit_sin = _mm256_castsi256_ps(imm0);
	__m256 poly_mask = _mm256_castsi256_ps(imm2);

	/* The magic pass: "Extended precision modular arithmetic"
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(__m256*)_ps256_minus_cephes_DP1;
	xmm2 = *(__m256*)_ps256_minus_cephes_DP2;
	xmm3 = *(__m256*)_ps256_minus_cephes_DP3;
	x = _mm256_comp_fmadd_ps(y, xmm1, x);
	x = _mm256_comp_fmadd_ps(y, xmm2, x);
	x = _mm256_comp_fmadd_ps(y, xmm3, x);

#if AVX2
	imm4 = _mm256_comp_sub_epi32(imm4, *(__m256i*)_pi32_256_2);
	imm4 = _mm256_andnot_si256(imm4, *(__m256i*)_pi32_256_4);
	imm4 = _mm256_comp_slli_epi32(imm4, 29);
#else
	imm4_1 = _mm_sub_epi32(imm4_1, *(__m128i*)_pi32avx_2);
	imm4_2 = _mm_sub_epi32(imm4_2, *(__m128i*)_pi32avx_2);

	imm4_1 = _mm_andnot_si128(imm4_1, *(__m128i*)_pi32avx_4);
	imm4_2 = _mm_andnot_si128(imm4_2, *(__m128i*)_pi32avx_4);

	imm4_1 = _mm_slli_epi32(imm4_1, 29);
	imm4_2 = _mm_slli_epi32(imm4_2, 29);

	COPY_XMM_TO_IMM(imm4_1, imm4_2, imm4);
#endif

	__m256 sign_bit_cos = _mm256_castsi256_ps(imm4);

	sign_bit_sin = _mm256_xor_ps(sign_bit_sin, swap_sign_bit_sin);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	__m256 z = _mm256_mul_ps(x, x);
	y = *(__m256*)_ps256_coscof_p0;

	y = _mm256_comp_fmadd_ps(y, z, *(__m256*)_ps256_coscof_p1);
	y = _mm256_comp_fmadd_ps(y, z, *(__m256*)_ps256_coscof_p2);
	y = _mm256_mul_ps(y, z);
	y = _mm256_mul_ps(y, z);
	// y = y - z * 0.5
	y = _mm256_comp_fnmadd_ps(z, *(__m256*)_ps256_0p5, y);
	y = _mm256_add_ps(y, *(__m256*)_ps256_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	__m256 y2 = *(__m256*)_ps256_sincof_p0;
	y2 = _mm256_comp_fmadd_ps(y2, z, *(__m256*)_ps256_sincof_p1);
	y2 = _mm256_comp_fmadd_ps(y2, z, *(__m256*)_ps256_sincof_p2);
	y2 = _mm256_mul_ps(y2, z);
	y2 = _mm256_comp_fmadd_ps(y2, x, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	__m256 ysin2 = _mm256_and_ps(xmm3, y2);
	__m256 ysin1 = _mm256_andnot_ps(xmm3, y);
	y2 = _mm256_sub_ps(y2, ysin2);
	y = _mm256_sub_ps(y, ysin1);

	xmm1 = _mm256_add_ps(ysin1, ysin2);
	xmm2 = _mm256_add_ps(y, y2);

	/* update the sign */
	*s = _mm256_xor_ps(xmm1, sign_bit_sin);
	*c = _mm256_xor_ps(xmm2, sign_bit_cos);
}

INLINE __m256 tan256_ps(__m256 x)
{
	__m256 ysin, ycos;
	__m256 eps = _mm256_set1_ps(1E-8f);
	sincos256_ps(x, &ysin, &ycos);
	__m256 mask = _mm256_cmp_ps(ycos, _mm256_setzero_ps(), _CMP_EQ_OS);
	__m256 _tmp = _mm256_and_ps(eps, mask);
	ycos = _mm256_add_ps(ycos, _tmp);
	__m256 ytan = _mm256_div_ps(ysin, ycos);
	return ytan;
}

INLINE __m256 pow256_ps(__m256 a, __m256 b)
{
	// pow(x, m) = exp(m * log(x))
	return exp256_ps(_mm256_mul_ps(b, log256_ps(a)));
}

// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2022 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include <immintrin.h>
#include <emmintrin.h>

/* yes I know, the top of this file is quite ugly */

#ifdef _MSC_VER /* visual c++ */
	#define ALIGN64_BEG __declspec(align(64))
	#define ALIGN64_END
#else /* gcc or icc */
	#define ALIGN64_BEG
	#define ALIGN64_END __attribute__((aligned(64)))
#endif

/* declare some AVX constants -- why can't I figure a better way to do that? */
#define _PS512_CONST(Name, Val)  static constexpr ALIGN64_BEG f32 _ps512_##Name[16] ALIGN64_END = {Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val}
#define _PI32_CONST512(Name, Val)  static constexpr ALIGN64_BEG int _pi32_512_##Name[16] ALIGN64_END = {Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val}
#define _PS512_CONST_TYPE(Name, Type, Val)  static constexpr ALIGN64_BEG Type _ps512_##Name[16] ALIGN64_END = {Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val, Val}

_PS512_CONST(1, 1.0f);
_PS512_CONST(0p5, 0.5f);
/* the smallest non denormalized f32 number */
_PS512_CONST_TYPE(min_norm_pos, int, 0x00800000);
_PS512_CONST_TYPE(mant_mask, int, 0x7f800000);
_PS512_CONST_TYPE(inv_mant_mask, int, ~0x7f800000);

_PS512_CONST_TYPE(sign_mask, int, (int)0x80000000);

_PI32_CONST512(0, 0);
_PI32_CONST512(1, 1);
_PI32_CONST512(inv1, ~1);
_PI32_CONST512(2, 2);
_PI32_CONST512(4, 4);
_PI32_CONST512(0x7f, 0x7f);

_PS512_CONST(cephes_SQRTHF, 0.707106781186547524f);
_PS512_CONST(cephes_log_p0, 7.0376836292E-2f);
_PS512_CONST(cephes_log_p1, -1.1514610310E-1f);
_PS512_CONST(cephes_log_p2, 1.1676998740E-1f);
_PS512_CONST(cephes_log_p3, -1.2420140846E-1f);
_PS512_CONST(cephes_log_p4, +1.4249322787E-1f);
_PS512_CONST(cephes_log_p5, -1.6668057665E-1f);
_PS512_CONST(cephes_log_p6, +2.0000714765E-1f);
_PS512_CONST(cephes_log_p7, -2.4999993993E-1f);
_PS512_CONST(cephes_log_p8, +3.3333331174E-1f);
_PS512_CONST(cephes_log_q1, -2.12194440e-4f);
_PS512_CONST(cephes_log_q2, 0.693359375f);

//natural logarithm computed for 8 simultaneous f32
//return NaN for x <= 0
INLINE __m512 log512_ps(__m512 x)
{
	__m512i imm0;
	__m512 one = *(__m512*)_ps512_1;

	__mmask16 invalid_mask = _mm512_cmp_ps_mask(x, _mm512_setzero_ps(), _CMP_LE_OQ);

	x = _mm512_max_ps(x, *(__m512*)_ps512_min_norm_pos); /* cut off denormalized stuff */

	imm0 = _mm512_srli_epi32(_mm512_castps_si512(x), 23);

	/* keep only the fractional part */
	x = _mm512_and_ps(x, *(__m512*)_ps512_inv_mant_mask);
	x = _mm512_or_ps(x, *(__m512*)_ps512_0p5);

	imm0 = _mm512_sub_epi32(imm0, *(__m512i*)_pi32_512_0x7f);
	__m512 e = _mm512_cvtepi32_ps(imm0);

	e = _mm512_add_ps(e, one);

	/* part2:
	   if( x < SQRTHF ) {
		 e -= 1;
		 x = x + x - 1.0;
	   } else { x = x - 1.0; }
	*/
	__mmask16 mask = _mm512_cmp_ps_mask(x, *(__m512*)_ps512_cephes_SQRTHF, _CMP_LT_OQ);
	__m512 tmp = _mm512_sub_ps(x, one);
	e = _mm512_mask_sub_ps(e, mask, e, one);
	x = _mm512_mask_add_ps(tmp, mask, tmp, x);

	__m512 z = _mm512_mul_ps(x, x);

	__m512 y = *(__m512*)_ps512_cephes_log_p0;
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_log_p1);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_log_p2);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_log_p3);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_log_p4);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_log_p5);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_log_p6);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_log_p7);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_log_p8);
	y = _mm512_mul_ps(y, x);

	y = _mm512_mul_ps(y, z);

	y = _mm512_fmadd_ps(e, *(__m512*)_ps512_cephes_log_q1, y);

	//y = -z * 0.5 + y
	y = _mm512_fnmadd_ps(z, *(__m512*)_ps512_0p5, y);

	x = _mm512_add_ps(x, y);
	x = _mm512_fmadd_ps(e, *(__m512*)_ps512_cephes_log_q2, x);
	y = _mm512_or_ps(x, _mm512_castsi512_ps(_mm512_movm_epi32(invalid_mask))); // negative arg will be NAN
	return y;
}

_PS512_CONST(exp_hi, 88.3762626647949f);
_PS512_CONST(exp_lo, -88.3762626647949f);

_PS512_CONST(cephes_LOG2EF, 1.44269504088896341f);
_PS512_CONST(cephes_exp_C1, 0.693359375f);
_PS512_CONST(cephes_exp_C2, -2.12194440e-4f);

_PS512_CONST(cephes_exp_p0, 1.9875691500E-4f);
_PS512_CONST(cephes_exp_p1, 1.3981999507E-3f);
_PS512_CONST(cephes_exp_p2, 8.3334519073E-3f);
_PS512_CONST(cephes_exp_p3, 4.1665795894E-2f);
_PS512_CONST(cephes_exp_p4, 1.6666665459E-1f);
_PS512_CONST(cephes_exp_p5, 5.0000001201E-1f);

INLINE __m512 exp512_ps(__m512 x)
{
	__m512 tmp = _mm512_setzero_ps(), fx;
	__m512i imm0;
	__m512 one = *(__m512*)_ps512_1;

	x = _mm512_min_ps(x, *(__m512*)_ps512_exp_hi);
	x = _mm512_max_ps(x, *(__m512*)_ps512_exp_lo);

	/* express exp(x) as exp(g + n*log(2)) */
	fx = _mm512_fmadd_ps(x, *(__m512*)_ps512_cephes_LOG2EF, *(__m512*)_ps512_0p5);

	tmp = _mm512_roundscale_ps(fx, _MM_FROUND_TO_NEG_INF);

	/* if greater, subtract 1 */
	__mmask16 mask = _mm512_cmp_ps_mask(tmp, fx, _CMP_GT_OQ);
	fx = _mm512_mask_sub_ps(tmp, mask, tmp, one);

	// x = x - fx * exp_C1
	x = _mm512_fnmadd_ps(fx, *(__m512*)_ps512_cephes_exp_C1, x);
	// x = x - fx * exp_C2
	x = _mm512_fnmadd_ps(fx, *(__m512*)_ps512_cephes_exp_C2, x);

	tmp = _mm512_mul_ps(x, x);

	__m512 y = *(__m512*)_ps512_cephes_exp_p0;
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_exp_p1);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_exp_p2);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_exp_p3);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_exp_p4);
	y = _mm512_fmadd_ps(y, x, *(__m512*)_ps512_cephes_exp_p5);
	y = _mm512_fmadd_ps(y, tmp, x);
	y = _mm512_add_ps(y, one);

	/* build 2^n */
	imm0 = _mm512_cvttps_epi32(fx);
	imm0 = _mm512_add_epi32(imm0, *(__m512i*)_pi32_512_0x7f);
	imm0 = _mm512_slli_epi32(imm0, 23);
	__m512 pow2n = _mm512_castsi512_ps(imm0);
	y = _mm512_mul_ps(y, pow2n);
	return y;
}

_PS512_CONST(tanh_hi, 9.0f);
_PS512_CONST(tanh_lo, -9.0f);

_PS512_CONST(cephes_tanh_p0, -2.76076847742355E-16f);
_PS512_CONST(cephes_tanh_p1, 2.00018790482477E-13f);
_PS512_CONST(cephes_tanh_p2, -8.60467152213735E-11f);
_PS512_CONST(cephes_tanh_p3, 5.12229709037114E-08f);
_PS512_CONST(cephes_tanh_p4, 1.48572235717979E-05f);
_PS512_CONST(cephes_tanh_p5, 6.37261928875436E-04f);
_PS512_CONST(cephes_tanh_p6, 4.89352455891786E-03f);

_PS512_CONST(cephes_tanh_p7, 1.19825839466702e-06f);
_PS512_CONST(cephes_tanh_p8, 1.18534705686654e-04f);
_PS512_CONST(cephes_tanh_p9, 2.26843463243900e-03f);

// an approximation of tanh
inline __m512 tanh512_ps(const __m512 x)
{
	__m512 value = x;
	value = _mm512_max_ps(*(__m512*)_ps512_tanh_lo, value);
	value = _mm512_min_ps(*(__m512*)_ps512_tanh_hi, value);

	__m512 value_squared = _mm512_mul_ps(value, value);

	__m512 p;
	p = _mm512_fmadd_ps(value_squared, *(__m512*)_ps512_cephes_tanh_p0, *(__m512*)_ps512_cephes_tanh_p1);
	p = _mm512_fmadd_ps(p, value_squared, *(__m512*)_ps512_cephes_tanh_p2);
	p = _mm512_fmadd_ps(p, value_squared, *(__m512*)_ps512_cephes_tanh_p3);
	p = _mm512_fmadd_ps(p, value_squared, *(__m512*)_ps512_cephes_tanh_p4);
	p = _mm512_fmadd_ps(p, value_squared, *(__m512*)_ps512_cephes_tanh_p5);
	p = _mm512_fmadd_ps(p, value_squared, *(__m512*)_ps512_cephes_tanh_p6);
	p = _mm512_mul_ps(p, value);

	__m512 q;
	q = _mm512_fmadd_ps(value_squared, *(__m512*)_ps512_cephes_tanh_p7, *(__m512*)_ps512_cephes_tanh_p8);
	q = _mm512_fmadd_ps(q, value_squared, *(__m512*)_ps512_cephes_tanh_p9);
	q = _mm512_fmadd_ps(q, value_squared, *(__m512*)_ps512_cephes_tanh_p6);

	__m512 dst = _mm512_div_ps(p, q);
	return dst;
}

_PS512_CONST(minus_cephes_DP1, -0.78515625f);
_PS512_CONST(minus_cephes_DP2, -2.4187564849853515625e-4f);
_PS512_CONST(minus_cephes_DP3, -3.77489497744594108e-8f);
_PS512_CONST(sincof_p0, -1.9515295891E-4f);
_PS512_CONST(sincof_p1, 8.3321608736E-3f);
_PS512_CONST(sincof_p2, -1.6666654611E-1f);
_PS512_CONST(coscof_p0, 2.443315711809948E-005f);
_PS512_CONST(coscof_p1, -1.388731625493765E-003f);
_PS512_CONST(coscof_p2, 4.166664568298827E-002f);
_PS512_CONST(cephes_FOPI, 1.27323954473516f); // 4 / M_PI

/* evaluation of 8 sines at onces using AVX intrisics

   The code is the exact rewriting of the cephes sinf function.
   Precision is excellent as long as x < 8192 (I did not bother to
   take into account the special handling they have for greater values
   -- it does not return garbage for arguments over 8192, though, but
   the extra precision is missing).

   Note that it is such that sinf((f32)M_PI) = 8.74e-8, which is the
   surprising but correct result.

*/
INLINE __m512 sin512_ps(__m512 x)
{   // any x
	__m512 xmm1, xmm2, xmm3, sign_bit, y;
	__m512i imm0, imm2;

	sign_bit = x;
	/* take the absolute value */
	x = _mm512_castsi512_ps(_mm512_and_epi32(_mm512_castps_si512(x), _mm512_set1_epi32(0x7fffffff)));
	/* extract the sign bit (upper one) */
	sign_bit = _mm512_and_ps(sign_bit, *(__m512*)_ps512_sign_mask);

	/* scale by 4/Pi */
	y = _mm512_mul_ps(x, *(__m512*)_ps512_cephes_FOPI);

	/* store the integer part of y in mm0 */
	imm2 = _mm512_cvttps_epi32(y);
	/* j=(j+1) & (~1) (see the cephes sources) */
	imm2 = _mm512_add_epi32(imm2, *(__m512i*)_pi32_512_1);
	imm2 = _mm512_and_si512(imm2, *(__m512i*)_pi32_512_inv1);
	y = _mm512_cvtepi32_ps(imm2);

	/* get the swap sign flag */
	imm0 = _mm512_and_si512(imm2, *(__m512i*)_pi32_512_4);
	imm0 = _mm512_slli_epi32(imm0, 29);
	/* get the polynom selection mask
	   there is one polynom for 0 <= x <= Pi/4
	   and another one for Pi/4<x<=Pi/2

	   Both branches will be computed.
	*/
	imm2 = _mm512_and_si512(imm2, *(__m512i*)_pi32_512_2);
	imm2 = _mm512_movm_epi32(_mm512_cmpeq_epi32_mask(imm2, *(__m512i*)_pi32_512_0));

	__m512 swap_sign_bit = _mm512_castsi512_ps(imm0);
	__m512 poly_mask = _mm512_castsi512_ps(imm2);
	sign_bit = _mm512_xor_ps(sign_bit, swap_sign_bit);

	/* The magic pass: "Extended precision modular arithmetic"
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(__m512*)_ps512_minus_cephes_DP1;
	xmm2 = *(__m512*)_ps512_minus_cephes_DP2;
	xmm3 = *(__m512*)_ps512_minus_cephes_DP3;
	x = _mm512_fmadd_ps(y, xmm1, x);
	x = _mm512_fmadd_ps(y, xmm2, x);
	x = _mm512_fmadd_ps(y, xmm3, x);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = *(__m512*)_ps512_coscof_p0;
	__m512 z = _mm512_mul_ps(x, x);

	y = _mm512_fmadd_ps(y, z, *(__m512*)_ps512_coscof_p1);
	y = _mm512_fmadd_ps(y, z, *(__m512*)_ps512_coscof_p2);
	y = _mm512_mul_ps(y, z);
	y = _mm512_mul_ps(y, z);
	// y = y - z * 0.5
	y = _mm512_fnmadd_ps(z, *(__m512*)_ps512_0p5, y);
	y = _mm512_add_ps(y, *(__m512*)_ps512_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	__m512 y2 = *(__m512*)_ps512_sincof_p0;
	y2 = _mm512_fmadd_ps(y2, z, *(__m512*)_ps512_sincof_p1);
	y2 = _mm512_fmadd_ps(y2, z, *(__m512*)_ps512_sincof_p2);
	y2 = _mm512_mul_ps(y2, z);
	y2 = _mm512_fmadd_ps(y2, x, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	y2 = _mm512_and_ps(xmm3, y2); //, xmm3);
	y = _mm512_andnot_ps(xmm3, y);
	y = _mm512_add_ps(y, y2);
	/* update the sign */
	y = _mm512_xor_ps(y, sign_bit);

	return y;
}

/* almost the same as sin_ps */
INLINE __m512 cos512_ps(__m512 x)
{   // any x
	__m512 xmm1, xmm2, xmm3, y;
	__m512i imm0, imm2;

	/* take the absolute value */
	x = _mm512_castsi512_ps(_mm512_and_epi32(_mm512_castps_si512(x), _mm512_set1_epi32(0x7fffffff)));

	/* scale by 4/Pi */
	y = _mm512_mul_ps(x, *(__m512*)_ps512_cephes_FOPI);

	/* store the integer part of y in mm0 */
	imm2 = _mm512_cvttps_epi32(y);
	/* j=(j+1) & (~1) (see the cephes sources) */
	imm2 = _mm512_add_epi32(imm2, *(__m512i*)_pi32_512_1);
	imm2 = _mm512_and_si512(imm2, *(__m512i*)_pi32_512_inv1);
	y = _mm512_cvtepi32_ps(imm2);
	imm2 = _mm512_sub_epi32(imm2, *(__m512i*)_pi32_512_2);

	/* get the swap sign flag */
	imm0 = _mm512_andnot_si512(imm2, *(__m512i*)_pi32_512_4);
	imm0 = _mm512_slli_epi32(imm0, 29);
	/* get the polynom selection mask */
	imm2 = _mm512_and_si512(imm2, *(__m512i*)_pi32_512_2);
	imm2 = _mm512_movm_epi32(_mm512_cmpeq_epi32_mask(imm2, *(__m512i*)_pi32_512_0));

	__m512 sign_bit = _mm512_castsi512_ps(imm0);
	__m512 poly_mask = _mm512_castsi512_ps(imm2);

	/* The magic pass: "Extended precision modular arithmetic"
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(__m512*)_ps512_minus_cephes_DP1;
	xmm2 = *(__m512*)_ps512_minus_cephes_DP2;
	xmm3 = *(__m512*)_ps512_minus_cephes_DP3;
	x = _mm512_fmadd_ps(y, xmm1, x);
	x = _mm512_fmadd_ps(y, xmm2, x);
	x = _mm512_fmadd_ps(y, xmm3, x);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = *(__m512*)_ps512_coscof_p0;
	__m512 z = _mm512_mul_ps(x, x);

	y = _mm512_fmadd_ps(y, z, *(__m512*)_ps512_coscof_p1);
	y = _mm512_fmadd_ps(y, z, *(__m512*)_ps512_coscof_p2);
	y = _mm512_mul_ps(y, z);
	y = _mm512_mul_ps(y, z);
	// y = y - z * 0.5
	y = _mm512_fnmadd_ps(z, *(__m512*)_ps512_0p5, y);
	y = _mm512_add_ps(y, *(__m512*)_ps512_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	__m512 y2 = *(__m512*)_ps512_sincof_p0;
	y2 = _mm512_mul_ps(y2, z);
	y2 = _mm512_add_ps(y2, *(__m512*)_ps512_sincof_p1);
	y2 = _mm512_mul_ps(y2, z);
	y2 = _mm512_add_ps(y2, *(__m512*)_ps512_sincof_p2);
	y2 = _mm512_mul_ps(y2, z);
	y2 = _mm512_mul_ps(y2, x);
	y2 = _mm512_add_ps(y2, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	y2 = _mm512_and_ps(xmm3, y2); //, xmm3);
	y = _mm512_andnot_ps(xmm3, y);
	y = _mm512_add_ps(y, y2);
	/* update the sign */
	y = _mm512_xor_ps(y, sign_bit);

	return y;
}

/* since sin512_ps and cos512_ps are almost identical, sincos512_ps could replace both of them..
   it is almost as fast, and gives you a free cosine with your sine */
INLINE void sincos512_ps(__m512 x, __m512* s, __m512* c)
{
	__m512 xmm1, xmm2, xmm3, sign_bit_sin, y;
	__m512i imm0, imm2, imm4;

	sign_bit_sin = x;
	/* take the absolute value */
	x = _mm512_castsi512_ps(_mm512_and_epi32(_mm512_castps_si512(x), _mm512_set1_epi32(0x7fffffff)));
	/* extract the sign bit (upper one) */
	sign_bit_sin = _mm512_and_ps(sign_bit_sin, *(__m512*)_ps512_sign_mask);

	/* scale by 4/Pi */
	y = _mm512_mul_ps(x, *(__m512*)_ps512_cephes_FOPI);

	/* store the integer part of y in imm2 */
	imm2 = _mm512_cvttps_epi32(y);

	/* j=(j+1) & (~1) (see the cephes sources) */
	imm2 = _mm512_add_epi32(imm2, *(__m512i*)_pi32_512_1);
	imm2 = _mm512_and_si512(imm2, *(__m512i*)_pi32_512_inv1);

	y = _mm512_cvtepi32_ps(imm2);
	imm4 = imm2;

	/* get the swap sign flag for the sine */
	imm0 = _mm512_and_si512(imm2, *(__m512i*)_pi32_512_4);
	imm0 = _mm512_slli_epi32(imm0, 29);
	//__m512 swap_sign_bit_sin = _mm512_castsi512_ps(imm0);

	/* get the polynom selection mask for the sine*/
	imm2 = _mm512_and_si512(imm2, *(__m512i*)_pi32_512_2);
	imm2 = _mm512_movm_epi32(_mm512_cmpeq_epi32_mask(imm2, *(__m512i*)_pi32_512_0));
	//__m512 poly_mask = _mm512_castsi512_ps(imm2);

	__m512 swap_sign_bit_sin = _mm512_castsi512_ps(imm0);
	__m512 poly_mask = _mm512_castsi512_ps(imm2);

	/* The magic pass: "Extended precision modular arithmetic"
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(__m512*)_ps512_minus_cephes_DP1;
	xmm2 = *(__m512*)_ps512_minus_cephes_DP2;
	xmm3 = *(__m512*)_ps512_minus_cephes_DP3;
	x = _mm512_fmadd_ps(y, xmm1, x);
	x = _mm512_fmadd_ps(y, xmm2, x);
	x = _mm512_fmadd_ps(y, xmm3, x);

	imm4 = _mm512_sub_epi32(imm4, *(__m512i*)_pi32_512_2);
	imm4 = _mm512_andnot_si512(imm4, *(__m512i*)_pi32_512_4);
	imm4 = _mm512_slli_epi32(imm4, 29);

	__m512 sign_bit_cos = _mm512_castsi512_ps(imm4);

	sign_bit_sin = _mm512_xor_ps(sign_bit_sin, swap_sign_bit_sin);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	__m512 z = _mm512_mul_ps(x, x);
	y = *(__m512*)_ps512_coscof_p0;

	y = _mm512_fmadd_ps(y, z, *(__m512*)_ps512_coscof_p1);
	y = _mm512_fmadd_ps(y, z, *(__m512*)_ps512_coscof_p2);
	y = _mm512_mul_ps(y, z);
	y = _mm512_mul_ps(y, z);
	// y = y - z * 0.5
	y = _mm512_fnmadd_ps(z, *(__m512*)_ps512_0p5, y);
	y = _mm512_add_ps(y, *(__m512*)_ps512_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	__m512 y2 = *(__m512*)_ps512_sincof_p0;
	y2 = _mm512_fmadd_ps(y2, z, *(__m512*)_ps512_sincof_p1);
	y2 = _mm512_fmadd_ps(y2, z, *(__m512*)_ps512_sincof_p2);
	y2 = _mm512_mul_ps(y2, z);
	y2 = _mm512_fmadd_ps(y2, x, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	__m512 ysin2 = _mm512_and_ps(xmm3, y2);
	__m512 ysin1 = _mm512_andnot_ps(xmm3, y);
	y2 = _mm512_sub_ps(y2, ysin2);
	y = _mm512_sub_ps(y, ysin1);

	xmm1 = _mm512_add_ps(ysin1, ysin2);
	xmm2 = _mm512_add_ps(y, y2);

	/* update the sign */
	*s = _mm512_xor_ps(xmm1, sign_bit_sin);
	*c = _mm512_xor_ps(xmm2, sign_bit_cos);
}

INLINE __m512 tan512_ps(__m512 x)
{
	__m512 ysin, ycos;
	__m512 eps = _mm512_set1_ps(1E-8f);
	sincos512_ps(x, &ysin, &ycos);
	__mmask16 mask = _mm512_cmp_ps_mask(ycos, _mm512_setzero_ps(), _CMP_EQ_OS);
	ycos = _mm512_mask_add_ps(ycos, mask, ycos, eps);
	__m512 ytan = _mm512_div_ps(ysin, ycos);
	return ytan;
}

INLINE __m512 pow512_ps(__m512 a, __m512 b)
{
	// pow(x, m) = exp(m * log(x))
	return exp512_ps(_mm512_mul_ps(b, log512_ps(a)));
}

INLINE __m512 asin512_ps(__m512 x)
{
	const __m512 magic_negative_zero = _mm512_set1_ps(-0.0f);
	const __m512 magic_half_one = _mm512_set1_ps(0.5f);
	const __m512 magic_one = _mm512_set1_ps(1.0f);
	const __m512 magic_a4 = _mm512_set1_ps(0.023994016f);
	const __m512 magic_a5 = _mm512_set1_ps(0.042417344f);
	const __m512 magic_a2 = _mm512_set1_ps(0.07494697f);
	const __m512 magic_a3 = _mm512_set1_ps(0.045520633f);
	const __m512 magic_a0 = _mm512_set1_ps(1.0f);
	const __m512 magic_a1 = _mm512_set1_ps(0.166667819f);
	const __m512 magic_half_pi = _mm512_set1_ps(1.5707964f);
	const __m512 magic_three = _mm512_set1_ps(3.0f);

	// negative_mask = magic_negative_zero && x;
	__m512 negative_mask = _mm512_and_ps(magic_negative_zero, x);

	// absolute = abs(x);
	__m512 absolute = _mm512_andnot_ps(magic_negative_zero, x);

	// Reference: https://en.wikipedia.org/wiki/Small-angle_approximation

	// is_small_input = (absolute <= 0.5f);
	__mmask16 is_small_input = _mm512_cmp_ps_mask(
		absolute,
		magic_half_one,
		_CMP_LE_OQ);

	// is_big_input = (is_small_input ? 0.0f : 1.0f);
	__m512 is_big_input = _mm512_maskz_mov_ps(~is_small_input, magic_one);

	// big_input_approx = sqrt(0.5f * (1 - absolute));
	__m512 big_input_approx = _mm512_sqrt_ps(_mm512_mul_ps(
		magic_half_one,
		_mm512_sub_ps(magic_one, absolute)));

	// input_approx = (is_small_input ? absolute : big_input_approx);
	__m512 input_approx = _mm512_mask_mov_ps(
		big_input_approx,
		is_small_input,
		absolute);

	// square_of_input_approx = input_approx * input_approx;
	__m512 square_of_input_approx = _mm512_mul_ps(input_approx, input_approx);

	// fourth_power_of_input_approx =
	//     square_of_input_approx * square_of_input_approx;
	__m512 fourth_power_of_input_approx = _mm512_mul_ps(
		square_of_input_approx, square_of_input_approx);

	// TODO: Need more explanations.
	// x1 = ((fourth_power_of_input_approx * magic_a4) + magic_a2);
	// x2 = ((fourth_power_of_input_approx * magic_a5) + magic_a3);
	// x3 = ((fourth_power_of_input_approx * x1) + magic_a0);
	// x4 = ((fourth_power_of_input_approx * x2) + magic_a1);
	// output_approx = ((square_of_input_approx * x4) + x3);
	__m512 output_approx = _mm512_fmadd_ps(
		square_of_input_approx,
		_mm512_fmadd_ps(
			fourth_power_of_input_approx,
			_mm512_fmadd_ps(
				fourth_power_of_input_approx,
				magic_a5,
				magic_a3),
			magic_a1),
		_mm512_fmadd_ps(
			fourth_power_of_input_approx,
			_mm512_fmadd_ps(
				fourth_power_of_input_approx,
				magic_a4,
				magic_a2),
			magic_a0));

	// TODO: Need more explanations.
	// x1 = ((0.5 * PI) * is_big_input);
	// x2 = (output_approx * input_approx);
	// x3 = (-(3.0f * is_big_input) + 1.0f);
	// final_approx = ((x2 * x3) + x1);
	__m512 final_approx = _mm512_fmadd_ps(
		_mm512_mul_ps(output_approx, input_approx),
		_mm512_fnmadd_ps(magic_three, is_big_input, magic_one),
		_mm512_mul_ps(magic_half_pi, is_big_input));

	// return (final_approx || negative_mask);
	return _mm512_or_ps(final_approx, negative_mask);
}

INLINE __m512 acos512_ps(__m512 x)
{
	const __m512 magic_negative_zero = _mm512_set1_ps(-0.0f);
	const __m512 magic_zero = _mm512_set1_ps(0.0f);
	const __m512 magic_half_one = _mm512_set1_ps(0.5f);
	const __m512 magic_one = _mm512_set1_ps(1.0f);
	const __m512 magic_a4 = _mm512_set1_ps(0.023994016f);
	const __m512 magic_a5 = _mm512_set1_ps(0.042417344f);
	const __m512 magic_a2 = _mm512_set1_ps(0.07494697f);
	const __m512 magic_a3 = _mm512_set1_ps(0.045520633f);
	const __m512 magic_a0 = _mm512_set1_ps(1.0f);
	const __m512 magic_a1 = _mm512_set1_ps(0.166667819f);
	const __m512 magic_half_pi = _mm512_set1_ps(1.5707964f);
	const __m512 magic_pi = _mm512_set1_ps(3.1415927f);

	// negative_mask = magic_negative_zero && x;
	__m512 negative_mask = _mm512_and_ps(magic_negative_zero, x);

	// absolute = abs(x);
	__m512 absolute = _mm512_andnot_ps(magic_negative_zero, x);

	// Reference: https://en.wikipedia.org/wiki/Small-angle_approximation

	// is_small_input = (absolute <= 0.5f);
	__mmask16 is_small_input = _mm512_cmp_ps_mask(
		absolute,
		magic_half_one,
		_CMP_LE_OQ);

	// big_input_approx = sqrt(0.5f * (1 - absolute));
	__m512 big_input_approx = _mm512_sqrt_ps(_mm512_mul_ps(
		magic_half_one,
		_mm512_sub_ps(magic_one, absolute)));

	// input_approx = (is_small_input ? absolute : big_input_approx);
	__m512 input_approx = _mm512_mask_mov_ps(
		big_input_approx,
		is_small_input,
		absolute);

	// square_of_input_approx = input_approx * input_approx;
	__m512 square_of_input_approx = _mm512_mul_ps(input_approx, input_approx);

	// fourth_power_of_input_approx =
	//     square_of_input_approx * square_of_input_approx;
	__m512 fourth_power_of_input_approx = _mm512_mul_ps(
		square_of_input_approx, square_of_input_approx);

	// TODO: Need more explanations.
	// x1 = ((fourth_power_of_input_approx * magic_a4) + magic_a2);
	// x2 = ((fourth_power_of_input_approx * magic_a5) + magic_a3);
	// x3 = ((fourth_power_of_input_approx * x1) + magic_a0);
	// x4 = ((fourth_power_of_input_approx * x2) + magic_a1);
	// output_approx = ((square_of_input_approx * x4) + x3);
	__m512 output_approx = _mm512_fmadd_ps(
		square_of_input_approx,
		_mm512_fmadd_ps(
			fourth_power_of_input_approx,
			_mm512_fmadd_ps(
				fourth_power_of_input_approx,
				magic_a5,
				magic_a3),
			magic_a1),
		_mm512_fmadd_ps(
			fourth_power_of_input_approx,
			_mm512_fmadd_ps(
				fourth_power_of_input_approx,
				magic_a4,
				magic_a2),
			magic_a0));

	// TODO: Need more explanations.
	// x1 = (output_approx * input_approx);
	__m512 x1 = _mm512_mul_ps(output_approx, input_approx);

	// TODO: Need more explanations.
	// small_final_approx = ((0.5 * PI) - (x1 | negative_mask));
	__m512 small_final_approx = _mm512_sub_ps(
		magic_half_pi,
		_mm512_or_ps(x1, negative_mask));

	// TODO: Need more explanations.
	// big_final_approx = (((x < 0.0f) & PI) + ((x1 * 2) | negative_mask));
	__m512 big_final_approx = _mm512_add_ps(
		_mm512_maskz_mov_ps(
			_mm512_cmp_ps_mask(x, magic_zero, _CMP_LT_OQ),
			magic_pi),
		_mm512_or_ps(_mm512_add_ps(x1, x1), negative_mask));

	// return (is_small_input ? small_final_approx : big_final_approx);
	return _mm512_mask_mov_ps(
		big_final_approx,
		is_small_input,
		small_final_approx);
}

INLINE __m512 atan512_ps(__m512 x)
{
	const __m512 magic_negative_zero = _mm512_set1_ps(-0.0f);
	const __m512 magic_one = _mm512_set1_ps(1.0f);
	const __m512 magic_negative_one = _mm512_set1_ps(-1.0f);
	const __m512 magic_half_pi = _mm512_set1_ps(1.5707964f);
	const __m512 magic_a0 = _mm512_set1_ps(1.0f);
	const __m512 magic_a1 = _mm512_set1_ps(-0.33333072f);
	const __m512 magic_a2 = _mm512_set1_ps(0.1999262f);
	const __m512 magic_a3 = _mm512_set1_ps(-0.14203644f);
	const __m512 magic_a4 = _mm512_set1_ps(0.10640934f);
	const __m512 magic_a5 = _mm512_set1_ps(-0.07504295f);
	const __m512 magic_a6 = _mm512_set1_ps(0.04269152f);
	const __m512 magic_a7 = _mm512_set1_ps(-0.01606863f);
	const __m512 magic_a8 = _mm512_set1_ps(0.0028498897f);

	// negative_mask = magic_negative_zero && x;
	__m512 negative_mask = _mm512_and_ps(magic_negative_zero, x);

	// absolute = abs(x);
	__m512 absolute = _mm512_andnot_ps(magic_negative_zero, x);

	// Reference: https://en.wikipedia.org/wiki/Small-angle_approximation

	// is_small_input = (1.0f < absolute);
	__mmask16 is_small_input = _mm512_cmp_ps_mask(
		magic_one,
		absolute,
		_CMP_LT_OQ);

	// x1 = (is_small_input ? -1.0f : absolute);
	// x2 = (is_small_input ? absolute : 1.0f)
	// input_approx = x1 / x2;
	__m512 input_approx = _mm512_div_ps(
		_mm512_mask_mov_ps(absolute, is_small_input, magic_negative_one),
		_mm512_mask_mov_ps(magic_one, is_small_input, absolute));

	// square_of_input_approx = input_approx * input_approx;
	__m512 square_of_input_approx = _mm512_mul_ps(input_approx, input_approx);

	// fourth_power_of_input_approx =
	//     square_of_input_approx * square_of_input_approx;
	__m512 fourth_power_of_input_approx = _mm512_mul_ps(
		square_of_input_approx, square_of_input_approx);

	// TODO: Need more explanations.
	// x1 = ((fourth_power_of_input_approx * magic_a7) + magic_a5);
	// x2 = ((fourth_power_of_input_approx * magic_a8) + magic_a6);
	// x3 = ((fourth_power_of_input_approx * x1) + magic_a3);
	// x4 = ((fourth_power_of_input_approx * x2) + magic_a4);
	// x5 = ((fourth_power_of_input_approx * x3) + magic_a1);
	// x6 = ((fourth_power_of_input_approx * x4) + magic_a2);
	// x7 = ((fourth_power_of_input_approx * x6) + magic_a0);
	// output_approx = ((square_of_input_approx * x5) + x7);
	__m512 output_approx = _mm512_fmadd_ps(
		square_of_input_approx,
		_mm512_fmadd_ps(
			fourth_power_of_input_approx,
			_mm512_fmadd_ps(
				fourth_power_of_input_approx,
				_mm512_fmadd_ps(
					fourth_power_of_input_approx,
					magic_a7,
					magic_a5),
				magic_a3),
			magic_a1),
		_mm512_fmadd_ps(
			fourth_power_of_input_approx,
			_mm512_fmadd_ps(
				fourth_power_of_input_approx,
				_mm512_fmadd_ps(
					fourth_power_of_input_approx,
					_mm512_fmadd_ps(
						fourth_power_of_input_approx,
						magic_a8,
						magic_a6),
					magic_a4),
				magic_a2),
			magic_a0));

	// TODO: Need more explanations.
	// x1 = (output_approx * input_approx);
	// if (is_small_input) x1 += (0.5 * PI);
	// return (negative_mask ? -x1 : x1);
	return _mm512_or_ps(
		_mm512_add_ps(
			_mm512_mul_ps(output_approx, input_approx),
			_mm512_maskz_mov_ps(is_small_input, magic_half_pi)),
		negative_mask);
}

INLINE __m512 atan2512_ps(__m512 y, __m512 x)
{
	// Reference: https://mazzo.li/posts/vectorized-atan2.html
	const __m512 magic_zero = _mm512_set1_ps(0.0f);
	const __m512 magic_negative_zero = _mm512_set1_ps(-0.0f);
	const __m512 magic_pi = _mm512_set1_ps(3.1415927f);
	const __m512 magic_half_pi = _mm512_set1_ps(1.5707964f);

	// not_equal_zero_x = (x != 0.0f);
	__mmask16 not_equal_zero_x = _mm512_cmp_ps_mask(
		x,
		magic_zero,
		_CMP_NEQ_OQ);

	// not_equal_zero_y = (y != 0.0f);
	__mmask16 not_equal_zero_y = _mm512_cmp_ps_mask(
		y,
		magic_zero,
		_CMP_NEQ_OQ);

	// normal_mode = ((x != 0.0f) & (y != 0.0f));
	__mmask16 normal_mode = (not_equal_zero_x & not_equal_zero_y);

	// negative_mask_x = magic_negative_zero && x;
	__m512 negative_mask_x = _mm512_and_ps(magic_negative_zero, x);

	// negative_mask_y = magic_negative_zero && y;
	__m512 negative_mask_y = _mm512_and_ps(magic_negative_zero, y);

	// pi_additions = ((x < 0.0f) ? ((y < 0.0f) ? -PI : PI) : 0.0f);
	__m512 pi_additions = _mm512_mask_mov_ps(
		magic_zero,
		_mm512_cmp_ps_mask(x, magic_zero, _CMP_LT_OQ),
		_mm512_mask_mov_ps(
			magic_pi,
			_mm512_cmp_ps_mask(y, magic_zero, _CMP_LT_OQ),
			_mm512_or_ps(magic_negative_zero, magic_pi)));

	// normal_result = (atan(y / x) + pi_additions);
	__m512 normal_result = _mm512_add_ps(
		atan512_ps(_mm512_div_ps(y, x)),
		pi_additions);

	// negative_mask_full_x = ((negative_mask_x | PI) < 0.0f);
	__mmask16 negative_mask_full_x = _mm512_cmp_ps_mask(
		_mm512_or_ps(negative_mask_x, magic_pi),
		magic_zero,
		_CMP_LT_OQ);

	// x1 = (negative_mask_y ? -(0.5 * PI) : (0.5 * PI));
	// x2 = (negative_mask_full_x ? PI : 0.0f);
	// special_result = ((y != 0.0f) ? x1 : x2);
	__m512 special_result = _mm512_mask_mov_ps(
		_mm512_mask_mov_ps(magic_zero, negative_mask_full_x, magic_pi),
		not_equal_zero_y,
		_mm512_or_ps(negative_mask_y, magic_half_pi));

	// return (normal_mode ? normal_result : special_result);
	return _mm512_mask_mov_ps(special_result, normal_mode, normal_result);
}

INLINE __m512 abs512_ps(__m512 x)
{
	const __m512 abs_mask = _mm512_castsi512_ps(_mm512_set1_epi32(0x7fffffff));
	return _mm512_and_ps(abs_mask, x);
}

//https://github.com/RJVB/MacSTL/blob/master/test/sse_mathfun.h
#define _PD_CONST(Name, Val) static constexpr ALIGN16_BEG double _pd_##Name[2] ALIGN16_END = { Val, Val }
#define _PD_CONST_TYPE(Name, Type, Val) static constexpr ALIGN16_BEG Type _pd_##Name[2] ALIGN16_END = { ((Type)Val), ((Type)Val) }
_PD_CONST(1, 1.0);
_PD_CONST(_1, -1.0);
_PD_CONST(0p5, 0.5);
_PD_CONST(minus_cephes_DP1, -0.78515625);
_PD_CONST(minus_cephes_DP2, -2.4187564849853515625e-4);
_PD_CONST(minus_cephes_DP3, -3.77489497744594108e-8);
_PD_CONST(sincof_p0, -1.9515295891E-4);
_PD_CONST(sincof_p1, 8.3321608736E-3);
_PD_CONST(sincof_p2, -1.6666654611E-1);
_PD_CONST(coscof_p0, 2.443315711809948E-005);
_PD_CONST(coscof_p1, -1.388731625493765E-003);
_PD_CONST(coscof_p2, 4.166664568298827E-002);
_PD_CONST(cephes_FOPI, 1.27323954473516); // 4 / M_PI
_PD_CONST_TYPE(sign_mask, long long, 0x8000000000000000LL);
_PD_CONST_TYPE(inv_sign_mask, long long, ~0x8000000000000000LL);

INLINE void sincos_pd(__m128d x, __m128d* s, __m128d* c)noexcept
{
	__m128d xmm1, xmm2, sign_bit_sin, y, y2, z, swap_sign_bit_sin, poly_mask;
	__m128d sign_bit_cos;
	__m128i emm2;
	sign_bit_sin = x;
	/* take the absolute value */
	x = _mm_and_pd(x, *(__m128d*)_pd_inv_sign_mask);
	/* extract the sign bit (upper one) */
	sign_bit_sin = _mm_and_pd(sign_bit_sin, *(__m128d*)_pd_sign_mask);

	/* scale by 4/Pi */
	y = _mm_mul_pd(x, *(__m128d*)_pd_cephes_FOPI);

	/* store the integer part of y in emm2 */
	emm2 = _mm_cvttpd_epi32(y);

	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = _mm_and_si128(_mm_add_epi64(_mm_cvttpd_epi32(y), *(__m128i*)_pi32_1), *(__m128i*)_pi32_inv1);
	y = _mm_cvtepi32_pd(emm2);

	/* get the swap sign flag for the sine */
	{
		__m128 sss = _mm_castsi128_ps(_mm_slli_epi32(_mm_and_si128(emm2, *(__m128i*)_pi32_4), 29));
		f32* fsss = ((f32*)&sss);
		swap_sign_bit_sin = _mm_setr_pd(fsss[0], fsss[1]);
	}

	/* get the polynom selection mask for the sine*/
	{
		__m128 pm = _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_and_si128(emm2, *(__m128i*)_pi32_2), _mm_setzero_si128()));
		f32* fpm = ((f32*)&pm);
		poly_mask = _mm_setr_pd(fpm[0], fpm[1]);
	}

	/* The magic pass: "Extended precision modular arithmetic"
	 x = ((x - y * DP1) - y * DP2) - y * DP3; */
#ifdef __GNUC__
	x += y * (*(__m128d*)_pd_minus_cephes_DP1 + *(__m128d*)_pd_minus_cephes_DP2 + *(__m128d*)_pd_minus_cephes_DP3);
#else
	x = _mm_add_pd(x, _mm_mul_pd(y, _mm_add_pd(_mm_add_pd(*(__m128d*)_pd_minus_cephes_DP1, *(__m128d*)_pd_minus_cephes_DP2), *(__m128d*)_pd_minus_cephes_DP3)));
#endif

	{
		__m128 sbc = _mm_castsi128_ps(_mm_slli_epi32(_mm_andnot_si128(_mm_sub_epi32(emm2, *(__m128i*)_pi32_2), *(__m128i*)_pi32_4), 29));
		f32* fsbc = ((f32*)&sbc);
		sign_bit_cos = _mm_setr_pd(fsbc[0], fsbc[1]);
	}

	sign_bit_sin = _mm_xor_pd(sign_bit_sin, swap_sign_bit_sin);


	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
#ifdef __GNUC__
	z = x * x;
	y = ((((*(__m128d*)_pd_coscof_p0) * z + *(__m128d*)_pd_coscof_p1) * z + *(__m128d*)_pd_coscof_p2) * z
		 - *(__m128d*)_pd_0p5) * z + *(__m128d*)_pd_1;
#else
	z = _mm_mul_pd(x, x);
	y = _mm_add_pd(
		_mm_mul_pd(
			_mm_sub_pd(
				_mm_mul_pd(
					_mm_add_pd(
						_mm_mul_pd(
							_mm_add_pd(
								_mm_mul_pd(*(__m128d*)_pd_coscof_p0, z),
								*(__m128d*)_pd_coscof_p1),
							z),
						*(__m128d*)_pd_coscof_p2),
					z),
				*(__m128d*)_pd_0p5),
			z),
		*(__m128d*)_pd_1);
#endif

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

#ifdef __GNUC__
	y2 = ((((((*(__m128d*)_pd_sincof_p0) * z) + *(__m128d*)_pd_sincof_p1) * z) + *(__m128d*)_pd_sincof_p2) * z
		  + *(__m128d*)_pd_1) * x;
#else
	y2 = _mm_mul_pd(
		_mm_add_pd(
			_mm_mul_pd(
				_mm_add_pd(
					_mm_mul_pd(
						_mm_add_pd(
							_mm_mul_pd(*(__m128d*)_pd_sincof_p0, z),
							*(__m128d*)_pd_sincof_p1),
						z),
					*(__m128d*)_pd_sincof_p2),
				z),
			*(__m128d*)_pd_1),
		x);
#endif

	/* select the correct result from the two polynoms */
	{
#if defined(__GNUC__0) && !defined(__MINGW32__)
		xmm1 = _mm_andnot_pd(poly_mask, y) + (poly_mask & y2);
		xmm2 = y + y2 - xmm1;
		/* update the sign */
		*s = xmm1 | sign_bit_sin;
		*c = xmm2 | sign_bit_cos;
#else
		xmm1 = _mm_add_pd(_mm_andnot_pd(poly_mask, y), _mm_and_pd(poly_mask, y2));
		xmm2 = _mm_sub_pd(_mm_add_pd(y, y2), xmm1);
		/* update the sign */
		*s = _mm_xor_pd(xmm1, sign_bit_sin);
		*c = _mm_xor_pd(xmm2, sign_bit_cos);
#endif
	}
}