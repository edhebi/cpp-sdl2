#pragma once

#include <SDL.h>
#include <string>

#include "window.hpp"

namespace sdl
{
///Get version as string
inline std::string version()
{
	// Need to use SDL_GetVersion, not the SDL_VERSION macro.
	// SDL_VERSION give you the version of SDL used to build the program
	// SDL_GetVersion get the version number from the dynamically linked library
	SDL_version v;
	SDL_GetVersion(&v);
	return std::to_string(int(v.major)) + '.' + std::to_string(int(v.minor)) + '.'
		   + std::to_string(int(v.patch));
}

///Show a message box, usefull to display error messages
inline void show_message_box(uint32_t flags, std::string const& title, std::string const& message)
{
	SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), nullptr);
}

///Show a message box, as a modal child of `parent`
inline void show_message_box(
	uint32_t flags, std::string const& title, std::string const& message, sdl::Window const& parent)
{
	SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), parent.ptr());
}

///Get information about the system (os, cpu, ram...)
namespace system
{
// new in 2.0.9
#if SDL_VERSION_ATLEAST(2, 0, 9)
inline bool is_tablet()
{
	return SDL_IsTablet();
}

inline bool has_AVX512F()
{
	return SDL_HasAVX512F();
}
#endif

///Get used platform as a string
inline std::string platform()
{
	return SDL_GetPlatform();
}

///Get the size of a cacheline
/// \return size of "L1" cache, in bytes
inline int cpu_cacheline_size()
{
	return SDL_GetCPUCacheLineSize();
}

///Get the numbers of CPU in the system
inline int cpu_count()
{
	return SDL_GetCPUCount();
}

///Get the amount of ram in the system
///`\return amount of RAM in MB
inline int system_ram()
{
	return SDL_GetSystemRAM();
}

///Returns true if system has AMD 3DNow! support
inline bool has_3DNow()
{
	return SDL_Has3DNow();
}

///Return true if CPU has AVX instruction set support
inline bool has_AVX()
{
	return SDL_HasAVX();
}

///Return true if CPU has AVX2 instruction set support
inline bool has_AVX2()
{
	return SDL_HasAVX2();
}

///Return true if cpu has Apple/IBM/Motorola AltiVec SIMD support
inline bool has_AltiVec()
{
	return SDL_HasAltiVec();
}

///Return true if cpu has Intel MMX support
inline bool has_MMX()
{
	return SDL_HasMMX();
}

///Return true if current cpu has a TSC register
inline bool has_RDTSC()
{
	return SDL_HasRDTSC();
}

///Return true if cpu supports 1st gen SSE
inline bool has_SSE()
{
	return SDL_HasSSE();
}

///Return true if cpu supports SSE2
inline bool has_SSE2()
{
	return has_SSE2();
}

///Return true if cpu supports SSE3
inline bool has_SSE3()
{
	return has_SSE3();
}

///Return true if cpu supports SSE41
inline bool has_SSE41()
{
	return has_SSE41();
}

///Return true if cpu supports SSE42
inline bool has_SSE42()
{
	return has_SSE42();
}
} // namespace system

///Power related functions
namespace power
{
///Power states
enum class state
{
	unknown	   = SDL_POWERSTATE_UNKNOWN,
	on_battery = SDL_POWERSTATE_ON_BATTERY,
	no_battery = SDL_POWERSTATE_NO_BATTERY,
	charging   = SDL_POWERSTATE_CHARGING,
	charged	   = SDL_POWERSTATE_CHARGED
};

///Get current powerstate. See sdl::power::state enumeration for values
inline state get_state()
{
	return state(SDL_GetPowerInfo(nullptr, nullptr));
}

///Get battery remaining time
/// \return Estimated number of seconds untils the battery dies
inline int get_battery_remaining_time()
{
	int time = 0;
	SDL_GetPowerInfo(&time, nullptr);
	return time;
}

///Get the current battery charge
/// \return estimated remaining charge in the battery in percents
inline int get_battery_remaining_charge()
{
	int charge = 0;
	SDL_GetPowerInfo(nullptr, &charge);
	return charge;
}
} // namespace power

///Clipboard management functions
namespace clipboard
{
///Returns true if clipboard has text
inline bool has_text()
{
	return SDL_HasClipboardText();
}

///Returns the content of the clipboard
inline std::string text()
{
	return SDL_GetClipboardText();
}

///Set the clipboard to a specific value
inline int get_text(std::string const& text)
{
	return SDL_SetClipboardText(text.c_str());
}
} // namespace clipboard
} // namespace sdl
