#pragma once
#include "Abs.h"
#include "Compact.h"
#include "Compiler.h"
#include "ConceptArithmetic.h"
#include "ConceptCvrefOf.h"
#include "ConceptForEach.h"
#include "Consteval.h"
#include "ElementAt.h"
#include "Fold.h"
#include "Identity.h"
#include "Inline.h"
#include "Iota.h"
#include "IsFinite.h"
#include "Like.h"
#include "MacroIf.h"
#include "MacroMovForward.h"
#include "OddEven.h"
#include "OptionalImpl.h"
#include "SignedIntermediateValue.h"
#include "Sqrt.h"
#include "StaticAssert.h"
#include <array>
#include <utility>
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (__FINITE_MATH_ONLY__==1)
	#error delete those flags
#endif
namespace details
{
	struct IsFiniteImpl
	{
		INLINE static constexpr bool operator()(auto&& x)noexcept { return Math::isFinite(FWD(x)); }
	};
}
namespace Math
{
	template<$UInt T>
	struct TensorIndex
	{
		T index;
		INLINE constexpr explicit TensorIndex(T index)noexcept: index{ index } {}
		template<class Container>
		INLINE friend constexpr Like<Container, decltype(std::declval<Container>()[T{}])>operator|(Container&& l, TensorIndex r)noexcept
		{
			return static_cast<Like<Container, decltype(std::declval<Container>()[T{}])>>(l.m_data[r.index]);
		}
	};
#if TEMPLATE_FOR
	template<class, usize...>struct Tensor;//scaler: should not instantiate
#else
	template<class...>struct TensorImpl;
	template<class T, usize...Ns>using Tensor = TensorImpl<T, std::index_sequence<Ns...>, std::make_index_sequence<std::array{ Ns... }[0]>>;
	template<class T>struct TensorImpl<T, std::index_sequence<>, std::index_sequence<>>;//scaler: should not instantiate
#endif

	template<class T, usize N>using Vector = Tensor<T, N>;
	template<class T>using Vector2 = Vector<T, 2>;using float2 = Vector2<f32>;
	template<class T>using Vector3 = Vector<T, 3>;using float3 = Vector3<f32>;
	template<class T>using Vector4 = Vector<T, 4>;using float4 = Vector4<f32>;

	template<class T, usize R, usize C>using Matrix = Tensor<T, R, C>;
	template<class T, usize R>using SquareMatrix = Matrix<T, R, R>;
	template<class T>using Matrix2x2 = Matrix<T, 2, 2>; using float2x2 = Matrix2x2<f32>;
	template<class T>using Matrix2x3 = Matrix<T, 2, 3>; using float2x3 = Matrix2x3<f32>;
	template<class T>using Matrix3x2 = Matrix<T, 3, 2>; using float3x2 = Matrix3x2<f32>;
	template<class T>using Matrix3x3 = Matrix<T, 3, 3>; using float3x3 = Matrix3x3<f32>;
	template<class T>using Matrix3x4 = Matrix<T, 3, 4>; using float3x4 = Matrix3x4<f32>;
	template<class T>using Matrix4x3 = Matrix<T, 4, 3>; using float4x3 = Matrix4x3<f32>;
	template<class T>using Matrix4x4 = Matrix<T, 4, 4>; using float4x4 = Matrix4x4<f32>;
	
	template<class...>struct DeterminantImpl;
	template<class T, auto...I, auto...ExcludedX, auto...ExcludedY>
	struct DeterminantImpl<T, std::index_sequence<I...>, std::index_sequence<ExcludedX...>, std::index_sequence<ExcludedY...>>
	{
		static constexpr auto minor_size{ compact<sizeof...(I)> };
		static constexpr auto N{ compact<minor_size + sizeof...(ExcludedX)> };//original size
		template<usize virtualX>static constexpr Compact<N - 1> pX{ []static{Compact<N - 1> result{}; for (usize i{};; ++result)if ((...&& (result != ExcludedX))) { if (i == virtualX)return result; ++i; }}() };
		template<usize virtualY>static constexpr Compact<N - 1> pY{ []static{Compact<N - 1> result{}; for (usize i{};; ++result)if ((...&& (result != ExcludedY))) { if (i == virtualY)return result; ++i; }}() };
		static constexpr T operator()($CvrefOf<SquareMatrix<T, N>> auto&& m)noexcept
		{
			if constexpr (minor_size == 1)
				return m[pX<0>][pY<0>];
			else if constexpr (minor_size == 2)
				return m[pX<0>][pY<0>] * m[pX<1>][pY<1>] - m[pX<0>][pY<1>] * m[pX<1>][pY<0>];
			else if constexpr (minor_size == 3)
			{
				const T _00{ m[pX<0>][pY<0>] }, _01{ m[pX<0>][pY<1>] }, _02{ m[pX<0>][pY<2>] };
				const T _10{ m[pX<1>][pY<0>] }, _11{ m[pX<1>][pY<1>] }, _12{ m[pX<1>][pY<2>] };
				const T _20{ m[pX<2>][pY<0>] }, _21{ m[pX<2>][pY<1>] }, _22{ m[pX<2>][pY<2>] };
				return _00 * (_11 * _22 - _12 * _21) - _01 * (_10 * _22 - _12 * _20) + _02 * (_10 * _21 - _11 * _20);
			}
			else if constexpr (minor_size == 4)
			{
				const T _00{ m[pX<0>][pY<0>] }, _01{ m[pX<0>][pY<1>] }, _02{ m[pX<0>][pY<2>] }, _03{ m[pX<0>][pY<3>] };
				const T _10{ m[pX<1>][pY<0>] }, _11{ m[pX<1>][pY<1>] }, _12{ m[pX<1>][pY<2>] }, _13{ m[pX<1>][pY<3>] };
				const T _20{ m[pX<2>][pY<0>] }, _21{ m[pX<2>][pY<1>] }, _22{ m[pX<2>][pY<2>] }, _23{ m[pX<2>][pY<3>] };
				const T _30{ m[pX<3>][pY<0>] }, _31{ m[pX<3>][pY<1>] }, _32{ m[pX<3>][pY<2>] }, _33{ m[pX<3>][pY<3>] };
				const T m00{ _11 * (_22 * _33 - _23 * _32) - _12 * (_21 * _33 - _23 * _31) + _13 * (_21 * _32 - _22 * _31) };
				const T m01{ _10 * (_22 * _33 - _23 * _32) - _12 * (_20 * _33 - _23 * _30) + _13 * (_20 * _32 - _22 * _30) };
				const T m02{ _10 * (_21 * _33 - _23 * _31) - _11 * (_20 * _33 - _23 * _30) + _13 * (_20 * _31 - _21 * _30) };
				const T m03{ _10 * (_21 * _32 - _22 * _31) - _11 * (_20 * _32 - _22 * _30) + _12 * (_20 * _31 - _21 * _30) };
				return _00 * m00 - _01 * m01 + _02 * m02 - _03 * m03;
			}
			else if constexpr (minor_size <= IF(CLANG, 7, 6))
				return (... + (SignedIntermediateValue<T, isEven(I)>{m[pX<0>][pY<I>] * DeterminantImpl<T, std::make_index_sequence<minor_size - 1>, std::index_sequence<pX<0>, ExcludedX...>, std::index_sequence<pY<I>, ExcludedY...>>::operator()(m)}));
			else
#if TEMPLATE_FOR
			{
				if constexpr (std::is_rvalue_reference_v<decltype(m)>)
				{
					T sign{ 1 }, prev{ 1 };
					template for (constexpr auto i : iota<minor_size - 1>)
					{
						auto pivot{ pX<i> };//physical
						T maxv{ Math::abs(m[pX<i>][pY<i>]) };
						template for (constexpr auto k : iota<i + 1, minor_size>)
							if (const T val{ Math::abs(m[pX<k>][pY<i>]) }; val > maxv)
							{
								maxv = val;
								pivot = pX<k>;
							}
						if (maxv == T{}) return {};
						if (pivot != pX<i>)
						{
							sign = -sign;
							template for (constexpr auto j : iota<i, minor_size>)
								std::swap(m[pX<i>][pY<j>], m[pivot][pY<j>]);
						}
						template for (constexpr auto j : iota<i + 1, minor_size>)
							template for (constexpr auto k : iota<i + 1, minor_size>)
								m[pX<j>][pY<k>] = (m[pX<j>][pY<k>] * m[pX<i>][pY<i>] - m[pX<j>][pY<i>] * m[pX<i>][pY<k>]) / prev;
						prev = m[pX<i>][pY<i>];
					}
					return sign * m[pX<minor_size - 1>][pY<minor_size - 1>];
				}
				else
				{
					T a[minor_size][minor_size];
					template for (constexpr auto vi : iota<minor_size>)
						template for (constexpr auto vj : iota<minor_size>)
							a[vi][vj] = m[pX<vi>][pY<vj>];
					T sign{ 1 }, prev{ 1 };
					template for (constexpr auto i : iota<minor_size - 1>)
					{
						auto pivot{ i };
						T maxv{ Math::abs(a[i][i]) };
						template for (constexpr auto k : iota<i + 1, minor_size>)
							if (const T val{ Math::abs(a[k][i]) }; val > maxv)
							{
								maxv = val;
								pivot = k;
							}
						if (maxv == T{}) return {};
						if (pivot != i)
						{
							sign = -sign;
							template for (constexpr auto j : iota<i, minor_size>)
								std::swap(a[i][j], a[pivot][j]);
						}
						template for (constexpr auto j : iota<i + 1, minor_size>)
							template for (constexpr auto k : iota<i + 1, minor_size>)
								a[j][k] = (a[j][k] * a[i][i] - a[j][i] * a[i][k]) / prev;
						prev = a[i][i];
					}
					return sign * a[minor_size - 1][minor_size - 1];
				}
			}
#else
			{
				static_assert(sizeof...(ExcludedX) == 0);
				static_assert(sizeof...(ExcludedY) == 0);
				static constexpr auto impl{ [] (SquareMatrix<T, N>&& m)STATIC_LAMBDA_INLINE->T
				{
					using Index = Compact<minor_size>;
					T sign{ 1 }, det{ 1 };
					for (const auto i : iota<minor_size - 1>)
					{
						auto pivot{ i };
						T maxv{ Math::abs(m[i][i]) };
						for (const auto r : iota_up<Index>(i + 1, minor_size))
							if (const T val{ Math::abs(m[r][i]) }; val > maxv)
							{
								maxv = val;
								pivot = r;
							}
						if (maxv == T{})return {};
						if (pivot != i)
						{
							sign = -sign;
							for (const auto c : iota_up<Index>(i, minor_size))
								std::swap(m[i][c], m[pivot][c]);
						}
						const T pivot_value{ m[i][i] };
						det *= pivot_value;
						const T inv_pivot{ T{ 1 } / pivot_value };
						for (const auto r : iota_up<Index>(i + 1, minor_size))
						{
							const T factor{ m[r][i] * inv_pivot };
							for (const auto c : iota_up<Index>(i + 1, minor_size))
								m[r][c] -= factor * m[i][c];
						}
					}
					return sign * det * m[minor_size - 1][minor_size - 1];
				} };
				if constexpr (std::is_rvalue_reference_v<decltype(m)>)return impl(FWD(m));
				else return impl(SquareMatrix<T, N>{ m });
			}
#endif
		}
	};
	template<class T, usize N>					using Determinant		= DeterminantImpl<T, std::make_index_sequence<N	>   , std::index_sequence< >, std::index_sequence< >>;
	template<class T, usize N, auto X, auto Y>	using DeterminantMinor	= DeterminantImpl<T, std::make_index_sequence<N - 1>, std::index_sequence<X>, std::index_sequence<Y>>;
#if TEMPLATE_FOR
	template<class T, usize N, usize...Ns>
	struct Tensor<T, N, Ns...>
	{
		using Element = std::remove_pointer_t<decltype([]static
		{
			if constexpr (sizeof...(Ns) == 0)return Identity<T*>{};
			else return Identity<Tensor<T, Ns...>*>{};
		}())>;
		using Index = Compact<N>;
		Element m_data[N];

