#pragma once
//https://stackoverflow.com/a/62984543
#define DEPAREN(...) ESC(ISH __VA_ARGS__)
#define ISH(...) ISH __VA_ARGS__
#define ESC(...) ESC_(__VA_ARGS__)
#define ESC_(...) VAN ## __VA_ARGS__
#define VANISH
#define DEPAREN2(...) DEPAREN(DEPAREN(__VA_ARGS__))