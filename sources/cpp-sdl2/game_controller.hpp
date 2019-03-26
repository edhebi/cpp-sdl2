#pragma once

#include "exception.hpp"
#include <SDL_gamecontroller.h>
#include <chrono>
#include <string>

namespace sdl
{
///\brief Represent a gamepad (game controller)
class GameController
{
public:
	GameController(int joystick_index)
		: controller_(SDL_GameControllerOpen(joystick_index))
	{
		if (!controller_)
		{
			throw Exception("SDL_GameControllerOpen");
		}
	}

	GameController()					  = default;
	GameController(GameController const&) = delete;
	GameController& operator=(GameController const&) = delete;

	GameController(GameController&& other) { move_from(other); }

	GameController& operator=(GameController&& other)
	{
		move_from(other);
		return *this;
	}

	~GameController()
	{
		if (controller_) SDL_GameControllerClose(controller_);
	}

	SDL_GameController* ptr() const { return controller_; }

	bool is_attached() const
	{
		return SDL_GameControllerGetAttached(controller_) == SDL_TRUE;
	}

	int16_t get_axis(SDL_GameControllerAxis axis) const
	{
		return SDL_GameControllerGetAxis(controller_, axis);
	}

	int8_t get_button(SDL_GameControllerButton button) const
	{
		return SDL_GameControllerGetButton(controller_, button);
	}

	int rumble(
		uint16_t				  low_freq,
		uint16_t				  high_freq,
		std::chrono::milliseconds duration)
	{
		return rumble(
			low_freq, high_freq, static_cast<uint32_t>(duration.count()));
	}

	int rumble(
		uint16_t low_freq, uint16_t high_freq, uint32_t millisec_duration)
	{
		return SDL_GameControllerRumble(
			controller_, low_freq, high_freq, millisec_duration);
	}

	std::string get_name()
	{
		if (!controller_) return {};

		return {SDL_GameControllerName(controller_)};
	}

	static std::string get_controller_name(int joystick_index)
	{
		const char* name = SDL_GameControllerNameForIndex(joystick_index);
		if (!name)
			throw Exception("SDL_GameControllerNameForIndex");
		return { name };
	}

	// static std::string get_axis_name(SDL_GameControllerAxis axis)
	//{
	//	//todo error when SDL return null
	//	return { SDL_GameControllerGetStringForAxis(axis) };
	//}

	// static std::string get_button_name(SDL_GameControllerButton button)
	//{
	//	//todo error when SDL return null
	//	return { SDL_GameControllerGetStringForButton(button) };
	//}

	// Bindings management wrappers
	static int load_mapping_database(std::string const& file_path)
	{
		return load_mapping_database(file_path.c_str());
	}

	static int load_mapping_database(const char* file_path)
	{
		const auto state = SDL_GameControllerAddMappingsFromFile(file_path);
		if (state < 0)
		{
			throw Exception("SDL_GameControllerAddMappingsFromFile");
		}

		return state;
	}

	static int add_mapping(std::string const& mapping_string)
	{
		return add_mapping(mapping_string.c_str());
	}
	static int add_mapping(const char* mapping_string)
	{
		const auto state = SDL_GameControllerAddMapping(mapping_string);

		if (state < 0)
		{
			throw Exception("SDL_GameControllerAddMapping");
		}

		return state;
	}

	// convinience functions
	static std::vector<GameController> open_all_available_controllers()
	{
		std::vector<GameController> controllers;

		for (int nb_sticks = SDL_NumJoysticks(), i = 0; i < nb_sticks; ++i)
		{
			if (SDL_IsGameController(i))
			{
				try
				{
					controllers.emplace_back(i);
				}
				catch (sdl::Exception const& e)
				{
					// could not open this controller
					continue;
				}
			}
		}

		return controllers;
	}

private:
	void move_from(GameController& other)
	{
		controller_		  = other.ptr();
		other.controller_ = nullptr;
	}

	SDL_GameController* controller_ = nullptr;
};
} // namespace sdl
