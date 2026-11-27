#pragma once
#include <type_traits>
#include "../Util/Algorithm/MinMax.h"
#include "../Util/Assert.h"
#include "../Util/BasicTypes.h"
#include "../Util/Concepts/Floating.h"
#include "../Util/Macro/Lexi.h"
#include "../Util/Traits/Like.h"
#include "../Util/Unreachable.h"
#include "Numeric/Fmod.h"

template<$Floating>struct Hsla;
template<$Floating>struct Hsl;
template<$Floating>struct Rgba;
template<$Floating>struct Rgb;
//template<>struct Rgba<u8>;

template<$Floating T>
constexpr T pqt2rgb(T p, T q, T t)noexcept
{
	if (t < T{ 0 }) t += T{ 1 };
	if (t > T{ 1 }) t -= T{ 1 };
	if (t < T{ 1 } / T{ 6 }) return (p + (q - p) * T{ 6 } * t);
	if (t < T{ 1 } / T{ 2 }) return q;
	if (t < T{ 2 } / T{ 3 }) return (p + (q - p) * (T{ 2 } / T{ 3 } - t) * T{ 6 });
	return p;
}

template<$Floating T>
struct Rgb
{
public:
	T r, g, b;
	DEFINE_LEXI(r,g,b);

	constexpr Rgb()noexcept = default;
	constexpr Rgb(T red, T green, T blue)noexcept : r{red}, g{green}, b{blue}
	{
		ASSERT(r >= T{ 0 } && r <= T{ 1 });
		ASSERT(g >= T{ 0 } && g <= T{ 1 });
		ASSERT(b >= T{ 0 } && b <= T{ 1 });
	}
	constexpr Rgb(const Rgba<T>& rgba)noexcept: r{rgba.r},g{rgba.g},b{rgba.b} {}
	constexpr Rgb(const Hsl<T>& hsl)noexcept
	{
		const T s{hsl.s};
		const T l{hsl.l};
		if (s == 0) { r = l; g = l; b = l; }
		else
		{
			const T h{Math::fmod_positive(hsl.h,T{ 1 })};
			const T q{l < T(0.5) ? l * (T{ 1 } + s) : l + s - l * s};
			const T p{T{ 2 } * l - q};
			r = pqt2rgb(p, q, h + (T{ 1 } / T{ 3 }));
			g = pqt2rgb(p, q, h);
			b = pqt2rgb(p, q, h - (T{ 1 } / T{ 3 }));
		}
	}

	constexpr Rgb tint(const Rgb& color, T ratio)const noexcept
	{
		if (ratio <= 0)return *this;
		if (ratio >= 1)return {color.r,color.g,color.b};
		return
		{
			r * (1 - ratio) + color.r * ratio,
			g * (1 - ratio) + color.g * ratio,
			b * (1 - ratio) + color.b * ratio
		};
	}

	static constexpr u8 size()noexcept{ return 3; };
	template<class Self>constexpr LikePtr<Self, T> data(this Self&& self) noexcept{ return &self.r; };
	template<class Self>constexpr LikePtr<Self, T> begin(this Self&& self) noexcept{ return &self.r; };
	template<class Self>constexpr LikePtr<Self, T> end(this Self&& self) noexcept{ return self.begin() + size(); };//ub

	template<class Self>
	constexpr Like<Self, T> operator[](this Self&& self, u8 index) noexcept
	{
		switch (index)
		{
			case 0:return FWD_LIKE(self, self.r);
			case 1:return FWD_LIKE(self, self.g);
			case 2:return FWD_LIKE(self, self.b);
			default:UNREACHABLE;
		}
	}

	template<class Stream>constexpr friend Stream& operator<<(Stream& os, const Rgb& value)noexcept { os << '<' << value.r << ',' << value.g << ',' << value.b << '>'; return os; }
};

template<$Floating T>
struct Rgba
{
	T r, g, b, a;
	DEFINE_LEXI(r,g,b,a);
	//constexpr Rgba(const Rgba<u8>& rhs) :r{ rhs.r / static_cast<T>(0xff) }, g{ rhs.g / static_cast<T>(0xff) }, b{ rhs.b / static_cast<T>(0xff) }, a{ rhs.a / static_cast<T>(0xff) } {}

	constexpr Rgba()noexcept = default;
	constexpr Rgba(T red, T green, T blue, T alpha)noexcept : r{red}, g{green}, b{blue}, a{alpha}
	{
		ASSERT(r >= T{ 0 } && r <= T{ 1 });
		ASSERT(g >= T{ 0 } && g <= T{ 1 });
		ASSERT(b >= T{ 0 } && b <= T{ 1 });
		ASSERT(a >= T{ 0 } && a <= T{ 1 });
	}

