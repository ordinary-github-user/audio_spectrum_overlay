#include "AudioSpectrum.h"
#include "lib/Windows/CommandLine.h"
i32 __stdcall wWinMain(HINSTANCE__*, HINSTANCE__*, c16*, i32)
{
	AudioSpectrum::loop([]static
	{
		Windows::DosPath path;
		if (!Sca::parse(path, commandline()))
			path = Windows::exeDir() / TS("config.sjn");
		return path;
	}());
	return 0;
}