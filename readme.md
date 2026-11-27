# demos (videos have sound, click 🔊 to unmute)

https://github.com/user-attachments/assets/769fb24d-1eaa-4807-b4a0-50c1c5042a9b

https://github.com/user-attachments/assets/f1f29d64-feb8-467d-a72a-45c80188d0d2

https://github.com/user-attachments/assets/ff788098-3fea-46e1-94d1-1a791d8926c2

# what code does

1. listen to some audio endpoints (speaker/headphone/mic) with WASAPI

2. create a topmost click-through overlay window

3. draw some `frequency`->`amplitude` graphs based on what code hears in audio endpoints(x-axis = `frequency`, y-axis = `amplitude`)

    frequency is log-spaced (`ConstantQ`), meaning distance between any piano key to its next is always same

    graph type includes:

	| graph | math |
	|---|---|
	| `complex` | $\left\langle\mathrm{Real}(f),\mathrm{Imag}(f)\right\rangle$ |
	| `abs` | $A(f)=\sqrt{\mathrm{Real}(f)^2 + \mathrm{Imag}(f)^2}$ |
	| `harmonic_product` | $\prod_{k=1}^{N} A(kf)$ |
	| `harmonic_product_normalized` | $\sqrt[N]{\prod_{k=1}^{N} A(kf)}$ |
	| `harmonic_product_skip_f0` | $\prod_{k=2}^{N+1} A(kf)$ |
	| `harmonic_product_skip_f0_normalized` | $\sqrt[N]{\prod_{k=2}^{N+1} A(kf)}$ |
	| `harmonic_product_odd` | $\prod_{k=1}^{N} A((2k-1)f)$ |
	| `harmonic_product_odd_normalized` | $\sqrt[N]{\prod_{k=1}^{N} A((2k-1)f)}$ |


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

(`CMakeLists.txt` is only for producing debug build when pressing `F5` in `vscode`. for optimized build, run 1 of below 3 `.bat` files)

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

5. set default audio output device frequency to maximum

![setting](sound_setting.png)

6. run `.exe` (make sure program have read/write access to config file, default path is `config.sjn` in same folder as `.exe`. you can also pass custom config path via command line, relative to `.exe` folder or absolute)

7. edit config file and see changes right away

# config file explained

