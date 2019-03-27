#pragma once

#include <SDL_haptic.h>

namespace sdl
{
class Haptic
{
	SDL_Haptic* haptic_ = nullptr;

	using effect_sdlid = int;
	using effect_list  = std::vector<effect_sdlid>;
	effect_list my_effects{};

	void move_from(Haptic& other)
	{
		haptic_ = other.haptic_;
		my_effects = std::move(other.my_effects);
		other.haptic_ = nullptr;
	}

public:

	SDL_Haptic* ptr() const
	{
		return haptic_;
	}

	using effect_handle = effect_list::size_type;
	static constexpr effect_handle invalid_effect = effect_list::size_type(~0ULL);

	union Effect
	{
		// This is analog to how we handle sdl events, this definition is copied
		// from SDL_haptic.h
		Uint16				type;	   /**< Effect type. */
		SDL_HapticConstant	constant;  /**< Constant effect. */
		SDL_HapticPeriodic	periodic;  /**< Periodic effect. */
		SDL_HapticCondition condition; /**< Condition effect. */
		SDL_HapticRamp		ramp;	   /**< Ramp effect. */
		SDL_HapticLeftRight leftright; /**< Left/Right effect. */
		SDL_HapticCustom	custom;	   /**< Custom effect. */

		///this permit to treat an sdl::haptic::effect instance as if it was an SDL_HapticEffect to it's own content
		operator SDL_HapticEffect*() const
		{
			//"I solemnly swear that I am up to no good."
			return (SDL_HapticEffect*)(this); 
		}

		Effect()
		{
			// be sure to zero initialize the enclosed effect
			SDL_memset(this, 0, sizeof(Effect));
		}
	};

	Haptic(){}

	Haptic(int haptic_index) : haptic_{SDL_HapticOpen(haptic_index)}
	{
		if (!haptic_)
		{
			throw Exception("SDL_HapticOpen");
		}
	}

	Haptic(SDL_Joystick* joystick)
		: haptic_{SDL_HapticOpenFromJoystick(joystick)}
	{
		if (!haptic_)
		{
			throw Exception("SDL_HapticOpenFromJoystick");
		}
	}

	~Haptic()
	{
		if (haptic_)
		{
			//todo add another RAII wrapper so we destroy these effects when we die?
			for (auto effect : my_effects)
				SDL_HapticDestroyEffect(haptic_, effect);
			SDL_HapticClose(haptic_);
		}
	}

	Haptic(Haptic const&) = delete;
	Haptic& operator=(Haptic const&) = delete;

	Haptic(Haptic&& other)
	{
		move_from(other);
	}

	Haptic& operator=(Haptic&& other)
	{
		move_from(other);
		return *this;
	}

	bool valid() const
	{
		return haptic_ != nullptr;
	}

	unsigned int get_capabilities() const
	{
		if (!valid())  //we're not an opened device... So we should decide what to do here...
			return 0U;

		const auto capabilities = SDL_HapticQuery(haptic_);

		if(!capabilities)
		{
			//couldn't get the capabilities of this haptic device.
			throw Exception("SDL_HapticQuery");
		}

		return capabilities;
	}

	bool is_capable_of(int haptic_flag) const
	{
		const auto caps = get_capabilities();
		return (haptic_flag & caps) != 0;
	}

	effect_handle new_effect(Effect const& e)
	{
		//We need to be able to play the haptic effect
		if (!is_capable_of(e.type)) 
			return invalid_effect;

		//maybe we should have another RAII wrapper around the registered haptic effects
		const effect_sdlid sdl_effect_handle = SDL_HapticNewEffect(haptic_, e);

		if (sdl_effect_handle < 0)
			throw Exception("SDL_HapticNewEffect");

		my_effects.push_back(sdl_effect_handle);
		return effect_handle(my_effects.size()-1);
	}

	effect_list::size_type registered_effect_count() const
	{
		return my_effects.size();
	}

	effect_sdlid get_effect_sdlid(effect_handle h) const
	{
		return my_effects.at(h);
	}

	void run_effect(effect_handle h, uint32_t iterations = 1) const
	{
		if(SDL_HapticRunEffect(haptic_, get_effect_sdlid(h), iterations) < 0)
		{
			throw Exception("SDL_HapticRunEffect");
		}
	}

	bool is_effect_compatible(Effect const& e) const
	{
		return is_capable_of(e.type);
	}

};
} // namespace sdl
