#pragma once
#include "BasicTypes.h"
#include "ConceptLinearContainer.h"
template<auto...>struct ValueList {};
template<auto, class>struct MakeValueListImpl;
template<$LinearContainer auto List, usize...I>
struct MakeValueListImpl<List, std::index_sequence<I...>>
{
	using type = ValueList<List[I]...>;
};
template<$LinearContainer auto List>
using MakeValueList = typename MakeValueListImpl<List, std::make_index_sequence<List.size()>>::type;