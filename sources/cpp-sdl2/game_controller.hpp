#pragma once

#include "exception.hpp"
#include "haptic.hpp"
#include <SDL.h>
#include <SDL_gamecontroller.h>
#include <cassert>
#include <chrono>
#include <string>

namespace sdl
{
///\brief Represent a gamepad (game controller)
class GameController
{
public:
	///Construct a controller from a joystick index, throws if that index is not a game controller
	GameController(int joystick_index) : controller_(SDL_GameControllerOpen(joystick_index))
	{
		if (!controller_)
		{
			throw Exception("SDL_GameControllerOpen");
		}
	}

	///Construct a controller from a GameController pointer. This object will take ownership of the controller pointed.
	GameController(SDL_GameController* controller) : controller_(controller) {}

	///Construct an empty controller
	GameController() = default;

	///Not copyable
	GameController(GameController const&) = delete;
	GameController& operator=(GameController const&) = delete;

	///move ctor
	GameController(GameController&& other) noexcept { *this = std::move(other); }

	///move-assing operator
	GameController& operator=(GameController&& other) noexcept
	{
		if (controller_ != other.controller_)
		{
			controller_ = other.ptr();
			// We never want the value of the "owned_" boolean to change in the
			// life of one of these objects. however, in case of a move
			// operation, we need to transfer the owning status of the wrapper
			// In the general case, we would only want to rely on the fact that
			// the pointer is null or not. But here, we want to be able to
			// easily construct and manipulate a pointer that is not managed as
			// an RAAI object. This is ugly, and I'm upset about it. But we're
			// gonna cast-away const for once
			*(const_cast<bool*>(&owned_))		= other.owned_;
			*(const_cast<bool*>(&other.owned_)) = false;
			other.controller_					= nullptr;
		}
		return *this;
	}

	///Close the opened controller pointer, unless this wrapper was created via GameController::non_owning()
	~GameController()
	{
		if (owned_ && controller_) SDL_GameControllerClose(controller_);
	}

	///Get the SDL pointer
	SDL_GameController* ptr() const { return controller_; }

	///Open the haptic device from the controller
	Haptic open_haptic() const { return {SDL_GameControllerGetJoystick(controller_)}; }

	///Return true if this controller is attached
	bool is_attached() const { return SDL_GameControllerGetAttached(controller_) == SDL_TRUE; }

	///Get the current imediate value of the given axis
	int16_t get_axis(SDL_GameControllerAxis axis) const
	{
		return SDL_GameControllerGetAxis(controller_, axis);
	}

	///Get the current imedate value of the given button
	int8_t get_button(SDL_GameControllerButton button) const
	{
		return SDL_GameControllerGetButton(controller_, button);
	}

#if SDL_VERSION_ATLEAST(2, 0, 9)
	///Play a simple rumble. If the controller has 2 motors, the two values will control one of them. If the controller only has one, the values will be mixed together
	int rumble(uint16_t low_freq, uint16_t high_freq, std::chrono::milliseconds duration) const
	{
		return rumble(low_freq, high_freq, static_cast<uint32_t>(duration.count()));
	}

	///\copydoc GameController::rumble
	int rumble(uint16_t low_freq, uint16_t high_freq, uint32_t millisec_duration) const
	{
		return SDL_GameControllerRumble(controller_, low_freq, high_freq, millisec_duration);
	}
#endif

	std::string name() const
	{
		if (!controller_) return {};

		return {SDL_GameControllerName(controller_)};
	}

	static std::string get_controller_name(int joystick_index)
	{
		const char* name = SDL_GameControllerNameForIndex(joystick_index);
		if (!name) throw Exception("SDL_GameControllerNameForIndex");
		return {name};
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
	///Load a file database
	static int load_mapping_database(std::string const& file_path)
	{
		return load_mapping_database(file_path.c_str());
	}

	///Load a file database
	static int load_mapping_database(const char* file_path)
	{
		const auto state = SDL_GameControllerAddMappingsFromFile(file_path);
		if (state < 0)
		{
			throw Exception("SDL_GameControllerAddMappingsFromFile");
		}

		return state;
	}

	///Add a mapping string
	static int add_mapping(std::string const& mapping_string)
	{
		return add_mapping(mapping_string.c_str());
	}

	///Add a mapping string
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
	///Try to open all available controllers, and return an array of all controller sucessfully openned
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
				catch (sdl::Exception const& /*e*/)
				{
					// could not open this controller
					continue;
				}
			}
		}

		return controllers;
	}

	///Create a non_owning controller around a stick ID, to use the C++ API without managing the controller
	static GameController non_owning(SDL_JoystickID joystick_id)
	{
		return {SDL_GameControllerFromInstanceID(joystick_id), false};
	}

	///Create a non_owning controller around an SDL controller pointer, to use the C++ aPI withiout managing the controller
	static GameController non_owning(SDL_GameController* controller) { return {controller, false}; }

private:
	///Private controller for a non-onwer controller. The bool argument is expected to be false here
	GameController(SDL_GameController* controller, bool non_owned)
		: controller_(controller), owned_(non_owned)
	{
		assert(!owned_);
	}

	SDL_GameController* controller_ = nullptr;
	bool const			owned_		= true;
};
} // namespace sdl
