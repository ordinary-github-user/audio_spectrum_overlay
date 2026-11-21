#pragma once
#include "MappedPeView.h"
#include "TibTebPeb.h"
#include "TString.h"

#define IMPORT_FUNCTION_WITH_TYPE_EX(var,name,type)auto var { []static{auto result { pe.getFunctionTrueAddressByName<type>(TS(#name))};if(!result)CRASH();return result;}() };
#define IMPORT_FUNCTION_WITH_TYPE(var,type)IMPORT_FUNCTION_WITH_TYPE_EX(var,var,type)
#define IMPORT_FUNCTION_EX(var,name) IMPORT_FUNCTION_WITH_TYPE_EX(var,name,decltype(&::name))
#define IMPORT_FUNCTION(name) IMPORT_FUNCTION_EX(name,name)