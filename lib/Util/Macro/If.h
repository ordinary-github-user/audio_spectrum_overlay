#pragma once
#include "Concat.h"
#define IF0(a,b) b
#define IF1(a,b) a
#define IF(x,a,b) CAT(IF,x)(a,b)