		static constexpr Compact<1 + sizeof...(Ns)> rank{ 1 + sizeof...(Ns) };
		static CONSTEVAL Index size() noexcept { return N; }
		template<class Self>constexpr LikePtr<Self, Element> data(this Self&& self)noexcept { return &self.m_data[0]; }
		template<class Self>constexpr LikePtr<Self, Element> begin(this Self&& self)noexcept { return &self.m_data[0]; }
		template<class Self>constexpr LikePtr<Self, Element> end(this Self&& self)noexcept { return &self.m_data[N]; }
		template<class Self>constexpr Like<Self, Element> operator[](this Self&& self, Index index)noexcept { return static_cast<Like<Self, Element>>(FWD(self).m_data[index]); }
		template<class Self>
		constexpr Like<Self, T> operator[](this Self&& self, Index index0, Identity<Index, Ns>...indexes)noexcept
		requires(sizeof...(Ns) != 0)
		{
			return (FWD(self)[index0] | ... | TensorIndex{ indexes });
		}

		constexpr Tensor operator-()const noexcept { static constexpr auto [...I] { iota<N> }; return { -m_data[compact<I>]... }; }
		//constexpr Tensor operator+()const noexcept { static constexpr auto [...I] { iota<N> }; return { +m_data[compact<I>]... }; }
		//constexpr Tensor operator~()const noexcept { static constexpr auto [...I] { iota<N> }; return { ~m_data[compact<I>]... }; }
		
		//vector only
		constexpr T      lengthSqr      ()const noexcept requires(sizeof...(Ns) == 0) { static constexpr auto [...I] { iota<N> }; return      (... + (m_data[compact<I>] * m_data[compact<I>])); }
		constexpr T      length         ()const noexcept requires(sizeof...(Ns) == 0) { static constexpr auto [...I] { iota<N> }; return sqrt((... + (m_data[compact<I>] * m_data[compact<I>]))); }
		constexpr Tensor normalized     ()const noexcept requires(sizeof...(Ns) == 0) { return (*this) / length(); }
		constexpr Tensor normalized_safe()const noexcept requires(sizeof...(Ns) == 0) { if (const T lenSqr{ lengthSqr() }; lenSqr != T{}) [[likely]] return (*this) / sqrt(lenSqr); return {}; }
		constexpr void   normalize      ()      noexcept requires(sizeof...(Ns) == 0) { (*this) /= length(); }
		constexpr void   normalize_safe ()      noexcept requires(sizeof...(Ns) == 0) { if (const T lenSqr{ lengthSqr() }; lenSqr != T{}) [[likely]] (*this) /= sqrt(lenSqr); }
		friend constexpr T      distanceSqr(const Tensor& l, const Tensor& r) noexcept requires(sizeof...(Ns) == 0) { return (l - r).lengthSqr(); }
		friend constexpr T      distance   (const Tensor& l, const Tensor& r) noexcept requires(sizeof...(Ns) == 0) { return (l - r).length(); }
		friend constexpr Tensor cross      (const Tensor& l, const Tensor& r) noexcept requires(sizeof...(Ns) == 0 && N == 3) { return { l[1] * r[2] - l[2] * r[1],l[2] * r[0] - l[0] * r[2],l[0] * r[1] - l[1] * r[0] }; }
		friend constexpr T      cross      (const Tensor& l, const Tensor& r) noexcept requires(sizeof...(Ns) == 0 && N == 2) { return l[0] * r[1] - r[0] * l[1]; }
		friend constexpr T      dot        (const Tensor& l, const Tensor& r) noexcept 
		{
			static constexpr auto [...I] { iota<N> };
			if constexpr (sizeof...(Ns) == 0)return (... + (l[compact<I>] * r[compact<I>]));
			else return (... + dot(l[compact<I>], r[compact<I>]));
		}

		friend constexpr bool operator==(const Tensor& l, const Tensor& r)noexcept { static constexpr auto [...I] { iota<N> }; return (... && (l[compact<I>] == r[compact<I>])); }
		friend constexpr bool operator!=(const Tensor& l, const Tensor& r)noexcept { static constexpr auto [...I] { iota<N> }; return (... || (l[compact<I>] != r[compact<I>])); }

		friend constexpr Tensor operator+(const Tensor& l, const Tensor& r)noexcept { static constexpr auto [...I] { iota<N> }; return { l[compact<I>] + r[compact<I>]... }; }
		friend constexpr Tensor operator-(const Tensor& l, const Tensor& r)noexcept { static constexpr auto [...I] { iota<N> }; return { l[compact<I>] - r[compact<I>]... }; }
		friend constexpr Tensor operator|(const Tensor& l, const Tensor& r)noexcept { static constexpr auto [...I] { iota<N> }; return { l[compact<I>] | r[compact<I>]... }; }
		friend constexpr Tensor operator&(const Tensor& l, const Tensor& r)noexcept { static constexpr auto [...I] { iota<N> }; return { l[compact<I>] & r[compact<I>]... }; }
		friend constexpr Tensor operator^(const Tensor& l, const Tensor& r)noexcept { static constexpr auto [...I] { iota<N> }; return { l[compact<I>] ^ r[compact<I>]... }; }
		friend constexpr Tensor operator*($Arithmetic auto l, const Tensor& r)noexcept { static constexpr auto [...I] { iota<N> }; return { l * r[compact<I>]... }; }
		friend constexpr Tensor operator*(const Tensor& l, $Arithmetic auto r)noexcept { static constexpr auto [...I] { iota<N> }; return { l[compact<I>] * r... }; }
		friend constexpr Tensor operator/(const Tensor& l, $Arithmetic auto r)noexcept { static constexpr auto [...I] { iota<N> }; return { l[compact<I>] / r... }; }

