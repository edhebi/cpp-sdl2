#pragma once

#include <SDL_timer.h>
#include "exception.hpp"
#include <cstdint>
#include <chrono>

namespace sdl
{
	///Represent an SDL timer
	class Timer
	{
		SDL_TimerID timer_ = 0;

		///construct 
		Timer(SDL_TimerID timer) : timer_{timer}
		{
		}

	public:

		SDL_TimerID timer_id() const
		{
			return timer_;
		}

		///A timer object that is not tied to a timer id in SDL doesn't make senes
		Timer() = delete;

		///Just so that the type of a callback function can be 
		using Callback = SDL_TimerCallback;

		///Remove the timer, return true if timer was removed
		bool remove()
		{
			if (timer_ > 0)
				return SDL_RemoveTimer(timer_);
			else
				return false;
		}

		///Factory function
		static Timer create(uint32_t interval, Callback function, void* user_context)
		{
			const auto id = SDL_AddTimer(interval, function, user_context);

			if(!id)
			{
				throw Exception("SDL_AddTimer");
			}

			return { id };
		}

		///Factory function using std::chrono
		static Timer create(std::chrono::milliseconds interval, Callback function, void* user_context)
		{
			return create(interval.count(), function, user_context);
		}

		///Wait for `millisec` milliseconds
		static void delay(std::chrono::milliseconds millisec)
		{
			delay(millisec.count());
		}

		///Wait for `millisec` milliseconds
		static void delay(uint32_t millisec)
		{
			SDL_Delay(millisec);
		}

		///Returns the number of milliseconds elapsed as a unint32_t (standard SDL API)
		static uint32_t ticks_u32()
		{
			return static_cast<uint32_t>(ticks().count());
		}

		///Retruns the number of milliseconds
		static std::chrono::milliseconds ticks()
		{
			return std::chrono::milliseconds(SDL_GetTicks());
		}

		///Return the performance counter value
		static uint64_t perf_counter()
		{
			return SDL_GetPerformanceCounter();
		}

		///Return the performace frequency value
		static uint64_t perf_frequency()
		{
			return SDL_GetPerformanceFrequency();
		}
	};
}
