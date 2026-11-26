# demos (videos have sound, click 🔊 to unmute)

https://github.com/user-attachments/assets/769fb24d-1eaa-4807-b4a0-50c1c5042a9b

https://github.com/user-attachments/assets/f1f29d64-feb8-467d-a72a-45c80188d0d2

https://github.com/user-attachments/assets/ff788098-3fea-46e1-94d1-1a791d8926c2

# what code does

1. listen to some audio endpoints (speaker/headphone/mic) with WASAPI

2. create a topmost click-through overlay window

3. draw some `frequency`->`amplitude` graphs based on what code hears in audio endpoints(x-axis = `frequency`, y-axis = `amplitude`)

    frequency is log-spaced, meaning distance between any piano key to its next is always same

    graph type includes:

    - `complex`: $\mathrm{Re}(f),\ \mathrm{Im}(f)$
    - `abs`: $A(f)=\sqrt{\mathrm{Re}(f)^2 + \mathrm{Im}(f)^2}$
    - `harmonic_product`: $\prod_{k=1}^{N} A(kf)$
    - `harmonic_product_normalized`: $\left(\prod_{k=1}^{N} A(kf)\right)^{\frac{1}{N}}$
    - `harmonic_product_skip_f0`: $\prod_{k=2}^{N+1} A(kf)$
    - `harmonic_product_skip_f0_normalized`: $\left(\prod_{k=2}^{N+1} A(kf)\right)^{\frac{1}{N}}$
    - `harmonic_product_odd`: $\prod_{k=1}^{N} A((2k-1)f)$
    - `harmonic_product_odd_normalized`: $\left(\prod_{k=1}^{N} A((2k-1)f)\right)^{\frac{1}{N}}$


4. for each graph, draw `note name` of top `n` local maximum, so you can see which notes are played (`n` is configurable in config file, default value is `4`)

5. monitor and hot-reload config file

# credit

1. https://github.com/fastfloat/fast_float

2. https://github.com/jk-jeon/dragonbox

3. https://github.com/ifduyue/musl/tree/master/src/math

4. https://github.com/shibatch/sleef/blob/7df8e36/src/libm/sleefsp.c

5. https://github.com/shibatch/sleef/blob/7df8e36/src/libm/sleefdp.c

6. https://github.com/Quuxplusone/Xoshiro256ss

7. https://github.com/Tencent/ncnn/blob/master/src/layer/x86/sse_mathfun.h

8. https://github.com/Tencent/ncnn/blob/master/src/layer/x86/avx_mathfun.h

9. https://github.com/Tencent/ncnn/blob/master/src/layer/x86/avx512_mathfun.h

10. https://github.com/RJVB/MacSTL/blob/master/test/sse_mathfun.h

11. https://github.com/facebook/folly/blob/main/folly/Traits.h

12. https://github.com/swansontec/map-macro

13. https://www.scs.stanford.edu/~dm/blog/va-opt.html

14. https://github.com/ww898/utf-cpp

15. https://github.com/lucasg/Dependencies/blob/master/ClrPhlib/src/managed/Phlib.cpp

16. https://github.com/JustasMasiulis/inline_syscall

17. https://github.com/skelsec/SMBtest/blob/master/NTStatus.py

18. https://github.com/wine-mirror/wine/blob/master/include/ntstatus.h

19. https://www.foonathan.net/2020/09/move-forward

20. https://stackoverflow.com/a/62984543

21. https://github.com/winsiderss/phnt

22. https://youtu.be/xjSkpY0XzAQ (demo song)

# how to compile

you only have to compile code with 1 compiler, not all 3. my favorite is clang-cl

#### msvc:

1. download and install latest [visual studio preview community](https://visualstudio.microsoft.com/insiders/), install desktop c++

2. run `build-msvc-release64.bat`

    `build-msvc-release64.bat` uses `/arch:AVX512`, program will crash if your cpu does not run `avx512` code, in which case you need to change `/arch:AVX512` to something else

#### clang-cl:

1. download and install latest [visual studio preview community](https://visualstudio.microsoft.com/insiders/), install desktop c++

2. install latest [llvm](https://github.com/llvm/llvm-project/releases/)

3. run `build-llvm-release64.bat`

    `build-llvm-release64.bat` uses `-march=native`, meaning compiled `.exe` will be optimized for whatever cpu its compiled on, and it might crash when running on different cpu

#### g++:

1. build g++ from https://github.com/gcc-mirror/gcc (task is left as a exercise for reader)

2. run `build-gcc-release64.bat`

    `build-gcc-release64.bat` uses `-march=native`, meaning compiled `.exe` will be optimized for whatever cpu its compiled on, and it might crash when running on different cpu

# how to use

1. install x64 cpu, preferably with avx512

2. install windows10/windows11

3. install at least 1 audio endpoint (such as headphone, speaker, mic)

4. compile code into `.exe`(see `how to compile` section)

5. set default audio output device frequency to maximum![setting](sound_setting.png)

6. run `.exe` (make sure program have read/write access to config file, default path is `config.sjn` in same folder as `.exe`. you can also pass custom path via command line, relative to `.exe` folder or absolute)

7. edit config file and see changes right away

# config file

todo