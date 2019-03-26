#define CPP_SDL2_GL_WINDOW

#include <cpp-sdl2/sdl.hpp>
#include <cstdlib> // Using C-style rand
#include <ctime>

#include <iostream>

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	std::srand(unsigned(std::time(nullptr)));

	// The following classes manages the lifetime of SDL declared resources RAII
	// style

	auto root = sdl::Root(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);

	auto window = sdl::Window{"Random Colors", {600, 600}};

	auto renderer = window.make_renderer();

	auto color = sdl::Color::Black();

	auto done	= false;
	auto redraw = true;
	auto event	= sdl::Event{};

	// This utility function will open all the game controllers connected to the
	// system that are known from the SDL GameController API
	auto controllers = sdl::GameController::open_all_available_controllers();
	if (!controllers.empty())
	{
		std::cout << "Detected controllers:\n";
		for (auto& controller : controllers)
		{
			std::cout << "  " << controller.get_name() << '\n';
		}
	}

	while (!done)
	{
		if (redraw)
		{
			renderer.clear(color);
			renderer.present();
			redraw = false;
		}

		event.wait();

		switch (event.type)
		{
		case SDL_QUIT: done = true; break;
		case SDL_MOUSEBUTTONUP:
		case SDL_KEYUP:
		case SDL_CONTROLLERBUTTONDOWN:
			color.r = std::rand() % 256;
			color.g = std::rand() % 256;
			color.b = std::rand() % 256;
			redraw	= true;
		default: break;
		}
	}
	return 0;

	// Cleanup is done automatically
}
