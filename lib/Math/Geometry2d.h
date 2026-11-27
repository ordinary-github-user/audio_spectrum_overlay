#pragma once
#include "../Util/Algorithm/MinMax.h"
#include "../Util/Algorithm/Reverse.h"
#include "../Util/Algorithm/Sort.h"
#include "../Util/Concepts/FixSized.h"
#include "../Util/DataStructures/FlatSet.h"
#include "../Util/DataStructures/GrowableArray.h"
#include "../Util/EnumDefinition.h"
#include "../Util/Macro/MovForward.h"
#include "Angle.h"
#include "MathConstant.h"
#include "Numeric/Abs.h"
#include "Numeric/Sqrt.h"
#include "Tensor/Tensor.h"
namespace Math
{
	template<$Floating T>
	struct Transform2x2
	{
	protected:
		Matrix2x2<T> m_matrix;
		constexpr Transform2x2(const Matrix2x2<T>& matrix)noexcept :m_matrix{ matrix } {}
	public:
		static constexpr Transform2x2 identity()noexcept
		{
			return Matrix2x2<T>{ {
				{1,0},
				{0,1}
			} };
		}

		static constexpr Transform2x2 rotation(const Rad<T>& angle)noexcept
		{
			const SinCos<T> sc{ sincos(angle) };
			return Matrix2x2<T>{ {
				{ sc.c,-sc.s },
				{ sc.s, sc.c }
			} };
		}

		static constexpr Transform2x2 stretching(const Vector2<T>& scale)noexcept
		{
			return Matrix2x2<T>{ {
				{scale[0],0},
				{0,scale[1]}
			} };
		}

		static constexpr Transform2x2 shearing(const Vector2<T>& scale)noexcept
		{
			return Matrix2x2<T>{ {
				{1,scale[1] },
				{scale[0],1 }
			} };
		}

		static constexpr Transform2x2 flipX()noexcept
		{
			return Matrix2x2<T>{ {
				{-1,0 },
				{ 0,1 }
			} };
		}

		static constexpr Transform2x2 flipY()noexcept
		{
			return Matrix2x2<T>{ {
				{ 1, 0 },
				{ 0,-1 }
			} };
		}

		constexpr Transform2x2 inverse()const noexcept { return { m_matrix.inverse()}; }

		constexpr Transform2x2 operator*(const Transform2x2& rhs)const noexcept { return m_matrix * rhs.m_matrix; }

		constexpr Vector2<T> operator*(const Vector2<T>& vec)const noexcept { return m_matrix * vec; }

		template<class Stream>
		friend constexpr Stream& operator<<(Stream& os, const Transform2x2& value)noexcept { return os << value.m_matrix; }
	};
	//template<$Floating T>
	//struct Transform2
	//{
	//protected:
	//	using base_t = Transform2<T>;
	//	using mat_t = Matrix3x3<T>;
	//	using Point = Vector2<T>;
	//	mat_t m_matrix;
	//public:
	//	constexpr Transform2()noexcept :m_matrix{ { { 1,0,0 },{ 0,1,0 },{ 0,0,1 } } } {}
	//	constexpr Transform2(const base_t&)noexcept = default;
	//	constexpr Transform2(base_t&&)noexcept = default;
	//	constexpr base_t& operator=(const base_t&)noexcept = default;
	//	constexpr base_t& operator=(base_t&&)noexcept = default;
	//	constexpr Transform2(const mat_t& value)noexcept :m_matrix{ value } {}
	//	constexpr const mat_t& matrix()const noexcept { return m_matrix; }
	//	static constexpr base_t identity()noexcept
	//	{
	//		return mat_t{ {
	//			{1,0,0},
	//			{0,1,0},
	//			{0,0,1}
	//		} };
	//	}
	//	static constexpr base_t translation(const Point& vec)noexcept
	//	{
	//		return mat_t{ {
	//			{ 1,0,0 },
	//			{ 0,1,0 },
	//			{vec[0],vec[1],1}
	//		} };
	//	}
	//	static constexpr base_t rotation(const Rad<T>& angle)noexcept
	//	{
	//		const SinCos<T> sc{ sincos(angle) };

