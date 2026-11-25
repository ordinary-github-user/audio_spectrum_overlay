call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat" amd64 >nul 2>&1
@echo on
md msvc-Optimized64
@echo Started: %date% %time%
"cl.exe" /D _HAS_EXCEPTIONS=0 /D NDEBUG /D USE_LOCKS=0 main.cpp /Fo"msvc-Optimized64\\" /EHs- /EHa- /W4 /WX- /diagnostics:caret /O2 /Ob2 /Oi /Ot /GL /Gy /Gw /GS- /fp:precise /GR- /std:c++latest /permissive- /Zc:__cplusplus /Zc:preprocessor /utf-8 /Gd /TP /constexpr:steps2147483647 /MT /Bt+ /c /arch:AVX512 || goto :onExit
"link.exe" msvc-Optimized64\main.obj /OUT:"msvc-Optimized64-main.exe" /SUBSYSTEM:WINDOWS /NOLOGO /LTCG /MANIFEST:NO /OPT:REF /OPT:ICF /DYNAMICBASE:NO /HIGHENTROPYVA:NO /NXCOMPAT /MERGE:_RDATA=.text /MERGE:.rdata=.text /MERGE:.00cfg=.text /MERGE:.gehcont=.text /IGNORE:4254 kernel32.lib /FIXED || goto :onExit
@echo Completed: %date% %time%
rmdir /S /Q msvc-Optimized64
:onExit
