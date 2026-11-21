#pragma once
#include "Compiler.h"
#include "ConceptConstructible.h"
#include "ConceptSame.h"
#include "ConceptString.h"
#include "Inline.h"
#include "MacroMovForward.h"
#include "ObfuString.h"
#include "StaticAssert.h"
#include "Strcmp.h"
#include "StringSso.h"
#include "StringView.h"
#include "TextEncodingStringConvert.h"
#include "TString.h"
#include "Wtf16Iequal.h"
namespace Windows
{
	//absolute path
	struct DosPathView;
	struct DosPath;
	struct NtPathView;
	struct NtPath;
	template<class T>concept $Path = $AnyOf<std::remove_cvref_t<T>, DosPathView, DosPath, NtPathView, NtPath>;

	template<$Path T>
	using PathView = std::conditional_t<$AnyOf<std::remove_cvref_t<T>, NtPath, NtPathView>, NtPathView, DosPathView>;

	struct PathImpl
	{
		//include last '\'
		template<class Self>
		constexpr PathView<Self> parent_path(this Self&& self) noexcept
		{
			if (self.m_data.size() > 1)
				for (const c16* p{ self.m_data.back() == '\\' ? self.m_data.end() - 2 : self.m_data.end() - 1 }; p != self.m_data.begin(); --p)
					if (*p == '\\')
						return PathView<Self>{ self.m_data.data(), p + 1 };
			return PathView<Self>{};
		}

		constexpr StringView16 filename(this auto&& self) noexcept
		{
			if (self.m_data.size() > 1)
				for (auto idx{ self.m_data.size() }; idx--;)
					if (self.m_data[idx] == '\\')
						return { &self.m_data[idx + 1], self.m_data.end() };
			return self.m_data;
		}
		constexpr StringView16 filenameWithoutExtension(this auto&& self) noexcept
		{
			const StringView16 fullname{ self.filename() };
			const c16* pDot{ fullname.rfind('.') };
			if (!pDot)
				return fullname;
			else
				return { fullname.data(), pDot };
		}

		constexpr StringView16 extension(this auto&& self) noexcept
		{
			const StringView16 fullname{ self.filename() };
			const c16* pDot{ fullname.rfind('.') };
			if (!pDot)
				return {};
			else
				return { pDot, fullname.end() };
		}
		INLINE constexpr auto size(this auto&& self)noexcept{return self.m_data.size();}
		INLINE constexpr auto begin(this auto&& self)noexcept{return self.m_data.begin();}
		INLINE constexpr auto data(this auto&& self)noexcept{return self.m_data.data();}
		INLINE constexpr auto end(this auto&& self)noexcept{return self.m_data.end();}
		template<class Self>INLINE constexpr std::conditional_t<$AnyOf<std::remove_cvref_t<Self>, NtPath, DosPath>, Like<Self, c16>, c16> front(this Self&& self) noexcept { return self.m_data.front(); }
		template<class Self>INLINE constexpr std::conditional_t<$AnyOf<std::remove_cvref_t<Self>, NtPath, DosPath>, Like<Self, c16>, c16> back(this Self&& self) noexcept { return self.m_data.back(); }
		template<class Self>INLINE constexpr std::conditional_t<$AnyOf<std::remove_cvref_t<Self>, NtPath, DosPath>, Like<Self, c16>, c16> operator[](this Self&& self, decltype(self.size()) index) noexcept { return self.m_data[index]; }
	};

	struct DosPathView :PathImpl
	{
		StringView16 m_data;

		explicit constexpr DosPathView(auto&&...args)noexcept requires
			(
				requires{StringView16{ FWD(args)... }; } &&
				!(sizeof...(args) == 1 && (...&&($AnyOf<std::remove_cvref_t<decltype(args)>, NtPathView, NtPath>)))
			) : m_data{FWD(args)...} {}

		constexpr bool isAbsolute()const noexcept { return m_data.find(':') != nullptr; }
		constexpr bool isRelative()const noexcept { return m_data.find(':') == nullptr; }

		template<class Stream>
		constexpr friend Stream& operator<<(Stream& os, const DosPathView& filepath)noexcept { os << filepath.m_data; return os; }
	};

