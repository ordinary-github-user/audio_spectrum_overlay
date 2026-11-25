#pragma once
#include "../Util/Inline.h"
#include "../Util/Unreachable.h"
#include "DegreeRadian.h"
namespace Math
{
	template<$Angle Angle>
	struct PitchYaw
	{
		using T = Angle::data_type;
		Angle pitch, yaw;

		//todo: sincos_ps
		constexpr Vector3<T> direction()const noexcept//convention dependent
		{
			const SinCos<T> scPitch{ sincos(pitch) };
			const SinCos<T> scYaw  { sincos(yaw) };
			return 
			{
				scYaw.s * scPitch.c,
				-scPitch.s,
				scYaw.c * scPitch.c
			};
		}

		static constexpr PitchYaw look(const Vector3<T>& relPos)noexcept
		{
			const T lenSqr{ lengthSqr(relPos) };
			if (lenSqr == 0) [[unlikely]]
				return {};
			const T len{ sqrt(lenSqr) };
			return
			{
				.pitch{asin(-relPos[1] / len)},
				.yaw{atan2(relPos[0], relPos[2])}
			};
		}

		//template<$Angle A>
		//constexpr explicit operator PitchYaw<A>()const noexcept
		//{
		//	return { .pitch{static_cast<A>(pitch)}, .yaw{static_cast<A>(yaw)} };
		//}
	};

	template<$Angle AngleL, $Angle AngleR>INLINE constexpr PitchYaw<AngleL>& operator+=(PitchYaw<AngleL>& l, const PitchYaw<AngleR>& r)noexcept { l.pitch += r.pitch; l.yaw += r.yaw; return l; }
	template<$Angle AngleL, $Angle AngleR>INLINE constexpr PitchYaw<AngleL>& operator-=(PitchYaw<AngleL>& l, const PitchYaw<AngleR>& r)noexcept { l.pitch -= r.pitch; l.yaw -= r.yaw; return l; }
	template<$Angle Angle>INLINE constexpr PitchYaw<Angle>& operator*=(PitchYaw<Angle>& l, auto r)noexcept { l.pitch *= r; l.yaw *= r; return l; }
	template<$Angle Angle>INLINE constexpr PitchYaw<Angle>& operator/=(PitchYaw<Angle>& l, auto r)noexcept { l.pitch /= r; l.yaw /= r; return l; }
	template<$Angle Angle>INLINE constexpr PitchYaw<Angle> operator*(auto l, const PitchYaw<Angle>& r)noexcept { return { .pitch{r.pitch * l}, .yaw{r.yaw * l} }; }
	template<$Angle Angle>INLINE constexpr PitchYaw<Angle> operator*(const PitchYaw<Angle>& l, auto r)noexcept { return { .pitch{l.pitch * r}, .yaw{l.yaw * r} }; }
	template<$Angle Angle>INLINE constexpr PitchYaw<Angle> operator/(const PitchYaw<Angle>& l, auto r)noexcept { return { .pitch{l.pitch / r}, .yaw{l.yaw / r} }; }
	template<$Angle Angle>INLINE constexpr PitchYaw<Angle> operator+(const PitchYaw<Angle>& l, const PitchYaw<Angle>& r)noexcept { return { .pitch{l.pitch + r.pitch}, .yaw{l.yaw + r.yaw} }; }
	template<$Angle Angle>INLINE constexpr PitchYaw<Angle> operator-(const PitchYaw<Angle>& l, const PitchYaw<Angle>& r)noexcept { return { .pitch{l.pitch - r.pitch}, .yaw{l.yaw - r.yaw} }; }
	template<$Angle Angle>INLINE constexpr bool operator==(const PitchYaw<Angle>& l, const PitchYaw<Angle>& r)noexcept { return l.pitch == r.pitch && l.yaw == r.yaw; }
	template<$Angle Angle>INLINE constexpr bool operator!=(const PitchYaw<Angle>& l, const PitchYaw<Angle>& r)noexcept { return l.pitch != r.pitch || l.yaw != r.yaw; }

	template<$Angle Angle>
	INLINE constexpr bool isFinite(const PitchYaw<Angle>& angles)noexcept
	{
		return isFinite(static_cast<Angle::data_type>(angles.pitch)) && isFinite(static_cast<Angle::data_type>(angles.yaw));
	}

	template<$Angle To, $Angle AngleFrom>
	INLINE constexpr PitchYaw<To> cast(const PitchYaw<AngleFrom>& from)noexcept
	{
		return
		{
			.pitch{static_cast<To>(from.pitch)},
			.yaw{static_cast<To>(from.yaw)},
		};
	}

