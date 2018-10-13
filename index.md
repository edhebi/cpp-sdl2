# cpp-sdl2 [![Build Status](https://travis-ci.com/Edhebi/cpp-sdl2.svg?branch=master)](https://travis-ci.com/Edhebi/cpp-sdl2)
Basic c++ bindings of SDL2 (https://wiki.libsdl.org/FrontPage) implemented as an header-only library

 - [API Documentation](./doc/)

## Usage

Makes the content of the "sources" directory visible by your compiler. `#include "sdl.hpp"` Link against the SDL2 and SDL_image libraries as usual.

By default, the code will throw exception in case of an SDL_Error. The Exception will contain the string returned by `SDL_GetError()`

`cpp-sdl2` also conviniently wrap SDL's functionallities aimed at making cross-platform OpenGL/Vulkan developement easier. For instance, the window class can help you create an opengl context, or a vulkan instance/surface without having to worry if you are running on a Windows platform or something else. This functionality can be activated by defining `CPP_SDL2_GL_WINDOW`or `CPP_SDL2_VK_WINDOW` before including `sdl.hpp`

Exception support can be disabled by defining `CPP_SDL2_NOEXCEPTIONS` in the preprocessor. 

To be able to easilly load images into surfaces, you can install SDL_Image 2, and define `CPP_SDL2_USE_SDL_IMAGE`

## Dependencies

- SDL2
- SDL_image 2 (optional)

You will need a C++17 complient compiler

## Example program

```cpp
#include "sdl/sdl.hpp"
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