	struct DosPath :PathImpl
	{
		String16 m_data;

		constexpr DosPath()noexcept = default;

		explicit constexpr DosPath($Constructible<String16> auto&& str)noexcept requires(!$Path<decltype(str)>) : m_data{FWD(str)} {}

		constexpr DosPath(DosPath&& rhs)noexcept :m_data{ MOV(rhs.m_data) } {}
		constexpr DosPath(const DosPath& rhs)noexcept :m_data{ rhs.m_data } {}

		constexpr DosPath& operator=(DosPath&& rhs)noexcept { m_data = MOV(rhs.m_data); return *this; }
		constexpr DosPath& operator=(const DosPath& rhs)noexcept { m_data = rhs.m_data; return *this; }
		constexpr DosPath& operator=(const DosPathView& rhs)noexcept { m_data = rhs.m_data; return *this; }

		//explicit constexpr DosPath(StringView8 str)noexcept :m_data{ str.begin(), str.end() } {}
		explicit constexpr DosPath(DosPathView str)noexcept :m_data{ str.m_data } {}

		constexpr void clear()noexcept { m_data.clear(); }

		constexpr bool isAbsolute()const noexcept { return m_data.find(':') != nullptr; }
		constexpr bool isRelative()const noexcept { return m_data.find(':') == nullptr; }

		constexpr operator DosPathView()const noexcept { return DosPathView{ m_data }; }

		template<class Stream>
		constexpr friend Stream& operator<<(Stream& os, const DosPath& filepath)noexcept { os << filepath.m_data; return os; }
	};

	constexpr bool operator==(const DosPathView& l, const DosPathView& r)noexcept { return Wtf16::iequal(l.m_data, r.m_data); }
	constexpr bool operator==(const DosPath& l, const DosPath& r)noexcept { return Wtf16::iequal(l.m_data, r.m_data); }
	constexpr bool operator==(const DosPathView& l, const DosPath& r)noexcept { return Wtf16::iequal(l.m_data, r.m_data); }
	constexpr bool operator==(const DosPath& l, const DosPathView& r)noexcept { return Wtf16::iequal(l.m_data, r.m_data); }

	struct NtPathView :PathImpl
	{
		StringView16 m_data;

		explicit constexpr NtPathView(auto&&...args)noexcept requires
		(
			requires{StringView16{ FWD(args)... }; } &&
			!(sizeof...(args) == 1 && (...&&($AnyOf<std::remove_cvref_t<decltype(args)>, DosPathView, DosPath>)))
		) : m_data{FWD(args)...} {}

		//\??\c:\abc.txt
		//\Device\HarddiskVolume3\abc.txt
		//explicit constexpr operator DosPathView()const noexcept { return DosPathView{ m_data.substr(4) }; }

		template<class Stream>
		constexpr friend Stream& operator<<(Stream& os, const NtPathView& filepath)noexcept { os << filepath.m_data; return os; }
	};

	struct NtPath :PathImpl
	{
		String16 m_data;

		constexpr NtPath()noexcept = default;

		explicit constexpr NtPath($Constructible<String16> auto&& str)noexcept requires(!$Path<decltype(str)>) :m_data{ FWD(str) } {}
		
		constexpr NtPath(Uninitialized, usize size)noexcept :m_data{ uninitialized, size } {}

		constexpr NtPath(DosPathView dospath)noexcept :m_data{ uninitialized, 4 + dospath.m_data.size() }//TODO operator+
		{
			Algorithm::copy_assign(S16(LR"(\??\)"), m_data.data());
			Algorithm::copy_assign(dospath.m_data, m_data.data() + 4);
		}

		constexpr NtPath(NtPath&& rhs)noexcept :m_data{ MOV(rhs.m_data) } {}
		constexpr NtPath(const NtPath& rhs)noexcept :m_data{ rhs.m_data } {}

		constexpr NtPath& operator=(NtPath&& rhs)noexcept { m_data = MOV(rhs.m_data); return *this; }
		constexpr NtPath& operator=(const NtPath& rhs)noexcept { m_data = rhs.m_data; return *this; }