	template<$Floating To, $Angle AngleFrom>
	requires($Same<To, typename AngleFrom::data_type>)
	INLINE constexpr Vector2<To> cast(const PitchYaw<AngleFrom>& from)noexcept
	{
		return
		{
			static_cast<To>(from.pitch),
			static_cast<To>(from.yaw)
		};
	}

	template<class To, $Angle AngleFrom>
	requires(sizeof(To) == sizeof(AngleFrom))
	INLINE const Vector2<To>& interpreted_as(const PitchYaw<AngleFrom>& from)noexcept
	{
		return reinterpret_cast<const Vector2<To>&>(from);
	}

	template<class Stream, class T>
	constexpr Stream& operator<<(Stream& os, const PitchYaw<T>& angles)noexcept
	{
		os << '[' << angles.pitch << ',' << angles.yaw << ']';
		return os;
	}

	template<$Angle Angle>
	struct PitchYawRoll
	{
		using T = Angle::data_type;
		Angle pitch, yaw, roll;

		//todo: sincos_ps
		constexpr Vector3<T> direction()const noexcept//convention dependent
		{
			const SinCos<T> scPitch{ sincos(pitch) };
			const SinCos<T> scYaw  { sincos(yaw) };
			return
			{
				scYaw.s * scPitch.c,
				-scPitch.s,
				scYaw.c * scPitch.c
			};
		}

		static constexpr PitchYawRoll look(const Vector3<T>& relPos)noexcept
		{
			const T lenSqr{ lengthSqr(relPos) };
			if (lenSqr == 0) [[unlikely]]
				return {};
			const T len{ sqrt(lenSqr) };
			return
			{
				.pitch{asin(-relPos[1] / len)},
				.yaw{atan2(relPos[0], relPos[2])},
				.roll{}
			};
		}
	};

	template<$Angle AngleL, $Angle AngleR>INLINE constexpr PitchYawRoll<AngleL>& operator+=(PitchYawRoll<AngleL>& l, const PitchYawRoll<AngleR>& r)noexcept { l.pitch += r.pitch; l.yaw += r.yaw; l.roll += r.roll; return l; }
	template<$Angle AngleL, $Angle AngleR>INLINE constexpr PitchYawRoll<AngleL>& operator-=(PitchYawRoll<AngleL>& l, const PitchYawRoll<AngleR>& r)noexcept { l.pitch -= r.pitch; l.yaw -= r.yaw; l.roll -= r.roll; return l; }
	template<$Angle Angle>INLINE constexpr PitchYawRoll<Angle>& operator*=(PitchYawRoll<Angle>& l, auto r)noexcept { l.pitch *= r; l.yaw *= r; l.roll *= r; return l; }
	template<$Angle Angle>INLINE constexpr PitchYawRoll<Angle>& operator/=(PitchYawRoll<Angle>& l, auto r)noexcept { l.pitch /= r; l.yaw /= r; l.roll /= r; return l; }
	template<$Angle Angle>INLINE constexpr PitchYawRoll<Angle> operator*(auto l, const PitchYawRoll<Angle>& r)noexcept { return { .pitch{r.pitch * l}, .yaw{r.yaw * l}, .roll{r.roll * l} }; }
	template<$Angle Angle>INLINE constexpr PitchYawRoll<Angle> operator*(const PitchYawRoll<Angle>& l, auto r)noexcept { return { .pitch{l.pitch * r}, .yaw{l.yaw * r}, .roll{l.roll * r} }; }
	template<$Angle Angle>INLINE constexpr PitchYawRoll<Angle> operator/(const PitchYawRoll<Angle>& l, auto r)noexcept { return { .pitch{l.pitch / r}, .yaw{l.yaw / r}, .roll{l.roll / r} }; }
	template<$Angle Angle>INLINE constexpr PitchYawRoll<Angle> operator+(const PitchYawRoll<Angle>& l, const PitchYawRoll<Angle>& r)noexcept { return { .pitch{l.pitch + r.pitch}, .yaw{l.yaw + r.yaw}, .roll{l.roll + r.roll} }; }
	template<$Angle Angle>INLINE constexpr PitchYawRoll<Angle> operator-(const PitchYawRoll<Angle>& l, const PitchYawRoll<Angle>& r)noexcept { return { .pitch{l.pitch - r.pitch}, .yaw{l.yaw - r.yaw}, .roll{l.roll - r.roll} }; }
	template<$Angle Angle>INLINE constexpr bool operator==(const PitchYawRoll<Angle>& l, const PitchYawRoll<Angle>& r)noexcept { return l.pitch == r.pitch && l.yaw == r.yaw && l.roll == r.roll; }
	template<$Angle Angle>INLINE constexpr bool operator!=(const PitchYawRoll<Angle>& l, const PitchYawRoll<Angle>& r)noexcept { return l.pitch != r.pitch || l.yaw != r.yaw || l.roll != r.roll; }

