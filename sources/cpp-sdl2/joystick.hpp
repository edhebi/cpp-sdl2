#pragma once

#include "exception.hpp"
#include "haptic.hpp"
#include "vec2.hpp"
#include <SDL_joystick.h>
#include <cassert>

namespace sdl
{
class Joystick
{
	SDL_Joystick* joystick_ = nullptr;
	const bool	  owner_	= true;

	Joystick(SDL_Joystick* stick, bool owning_state)
		: joystick_(stick), owner_(owning_state)
	{
		assert(!owner_);
	}

	void move_from(Joystick& other)
	{
		joystick_		= other.joystick_;
		other.joystick_ = nullptr;

		// this is the exceptional case when we actually want to reassign the
		// value of the "owner" boolean
		const_cast<bool&>(owner_)		= other.owner_;
		const_cast<bool&>(other.owner_) = false; // for good measure
	}

public:
	Joystick() = default;
	Joystick(int device_index) : joystick_(SDL_JoystickOpen(device_index))
	{
		if (!joystick_) throw Exception("SDL_JoystickOpen");
	}

	Joystick(SDL_Joystick* joystick) : joystick_(joystick) {}

	Joystick(Joystick const&) = delete;
	Joystick& operator=(Joystick const&) = delete;

	Joystick(Joystick&& other) { move_from(other); }

	Joystick& operator=(Joystick&& other)
	{
		move_from(other);
		return *this;
	}

	~Joystick()
	{
		if (owner_ && joystick_) SDL_JoystickClose(joystick_);
	}

	Haptic open_haptic() const { return Haptic(joystick_); }

	SDL_JoystickPowerLevel power_level() const
	{
		const auto power = SDL_JoystickCurrentPowerLevel(joystick_);

		if (power == SDL_JOYSTICK_POWER_EMPTY)
			throw Exception("SDL_JoystickCurrentPowerLevel");

		return power;
	}

	bool attached() const
	{
		return SDL_JoystickGetAttached(joystick_) == SDL_TRUE;
	}

	int16_t get_axis(int axis) const
	{
		return SDL_JoystickGetAxis(joystick_, axis);
	}

	sdl::Vec2i get_ball(int ball) const
	{
		Vec2i	  d;
		const int status = SDL_JoystickGetBall(joystick_, ball, &d.x, &d.y);

		if (status < 0) throw Exception("SDL_JoystickGetBall");

		return d;
	}

	uint8_t get_button(int button)
	{
		return SDL_JoystickGetButton(joystick_, button);
	}

	uint8_t get_hat(int hat) { return SDL_JoystickGetHat(joystick_, hat); }

	std::string name() const { return {SDL_JoystickName(joystick_)}; }

	int num_hats() const
	{
		const int value = SDL_JoystickNumHats(joystick_);

		if (value < 0) throw Exception("SDL_JoystickNumHats");

		return value;
	}

	int num_buttons() const
	{
		const int value = SDL_JoystickNumButtons(joystick_);

		if (value < 0) throw Exception("SDL_JoystickNumButtons");

		return value;
	}

	int num_balls() const
	{
		const int value = SDL_JoystickNumBalls(joystick_);

		if (value < 0) throw Exception("SDL_JoystickNumBalls");

		return value;
	}

	int num_axes() const
	{
		const int value = SDL_JoystickNumAxes(joystick_);

		if (value < 0) throw Exception("SDL_JoystickNumAxes");

		return value;
	}

	SDL_JoystickID instance_id() const
	{
		const auto value = SDL_JoystickInstanceID(joystick_);

		if (value < 0) throw Exception("SDL_JoystickInstanceID");

		return value;
	}

	static Joystick non_owning(SDL_Joystick* stick) { return {stick, false}; }

	static Joystick non_joystick(SDL_JoystickID joyid)
	{
		auto object = Joystick(SDL_JoystickFromInstanceID(joyid), false);

		if (object.joystick_ == nullptr)
			throw Exception("SDL_JoystickFromInstanceID");

		return object;
	}
};
} // namespace sdl
