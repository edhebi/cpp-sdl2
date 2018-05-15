# cpp-sdl2
basic c++ bindings of SDL2 (https://wiki.libsdl.org/FrontPage)

# Example programm
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

	auto root = sdl::Root(SDL_INIT_EVENTS);

	auto window = sdl::Window{ "Random Colors", {600, 600} };

	auto renderer = window.make_renderer();
	renderer.clear(sdl::Color::Black());
	renderer.present();

	sdl::Color color;

	bool done   = false;
	bool redraw = false;
	sdl::Event event;

	while (!done)
	{
		event.wait();
		
		if (event.type == SDL_QUIT)
		{
			done = true;
		}

		if (event.type == SDL_MOUSEBUTTONUP)
		{
			color.r = std::rand() % 255;
			color.g = std::rand() % 255;
			color.b = std::rand() % 255;
			redraw = true;
		}

		if (redraw)
		{
			renderer.clear(color);
			renderer.present();
			redraw = false;
		}
	}
	return 0;
}
```