	//		return mat_t{ {
	//			{ sc.c,sc.s,0 },
	//			{ -sc.s,sc.c,0},
	//			{ 0,0,1}
	//		} };
	//	}
	//	static constexpr base_t stretching(const Point& scale)noexcept
	//	{
	//		return mat_t{ {
	//			{scale[0],0,0},
	//			{0,scale[1],0},
	//			{0,0,1}
	//		} };
	//	}
	//	static constexpr base_t shearing(const Point& scale)noexcept
	//	{
	//		return mat_t{ {
	//			{1,scale[1],0 },
	//			{scale[0],1,0 },
	//			{ 0,0,1 }
	//		} };
	//	}
	//	static constexpr base_t flipX()noexcept
	//	{
	//		return mat_t{ {
	//			{-1,0,0 },
	//			{ 0,1,0 },
	//			{ 0,0,1 }
	//		} };
	//	}
	//	static constexpr base_t flipY()noexcept
	//	{
	//		return mat_t{ {
	//			{ 1,0,0 },
	//			{ 0,-1,0 },
	//			{ 0,0,1 }
	//		} };
	//	}
	//	constexpr Transform2<T> inverse()const noexcept { return { m_matrix.inverse()}; }
	//	constexpr base_t operator*(const base_t& rhs)const noexcept { return m_matrix * rhs.m_matrix; }
	//	constexpr Point operator*(const Point& vec)const noexcept//TODO: optimize
	//	{
	//		const Vector3<T> extendedVec{ vec[0],vec[1],1.f };
	//		const Vector3<T> result{ m_matrix * extendedVec };
	//		return { result[0],result[1] };
	//	}
	//	template<class Stream>
	//	friend constexpr Stream& operator<<(Stream& os, const base_t& value)noexcept { return os << value.m_matrix; }
	//};

	template<$Floating T>
	struct Transform2x3
	{
		Matrix2x3<T> matrix;
		static constexpr Transform2x3<T> from(const Rad<T>& angle, const Math::Vector2<T>& pos) noexcept
		{
			const SinCos<T> sc{ sincos(angle) };
			return {{{
				{ sc.c,-sc.s, pos[0] },
				{ sc.s, sc.c, pos[1] }
			}}};
		}
		constexpr Math::Vector2<T> operator*(const Math::Vector2<T>& rhs)const noexcept
		{
			return
			{
				matrix[0][0] * rhs[0] + matrix[0][1] * rhs[1] + matrix[0][2],
				matrix[1][0] * rhs[0] + matrix[1][1] * rhs[1] + matrix[1][2]
			};
		}
	};

	template<$Floating T>struct Segment2;
	template<$Floating T>struct Line2;

	template<$Floating T>
	struct Line2
	{
	protected:
		using base_t = Line2<T>;
		using Point = Vector2<T>;
	public:
		Point n;
		T d;

		constexpr Line2(const Point& p0, const Point& p1 = { 0,0 })noexcept :n{ (p1 - p0).normalClockWise().normalized() }, d{ dot(n,p0) } {}
		constexpr Line2(const Point& normal, T dist)noexcept : n{ normal }, d{ dist } {}

		constexpr base_t& makeDistPositive()noexcept { if (d < 0) { n = -n; d = -d; }return *this; }

		constexpr T distance(const Point& point)const noexcept { return (dot(point, n) - d); }
		constexpr Point projection(const Point& point)const noexcept { return point - distance(point) * n; }
	};

	template<$Floating T>
	struct Ray2
	{
		using type = T;
		using Point = Vector2<T>;
		Point origin;
		Point direction; //Must be unit length

		constexpr Ray2()noexcept = delete;
		constexpr Ray2(const Point& origin, const Point& direction)noexcept : origin{ origin }, direction{ direction } {}

		constexpr bool isFacing(const Point& target)const noexcept
		{
			return dot(direction, target - origin) > 0;
		}

		constexpr Point projection(const Point& point)const noexcept
		{
			return origin + direction * dot(direction, point - origin);
		}

		constexpr Point operator[](T t)const noexcept { return origin + direction * t; }
	};