	template<$Angle Angle>
	INLINE constexpr bool isFinite(const PitchYawRoll<Angle>& angles)noexcept
	{
		return isFinite(static_cast<Angle::data_type>(angles.pitch)) && isFinite(static_cast<Angle::data_type>(angles.yaw))&& isFinite(static_cast<Angle::data_type>(angles.roll));
	}

	template<$Angle To, $Angle AngleFrom>
	INLINE constexpr PitchYawRoll<To> cast(const PitchYawRoll<AngleFrom>& from)noexcept
	{
		return
		{
			.pitch{static_cast<To>(from.pitch)},
			.yaw{static_cast<To>(from.yaw)},
			.roll{static_cast<To>(from.roll)},
		};
	}

	template<$Floating To, $Angle AngleFrom>
	requires($Same<To, typename AngleFrom::data_type>)
	INLINE constexpr Vector3<To> cast(const PitchYawRoll<AngleFrom>& from)noexcept
	{
		return
		{
			static_cast<To>(from.pitch),
			static_cast<To>(from.yaw),
			static_cast<To>(from.roll)
		};
	}

	template<class To, $Angle AngleFrom>
	requires(sizeof(To) == sizeof(AngleFrom))
	INLINE const Vector3<To>& interpreted_as(const PitchYawRoll<AngleFrom>& from)noexcept
	{
		return reinterpret_cast<const Vector3<To>&>(from);
	}

	template<class Stream, class T>
	constexpr Stream& operator<<(Stream& os, const PitchYawRoll<T>& angles)noexcept
	{
		os << '[' << angles.pitch << ',' << angles.yaw << ',' << angles.roll << ']';
		return os;
	}

	template<class T> using degree2 = PitchYaw<Deg<T>>;
	template<class T> using degree3 = PitchYawRoll<Deg<T>>;
	template<class T> using radian2 = PitchYaw<Rad<T>>;
	template<class T> using radian3 = PitchYawRoll<Rad<T>>;

	template<class T>
	struct Quaternion
	{
		T x, y, z, w;

		friend constexpr void normalize(Quaternion& q)noexcept
		{
			const T len{ sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w) };
			q.x /= len;
			q.y /= len;
			q.z /= len;
			q.w /= len;
		}