		template<class U>constexpr Tensor& operator+=(const Tensor<U, N, Ns...>& r)noexcept { template for (constexpr auto I: iota<N>) m_data[compact<I>] += r[compact<I>]; return *this; }
		template<class U>constexpr Tensor& operator-=(const Tensor<U, N, Ns...>& r)noexcept { template for (constexpr auto I: iota<N>) m_data[compact<I>] -= r[compact<I>]; return *this; }
		template<class U>constexpr Tensor& operator|=(const Tensor<U, N, Ns...>& r)noexcept { template for (constexpr auto I: iota<N>) m_data[compact<I>] |= r[compact<I>]; return *this; }
		template<class U>constexpr Tensor& operator&=(const Tensor<U, N, Ns...>& r)noexcept { template for (constexpr auto I: iota<N>) m_data[compact<I>] &= r[compact<I>]; return *this; }
		template<class U>constexpr Tensor& operator^=(const Tensor<U, N, Ns...>& r)noexcept { template for (constexpr auto I: iota<N>) m_data[compact<I>] ^= r[compact<I>]; return *this; }
		constexpr Tensor& operator*=($Arithmetic auto r)noexcept { template for (constexpr auto I: iota<N>)m_data[I] *= r; return *this; }
		constexpr Tensor& operator/=($Arithmetic auto r)noexcept { template for (constexpr auto I: iota<N>)m_data[I] /= r; return *this; }

		template<class U>requires(/*                  */sizeof(T) == sizeof(U))friend     const Tensor<U, N, Ns...>& interpreted_as(const Tensor& tensor)noexcept { return reinterpret_cast<const Tensor<U, N, Ns...>&>(tensor); }
		template<class U>requires(sizeof...(Ns) == 0 && sizeof(T) == sizeof(U))friend constexpr Tensor<U, N, Ns...>  bitcast/*   */(const Tensor& tensor)noexcept { static constexpr auto [...I] { iota<N> }; return { (__builtin_bit_cast(U,tensor.m_data[compact<I>]))... }; }
		template<class U>requires(sizeof...(Ns) != 0 && sizeof(T) == sizeof(U))friend constexpr Tensor<U, N, Ns...>  bitcast/*   */(const Tensor& tensor)noexcept { static constexpr auto [...I] { iota<N> }; return { bitcast<U>(tensor.m_data[compact<I>])... }; }
		template<class U>requires(sizeof...(Ns) == 0)                          friend constexpr Tensor<U, N, Ns...>  cast/*      */(const Tensor& tensor)noexcept { static constexpr auto [...I] { iota<N> }; return { static_cast<U>(tensor.m_data[compact<I>])... }; }
		template<class U>requires(sizeof...(Ns) != 0)                          friend constexpr Tensor<U, N, Ns...>  cast/*      */(const Tensor& tensor)noexcept { static constexpr auto [...I] { iota<N> }; return { cast<U>(tensor.m_data[compact<I>])... }; }

		template<usize M, usize...Ms>
		requires(sizeof...(Ms) == sizeof...(Ns))
		friend constexpr Tensor<T, M, Ms...> reshape(const Tensor& tensor)noexcept
		{
			static constexpr auto [...I] { iota<std::min(M, N)> };
			if constexpr (sizeof...(Ns) == 0)
				return { tensor[compact<I>]... };
			else
				return { reshape<Ms...>(tensor[compact<I>])... };
		}

		template<std::array<usize, rank> indexes, class Self>
		requires([]static
		{
			static constexpr std::array<usize, rank> dims{N, Ns...};
			for (const auto i : iota<rank>)
				if (indexes[i] >= dims[i])
					return false;
			return true;
		}())
		constexpr Like<Self, T> at(this Self&& self)noexcept
		{
			if consteval
			{
				static constexpr auto [...idx] { indexes };
				return (FWD(self) | ... | TensorIndex{ compact<idx> });
			}
			else
			{
				static constexpr usize flattened_index{ [] static
				{
					usize result{};
					template for (constexpr auto J : iota<rank>)
						result = result * element_at<J>(N, Ns...) + indexes[J];
					return result;
				}() };
				return reinterpret_cast<LikePtr<Self, T>>(&self.m_data)[flattened_index];//technically ub
			}
		}

		template<usize...P>
		requires(sizeof...(P) == rank && (... && (P < rank)) && [] static
		{
			bool visited[rank]{};
			(..., (visited[P] = true));
			for (const bool x : visited)
				if (!x)
					return false;
			return true;
		}())
		constexpr Tensor<T, element_at<P>(N, Ns...)...> transpose()const noexcept
		{
			Tensor<T, element_at<P>(N, Ns...)...> result;
#if CLANG//clang bug
			static constexpr auto to_index{ [](usize rest) static
			{
				std::array<usize, rank> result;
				template for (constexpr auto J : reversed(iota<rank>))
				{
					result[J] = rest % element_at<P...[J]>(N, Ns...);
					rest /= element_at<P...[J]>(N, Ns...);
				}
				return result;
			} };
			static constexpr auto from_index{ [](std::array<usize, rank> to) static
			{
				std::array<usize, rank> result;
				template for (constexpr auto J : iota<rank>)
					result[P...[J]] = to[J];
				return result;
			} };
			template for (constexpr auto I : iota<(N * ... * Ns)>)
			{
				static constexpr std::array to{ to_index(I) };
				static constexpr std::array from{ from_index(to) };
				result.template at<to>() = at<from>();
			}
#else
			template for (constexpr auto I : iota<(N * ... * Ns)>)
			{
				static constexpr auto flattened_to{ I };
				static constexpr std::array to{ [] static
				{
					std::array<usize, rank> result;
					usize rest{ flattened_to };
					template for (constexpr auto J : reversed(iota<rank>))
					{
						result[J] = rest % element_at<P...[J]>(N, Ns...);
						rest /= element_at<P...[J]>(N, Ns...);
					}
					return result;
				}() };
				static constexpr std::array from{ [] static
				{
					std::array<usize, rank> result;
					template for (constexpr auto J : iota<rank>)
						result[P...[J]] = to[J];
					return result;
				}() };
				result.template at<to>() = at<from>();
			}
#endif
			return result;
		}
		INLINE constexpr Tensor<T, Ns..., N> transpose()const noexcept requires(sizeof...(Ns) == 1) { return transpose<1, 0>(); }
		template<auto X, auto Y>INLINE constexpr T determinant_minor(this auto&& self)noexcept requires(sizeof...(Ns) == 1 && (...&&(N == Ns))) { return DeterminantMinor<T, N, X, Y>::operator()(FWD(self)); }
		INLINE constexpr T determinant(this auto&& self)noexcept requires(sizeof...(Ns) == 1 && (...&&(N == Ns))) { return Determinant<T, N>::operator()(FWD(self)); }

		constexpr Opt<Tensor, details::IsFiniteImpl{}> inverse(this auto&& self) noexcept requires(sizeof...(Ns) == 1 && (...&&(N == Ns)))//square matrix
		{
			if constexpr (N == 1)return Tensor{ {{T{1} / self[0][0]}} };
			else if constexpr (N == 2)
			{
				const T det{ self.determinant() };
				return Tensor{ { { self[1][1] / det, -self[0][1] / det }, { -self[1][0] / det, self[0][0] / det } } };
			}
			else if constexpr (N == 3)
			{
				const T det{ self.determinant() };
				return Tensor
				{ {
					{(self[1][1] * self[2][2] - self[1][2] * self[2][1]) / det, (self[0][2] * self[2][1] - self[0][1] * self[2][2]) / det, (self[0][1] * self[1][2] - self[0][2] * self[1][1]) / det},
					{(self[1][2] * self[2][0] - self[1][0] * self[2][2]) / det, (self[0][0] * self[2][2] - self[0][2] * self[2][0]) / det, (self[0][2] * self[1][0] - self[0][0] * self[1][2]) / det},
					{(self[1][0] * self[2][1] - self[1][1] * self[2][0]) / det, (self[0][1] * self[2][0] - self[0][0] * self[2][1]) / det, (self[0][0] * self[1][1] - self[0][1] * self[1][0]) / det}
				} };
			}
			else
			{
				static constexpr auto impl{ [] (Tensor<T, N, N>&& m)STATIC_LAMBDA_INLINE
				{
					Tensor result{ []static CONSTEVAL
					{
						Tensor eye;
						for (const auto i : iota<N>)
							for (const auto j : iota<N>)
								eye[i][j] = i == j;
						return eye;
					}() };
					template for (constexpr auto i : iota<N>)
					{
						auto pivot{ i };
						T maxv[[maybe_unused]]{ Math::abs(m[i][i]) };//gcc bug
						template for (constexpr auto k : iota<i + 1, N>)
							if (const T val{ Math::abs(m[k][i]) }; val > maxv)
							{
								maxv = val;
								pivot = k;
							}
						if (pivot != i)
						{
							template for (constexpr auto j : iota<N>)
								std::swap(m[i][j], m[pivot][j]);
							template for (constexpr auto j : iota<N>)
								std::swap(result[i][j], result[pivot][j]);
						}
						const T diag [[maybe_unused]] { m[i][i] };//gcc bug: warning: variable 'diag' set but not used
						template for (constexpr auto j : iota<i + 1, N>)
						{
							const T factor{ m[j][i] / diag };
							template for (constexpr auto k : iota<i, N>)
								m[j][k] -= factor * m[i][k];
							template for (constexpr auto k : iota<0, N>)
								result[j][k] -= factor * result[i][k];
						}
					}
					template for (constexpr auto i : reversed(iota<N>))
					{
						const T diag{ m[i][i] };
						template for (constexpr auto j : iota<i>)
						{
							const T factor{ m[j][i] / diag };
							template for (constexpr auto k : iota<N>)
								result[j][k] -= factor * result[i][k];
						}
						template for (constexpr auto j : iota<N>)
							result[i][j] /= diag;
					}
					return result;
				} };
				if constexpr (std::is_rvalue_reference_v<decltype(self)>)return impl(FWD(self));
				else return impl(Tensor{ self });
			}
		}

