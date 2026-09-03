md gcc-release64
@echo Started: %date% %time%
"C:\GCC\bin\g++.exe" -D _HAS_EXCEPTIONS=0 -D NDEBUG main.cpp -o "gcc-release64/main.o" -B c:\gcc\bin\ -fno-exceptions -fno-asynchronous-unwind-tables -Wall -fdiagnostics-show-caret -O3 -ftrivial-auto-var-init=uninitialized -fno-stack-protector -fno-rtti -std=c++26 -freflection -fconstexpr-ops-limit=4294967295 -ftemplate-depth=65535 -ftime-report -ffunction-sections -fdata-sections -c -m64 -march=native || goto :onExit
"C:\GCC\bin\g++.exe" "gcc-release64/main.o" -o "./gcc-release64-main.exe" -B c:\gcc\bin\ -mwindows -municode -Wl,--gc-sections -Wl,--disable-dynamicbase -Wl,--disable-high-entropy-va -Wl,--disable-reloc-section -Wl,--nxcompat -Wl,--no-insert-timestamp -static -lksuser || goto :onExit
for %%A in (".\gcc-release64-main.exe") do "c:\gcc\bin\strip.exe" --strip-all "%%~fA"
:onExit
rmdir /S /Q gcc-release64
@echo Completed: %date% %time%
pause
