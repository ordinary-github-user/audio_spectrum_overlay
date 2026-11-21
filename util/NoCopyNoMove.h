#pragma once
struct NoCopy
{
	NoCopy(const NoCopy&) = delete;
	NoCopy& operator=(const NoCopy&) = delete;
protected:
	constexpr NoCopy()noexcept {}
	constexpr NoCopy(NoCopy&&)noexcept {}
	constexpr NoCopy& operator=(NoCopy&&)noexcept { return *this; }
};

struct NoMove
{
	NoMove(NoMove&&) = delete;
	NoMove& operator=(NoMove&&) = delete;
protected:
	constexpr NoMove()noexcept {}
	constexpr NoMove(const NoMove&)noexcept {}
	constexpr NoMove& operator=(const NoMove&)noexcept { return *this; }
};

struct NoCopyMove
{
	NoCopyMove(NoCopyMove&&) = delete;
	NoCopyMove& operator=(NoCopyMove&&) = delete;
	NoCopyMove(const NoCopyMove&) = delete;
	NoCopyMove& operator=(const NoCopyMove&) = delete;
protected:
	constexpr NoCopyMove()noexcept {}
};