		friend constexpr Quaternion normalized(const Quaternion& q)noexcept
		{
			const T len{ sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w) };
			return { .x{q.x / len}, .y{q.y / len}, .z{q.z / len}, .w{q.w / len} };
		}

		constexpr Quaternion(T x, T y, T z, T w)noexcept :x{ x }, y{ y }, z{ z }, w{ w } {}

		//todo: sincos_ps
		constexpr Quaternion(const PitchYaw<Rad<T>>& angles)noexcept
		{
			const SinCos<T> scPitch{ sincos(angles.pitch * T(0.5)) };
			const SinCos<T> scYaw  { sincos(angles.yaw * T(0.5)) };
			x = -scPitch.s * scYaw.s;
			y =  scPitch.s * scYaw.c;
			z =  scPitch.c * scYaw.s;
			w =  scPitch.c * scYaw.c;
			normalize(*this);
		}

		//todo: sincos256_ps
		constexpr Quaternion(const PitchYawRoll<Rad<T>>& angles)noexcept
		{
			const SinCos<T> scPitch{ sincos(angles.pitch * T(0.5)) };
			const SinCos<T> scYaw  { sincos(angles.yaw   * T(0.5)) };
			const SinCos<T> scRoll { sincos(angles.roll  * T(0.5)) };
			const T cpcy{ scPitch.c * scYaw.c };
			const T spsy{ scPitch.s * scYaw.s };
			const T cpsy{ scPitch.c * scYaw.s };
			const T spcy{ scPitch.s * scYaw.c };
			x = scRoll.s * cpcy - scRoll.c * spsy;
			y = scRoll.c * spcy + scRoll.s * cpsy;
			z = scRoll.c * cpsy - scRoll.s * spcy;
			w = scRoll.c * cpcy + scRoll.s * spsy;
			normalize(*this);
		}

		constexpr Quaternion(const Vector3<T>& unitAxis, const Rad<T>& angle)noexcept
		{
			const SinCos<T> scHalfAngle{ sincos(angle * T(0.5)) };
			x = scHalfAngle.s * unitAxis[0];
			y = scHalfAngle.s * unitAxis[1];
			z = scHalfAngle.s * unitAxis[2];
			w = scHalfAngle.c;
		}

		constexpr operator PitchYawRoll<Rad<T>>()const noexcept
		{
			// roll (x-axis rotation)
			const T sinr_cosp{ T{2} *(w * x + y * z) };
			const T cosr_cosp{ T{1} - T{2} *(x * x + y * y) };
			const Rad<T> roll{ atan2(sinr_cosp, cosr_cosp) };

			// pitch (y-axis rotation)
			const T sinp{ T{2} *(w * y - z * x) };
			const Rad<T> pitch{ (abs(sinp) >= 1) ? copysign(pi<T> / 2, sinp) : asin(sinp) };

			// yaw (z-axis rotation)
			const T siny_cosp{ T{ 2 } *(w * z + x * y) };
			const T cosy_cosp{ T{ 1 } - T{ 2 } *(y * y + z * z) };
			const Rad<T> yaw{ atan2(siny_cosp, cosy_cosp) };

			return { pitch,yaw,roll };
		}

		template<Vector3<T> forward = Vector3<T>{ 1,0,0 }, Vector3<T> up = Vector3<T>{ 0,1,0 }>
		static constexpr Quaternion look(const Vector3<T>& unitDirection)noexcept
		{
			const T d{ dot(forward, unitDirection) };
			if (d <= T{ -1 })//unitDirection is backward
				return constant<Quaternion{ up, Rad<T>::template circle<T(0.5)>}>;
			else if (d >= T{ 1 })//unitDirection is forward
				return identity();
			else
			{
				const Rad<T> rotAngle{ acos(d) };
				const Vector3<T> rotAxis{ normalized(cross(forward, unitDirection)) };
				return { rotAxis, rotAngle };
			}
		}

		static constexpr Quaternion identity()noexcept { return { .x{},.y{},.z{},.w{1} }; }

		constexpr Quaternion inversed()noexcept { return { -x,-y,-z, w }; }
	};

	template<class Stream, class T>
	constexpr Stream& operator<<(Stream& os, const Quaternion<T>& q)noexcept
	{
		os << '[' << q.w << q.x << q.y << q.z << ']';
		return os;
	}

	template<class T>constexpr bool isFinite(const Quaternion<T>& q)noexcept { return isFinite(q.w) && isFinite(q.x) && isFinite(q.y) && isFinite(q.z); }

	template<class T>
	constexpr Quaternion<T> operator*(const Quaternion<T>& lhs, const Quaternion<T>& rhs)noexcept
	{
		return
		{
			lhs.x * rhs.w + lhs.w * rhs.x + lhs.z * rhs.y - lhs.y * rhs.z,
			lhs.y * rhs.w - lhs.z * rhs.x + lhs.w * rhs.y + lhs.x * rhs.z,
			lhs.z * rhs.w + lhs.y * rhs.x - lhs.x * rhs.y + lhs.w * rhs.z,
			lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
		};
	}

	template<class T>
	constexpr Quaternion<T>& operator*=(Quaternion<T>& lhs, const Quaternion<T>& rhs)noexcept
	{
		lhs.x = lhs.x * rhs.w + lhs.w * rhs.x + lhs.z * rhs.y - lhs.y * rhs.z;
		lhs.y = lhs.y * rhs.w - lhs.z * rhs.x + lhs.w * rhs.y + lhs.x * rhs.z;
		lhs.z = lhs.z * rhs.w + lhs.y * rhs.x - lhs.x * rhs.y + lhs.w * rhs.z;
		lhs.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
		return lhs;
	}

	template<class T>
	constexpr Vector3<T> operator*(const Quaternion<T>& quaternion, const Vector3<T>& point)noexcept
	{
		const Vector3<T> qvec{ quaternion.x, quaternion.y, quaternion.z };
		Vector3<T> result{ cross(qvec, point) };
		const Vector3<T> uuv{ T{2} *cross(qvec, result) };
		result *= (T{ 2 } *quaternion.w);
		result += uuv;
		result += point;
		return result;
	}
}