		constexpr Opt<Tensor<T, N>, details::IsFiniteImpl{}> solve(this auto&& self, Tensor<T, N> v) noexcept requires(sizeof...(Ns) == 1 && (...&&(N == Ns)))//square matrix
		{
			if constexpr (N <= 4)
			{
				Tensor<T, N> result;
				template for (constexpr auto I : iota<N>)
				{
					Tensor<T, N, N> A;
					template for (constexpr auto R : iota<N>)
						template for (constexpr auto C : iota<N>)
							if constexpr (C == I)A[R][C] = v[R];
							else A[R][C] = self[R][C];
					result[I] = MOV(A).determinant();
				}
				result /= FWD(self).determinant();
				return result;
			}
			static constexpr auto impl{ [] (Tensor<T, N, N>&& m, Tensor<T, N>& x)STATIC_LAMBDA_INLINE
			{
				template for (constexpr auto i : iota<N>)
				{
					auto pivot{ i };
					T maxv[[maybe_unused]]{ Math::abs(m[i][i]) };//gcc bug
					template for (constexpr auto r : iota<i + 1, N>)
						if (const T val{ Math::abs(m[r][i]) }; val > maxv)
						{
							maxv = val;
							pivot = r;
						}
					if (pivot != i)
					{
						template for (constexpr auto c : iota<N>)
							std::swap(m[i][c], m[pivot][c]);
						std::swap(x[i], x[pivot]);
					}
					const T diag [[maybe_unused]] { m[i][i] };//gcc bug: warning: variable 'diag' set but not used
					template for (constexpr auto r : iota<i + 1, N>)
					{
						const T factor{ m[r][i] / diag };
						template for (constexpr auto c : iota<i, N>)
							m[r][c] -= factor * m[i][c];
						x[r] -= factor * x[i];
					}
				}
				template for (constexpr auto i : reversed(iota<N>))
				{
					x[i] /= m[i][i];
					template for (constexpr auto r : iota<i>)
						x[r] -= m[r][i] * x[i];
				}
			} };
			if constexpr (std::is_rvalue_reference_v<decltype(self)>)impl(FWD(self), v);
			else impl(Tensor<T, N, N>{self}, v);
			return v;
		}
	};

	template<class Stream, class T, usize...Ns>
	constexpr Stream& operator<<(Stream& os, const Tensor<T, Ns...>& tensor)noexcept
	{
		if constexpr (tensor.rank == 1)
		{
			os << '[';
			template for (constexpr auto I : iota<Ns...[0]>)
			{
				if constexpr (I)os << ',';
				os << tensor[I];
			}
			os << ']';
		}
		else if constexpr (tensor.rank == 2)
		{
			static constexpr auto print_vector{ [] (Stream& os, const Tensor<T, Ns...[1]>& vector)static
			{
				template for (constexpr auto I : iota<Ns...[1]>)
				{
					if constexpr (I)os << '\t';
					os << vector[I];
				}
			} };
			template for (constexpr auto I : iota<Ns...[0]>)
			{
				if constexpr (I)os << '\n';
				print_vector(os, tensor[I]);
			}
		}
		else
		{
			static constexpr auto print_tensor{ [] <auto...J> (this auto&& self, Stream& os, const Tensor<T, J...>&tensor)
			{
				os << '[';
				template for (constexpr auto I : iota<J...[0]>)
				{
					if constexpr (I)os << ',';
					if constexpr (tensor.rank == 1)os << tensor[I];
					else/*                       */self(os, tensor[I]);
				}
				os << ']';
			} };
			print_tensor(os, tensor);
		}
		return os;
	}
#else
	template<class T, usize N, usize...Ns, usize...I>
	struct TensorImpl<T, std::index_sequence<N, Ns...>, std::index_sequence<I...>>
	{
		using Element = std::remove_pointer_t<decltype([]static
		{
			if constexpr (sizeof...(Ns) == 0)return Identity<T*>{};
			else return Identity<Tensor<T, Ns...>*>{};
		}())>;
		using Index = Compact<N>;
		Element m_data[N];

		static constexpr Compact<1 + sizeof...(Ns)> rank{ 1 + sizeof...(Ns) };
		static CONSTEVAL Index size() noexcept { return N; }
		template<class Self>constexpr LikePtr<Self, Element> data(this Self&& self)noexcept { return &self.m_data[0]; }
		template<class Self>constexpr LikePtr<Self, Element> begin(this Self&& self)noexcept { return &self.m_data[0]; }
		template<class Self>constexpr LikePtr<Self, Element> end(this Self&& self)noexcept { return &self.m_data[N]; }
		template<class Self>constexpr Like<Self, Element> operator[](this Self&& self, Index index)noexcept { return static_cast<Like<Self, Element>>(FWD(self).m_data[index]); }
		template<class Self>
		constexpr Like<Self, T> operator[](this Self&& self, Index index0, Identity<Index, Ns>...indexes)noexcept
		requires(sizeof...(Ns) != 0)
		{
			return (FWD(self)[index0] | ... | TensorIndex{ indexes });
		}

		constexpr TensorImpl operator-()const noexcept { return { -m_data[I]... }; }
		//constexpr TensorImpl operator+()const noexcept { return { +m_data[I]... }; }
		//constexpr TensorImpl operator~()const noexcept { return { ~m_data[I]... }; }

		//vector only
		constexpr T          lengthSqr      ()const noexcept requires(sizeof...(Ns) == 0) { return      (... + (m_data[I] * m_data[I])); }
		constexpr T          length         ()const noexcept requires(sizeof...(Ns) == 0) { return sqrt((... + (m_data[I] * m_data[I]))); }
		constexpr TensorImpl normalized     ()const noexcept requires(sizeof...(Ns) == 0) { return (*this) / length(); }
		constexpr TensorImpl normalized_safe()const noexcept requires(sizeof...(Ns) == 0) { if (const T lenSqr{ lengthSqr() }; lenSqr != T{}) [[likely]] return (*this) / sqrt(lenSqr); return {}; }
		constexpr void       normalize      ()      noexcept requires(sizeof...(Ns) == 0) { (*this) /= length(); }
		constexpr void       normalize_safe ()      noexcept requires(sizeof...(Ns) == 0) { if (const T lenSqr{ lengthSqr() }; lenSqr != T{}) [[likely]] (*this) /= sqrt(lenSqr); }
		friend constexpr T          distanceSqr (const TensorImpl& l, const TensorImpl& r) noexcept requires(sizeof...(Ns) == 0) { return (l - r).lengthSqr(); }
		friend constexpr T          distance    (const TensorImpl& l, const TensorImpl& r) noexcept requires(sizeof...(Ns) == 0) { return (l - r).length(); }
		friend constexpr TensorImpl cross       (const TensorImpl& l, const TensorImpl& r) noexcept requires(sizeof...(Ns) == 0 && N == 3) { return { l[1] * r[2] - l[2] * r[1],l[2] * r[0] - l[0] * r[2],l[0] * r[1] - l[1] * r[0] }; }
		friend constexpr T          cross       (const TensorImpl& l, const TensorImpl& r) noexcept requires(sizeof...(Ns) == 0 && N == 2) { return l[0] * r[1] - r[0] * l[1]; }
		friend constexpr T          dot         (const TensorImpl& l, const TensorImpl& r) noexcept 
		{
			if constexpr(sizeof...(Ns) == 0)return (... + (l[I] * r[I]));
			else return (... + dot(l[I], r[I]));
		}

		friend constexpr bool  operator==(const TensorImpl& l, const TensorImpl& r)noexcept { return (... && (l[I] == r[I])); }
		friend constexpr bool  operator!=(const TensorImpl& l, const TensorImpl& r)noexcept { return (... || (l[I] != r[I])); }

		friend constexpr TensorImpl operator+(const TensorImpl& l, const TensorImpl& r)noexcept { return { l[I] + r[I]... }; }
		friend constexpr TensorImpl operator-(const TensorImpl& l, const TensorImpl& r)noexcept { return { l[I] - r[I]... }; }
		friend constexpr TensorImpl operator|(const TensorImpl& l, const TensorImpl& r)noexcept { return { l[I] | r[I]... }; }
		friend constexpr TensorImpl operator&(const TensorImpl& l, const TensorImpl& r)noexcept { return { l[I] & r[I]... }; }
		friend constexpr TensorImpl operator^(const TensorImpl& l, const TensorImpl& r)noexcept { return { l[I] ^ r[I]... }; }
		friend constexpr TensorImpl operator*($Arithmetic auto l , const TensorImpl& r)noexcept { return { l * r[I]... }; }
		friend constexpr TensorImpl operator*(const TensorImpl& l, $Arithmetic auto  r)noexcept { return { l[I] * r... }; }
		friend constexpr TensorImpl operator/(const TensorImpl& l, $Arithmetic auto  r)noexcept { return { l[I] / r... }; }

