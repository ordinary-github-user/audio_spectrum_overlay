#pragma once
#include "SjnPrint.h"
#include "SjnParse.h"
template<Sjn::ParsingOptions parsing, Sjn::PrintingOptions printing>
constexpr bool is_compatible{ parsing.Comment || !(printing.EnumFlagName || printing.EnumFlagFields) };