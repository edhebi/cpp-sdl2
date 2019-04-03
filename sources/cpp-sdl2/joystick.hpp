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
	const bool	owner_	= true;

	///Construct a non-owning joystick object
	Joystick(SDL_Joystick* stick, bool owning_state)
		: joystick_(stick), owner_(owning_state)
	{
		assert(!owner_);
	}

	///Move utility
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
	///Default ctor, create a non-valid (empty) joystick object
	Joystick() : owner_(false) {}

	///Create a joystick by opening a joystick device
	Joystick(int device_index) : joystick_(SDL_JoystickOpen(device_index))
	{
		if (!joystick_) throw Exception("SDL_JoystickOpen");
	}

	///Create a joystick from an SDL joystick
	Joystick(SDL_Joystick* joystick) : joystick_(joystick) {}

	Joystick(Joystick const&) = delete;
	Joystick& operator=(Joystick const&) = delete;

	///move ctor
	Joystick(Joystick&& other) { move_from(other); }

	///move assign operator
	Joystick& operator=(Joystick&& other)
	{
		move_from(other);
		return *this;
	}

	///Will automatically close the joystick when going out of scope if this object owns the pointer
	~Joystick()
	{
		if (owner_ && joystick_) SDL_JoystickClose(joystick_);
	}

	///Open haptics for this device
	Haptic open_haptic() const { return Haptic(joystick_); }

	///Get the power level of the joystick. Will throw if unknown
	SDL_JoystickPowerLevel power_level() const
	{
		const auto power = SDL_JoystickCurrentPowerLevel(joystick_);

		if (power == SDL_JOYSTICK_POWER_UNKNOWN)
			throw Exception("SDL_JoystickCurrentPowerLevel");

		return power;
	}

	///Return true if device is currently attached
	bool attached() const
	{
		return SDL_JoystickGetAttached(joystick_) == SDL_TRUE;
	}

	///Get the current immediate value of the given axis
	int16_t get_axis(int axis) const
	{
		return SDL_JoystickGetAxis(joystick_, axis);
	}

	///Get the current immediate value of the given trackball
	sdl::Vec2i get_ball(int ball) const
	{
		Vec2i	 d;
		const int status = SDL_JoystickGetBall(joystick_, ball, &d.x, &d.y);

		if (status < 0) throw Exception("SDL_JoystickGetBall");

		return d;
	}

	///Get the current immediate value of the given button
	uint8_t get_button(int button) const
	{
		return SDL_JoystickGetButton(joystick_, button);
	}

	///Get the current immediate value of the given hat
	uint8_t get_hat(int hat) const
	{
		return SDL_JoystickGetHat(joystick_, hat);
	}

	///Get the name of the joystick
	std::string name() const { return {SDL_JoystickName(joystick_)}; }

	///Get how many hats
	int num_hats() const
	{
		const int value = SDL_JoystickNumHats(joystick_);

		if (value < 0) throw Exception("SDL_JoystickNumHats");

		return value;
	}

	///Get how many buttons
	int num_buttons() const
	{
		const int value = SDL_JoystickNumButtons(joystick_);

		if (value < 0) throw Exception("SDL_JoystickNumButtons");

		return value;
	}

	///Get how many balls
	int num_balls() const
	{
		const int value = SDL_JoystickNumBalls(joystick_);

		if (value < 0) throw Exception("SDL_JoystickNumBalls");

		return value;
	}

	///Get how many axes
	int num_axes() const
	{
		const int value = SDL_JoystickNumAxes(joystick_);

		if (value < 0) throw Exception("SDL_JoystickNumAxes");

		return value;
	}

	///Get this joystick instance id
	SDL_JoystickID instance_id() const
	{
		const auto value = SDL_JoystickInstanceID(joystick_);

		if (value < 0) throw Exception("SDL_JoystickInstanceID");

		return value;
	}

	///construct a non-owning wrapper around this joystick
	static Joystick non_owning(SDL_Joystick* stick) { return {stick, false}; }

	///construct a non-owning wrapper around this joystick id
	static Joystick non_joystick(SDL_JoystickID joyid)
	{
		auto object = Joystick(SDL_JoystickFromInstanceID(joyid), false);

		if (object.joystick_ == nullptr)
			throw Exception("SDL_JoystickFromInstanceID");

		return object;
	}

	bool operator==(Joystick const& other) const
	{
		return joystick_ == other.joystick_;
	}

	bool operator==(SDL_Joystick* other) const { return joystick_ == other; }

	bool operator==(SDL_JoystickID other) const
	{
		return instance_id() == other;
	}
};
} // namespace sdl
