#pragma once
#include "Assert.h"
#include "Compiler.h"
#include "ConceptNotCvref.h"
#include "Inline.h"
#include "Like.h"
#include "MacroMovForward.h"
#include "Unreachable.h"
#include <memory>
#include <utility>

struct InPlace {};
constexpr InPlace inPlace[[maybe_unused]]{};
template<class T>
struct OptImpl
{
	INLINE constexpr explicit operator bool(this auto&& self) noexcept { return self.has_value(); }
	INLINE constexpr T value_or(this auto&& self, auto&& x) noexcept requires requires{static_cast<T>(FWD(x)); } { return self.has_value() ? FWD(self).m_value : static_cast<T>(FWD(x)); }

	template<class Self>INLINE constexpr Like<Self, T> value(this Self&& self) noexcept
	{
		ASSERT(self.has_value());
		return FWD(self).m_value;
	}
	template<class Self>INLINE constexpr Like<Self, T> operator*(this Self&& self) noexcept
	{
		ASSERT(self.has_value());
		return FWD(self).m_value;
	}
	template<class Self>INLINE constexpr LikePtr<Self, T> operator->(this Self&& self) noexcept
	{
		ASSERT(self.has_value());
		return __builtin_addressof(self.m_value);
	}
};

template<class T, auto...>
struct Opt;

template<$NotCvref T>
struct Opt<T>: OptImpl<T>
{
	union { T m_value; };
	bool m_hasValue;
	INLINE constexpr Opt()noexcept :m_hasValue{} 
#if 1
	{}
#else
	{
	#if GCC//gcc bug
		if consteval
		{
			if constexpr (requires{T{}; })CTOR_CONST_AT(__builtin_addressof(m_value));else UNREACHABLE;
		}
	#endif
	}
#endif

	INLINE constexpr Opt(InPlace, auto&&...args)noexcept requires(requires{T{ FWD(args)... }; }) : m_value{ FWD(args)... }, m_hasValue{ true } {}
	INLINE constexpr Opt(auto&&...args)noexcept requires(requires{T{ FWD(args)... }; }) : m_value{ FWD(args)... }, m_hasValue{ true } {}

	INLINE constexpr Opt& operator=(const Opt& rhs)noexcept
	{
		ASSERT(this != __builtin_addressof(rhs));//self assignment
		this->~Opt();
		CTOR_CONST_AT(this, rhs);
		return *this;
	}

	INLINE constexpr Opt& operator=(Opt&& rhs)noexcept
	{
		ASSERT(this != __builtin_addressof(rhs));//self assignment
		this->~Opt();
		CTOR_CONST_AT(this, MOV(rhs));
		return *this;
	}

	INLINE constexpr Opt& operator=(auto&& arg)noexcept
	requires(requires{T{ FWD(arg) }; })
	{
		if constexpr(__is_trivially_copyable(T) && sizeof(T) <= 64)//branchless if copyable && copy is cheap
		{
			CTOR_CONST_AT(__builtin_addressof(m_value), FWD(arg));
			m_hasValue = true;
		}
		else
		{
			if (m_hasValue)
			{
				if constexpr (requires{m_value = FWD(arg); })
					m_value = FWD(arg);
				else
				{
					if constexpr (!__is_trivially_destructible(T))
						m_value.~T();
					CTOR_CONST_AT(__builtin_addressof(m_value), FWD(arg));
				}
			}
			else
			{
				CTOR_CONST_AT(__builtin_addressof(m_value), FWD(arg));
				m_hasValue = true;
			}
		}
		return *this;
	}

	INLINE constexpr void unchecked_emplace(auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		if constexpr (!__is_trivially_destructible(T))
			ASSERT(!m_hasValue);
		CTOR_CONST_AT(__builtin_addressof(m_value), FWD(args)...);
		m_hasValue = true;
	}

	INLINE constexpr void emplace(auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		if constexpr (!__is_trivially_destructible(T))
			if (m_hasValue)
				m_value.~T();
		CTOR_CONST_AT(__builtin_addressof(m_value), FWD(args)...);
		m_hasValue = true;
	}

	INLINE constexpr void reset()noexcept 
	{
		if constexpr (!__is_trivially_destructible(T))
		{
			if (m_hasValue)
			{
				m_hasValue = false;
				m_value.~T();
			}
		}
		else
			m_hasValue = false;
	}

	INLINE constexpr ~Opt()noexcept 
	{
		if constexpr (!__is_trivially_destructible(T))
			if (m_hasValue)
				m_value.~T();
	}

	INLINE constexpr Opt(const Opt& rhs)noexcept :m_hasValue{ rhs.m_hasValue }
	{
		if (m_hasValue)
			CTOR_CONST_AT(__builtin_addressof(m_value), rhs.m_value);
#if GCC//gcc bug
		else if consteval
		{
			if constexpr (requires{T{}; })CTOR_CONST_AT(__builtin_addressof(m_value)); else UNREACHABLE;
		}
#endif
	}