		template<class U>constexpr TensorImpl& operator+=(const Tensor<U, N, Ns...>& r)noexcept { (..., (m_data[I] += r[I])); return *this; }
		template<class U>constexpr TensorImpl& operator-=(const Tensor<U, N, Ns...>& r)noexcept { (..., (m_data[I] -= r[I])); return *this; }
		template<class U>constexpr TensorImpl& operator|=(const Tensor<U, N, Ns...>& r)noexcept { (..., (m_data[I] |= r[I])); return *this; }
		template<class U>constexpr TensorImpl& operator&=(const Tensor<U, N, Ns...>& r)noexcept { (..., (m_data[I] &= r[I])); return *this; }
		template<class U>constexpr TensorImpl& operator^=(const Tensor<U, N, Ns...>& r)noexcept { (..., (m_data[I] ^= r[I])); return *this; }
		constexpr TensorImpl& operator*=($Arithmetic auto r)noexcept { (..., (m_data[I] *= r)); return *this; }
		constexpr TensorImpl& operator/=($Arithmetic auto r)noexcept { (..., (m_data[I] /= r)); return *this; }

		template<class U>requires(/*                  */sizeof(T) == sizeof(U))friend     const Tensor<U, N, Ns...>& interpreted_as(const TensorImpl& tensor)noexcept { return reinterpret_cast<const Tensor<U, N, Ns...>&>(tensor); }
		template<class U>requires(sizeof...(Ns) == 0 && sizeof(T) == sizeof(U))friend constexpr Tensor<U, N, Ns...>  bitcast/*   */(const TensorImpl& tensor)noexcept { return { (__builtin_bit_cast(U,tensor.m_data[I]))... }; }
		template<class U>requires(sizeof...(Ns) != 0 && sizeof(T) == sizeof(U))friend constexpr Tensor<U, N, Ns...>  bitcast/*   */(const TensorImpl& tensor)noexcept { return { bitcast<U>(tensor.m_data[I])... }; }
		template<class U>requires(sizeof...(Ns) == 0)                          friend constexpr Tensor<U, N, Ns...>  cast/*      */(const TensorImpl& tensor)noexcept { return { static_cast<U>(tensor.m_data[I])... }; }
		template<class U>requires(sizeof...(Ns) != 0)                          friend constexpr Tensor<U, N, Ns...>  cast/*      */(const TensorImpl& tensor)noexcept { return { cast<U>(tensor.m_data[I])... }; }

		template<usize M, usize...Ms>
		requires(sizeof...(Ms) == sizeof...(Ns))
		friend constexpr Tensor<T, M, Ms...> reshape(const TensorImpl& tensor)noexcept
		{
			static constexpr auto impl{ [] <usize...J>(std::index_sequence<J...>)static
			{
				return[](const TensorImpl& tensor)STATIC_LAMBDA_INLINE->Tensor<T, M, Ms...>
				{
					if constexpr (sizeof...(Ns) == 0)
						return {tensor[J]...};
					else
						return {reshape<Ms...>(tensor[J])...};
				};
			}(std::make_index_sequence<std::min(M, N)>{}) };
			return impl(tensor);
		}

		template<std::array<usize, rank> indexes, class Self>
#if !defined(__INTELLISENSE__)//msvc bug: broken intellisense
		requires([]static
		{
			for (const auto i : iota<1 + sizeof...(Ns)>)
				if (indexes[i] >= std::array{ N, Ns... }[i])
					return false;
			return true;
		}())
#endif
		constexpr Like<Self, T> at(this Self&& self)noexcept
		{
			if consteval
			{
				return[&]<usize...J>(std::index_sequence<J...>) -> Like<Self, T>
				{
					return (FWD(self) | ... | TensorIndex{ compact<indexes[J]> });
				}(std::make_index_sequence<rank>{});
			}
			else
			{
				static constexpr usize flattened_index{ [] static
				{
					usize result{};
					for (const auto J : iota<rank>)
						result = result * std::array{ N, Ns... }[J] + indexes[J];
					return result;
				}() };
				return reinterpret_cast<LikePtr<Self, T>>(&self.m_data)[flattened_index];//technically ub
			}
		}

		template<usize...P>
#if !defined(__INTELLISENSE__)//msvc bug: broken intellisense
		requires(sizeof...(P) == rank && (...&&(P < rank)) && [] static
		{
			bool visited[1 + sizeof...(Ns)]{};//msvc bug: rank is undefined
			(..., (visited[P] = true));
			for (const bool x : visited)
				if (!x)
					return false;
			return true;
		}())
#endif
		constexpr Tensor<T, element_at<P>(N, Ns...)...> transpose()const noexcept
		{
			static constexpr auto rank_{ compact<1 + sizeof...(Ns)> };//msvc bug: rank is undefined
			static constexpr auto impl{ [] <usize...J>(std::index_sequence<J...>)static
			{
				return[](const TensorImpl& tensor)STATIC_LAMBDA_INLINE
				{
					static constexpr std::array map{[]static
					{
						static constexpr auto flattened_to{ J };
						static constexpr std::array to{ [] static
						{
							std::array<usize, rank_> result;
							usize rest{ flattened_to };
							for (const auto K : reversed(iota<rank_>))
							{
								result[K] = rest % std::array{ N, Ns... }[std::array{ P... }[K]];
								rest /= std::array{ N, Ns... }[std::array{ P... }[K]];
							}
							return result;
						}() };
						static constexpr std::array from{ [] static
						{
							std::array<usize, rank_> result;
							for (const auto K : iota<rank_>)
								result[std::array{ P... }[K]] = to[K];
							return result;
						}() };
						return std::pair{to, from};
					}()... };
					Tensor<T, element_at<P>(N, Ns...)...> result;
					(..., (result.template at<map[J].first>() = tensor.template at<map[J].second>()));
					return result;
				};
			}(std::make_index_sequence<(N * ... * Ns)>{}) };
			return impl(*this);
		}
		INLINE constexpr Tensor<T, Ns..., N> transpose()const noexcept requires(sizeof...(Ns) == 1) { return transpose<1, 0>(); }
		template<auto X, auto Y>INLINE constexpr T determinant_minor(this auto&& self)noexcept requires(sizeof...(Ns) == 1 && (...&&(N == Ns))) { return DeterminantMinor<T, N, X, Y>::operator()(FWD(self)); }
		INLINE constexpr T determinant(this auto&& self)noexcept requires(sizeof...(Ns) == 1 && (...&&(N == Ns))) { return Determinant<T, N>::operator()(FWD(self)); }

		constexpr Opt<TensorImpl, details::IsFiniteImpl{}> inverse(this auto&& self) noexcept requires(sizeof...(Ns) == 1 && (...&&(N == Ns)))//square matrix
		{
			if constexpr (N == 1)return TensorImpl{ {{T{1} / self[0][0]}} };
			else if constexpr (N == 2)
			{
				const T det{ self.determinant() };
				return TensorImpl{ { { self[1][1] / det, -self[0][1] / det }, { -self[1][0] / det, self[0][0] / det } } };
			}
			else if constexpr (N == 3)
			{
				const T det{ self.determinant() };
				return TensorImpl
				{ {
					{(self[1][1] * self[2][2] - self[1][2] * self[2][1]) / det, (self[0][2] * self[2][1] - self[0][1] * self[2][2]) / det, (self[0][1] * self[1][2] - self[0][2] * self[1][1]) / det},
					{(self[1][2] * self[2][0] - self[1][0] * self[2][2]) / det, (self[0][0] * self[2][2] - self[0][2] * self[2][0]) / det, (self[0][2] * self[1][0] - self[0][0] * self[1][2]) / det},
					{(self[1][0] * self[2][1] - self[1][1] * self[2][0]) / det, (self[0][1] * self[2][0] - self[0][0] * self[2][1]) / det, (self[0][0] * self[1][1] - self[0][1] * self[1][0]) / det}
				} };
			}
			else if constexpr (N <= 5)return TensorImpl
			{
				Fold::construct<Tensor<T, N>, N>([&]<auto J>(ImmediateValue<J>) LAMBDA_INLINE
				{
					return signed_value<isEven(I + J)>(self.template determinant_minor<J, I>());
				})...
			}* (T{ 1 } / self.determinant());
			else
			{
				static constexpr auto impl{ [] (TensorImpl&& m)STATIC_LAMBDA_INLINE
				{
					TensorImpl result{ []static CONSTEVAL
					{
						TensorImpl eye;
						for (const auto i : iota<N>)
							for (const auto j : iota<N>)
								eye[i][j] = i == j;
						return eye;
					}() };
					for (const auto i : iota<N>)
					{
						auto pivot{ i };
						T maxv{ Math::abs(m[i][i]) };
						for (const auto k : iota_up<Compact<N>>(i + 1, N))
							if (const T val{ Math::abs(m[k][i]) }; val > maxv)
							{
								maxv = val;
								pivot = k;
							}
						if (pivot != i)
						{
							for (const auto j : iota<N>)
								std::swap(m[i][j], m[pivot][j]);
							for (const auto j : iota<N>)
								std::swap(result[i][j], result[pivot][j]);
						}
						const T diag_inv{ T{1} / m[i][i] };
						for (const auto j : iota_up<Compact<N>>(i + 1, N))
						{
							const T factor{ m[j][i] * diag_inv };
							m[j][i] = T{};
							for (const auto k : iota_up<Compact<N>>(i + 1, N))
								m[j][k] -= factor * m[i][k];
							for (const auto k : iota_up<Compact<N>>(0, N))
								result[j][k] -= factor * result[i][k];
						}
					}
					for (const auto i : reversed(iota<N>))
					{
						const T diag_inv{ T{1} / m[i][i] };
						for (const auto j : iota_up(i))
						{
							const T factor{ m[j][i] * diag_inv };
							for (const auto k : iota<N>)
								result[j][k] -= factor * result[i][k];
						}
						for (const auto j : iota<N>)
							result[i][j] *= diag_inv;
					}
					return result;
				} };
				if constexpr (std::is_rvalue_reference_v<decltype(self)>)return impl(FWD(self));
				else return impl(TensorImpl{ self });
			}
		}