	//https://stackoverflow.com/a/565282
	template<$Floating T>
	constexpr T intersect_impl(const Ray2<T>& l, const Ray2<T>& r)noexcept
	{
		return cross(r.origin - l.origin, r.direction) / cross(l.direction, r.direction);
	}
	template<$Floating T>
	constexpr bool intersect(const Ray2<T>& l, const Ray2<T>& r, Vector2<T>& result)noexcept
	{
		const T lT{ intersect_impl(l,r) };
		const T rT{ intersect_impl(l,r) };
		if (lT >= T{} && lT < std::numeric_limits<T>::infinity() &&
			rT >= T{} && rT < std::numeric_limits<T>::infinity()) [[likely]]
		{
			result = l[lT];
			return true;
		}
		else
			return false;
	}

	template<$Floating T>
	struct Segment2
	{
	protected:
		using Line = Line2<T>;
		using Point = Vector2<T>;
	public:
		Point p0, p1;

		constexpr Segment2()noexcept :p0{}, p1{} {}

		constexpr Segment2(const Point& from, const Point& to)noexcept :p0{ from }, p1{ to } {}

		constexpr Line line()const noexcept { return { p0, p1 }; }

		constexpr Point direction()const noexcept { return p1 - p0; }
		constexpr T length()const noexcept { return length(direction()); }

		constexpr bool isInRect(const Point& point)const noexcept
		{
			if (p0[0] < p1[0])
			{
				if (p0[1] < p1[1])
					return p0[0] <= point[0] && point[0] <= p1[0] && p0[1] <= point[1] && point[1] <= p1[1];
				else
					return p0[0] <= point[0] && point[0] <= p1[0] && p1[1] <= point[1] && point[1] <= p0[1];
			}
			else
			{
				if (p0[1] < p1[1])
					return p1[0] <= point[0] && point[0] <= p0[0] && p0[1] <= point[1] && point[1] <= p1[1];
				else
					return p1[0] <= point[0] && point[0] <= p0[0] && p1[1] <= point[1] && point[1] <= p0[1];
			}
		}

		constexpr T distance(const Point& target)const noexcept
		{
			Line l{ line() };

			if (isInRect(target))
				return l.distance(target);
			else
			{
				const T distSqr0{ p0.distanceSqr(target) };
				const T distSqr1{ p1.distanceSqr(target) };
				return distSqr0 < distSqr1 ? sqrt(distSqr0) : sqrt(distSqr1);
			}
		}

		constexpr explicit operator Ray2<T>()const noexcept
		{
			return {p0, normalized(p1 - p0)};
		}

		constexpr Point closestPoint(const Point& target)const noexcept
		{
			const Point proj{ line().projection(target) };
			return isInRect(proj) ? proj : ((p0.distanceSqr(target) < p1.distanceSqr(target)) ? p0 : p1);
		}
	};

	//template<$Floating T>
	//constexpr Segment2<T> operator*(const Transform2<T>& transform, const Segment2<T>& seg)noexcept
	//{
	//	return { transform * seg.p0,transform * seg.p1 };
	//}

	template<$Floating T>
	struct SegmentEx2
	{
	protected:
		using Line = Line2<T>;
		using Point = Vector2<T>;
	public:
		Point p0, p1;
		T len;
		Point direction;

		constexpr SegmentEx2()noexcept :p0{}, p1{}, len{}, direction{} {}

		constexpr SegmentEx2(const Point& from, const Point& to)noexcept :p0{ from }, p1{ to } 
		{
			direction = p1 - p0;
			len = length(direction);
			direction /= len;
		}

		constexpr Vector2<T> operator[](T t)const noexcept { return p0 + direction * t; }

		//constexpr Line line()const noexcept { return { p0, p1 }; }
		//constexpr Point direction()const noexcept { return p1 - p0; }
		//constexpr T length()const noexcept { return len; }
		//constexpr T distance(const Point& target)const noexcept
		//{
		//	Line l{ line() };
		//	if (isInRect(target))
		//		return l.distance(target);
		//	else
		//	{
		//		const T distSqr0{ p0.distanceSqr(target) };
		//		const T distSqr1{ p1.distanceSqr(target) };
		//		return distSqr0 < distSqr1 ? sqrt(distSqr0) : sqrt(distSqr1);
		//	}
		//}
		//constexpr explicit operator Ray2<T>()const noexcept
		//{
		//	return { p0, normalized(p1 - p0) };
		//}
		//constexpr Point closestPoint(const Point& target)const noexcept
		//{
		//	const Point proj{ line().projection(target) };
		//	return isInRect(proj) ? proj : ((p0.distanceSqr(target) < p1.distanceSqr(target)) ? p0 : p1);
		//}
	};

