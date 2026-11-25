#pragma once
#define EXPAND0(...) __VA_ARGS__
#define EXPAND1(...) EXPAND0(EXPAND0(EXPAND0(__VA_ARGS__)))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__)))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__)))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__)))
#define EXPAND(...)  EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__)))
//Evaluation Depth https://github.com/swansontec/map-macro
//Each level of the EVAL macro multiplies the effort of the previous level by 3, but also adds one evaluation of its own. Invoking the macro as a whole adds one more level, taking the total to:
//1 + (3 * (3 * (3 * (3 * (3 * (1) + 1) + 1) + 1) + 1) + 1) = 365
//Other interesting combinations include:
//calls/level levels  total depth
//2           3       16
//2           4       32
//2           5       64
//3           3       41
//3           4       122
//3           5       365
//4           3       86
//4           4       342
//4           5       1366
//5           2       32
//5           3       157
//5           4       782