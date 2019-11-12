#include "cpp-sdl2/sdl.hpp"

void useless_function()
{
	sdl::show_message_box(
		SDL_MESSAGEBOX_INFORMATION,
		"Testing...",
		"The only existance of this file is to test if we can have 2 "
		"compilation unit including the whole library and not explode. This "
		"has been an issue before...");
}