	constexpr Rgba(const Hsla<T>& hsla)noexcept : a{hsla.a}
	{
		const T s{hsla.s};
		const T l{hsla.l};
		if (s == 0) { r = l; g = l; b = l; }
		else
		{
			const T h{Math::fmod_positive(hsla.h,T{ 1 })};
			const T q{l < T(0.5) ? l * (T{ 1 } + s) : l + s - l * s};
			const T p{T{ 2 } * l - q};
			r = pqt2rgb(p, q, h + (T{ 1 } / T{ 3 }));
			g = pqt2rgb(p, q, h);
			b = pqt2rgb(p, q, h - (T{ 1 } / T{ 3 }));
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

	static constexpr u8 size()noexcept{ return 4; };
	template<class Self>constexpr LikePtr<Self, T> data(this Self&& self) noexcept{ return &self.r; };
	template<class Self>constexpr LikePtr<Self, T> begin(this Self&& self) noexcept{ return &self.r; };
	template<class Self>constexpr LikePtr<Self, T> end(this Self&& self) noexcept{ return self.begin() + size(); };//ub

	template<class Self>
	constexpr Like<Self, T> operator[](this Self&& self, u8 index) noexcept
	{
		switch (index)
		{
			case 0:return FWD_LIKE(self, self.r);
			case 1:return FWD_LIKE(self, self.g);
			case 2:return FWD_LIKE(self, self.b);
			case 3:return FWD_LIKE(self, self.a);
			default:UNREACHABLE;
		}
	}

	constexpr Rgba premuled()const noexcept { return { r * a,g * a,b * a,a }; }

	template<class Stream>constexpr friend Stream& operator<<(Stream& os, const Rgba& value)noexcept { os << '<' << value.r << ',' << value.g << ',' << value.b << ',' << value.a << '>'; return os; }
};

//template<class T>struct Argb;
//
//template<>
//struct Argb<u8>
//{
//	u8 a, r, g, b;
//	constexpr Argb()noexcept = default;
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
	constexpr Bgra()noexcept = default;

	constexpr Bgra(u8 blue, u8 green, u8 red, u8 alpha)noexcept : b{blue}, g{ green }, r{ red }, a{ alpha } {};

	template<$Floating U>
	constexpr Bgra(const Rgba<U>& rgba)noexcept :b{static_cast<u8>(rgba.b * 255.f)}, g{ static_cast<u8>(rgba.g * 255.f) }, r{ static_cast<u8>(rgba.r * 255.f) }, a{ static_cast<u8>(rgba.a * 255.f) } {};
};

////r [0,255]
////g [0,255]
////b [0,255]
////a [0,255]
//template<>
//struct Rgba<u8>
//{
//	u8 r, g, b, a;
//	constexpr Rgba() :r{ 0 }, g{ 0 }, b{ 0 }, a{ 1 } {}
//	constexpr Rgba(const Rgba&) = default;
//	constexpr Rgba(Rgba&&) = default;
//	constexpr Rgba& operator=(const Rgba&) = default;
//	constexpr Rgba& operator=(Rgba&&) = default;
//	constexpr Rgba& operator=(u32 rhs) { *(u32*)this = rhs; return *this; }
//	constexpr Rgba(u32 rhs) :r{ static_cast<u8>(rhs << 24) }, g{ static_cast<u8>(rhs << 16) }, b{ static_cast<u8>(rhs << 8) }, a{ static_cast<u8>(rhs) } {}
//	template<class U>constexpr Rgba(const Rgba<U>& rhs) : r{ static_cast<u8>(rhs.r * 0xff) }, g{ static_cast<u8>(rhs.g * 0xff) }, b{ static_cast<u8>(rhs.b * 0xff) }, a{ static_cast<u8>(rhs.a * 0xff) } {}
//
//	template<class U>constexpr Rgba(const Hsla<U>& hsla) : Rgba{ Rgba<f32>{hsla} } {}
//
//	template<class Stream>friend Stream& operator<<(Stream& os, const Rgba& value) { return os << '<' << value.r << ',' << value.g << ',' << value.b << ',' << value.a << '>'; }
//};

template<$Floating T>
struct Hsl
{
	T h, s, l;