	template<$Floating T, $AnyOf<Vector2<T>, T>...Result>
	constexpr bool intersect(const Segment2<T>& l, const Segment2<T>& r, Result&...result)noexcept
	{
		const T lLength{ l.length() };
		const T rLength{ r.length() };
		const Ray2<T> rayL{l.p0, (l.p1 - l.p0) / lLength};
		const Ray2<T> rayR{r.p0, (r.p1 - r.p0) / rLength};
		const T lT{ intersect_impl(rayL, rayR) };
		if (!(lT > 0 && lT < lLength))return false;
		const T rT{ intersect_impl(rayR, rayL) };
		if (!(rT > 0 && rT < rLength))return false;
		([&]LAMBDA_INLINE
		{
			if constexpr ($Same<Result, T>)
				result = lT;
			else if constexpr ($Same<Result, Vector2<T>>)
				result = rayL[lT];
			else STATIC_ERROR();
		}(), ...);
		return true;
	}

	template<$Floating T, $AnyOf<Vector2<T>, T>...Result>
	constexpr bool intersect(const SegmentEx2<T>& l, const Segment2<T>& r, Result&...result)noexcept
	{
		const T lLength{ l.len };
		const T rLength{ r.length() };
		const Ray2<T> rayL{l.p0, l.direction};
		const Ray2<T> rayR{r.p0, (r.p1 - r.p0) / rLength};
		const T lT{ intersect_impl(rayL, rayR) };
		if (!(lT > 0 && lT < lLength))return false;
		const T rT{ intersect_impl(rayR, rayL) };
		if (!(rT > 0 && rT < rLength))return false;
		([&]LAMBDA_INLINE
		{
			if constexpr ($Same<Result, T>)
				result = lT;
			else if constexpr ($Same<Result, Vector2<T>>)
				result = rayL[lT];
			else STATIC_ERROR();
		}(), ...);
		return true;
	}

	template<$Floating T>
	constexpr bool intersect(const Ray2<T>& l, const Segment2<T>& r, $Same<Vector2<T>> auto&...result)noexcept
	requires(sizeof...(result) <= 1)
	{
		const T rLength{ r.length() };
		const Ray2<T> rayR{r.p0, (r.p1 - r.p0) / rLength};
		const T lT{ intersect_impl(l, rayR) };
		if (!(lT > T{} && lT < std::numeric_limits<T>::infinity()))return false;
		const T rT{ intersect_impl(rayR, l) };
		if (!(rT > T{} && rT < rLength))return false;
		((result = l[lT]),...);
		return true;
	}
	
	template<$Floating T>
	constexpr bool intersect(const Segment2<T>& l, const Ray2<T>& r, $Same<Vector2<T>> auto&...result)noexcept
	requires(sizeof...(result) <= 1)
	{
		return intersect(r, l, result...);
	}

	template<$Floating T>
	struct Polygon2
	{
		using Point = Vector2<T>;
		GrowableArray<Point> m_vertices;
		using Index = std::remove_cvref_t<decltype(std::declval<GrowableArray<Point>>().size())>;
		
		constexpr Polygon2(auto&&...args)noexcept :m_vertices{FWD(args)...} {}

		const Point& operator[](Index index)const noexcept { return m_vertices[index]; }Point& operator[](Index index)noexcept { return m_vertices[index]; }
		const Point* end()const noexcept { return m_vertices.end(); }Point* end() noexcept { return m_vertices.end(); }
		const Point* begin()const noexcept { return m_vertices.begin(); }Point* begin() noexcept { return m_vertices.begin(); }
		const Point* data()const noexcept { return m_vertices.data(); }Point* data()noexcept { return m_vertices.data(); }
		Index size()const noexcept { return m_vertices.size(); }
		void reverse() noexcept { Algorithm::reverse(m_vertices); }

		//return true to break
		template<$SigAnyOf<void(const Index, const Index), bool(const Index, const Index)> Action>
		INLINE void edgeIndexes(Action&& action)const noexcept
		{
#if 1
			Index i{}, j{ size() - 1 };
			for (; i < size(); j = i++)
			{
				if constexpr($Sig<Action, void(const Index, const Index)>)
					FWD(action)(j, i);
				else
				{
					if (FWD(action)(j, i))
						return;
				}
			}
#else
			const Index s{ m_vertices.size() };
			for (Index i{}; i != s; ++i)
				FWD(action)(i, (i + 1) % s);
#endif
		}