```c++
{
	beep:false//when config is (re)loaded, make 1000hz beep noise if success, make 500hz beep noise if failure (like bad syntax, or no file read access)
	wait:0.1//overlay updates when audio end point produce new frame data, or when time since last update is greater than "wait" seconds
	keybinds
	{
		//press "pause_break" to terminate process
		exit["pause_break"]//a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,0,1,2,3,4,5,6,7,8,9,mouse_left,mouse_right,mouse_middle,xButton1,xButton2,numpad_0,numpad_1,numpad_2,numpad_3,numpad_4,numpad_5,numpad_6,numpad_7,numpad_8,numpad_9,numpad_mul,numpad_plus,numpad_minus,numpad_dec,numpad_div,numlock,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24,win_left,win_right,shift_left,shift_right,control_left,control_right,alt_left,alt_right,esc,back_tick,tab,cap,snapshot,scroll_lock,pause_break,insert,del,home,end,page_up,page_down,left,right,up,down,minus,equal,back_slash,back_space,square_bracket_left,square_bracket_right,enter,semicolon,quote,comma,period,slash,apps,space,shift,control,alt,sleep,select,print,execute,help,separator,numpad_equal,clear,numpad_enter,wheel_up,wheel_down
		
		//press ["cap","wheel_up"] together to increase graph amplitude
		boostDbInc["cap","wheel_up"]//a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,0,1,2,3,4,5,6,7,8,9,mouse_left,mouse_right,mouse_middle,xButton1,xButton2,numpad_0,numpad_1,numpad_2,numpad_3,numpad_4,numpad_5,numpad_6,numpad_7,numpad_8,numpad_9,numpad_mul,numpad_plus,numpad_minus,numpad_dec,numpad_div,numlock,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24,win_left,win_right,shift_left,shift_right,control_left,control_right,alt_left,alt_right,esc,back_tick,tab,cap,snapshot,scroll_lock,pause_break,insert,del,home,end,page_up,page_down,left,right,up,down,minus,equal,back_slash,back_space,square_bracket_left,square_bracket_right,enter,semicolon,quote,comma,period,slash,apps,space,shift,control,alt,sleep,select,print,execute,help,separator,numpad_equal,clear,numpad_enter,wheel_up,wheel_down
		
		//press ["cap","wheel_down"] together to decrease graph amplitude
		boostDbDec["cap","wheel_down"]//a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,0,1,2,3,4,5,6,7,8,9,mouse_left,mouse_right,mouse_middle,xButton1,xButton2,numpad_0,numpad_1,numpad_2,numpad_3,numpad_4,numpad_5,numpad_6,numpad_7,numpad_8,numpad_9,numpad_mul,numpad_plus,numpad_minus,numpad_dec,numpad_div,numlock,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24,win_left,win_right,shift_left,shift_right,control_left,control_right,alt_left,alt_right,esc,back_tick,tab,cap,snapshot,scroll_lock,pause_break,insert,del,home,end,page_up,page_down,left,right,up,down,minus,equal,back_slash,back_space,square_bracket_left,square_bracket_right,enter,semicolon,quote,comma,period,slash,apps,space,shift,control,alt,sleep,select,print,execute,help,separator,numpad_equal,clear,numpad_enter,wheel_up,wheel_down
		
		//press "cap" to display note name where cursor is pointing at
		showCursorNote["cap"]//a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,0,1,2,3,4,5,6,7,8,9,mouse_left,mouse_right,mouse_middle,xButton1,xButton2,numpad_0,numpad_1,numpad_2,numpad_3,numpad_4,numpad_5,numpad_6,numpad_7,numpad_8,numpad_9,numpad_mul,numpad_plus,numpad_minus,numpad_dec,numpad_div,numlock,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24,win_left,win_right,shift_left,shift_right,control_left,control_right,alt_left,alt_right,esc,back_tick,tab,cap,snapshot,scroll_lock,pause_break,insert,del,home,end,page_up,page_down,left,right,up,down,minus,equal,back_slash,back_space,square_bracket_left,square_bracket_right,enter,semicolon,quote,comma,period,slash,apps,space,shift,control,alt,sleep,select,print,execute,help,separator,numpad_equal,clear,numpad_enter,wheel_up,wheel_down
	}
	//you can have multiple sources
	//for example: 1 input(mic), 1 output(speaker)
	//and hum along to yt songs, see how close your notes match
	//thats my original goal of coding this project
	//turns out my overlay only visualizes how bad my singing is
	//it does not improve my singing at all
	sources
	[
		{
			//output = speaker, headphone; input = mic
			type:"output"//input,output

			//rect & triangle are the only 2 worth using
			//the rest makes peaks too wide
			windowFunction:"triangle"//rect,triangle,hann,kaiser,blackman_harris

			//you can enter frequency number (like 440), or note name (like "F#0", "Ab0")
			frequency
			{
				//dudes singing can go below C3
				//but i dont listen to dudes singing often, so "C3" it is
				min:"C3"//left  most frequency
				max:"C7"//right most frequency
			}
			nBinsPerOctave:128//how many frequencies to track per octave
			nCyclePerWindow:16//constant Q: each real/imag is computed with 16 * wavelen amount of frame data
			boostDb:12//keybinds.boostDbInc and keybinds.boostDbDec changes this number, you dont need to edit it by hand
		}
	]
	globalScale:1//scale graph height linearly by 1x

	graphs
	[
		{
			enable:true
			sourceId:0//sources[0] is used for this graph
			left :0//graph left  border position relative to monitor width
			right:1//graph right border position relative to monitor width
			base:0.5//graph height relative to screen height
			displayMode:"complex"//complex,abs,harmonic_product,harmonic_product_normalized,harmonic_product_skip_f0,harmonic_product_skip_f0_normalized,harmonic_product_odd,harmonic_product_odd_normalized
			nHarmonic:0//complex is not harmonic, so this value does not apply (must be set to 0)
			scale:10000//graph height linear scale
			autoScaleGraphId:null//no scaling against any other graph
			strokeWidth:0.003//stroke width of the graph lines
			color[0,1,0.85,0.8]//hsla = [hue, saturation, lightness, alpha]
			maximum//draw note name at graph local maximums
			{
				n:4//draw 4 local maximums
				font
				{
					size:0.05//relative to screen height
					familyName"consolas"
				}
			}
		}
		{
			enable:true
			sourceId:0
			left:0
			right:1
			base:0.05
			displayMode:"harmonic_product"//complex,abs,harmonic_product,harmonic_product_normalized,harmonic_product_skip_f0,harmonic_product_skip_f0_normalized,harmonic_product_odd,harmonic_product_odd_normalized
			nHarmonic:5//harmonic_product is harmonic, see "what code does" -> "graph type includes", N = nHarmonic, same goes for other harmonic display mode
			scale:-16000000000//when autoScaleGraphId is not null, scale is not used (but whether graph grows up/down depends on scale's signbit)
			autoScaleGraphId:0//max height of this graph is scaled to max height of graphs[0] (the first graph)
			strokeWidth:0.003
			color[0.47,1,0.85,0.8]
			maximum
			{
				n:4
				font
				{
					size:0.05
					familyName"consolas"
				}
			}
		}
		{
			enable:true
			sourceId:0
			left:0
			right:1
			base:0.95
			displayMode:"harmonic_product_skip_f0"//complex,abs,harmonic_product,harmonic_product_normalized,harmonic_product_skip_f0,harmonic_product_skip_f0_normalized,harmonic_product_odd,harmonic_product_odd_normalized
			nHarmonic:4
			scale:160000000
			autoScaleGraphId:0
			strokeWidth:0.003
			color[0.11,1,0.85,0.8]
			maximum
			{
				n:4
				font
				{
					size:0.05
					familyName"consolas"
				}
			}
		}
		{
			enable:true
			sourceId:0
			left:0
			right:1
			base:0.95
			displayMode:"harmonic_product_odd"//complex,abs,harmonic_product,harmonic_product_normalized,harmonic_product_skip_f0,harmonic_product_skip_f0_normalized,harmonic_product_odd,harmonic_product_odd_normalized
			nHarmonic:4
			scale:160000000
			autoScaleGraphId:0
			strokeWidth:0.003
			color[0.36,1,0.85,0.8]
			maximum
			{
				n:4
				font
				{
					size:0.05
					familyName"consolas"
				}
			}
		}
	]
	fps
	{
		enable:false//draw fps number
		position[0.045,0.04]//[horizontal, vertical] relative to screen width/height
		font
		{
			size:0.05
			familyName"consolas"
		}
	}
}
```