		constexpr NtPath& operator=(const NtPathView& rhs)noexcept { m_data = rhs.m_data; return *this; }

		explicit constexpr NtPath(NtPathView str)noexcept :m_data{ str.m_data } {}

		constexpr void clear()noexcept { m_data.clear(); }

		constexpr operator NtPathView()const noexcept { return NtPathView{ m_data }; }
		
		//\??\c:\abc.txt
		//\Device\HarddiskVolume3\abc.txt
		//explicit constexpr operator DosPathView()const noexcept { return DosPathView{ m_data.substr(4) }; }

		template<class Stream>
		constexpr friend Stream& operator<<(Stream& os, const NtPath& filepath)noexcept { os << filepath.m_data; return os; }
	};

	constexpr bool operator==(const NtPathView& l, const NtPathView& r)noexcept { return Wtf16::iequal(l.m_data, r.m_data); }
	constexpr bool operator==(const NtPath& l, const NtPath& r)noexcept { return Wtf16::iequal(l.m_data, r.m_data); }
	constexpr bool operator==(const NtPathView& l, const NtPath& r)noexcept { return Wtf16::iequal(l.m_data, r.m_data); }
	constexpr bool operator==(const NtPath& l, const NtPathView& r)noexcept { return Wtf16::iequal(l.m_data, r.m_data); }

	namespace details
	{
		template<$AnyOf<DosPath, NtPath> Path>
		INLINE constexpr Path pathAppend(auto&& filepath, $StringOfCharSize<2> auto&& rhs)noexcept
		requires($AnyOf<std::remove_cvref_t<decltype(filepath)>, Path, PathView<Path>>)
		{
			if (filepath.m_data.empty())[[unlikely]]
				return Path{ rhs };
			Path result;
			result.m_data.reserve(filepath.m_data.size() + rhs.size() + 1);
			result.m_data.append(filepath.m_data);
			if (filepath.m_data.back() != '\\')
				result.m_data.push_back('\\');
			const c16* pSegment{ rhs.data() };
			const c16* pEnd{ rhs.data() + rhs.size() };
			for (;;)
			{
				const c16* pSegmentEnd{ pSegment };
				while (pSegmentEnd != pEnd && *pSegmentEnd != '\\')
					++pSegmentEnd;
				const bool last{ pSegmentEnd == pEnd };
				const usize segmentLength{ static_cast<usize>(pSegmentEnd - pSegment) };
				/* */if (segmentLength == 1 && pSegment[0] == '.') {}
				else if (segmentLength == 2 && pSegment[0] == '.' && pSegment[1] == '.')
				{
					result.m_data.pop_back();
					while (result.m_data.size() && result.m_data.back() != '\\')
						result.m_data.pop_back();
				}
				else
					result.m_data.append(pSegment, last ? pEnd : (pSegmentEnd + 1));
				if (last)return result;
				pSegment = pSegmentEnd + 1;
			}
		}
		template<$AnyOf<DosPath, NtPath> Path>
		INLINE constexpr Path pathAppend(auto&& filepath, $StringOfCharSize<1> auto&& rhs)noexcept
		requires($AnyOf<std::remove_cvref_t<decltype(filepath)>, Path, PathView<Path>>)
		{
			return pathAppend<Path>(FWD(filepath), utf8ToUtf16(FWD(rhs)));
		}

		template<$AnyOf<DosPath, NtPath> Path, FixedString rhs>
		INLINE constexpr Path pathAppend(auto&& filepath)noexcept
		requires
		(
			$AnyOf<std::remove_cvref_t<decltype(filepath)>, Path, PathView<Path>> &&
			rhs.size() > 0 && 
			rhs[0] != '\\' &&
			!rhs.template starts_with<L"..\\">() &&
			!rhs.template starts_with<L".\\">() &&
			!rhs.template find<L"\\.\\">() &&
			!rhs.template find<L"\\..\\">()
		)
		{
			Path result;
			result.m_data.reserve(filepath.m_data.size() + compact<rhs.size() + 1>);
			result.m_data.append(filepath.m_data);
			if (!filepath.m_data.empty() && filepath.m_data.back() != '\\')
				result.m_data.push_back('\\');
			if consteval { result.m_data.append(rhs); }
			else/*    */ { result.m_data.append(ObfuString{ tstring<rhs> }); }
			return result;
		}
	}