		INLINE void edges($Sig<void(const Point, const Point)> auto&& action)const noexcept
		{
			edgeIndexes([&](Index i, Index j)LAMBDA_INLINE{ return action(m_vertices[i],m_vertices[j]); });
		}

		//https://stackoverflow.com/a/1165943
		//positive = clockwise, negative = counter-clockwise
		T winding()const noexcept
		{
			T result{};
			edges([&](const Point& p0, const Point& p1)LAMBDA_INLINE{ result += (p1[0] - p0[0]) * (p1[1] + p0[1]); });
			return result;
		}

		void makeWindingNegative()noexcept { if (winding() > T{})reverse(); }
		void makeWindingPositive()noexcept { if (winding() < T{})reverse(); }

		T area()const noexcept
		{
			T result{};
			edges([&](const Point& p0, const Point& p1)LAMBDA_INLINE{ result += cross(p0, p1); });
			return result * T(0.5);
		}

		Point centroid()const noexcept
		{
#if 1
			Point centroid{};
			T area2{};
			edges([&](const Point& p0, const Point& p1)LAMBDA_INLINE
			{
				const T c{cross(p0, p1)};
				area2 += c;
				centroid += (p0 + p1) * c;
			});
			return centroid / (area2 * 3);
#else
			Point centroid{};
			edges([&](const Point& p0, const Point& p1)LAMBDA_INLINE{ centroid += (p0 + p1) * cross(p0, p1); });
			return centroid / (area() * 6);
#endif
		}

		//https://stackoverflow.com/a/2922778
		bool contains(const Point& point)const noexcept
		{
			bool result{};
			edges([&](const Point& p0, const Point& p1)LAMBDA_INLINE
			{
				if (((p1[1] > point[1]) != (p0[1] > point[1])) && (point[0] < (p0[0] - p1[0]) * (point[1] - p1[1]) / (p0[1] - p1[1]) + p1[0]))
					result = !result;
			});
			return result;
		}

		bool contains($LinearContainerOf<Point> auto&& points)const noexcept
		{
			for (const Point& point : points)
				if (!contains(point))
					return false;
			return true;
		}

		void removeAdjacentPoints(T maxLengthSqr = T(0.000001))noexcept
		{
			while(m_vertices.size() > 3)
			{
				bool found{};
				edgeIndexes([&](Index i0, Index i1)LAMBDA_INLINE
				{
					const Point& p0{m_vertices[i0]};
					const Point& p1{m_vertices[i1]};
					if (lengthSqr(p0 - p1) <= maxLengthSqr)
					{
						m_vertices.erase(i1);
						found = true;
						return true;
					}
					return false;
				});
				if (!found)break;
			}
		}
	};

	DEFINE_ENUM_PRINTABLE(PolygonMergeResult, u8,
		no_intersection,		//result not set
		yes_intersection,		//result     set
		l_in_r,					//result not set
		r_in_l,					//result not set
		error					//result not set
	);