	constexpr Hsl()noexcept = default;
	constexpr Hsl(T hue, T saturation, T lightness)noexcept : h{hue}, s{saturation}, l{lightness}
	{
		ASSERT(h >= T{ 0 } && h <= T{ 1 });
		ASSERT(s >= T{ 0 } && s <= T{ 1 });
		ASSERT(l >= T{ 0 } && l <= T{ 1 });
	}
	constexpr Hsl(const Hsla<T>& hsla)noexcept: h{hsla.h}, s{hsla.s}, l{hsla.l} {}
	constexpr Hsl(const Rgb<T>& rgb)noexcept
	{
		const T maxComp{static_cast<T>(Algorithm::max(rgb.r,rgb.g,rgb.b))};
		const T minComp{static_cast<T>(Algorithm::min(rgb.r,rgb.g,rgb.b))};
		l = (maxComp + minComp) / T{ 2 };
		if (maxComp == minComp) { h = 0; s = 0; }
		else
		{
			const T d{maxComp - minComp};
			s = l > T(0.5) ? d / (T{ 2 } - maxComp - minComp) : d / (maxComp + minComp);
			/* */if (maxComp == rgb.r)h = ((rgb.g - rgb.b) / d + (rgb.g < rgb.b ? T{ 6 } : T{ 0 })) / T{ 6 };
			else if (maxComp == rgb.g)h = ((rgb.b - rgb.r) / d + T{ 2 }) / T{ 6 };
			else if (maxComp == rgb.b)h = ((rgb.r - rgb.g) / d + T{ 4 }) / T{ 6 };
		}
	}

	constexpr Hsl& rotate(T value)noexcept { h += value; return *this; }

	static constexpr u8 size()noexcept { return 3; }
	template<class Self>constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return &self.h; };
	template<class Self>constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return &self.h; };
	template<class Self>constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + size(); };//ub

	template<class Self>
	constexpr Like<Self, T> operator[](this Self&& self, u8 index) noexcept
	{
		switch (index)
		{
			case 0:return FWD_LIKE(self, self.h);
			case 1:return FWD_LIKE(self, self.s);
			case 2:return FWD_LIKE(self, self.l);
			default:UNREACHABLE;
		}
	}

	template<class Stream>constexpr friend Stream& operator<<(Stream& os, const Hsl& value)noexcept { os << '<' << value.h << ',' << value.s << ',' << value.l << '>'; return os; }
};

template<$Floating T>
struct Hsla
{
	T h, s, l, a;

	constexpr Hsla()noexcept = default;
	constexpr Hsla(T hue, T saturation, T lightness, T alpha)noexcept : h{hue}, s{saturation}, l{lightness}, a{alpha}
	{
		ASSERT(h >= T{ 0 } && h <= T{ 1 });
		ASSERT(s >= T{ 0 } && s <= T{ 1 });
		ASSERT(l >= T{ 0 } && l <= T{ 1 });
		ASSERT(a >= T{ 0 } && a <= T{ 1 });
	}

	constexpr Hsla(const Rgba<T>& rgba)noexcept : a{static_cast<T>(rgba.a)}
	{
		const T maxComp{static_cast<T>(Algorithm::max(rgba.r,rgba.g,rgba.b))};
		const T minComp{static_cast<T>(Algorithm::min(rgba.r,rgba.g,rgba.b))};
		l = (maxComp + minComp) / T{ 2 };
		if (maxComp == minComp) { h = 0; s = 0; }
		else
		{
			const T d{maxComp - minComp};
			s = l > T(0.5) ? d / (T{ 2 } - maxComp - minComp) : d / (maxComp + minComp);
			/* */if (maxComp == rgba.r)h = ((rgba.g - rgba.b) / d + (rgba.g < rgba.b ? T{ 6 } : T{ 0 })) / T{ 6 };
			else if (maxComp == rgba.g)h = ((rgba.b - rgba.r) / d + T{ 2 }) / T{ 6 };
			else if (maxComp == rgba.b)h = ((rgba.r - rgba.g) / d + T{ 4 }) / T{ 6 };
		}
	}

	constexpr Hsla& rotate(T value)noexcept { h += value; return *this; }

	static constexpr u8 size()noexcept { return 4; }
	template<class Self>constexpr LikePtr<Self, T> data(this Self&& self) noexcept { return &self.h; };
	template<class Self>constexpr LikePtr<Self, T> begin(this Self&& self) noexcept { return &self.h; };
	template<class Self>constexpr LikePtr<Self, T> end(this Self&& self) noexcept { return self.begin() + size(); };//ub
	template<class Self>
	constexpr Like<Self, T> operator[](this Self&& self, u8 index) noexcept
	{
		switch (index)
		{
			case 0:return FWD_LIKE(self, self.h);
			case 1:return FWD_LIKE(self, self.s);
			case 2:return FWD_LIKE(self, self.l);
			case 3:return FWD_LIKE(self, self.a);
			default:UNREACHABLE;
		}
	}
	template<class Stream>constexpr friend Stream& operator<<(Stream& os, const Hsla& value)noexcept { os << '<' << value.h << ',' << value.s << ',' << value.l << ',' << value.a << '>'; return os; }
};