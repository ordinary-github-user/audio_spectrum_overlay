#pragma once
#include "../Util/Inline.h"
#include "File/Filepath.h"
#include "TibTebPeb.h"
namespace Windows
{
	INLINE DosPathView exePath() noexcept{ return DosPathView{ thisPeb().pProcessParameters->ImagePathName }; }
	INLINE DosPathView exeDir() noexcept{ return exePath().parent_path(); }
	INLINE DosPathView currDir() noexcept{ return DosPathView{ thisPeb().pProcessParameters->CurrentDirectory.DosPath }; }
}