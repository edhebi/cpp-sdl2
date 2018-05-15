#pragma once

#include <ostream>

#include <SDL2/SDL_pixels.h>
#include "exception.hpp"

namespace sdl
{
class Color : public SDL_Color
{
public:
	constexpr Color(): SDL_Color{ 0, 0, 0, 0 } {}
	constexpr Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255): SDL_Color{ r, g, b, a } {}

	constexpr Color(Color const&) = default;
	constexpr Color(Color&&) = default;

	Color(Uint32 raw, SDL_PixelFormat const& format) { SDL_GetRGBA(raw, &format, &r, &g, &b, &a); }

	Color(Uint32 raw, Uint32 format)
	{
		auto f = SDL_AllocFormat(format);
		if (!f) throw Exception{ "SDL_AllocFormat" };
		SDL_GetRGBA(raw, f, &r, &g, &b, &a);
		SDL_FreeFormat(f);
	}

	Color& operator=(Color const&) = default;
	Color& operator=(Color&&) = default;

	Uint32 as_uint(SDL_PixelFormat const& format) const
	{
		if (SDL_ISPIXELFORMAT_ALPHA(format.format))
		{
			return SDL_MapRGBA(&format, r, g, b, a);
		}
		else
		{
			return SDL_MapRGB(&format, r, g, b);
		}
	}

	Uint32 as_uint(Uint32 format) const
	{
		auto f = SDL_AllocFormat(format);
		if (!f) throw Exception{ "SDL_AllocFormat" };
		auto raw = as_uint(*f);
		SDL_FreeFormat(f);
		return raw;
	}

	bool operator==(Color const& c) const
	{
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}

	friend std::ostream& operator<<(std::ostream& stream, Color const& c)
	{
		return stream
			<< "(r:" << c.r
			<< ",g:" << c.g
			<< ",b:" << c.b
			<< ",a:" << c.a << ")";
	}

	static constexpr Color White      () { return { 255, 255, 255, 255 }; }
	static constexpr Color Black      () { return {   0,   0,   0, 255 }; }
	static constexpr Color Red        () { return { 255,   0,   0, 255 }; }
	static constexpr Color Green      () { return {   0, 255,   0, 255 }; }
	static constexpr Color Blue       () { return {   0,   0, 255, 255 }; }
	static constexpr Color Transparent() { return {   0,   0,   0,   0 }; }
};


} // namespace sdl