	INLINE constexpr Opt(Opt&& rhs)noexcept :m_hasValue{ rhs.m_hasValue }
	{
		if (m_hasValue)
		{
			CTOR_CONST_AT(__builtin_addressof(m_value), MOV(rhs.m_value));
			rhs.m_hasValue = false;
		}
#if GCC//gcc bug
		else if consteval
		{
			if constexpr (requires{T{}; })CTOR_CONST_AT(__builtin_addressof(m_value)); else UNREACHABLE;
		}
#endif
	}

	INLINE constexpr bool has_value()const noexcept { return m_hasValue; }
};

template<$NotCvref T, T default_value>
requires(default_value == default_value)
struct Opt<T, default_value>: OptImpl<T>
{
	T m_value;

	INLINE constexpr Opt()noexcept :m_value{ default_value } {}

	INLINE constexpr Opt(InPlace, auto&&...args)noexcept requires(requires{T{ FWD(args)... }; }) : m_value{ FWD(args)... } {}
	INLINE constexpr Opt(auto&&...args)noexcept requires(requires{T{ FWD(args)... }; }) : m_value{ FWD(args)... } {}

	INLINE constexpr void unchecked_emplace(auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		ASSERT(m_value == default_value);
		if constexpr (sizeof...(args) == 1 && requires{(..., (m_value = FWD(args))); })
			(..., (m_value = FWD(args)));
		else
		{
			if constexpr (!__is_trivially_destructible(T))
				m_value.~T();
			CTOR_CONST_AT(__builtin_addressof(m_value), FWD(args)...);
		}
	}

	INLINE constexpr void reset()noexcept { m_value = default_value; }
	INLINE constexpr bool has_value()const noexcept { return m_value != default_value; }
};

template<$NotCvref T, /*$Sig<bool(const T)>*/ auto isValid>//cant concept because T might be incomplete
requires(requires(const T x) { { isValid(x) }->$Same<bool>; })
struct Opt<T, isValid>: OptImpl<T>
{
	T m_value;

	INLINE constexpr Opt()noexcept = delete;

	INLINE constexpr Opt(InPlace, auto&&...args)noexcept requires(requires{T{ FWD(args)... }; }) : m_value{ FWD(args)... } {}
	INLINE constexpr Opt(auto&&...args)noexcept requires(requires{T{ FWD(args)... }; }) : m_value{ FWD(args)... } {}

	INLINE constexpr void unchecked_emplace(auto&&...args)noexcept
	requires(requires{T{ FWD(args)... }; })
	{
		ASSERT(!isValid(m_value));
		if constexpr (sizeof...(args) == 1 && requires{(..., (m_value = FWD(args))); })
			(..., (m_value = FWD(args)));
		else
		{
			if constexpr (!__is_trivially_destructible(T))
				m_value.~T();
			CTOR_CONST_AT(__builtin_addressof(m_value), FWD(args)...);
		}
	}

	INLINE constexpr void reset()noexcept = delete;
	INLINE constexpr bool has_value()const noexcept { return isValid(m_value); }
};

template<class T, auto...args>INLINE constexpr bool operator==(const Opt<T, args...>& l, const Opt<T, args...>& r)noexcept { return l ? r && (*l == *r) : !r; }
template<class T, auto...args>INLINE constexpr bool operator!=(const Opt<T, args...>& l, const Opt<T, args...>& r)noexcept { return l ? !r || (*l != *r) : r; }
template<class T, auto...args>INLINE constexpr bool operator==(const Opt<T, args...>& l, const T& r)noexcept { return l && (*l == r); }
template<class T, auto...args>INLINE constexpr bool operator==(const T& l, const Opt<T, args...>& r)noexcept { return r && (l == *r); }
template<class T, auto...args>INLINE constexpr bool operator!=(const Opt<T, args...>& l, const T& r)noexcept { return !l || (*l != r); }
template<class T, auto...args>INLINE constexpr bool operator!=(const T& l, const Opt<T, args...>& r)noexcept { return !r || (l != *r); }
//template<class T, auto...args>INLINE constexpr bool operator<(const Opt<T, args...>& l, const Opt<T, args...>& r)noexcept { return l && r && *l < *r; }
//template<class T, auto...args>INLINE constexpr bool operator>(const Opt<T, args...>& l, const Opt<T, args...>& r)noexcept { return l && r && *l > *r; }
//template<class T, auto...args>INLINE constexpr bool operator<=(const Opt<T, args...>& l, const Opt<T, args...>& r)noexcept { return l && r && *l <= *r; }
//template<class T, auto...args>INLINE constexpr bool operator>=(const Opt<T, args...>& l, const Opt<T, args...>& r)noexcept { return l && r && *l >= *r; }