	template<bool Safe = true, $Floating T>
	PolygonMergeResult merge(const Polygon2<T>& l, const Polygon2<T>& r, Polygon2<T>& result)noexcept
	{
		using Index = Polygon2<T>::Index;
		using Point = Vector2<T>;
		using Segment = Segment2<T>;
		using SegmentEx = SegmentEx2<T>;

		if constexpr (Safe)
		{
			if (l.winding() >= T{} || r.winding() >= T{})
				return PolygonMergeResult::error;
		}

		OrderedFlatSet<Index> lCandidateIndexes;
		OrderedFlatSet<Index> rCandidateIndexes;
		lCandidateIndexes.reserve(l.size());
		rCandidateIndexes.reserve(r.size());
		for (const Point& p : l)
			if (!r.contains(p))
				lCandidateIndexes.insert_unsorted(l.m_vertices.index_of(p));
		lCandidateIndexes.sort();

		for (const Point& p : r)
			if (!l.contains(p))
				rCandidateIndexes.insert_unsorted(r.m_vertices.index_of(p));
		rCandidateIndexes.sort();

		if (lCandidateIndexes.empty() && rCandidateIndexes.empty()) [[unlikely]]//todo
			return PolygonMergeResult::error;

		const bool lVertsAllInsideR{ lCandidateIndexes.empty() };
		const bool rVertsAllInsideL{ rCandidateIndexes.empty() };

		enum class Side { left, right };

		bool resultFound{};
		T resultArea;

		const usize maxResultSize{(l.size() + r.size()) * 2};
		while (!lCandidateIndexes.empty() || !rCandidateIndexes.empty())
		{
			Polygon2<T> currentResult;
			currentResult.m_vertices.reserve(maxResultSize);
			bool hasIntersection{};
			const Side startSide{ lCandidateIndexes.empty()?Side::right:Side::left };

			const Index startIndex{ startSide == Side::left ? lCandidateIndexes[0] : rCandidateIndexes[0] };
			Side active{ startSide };
			const Polygon2<T>* pActive;
			const Polygon2<T>* pInactive;
			OrderedFlatSet<Index>* pCandidates;

			if (active == Side::left) { pActive = &l; pInactive = &r; pCandidates = &lCandidateIndexes; }
			else /*                */ { pActive = &r; pInactive = &l; pCandidates = &rCandidateIndexes; }

			const auto swapActive{ [&]LAMBDA_INLINE
			{
				if (active == Side::left) { pActive = &r; pInactive = &l; pCandidates = &rCandidateIndexes; active = Side::right; }
				else /*                */ { pActive = &l; pInactive = &r; pCandidates = &lCandidateIndexes; active = Side::left; }
			} };

			pCandidates->erase(startIndex);
			Point currentPoint{ (*pActive)[startIndex] };
			Index nextIndex{ (startIndex + 1) % pActive->size() };

			Opt<usize> prevIntersectionIndex{};
			for (;;)
			{
				currentResult.m_vertices.emplace_back(currentPoint);

				if constexpr (Safe)
				{
					if (currentResult.size() >= maxResultSize)
						return PolygonMergeResult::error;
				}

				Index intersectionNextIndex;
				T closestT;
				bool edgeIntersects{};
				const SegmentEx seg{ currentPoint, (*pActive)[nextIndex] };
				pInactive->edgeIndexes([&](Index i0, Index i1)LAMBDA_INLINE
				{
					if(prevIntersectionIndex && *prevIntersectionIndex == i1)
						return;
					const Point& p0{(*pInactive)[i0]};
					const Point& p1{(*pInactive)[i1]};
					T t;
					if (intersect(seg, Segment{ p0, p1 }, t))
					{
						if (!edgeIntersects || t < closestT)
						{
							closestT = t;
							intersectionNextIndex = i1;
						}
						edgeIntersects = true;
						hasIntersection = true;
					}
				});
				if (edgeIntersects)
				{
					prevIntersectionIndex = nextIndex;
					const Point intersection{ seg[closestT] };
					currentPoint = intersection;
					nextIndex = intersectionNextIndex;
					swapActive();
				}
				else
				{
					pCandidates->erase(nextIndex);
					if (startSide == active && startIndex == nextIndex) [[unlikely]]
					{
						const T area{ currentResult.area() };
						if (!resultFound || area > resultArea)
						{
							if (!hasIntersection)
							{
								if (lVertsAllInsideR)
									return PolygonMergeResult::l_in_r;
								else if (rVertsAllInsideL)
									return PolygonMergeResult::r_in_l;
								else//not connected
									return PolygonMergeResult::no_intersection;
							}
							else
							{
								result = MOV(currentResult);
								resultFound = true;
								resultArea = area;
								break;
							}
						}
						break;
					}
					nextIndex = (nextIndex + 1) % pActive->size();
					currentPoint = seg.p1;
					prevIntersectionIndex = {};
				}
			}
		}
		return PolygonMergeResult::yes_intersection;
	}

	template<$Floating T>
	struct AABB2
	{
		using Point = Vector2<T>;
		Point min;
		Point max;

		constexpr Point size()const noexcept { return max - min; }
		constexpr Point halfSize()const noexcept { return size() * static_cast<T>(0.5); }
		constexpr Point center()const noexcept { return (min + max) * static_cast<T>(0.5); }
		
