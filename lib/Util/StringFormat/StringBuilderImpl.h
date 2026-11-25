#pragma once
#include "../Concepts/Integral.h"
#include "../Traits/Like.h"
template<class Char, class Buffer>
struct StringBuilderImpl
{
private:
	Buffer m_buffer;
public:

	using char_type = Char;

	constexpr StringBuilderImpl(auto&&...args)noexcept
	requires(requires{(..., (operator<<(*this, FWD(args)))); })
	{
		(...,(operator<<(*this, FWD(args))));
	}

	template<class Self>constexpr Like<Self, Buffer> str(this Self&& self) noexcept { return FWD(self).m_buffer; }
	constexpr void put(Char c)noexcept { m_buffer.push_back(c); }

	constexpr void write(const Char* s, $UInt auto count)noexcept
	requires(requires{m_buffer.append(s, count); })
	{
		m_buffer.append(s, count);
	}

	constexpr void write(const Char* s, $UInt auto count)noexcept
	requires(!requires{m_buffer.append(s, count); }&& requires{m_buffer.insert(m_buffer.end(), s, s + count); })
	{
		m_buffer.insert(m_buffer.end(), s, s + count);
	}
	
	constexpr void write(const Char* pStart, const Char* pEnd)noexcept
	requires(requires{m_buffer.append(pStart, usize{}); } || requires{m_buffer.insert(m_buffer.end(), pStart, usize{}); })
	{
		write(pStart, static_cast<usize>(pEnd - pStart));
	}

	constexpr void reserve(usize s)noexcept requires(requires{std::declval<Buffer&>().reserve(usize{}); }) { m_buffer.reserve(s); }
	constexpr void grow(usize s)noexcept requires(requires{std::declval<Buffer&>().reserve(usize{}); }) { reserve(m_buffer.capacity() + s); }
	constexpr void clear()noexcept { m_buffer.clear(); }
	constexpr bool empty()const noexcept { return m_buffer.empty(); }
};