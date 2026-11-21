#pragma once
#include "ConceptFloating.h"
#include "ConceptSame.h"
#include "Constant.h"
#include "DelayInstantiation.h"
#include "Fmod.h"
#include "Inline.h"
#include "IsFinite.h"
#include "MacroLexi.h"
#include "MathConstant.h"
#include "StaticError.h"
#include "Tensor.h"
#include "Trig.h"
namespace Math
{
	template<$Floating T>struct Deg;
	template<$Floating T>struct Rad;

	template<class>struct is_radian { static constexpr bool value{}; };
	template<class T>struct is_radian<Rad<T>> { static constexpr bool value{ true }; };
	template<class T>concept $Radian = is_radian<std::remove_cvref_t<T>>::value;

	template<class>struct is_degree { static constexpr bool value{}; };
	template<class T>struct is_degree<Deg<T>> { static constexpr bool value{ true }; };
	template<class T>concept $Degree = is_degree<std::remove_cvref_t<T>>::value;

	template<class T>concept $Angle = is_radian<std::remove_cvref_t<T>>::value || is_degree<std::remove_cvref_t<T>>::value;

	template<$Floating T>
	struct Rad
	{
	//protected:
		T m_data;
	//public:
		using data_type = T;

		INLINE constexpr Rad()noexcept :m_data{} {}
		INLINE constexpr Rad(T rhs)noexcept : m_data{ rhs } {}
		template<$Floating U>INLINE constexpr Rad(Rad<U> rhs)noexcept : m_data{ rhs.m_data } {}
		template<$Floating U>INLINE constexpr Rad(Deg<U> deg)noexcept : m_data{ static_cast<U>(deg) * (pi<T> / T(180)) } {}

		INLINE constexpr explicit operator T& ()noexcept { return m_data; }
		INLINE constexpr explicit operator const T& ()const noexcept { return m_data; }
		INLINE constexpr explicit operator bool()const noexcept { return m_data != T{}; }

		template<$Angle A>
		INLINE friend constexpr A cast(Rad rad)noexcept
		{
			using U = A::data_type;
			return Rad<U>{static_cast<U>(rad.m_data)};
		}

		template<T ratio = T{ 1 }> static constexpr Rad<delay_instantiation<T, ratio>> circle{ constant<ratio * tau<T>> };
		DEFINE_LEXI(m_data)

		static constexpr Rad<T> fromVector(const Vector2<T>& vec)noexcept { return Trig::atan2(vec[1], vec[0]); }
		constexpr Vector2<T> toUnitVector()const noexcept 
		{
			const SinCos<T> sc{ Trig::sincos(m_data) };
			return { sc.c, sc.s }; 
		}

		//[0,2pi)
		constexpr Rad<T>& normalize()noexcept
		{
			m_data = Math::fmod(m_data, Math::tau<T>);
			if (m_data < T{})
				m_data += Math::tau<T>;
			return *this;
		}

		//[0,2pi)
		constexpr Rad<T> normalized()const noexcept
		{
			Rad<T> result{*this};
			result.normalize();
			return result;
		}

		template<class Stream>
		constexpr friend Stream& operator<<(Stream& os, Rad<T> rad) noexcept
		{
			if constexpr ($Same<c16, typename Stream::char_type>)
				os << static_cast<T>(rad) * (T{ 180 } / pi<T>) << L'°';
			else
				STATIC_ERROR(not implemented); //os << static_cast<T>(rad) * (T{ 180 } / pi<T>) << TS("°");
			return os;
		}
	};
	template<$Floating T>Rad(Deg<T>) -> Rad<T>;