		constexpr Opt<Tensor<T, N>, details::IsFiniteImpl{}> solve(this auto&& self, Tensor<T, N> v) noexcept requires(sizeof...(Ns) == 1 && (...&&(N == Ns)))//square matrix
		{
			if constexpr (N == 1)
				return { v[0] / self[0][0] };
			else if constexpr (N <= 4)//https://www.youtube.com/watch?v=95zVTo2nu3Q
			{
				const T det_inv{ T{ 1 } / self.determinant() };
#if GCC//gcc bug
				return Fold::construct<Tensor<T, N>, N>([&]<auto X>(ImmediateValue<X>) LAMBDA_INLINE
				{
					return Fold::construct<TensorImpl, N>([&]<auto R>(ImmediateValue<R>) LAMBDA_INLINE
					{
						return Fold::construct<Tensor<T, N>, N>([&]<auto C>(ImmediateValue<C>) LAMBDA_INLINE
						{
							if constexpr (C == X)return v[R];
							else return self[R][C];
						});
					}).determinant() * det_inv;
				});
#else
				return Tensor<T, N>
				{(
					Fold::construct<TensorImpl, N>([&]<auto R>(ImmediateValue<R>) LAMBDA_INLINE
					{
						return Fold::construct<Tensor<T, N>, N>([&]<auto C>(ImmediateValue<C>) LAMBDA_INLINE
						{
							if constexpr (C == I)return v[R];
							else return self[R][C];
						});
					}).determinant() * det_inv
				)...};
#endif
			}
			else
			{
				static constexpr auto impl{ [] (Tensor<T, N, N>&& m, Tensor<T, N>& x)STATIC_LAMBDA_INLINE
				{
					for (const auto i : iota<N>)
					{
						auto pivot{ i };
						T maxv{ Math::abs(m[i][i]) };
						for (const auto r : iota_up<Compact<N>>(i + 1, N))
							if (const T val{ Math::abs(m[r][i]) }; val > maxv)
							{
								maxv = val;
								pivot = r;
							}
						if (pivot != i)
						{
							for (const auto c : iota<N>)
								std::swap(m[i][c], m[pivot][c]);
							std::swap(x[i], x[pivot]);
						}
						const T diag_inv{ T{1} / m[i][i] };
						for (const auto r : iota_up<Compact<N>>(i + 1, N))
						{
							const T factor{ m[r][i] * diag_inv };
							m[r][i] = T{};
							for (const auto c : iota_up<Compact<N>>(i + 1, N))
								m[r][c] -= factor * m[i][c];
							x[r] -= factor * x[i];
						}
					}
					for (const auto i : reversed(iota<N>))
					{
						x[i] /= m[i][i];
						for (const auto r : iota_up(i))
							x[r] -= m[r][i] * x[i];
					}
				} };
				if constexpr (std::is_rvalue_reference_v<decltype(self)>)impl(FWD(self), v);
				else impl(Tensor<T, N, N>{self}, v);
				return v;
			}
		}
	};

	//vector
	template<class Stream, class T, usize N, usize...Is>
	constexpr Stream& operator<<(Stream& os, const TensorImpl<T, std::index_sequence<N>, std::index_sequence<0, Is...>>& tensor)noexcept
	{
		os << '[';
		os << tensor[0];
		(..., (
			(os << ','),
			(os << tensor[Is])
		));
		os << ']';
		return os;
	}

	//matrix
	template<class Stream, class T, usize M, usize N, usize...Is>
	constexpr Stream& operator<<(Stream& os, const TensorImpl<T, std::index_sequence<M, N>, std::index_sequence<0, Is...>>& tensor)noexcept
	{
		static constexpr auto print_vector{ [] <usize...Js>(Stream& os, const TensorImpl<T, std::index_sequence<N>, std::index_sequence<0, Js...>>& vector)static
		{
			os << vector[0];
			(..., (
				(os << '\t'),
				(os << vector[Js])
			));
		} };
		print_vector(os, tensor[0]);
		(..., (
			(os << '\n'),
			print_vector(os, tensor[Is])
		));
		return os;
	}

	//tensor rank > 2
	template<class Stream, class T, usize...Ns>
	requires(sizeof...(Ns) > 2)
	constexpr Stream& operator<<(Stream& os, const Tensor<T, Ns...>& tensor)noexcept
	{
		static constexpr auto print_tensor{ [] <usize M, usize...Ms, usize...Is>(this auto&& self, Stream& os, const TensorImpl<T, std::index_sequence<M, Ms...>, std::index_sequence<0, Is...>>& tensor)
		{
			os << '[';
			if constexpr (sizeof...(Ms))
			{
				self(os, tensor[0]);
				(..., (
					(os << ','),
					self(os, tensor[Is])
				));
			}
			else
			{
				os << tensor[0];
				(..., (
					(os << ','),
					(os << tensor[Is])
				));
			}
			os << ']';
		} };
		print_tensor(os, tensor);
		return os;
	}
#endif
	template<class T, usize R0, usize C0, usize R1, usize C1>
	requires(C0 == R1)
	constexpr Matrix<T, R0, C1> operator*(const Matrix<T, R0, C0>& lhs, const Matrix<T, R1, C1>& rhs)noexcept
	{
#if TEMPLATE_FOR
		Matrix<T, R0, C1> result;
		template for(constexpr auto r0: iota<R0>)
			template for(constexpr auto c1: iota<C1>)
			{
				static constexpr auto [...c0] { iota<C0> };
				result[r0][c1] = (... + (lhs[r0][compact<c0>] * rhs[compact<c0>][c1]));
			}
		return result;
#else
		return Fold::construct<Matrix<T, R0, C1>, R0>([&](auto r0)LAMBDA_INLINE
		{
			return Fold::construct<Vector<T, C1>, C1>([&](auto c1)LAMBDA_INLINE
			{
				return Fold::sum<C0>([&](auto c0)LAMBDA_INLINE{return lhs[r0][c0] * rhs[c0][c1]; });
			});
		});
#endif
	}

	template<class T, usize R, usize C>
	constexpr Vector<T, R> operator*(const Matrix<T, R, C>& lhs, const Vector<T, C>& rhs)noexcept
	{
#if VARIADIC_STRUCTURED_BINDING
		static constexpr auto [...r] { iota<R> };
		return { dot(lhs[compact<r>], rhs)... };
#else
		return Fold::construct<Vector<T, R>, R>([&](auto r)LAMBDA_INLINE
		{
			return dot(lhs[r], rhs);
		});
#endif
	}
}
namespace details::TensorTests
{
	using namespace Math;

	constexpr bool almost_equal($Floating auto l, $Floating auto r)noexcept
	{
		using T = std::remove_cvref_t<decltype(l)>;
		static constexpr T error{ T(0.00001) };
		const auto diff{ l - r };
		return diff > -error && diff < error;
	}

	constexpr bool almost_equal($ForEach auto&& l, $ForEach auto&& r)noexcept
	{
		auto lBegin{ l.begin() };
		auto rBegin{ r.begin() };
		for (; lBegin != l.end(); ++lBegin, ++rBegin)
			if (!almost_equal(*lBegin, *rBegin))
				return false;
		return true;
	}
	
	constexpr bool almost_equal(auto&& l, auto&& r)noexcept
	requires(requires{l.has_value(); })
	{
		return l.has_value() && almost_equal(*FWD(l), FWD(r));
	}

	constexpr bool almost_equal(auto&& l, auto&& r)noexcept
	requires(requires{r.has_value(); })
	{
		return r.has_value() && almost_equal(FWD(l), *FWD(r));
	}

