#pragma once

#include "exception.hpp"
#include "surface.hpp"
#include "window.hpp"
#include <SDL.h>

namespace sdl
{
///Namespace containing everything that has to do with mouse management
namespace Mouse
{
///Set the mose in relative mode
inline void set_relative(bool enabled)
{
	if (SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE) < 0)
		throw Exception("SDL_SetRelativeMouseMode");
}

///Get if the mouse is in relative mode
inline bool get_relative()
{
	return SDL_GetRelativeMouseMode() == SDL_TRUE;
}

///Wrap inside the given windows
inline void warp_in_window(Window const& window, Vec2i const& position)
{
	SDL_WarpMouseInWindow(window.ptr(), position.x, position.y);
}

///Wrap at this global and absolute position
inline void warp_global(Vec2i const& position)
{
	SDL_WarpMouseGlobal(position.x, position.y);
}

///Cursor object
class Cursor
{
	///SDL pointer
	SDL_Cursor* cursor_ = nullptr;

public:
	///Construct a system cursor
	Cursor(SDL_SystemCursor id) : cursor_(SDL_CreateSystemCursor(id))
	{
		if (!cursor_)
		{
			throw Exception("SDL_CreateSystemCursor");
		}
	}

	///Construct a cursor from MSB bitmap data
	Cursor(
		const uint8_t* data,
		const uint8_t* mask,
		Vec2i const&   size,
		Vec2i const&   hot)
		: cursor_(SDL_CreateCursor(data, mask, size.x, size.y, hot.x, hot.y))
	{
		if (!cursor_)
		{
			throw Exception("SDL_CreateCursor");
		}
	}

	///Create cursor from a SDL_Surface
	Cursor(Surface const& surface, Vec2i const& hot)
		: cursor_(SDL_CreateColorCursor(surface.ptr(), hot.x, hot.y))
	{
		if (!cursor_)
		{
			throw Exception("SDL_CreateColorCursor");
		}
	}

	///Free the cursor
	~Cursor()
	{
		if (cursor_) SDL_FreeCursor(cursor_);
	}

	///Set this cursor as the current cursor
	void set() const
	{
		if (cursor_) SDL_SetCursor(cursor_);
	}

	///Set the cursor as visible
	static void show()
	{
		const auto value = SDL_ShowCursor(SDL_ENABLE);

		if (value != SDL_ENABLE)
		{
			throw Exception("SDL_ShowCursor");
		}
	}

	///Set the cursor as invisible
	static void hide()
	{
		const auto value = SDL_ShowCursor(SDL_DISABLE);

		if (value != SDL_DISABLE)
		{
			throw Exception("SDL_ShowCursor");
		}
	}

	///Return true or false if the cursor is curently visible or not
	static bool visible()
	{
		const auto value = SDL_ShowCursor(SDL_QUERY);

		if (value < 0)
		{
			throw Exception("SDL_ShowCursor");
		}

		return value == SDL_ENABLE;
	}
};
} // namespace Mouse
} // namespace sdl
