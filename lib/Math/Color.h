#pragma once
#include <type_traits>
#include "../Util/Algorithm/MinMax.h"
#include "../Util/BasicTypes.h"
#include "../Util/Compiler.h"
#include "../Util/Concepts/Floating.h"
#include "../Util/Macro/Consteval.h"
#include "../Util/Macro/If.h"
#include "../Util/Traits/Like.h"
#include "../Util/Unreachable.h"
#include "Numeric/Fmod.h"

template<$Floating T>struct Hsla;
//template<>struct Rgba<u8>;

//r [0,1]
//g [0,1]
//b [0,1]
//a [0,1]
template<$Floating T>
struct Rgba
{
protected:
	static constexpr T pqt2rgb(T p, T q, T t)noexcept
	{
		if (t < 0) t += T(1);
		if (t > 1) t -= T(1);
		if (t < T(1) / T(6)) return (p + (q - p) * T(6) * t);
		if (t < T(1) / T(2)) return q;
		if (t < T(2) / T(3)) return (p + (q - p) * (T(2) / T(3) - t) * T(6));
		return p;
	}
public:
	T r, g, b, a;

	//constexpr Rgba(const Rgba<u8>& rhs) :r{ rhs.r / static_cast<T>(0xff) }, g{ rhs.g / static_cast<T>(0xff) }, b{ rhs.b / static_cast<T>(0xff) }, a{ rhs.a / static_cast<T>(0xff) } {}

	constexpr Rgba()noexcept: r{}, g{}, b{}, a{} {}

	constexpr Rgba(T red, T green, T blue, T alpha)noexcept : r{red}, g{green}, b{blue}, a{alpha} {}

	template<class U>
	constexpr Rgba(const Hsla<U>& hsla)noexcept : a{hsla.a}
	{
		const U s{hsla.s};
		const U l{hsla.l};
		if (s == 0)
		{
			r = l;
			g = l;
			b = l;
		}
		else
		{
			const U h{Math::fmod(hsla.h,U(1))};
			const U q{l < U(0.5) ? l * (U(1) + s) : l + s - l * s};
			const U p{U(2) * l - q};
			r = pqt2rgb(p, q, h + (U(1) / U(3)));
			g = pqt2rgb(p, q, h);
			b = pqt2rgb(p, q, h - (U(1) / U(3)));
		}
	}

	constexpr Rgba tint(const Rgba& color, T ratio)const noexcept
	{
		if (ratio <= 0)return *this;
		if (ratio >= 1)return {color.r,color.g,color.b,a};
		return 
		{
			r * (1 - ratio) + color.r * ratio,
			g * (1 - ratio) + color.g * ratio,
			b * (1 - ratio) + color.b * ratio,
			a
		};
	}

	constexpr Rgba operator+(const Rgba& rhs)const { return {r + rhs.r,g + rhs.g,b + rhs.b,a + rhs.a}; }
	constexpr Rgba operator-(const Rgba& rhs)const { return {r - rhs.r,g - rhs.g,b - rhs.b,a - rhs.a}; }
	constexpr Rgba operator*(auto x)const noexcept { return { r * x, g * x, b * x, a * x }; }

	constexpr bool operator==(const Rgba& rhs)const noexcept { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
	constexpr bool operator!=(const Rgba& rhs)const noexcept { return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a; }

	template<class Self>constexpr LikePtr<Self, T> data(this Self&& self) noexcept{ return &self.r; };
	template<class Self>constexpr LikePtr<Self, T> begin(this Self&& self) noexcept{ return &self.r; };
	template<class Self>constexpr LikePtr<Self, T> end(this Self&& self) noexcept{ return self.begin() + 4; };

	template<class Self>
	constexpr Like<Self, T> operator[](this Self&& self, u8 index) noexcept
	{
		switch (index)
		{
			case 0:return FWD(self).r;
			case 1:return FWD(self).g;
			case 2:return FWD(self).b;
			case 3:return FWD(self).a;
			default:UNREACHABLE;
		}
	}

	static constexpr u8 size()noexcept{ return 4; };

	constexpr Rgba premuled()const noexcept { return { r * a,g * a,b * a,a }; }

	template<class Stream>
	constexpr friend Stream& operator<<(Stream& os, const Rgba& value)noexcept
	{
		os << '<' << value.r << ',' << value.g << ',' << value.b << ',' << value.a << '>';
		return os;
	}
};

//template<class T>struct Argb;
//
//template<>
//struct Argb<u8>
//{
//	u8 a, r, g, b;
//	constexpr Argb()noexcept :a{}, r{}, g{}, b{} {};
//
//	constexpr Argb(u8 alpha, u8 red, u8 green, u8 blue)noexcept :a{alpha}, r{red}, g{green}, b{blue} {};
//
//	template<class U>
//	constexpr Argb(const Rgba<U>& rgba)noexcept :a{static_cast<u8>(rgba.a * 255.f)}, r{ static_cast<u8>(rgba.r * 255.f) }, g{ static_cast<u8>(rgba.g * 255.f) }, b{ static_cast<u8>(rgba.b * 255.f) } {};
//};

template<class T>struct Bgra;

template<>
struct Bgra<u8>
{
	u8 b,g,r,a;
	constexpr Bgra()noexcept :b{}, g{}, r{}, a{} {};

