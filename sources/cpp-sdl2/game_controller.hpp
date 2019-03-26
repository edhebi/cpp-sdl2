#pragma once

#include <SDL_gamecontroller.h>

namespace sdl
{

///\brief Represent a gamepad (game controller)
class GameController
{
public:
	GameController(int joystick_index) :
		controller_(SDL_GameControllerOpen(joystick_index))
	{
		if(!controller_)
		{
			//error
		}
	}

	GameController() = default;

	GameController(GameController const&) = delete;
	GameController& operator=(GameController const&) = delete;
	GameController(GameController&& other)
	{
		move_from(other);
	}
	GameController& operator=(GameController&& other)
	{
		move_from(other);
		return *this;
	}

	~GameController()
	{
		if (controller_)
			SDL_GameControllerClose(controller_);
	}

	SDL_GameController* ptr() const
	{
		return controller_;
	}


private:

	void move_from(GameController& other)
	{
		controller_ = other.ptr();
		other.controller_ = nullptr;
	}

	SDL_GameController* controller_ = nullptr;

};

}
