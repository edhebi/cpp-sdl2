#define CPP_SDL2_VK_WINDOW
#define CPP_SDL2_GL_WINDOW

#include "sdl.hpp"
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