	//operator[]
#if MSVC//msvc bug: expression did not evaluate to a constant
	STATIC_ASSERT([]static{Tensor<i32, 3> t{ 1, 2, 3 };return t[1] == 2;}());
#else
	STATIC_ASSERT(Tensor<i32, 3>{1, 2, 3}[1] == 2);
#endif
	STATIC_ASSERT(Tensor<i32, 2, 3>{{{1, 2, 3}, { 4, 5, 6 }}}[1, 2] == 6);
	STATIC_ASSERT(Tensor<i32, 2, 2, 3>{{{{{ 1, 2, 3}, { 4,  5,  6 }}}, { {{ 7,  8,  9}, {10, 11, 12}} }}}[1, 0, 2] == 9);
	//reshape
	STATIC_ASSERT(reshape<2>(Tensor<i32, 3>{1, 2, 3}) == Tensor<i32, 2>{1, 2});
	STATIC_ASSERT(reshape<4>(Tensor<i32, 3>{1, 2, 3}) == Tensor<i32, 4>{1, 2, 3, 0});
	STATIC_ASSERT(reshape<1, 1>(Tensor<i32, 2, 2>{{{1, 2}, { 3,4 }}}) == Tensor<i32, 1, 1>{{{1}}});
	STATIC_ASSERT(reshape<1, 3>(Tensor<i32, 2, 2>{{{1, 2}, { 3,4 }}}) == Tensor<i32, 1, 3>{{{1, 2, 0}}});
	STATIC_ASSERT(reshape<3, 1>(Tensor<i32, 2, 2>{{{1, 2}, { 3,4 }}}) == Tensor<i32, 3, 1>{{{1}, { 3 }, { 0 }}});
	STATIC_ASSERT(reshape<3, 3>(Tensor<i32, 2, 2>{{{1, 2}, { 3,4 }}}) == Tensor<i32, 3, 3>{{{1, 2, 0}, { 3,4,0 }, { 0,0,0 }}});
	//transpose
	STATIC_ASSERT(Tensor<f32, 3>{1, 2, 3}.transpose<0>() == Tensor<f32, 3>{1, 2, 3});
	STATIC_ASSERT(Tensor<i32, 2, 2>{{{1, 2}, { 3, 4 }}}.transpose<0, 1>() == Tensor<i32, 2, 2>{{{1, 2}, { 3, 4 }}});
	STATIC_ASSERT(Tensor<i32, 2, 2>{{{1, 2}, { 3, 4 }}}.transpose<1, 0>() == Tensor<i32, 2, 2>{{{1, 3}, { 2, 4 }}});
	STATIC_ASSERT(Tensor<i32, 2, 3>{{{1, 2, 3}, { 4, 5, 6 }}}.transpose() == Tensor<i32, 3, 2>{{{1, 4}, { 2, 5 }, { 3, 6 }}});
	STATIC_ASSERT(Tensor<i32, 1, 2, 3>{{{{{ 1, 2, 3}, { 4,  5,  6 }}}}}.transpose<2, 0, 1>() == Tensor<i32, 3, 1, 2>{{{{{1, 4}}}, { {{2,5}} }, { {{3,6}} }}});
	STATIC_ASSERT(Tensor<i32, 2, 2, 3>{{{{{ 1, 2, 3}, { 4,  5,  6 }}}, { {{ 7,  8,  9}, {10, 11, 12}} }}}.transpose<2, 0, 1>() == Tensor<i32, 3, 2, 2>{{{{{1, 4}, { 7,10 }}}, { {{2,5},{8,11}} }, { {{3,6},{9,12}} }}});
	//dot
	STATIC_ASSERT(dot(Tensor<i32, 2>{1, 2}, Tensor<i32, 2>{3, 4}) == 11);
	STATIC_ASSERT(dot(Tensor<i32, 2, 3>{{{1, 2, 3}, { 4,5,6 }}}, Tensor<i32, 2, 3>{{{7, 8, 9}, { 10,11,12 }}}) == 217);
	//matrix x vector
	STATIC_ASSERT(Matrix<f64, 1, 1>{{{1}}} *Vector<f64, 1>{1} == Vector<f64, 1>{1});
	STATIC_ASSERT(Matrix<f64, 1, 2>{{{1, 2}}} *Vector<f64, 2>{3, 4} == Vector<f64, 1>{11});
	STATIC_ASSERT(Matrix<f64, 2, 3>{{{1, 2, 3}, { 4, 5, 6 }}} *Vector<f64, 3>{7, 8, 9} == Vector<f64, 2>{50, 122});
	//matrix x matrix
	STATIC_ASSERT(Matrix<f64, 2, 3>{{{1, 2, 3}, { 4, 5, 6 }}} *Matrix<f64, 3, 2>{{{7, 8}, { 9,10 }, { 11,12 }}} == Matrix<f64, 2, 2>{{{58, 64}, { 139,154 }}});
	//determinant
	STATIC_ASSERT(almost_equal(1.        , SquareMatrix<f64, 1>{{{1}}}.determinant()));
	STATIC_ASSERT(almost_equal(-2.       , SquareMatrix<f64, 2>{{{ 1, 2 }, { 3, 4 }}}.determinant()));
	STATIC_ASSERT(almost_equal(-3.       , SquareMatrix<f64, 2>{{{ 5, 6 }, { 8, 9 }}}.determinant()));
	STATIC_ASSERT(almost_equal(0.        , SquareMatrix<f64, 3>{{{ 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }}}.determinant()));
	STATIC_ASSERT(almost_equal(24.       , SquareMatrix<f64, 3>{{{ 1, 2, 3 }, { 0, 4, 5 }, { 0, 0, 6 }}}.determinant()));
	STATIC_ASSERT(almost_equal(61.       , SquareMatrix<f64, 3>{{{ 2, 5, 6 }, { 3, 1, 4 }, { 7, 8, 9 }}}.determinant()));
	STATIC_ASSERT(almost_equal(0.        , SquareMatrix<f64, 4>{{{ 1, 2, 3, 4 }, { 2, 3, 4, 5 }, { 3, 4, 5, 6 }, { 6, 7, 8, 9 }}}.determinant()));
	STATIC_ASSERT(almost_equal(1.        , SquareMatrix<f64, 4>{{{ 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 }}}.determinant()));
	STATIC_ASSERT(almost_equal(180.      , SquareMatrix<f64, 4>{{{ 1, 0, 0, 0 }, { 2, 3, 0, 0 }, { 4, 5, 6, 0 }, { 7, 8, 9, 10}}}.determinant()));
	STATIC_ASSERT(almost_equal(11700.    , SquareMatrix<f64, 5>{{{ 1, 2, 3, 4, 5 }, { 0,6,7,8,9 }, { 0,0,10,11,12 }, { 0,0,0,13,14 }, { 0,0,0,0,15 }}}.determinant()));
	STATIC_ASSERT(almost_equal(6045.     , SquareMatrix<f64, 5>{{{ 1, 4, 5, 9, 7 }, { 3,8,2,9,8 }, { 3,3, 2, 9, 3 }, { 5,8,6, 2, 4 }, { 5,1,3,5, 3 }}}.determinant()));
	STATIC_ASSERT(almost_equal(233328.   , SquareMatrix<f64, 6>{{{ 9, -4, -9, -6, 4, -3 }, { 0, 6, 4, 4, 1, -4 }, { 4, 4, 0, -6, 2, -6 }, { 1, -5, -2, 4, -7, -5 }, { 8, -3, 3, 8, 6, 4 }, { -4, -4, 0, -9, -7, 7 }}}.determinant()));
	STATIC_ASSERT(almost_equal(25159217. , SquareMatrix<f64, 7>{{{ -1, -2, 3, -3, -6, -1, 1 }, { -9, 7, -8, -7, -6, -2, 1 }, { -3, -6, 8, -5, 1, -6, -1 }, { -6, -1, -3, -6, 3, 5, -9 }, { 1, 5, 7, 3, 5, 7, 0 }, { 8, -4, -1, 1, 6, 8, 6 }, { -6, -8, -3, 8, 4, 4, 8 }}}.determinant()));
	STATIC_ASSERT(almost_equal(-12701394., SquareMatrix<f64, 8>{{{ 9, 5, 7, -6, 1, -2, -8, 8 }, { 9, -5, 4, -4, 4, -8, 9, -2 }, { 3, 3, 5, 0, -2, -8, -6, -2 }, { -7, 2, -2, 3, -5, 8, 3, 3 }, { 6, -6, -3, 2, 9, 4, -9, 4 }, { 1, -2, -8, 7, 7, -7, 1, -2 }, { -7, -2, -6, 3, 2, 9, 2, -4 }, { -4, 6, 1, -2, -9, 8, -6, 3 }}}.determinant()));
	STATIC_ASSERT(almost_equal(454912606., SquareMatrix<f64, 9>{{{ -5, -8, -7, -5, 3, 0, -9, -7, 4 }, { 4, -6, 5, 1, 1, -6, -8, 9, -4 }, { 1, 7, 3, -4, -5, -4, -6, 5, 0 }, { 5, -2, 4, 0, -2, 1, -6, -4, -6 }, { -2, -7, 0, 6, -6, -7, 1, -6, -3 }, { -9, -5, 3, 5, 3, -5, 1, 6, 9 }, { -5, -7, -5, 6, -8, 7, 6, 1, -7 }, { 4, 1, -5, -3, -3, -2, 8, -5, 1 }, { -2, -1, -3, -4, 7, 0, -3, -4, 3 }}}.determinant()));
	//inverse
	STATIC_ASSERT(almost_equal(SquareMatrix<f64, 1>{{{2.}}}.inverse(), SquareMatrix<f64, 1>{{{0.5}}}));
	STATIC_ASSERT(almost_equal(SquareMatrix<f64, 2>{{{1., 2.}, { 3.,4. }}}.inverse(), SquareMatrix<f64, 2>{{{-2., 1.}, { 1.5,-0.5 }}}));
	STATIC_ASSERT(almost_equal(SquareMatrix<f64, 3>{{{1., 2., 3.}, { 4.,5.,6. }, { 7.,8.,69. }}}.inverse(), SquareMatrix<f64, 3>{{{-1.65, 0.63333333, 0.01666667}, { 1.3,-0.26666667,-0.03333333 }, { 0.01666667,-0.03333333,0.01666667 }}}));
	STATIC_ASSERT(almost_equal(SquareMatrix<f64, 4>{{{1., 0., 0., 0.}, { 2.,3.,0.,0. }, { 4.,5.,6.,0. }, { 7.,8.,9.,10. }}}.inverse(), SquareMatrix<f64, 4>{{{1., 0., 0., 0.}, { -0.66666667,0.33333333,0.,0. }, { -0.11111111,-0.27777778,0.16666667,0. }, { -0.06666667,-0.01666667,-0.15,0.1 }}}));
	STATIC_ASSERT(almost_equal(SquareMatrix<f64, 5>{{{1., 2., 3., 4., 5.}, { 0.,6.,7.,8.,9. }, { 0.,0.,10.,11.,12. }, { 0.,0.,0.,13.,14. }, { 0.,0.,0.,0.,15. }}}.inverse(), SquareMatrix<f64, 5>{{{1., -0.33333333, -0.06666667, -0.04615385, -0.03692308}, { 0.,0.16666667,-0.11666667,-0.00384615,-0.00307692 }, { 0.,0.,0.1,-0.08461538,-0.00102564 }, { 0.,0.,0.,0.07692308,-0.07179487 }, { 0.,0.,0.,0.,0.06666667 }}}));
	STATIC_ASSERT(almost_equal(SquareMatrix<f64, 6>{{{1., 0.1, 0.1, 0.1, 0.1, 0.1}, { 0.,1.,0.1,0.1,0.1,0.1 }, { 0.,0.,1.,0.1,0.1,0.1 }, { 0.,0.,0.,1.,0.1,0.1 }, { 0.,0.,0.,0.,1.,0.1 }, { 0.,0.,0.,0.,0.,1. }}}.inverse(), SquareMatrix<f64, 6>{{{1., -0.1, -0.09, -0.081, -0.0729, -0.06561}, { 0.,1.,-0.1,-0.09,-0.081,-0.0729 }, { 0.,0.,1.,-0.1,-0.09,-0.081 }, { 0.,0.,0.,1.,-0.1,-0.09 }, { 0.,0.,0.,0.,1.,-0.1 }, { 0.,0.,0.,0.,0.,1. }}}));
	STATIC_ASSERT(almost_equal(SquareMatrix<f64, 7>{{{1., 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}, { 0.,1.,0.1,0.1,0.1,0.1,0.1 }, { 0.,0.,1.,0.1,0.1,0.1,0.1 }, { 0.,0.,0.,1.,0.1,0.1,0.1 }, { 0.,0.,0.,0.,1.,0.1,0.1 }, { 0.,0.,0.,0.,0.,1.,0.1 }, { 0.,0.,0.,0.,0.,0.,1. }}}.inverse(), SquareMatrix<f64, 7>{{{1., -0.1, -0.09, -0.081, -0.0729, -0.06561, -0.059049}, { 0.,1.,-0.1,-0.09,-0.081,-0.0729,-0.06561 }, { 0.,0.,1.,-0.1,-0.09,-0.081,-0.0729 }, { 0.,0.,0.,1.,-0.1,-0.09,-0.081 }, { 0.,0.,0.,0.,1.,-0.1,-0.09 }, { 0.,0.,0.,0.,0.,1.,-0.1 }, { 0.,0.,0.,0.,0.,0.,1. }}}));
	STATIC_ASSERT(almost_equal(SquareMatrix<f64, 8>{{{1., 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}, { 0.,1.,0.1,0.1,0.1,0.1,0.1,0.1 }, { 0.,0.,1.,0.1,0.1,0.1,0.1,0.1 }, { 0.,0.,0.,1.,0.1,0.1,0.1,0.1 }, { 0.,0.,0.,0.,1.,0.1,0.1,0.1 }, { 0.,0.,0.,0.,0.,1.,0.1,0.1 }, { 0.,0.,0.,0.,0.,0.,1.,0.1 }, { 0.,0.,0.,0.,0.,0.,0.,1. }}}.inverse(), SquareMatrix<f64, 8>{{{1., -0.1, -0.09, -0.081, -0.0729, -0.06561, -0.059049, -0.0531441}, { 0.,1.,-0.1,-0.09,-0.081,-0.0729,-0.06561,-0.059049 }, { 0.,0.,1.,-0.1,-0.09,-0.081,-0.0729,-0.06561 }, { 0.,0.,0.,1.,-0.1,-0.09,-0.081,-0.0729 }, { 0.,0.,0.,0.,1.,-0.1,-0.09,-0.081 }, { 0.,0.,0.,0.,0.,1.,-0.1,-0.09 }, { 0.,0.,0.,0.,0.,0.,1.,-0.1 }, { 0.,0.,0.,0.,0.,0.,0.,1. }}}));
	//solve
	STATIC_ASSERT(almost_equal(Vector<f64, 1>{3.0}, SquareMatrix<f64, 1>{{{2.0}}}.solve({ 6.0 })));
	STATIC_ASSERT(almost_equal(Vector<f64, 2>{1.0, 2.0}, SquareMatrix<f64, 2>{{{ 1.0, 2.0 }, { 3.0, 4.0 }}}.solve({ 5.0, 11.0 })));
	STATIC_ASSERT(almost_equal(Vector<f64, 3>{1.0, 2.0, 3.0}, SquareMatrix<f64, 3>{{{ 2.0, 1.0, 3.0 }, { 0.0, 4.0, 5.0 }, { 7.0, 8.0, 9.0 }}}.solve({ 13.0, 23.0, 50.0 })));
	STATIC_ASSERT(almost_equal(Vector<f64, 4>{1.0, -1.0, 2.0, -2.0}, SquareMatrix<f64, 4>{{{ 1.0, 0.2, 0.3, 0.4 }, { 0.5, 1.0, 0.6, 0.7 }, { 0.8, 0.1, 1.0, 0.9 }, { 0.2, 0.3, 0.4, 1.0 }}}.solve({ 0.6, -0.7, 0.9, -1.3 })));
	STATIC_ASSERT(almost_equal(Vector<f64, 5>{1.0, -2.0, 3.0, -4.0, 5.0}, SquareMatrix<f64, 5>{{{ 1, 4, 5, 9, 7 }, { 3, 8, 2, 9, 8 }, { 3, 3, 2, 9, 3 }, { 5, 8, 6, 2, 4 }, { 5, 1, 3, 5, 3 }}}.solve({ 7.0, -3.0, -18.0, 19.0, 7.0 })));
	STATIC_ASSERT(almost_equal(Vector<f64, 6>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}, SquareMatrix<f64, 6>{{{ 2.0, 0.1, 0.2, 0.3, 0.4, 0.5 }, { 0.3, 2.0, 0.4, 0.5, 0.6, 0.7 }, { 0.5, 0.2, 2.0, 0.6, 0.1, 0.3 }, { 0.7, 0.3, 0.4, 2.0, 0.2, 0.6 }, { 0.2, 0.8, 0.5, 0.4, 2.0, 0.1 }, { 0.6, 0.4, 0.3, 0.2, 0.5, 2.0 }}}.solve({ 9.0, 14.7, 11.6, 15.1, 15.5, 17.6 })));
	STATIC_ASSERT(almost_equal(Vector<f64, 7>{1.0, -1.0, 2.0, -2.0, 3.0, -3.0, 4.0}, SquareMatrix<f64, 7>{{{ 3.0, 0.5, 0.2, 0.1, 0.4, 0.3, 0.2 }, { 0.6, 3.0, 0.5, 0.4, 0.2, 0.1, 0.7 }, { 0.3, 0.6, 3.0, 0.5, 0.4, 0.2, 0.1 }, { 0.2, 0.3, 0.6, 3.0, 0.5, 0.4, 0.2 }, { 0.4, 0.2, 0.3, 0.6, 3.0, 0.5, 0.1 }, { 0.5, 0.3, 0.2, 0.3, 0.6, 3.0, 0.4 }, { 0.7, 0.1, 0.4, 0.2, 0.3, 0.5, 3.0 }}}.solve({ 3.8, 0.9, 5.7, -3.8, 7.5, -5.6, 12.4 })));
	STATIC_ASSERT(almost_equal(Vector<f64, 8>{1.0, 2.0, -1.0, 3.0, -2.0, 4.0, -3.0, 5.0}, SquareMatrix<f64, 8>{{{ 4.0, 0.2, 0.3, 0.1, 0.5, 0.4, 0.6, 0.7 },{ 0.3, 4.0, 0.1, 0.2, 0.6, 0.5, 0.4, 0.8 },{ 0.2, 0.3, 4.0, 0.4, 0.1, 0.6, 0.5, 0.7 },{ 0.5, 0.2, 0.3, 4.0, 0.7, 0.1, 0.6, 0.4 },{ 0.4, 0.6, 0.2, 0.3, 4.0, 0.7, 0.1, 0.5 },{ 0.6, 0.4, 0.5, 0.2, 0.3, 4.0, 0.7, 0.1 },{ 0.7, 0.5, 0.4, 0.6, 0.2, 0.3, 4.0, 0.8 },{ 0.1, 0.7, 0.6, 0.5, 0.4, 0.2, 0.3, 4.0 }}}.solve({ 6.7, 12.4, 2.2, 11.8, -0.7, 15.3, -4.1, 21.5 })));
}