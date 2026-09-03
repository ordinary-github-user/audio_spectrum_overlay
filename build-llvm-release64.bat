call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat" amd64 >nul 2>&1
@echo on
md llvm-release64
@echo Started: %date% %time%
"C:\Program Files\LLVM\bin\clang-cl.exe" /D _HAS_EXCEPTIONS=0 /D NDEBUG main.cpp /Fo"llvm-release64\\" /EHs- /EHa- /W4 /WX- /diagnostics:caret /O2 /Ob2 /Oi /Ot -flto=full -ftrivial-auto-var-init=uninitialized /GS- /fp:precise /GR- /std:c++latest /permissive- /Gd /TP /clang:-fconstexpr-steps=4294967295 -fbracket-depth=4294967295 -ftime-trace /c -march=native -m64 || goto :onExit
"C:\Program Files\LLVM\bin\lld-link.exe" llvm-release64\main.obj /OUT:"llvm-release64-main.exe" /SUBSYSTEM:WINDOWS /NOLOGO /MANIFEST:NO /OPT:REF /OPT:ICF /DYNAMICBASE:NO /HIGHENTROPYVA:NO /NXCOMPAT /MERGE:".pdata=.text" /MERGE:"_RDATA=.text" /MERGE:".rdata=.text" /MERGE:".00cfg=.text" /MERGE:".gehcont=.text" /FIXED || goto :onExit
:onExit
rmdir /S /Q llvm-release64
@echo Completed: %date% %time%
pause