	constexpr Bgra(u8 blue, u8 green, u8 red, u8 alpha)noexcept : b{blue}, g{ green }, r{ red }, a{ alpha } {};

	template<class U>
	constexpr Bgra(const Rgba<U>& rgba)noexcept :b{static_cast<u8>(rgba.b * 255.f)}, g{ static_cast<u8>(rgba.g * 255.f) }, r{ static_cast<u8>(rgba.r * 255.f) }, a{ static_cast<u8>(rgba.a * 255.f) } {};
};

////r [0,255]
////g [0,255]
////b [0,255]
////a [0,255]
//template<>
//struct Rgba<u8>
//{
//protected:
//	using base_t = Rgba<u8>;
//public:
//	u8 r, g, b, a;
//	constexpr Rgba() :r{ 0 }, g{ 0 }, b{ 0 }, a{ 1 } {}
//	constexpr Rgba(const base_t&) = default;
//	constexpr Rgba(base_t&&) = default;
//	constexpr base_t& operator=(const base_t&) = default;
//	constexpr base_t& operator=(base_t&&) = default;
//	constexpr base_t& operator=(u32 rhs) { *(u32*)this = rhs; return *this; }
//	constexpr Rgba(u32 rhs) :r{ static_cast<u8>(rhs << 24) }, g{ static_cast<u8>(rhs << 16) }, b{ static_cast<u8>(rhs << 8) }, a{ static_cast<u8>(rhs) } {}
//	template<class U>constexpr Rgba(const Rgba<U>& rhs) : r{ static_cast<u8>(rhs.r * 0xff) }, g{ static_cast<u8>(rhs.g * 0xff) }, b{ static_cast<u8>(rhs.b * 0xff) }, a{ static_cast<u8>(rhs.a * 0xff) } {}
//
//	template<class U>constexpr Rgba(const Hsla<U>& hsla) : Rgba{ Rgba<f32>{hsla} } {}
//
//	template<class Stream>friend Stream& operator<<(Stream& os, const base_t& value) { return os << '<' << value.r << ',' << value.g << ',' << value.b << ',' << value.a << '>'; }
//};

//h [0,1]
//s [0,1]
//l [0,1]
//a [0,1]
template<$Floating T>
struct Hsla
{
	T h, s, l, a;

	constexpr Hsla()noexcept:h{}, s{1}, l{T(0.5)}, a{1} {}
	//constexpr Hsla(T hue = 0)noexcept:h{hue}, s{1}, l{T(0.5)}, a{1} {}

	constexpr Hsla(T hue, T saturation, T lightness, T alpha)noexcept : h{hue}, s{saturation}, l{lightness}, a{alpha} {}

	template<$Floating U>
	constexpr Hsla(const Rgba<U>& rgba)noexcept : a{static_cast<T>(rgba.a)}
	{
		const T maxComp{static_cast<T>(Algorithm::max(rgba.r,rgba.g,rgba.b))};
		const T minComp{static_cast<T>(Algorithm::min(rgba.r,rgba.g,rgba.b))};
		l = (maxComp + minComp) / T{2};
		if (maxComp == minComp)
		{
			h = 0;
			s = 0;
		}
		else
		{
			const T d{maxComp - minComp};
			s = l > T{0.5} ? d / (T{2} - maxComp - minComp) : d / (maxComp + minComp);

			if (maxComp == rgba.r)
				h = ((rgba.g - rgba.b) / d + (rgba.g < rgba.b ? T{ 6 } : T{ 0 })) / T{6};
			else if (maxComp == rgba.g)
				h = ((rgba.b - rgba.r) / d + T{2}) / T{6};
			else if (maxComp == rgba.b)
				h = ((rgba.r - rgba.g) / d + T{4}) / T{6};
		}
	}

	constexpr Hsla& rotate(T value)noexcept { h += value; return *this; }

	template<class Self>constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return &self.h; };
	template<class Self>constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return &self.h; };
	template<class Self>constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + 4; };

	template<class Self>
	constexpr Like<Self, T> operator[](this Self&& self, u8 index) noexcept
	{
		switch (index)
		{
			case 0:return FWD(self).h;
			case 1:return FWD(self).s;
			case 2:return FWD(self).l;
			case 3:return FWD(self).a;
			default:UNREACHABLE;
		}
	}

	static constexpr u8 size()noexcept { return 4; }

	template<class Stream>
	constexpr friend Stream& operator<<(Stream& os, const Hsla& value)noexcept
	{
		os << '<' << value.h << ',' << value.s << ',' << value.l << ',' << value.a << '>';
		return os;
	}
};