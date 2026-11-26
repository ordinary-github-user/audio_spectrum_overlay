#pragma once
#include "../FunctionTraitsImpl.h"
template<class T>            concept $Function     = requires{ FunctionTraits<std::remove_cvref_t<T>>{}; };
template<class T, class   R> concept $Ret          = $Function<T> && $Same <typename FunctionTraits<std::remove_cvref_t<T>>::Ret , R>;
template<class T, class...R> concept $RetAnyOf     = $Function<T> && $AnyOf<typename FunctionTraits<std::remove_cvref_t<T>>::Ret , R...>;
template<class T, class   As>concept $ArgList      = $Function<T> && $Same <typename FunctionTraits<std::remove_cvref_t<T>>::Args, As>;
template<class T, class   As>concept $ArgListAnyOf = $Function<T> && $AnyOf<typename FunctionTraits<std::remove_cvref_t<T>>::Args, As>;
template<class T, class...As>concept $Args         = $Function<T> && $Same <typename FunctionTraits<std::remove_cvref_t<T>>::Args, TypeList<As...>>;