	template<$Floating T>INLINE constexpr T operator/(Rad<T> lhs, Rad<T> rhs)noexcept { return static_cast<T>(lhs) / static_cast<T>(rhs); }
	template<$Floating T, class U>INLINE constexpr Rad<T> operator/(Rad<T> lhs, U rhs)noexcept { return static_cast<Rad<T>>(static_cast<T>(lhs) / rhs); }
	template<$Floating T, class U>INLINE constexpr Rad<T> operator*(Rad<T> lhs, U rhs)noexcept { return static_cast<Rad<T>>(static_cast<T>(lhs) * rhs); }
	template<$Floating T, class U>INLINE constexpr Rad<T> operator*(U lhs, Rad<T> rhs)noexcept { return rhs * lhs; }
	template<$Floating T>INLINE constexpr Rad<T> operator+(Rad<T> lhs, Rad<T> rhs)noexcept { return static_cast<Rad<T>>(static_cast<T>(lhs) + static_cast<T>(rhs)); }
	template<$Floating T>INLINE constexpr Rad<T> operator-(Rad<T> lhs, Rad<T> rhs)noexcept { return static_cast<Rad<T>>(static_cast<T>(lhs) - static_cast<T>(rhs)); }
	template<$Floating T, $Floating U>INLINE constexpr Rad<T>& operator+=(Rad<T>& lhs, Rad<U> rhs)noexcept { lhs.m_data += rhs.m_data; return lhs; }
	template<$Floating T, $Floating U>INLINE constexpr Rad<T>& operator-=(Rad<T>& lhs, Rad<U> rhs)noexcept { lhs.m_data -= rhs.m_data; return lhs; }
	template<$Floating T, class U>INLINE constexpr Rad<T>& operator*=(Rad<T>& lhs, U rhs)noexcept { lhs.m_data *= rhs; return lhs; }
	template<$Floating T, class U>INLINE constexpr Rad<T>& operator/=(Rad<T>& lhs, U rhs)noexcept { lhs.m_data /= rhs; return lhs; }
	template<$Floating T>INLINE constexpr Rad<T> operator-(Rad<T> x)noexcept { return -static_cast<T>(x); }
	template<$Floating T>INLINE constexpr Rad<T> operator+(Rad<T> x)noexcept { return +static_cast<T>(x); }

	template<$Floating T>
	struct Deg
	{
	//protected:
		T m_data;
	//public:
		using data_type = T;

		INLINE constexpr Deg()noexcept :m_data{} {}
		INLINE constexpr Deg(T rhs)noexcept : m_data{ rhs } {}
		template<$Floating U>INLINE constexpr Deg(Deg<U> rhs)noexcept : m_data{ rhs.m_data } {}
		template<$Floating U>INLINE constexpr Deg(Rad<U> rad)noexcept : m_data{ static_cast<U>(rad) * (T(180) / pi<T>) } {}

		INLINE constexpr explicit operator T& ()noexcept { return m_data; }
		INLINE constexpr explicit operator const T& ()const noexcept { return m_data; }
		INLINE constexpr explicit operator bool()const noexcept { return m_data != T{}; }

		template<$Angle A>
		INLINE friend constexpr A cast(Deg deg)noexcept
		{
			using U = A::data_type;
			return Deg<U>{static_cast<U>(deg.m_data)};
		}

		template<T ratio = T{1}>static constexpr Deg<delay_instantiation<T, ratio>> circle{ constant<T{ 360 } * ratio> };

		DEFINE_LEXI(m_data)

		static constexpr Deg<T> fromVector(const Vector2<T>& vec)noexcept { return Rad<T>::fromVector(vec); }
		constexpr Vector2<T> toUnitVector()const noexcept { return Rad<T>{*this}.toUnitVector(); }

		template<class Stream>
		constexpr friend Stream& operator<<(Stream& os, Deg<T> deg) noexcept
		{
			if constexpr ($Same<c16, typename Stream::char_type>)
				os << deg.m_data << L'°';
			else
				STATIC_ERROR(not implemented);//os << deg.m_data << TS("°");
			return os;
		}
	};
	template<$Floating T>Deg(Rad<T>) -> Deg<T>;

