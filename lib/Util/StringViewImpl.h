#pragma once
#include <utility>
#include "Concepts/Integral.h"
#include "Concepts/LinearContainer.h"
#include "Concepts/Same.h"
#include "Concepts/String.h"
#include "Inline.h"
#include "StringImpl.h"
#include "StringLexicographicalCompare.h"
template<$Char T>
struct StringView: StringImpl
{
private:
	T* m_pBegin;
	T* m_pEnd;
public:
	using char_type = T;

	INLINE constexpr T& operator[](usize index)const noexcept { return m_pBegin[index]; }
	INLINE constexpr usize size()const noexcept { return m_pEnd - m_pBegin; }
	INLINE constexpr isize ssize()const noexcept { return m_pEnd - m_pBegin; }
	INLINE constexpr T* begin()const noexcept { return m_pBegin; }
	INLINE constexpr T* end()const noexcept { return m_pEnd; }
	INLINE constexpr T* data()const noexcept { return m_pBegin; }
	INLINE constexpr T& front()const noexcept { return *m_pBegin; }
	INLINE constexpr T& back()const noexcept { return *(m_pEnd - 1); }
	INLINE constexpr bool empty()const noexcept { return m_pBegin == m_pEnd; }

	INLINE constexpr StringView<T> substr(usize pos, usize count)const noexcept { return { m_pBegin + pos, m_pBegin + pos + count }; }
	INLINE constexpr StringView<T> substr(usize pos)const noexcept { return { m_pBegin + pos, m_pEnd }; }

	INLINE constexpr StringView()noexcept :m_pBegin{ nullptr }, m_pEnd{ nullptr } {}

	template<class U>
	requires(std::is_const_v<T>&& $Same<std::remove_const_t<T>, U>)
	INLINE constexpr StringView(const StringView<U>& span)noexcept : m_pBegin{ span.begin() }, m_pEnd{ span.end() }{}
	
	//INLINE constexpr StringView& operator=(const StringView& span)noexcept { CTOR_CONST_AT(this, span); return *this; }

	INLINE constexpr StringView(T* pBegin, T* pEnd)noexcept :m_pBegin{ pBegin }, m_pEnd{ pEnd }{}

	INLINE constexpr StringView(T* pBegin, usize size)noexcept : m_pBegin{ pBegin }, m_pEnd{ pBegin + size }{}

	INLINE constexpr StringView($AnyOf<std::remove_cv_t<T>*, const std::remove_cv_t<T>*> auto pBegin) noexcept : m_pBegin{ pBegin }
	{
		while (*pBegin)
			++pBegin;
		CTOR_CONST_AT(&m_pEnd, pBegin);
	}

	template<usize N>
	INLINE constexpr StringView(T(&arr)[N])noexcept : StringView{ &arr[0], N - 1 } {}

	INLINE constexpr StringView($LinearContainerOf<T> auto&& container)noexcept /*requires(!std::is_rvalue_reference_v<decltype(container)>)*/ : StringView{container.data(),container.size()} {}
};
template<class T>StringView(T*, $UInt auto) -> StringView<T>;
template<class T>StringView(T*, T*) -> StringView<T>;
template<$String T>StringView(T) -> StringView<ElementOf<T>>;
using StringView8 = StringView<const c8>;
using StringView16 = StringView<const c16>;
using StringView32 = StringView<const c32>;
namespace detail
{
	template<class CharL, class CharR>
	requires(sizeof(CharL) == sizeof(CharR))
	INLINE constexpr bool string_view_equal(const StringView<CharL>& lhs, const StringView<CharR>& rhs)noexcept
	{
		if (lhs.size() != rhs.size())
			return false;
		const CharL* pL{ lhs.begin() };
		const CharR* pR{ rhs.begin() };
		for (; pL != lhs.end(); ++pL, ++pR)
			if (*pL != *pR)
				return false;
		return true;
	}

	template<class>constexpr bool IsStringViewImpl{};
	template<class T>constexpr bool IsStringViewImpl<StringView<T>>{true};
}
template<class T>
concept $StringView = detail::IsStringViewImpl<std::remove_cvref_t<T>>;

template<class CharL, class CharR>requires(sizeof(CharL) == sizeof(CharR))INLINE constexpr bool operator==(const StringView<CharL>& lhs, const StringView<CharR>& rhs)noexcept { return detail::string_view_equal(lhs, rhs); }
template<class CharL, class CharR>requires(sizeof(CharL) == sizeof(CharR))INLINE constexpr bool operator!=(const StringView<CharL>& lhs, const StringView<CharR>& rhs)noexcept { return !detail::string_view_equal(lhs, rhs); }

template<class Char>INLINE constexpr bool operator==(const StringView<Char>& lhs, auto&& rhs)noexcept requires(!$StringView<decltype(rhs)>&& requires{StringView<const Char>{rhs};}) { return  detail::string_view_equal(lhs, StringView<const Char>{rhs}); }
template<class Char>INLINE constexpr bool operator!=(const StringView<Char>& lhs, auto&& rhs)noexcept requires(!$StringView<decltype(rhs)>&& requires{StringView<const Char>{rhs};}) { return !detail::string_view_equal(lhs, StringView<const Char>{rhs}); }

template<class Char>INLINE constexpr bool operator==(auto&& lhs, const StringView<Char>& rhs)noexcept requires(!$StringView<decltype(lhs)>&& requires{StringView<const Char>{lhs};}) { return  detail::string_view_equal(StringView<const Char>{lhs}, rhs); }
template<class Char>INLINE constexpr bool operator!=(auto&& lhs, const StringView<Char>& rhs)noexcept requires(!$StringView<decltype(lhs)>&& requires{StringView<const Char>{lhs};}) { return !detail::string_view_equal(StringView<const Char>{lhs}, rhs); }