		template<u8 index>
		constexpr Point cornerPoint() const noexcept
		{
			if constexpr (0 == index) return { min[0], min[1] };
			if constexpr (1 == index) return { min[0], max[1] };
			if constexpr (2 == index) return { max[0], max[1] };
			if constexpr (3 == index) return { max[0], min[1] };
		}

		constexpr bool contains(const Point& point)const noexcept { return min[0] <= point[0] && point[0] <= max[0] && min[1] <= point[1] && point[1] <= max[1]; }
	};

	template<$Floating T>AABB2(Vector2<T>, Vector2<T>) -> AABB2<T>;

	template<$Floating T>constexpr AABB2<T>& operator+=(AABB2<T>& aabb, const Vector2<T>& movement)noexcept { aabb.min += movement; aabb.max += movement; return aabb; }

	template<$Floating T>constexpr AABB2<T> operator+(const AABB2<T>& aabb, const Vector2<T>& movement)noexcept { return { aabb.min + movement, aabb.max + movement }; }
	template<$Floating T>constexpr AABB2<T> operator+(const Vector2<T>& movement, const AABB2<T>& aabb)noexcept { return { aabb.min + movement, aabb.max + movement }; }

	//minkowski difference
	template<$Floating T>
	constexpr AABB2<T> operator-(const AABB2<T>& l, const AABB2<T>& r)noexcept
	{
		return {
			{ l.min - r.min - r.size() },
			{ l.min - r.min + l.size() },
		};
	}

	template<$Floating T>
	constexpr bool counter_clockwise_or_linear(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c)noexcept
	{
#if 1
		const Vector2<T> l{ b - a };
		const Vector2<T> r{ c - b };
		return l[0] * r[1] <= r[0] * l[1];
#else
		return cross(b - a, c - b) <= 0;
#endif
	}