	template<$Floating T>INLINE constexpr T operator/(Deg<T> lhs, Deg<T> rhs)noexcept { return static_cast<T>(lhs) / static_cast<T>(rhs); }
	template<$Floating T, class U>INLINE constexpr Deg<T> operator/(Deg<T> lhs, U rhs)noexcept { return static_cast<Deg<T>>(static_cast<T>(static_cast<T>(lhs) / rhs)); }
	template<$Floating T, class U>INLINE constexpr Deg<T> operator*(Deg<T> lhs, U rhs)noexcept { return static_cast<Deg<T>>(static_cast<T>(static_cast<T>(lhs) * rhs)); }
	template<$Floating T, class U>INLINE constexpr Deg<T> operator*(U lhs, Deg<T> rhs)noexcept { return rhs * lhs; }
	template<$Floating T>INLINE constexpr Deg<T> operator+(Deg<T> lhs, Deg<T> rhs)noexcept { return static_cast<Deg<T>>(static_cast<T>(lhs) + static_cast<T>(rhs)); }
	template<$Floating T>INLINE constexpr Deg<T> operator-(Deg<T> lhs, Deg<T> rhs)noexcept { return static_cast<Deg<T>>(static_cast<T>(lhs) - static_cast<T>(rhs)); }
	template<$Floating T, $Floating U>INLINE constexpr Deg<T>& operator+=(Deg<T>& lhs, Deg<U> rhs)noexcept { lhs.m_data += rhs.m_data; return lhs; }
	template<$Floating T, $Floating U>INLINE constexpr Deg<T>& operator-=(Deg<T>& lhs, Deg<U> rhs)noexcept { lhs.m_data -= rhs.m_data; return lhs; }
	template<$Floating T, class U>INLINE constexpr Deg<T>& operator*=(Deg<T>& lhs, U rhs)noexcept { lhs.m_data *= rhs; return lhs; }
	template<$Floating T, class U>INLINE constexpr Deg<T>& operator/=(Deg<T>& lhs, U rhs)noexcept { lhs.m_data /= rhs; return lhs; }
	template<$Floating T>INLINE constexpr Deg<T> operator-(Deg<T> x)noexcept { return -static_cast<T>(x); }
	template<$Floating T>INLINE constexpr Deg<T> operator+(Deg<T> x)noexcept { return +static_cast<T>(x); }

	template<$Floating T>INLINE constexpr T sin(Rad<T> rad)noexcept { return Trig::sin(static_cast<T>(rad)); }
	template<$Floating T>INLINE constexpr T cos(Rad<T> rad)noexcept { return Trig::cos(static_cast<T>(rad)); }
	template<$Floating T>INLINE constexpr T tan(Rad<T> rad)noexcept { return Trig::tan(static_cast<T>(rad)); }
	template<$Floating T>INLINE constexpr SinCos<T> sincos(Rad<T> rad)noexcept { return Trig::sincos(static_cast<T>(rad)); }

	template<$Floating T>INLINE constexpr T sin(Deg<T> deg)noexcept { return Math::sin(static_cast<Rad<T>>(deg)); }
	template<$Floating T>INLINE constexpr T cos(Deg<T> deg)noexcept { return Math::cos(static_cast<Rad<T>>(deg)); }
	template<$Floating T>INLINE constexpr T tan(Deg<T> deg)noexcept { return Math::tan(static_cast<Rad<T>>(deg)); }
	template<$Floating T>INLINE constexpr SinCos<T> sincos(Deg<T> deg)noexcept { return Math::sincos(static_cast<Rad<T>>(deg)); }

	template<$Floating T>INLINE constexpr Rad<T> asin(T ratio)noexcept { return Trig::asin(ratio); }
	template<$Floating T>INLINE constexpr Rad<T> acos(T ratio)noexcept { return Trig::acos(ratio); }
	template<$Floating T>INLINE constexpr Rad<T> atan(T ratio)noexcept { return Trig::atan(ratio); }
	template<$Floating T>INLINE constexpr Rad<T> atan2(T y, T x)noexcept { return Trig::atan2(y, x); }

	template<$Floating T>INLINE constexpr bool isFinite(Deg<T> x)noexcept { return isFinite(T{ x }); }
	template<$Floating T>INLINE constexpr bool isFinite(Rad<T> x)noexcept { return isFinite(T{ x }); }
}