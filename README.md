# cpp-sdl2 
![linux](https://github.com/Edhebi/cpp-sdl2/workflows/linux/badge.svg)
![windows](https://github.com/Edhebi/cpp-sdl2/workflows/windows/badge.svg)
![doc](https://github.com/Edhebi/cpp-sdl2/workflows/doc/badge.svg)

Basic C++17 bindings of SDL2 (https://wiki.libsdl.org/FrontPage) implemented as an header-only library

## Doxygen doc

https://edhebi.github.io/cpp-sdl2/doc

## Usage

This library has been written in C++17. It should work out of the box with any modern version of GCC, Clang and MSVC, but you may need to [set](https://gcc.gnu.org/projects/cxx-status.html#cxx17) [some](https://clang.llvm.org/cxx_status.html) [flags](https://docs.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version?view=vs-2019) to C++17.

This is an header-only library, the only needed step to use is is to make the content of the "sources" directory visible by your compiler and to `#include "cpp-sdl2/sdl.hpp"` instead of `SDL.h`. Then you just link against the SDL2 and SDL_image libraries as usual.

By default, the code will throw exception in case of an SDL_Error. The Exception will contain the string returned by `SDL_GetError()`

`cpp-sdl2` conviniently wrap SDL's functionallities that are aimed at making cross-platform OpenGL and Vulkan developement easier. For instance, the window class can help you create an opengl context, or a vulkan instance/surface without having to worry if you are running on a Windows or a Linux platform or something else. This functionality can be activated by defining `CPP_SDL2_GL_WINDOW`or `CPP_SDL2_VK_WINDOW` before including `sdl.hpp`

Exception support can be disabled by defining `CPP_SDL2_NOEXCEPTIONS` in the preprocessor. 

To be able to easilly load images into surfaces, you can install SDL_Image 2, and define `CPP_SDL2_USE_SDL_IMAGE`

## Compile-time configuration

cpp-sdl2 aims at offering a simple and modern API to C++ developers of the functionality available to SDL. Some of these functionalities are only usable with the addition of external libraries, or with the presens of specific SDK on the system.

To enable these functionality, simply `#define CPP_SDL2_TOKEN` the specified token from the table below before including any files from cpp-sdl2.

Additional functionalities are disabled by default, as the goal is to be the most compatible out of the box. They can be easilly toggled in by defining a few perprocessor constants : 


| Preprocessor switch | Descripiton | Requirements| 
|---------------------|-----------|--------------|
|`CPP_SDL2_GL_WINDOW` | Makes OpenGL functionalities around windows availalbe | You should probably use an OpenGL extension loader like `glew`, `gl3w`, `glad` or `epoxy`|
|`CPP_SDL2_VK_WINDOW`| Adds the Vulkan-related functions to the window API to help you load Vulkan platform extensions and create your `vk::Instance` and `vk::Surface`| You need both a Vulkan runtime and a recent version of the Vulkan SDK. The wrappers are intened to be used with Nvidia's (now standardized) `vulkan.hpp` wrappers|
|`CPP_SDL2_USE_SDL_IMAGE`|Add a few helpers that permit to load image files inside `sdl::Surface` objects | Require you to include and link the `SDL_Image` library for `SDL2`|
|`CPP_SDL2_NOEXCEPTIONS`|Disable `cpp-sdl2` exceptions | This is mostly intended if you want to use `cpp-sdl2` on a platform where exceptions aren't available. Exceptions are used only **exceptionally** by `cpp-sdl2`, and their general problems in the context of game developements shouldn't be an issue as, if correctly used, the library should *not* throw any exceptions. This replaces any thrown exception with a logged message to the error output, followed by an `abort()` of your program.| 


## Dependencies

- SDL2
- SDL_image 2 (optional)
- OpenGL-complient video driver (optional. Only for OpenGL context creation)
- Vulkan SDK (optnional. for Vulkan instance/surface creation utilities)

The following versions of SDL2 are tesded and confirmed working:

- 2.0.8	(This is a "legacy" version as far as we are concerned. You should upgrade!)
- 2.0.9
- 2.0.10

**You will need a C++17 complient compiler**

## Example program

```cpp
#include "cpp-sdl2/sdl.hpp"
#include <cstdlib> // Using C-style rand
#include <ctime>

#include <iostream>

int main(int argc, char * argv[])
{
	(void)argc;
	(void)argv;

	std::srand(unsigned(std::time(nullptr)));

	// The following classes manages the lifetime of SDL declared resources RAII style

	auto root = sdl::Root(SDL_INIT_EVENTS);

	auto window = sdl::Window{ "Random Colors", {600, 600} };

	auto renderer = window.make_renderer();
	
	auto color = sdl::Color::Black();

	auto done   = false;
	auto redraw = true;
	auto event = sdl::Event{};

	while (!done)
	{
		if (redraw)
		{
			renderer.clear(color);
			renderer.present();
			redraw = false;
		}
	
		event.wait();
		
		if (event.type == SDL_QUIT)
		{
			done = true;
		}

		if (event.type == SDL_MOUSEBUTTONUP)
		{
			color.r = std::rand() % 256;
			color.g = std::rand() % 256;
			color.b = std::rand() % 256;
			redraw = true;
		}
	}
	return 0;

	// Cleanup is done automatically

}
```
