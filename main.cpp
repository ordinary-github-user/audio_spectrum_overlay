#include "AudioSpectrum.h"
#include "lib/Windows/CommandLine.h"
#include "lib/Windows/ExePath.h"
i32 __stdcall wWinMain(HINSTANCE__*, HINSTANCE__*, c16*, i32)
{
	AudioSpectrum::loop([] static -> Windows::DosPath
	{
		const StringView16 path{ CommandLine::excludeExePath() };
		/* */if (path.empty())return Windows::DosPath{ Windows::exeDir() / TS("config.sjn") };
		else if (Windows::DosPathView{ path }.isAbsolute())return Windows::DosPath{ path };
		else return Windows::exeDir() / path;
	}());
	return 0;
}