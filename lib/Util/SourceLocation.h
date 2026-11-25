#pragma once
#include "Macro/Stringfy.h"
#define LINE_STRING STR(__LINE__)
#define FILE_STRING STR(__FILE__)
#define SOURCE_LOCATION FILE_STRING ## LINE_STRING