	INLINE constexpr DosPath operator/(DosPathView filepath, $StringOfCharSizeAny<1,2> auto&& rhs)noexcept { return details::pathAppend<DosPath>(filepath, FWD(rhs)); }
	INLINE constexpr NtPath  operator/(NtPathView  filepath, $StringOfCharSizeAny<1,2> auto&& rhs)noexcept { return details::pathAppend<NtPath >(filepath, FWD(rhs)); }

	INLINE constexpr DosPath operator/(DosPathView filepath, $TString auto rhs)noexcept { return details::pathAppend<DosPath, rhs.chars>(filepath); }
	INLINE constexpr NtPath  operator/(NtPathView  filepath, $TString auto rhs)noexcept { return details::pathAppend<NtPath , rhs.chars>(filepath); }
}

STATIC_ASSERT(Windows::DosPathView{ LR"(c:\abc.txt)" } == 
			  Windows::DosPathView{ LR"(C:\AbC.TxT)" });

STATIC_ASSERT(Windows::NtPath{ StringView16{LR"(\??\c:\abc.txt)"} } ==
			  Windows::NtPathView{		    LR"(\??\C:\AbC.TxT)" });

STATIC_ASSERT(Windows::NtPathView{ LR"(\Device\HarddiskVolume3\foo)" } ==
			  Windows::NtPathView{ LR"(\device\HARDDISKVOLUME3\FOO)" });

STATIC_ASSERT(Windows::DosPathView{ L"" }.parent_path() == Windows::DosPathView{ L"" });
STATIC_ASSERT(Windows::DosPathView{ LR"(C:\)" }.parent_path() == Windows::DosPathView{ L"" });
STATIC_ASSERT(Windows::NtPathView{ LR"(\??\C:\)" }.parent_path() == Windows::NtPathView{ LR"(\??\)" });
STATIC_ASSERT(Windows::DosPathView{ LR"(C:\Dir\Sub\file.txt)" }.parent_path() == Windows::DosPathView{ LR"(C:\Dir\Sub\)" });
STATIC_ASSERT(Windows::NtPathView{ LR"(\Device\HarddiskVolume3\Dir\File.ext)" }.parent_path() == Windows::NtPathView{ LR"(\Device\HarddiskVolume3\Dir\)" });
STATIC_ASSERT(Windows::DosPathView{ LR"(C:\Root)" } / TS(L"Sub") / TS(L"file.txt") == Windows::DosPathView{ LR"(C:\Root\Sub\file.txt)" });
STATIC_ASSERT(Windows::DosPathView{ LR"(D:\noext)" }.extension().empty());
STATIC_ASSERT(Windows::DosPathView{ LR"(C:\Root)" } / StringView16{ LR"(.\Sub\.\file.txt)" } == Windows::DosPathView{ LR"(C:\Root\Sub\file.txt)" });
STATIC_ASSERT(Windows::DosPathView{ LR"(C:\Root\Sub)" } / StringView16{ LR"(..\.\x)" } == Windows::DosPathView{ LR"(C:\Root\x)" });
STATIC_ASSERT(Windows::DosPathView{ LR"(C:\Root\Sub)" } / StringView16{ LR"(.)" } == Windows::DosPathView{ LR"(C:\Root\Sub\)" });
STATIC_ASSERT(Windows::DosPathView{ LR"(C:\Root\Sub)" } / StringView16{ LR"(..)" } == Windows::DosPathView{ LR"(C:\Root\)" });
STATIC_ASSERT([] static
{
	const Windows::DosPathView p{ LR"(D:\path\thing.data)" };
	return p.filename()					== StringView16{ LR"(thing.data)" } &&
		   p.extension()				== StringView16{ LR"(.data)" } &&
		   p.filenameWithoutExtension()	== StringView16{ LR"(thing)" };
}());