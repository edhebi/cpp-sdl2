# cpp-sdl2
Basic c++ bindings of SDL2 (https://wiki.libsdl.org/FrontPage) implemented as an header-only library

## Usage

Makes the content of the "sources" directory visible by your compiler. `#include "sdl.hpp"` Link against the SDL2 and SDL_image libraries as usual.

By default, the code will throw exception in case of an SDL_Error. The Exception will contain the string returned by `SDL_GetError()`

Exception support can be disabled by defining `CPP_SDL2_NOEXCEPTIONS` in the preprocessor. 

## Dependencies

- SDL2
- SDL_image 2

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
