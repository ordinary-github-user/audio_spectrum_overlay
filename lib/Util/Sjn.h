#pragma once
#include "StringFormat/SjnParse.h"
#include "StringFormat/SjnPrint.h"
template<Sjn::ParsingOptions parsing, Sjn::PrintingOptions printing>
constexpr bool is_compatible{ parsing.Comment || !(printing.EnumFlagName || printing.EnumFlagFields) };