	//points.size() might be less than 3
	constexpr void grahamScan($LinearContainer auto& points)noexcept
	requires requires{ points.erase(points.begin()); }
	{
		using Element = std::remove_cvref_t<ElementOf<decltype(points)>>;
		using Index = std::remove_cvref_t<decltype(points.size())>;
		if (points.size() < 3) [[unlikely]] return;
		Element* pFirst{ points.begin() };
		for (Element& p : Span{ points.data() + 1, points.end() })
			if (p[1] < (*pFirst)[1] || (p[1] == (*pFirst)[1] && p[0] > (*pFirst)[0]))
				pFirst = &p;
		std::swap(points[0], *pFirst);
		std::sort(points.begin() + 1, points.end(), [&](const Element& l, const Element& r)LAMBDA_INLINE
		{
			const auto lhs{ (l[1] - points.front()[1]) * (r[0] - points.front()[0]) };
			const auto rhs{ (r[1] - points.front()[1]) * (l[0] - points.front()[0]) };
			/* */if (lhs < rhs)return true;
			else if (lhs > rhs)return false;
			else [[unlikely]] return l[1] - points.front()[1] < r[1] - points.front()[1];
		});
		for (Index i{ 1 }; i != points.size();)
		{
			if (points[i] == points[i - 1]) [[unlikely]]
				points.erase(i);
			else
				++i;
		}
		for (Index i{ 2 }; i < points.size();)
		{
			if (counter_clockwise_or_linear(points[i - 2], points[i - 1], points[i]))
			{
				points.erase(i - 1);
				i = Algorithm::max(static_cast<Index>(i - 1), Index{ 2 });
			}
			else
				++i;
		}
	}
	//template<$Floating T>
	//struct Vertices2
	//{
	//	using base_t = Vertices2<T>;
	//	using vertex_t = Vector2<T>;
	//	using container_t = GrowableArray<vertex_t>;
	//	container_t m_vertices;
	//	void leftCut(T x, $Sig<void(const float2)> auto&& action)const noexcept
	//	{
	//		const usize s{ m_vertices.size() };
	//		for (usize i{}; i < s; ++i)
	//		{
	//			const float2& p0{ m_vertices[i] };
	//			const float2& p1{ m_vertices[(i + 1) % s] };
	//			const float2 dist{ p1 - p0 };
	//			const f32 slope{ dist[1] / dist[0] };
	//			const bool bothLeft{ p0[0] < x && p1[0] < x };
	//			const bool bothRight{ p0[0] > x && p1[0] > x };
	//			const bool passThrough{ (p0[0] - x) * (p1[0] - x) < 0 };
	//			if (bothLeft)
	//				action(p1);
	//			else if (bothRight)
	//			{
	//			}
	//			else if (passThrough)
	//			{
	//				const float2 intersectPoint{ x, p0[1] + slope * (x - p0[0]) };
	//				action(intersectPoint);
	//				if (p1[0] < x)
	//					action(p1);
	//			}
	//			else
	//				action(p1);
	//		}
	//	}
	//	void rightCut(T x, $Sig<void(const float2)> auto&& action)const noexcept
	//	{
	//		const usize s{ m_vertices.size() };
	//		for (usize i{}; i < s; ++i)
	//		{
	//			const float2& p0{ m_vertices[i] };
	//			const float2& p1{ m_vertices[(i + 1) % s] };
	//			const float2 dist{ p1 - p0 };
	//			const f32 slope{ dist[1] / dist[0] };
	//			const bool bothLeft{ p0[0] < x && p1[0] < x };
	//			const bool bothRight{ p0[0] > x && p1[0] > x };
	//			const bool passThrough{ (p0[0] - x) * (p1[0] - x) < 0 };
	//			if (bothRight)
	//				action(p1);
	//			else if (bothLeft)
	//			{
	//			}
	//			else if (passThrough)
	//			{
	//				const float2 intersectPoint{ x, p0[1] + slope * (x - p0[0]) };
	//				action(intersectPoint);
	//				if (p1[0] > x)
	//					action(p1);
	//			}
	//			else
	//				action(p1);
	//		}
	//	}
	//	Vertices2& setPosition(const vertex_t& position)noexcept
	//	{
	//		const vertex_t centroid{ getCentroid() };
	//		for (vertex_t& vertex : m_vertices)
	//			vertex = vertex - centroid + position;
	//		return *this;
	//	}
	//	T getArea()const noexcept
	//	{
	//		T result{ 0 };
	//		usize n{ m_vertices.size() };
	//		for (usize i{}; i < n; ++i)
	//		{
	//			const vertex_t& lastVertex{ m_vertices[i] };
	//			const vertex_t& nextVertex{ m_vertices[(i + 1) % n] };
	//			result += cross(lastVertex, nextVertex);
	//		}
	//		return (result * T(0.5));
	//	}
	//	vertex_t getCentroid()const noexcept
	//	{
	//		vertex_t centroid;
	//		usize n{ m_vertices.size() };
	//		for (usize i{}; i < n; ++i)
	//		{
	//			const vertex_t& lastVertex{ m_vertices[i] };
	//			const vertex_t& nextVertex{ m_vertices[(i + 1) % n] };
	//			centroid += (lastVertex + nextVertex) * cross(lastVertex, nextVertex);
	//		}
	//		return centroid / (getArea() * 6);
	//	}
	//	vertex_t& operator[](usize index)noexcept { return m_vertices[index]; }
	//	const vertex_t& operator[](usize index)const noexcept { return m_vertices[index]; }
	//	const vertex_t* end()const noexcept { return m_vertices.end(); }vertex_t* end() noexcept { return m_vertices.end(); }
	//	const vertex_t* begin()const noexcept { return m_vertices.begin(); }vertex_t* begin() noexcept { return m_vertices.begin(); }
	//	const vertex_t* data()const noexcept { return m_vertices.data(); }vertex_t* data()noexcept { return m_vertices.data(); }
	//	usize size()const noexcept { return m_vertices.size(); }
	//	void reverse() noexcept { Algorithm::reverse(m_vertices); }
	//	base_t& rotate(f32 radian, const vertex_t& origin = { 0,0 })noexcept
	//	{
	//		for (vertex_t& v : m_vertices)
	//			v = v.rotatedByRadian(radian, origin);
	//		return *this;
	//	}
	//	constexpr base_t& operator+=(const vertex_t& rhs)noexcept
	//	{
	//		for (vertex_t& v : m_vertices)
	//			v += rhs;
	//		return *this;
	//	}
	//	base_t operator+(const vertex_t& rhs)const noexcept { base_t copy{ *this }; return copy += rhs; }
	//};
}