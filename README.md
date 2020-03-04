# Godot FastNoiseSIMD
This repository provides a module for [Godot Engine](https://github.com/godotengine/godot/) 3.2 that wraps the [FastNoiseSIMD](https://github.com/Auburns/FastNoiseSIMD) library.

It automatically detects the highest SIMD level available at runtime. It does not require SIMD to build or run, so you can use it in your games that will run on systems that don't support SIMD.

Many noise libraries provide singular lookups (e.g. get_nosie_3d(x, y, z). I have provided these singular functions, but they are slow. The speed gains come from requesting a large block of noise all at once (e.g. 16x16x16 or more).

## Features

These noise algorithms are provided:
* Value Noise 3D
* Perlin Noise 3D
* Simplex Noise 3D (Uncomment `//#define ENABLE_SIMPLEX` in `thirdparty/FastNoiseSIMD.h` to enable.)
* Cubic Noise 3D
* White Noise 3D
* Cellular Noise 3D

Plus these features:
* Multiple fractal options for all of the above
* Perturb input coordinates in 3D space
* Integrated up-sampling
* Easy to use 3D cave noise (Cellular: Div2Cave)

## Supported Compilers & Systems

This module works with Godot 3.2 with [PR #35144] (https://github.com/godotengine/godot/pull/35144) which has not yet been merged. It will eventually be updated to build for 4.0.

While FastNoiseSIMD does not require a processor that supports SIMD to run or build, it does require a modern compiler that supports SIMD instructions.

FastNosieSIMD has been tested on these compilers:
* MSVC v120/v140, 2013-2019
* Intel 16.0
* GCC 4.7, 8.3, 9.2 Linux
* Clang MacOSX

**Mingw-64/gcc is not supported** due to [1](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54412) [2](https://sourceforge.net/p/mingw-w64/mailman/message/34453497/) [3](https://stackoverflow.com/questions/30928265/mingw64-is-incapable-of-32-byte-stack-alignment-required-for-avx-on-windows-x64) [4](https://github.com/msys2/MSYS2-packages/issues/1209), etc.
So cross compiling from Linux to Windows using this compiler is not currently an option. That's why this module has not been included in the main Godot repository. 

However you should be able to build it without issue on your own system or you can use my prebuilt Godot binaries (pending).

These SIMD instruction sets are supported.
* SSE2
* SSE4.1
* AVX2 - FMA3
* AVX-512F
* ARM NEON

## How To Use

1. Go to the modules folder in your Godot source: `cd ~/godot/modules`
1. Clone this repository: `git clone https://github.com/tinmanjuggernaut/godot_fastnoise_simd.git`
1. Rename the folder to `fastnoise_simd`: `mv godot_fastnoise_simd fastnoise_simd`
1. Rebuild Godot.
1. Now when creating a new `Noise` object, such as in a texture, you should see FastNoiseSIMD along with the others. You can also view the online help and search FastNoiseSIMD.

## Benchmarks
Here you can see some benchmarks from FastNoiseSIMD compared to regular FastNoise and OpenSimplexNoise in Godot. These numbers are the time needed to generate a 2D image in ms.

|   | 1k | 2k | 4k |
---|---|---|---
OpenSimplexNoise | 107 | 435 | 1730 |
FN Perlin Fractal | 104 | 400 | 1600 |
SIMD Perlin Fractal (singular) | 304 | 1210 | 4800 |
**SIMD Perlin Fractal** | **28** | **107** | **430** | 
---|---|---|---
FN Value | 74 | 294 | 
**SIMD Value** | **13** | **44** |
---|---|---|---
FN heavily tweaked Cellular |  | 2284 |
**SIMD heavily tweaked Cellular** |  | **229** |

This library provides singular lookup functions (e.g. get_noise_3d(x,y,z)), but they are slow. Speed gains come from requesting large blocks at once, **especially when Z is a multiple of 8**.
 
Generating basic 2D textures with FastNoiseSIMD is 4-6x faster than FastNoise, and becomes even faster as the algorithms gets more complex, even into 3D. Generating large blocks of noise can be split up into multiple threads becoming *another* 3-7x faster or (up to 40x faster than FastNoise)! See [pyFastNoiseSIMD](https://github.com/robbmcleod/pyfastnoisesimd) for multithreaded benchmarks.

### What happens if you run this on a system that doesn't support SIMD?
All new intel 64-bit processors have SSE2 built in, so I can't turn it off for my system, but I have compared regular FastNoise with FastNoiseSIMD SSE2 and AVX2. I'd guess retrieving FastNoiseSIMD sets without using SIMD is on par with regular FastNoise. Or it may be better because the algorithm is working in one set, rather than being called for every individual pixel/voxel. 

Here I generated a 1k image using the library author's [demo app](https://github.com/Auburns/FastNoiseSIMD/releases/tag/0.7):

|   | FN | SIMD SSE2 | SIMD AVX2 
---|---|---|---
Value | 66| 23 | 6 
Perlin Fractal | 108 | 94 | 43
Cellular Noise Lookup to Simplex | 96 | 117 | 61
Cellular Distance2 Sub | 209 | 75 | 30 

## Screenshots

![image](https://user-images.githubusercontent.com/632766/72280304-ff8fb080-3672-11ea-8925-8461fa6b0d5b.jpg)
![image](https://user-images.githubusercontent.com/632766/74675870-89411980-51ef-11ea-82f8-cc243db21409.png)
![image](https://user-images.githubusercontent.com/632766/74677485-66b0ff80-51f3-11ea-8d3a-aac338fb81f5.png)

