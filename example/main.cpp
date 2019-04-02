#define CPP_SDL2_GL_WINDOW

#include <cpp-sdl2/sdl.hpp>
#include <cstdlib> // Using C-style rand
#include <ctime>
#include <chrono>
#include <iostream>

int main(int argc, char* argv[])
{
	using namespace std::literals::chrono_literals;

	(void)argc;
	(void)argv;

	std::srand(unsigned(std::time(nullptr)));

	// The following classes manages the lifetime of SDL declared resources RAII
	// style

	auto root =
		sdl::Root(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

	auto window = sdl::Window{"Random Colors", {600, 600}};

	auto renderer = window.make_renderer();

	auto color = sdl::Color::Black();

	auto done	= false;
	auto redraw = true;
	auto event	= sdl::Event{};

	// maximum power rumble for 200 millisecond
	sdl::Haptic::Effect hard_rumble_effect;
	hard_rumble_effect.type						 = SDL_HAPTIC_LEFTRIGHT;
	hard_rumble_effect.leftright.length			 = 200;
	hard_rumble_effect.leftright.large_magnitude = 0xFFFF;
	hard_rumble_effect.leftright.small_magnitude = 0xFFFF;

	// This utility function will open all the game controllers connected to the
	// system that are known from the SDL GameController API
	auto controllers = sdl::GameController::open_all_available_controllers();

	// Try to get the first controller's haptic device
	auto main_haptic = !controllers.empty() ? controllers.front().open_haptic()
											: sdl::Haptic();

	// This is to store the effect
	sdl::Haptic::InstalledEffect hard_rumble;

	try
	{
		if (main_haptic.valid()
			&& main_haptic.is_effect_compatible(hard_rumble_effect))
		{
			hard_rumble = main_haptic.new_effect(hard_rumble_effect);
			hard_rumble.run(1);
		}
	}
	catch (sdl::Exception const& e)
	{
		sdl::show_message_box(
			SDL_MESSAGEBOX_ERROR,
			"Couldn't install or play SDL haptic effect on controller",
			e.what(),
			window);
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
#if SDL_VERSION_ATLEAST(2, 0, 9)
		case SDL_CONTROLLERAXISMOTION:
			sdl::GameController::non_owning(event.caxis.which)
				.rumble(
					event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX
							|| event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY
						? 0xFFFF
						: 0x0,
					event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX
							|| event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY
						? 0xFFFF
						: 0x0,
					10);

			break;
#endif
		case SDL_QUIT: done = true; break;
		case SDL_MOUSEBUTTONUP:
		case SDL_KEYUP:
		case SDL_CONTROLLERBUTTONDOWN:
			color.r = std::rand() % 256;
			color.g = std::rand() % 256;
			color.b = std::rand() % 256;
			redraw	= true;
			break;
		case SDL_CONTROLLERBUTTONUP:
			if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A)
			{
				main_haptic.run_effect(hard_rumble);
			}
		default: break;
		}
	}
	return 0;

	// Cleanup is done automatically
}
