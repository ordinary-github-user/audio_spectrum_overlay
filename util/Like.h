#pragma once
#include <utility>
template<class...>struct LikeImpl;
template<class From, class To>struct LikeImpl<From&, To> { using type = To&; };
template<class From, class To>struct LikeImpl<const From&, To> { using type = const To&; };
template<class From, class To>struct LikeImpl<volatile From&, To> { using type = volatile To&; };
template<class From, class To>struct LikeImpl<const volatile From&, To> { using type = const volatile To&; };
template<class From, class To>struct LikeImpl<From&&, To> { using type = To&&; };
template<class From, class To>struct LikeImpl<const From&&, To> { using type = const To&&; };
template<class From, class To>struct LikeImpl<volatile From&&, To> { using type = volatile To&&; };
template<class From, class To>struct LikeImpl<const volatile From&&, To> { using type = const volatile To&&; };
template<class From, class To>struct LikeImpl<From, To> { using type = To&&; };
template<class From, class To>struct LikeImpl<const From, To> { using type = const To&&; };
template<class From, class To>struct LikeImpl<volatile From, To> { using type = volatile To&&; };
template<class From, class To>struct LikeImpl<const volatile From, To> { using type = const volatile To&&; };

template<class From, class To>using Like = typename LikeImpl<From, std::remove_cvref_t<To>>::type;
template<class From, class To>using LikePtr = std::remove_reference_t<Like<From, To>>*;