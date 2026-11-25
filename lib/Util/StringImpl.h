#pragma once
#include "BytePatternBoyerMooreSearch.h"
#include "Constant.h"
#include "DataStructures/FixedStringImpl.h"
#include "ElementOf.h"
#include "IndexTypeOf.h"
#include "Inline.h"
#include "Strcmp.h"
struct StringImpl
{
	template<class Self>INLINE constexpr IndexOf<Self> index_of(this Self&& self, const ElementOf<Self>& e)noexcept { return static_cast<IndexOf<Self>>(__builtin_addressof(e)-self.data()); }
	template<class Self>INLINE constexpr IndexOf<Self> index_of(this Self&& self, const ElementOf<Self>* p)noexcept { return static_cast<IndexOf<Self>>(p - self.data()); }

	template<FixedString target, Strcmp::Encoding encoding = Strcmp::Encoding::ascii, bool case_sensitive = true>
	constexpr auto starts_with(this auto&& self)noexcept
	requires(sizeof(ElementOf<decltype(self)>) == sizeof(ElementOf<decltype(target)>))
	{
		static constexpr auto s{ target.size() };
		return self.size() >= s && Strcmp::Equal<target, encoding, case_sensitive>::match(self.data());
	}

	template<FixedString target, Strcmp::Encoding encoding = Strcmp::Encoding::ascii, bool case_sensitive = true>
	constexpr auto ends_with(this auto&& self)noexcept
	requires(sizeof(ElementOf<decltype(self)>) == sizeof(ElementOf<decltype(target)>))
	{
		static constexpr auto s{ target.size() };
		return self.size() >= s && Strcmp::Equal<target, encoding, case_sensitive>::match(self.data() + self.size() - s);
	}
	
	//todo: optimize
	template<class Self>
	constexpr auto find(this Self&& self, ElementOf<Self> c)noexcept
	{
		using Iter = std::remove_cvref_t<decltype(self.begin())>;
		for (Iter pChar{ self.begin() }, pEnd{ self.end() }; pChar != pEnd; ++pChar)
			if (*pChar == c)
				return pChar;
		return Iter{};
	}
	//todo: optimize
	template<class Self>
	constexpr auto rfind(this Self&& self, ElementOf<Self> c)noexcept
	{
		using Iter = std::remove_cvref_t<decltype(self.begin())>;
		for (Iter pBegin{ self.begin() }, pChar{ self.end() };;)
		{
			if (pChar == pBegin)return Iter{};
			--pChar;
			if (*pChar == c)return pChar;
		}
	}
	//todo: optimize
	template<FixedString target, Strcmp::Encoding encoding = Strcmp::Encoding::ascii, bool case_sensitive = true>
	constexpr auto find(this auto&& self) noexcept
	requires(!target.empty() && sizeof(ElementOf<decltype(self)>) == sizeof(ElementOf<decltype(target)>))
	{
		using Iter = std::remove_cvref_t<decltype(self.begin())>;
		static constexpr auto s{ target.size() };
		if (self.size() < s)return Iter{};
		for (Iter ptr{ self.begin() }, pEnd{ self.end() - s }; ptr <= pEnd; ++ptr)
			if(Strcmp::Equal<target, encoding, case_sensitive>::match(ptr))
				return ptr;
		return Iter{};
	}
	//todo: optimize
	template<FixedString target, Strcmp::Encoding encoding = Strcmp::Encoding::ascii, bool case_sensitive = true>
	constexpr auto rfind(this auto&& self) noexcept
	requires(!target.empty() && sizeof(ElementOf<decltype(self)>) == sizeof(ElementOf<decltype(target)>))
	{
		using Iter = std::remove_cvref_t<decltype(self.begin())>;
		static constexpr auto s{ target.size() };
		if (self.size() < s)return Iter{};
		for (Iter ptr{ self.end() - s };; --ptr)
		{
			if (Strcmp::Equal<target, encoding, case_sensitive>::match(ptr))
				return ptr;
			if (ptr == self.begin())
				return Iter{};
		}
	}

	template<class Self>
	INLINE constexpr void erase(this Self&& self, ElementOf<Self>* pBegin, const ElementOf<Self>* pEnd)noexcept
	requires(requires{self.m_size -= static_cast<IndexOf<Self>>(pEnd - pBegin); })
	{
		auto pE{ self.end() };
		self.m_size -= static_cast<IndexOf<Self>>(pEnd - pBegin);
		for (; pEnd != pE; ++pBegin, ++pEnd)
			*pBegin = *pEnd;
	}

	template<class Self>
	INLINE constexpr void erase(this Self&& self, IndexOf<Self> index, IndexOf<Self> count = 1)noexcept
	requires(requires{self.m_size -= count; })
	{
		auto pBegin{ self.begin() + index };
		self.erase(pBegin, pBegin + count);
	}

	template<class Self>
	INLINE constexpr void erase(this Self&& self, ElementOf<Self>* pBegin, IndexOf<Self> count = 1)noexcept
	requires(requires{self.m_size -= count; })
	{
		self.erase(pBegin, pBegin + count);
	}

	template<FixedString target, class Self>
	INLINE constexpr void replace(this Self&& self, $LinearContainerOf<ElementOf<Self>> auto const& with)noexcept
	requires
	(
		!target.empty() && 
		requires{self.m_size -= IndexOf<Self>{}; }&&
		requires{self.insert(IndexOf<Self>{}, with.data() + target.size(), with.size() - target.size()); }
	)
	{
		IndexOf<Self> iNext{};
		for (ElementOf<Self>* pNext; (pNext = boyer_moore_string_scanner<target>.search(self.data() + iNext, self.end()));)
		{
			if (target.size() >= with.size())
			{
				for (const auto i : iota_up(static_cast<IndexOf<decltype(target)>>(with.size())))
					pNext[i] = with[i];
				pNext += with.size();
				self.erase(pNext, target.size() - with.size());
				iNext = self.index_of(pNext);
			}
			else
			{
				for (const auto i : iota<target.size()>)
					pNext[i] = with[i];
				pNext += target.size();
				iNext = self.index_of(pNext);
				self.insert(iNext, with.data() + target.size(), with.end());//pNext invalidated
				iNext += (with.size() - target.size());
			}
		}
	}
};