# cpp-sdl2 [![Build Status](https://travis-ci.com/Edhebi/cpp-sdl2.svg?branch=master)](https://travis-ci.com/Edhebi/cpp-sdl2) [![Build status](https://ci.appveyor.com/api/projects/status/aslfsrskbb02a4r3?svg=true)](https://ci.appveyor.com/project/Ybalrid/cpp-sdl2)
Basic c++ bindings of SDL2 (https://wiki.libsdl.org/FrontPage) implemented as an header-only library

## Doxygen doc

https://edhebi.github.io/cpp-sdl2/doc

## Usage

Makes the content of the "sources" directory visible by your compiler. `#include "cpp-sdl2/sdl.hpp"` Link against the SDL2 and SDL_image libraries as usual.

By default, the code will throw exception in case of an SDL_Error. The Exception will contain the string returned by `SDL_GetError()`

`cpp-sdl2` also conviniently wrap SDL's functionallities aimed at making cross-platform OpenGL/Vulkan developement easier. For instance, the window class can help you create an opengl context, or a vulkan instance/surface without having to worry if you are running on a Windows platform or something else. This functionality can be activated by defining `CPP_SDL2_GL_WINDOW`or `CPP_SDL2_VK_WINDOW` before including `sdl.hpp`

Exception support can be disabled by defining `CPP_SDL2_NOEXCEPTIONS` in the preprocessor. 

To be able to easilly load images into surfaces, you can install SDL_Image 2, and define `CPP_SDL2_USE_SDL_IMAGE`

## Compile-time configuration

cpp-sdl2 aims at offering a simple and modern API to C++ developers of the functionality available to SDL. Some of these functionalities are only usable with the addition of external libraries, or with the presens of specific SDK on the system.

The additional functionality is disabled by default, as the goal is to be the most compatible out of the box, but they can be easilly toggled in by defining a few perprocessor constants : 


| Preprocessor switch | Descripiton | Requirements| 
|---------------------|-----------|--------------|
|`CPP_SDL2_GL_WINDOW` | Makes OpenGL functionalities around windows availalbe | You should probably use an OpenGL extension loader like `glew`, `gl3w`, `glad` or `epoxy`|
|`CPP_SDL2_VK_WINDOW`| Adds the Vulkan-related functions to the window API to help you load Vulkan platform extensions and create your `vk::Instance` and `vk::Surface`| You need both a Vulkan runtime and a recent version of the Vulkan SDK. The wrappers are intened to be used with Nvidia's (now standardized) `vulkan.hpp` wrappers|
|`CPP_SDL2_USE_SDL_IMAGE`|Add a few helpers that permit to load image files inside `sdl::Surface` objects | Require you to include and link the `SDL_Image` library for `SDL2`|
|`CPP_SDL2_NOEXCEPTIONS`|Disable `cpp-sdl2` exceptions | This is mostly intended if you want to use `cpp-sdl2` on a platform where exceptions aren't available. Exceptions are used only **exceptionally** by `cpp-sdl2`, and their general problems in the context of game developements shouldn't be an issue as, if correctly used, the library should *not* throw any exceptions. This replaces any thrown exception with a logged message to the error output, followed by an `abort()` of your program.| 


## Dependencies

- SDL2
- SDL_image 2 (optional)

You will need a C++17 complient compiler

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
