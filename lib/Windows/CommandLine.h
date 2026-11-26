#pragma once
#include "../Util/StringFormat/ScaParse.h"
#include "TibTebPeb.h"
INLINE UnicodeString const& commandline()noexcept { return Windows::thisPeb().pProcessParameters->CommandLine; }