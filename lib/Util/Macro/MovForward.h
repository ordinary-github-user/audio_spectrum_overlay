#pragma once
#include <type_traits>
#include "../Compiler.h"
#include "../Traits/Like.h"
#if BARRY && !CLANG_CL
	#include <meta>
	__macro FWD_IMPL(std::meta::token_sequence tokens) { return^^{ static_cast<decltype(\(tokens))&&>(\(tokens)) }; }
	__macro MOV_IMPL(std::meta::token_sequence tokens) { return^^{ static_cast<std::remove_reference<decltype(\(tokens))>::type&&>(\(tokens)) }; }
	__macro FWD_LIKE_IMPL(std::meta::token_sequence as, std::meta::token_sequence tokens) { return^^{ static_cast<Like<decltype(\(as)), decltype(\(tokens))>>(\(tokens)) }; }
	#define FWD(...) FWD_IMPL!(__VA_ARGS__)
	#define MOV(...) MOV_IMPL!(__VA_ARGS__)
	#define FWD_LIKE(...) FWD_LIKE_IMPL!(__VA_ARGS__)
#else
	//https://foonathan.net/2020/09/move-forward
	#define FWD(...) static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)
	#define MOV(...) static_cast<std::remove_reference<decltype(__VA_ARGS__)>::type&&>(__VA_ARGS__)
	#define FWD_LIKE(as, ...) static_cast<Like<decltype(as), decltype(__VA_ARGS__)>>(__VA_ARGS__)
#endif