#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "renderer.hpp"
#include "vec2.hpp"

namespace sdl
{

class Window
{
public:
	Window(std::string const& title, Vec2i const& size, Uint32 flags = SDL_WINDOW_SHOWN)
		: window_{ SDL_CreateWindow(title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			size.x, size.y, flags) }
	{
		if (!window_) throw Exception{ "SDL_CreateWindow" };
	}
	
	Window(Window&&) = default;
	Window& operator=(Window&& other)
	{
		SDL_DestroyWindow(window_);
		window_ = std::move(other.window_);
		return *this;
	}

	Window(Window&) = delete;
	Window& operator=(Window&) = delete;

	virtual ~Window()
	{
		SDL_DestroyWindow(window_);
	}

	SDL_Window* ptr() { return window_; }

	Renderer make_renderer(Uint32 flags = SDL_RENDERER_ACCELERATED)
	{
		auto render = SDL_CreateRenderer(window_, -1, flags);
		if (!render) throw Exception{ "SDL_CreateRenderer" };
		return Renderer{ render };
	}

	int display_index()
	{
		auto r = SDL_GetWindowDisplayIndex(window_);
		if (r == -1) throw Exception{ "SDL_GetWindowDisplayIndex" };
		return r;
	}

	void set_display_mode(SDL_DisplayMode const& mode)
	{
		if (SDL_SetWindowDisplayMode(window_, &mode) != 0)
		{
			throw Exception("SDL_SetWindowDisplayMode");
		}
	}

	SDL_DisplayMode& display_mode(SDL_DisplayMode& mode)
	{
		if (SDL_GetWindowDisplayMode(window_, &mode) != 0)
		{
			throw Exception("SDL_GetWindowDisplayMode");
		}
		return mode;
	}
	SDL_DisplayMode display_mode() { SDL_DisplayMode m; display_mode(m); return m; }

	Uint32 flags() { return SDL_GetWindowFlags(window_); }

	Window& grab(bool g = true) { SDL_SetWindowGrab(window_, static_cast<SDL_bool>(g)); return *this; }
	Window& release(bool r = true) { return grab(!r); }
	bool grabbed() { return SDL_GetWindowGrab(window_); }

	Window& move_to(Vec2i const& v) { SDL_SetWindowPosition(window_, v.x, v.y); return *this; }
	Window& move_by(Vec2i const& v) { return move_to(position() + v); }
	Vec2i position() { Vec2i pos; SDL_GetWindowPosition(window_, &pos.x, &pos.y); return pos; }

	Vec2i resize(Vec2i const& newsize) { SDL_SetWindowSize(window_, newsize.x, newsize.y); }
	Vec2i size() { Vec2i s; SDL_GetWindowSize(window_, &s.x, &s.y); return s; }

	Window& rename(std::string const& t) { SDL_SetWindowTitle(window_, t.c_str()); return *this; }
	std::string title() { return std::string{ SDL_GetWindowTitle(window_) }; }

	void set_icon(Surface const& icon) { SDL_SetWindowIcon(window_, icon.ptr()); }
	void set_icon(std::string const& filename)
	{
		auto icon = Surface{ filename };
		SDL_SetWindowIcon(window_, icon.ptr());
	}


	 Window& hide()     { SDL_HideWindow(window_);     return *this;}
	 Window& maximize() { SDL_MaximizeWindow(window_); return *this;}
	 Window& minimize() { SDL_MinimizeWindow(window_); return *this;}
	 Window& raise()    { SDL_RaiseWindow(window_);    return *this;}
	 Window& restore()  { SDL_RestoreWindow(window_);  return *this;}


	bool fullscreen() { return flags() & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP); }
	Window& set_fullscreen(bool fs)
	{
		if (SDL_SetWindowFullscreen(window_, fs ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) != 0)
		{
			throw Exception{ "SDL_SetWindowFullscreen" };
		}
		return *this;
	}

	Window& toogle_fullscreen() { return set_fullscreen(!fullscreen()); }

	SDL_SysWMinfo getWMInfo()
	{
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		if(SDL_GetWindowWMInfo(window_, &info) == SDL_FALSE)
		{
			throw Exception("SDL_GetWindowWMInfo");
		}
		return info;
	}

private:

	SDL_Window * window_;
};

} // namespace sdl
