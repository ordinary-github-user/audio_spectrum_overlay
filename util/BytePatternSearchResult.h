#pragma once
#include "ConceptSig.h"
namespace BytePattern
{
	template<class Uint8, $SigAnyOf<void(Uint8* const), bool(Uint8* const)>...Action>
	struct ResultImpl;

	template<class Uint8>
	struct ResultImpl<Uint8>
	{
		using type = Uint8*;
	};

	template<class Uint8, $Sig<void(Uint8* const)> Action>
	struct ResultImpl<Uint8, Action>
	{
		using type = void;
	};

	template<class Uint8, $Sig<bool(Uint8* const)> Action>
	struct ResultImpl<Uint8, Action>
	{
		using type = bool;
	};

	template<class Uint8, $SigAnyOf<void(Uint8* const), bool(Uint8* const)>...Action>
	using Result = ResultImpl<Uint8, Action...>::type;
}