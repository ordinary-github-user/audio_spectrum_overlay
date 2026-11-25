#pragma once
#include "../BasicTypes.h"
#include "../Concepts/FixSized.h"
#include "../Concepts/Integral.h"
#include "../Concepts/LinearContainer.h"
#include "../Concepts/Printable.h"
#include "../Concepts/Reference.h"
#include "../Concepts/Same.h"
#include "../Inline.h"
#include "../Macro/Consteval.h"
template<class, usize...>struct Span;
template<$NotReference T>
struct Span<T>
{
	T* m_pBegin;
	T* m_pEnd;
	INLINE constexpr Span()noexcept :m_pBegin{ nullptr }, m_pEnd{ nullptr } {}
	template<class U>
	requires(std::is_const_v<T>&& $Same<std::remove_const_t<T>, U>)
	INLINE constexpr Span(const Span<U>& span)noexcept : m_pBegin{ span.begin() }, m_pEnd{ span.end() }{}
	template<usize N>
	INLINE constexpr Span(T(&arr)[N])noexcept : Span{ &arr[0], N } {}
	INLINE constexpr Span(T* pBegin, T* pEnd)noexcept :m_pBegin{ pBegin }, m_pEnd{ pEnd }{}
	INLINE constexpr Span(T* pBegin, $UInt auto size)noexcept : m_pBegin{ pBegin }, m_pEnd{ pBegin + size }{}
	INLINE constexpr Span($LinearContainerOf<T> auto&& container)noexcept /*requires(!std::is_rvalue_reference_v<decltype(container)>)*/ :Span{ container.data(),container.size() } {}
	INLINE constexpr T& operator[](usize index)const noexcept { return m_pBegin[index]; }
	INLINE constexpr usize size()const noexcept { return m_pEnd - m_pBegin; }
	INLINE constexpr isize ssize()const noexcept { return m_pEnd - m_pBegin; }
	INLINE constexpr T& front()const noexcept { return *m_pBegin; }
	INLINE constexpr T& back()const noexcept { return *(m_pEnd - 1); }
	INLINE constexpr T* begin()const noexcept { return m_pBegin; }
	INLINE constexpr T* end()const noexcept { return m_pEnd; }
	INLINE constexpr T* data()const noexcept { return m_pBegin; }
	INLINE constexpr bool empty()const noexcept { return m_pBegin == m_pEnd; }
	INLINE constexpr Span<T> subSpanFront(usize size)const noexcept { return {m_pBegin, m_pBegin + size}; }
	INLINE constexpr Span<T> subSpanBack(usize offset)const noexcept { return {m_pBegin + offset, m_pEnd}; }
	INLINE constexpr Span<T>& operator+=(usize offset)noexcept { m_pBegin += offset; return *this; }
	INLINE constexpr Span<T>& operator-=(usize offset)noexcept { m_pBegin -= offset; return *this; }
};
template<$NotReference T, usize Size>
struct Span<T, Size>
{
private:
	using Index = Compact<Size>;
	T* m_pBegin;
public:
	INLINE constexpr Span()noexcept :m_pBegin{ nullptr } {}
	template<class U>
	requires(std::is_const_v<T>&& $Same<std::remove_const_t<T>, U>)
	INLINE constexpr Span(const Span<U, Size>& span)noexcept : m_pBegin{ span.begin() } {}
	INLINE constexpr Span(T(&arr)[Size])noexcept : m_pBegin{ &arr[0] } {}
	INLINE constexpr Span(T* pBegin)noexcept :m_pBegin{ pBegin } {}
	INLINE constexpr Span($LinearContainerOf<T> auto&& container)noexcept /*requires(!std::is_rvalue_reference_v<decltype(container)>)*/ : m_pBegin{container.data()} {}
	INLINE constexpr T& operator[](usize index)const noexcept { return m_pBegin[index]; }
	static CONSTEVAL Index size() noexcept { return Size; }
	static CONSTEVAL isize ssize() noexcept { return Size; }//todo
	INLINE constexpr T& front()const noexcept { return *m_pBegin; }
	INLINE constexpr T& back()const noexcept { return m_pBegin[Size - 1]; }
	INLINE constexpr T* begin()const noexcept { return m_pBegin; }
	INLINE constexpr T* end()const noexcept { return m_pBegin + Size; }
	INLINE constexpr T* data()const noexcept { return m_pBegin; }
};
template<class T>Span(T*, $UInt auto)->Span<T>;
template<class T>Span(T*, T*)->Span<T>;
template<$LinearContainer T>requires(!$FixSized<T>)Span(T&&)->Span<std::remove_reference_t<decltype(*std::declval<T>().data())>>;
template<$LinearContainer T>requires( $FixSized<T>)Span(T&&)->Span<std::remove_reference_t<decltype(*std::declval<T>().data())>, static_size<T>>;

template<class L, auto...sizeL, class R, auto...sizeR>
constexpr bool operator==(Span<L, sizeL...> l, Span<R, sizeR...> r)noexcept
{
	if (l.size() != r.size())return false;
	auto pL{ l.begin() };
	auto pR{ r.begin() };
	auto pLEnd{ l.end()};
	for (;pL!=pLEnd;++pL, ++pR)
		if (*pL != *pR)
			return false;
	return true;
}

template<class Stream, $Printable<Stream> T, auto...Size>
constexpr Stream& operator<<(Stream& os, const Span<T, Size...>& span)noexcept
{
	os << '[';
	for (auto&& c : span)
	{
		if (&c != span.data())
			os << ',';
		os << c;
	}
	os << ']';
	return os;
}